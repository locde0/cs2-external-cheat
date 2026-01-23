#pragma once
#include "Windows.h"
#include <cstdint>

namespace platform {

	struct Size {
		int w, h;
	};

	class Overlay {
	public:
		bool create(const wchar_t*, int, int);
		void pumpMsgs();
		bool running() const { return _running; }

		HWND handle() const { return _hwnd; }
		Size size() const { return _size; }

		bool resize(Size&);

	private:
		static LRESULT CALLBACK wndProcSetup(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK wndProcThunk(HWND, UINT, WPARAM, LPARAM);
		LRESULT wndProc(HWND, UINT, WPARAM, LPARAM);

		void updateSize();

		HWND _hwnd = nullptr;
		bool _running = true;
		Size _size{};
		bool _resized = false;
	};

}