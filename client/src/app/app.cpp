#include "app.h"
#include "../platform/window.h"
#include "../core/path/path.h"

namespace app {
    App::App() 
        : _driver(L"kmd", core::path::makeFullPath(L"kmd.sys"))
    {}

    bool App::ensureConnection() {
        if (_is_attached && _target.isValid())
            return true;

        if (_is_attached) {
            resetConnection();
            return false;
        }

        static auto l_time = std::chrono::steady_clock::time_point{};
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - l_time).count() < 1000)
            return false;

        l_time = now;

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
        _target.reset();
        _overlay.hide();
    }

    bool App::resolve() {
        return _target.find(_driver.pid());
    }

    int App::run() {
        platform::enableDpiAwareness();

        if (!_driver.init())
			throw std::runtime_error("failed to init driver");

        if (!_overlay.create(L"overlay", { 1, 1 }))
            throw std::runtime_error("failed to create overlay window");

        _renderer.init(_overlay.handle(), _overlay.size());

        while (_overlay.running()) {
            _overlay.pumpMsgs();

            core::Extent size;
            if (_overlay.resize(size))
                _renderer.resize(size);

            if (!ensureConnection()) {
                Sleep(50);
                continue;
            }

            if (!_overlay.attach(_target)) {
                resetConnection();
                continue;
            }

            _facade.update();

            _draw.clear();
            _facade.build(_draw, _overlay.size());

            _renderer.begin();
            _renderer.draw(_draw);
            _renderer.end();
        }

        return 0;
    }
}