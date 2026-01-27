#include "offsets.h"
#include "../../../core/win.h"
#include "../../../core/config/config.h"
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

namespace {

    uint32_t getEntityIdentitySize(const json& cei) {
        auto& fields = cei["fields"];
        uint32_t max = 0;

        for (auto& [name, offset] : fields.items())
            if (offset > max)
                max = offset;

        return max + 8;
    }

}

namespace game::offsets {

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
            _client.dwLocalPlayerController = j1["client.dll"]["dwLocalPlayerController"];

            std::ifstream f2("client_dll.json");
            if (!f2) return false;
            json j2 = json::parse(f2);
            json& classes = j2["client.dll"]["classes"];

            _client.dwEntityIdentitySize = getEntityIdentitySize(classes["CEntityIdentity"]);

            _schemas.m_iHealth = classes["C_BaseEntity"]["fields"]["m_iHealth"];
            _schemas.m_iTeamNum = classes["C_BaseEntity"]["fields"]["m_iTeamNum"];
            _schemas.m_pGameSceneNode = classes["C_BaseEntity"]["fields"]["m_pGameSceneNode"];
            _schemas.m_pCollision = classes["C_BaseEntity"]["fields"]["m_pCollision"];
            _schemas.m_lifeState = classes["C_BaseEntity"]["fields"]["m_lifeState"];

            _schemas.m_iszPlayerName = classes["CBasePlayerController"]["fields"]["m_iszPlayerName"];
            _schemas.m_hPlayerPawn = classes["CCSPlayerController"]["fields"]["m_hPlayerPawn"];

            _schemas.m_vecAbsOrigin = classes["CGameSceneNode"]["fields"]["m_vecAbsOrigin"];
            _schemas.m_bDormant = classes["CGameSceneNode"]["fields"]["m_bDormant"];

            _schemas.m_vecMins = classes["CCollisionProperty"]["fields"]["m_vecMins"];
            _schemas.m_vecMaxs = classes["CCollisionProperty"]["fields"]["m_vecMaxs"];

            _schemas.m_pObserverServices = classes["C_BasePlayerPawn"]["fields"]["m_pObserverServices"];

            _schemas.m_hObserverTarget = classes["CPlayer_ObserverServices"]["fields"]["m_hObserverTarget"];

            return true;
        }
        catch (...) {
            return false;
        }
    }

}