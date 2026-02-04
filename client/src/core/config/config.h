#pragma once
#include <string>
#include <chrono>
#include <filesystem>
#include <thread>
#include "../types.h"

namespace core::config {

    struct SystemConfig {
        std::wstring process = L"cs2.exe";
        std::wstring module = L"client.dll";
    };

    struct UpdaterConfig {
        bool enabled = true;
        std::wstring client_url = L"https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json";
        std::wstring offsets_url = L"https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json";
    };

    struct OverlayConfig {
        std::wstring title = L"overlay";
        int delay = 20;
    };

    struct EntityEspConfig {
        bool enabled = true;
        bool box = true;
        bool health = true;
        //bool name = true;
        //bool skeleton = false;

        core::Color color = core::Color::red();
    };

    struct EspConfig {
        bool enabled = true;
        int box_thickness = 1;
        float bar_width = 4;

        EntityEspConfig enemies{ true, true, true, core::Color::red() };
        EntityEspConfig teammates{ false, false, false, core::Color::green() };
    };

    struct AppConfig {
        SystemConfig system;
        UpdaterConfig updater;
        OverlayConfig overlay;
        EspConfig esp;
    };

    class Settings {
    public:
        static const AppConfig& read() {
            return get()._cfg;
        }

        static Settings& get() {
            static Settings instance;
            return instance;
        }

        Settings(const Settings&) = delete;
        void operator=(const Settings&) = delete;

        void runAutoUpdate(const std::wstring&);
        void stopAutoUpdate();

    private:
        Settings() = default;
        ~Settings() { stopAutoUpdate(); }

        void load();
        void saveDefault();

        AppConfig _cfg{};
        std::wstring _path;

        std::atomic<bool> _running = false;
        std::thread _watcher_thread;
        std::filesystem::file_time_type _lw_time;
    };

}
