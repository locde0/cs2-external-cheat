#pragma once
#include <iostream>
#include <filesystem>
#include "../gfx/renderer.h"
#include "../platform/overlay.h"
#include "../render/draw.h"
#include "../domain/facade.h"
#include "../driver/driver.h"

namespace app {
	class App {
	public:
		App();

		int run();

	private:
		bool initDriver();
		bool ensureConnection();
		void resetConnection();

		bool resolve();
		bool updateWindowSync(bool);

		HWND _t_hwnd = nullptr;
		bool _is_attached = false;

		platform::Overlay _overlay;
		gfx::Renderer _renderer;
		driver::Driver _driver;
		domain::Facade _facade;
		render::DrawList _draw;
	};
}
