#pragma once
#include "../core/win.h"
#include <cstdint>
#include "../core/types.h"
#include "window.h"
#include "target.h"

namespace platform {

	class Overlay {
	public:
		bool create(const wchar_t*, const core::Extent&);
		void pumpMsgs();

		bool resize(core::Extent&);
		bool attach(const Target&);
		void hide();

		HWND handle() const { return _hwnd; }
		core::Extent size() const { return _size; }
		bool running() const { return _running; }

	private:
		static LRESULT CALLBACK wndProcSetup(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK wndProcThunk(HWND, UINT, WPARAM, LPARAM);
		LRESULT wndProc(HWND, UINT, WPARAM, LPARAM);

		void updateSize();

		HWND _hwnd = nullptr;
		bool _running = true;
		core::Extent _size{};
		bool _resized = false;
	};

}