#include "facade.h"

namespace domain {
	void Facade::update() {

	}

	void Facade::build(render::DrawList& out, int w, int h) {
		const float t = float(GetTickCount64() % 2000) / 2000.0f;
		const float x = 50.0f + t * (w - 200.0f);

		out.add({ x, 60.0f, 150.0f, 150.0f }, { 1.0f, 0.0f, 0.0f, 0.80f });
		out.add({ 60.0f, 240.0f, 220.0f, 90.0f }, { 0.0f, 1.0f, 0.0f, 0.60f });
	}
}