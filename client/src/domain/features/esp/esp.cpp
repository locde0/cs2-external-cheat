#include "esp.h"
#include "../../math/math.h"

namespace features {

    Esp::Esp() 
        : _cfg(core::config::Settings::read().esp)
    {}

	void Esp::run(const game::Context& ctx, render::DrawList& draw, const core::Extent& ss) {
        if (!_cfg.enabled) return;
        if (ctx.entities().empty()) return;

        const auto& local = ctx.local();

        for (const auto& ent : ctx.entities()) {
            if (ent.ptr == local.spect_ptr) continue;

            bool is_mate = ent.team == local.team;
            if (is_mate && !_cfg.teammates.enabled) continue;

            core::Color color = is_mate ? _cfg.teammates.color : _cfg.enemies.color;

            float w_height = ent.maxs.z - ent.mins.z;

            math::Vec3 feet = ent.feet;
            feet.z += ent.mins.z;
            math::Vec3 head = ent.feet;
            head.z += w_height;

            math::Vec3 s_feet, s_head;
            if (!math::w2s(feet, s_feet, local.vm, ss)) continue;
            if (!math::w2s(head, s_head, local.vm, ss)) continue;

            float h = s_feet.y - s_head.y;
            float w = h / 2.4f;
            float x = s_head.x - (w / 2.0f);
            float y = s_head.y;

            draw.add(
                { x, y, w, h },
                color
            );
        }
	}

}