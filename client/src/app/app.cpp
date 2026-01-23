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

}

namespace app {
    App::App() 
        : _driver(L"kmd", makeFullPath(L"kmd.sys")) 
    {}

    bool App::initDriver() {
        try {
            return _driver.init();
        }
        catch (...) { 
            return false; 
        }
    }

    bool App::ensureConnection() {
        if (_is_attached && _t_hwnd) {
            if (!IsWindow(_t_hwnd)) {
                resetConnection();
                return false;
            }
            return true;
        }

        static ULONGLONG lastCheckTime = 0;
        ULONGLONG now = GetTickCount64();
        if (now - lastCheckTime < 1000)
            return false;
        lastCheckTime = now;

        if (_driver.attach(L"notepad.exe", L"client.dll")) {
            if (resolve()) {
                _is_attached = true;
                ShowWindow(_overlay.handle(), SW_SHOWNA);
                return true;
            }
        }
        return false;
    }

    void App::resetConnection() {
        _is_attached = false;
        _t_hwnd = nullptr;
        
        if (_overlay.handle())
            ShowWindow(_overlay.handle(), SW_HIDE);
    }

    bool App::resolve() {
        _t_hwnd = platform::findMainWindow(_driver.pid());
        return _t_hwnd != nullptr;
    }

    bool App::updateWindowSync(bool wf) {
        if (!_t_hwnd || !IsWindow(_t_hwnd)) return false;

        platform::WindowInfo wi;
		if (!platform::queryWindowInfo(_t_hwnd, wi)) 
            return false;

        if (wi.minimized) {
            ShowWindow(_overlay.handle(), SW_HIDE);
            return true;
        }
        
        ShowWindow(_overlay.handle(), SW_SHOWNA);

        HWND insertAfter = platform::calcInsertAfter(_t_hwnd, _overlay.handle(), wi.topmost);
        int w = wi.bounds.right - wi.bounds.left;
        int h = wi.bounds.bottom - wi.bounds.top;

        SetWindowPos(
            _overlay.handle(),
            insertAfter,
            wi.bounds.left, wi.bounds.top, w, h,
            SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_ASYNCWINDOWPOS
        );

        return true;
    }

    int App::run() {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        if (!initDriver())
			throw std::runtime_error("failed to init driver");

        if (!_overlay.create(L"overlay", 1, 1))
            throw std::runtime_error("failed to create overlay window");

        ShowWindow(_overlay.handle(), SW_HIDE);

        auto cs = _overlay.size();
        _renderer.init(_overlay.handle(), cs.w, cs.h);

        while (_overlay.running()) {
            _overlay.pumpMsgs();

            platform::Size newSize{};
            if (_overlay.resize(newSize)) {
                _renderer.resize(newSize.w, newSize.h);
            }

            if (!ensureConnection()) {
                Sleep(50);
                continue;
            }

            if (!updateWindowSync(true)) {
                resetConnection();
                continue;
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