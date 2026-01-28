#include "esp.h"
#include "../../math/math.h"

namespace features {

    Esp::Esp() 
        : _cfg(core::config::Settings::read().esp)
    {}

	void Esp::run(const game::Context& ctx, render::DrawList& draw, const core::Extent& ss) {
        if (!_cfg.enabled) return;
        if (!_cfg.teammates.enabled && !_cfg.enemies.enabled) return;
        if (ctx.entities().empty()) return;

        const auto& local = ctx.local();

        for (const auto& ent : ctx.entities()) {
            if (ent.ptr == local.spect_ptr) continue;

            bool is_mate = ent.team == local.team;
            if (is_mate && !_cfg.teammates.enabled) continue;
            if (!is_mate && !_cfg.enemies.enabled) continue;

            const auto& cfg = is_mate ? _cfg.teammates : _cfg.enemies;
            BoundingBox box = calcBoundingBox(ent, local, ss);
            if (!box.valid) continue;

            if (cfg.box)
                drawBox(box, cfg.color, draw);

            if (cfg.health)
                drawHealthBar(box, ent.health, draw);
        }
	}

    Esp::BoundingBox Esp::calcBoundingBox(const game::Entity& ent, const game::LocalPlayer& local, const core::Extent& ss) {
        math::Vec3 feet = ent.feet;
        feet.z += ent.mins.z;
        math::Vec3 head = ent.feet;
        head.z += ent.maxs.z; // -ent.mins.z;

        math::Vec3 s_feet, s_head;
        if (!math::w2s(feet, s_feet, local.vm, ss) || !math::w2s(head, s_head, local.vm, ss)) 
            return { {0, 0}, {0, 0}, false };

        float h = s_feet.y - s_head.y;
        if (h < 1.0f) return { {0, 0}, {0, 0}, false };

        float w = h / 2.4f;
        float x = s_head.x - (w / 2.0f);
        float y = s_head.y;

        return { {x, y}, {w, h}, true };
    }

    void Esp::drawBox(const BoundingBox& box, const core::Color& color, render::DrawList& draw) {
        draw.addBox({ box.pos.x, box.pos.y, box.size.x, box.size.y, _cfg.box_thickness }, color);
    }
    
    void Esp::drawHealthBar(const BoundingBox& box, int health, render::DrawList& draw) {
        health = std::max(0, std::min(health, 100));

        float w = _cfg.bar_width;
        float pad = 2.0f;
        float offset_x = (_cfg.box_thickness > 1.0f) ? (_cfg.box_thickness + pad) : pad;

        float x = box.pos.x - pad - w;
        float y = box.pos.y;
        float h = box.size.y;

        draw.addRect(
            { x, y, w, h },
            { 0.0f, 0.0f, 0.0f, 0.6f }
        );

        float hp_perc = health / 100.0f;
        float fill_h = h * hp_perc;
        float fill_y = y + (h - fill_h);

        core::Color hp_color = {
            (health > 50) ? (1.0f - hp_perc) * 2.0f : 1.0f,
            (health > 50) ? 1.0f : hp_perc * 2.0f,
            0.0f,
            1.0f
        };

        draw.addRect(
            { x + 1, fill_y + 1, w - 2, fill_h - 2 },
            hp_color
        );
    }

}