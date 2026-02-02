#include "config.h"
#include "../win.h"
#include <iomanip>
#include <fstream>
#include <string>

namespace {

    std::wstring readStr(const std::wstring& file, const wchar_t* sect, const wchar_t* key, const wchar_t* def) {
        wchar_t buf[256];
        GetPrivateProfileStringW(sect, key, def, buf, 256, file.c_str());
        return buf;
    }

    bool readBool(const std::wstring& file, const wchar_t* sect, const wchar_t* key, bool def) {
        std::wstring s = readStr(file, sect, key, def ? L"true" : L"false");
        if (s == L"true" || s == L"1") return true;
        if (s == L"false" || s == L"0") return false;
        return def;
    }

    int readInt(const std::wstring& file, const wchar_t* sect, const wchar_t* key, int def) {
        return GetPrivateProfileIntW(sect, key, def, file.c_str());
    }

    float readFloat(const std::wstring& file, const wchar_t* sect, const wchar_t* key, float def) {
        std::wstring s = readStr(file, sect, key, L"");
        if (s.empty()) return def;
        try {
            return std::stof(s);
        }
        catch (...) {
            return def;
        }
    }

    core::Color readColor(const std::wstring& file, const wchar_t* sect, const wchar_t* key, const core::Color& def) {
        std::wstring s = readStr(file, sect, key, L"");
        if (s.empty()) return def;

        if (_wcsicmp(s.c_str(), L"red") == 0) return core::Color::red();
        if (_wcsicmp(s.c_str(), L"green") == 0) return core::Color::green();
        if (_wcsicmp(s.c_str(), L"blue") == 0) return core::Color::blue();
        if (_wcsicmp(s.c_str(), L"white") == 0) return core::Color::white();
        if (_wcsicmp(s.c_str(), L"black") == 0) return core::Color::black();
        if (_wcsicmp(s.c_str(), L"transparent") == 0) return core::Color::transparent();

        float r, g, b, a;
        if (swscanf_s(s.c_str(), L"%f,%f,%f,%f", &r, &g, &b, &a) == 4)
            return { r, g, b, a };

        return def;
    }

}

namespace core::config {

