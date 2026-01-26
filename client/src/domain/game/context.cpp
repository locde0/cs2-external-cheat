#include "context.h"
#include "../math/math.h"

namespace game {

	Context::Context(driver::Driver& d)
		: _driver(d), _off(offsets::Offsets::get())
	{}

	void Context::update() {
		_local.spect_ptr = 0;
		_local.ptr = _driver.read<uintptr_t>(_driver.b_addr() + _off.client().dwLocalPlayerPawn);

		if (!_local.ptr) return;

		uint32_t stride = _off.client().dwEntityIdentitySize;
		uintptr_t obs_services = _driver.read<uintptr_t>(_local.ptr + _off.schemas().m_pObserverServices);

		if (obs_services) {
			uint32_t obs_handle = _driver.read<uint32_t>(obs_services + _off.schemas().m_hObserverTarget);
			if (obs_handle) {
				uintptr_t e_list = _driver.read<uintptr_t>(_driver.b_addr() + _off.client().dwEntityList);

				uintptr_t list_entry = _driver.read<uintptr_t>(e_list + 0x8 * ((obs_handle & 0x7FFF) >> 9) + 16);
				if (list_entry)
					_local.spect_ptr = _driver.read<uintptr_t>(list_entry + stride * (obs_handle & 0x1FF));
			}
		}

		_local.team = _driver.read<int8_t>(_local.ptr + _off.schemas().m_iTeamNum);
		_local.vm = _driver.read<math::ViewMatrix>(_driver.b_addr() + _off.client().dwViewMatrix);

		uintptr_t e_list = _driver.read<uintptr_t>(_driver.b_addr() + _off.client().dwEntityList);

		_entities.clear();
		_entities.reserve(64);

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

			int health = _driver.read<int>(entity + _off.schemas().m_iHealth);
			if (health <= 0 || health > 100) continue;

			uintptr_t gsn = _driver.read<uintptr_t>(entity + _off.schemas().m_pGameSceneNode);
			uintptr_t collision = _driver.read<uintptr_t>(entity + _off.schemas().m_pCollision);
			if (!gsn || !collision) continue;

			Entity ent;
			ent.ptr = entity;
			ent.health = health;
			ent.team = _driver.read<int8_t>(entity + _off.schemas().m_iTeamNum);
			ent.feet = _driver.read<math::Vec3>(gsn + _off.schemas().m_vecAbsOrigin);
			ent.mins = _driver.read<math::Vec3>(collision + _off.schemas().m_vecMins);
			ent.maxs = _driver.read<math::Vec3>(collision + _off.schemas().m_vecMaxs);

			_entities.push_back(ent);
		}
	}

}