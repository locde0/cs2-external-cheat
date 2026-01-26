#include "offsets.h"
#include "../../../core/win.h"
#include "../../../core/config/config.h"
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

namespace domain::game::offsets {

    bool update() {
        const auto& cfg = core::config::Settings::read().updater;

        if (!cfg.enabled) return true;

        HRESULT hr1 = URLDownloadToFileW(NULL, cfg.client_url.c_str(), L"client_dll.json", 0, NULL);
        HRESULT hr2 = URLDownloadToFileW(NULL, cfg.offsets_url.c_str(), L"offsets.json", 0, NULL);

        return (hr1 == S_OK) && (hr2 == S_OK);
    }

    bool Offsets::load() {
        try {
            std::ifstream f1("offsets.json");
            if (!f1) return false;
            json j1 = json::parse(f1);

            _client.dwEntityList = j1["client.dll"]["dwEntityList"];
            _client.dwViewMatrix = j1["client.dll"]["dwViewMatrix"];
            _client.dwLocalPlayerPawn = j1["client.dll"]["dwLocalPlayerPawn"];

            std::ifstream f2("client_dll.json");
            if (!f2) return false;
            json j2 = json::parse(f2);

            _schemas.m_iHealth = j2["client.dll"]["classes"]["C_BaseEntity"]["fields"]["m_iHealth"];
            _schemas.m_iTeamNum = j2["client.dll"]["classes"]["C_BaseEntity"]["fields"]["m_iTeamNum"];
            _schemas.m_vOldOrigin = j2["client.dll"]["classes"]["C_BasePlayerPawn"]["fields"]["m_vOldOrigin"];

            return true;
        }
        catch (...) {
            return false;
        }
    }

}