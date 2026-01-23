#include <iostream>
#include "app/app.h"

int WINAPI main(HINSTANCE, HINSTANCE, PWSTR, int) {
	try {
		app::App a;
		return a.run();
	}
	catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "error", MB_ICONERROR);
		return EXIT_FAILURE;
	}
}
