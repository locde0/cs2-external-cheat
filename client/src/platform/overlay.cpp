#include "overlay.h"

namespace platform {

    bool Overlay::create(const wchar_t* title, const core::Extent& extent) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = wndProcSetup;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = title;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;

        RegisterClassExW(&wc);

        DWORD exStyle = WS_EX_LAYERED |
            WS_EX_TRANSPARENT |
            WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE |
            WS_EX_NOREDIRECTIONBITMAP;

        _hwnd = CreateWindowExW(
            exStyle, wc.lpszClassName, title,
            WS_POPUP,
            0, 0, extent.w, extent.h,
            nullptr, nullptr, wc.hInstance, this
        );

        if (!_hwnd) return false;

        ShowWindow(_hwnd, SW_HIDE);

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

    bool Overlay::attach(const Target& target) {
        HWND t_hwnd = static_cast<HWND>(target.handle());

        if (!t_hwnd || !IsWindow(t_hwnd)) return false;

        if (_l_state.t_hwnd != t_hwnd) {
            _l_state = {};
            _l_state.t_hwnd = t_hwnd;
            ShowWindow(_hwnd, SW_SHOWNA);
        }

        WindowInfo wi;
        if (!queryWindowInfo(t_hwnd, wi))
            return false;

        if (wi.minimized) {
            if (!_l_state.minimized) {
                hide();
                _l_state.minimized = true;
            }
            return true;
        }

        if (_l_state.minimized) {
            ShowWindow(_hwnd, SW_SHOWNA);
            _l_state.minimized = false;
            _l_state.bounds.x = -9999;
        }

        if (wi.bounds.left == _l_state.bounds.x && wi.bounds.top == _l_state.bounds.y && wi.size == _l_state.size)
            return true;

        HWND insertAfter = platform::calcInsertAfter(t_hwnd, _hwnd, wi.topmost);

        SetWindowPos(
            _hwnd,
            insertAfter,
            wi.bounds.left, wi.bounds.top, wi.size.w, wi.size.h,
            SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_ASYNCWINDOWPOS
        );
            
        _l_state.bounds.x = static_cast<float>(wi.bounds.left);
        _l_state.bounds.y = static_cast<float>(wi.bounds.top);
        _l_state.size = wi.size;
        return true;
    }

    void Overlay::hide() {
        ShowWindow(_hwnd, SW_HIDE);
    }

    bool Overlay::resize(core::Extent& out) {
		if (!_resized) return false;
		_resized = false;
        out = _size;
        return true;
    }

    void Overlay::updateSize() {
        RECT rect{};
        GetClientRect(_hwnd, &rect);
        _size.w = rect.right - rect.left;
        _size.h = rect.bottom - rect.top;
	}

    LRESULT CALLBACK Overlay::wndProcSetup(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lp);
            auto self = reinterpret_cast<Overlay*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndProcThunk));
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
        case WM_SIZE:
            updateSize();
            _resized = true;
            return 0;

        case WM_PAINT:
            ValidateRect(hwnd, nullptr);
            return 0;

        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            _running = false;
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

}