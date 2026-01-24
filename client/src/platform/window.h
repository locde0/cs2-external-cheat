#pragma once
#include "../core/win.h"
#include "../core/types.h"

namespace platform {

    struct WindowInfo {
        RECT bounds{};
		core::Extent size{};
        bool minimized = false;
        bool topmost = false;
    };

    HWND findMainWindow(DWORD);
    bool getWindowBounds(HWND, RECT&);
    bool queryWindowInfo(HWND, WindowInfo&);
    bool isWindowValid(HWND);

    HWND calcInsertAfter(HWND, HWND, bool);

    inline void enableDpiAwareness() {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

}