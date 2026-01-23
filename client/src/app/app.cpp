#include "app.h"

namespace {
    std::wstring getDir() {
        wchar_t buf[MAX_PATH];
        DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
        if (n == 0 || n == MAX_PATH) return L"";
        return std::filesystem::path(buf).parent_path().wstring();
    }

    std::wstring makeFullPath(const std::wstring& filename) {
        std::filesystem::path path = std::filesystem::path(getDir()) / filename;
        return path.lexically_normal().wstring();
    }

    struct EnumCtx {
        DWORD pid = 0;
        HWND best = nullptr;
    };

    bool isGoodMainWindow(HWND hwnd) {
        if (!IsWindow(hwnd)) return false;
        if (!IsWindowVisible(hwnd)) return false;

        LONG ex = GetWindowLongW(hwnd, GWL_EXSTYLE);
        if (ex & WS_EX_TOOLWINDOW) return false;

        if (GetWindow(hwnd, GW_OWNER) != nullptr) return false;

        int len = GetWindowTextLengthW(hwnd);
        if (len <= 0) return false;

        BOOL cloaked = FALSE;
        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
            if (cloaked) return false;
        }
        return true;
    }

    BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lp) {
        auto* ctx = reinterpret_cast<EnumCtx*>(lp);
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != ctx->pid) return TRUE;

        if (isGoodMainWindow(hwnd)) {
            ctx->best = hwnd;
            return FALSE;
        }
        return TRUE;
    }

    bool getWindowBounds(HWND hwnd, RECT& out) {
        if (!IsWindow(hwnd)) return false;

        HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &out, sizeof(out));
        if (SUCCEEDED(hr)) return true;

        return GetWindowRect(hwnd, &out) != 0;
    }

    HWND calcInsertAfterAboveTarget(HWND target) {
        HWND prev = GetWindow(target, GW_HWNDPREV);
        return prev ? prev : HWND_TOP;
    }
}

namespace app {
    App::App() 
        : _driver(L"kmd", makeFullPath(L"kmd.sys")) 
    {}

    bool App::initDriver() {
        try {
            if (!_driver.init()) return false;
            return true;
        }
        catch (...) { 
            return false; 
        }
    }

    bool App::ensureConnection() {
        if (_is_attached && _t_hwnd && IsWindow(_t_hwnd))
            return true;

        resetConnection();

        if (_driver.attach(L"notepad.exe", L"client.dll")) {
            if (resolve()) {
                _is_attached = true;
                return true;
            }
        }
        return false;
    }

    void App::resetConnection() {
        _is_attached = false;
        _t_hwnd = nullptr;
        ShowWindow(_overlay.handle(), SW_SHOWNA);
    }

    bool App::resolve() {
        _t_hwnd = nullptr;

        EnumCtx ctx{};
        ctx.pid = _driver.pid();

        EnumWindows(&enumWindowsProc, (LPARAM)&ctx);

        _t_hwnd = ctx.best;
        return _t_hwnd != nullptr;
    }

    bool App::updateWindowSync(bool wf) {
        if (!_t_hwnd || !IsWindow(_t_hwnd)) return false;

        RECT r{};
        if (!getWindowBounds(_t_hwnd, r)) return false;

        const bool minimized = IsIconic(_t_hwnd) != 0;
        const bool inFocus = (GetForegroundWindow() == _t_hwnd);

        if (minimized) {
            ShowWindow(_overlay.handle(), SW_HIDE);
            return true;
        }
        else {
            ShowWindow(_overlay.handle(), SW_SHOWNA);
        }

        HWND windowAboveTarget = GetWindow(_t_hwnd, GW_HWNDPREV);

        HWND insertAfter = windowAboveTarget;

        if (!windowAboveTarget || windowAboveTarget == _overlay.handle()) {
            insertAfter = HWND_TOP;
        }

        LONG targetExStyle = GetWindowLongW(_t_hwnd, GWL_EXSTYLE);
        if (targetExStyle & WS_EX_TOPMOST) {
            if (!windowAboveTarget) insertAfter = HWND_TOPMOST;
        }

        int w = r.right - r.left;
        int h = r.bottom - r.top;

        SetWindowPos(
            _overlay.handle(),
            insertAfter,
            r.left, r.top, w, h,
            SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_ASYNCWINDOWPOS
        );

        return true;


        /*if (!_t_hwnd || !IsWindow(_t_hwnd)) return false;

        RECT r{};
        if (!getWindowBounds(_t_hwnd, r)) return false;

        int w = r.right - r.left;
        int h = r.bottom - r.top;

        UINT flags = SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_ASYNCWINDOWPOS;

        SetWindowPos(
            _overlay.handle(),
            HWND_TOPMOST,
            r.left, r.top, w, h,
            flags
        );

        return true;*/
    }

    int App::run() {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        if (!initDriver())
			throw std::runtime_error("failed to init driver");

        while (!ensureConnection())
            Sleep(500);

        RECT r{};
        if (!getWindowBounds(_t_hwnd, r))
			throw std::runtime_error("failed to get target window bounds");

        const int w = r.right - r.left;
        const int h = r.bottom - r.top;

        if (!_overlay.create(L"overlay", (w > 0 ? w : 900), (h > 0 ? h : 600)))
            throw std::runtime_error("failed to create overlay window");

		updateWindowSync(true);

        auto cs = _overlay.size();
        _renderer.init(_overlay.handle(), cs.w, cs.h);

        while (_overlay.running()) {
            updateWindowSync(true);
            _overlay.pumpMsgs();

            platform::Size newSize{};
            if (_overlay.resize(newSize)) {
                _renderer.resize(newSize.w, newSize.h);
            }

            _facade.update();

            auto cur = _overlay.size();
            _draw.clear();
            _facade.build(_draw, cur.w, cur.h);

            _renderer.begin();
            _renderer.draw(_draw, cur.w, cur.h);
            _renderer.end();
        }

        return 0;
    }
}