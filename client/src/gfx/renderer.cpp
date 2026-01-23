#include "renderer.h"

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

    bool Renderer::init(HWND hwnd, int w, int h) {
        _width = w;
        _height = h;

        UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL flOut{};
        const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };

        throwIfFailed(
            D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                deviceFlags, levels, 1, D3D11_SDK_VERSION,
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
        desc.Width = (UINT)w;
        desc.Height = (UINT)h;
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

    void Renderer::resize(int w, int h) {
        if (!_swap) return;
        if (w <= 0 || h <= 0) return;

        _width = w; 
        _height = h;
        _rtv.Reset();

        throwIfFailed(_swap->ResizeBuffers(0, (UINT)w, (UINT)h, DXGI_FORMAT_UNKNOWN, 0), "ResizeBuffers");
        createRTV();

        float clear[4] = { 0.f, 0.f, 0.f, 0.f };
        _ctx->OMSetRenderTargets(1, _rtv.GetAddressOf(), nullptr);
        _ctx->ClearRenderTargetView(_rtv.Get(), clear);
        _swap->Present(1, 0);
        if (_dcompDevice) _dcompDevice->Commit();
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
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

    void Renderer::begin() {
        float clear[4] = { 0.f, 0.f, 0.f, 0.f };
        _ctx->OMSetRenderTargets(1, _rtv.GetAddressOf(), nullptr);
        _ctx->ClearRenderTargetView(_rtv.Get(), clear);

        D3D11_VIEWPORT vp{};
        vp.Width = (float)_width;
        vp.Height = (float)_height;
        vp.MinDepth = 0.f;
        vp.MaxDepth = 1.f;
        _ctx->RSSetViewports(1, &vp);

        _ctx->IASetInputLayout(_il.Get());
        _ctx->VSSetShader(_vs.Get(), nullptr, 0);
        _ctx->PSSetShader(_ps.Get(), nullptr, 0);

        float blendFactor[4] = { 0,0,0,0 };
        _ctx->OMSetBlendState(_blend.Get(), blendFactor, 0xFFFFFFFF);
    }

    static inline float PxToNdcX(float x, float w) { return (2.f * x / w) - 1.f; }
    static inline float PxToNdcY(float y, float h) { return 1.f - (2.f * y / h); }

    void Renderer::draw(const render::DrawList& list, int w, int h) {
        const auto& rects = list.rects();
        const UINT needed = (UINT)rects.size() * 6;
        if (needed == 0) return;

        if (!_vb || needed > _vbCap) {
            _vbCap = (needed < 256) ? 256 : needed;

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
            float x0 = cmd.rect.x;
            float y0 = cmd.rect.y;
            float x1 = cmd.rect.x + cmd.rect.w;
            float y1 = cmd.rect.y + cmd.rect.h;

            float l = PxToNdcX(x0, (float)w);
            float r = PxToNdcX(x1, (float)w);
            float t = PxToNdcY(y0, (float)h);
            float b = PxToNdcY(y1, (float)h);

            const auto c = cmd.color;

            *out++ = { l, t, c.r, c.g, c.b, c.a };
            *out++ = { r, t, c.r, c.g, c.b, c.a };
            *out++ = { r, b, c.r, c.g, c.b, c.a };

            *out++ = { l, t, c.r, c.g, c.b, c.a };
            *out++ = { r, b, c.r, c.g, c.b, c.a };
            *out++ = { l, b, c.r, c.g, c.b, c.a };
        }

        _ctx->Unmap(_vb.Get(), 0);

        UINT stride = sizeof(Vtx);
        UINT offset = 0;
        _ctx->IASetVertexBuffers(0, 1, _vb.GetAddressOf(), &stride, &offset);
        _ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        _ctx->Draw(needed, 0);
    }

    void Renderer::end() {
        _swap->Present(1, 0);
        if (_dcompDevice) _dcompDevice->Commit();
    }

}