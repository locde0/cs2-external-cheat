#include "facade.h"
#include "../core/win.h"
#include "../render/draw.h"

namespace domain {

	Facade::Facade(driver::Driver& d) 
		: _ctx(d), _esp()
	{}

	void Facade::update() {
		_ctx.update();
	}

	void Facade::build(render::DrawList& out, const core::Extent& size) {
		_esp.run(_ctx, out, size);
	}

}