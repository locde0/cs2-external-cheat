#include "app.h"
#include <thread>
#include "../platform/window.h"
#include "../core/path/path.h"
#include "../domain/game/offsets/offsets.h"
#include "../core/config/config.h"

#pragma comment(lib, "winmm.lib")

namespace app {
    App::App()
        : _cfg(core::config::Settings::read()), _facade(_driver)
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

        if (_driver.attach(_cfg.system.process.c_str(), _cfg.system.module.c_str())) {
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

        if (!game::offsets::update())
			throw std::runtime_error("failed to update offsets");

        if (!game::offsets::Offsets::get().load())
			throw std::runtime_error("failed to load offsets");

        if (!_overlay.create(_cfg.overlay.title.c_str(), {1, 1}))
            throw std::runtime_error("failed to create overlay window");

        _renderer.init(_overlay.handle(), _overlay.size());

        timeBeginPeriod(1);

        while (_overlay.running()) {
            _overlay.pumpMsgs();

            core::Extent size;
            if (_overlay.resize(size))
                _renderer.resize(size);

            if (!ensureConnection()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            if (!_overlay.attach(_target)) {
                resetConnection();
                continue;
            }

            _draw.clear();
            _facade.update();
            _facade.build(_draw, _overlay.size());

            _renderer.begin();
            _renderer.draw(_draw);
            _renderer.end();

            std::this_thread::sleep_for(std::chrono::milliseconds(_cfg.overlay.delay));
        }

        timeEndPeriod(1);
        core::config::Settings::get().stopAutoUpdate();
        return 0;
    }
}