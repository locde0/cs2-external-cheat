#include "facade.h"
#include "../core/win.h"
#include "../render/draw.h"

namespace domain {

	void Facade::update() {
		//_ctx.update(_driver);
	}

	void Facade::build(render::DrawList& out, const core::Extent& size) {
		_esp.run(_ctx, out, size);


		const float t = float(GetTickCount64() % 2000) / 2000.0f;

		core::Vec2 pos0{ 50.f + t * (size.w - 200.f), 60.f };
		core::Vec2 size0{ 150.f, 150.f };

		out.add(core::Rect{ pos0.x, pos0.y, size0.x, size0.y }, core::Color::red());
		out.add({ 60.0f, 240.0f, 220.0f, 90.0f }, core::Color::green());
	}

}