    void Settings::runAutoUpdate(const std::wstring& path) {
        _path = path;

        if (!std::filesystem::exists(_path)) saveDefault();
        load();

        std::error_code ec;
        _lw_time = std::filesystem::last_write_time(_path, ec);

        _running = true;
        _watcher_thread = std::thread(
            [this]() {
                while (_running) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    std::error_code ec;
                    auto w_time = std::filesystem::last_write_time(_path, ec);

                    if (!ec && w_time != _lw_time) {
                        load();
                        _lw_time = w_time;
                    }
                }
            }
        );
        _watcher_thread.detach();
    }

    void Settings::stopAutoUpdate() {
        _running = false;
    }

    void Settings::load() {
        _cfg.system.process = readStr(_path, L"system", L"process", _cfg.system.process.c_str());
        _cfg.system.module = readStr(_path, L"system", L"module", _cfg.system.module.c_str());

        _cfg.updater.enabled = readBool(_path, L"updater", L"enabled", _cfg.updater.enabled);
        _cfg.updater.client_url = readStr(_path, L"updater", L"client_url", _cfg.updater.client_url.c_str());
        _cfg.updater.offsets_url = readStr(_path, L"updater", L"offsets_url", _cfg.updater.offsets_url.c_str());

        _cfg.overlay.title = readStr(_path, L"overlay", L"title", _cfg.overlay.title.c_str());
        _cfg.overlay.delay = readInt(_path, L"overlay", L"delay", _cfg.overlay.delay);

        _cfg.esp.enabled = readBool(_path, L"esp", L"enabled", _cfg.esp.enabled);
        _cfg.esp.box_thickness = readInt(_path, L"esp", L"box_thickness", _cfg.esp.box_thickness);
        _cfg.esp.bar_width = readFloat(_path, L"esp", L"bar_width", _cfg.esp.bar_width);

        _cfg.esp.enemies.enabled = readBool(_path, L"esp_enemy", L"enabled", _cfg.esp.enemies.enabled);
        _cfg.esp.enemies.box = readBool(_path, L"esp_enemy", L"box", _cfg.esp.enemies.box);
        _cfg.esp.enemies.health = readBool(_path, L"esp_enemy", L"health", _cfg.esp.enemies.health);
        //_cfg.esp.enemies.name = readBool(_path, L"esp_enemy", L"name", _cfg.esp.enemies.name);
        _cfg.esp.enemies.color = readColor(_path, L"esp_enemy", L"color", _cfg.esp.enemies.color);

        _cfg.esp.teammates.enabled = readBool(_path, L"esp_team", L"enabled", _cfg.esp.teammates.enabled);
        _cfg.esp.teammates.box = readBool(_path, L"esp_team", L"box", _cfg.esp.teammates.box);
        _cfg.esp.teammates.health = readBool(_path, L"esp_team", L"health", _cfg.esp.teammates.health);
        //_cfg.esp.teammates.name = readBool(_path, L"esp_team", L"name", _cfg.esp.teammates.name);
        _cfg.esp.teammates.color = readColor(_path, L"esp_team", L"color", _cfg.esp.teammates.color);

        _cfg.memory.max_items = readInt(_path, L"memory", L"max_items", _cfg.memory.max_items);
        _cfg.memory.max_pawns = readInt(_path, L"memory", L"max_pawns", _cfg.memory.max_pawns);
        _cfg.memory.item_size = readInt(_path, L"memory", L"item_size", _cfg.memory.item_size);
        _cfg.memory.pawn_size = readInt(_path, L"memory", L"pawn_size", _cfg.memory.pawn_size);
    }

    void Settings::saveDefault() {
        std::wofstream file(_path);
        if (!file.is_open()) return;

        auto writeBool = [&](const wchar_t* key, bool val) {
            file << key << L"=" << (val ? L"true" : L"false") << L"\n";
        };

        auto writeColor = [&](const wchar_t* key, const core::Color& c) {
            file << key << L"=" << std::fixed << std::setprecision(2) 
                << c.r << L"," << c.g << L"," << c.b << L"," << c.a << L"\n";
        };

        file << L"[system]\n";
        file << L"process=" << _cfg.system.process << L"\n";
        file << L"module=" << _cfg.system.module << L"\n";
        file << L"\n";

        file << L"[updater]\n";
        writeBool(L"enabled", _cfg.updater.enabled);
        file << L"client_url=" << _cfg.updater.client_url << L"\n";
        file << L"offsets_url=" << _cfg.updater.offsets_url << L"\n";
        file << L"\n";

        file << L"[overlay]\n";
        file << L"title=" << _cfg.overlay.title << L"\n";
        file << L"delay=" << _cfg.overlay.delay << L"\n";
        file << L"\n";

        file << L"[esp]\n";
        writeBool(L"enabled", _cfg.esp.enabled);
        file << L"box_thickness=" << _cfg.esp.box_thickness << L"\n";
        file << L"bar_width=" << std::fixed << std::setprecision(1) << _cfg.esp.bar_width << L"\n";
        file << L"\n";

        file << L"[esp_enemy]\n";
        writeBool(L"enabled", _cfg.esp.enemies.enabled);
        writeBool(L"box", _cfg.esp.enemies.box);
        writeBool(L"health", _cfg.esp.enemies.health);
        //writeBool(L"name", _cfg.esp.enemies.name);
        writeColor(L"color", _cfg.esp.enemies.color);
        file << L"\n";

        file << L"[esp_team]\n";
        writeBool(L"enabled", _cfg.esp.teammates.enabled);
        writeBool(L"box", _cfg.esp.teammates.box);
        writeBool(L"health", _cfg.esp.teammates.health);
        //writeBool(L"name", _cfg.esp.teammates.name);
        writeColor(L"color", _cfg.esp.teammates.color);
        file << L"\n";

        file << L"[memory]\n";
        file << L"max_items=" << _cfg.memory.max_items << L"\n";
        file << L"max_pawns=" << _cfg.memory.max_pawns << L"\n";
        file << L"item_size=" << _cfg.memory.item_size << L"\n";
        file << L"pawn_size=" << _cfg.memory.pawn_size << L"\n";

        file.close();
    }

}