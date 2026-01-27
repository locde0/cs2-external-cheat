#include "context.h"
#include "../math/math.h"

namespace game {

	Context::Context(driver::Driver& d)
		: _driver(d), _off(offsets::Offsets::get()), _cfg(core::config::Settings::read().esp)
	{
        _entities.reserve(64);
		_buf.resize(64 * 256);
        _pawn_buf.resize(512 * 128);
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

        uintptr_t chunk0_ptr = _driver.read<uintptr_t>(e_list + 16);
        uintptr_t chunk1_ptr = _driver.read<uintptr_t>(e_list + 16 + 8);
        if (!chunk0_ptr || !chunk1_ptr) return;

        size_t size_ctrl = 64 * stride;
        size_t size_pawn = 512 * stride;

        if (_buf.size() < size_ctrl) _buf.resize(size_ctrl);
        if (!_driver.readBuf(chunk0_ptr, _buf.data(), size_ctrl)) return;
        
        if (_pawn_buf.size() < size_pawn) _pawn_buf.resize(size_pawn);
        if (!_driver.readBuf(chunk1_ptr, _pawn_buf.data(), size_pawn)) return;

        for (int i = 0; i < 64; i++) {
            uint8_t* entry_ptr = _buf.data() + (i * stride);
            uintptr_t e_ctrl = *reinterpret_cast<uintptr_t*>(entry_ptr);
            if (!e_ctrl) continue;

            uint32_t e_handle = _driver.read<uint32_t>(e_ctrl + _off.schemas().m_hPlayerPawn);
            if (!e_handle) continue;

            uintptr_t entity = 0;
            int chunk_id = (e_handle & 0x7fff) >> 9;
            int chunk_inx = e_handle & 0x1ff;

            if (chunk_id == 1) {
                uint8_t* pawn_entry_ptr = _pawn_buf.data() + (chunk_inx * stride);
                entity = *reinterpret_cast<uintptr_t*>(pawn_entry_ptr);
            }
            else {
                uintptr_t entry_pawn_list = _driver.read<uintptr_t>(e_list + 0x8 * chunk_id + 16);
                if (entry_pawn_list)
                    entity = _driver.read<uintptr_t>(entry_pawn_list + stride * chunk_inx);
            }

            if (!entity || entity == _local.ptr) continue;

            uint8_t life_state = _driver.read<uint8_t>(entity + _off.schemas().m_lifeState);
            if (life_state != 0) continue;

            int health = _driver.read<int>(entity + _off.schemas().m_iHealth);
            if (health <= 0 || health > 100) continue;

            int team = _driver.read<int>(entity + _off.schemas().m_iTeamNum);
            if (team == _local.team && !_cfg.teammates.enabled) continue;
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