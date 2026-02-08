#include "renderer.h"
#include <string>
#include <stdexcept>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include <dcomp.h>

#include "../render/draw.h"
#include "../core/hr.h"

#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace gfx {

    Microsoft::WRL::ComPtr<ID3DBlob> compile(const char* src, const char* entry, const char* target) {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        Microsoft::WRL::ComPtr<ID3DBlob> blob, err;
        HRESULT hr = D3DCompile(src, strlen(src), nullptr, nullptr, nullptr, entry, target, flags, 0, &blob, &err);
        if (FAILED(hr)) {
            std::string msg = err ? std::string((char*)err->GetBufferPointer(), err->GetBufferSize()) : "D3DCompile failed";
            throw std::runtime_error(msg);
        }
        return blob;
    }

    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    bool Renderer::init(HWND hwnd, const core::Extent& extent) {
        _size = extent;

        UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        D3D_FEATURE_LEVEL flOut{};
        const D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };

        throwIfFailed(
            D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                deviceFlags, levels, 3, D3D11_SDK_VERSION,
                &_dev, &flOut, &_ctx
            ),
            "D3D11CreateDevice"
        );

        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
        throwIfFailed(_dev.As(&dxgiDevice), "dev As IDXGIDevice");

        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        throwIfFailed(dxgiDevice->GetAdapter(&adapter), "GetAdapter");

        Microsoft::WRL::ComPtr<IDXGIFactory2> factory2;
        throwIfFailed(
            adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&factory2),
            "GetParent factory2"
        );

        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.Width = (UINT)_size.w;
        desc.Height = (UINT)_size.h;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BufferCount = 2;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        _swap.Reset();
        throwIfFailed(
            factory2->CreateSwapChainForComposition(_dev.Get(), &desc, nullptr, &_swap),
            "CreateSwapChainForComposition"
        );

        factory2->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

        _dcompDevice.Reset();
        _dcompTarget.Reset();
        _dcompVisual.Reset();

        throwIfFailed(
            DCompositionCreateDevice(dxgiDevice.Get(), __uuidof(IDCompositionDevice), (void**)&_dcompDevice),
            "DCompositionCreateDevice"
        );

        throwIfFailed(
            _dcompDevice->CreateTargetForHwnd(hwnd, TRUE, &_dcompTarget),
            "CreateTargetForHwnd"
        );

        throwIfFailed(_dcompDevice->CreateVisual(&_dcompVisual), "CreateVisual");
        throwIfFailed(_dcompVisual->SetContent(_swap.Get()), "Visual SetContent");
        throwIfFailed(_dcompTarget->SetRoot(_dcompVisual.Get()), "Target SetRoot");
        throwIfFailed(_dcompDevice->Commit(), "DComp Commit");

        createRTV();
        createPipeline();
        return true;
    }

    void Renderer::resize(const core::Extent& extent) {
        if (!_swap) return;
        if (extent.w <= 0 || extent.h <= 0) return;

        _size = extent;
        _rtv.Reset();

        throwIfFailed(_swap->ResizeBuffers(0, (UINT)_size.w, (UINT)_size.h, DXGI_FORMAT_UNKNOWN, 0), "ResizeBuffers");
        createRTV();

        bindAndClear();
        end();
    }

    void Renderer::createRTV() {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> bb;
        throwIfFailed(_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&bb), "GetBuffer");
        throwIfFailed(_dev->CreateRenderTargetView(bb.Get(), nullptr, &_rtv), "CreateRTV");
    }

    void Renderer::createPipeline() {
        const char* vsSrc = R"(
            struct VSIn { float2 pos : POSITION; float4 col : COLOR0; };
            struct VSOut { float4 pos : SV_Position; float4 col : COLOR0; };
            VSOut main(VSIn v) { VSOut o; o.pos = float4(v.pos, 0, 1); o.col = v.col; return o; }
        )";

        const char* psSrc = R"(
            struct PSIn { float4 pos : SV_Position; float4 col : COLOR0; };
            float4 main(PSIn p) : SV_Target {
                return float4(p.col.rgb * p.col.a, p.col.a); // premultiply
            }
        )";

        auto vsBlob = compile(vsSrc, "main", "vs_5_0");
        auto psBlob = compile(psSrc, "main", "ps_5_0");

        throwIfFailed(_dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vs), "CreateVS");
        throwIfFailed(_dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_ps), "CreatePS");

        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        throwIfFailed(_dev->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &_il), "CreateInputLayout");

        D3D11_BLEND_DESC bd{};
        bd.RenderTarget[0].BlendEnable = TRUE;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        throwIfFailed(_dev->CreateBlendState(&bd, &_blend), "CreateBlendState");
    }

    void Renderer::bindAndClear() {
        core::Color clear = core::Color::transparent();
        _ctx->OMSetRenderTargets(1, _rtv.GetAddressOf(), nullptr);
        _ctx->ClearRenderTargetView(_rtv.Get(), reinterpret_cast<const float*>(&clear));
    }

    void Renderer::begin() {
        bindAndClear();

        D3D11_VIEWPORT vp{};
        vp.Width = (float)_size.w;
        vp.Height = (float)_size.h;
        vp.MinDepth = 0.f;
        vp.MaxDepth = 1.f;
        _ctx->RSSetViewports(1, &vp);

        _ctx->IASetInputLayout(_il.Get());
        _ctx->VSSetShader(_vs.Get(), nullptr, 0);
        _ctx->PSSetShader(_ps.Get(), nullptr, 0);

        float blendFactor[4] = { 0,0,0,0 };
        _ctx->OMSetBlendState(_blend.Get(), blendFactor, 0xFFFFFFFF);
    }

    inline core::Vec2 pxToNdc(const core::Vec2& p, const core::Extent& size) {
        return {
            (2.f * p.x / (float)size.w) - 1.f,
            1.f - (2.f * p.y / (float)size.h)
        };
    }

    void Renderer::draw(const render::DrawList& list) {
        const auto& rects = list.rects();
        const auto& lines = list.lines();

        const UINT verts_rects = (UINT)rects.size() * 6;
        const UINT verts_lines = (UINT)lines.size() * 2;
        const UINT needed = verts_rects + verts_lines;
        if (needed == 0) return;

        if (!_vb || needed > _vbCap) {
            _vbCap = (needed < 512) ? 512 : needed + 128;

            D3D11_BUFFER_DESC bd{};
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.ByteWidth = _vbCap * sizeof(Vtx);

            _vb.Reset();
            throwIfFailed(_dev->CreateBuffer(&bd, nullptr, &_vb), "create VB");
        }

        D3D11_MAPPED_SUBRESOURCE ms{};
        throwIfFailed(_ctx->Map(_vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms), "map VB");
        auto* out = (Vtx*)ms.pData;

        for (const auto& cmd : rects) {
            core::Vec2 v_lt = { cmd.rect.x, cmd.rect.y };
            core::Vec2 v_rb = { cmd.rect.x + cmd.rect.w, cmd.rect.y + cmd.rect.h };

            core::Vec2 ndc_lt = pxToNdc(v_lt, _size);
            core::Vec2 ndc_rb = pxToNdc(v_rb, _size);

            *out++ = { ndc_lt, cmd.color };
            *out++ = { { ndc_rb.x, ndc_lt.y }, cmd.color };
            *out++ = { ndc_rb, cmd.color };

            *out++ = { ndc_lt, cmd.color };
            *out++ = { ndc_rb, cmd.color };
            *out++ = { { ndc_lt.x, ndc_rb.y }, cmd.color };
        }

        for (const auto& cmd : lines) {
            *out++ = { pxToNdc(cmd.a, _size), cmd.color };
            *out++ = { pxToNdc(cmd.b, _size), cmd.color };
        }

        _ctx->Unmap(_vb.Get(), 0);

        UINT stride = sizeof(Vtx);
        UINT offset = 0;
        _ctx->IASetVertexBuffers(0, 1, _vb.GetAddressOf(), &stride, &offset);

        if (verts_rects > 0) {
            _ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            _ctx->Draw(verts_rects, 0);
        }

        if (verts_lines > 0) {
            _ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            _ctx->Draw(verts_lines, verts_rects);
        }
    }

    void Renderer::end() {
        _swap->Present(0, 0);
        if (_dcompDevice) _dcompDevice->Commit();
    }

}