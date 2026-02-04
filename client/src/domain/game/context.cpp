#include "context.h"
#include <iostream>
#include "../math/math.h"

namespace game {

	Context::Context(driver::Driver& d)
		: _driver(d), _off(offsets::Offsets::get()), 
        _esp_cfg(core::config::Settings::read().esp)
	{
        _entities.reserve(64);
	}

	void Context::update() {
        _entities.clear();
        _local = {};

        uintptr_t e_list = _driver.read<uintptr_t>(_driver.b_addr() + _off.client().dwEntityList);
        if (!e_list) return;

        _local.ptr = _driver.read<uintptr_t>(_driver.b_addr() + _off.client().dwLocalPlayerPawn);
        if (!_local.ptr) return;

        _local.spect_ptr = 0;
        _local.team = _driver.read<uint8_t>(_local.ptr + _off.schemas().m_iTeamNum);

        uint32_t stride = _off.client().dwEntityIdentitySize;
        uintptr_t obs_services = _driver.read<uintptr_t>(_local.ptr + _off.schemas().m_pObserverServices);

        if (obs_services) {
            uint32_t obs_handle = _driver.read<uint32_t>(obs_services + _off.schemas().m_hObserverTarget);
            if (obs_handle) {
                uintptr_t l_entry = _driver.read<uintptr_t>(e_list + 0x8 * ((obs_handle & 0x7FFF) >> 9) + 16);
                if (l_entry)
                    _local.spect_ptr = _driver.read<uintptr_t>(l_entry + stride * (obs_handle & 0x1FF));
            }
        }

        for (int i = 0; i < 64; i++) {
            uintptr_t le = _driver.read<uintptr_t>(e_list + ((8 * (i & 0x7ff) >> 9) + 16));
            if (!le) continue;

            uintptr_t e_ctrl = _driver.read<uintptr_t>(le + stride * (i & 0x1ff));
            if (!e_ctrl) continue;

            uint32_t e_handle = _driver.read<uint32_t>(e_ctrl + _off.schemas().m_hPlayerPawn);
            if (!e_handle) continue;

            uintptr_t entry_pawn = _driver.read<uintptr_t>(e_list + 0x8 * ((e_handle & 0x7FFF) >> 9) + 16);
            if (!entry_pawn) continue;

            uintptr_t entity = _driver.read<uintptr_t>(entry_pawn + stride * (e_handle & 0x1FF));
            if (!entity || entity == _local.ptr) continue;

            uint8_t life_state = _driver.read<uint8_t>(entity + _off.schemas().m_lifeState);
            if (life_state != 0) continue;

            int health = _driver.read<int>(entity + _off.schemas().m_iHealth);
            if (health <= 0 || health > 100) continue;

            int team = _driver.read<int>(entity + _off.schemas().m_iTeamNum);
            if (team == _local.team && !_esp_cfg.teammates.enabled) continue;
			if (team < 2 || team > 3) continue;

            uintptr_t gsn = _driver.read<uintptr_t>(entity + _off.schemas().m_pGameSceneNode);
            if (!gsn) continue;

            bool is_dormant = _driver.read<bool>(gsn + _off.schemas().m_bDormant);
            if (is_dormant) continue;

            math::Vec3 origin = _driver.read<math::Vec3>(gsn + _off.schemas().m_vecAbsOrigin);
            if (origin.x == 0.f && origin.y == 0.f && origin.z == 0.f) continue;

            uintptr_t collision = _driver.read<uintptr_t>(entity + _off.schemas().m_pCollision);
            if (!collision) continue;

            Entity ent;
            ent.ptr = entity;
            ent.health = health;
            ent.team = team;
            ent.feet = origin;
            ent.mins = _driver.read<math::Vec3>(collision + _off.schemas().m_vecMins);
            ent.maxs = _driver.read<math::Vec3>(collision + _off.schemas().m_vecMaxs);

            _entities.push_back(ent);
        }

        _local.vm = _driver.read<math::ViewMatrix>(_driver.b_addr() + _off.client().dwViewMatrix);
    }

}