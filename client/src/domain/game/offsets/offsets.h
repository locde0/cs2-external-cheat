#pragma once
#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace game::offsets {

    bool update();

    class Offsets {
    public:
        struct ClientData {
            uintptr_t dwEntityList = 0;
            uintptr_t dwViewMatrix = 0;
            uintptr_t dwLocalPlayerPawn = 0;

            uint32_t dwEntityIdentitySize = 0;
        };

        struct SchemaData {
            // C_BaseEntity
            uintptr_t m_iHealth = 0; // int32
            uintptr_t m_iTeamNum = 0; // uint8
            uintptr_t m_pGameSceneNode = 0; // CGameSceneNode*
			uintptr_t m_pCollision = 0; // CCollisionProperty*

            // CBasePlayerController
            uintptr_t m_iszPlayerName = 0; // char[128]
            uintptr_t m_hPlayerPawn = 0; // CHandle<C_BasePlayerPawn>

            // CGameSceneNode
            uintptr_t m_vecAbsOrigin = 0; // Vector

            // CCollisionProperty
			uintptr_t m_vecMins = 0; // Vector
			uintptr_t m_vecMaxs = 0; // Vector

            // C_BasePlayerPawn
            uintptr_t m_pObserverServices = 0; // CPlayer_ObserverServices*

			// CPlayer_ObserverServices
            uintptr_t m_hObserverTarget = 0; // CHandle<C_BaseEntity>
        };

        static Offsets& get() {
            static Offsets instance;
            return instance;
        }

        Offsets(const Offsets&) = delete;
        Offsets& operator=(const Offsets&) = delete;

        const ClientData& client() const { return _client; }
        const SchemaData& schemas() const { return _schemas; }

        bool load();

    private:
        Offsets() = default;
        ~Offsets() = default;

        ClientData _client;
		SchemaData _schemas;
    };
}