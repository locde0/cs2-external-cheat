#pragma once
#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace domain::game::offsets {

    bool update();

    class Offsets {
    public:
        struct ClientData {
            uintptr_t dwEntityList = 0;
            uintptr_t dwViewMatrix = 0;
            uintptr_t dwLocalPlayerPawn = 0;
        };

        struct SchemaData {
            uintptr_t m_iHealth = 0;
            uintptr_t m_iTeamNum = 0;
            uintptr_t m_vOldOrigin = 0;
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