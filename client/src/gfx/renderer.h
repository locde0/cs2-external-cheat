#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include <dcomp.h>
#include <dwmapi.h>
#include "../render/draw.h"
#include "../core/hr.h"

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace gfx {
	class Renderer {
	public:
		bool init(HWND, int, int);
		void resize(int, int);

		void begin();
		void end();
		void draw(const render::DrawList&, int, int);

	private:
		struct Vtx {
			float x, y;
			float r, g, b, a;
		};

		void createRTV();
		void createPipeline();

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

		int _width = 0;
		int _height = 0;
	};

};