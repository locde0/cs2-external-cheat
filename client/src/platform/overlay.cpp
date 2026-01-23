#include "overlay.h"

namespace platform {

    bool Overlay::create(const wchar_t* title, int w, int h) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = &Overlay::wndProcSetup;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"overlay";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);

        RegisterClassExW(&wc);

        DWORD style = WS_POPUP;
        DWORD exStyle = WS_EX_LAYERED |
            WS_EX_TRANSPARENT |
            WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE |
            WS_EX_NOREDIRECTIONBITMAP;

        _hwnd = CreateWindowExW(
            exStyle, wc.lpszClassName, title,
            style,
            0, 0,
            w, h,
            nullptr, nullptr, wc.hInstance, this
        );

        if (!_hwnd) return false;

        SetLayeredWindowAttributes(_hwnd, 0, 255, LWA_ALPHA);
        SetWindowPos(
            _hwnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW
        );
        ShowWindow(_hwnd, SW_SHOWNA);

        updateSize();
        return true;
    }

    void Overlay::pumpMsgs() {
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
		}
    }

    bool Overlay::resize(Size& out) {
		if (!_resized) return false;
		_resized = false;
        out = _size;
        return true;
    }

    void Overlay::updateSize() {
        RECT rect{};
        GetClientRect(_hwnd, &rect);
        _size.w = rect.right - rect.left;
        _size.h= rect.bottom - rect.top;
	}

    LRESULT CALLBACK Overlay::wndProcSetup(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lp);
            auto self = reinterpret_cast<Overlay*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)self);
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)&Overlay::wndProcThunk);
            self->_hwnd = hwnd;
            return self->wndProc(hwnd, msg, wp, lp);
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    LRESULT CALLBACK Overlay::wndProcThunk(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        auto self = reinterpret_cast<Overlay*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        return self ? self->wndProc(hwnd, msg, wp, lp) : DefWindowProcW(hwnd, msg, wp, lp);
    }

    LRESULT Overlay::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
        case WM_PAINT:
            ValidateRect(hwnd, nullptr);
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_ERASEBKGND:
            return 1;

        case WM_SIZE:
            updateSize();
            _resized = true;
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            _running = false;
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
}