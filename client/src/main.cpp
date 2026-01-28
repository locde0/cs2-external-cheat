#include <cstdlib>
#include <exception>
#include <string>
#include "core/win.h"
#include "app/app.h"
#include "core/config/config.h"
#include "core/path/path.h"

int WINAPI main(HINSTANCE, HINSTANCE, PWSTR, int) {
	try {
		std::wstring config_path = core::path::makeFullPath(L"settings.ini");
		core::config::Settings::get().runAutoUpdate(config_path);

		app::App a;
		return a.run();
	}
	catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "error", MB_ICONERROR);
		return EXIT_FAILURE;
	}
}
