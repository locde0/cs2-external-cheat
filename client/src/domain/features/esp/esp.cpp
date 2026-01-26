#include "esp.h"

namespace features {

	void Esp::run(const game::Context& ctx, render::DrawList& draw, const core::Extent& ss) {
        if (!_enabled) return;

        const auto& local = ctx.local();
        const auto& vm = local.vm;

        for (const auto& ent : ctx.entities()) {
            if (!ent.is_valid || ent.health <= 0) continue;
            if (ent.team == local.team) continue;

            math::Vec3 screen_pos, screen_head;

            if (!math::w2s(ent.pos, screen_pos, vm, ss)) continue;
            if (!math::w2s(ent.head, screen_head, vm, ss)) continue;

            float height = screen_pos.y - screen_head.y;
            float width = height / 2.0f;
            float x = screen_pos.x - (width / 2.0f);
            float y = screen_head.y;

            if (_box) {
                draw.add(
                    { x, y, width, height },
                    core::Color::red()
                );
            }
        }
	}

}