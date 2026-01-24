#pragma once
#include <chrono>
#include "../gfx/renderer.h"
#include "../platform/overlay.h"
#include "../platform/target.h"
#include "../render/draw.h"
#include "../domain/facade.h"
#include "../driver/driver.h"

namespace app {

	class App {
	public:
		App();

		int run();

	private:
		bool ensureConnection();
		void resetConnection();

		bool resolve();

		bool _is_attached = false;

		platform::Target _target;
		platform::Overlay _overlay;
		gfx::Renderer _renderer;
		driver::Driver _driver;
		domain::Facade _facade;
		render::DrawList _draw;
	};

}
