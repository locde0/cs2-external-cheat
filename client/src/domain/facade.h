#pragma once
#include "../core/types.h"

namespace render { 
	class DrawList; 
}

namespace domain {

	struct State {

	};

	class Facade {
	public:
		void update();
		void build(render::DrawList&, const core::Extent&);
	};

}