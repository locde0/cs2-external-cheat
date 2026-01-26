#pragma once
#include "../../game/context.h"
#include "../../../render/draw.h"
#include "../../../core/types.h"

namespace features {

	class Esp {
	public:
		void run(const game::Context& ctx, render::DrawList& draw, const core::Extent& ss);

	private:
		bool _enabled = true;
		bool _box = true;
		bool _health = true;
	};

}