#pragma once
#include "Windows.h"
#include "../render/draw.h"

namespace domain {

	struct State {

	};

	class Facade {
	public:
		void update();
		void build(render::DrawList&, int, int);
	};

}