#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <vector>

#pragma comment(lib, "dwmapi.lib")

namespace platform {

    struct WindowInfo {
        RECT bounds{};
        bool minimized = false;
        bool topmost = false;
    };

    HWND findMainWindow(DWORD);
    bool getWindowBounds(HWND, RECT&);
    bool queryWindowInfo(HWND, WindowInfo&);

    HWND calcInsertAfter(HWND, HWND, bool);

}