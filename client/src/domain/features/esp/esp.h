#pragma once
#include "../../game/context.h"
#include "../../../render/draw.h"
#include "../../../core/types.h"
#include "../../../core/config/config.h"

namespace features {

	class Esp {
	public:
		explicit Esp();

		void run(const game::Context& ctx, render::DrawList& draw, const core::Extent& ss);

	private:
		const core::config::EspConfig& _cfg;
	};

}