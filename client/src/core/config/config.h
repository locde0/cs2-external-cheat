#pragma once
#include <string>
#include <chrono>
#include <filesystem>
#include "../types.h"

namespace core::config {

    struct SystemConfig {
        std::wstring driver_name = L"kmd";
        std::wstring driver_file = L"kmd.sys";

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
        int delay = 100;
    };

    struct EntityEspConfig {
        bool enabled = true;
        bool box = true;
        bool health = true;
        bool name = true;
        //bool skeleton = false;

        core::Color color = core::Color::red();
    };

    struct EspConfig {
        bool enabled = true;

        EntityEspConfig enemies{ true, true, true, true, core::Color::red() };
        EntityEspConfig teammates{ false, false, false, false, core::Color::green() };
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

        void init(const std::wstring& path);
        void update();

    private:
        Settings() = default;
        ~Settings() = default;

        void load();
        void saveDefault();

        AppConfig _cfg{};
        std::wstring _path;
        std::filesystem::file_time_type _lw_time;
        std::chrono::steady_clock::time_point _lc_time;
    };

}
