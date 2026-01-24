#pragma once
#include <wrl/client.h>
#include <cstdint>
#include "../core/win.h"
#include "../core/types.h"

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include <dcomp.h>

namespace render { 
	class DrawList; 
}

//struct ID3D11Device;
//struct ID3D11DeviceContext;
//struct IDXGISwapChain1;
//struct ID3D11RenderTargetView;
//struct ID3D11VertexShader;
//struct ID3D11PixelShader;
//struct ID3D11InputLayout;
//struct ID3D11BlendState;
//struct ID3D11Buffer;
//
//struct IDCompositionDevice;
//struct IDCompositionTarget;
//struct IDCompositionVisual;

namespace gfx {

	class Renderer {
	public:
		Renderer();
		~Renderer();

		bool init(HWND, const core::Extent&);
		void resize(const core::Extent&);

		void begin();
		void end();
		void draw(const render::DrawList&);

	private:
		struct Vtx {
			core::Vec2 pos;
			core::Color color;
		};

		void createRTV();
		void createPipeline();
		void bindAndClear();

		Microsoft::WRL::ComPtr<ID3D11Device> _dev;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _ctx;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> _swap;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _rtv;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> _vs;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> _ps;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> _il;
		Microsoft::WRL::ComPtr<ID3D11BlendState> _blend;

		Microsoft::WRL::ComPtr<IDCompositionDevice> _dcompDevice;
		Microsoft::WRL::ComPtr<IDCompositionTarget> _dcompTarget;
		Microsoft::WRL::ComPtr<IDCompositionVisual> _dcompVisual;

		Microsoft::WRL::ComPtr<ID3D11Buffer> _vb;
		UINT _vbCap = 0;

		core::Extent _size{};
	};

};