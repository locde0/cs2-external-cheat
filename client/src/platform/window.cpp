#include <dwmapi.h>
#include <vector>
#include "window.h"

#pragma comment(lib, "dwmapi.lib")

namespace {

    BOOL CALLBACK findWindowCallback(HWND hwnd, LPARAM lp) {
        auto* result = reinterpret_cast<std::pair<DWORD, HWND>*>(lp);
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);

        if (pid != result->first) return TRUE;

        if (!IsWindowVisible(hwnd)) return TRUE;
        if (GetWindow(hwnd, GW_OWNER) != nullptr) return TRUE;
        if (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
        if (GetWindowTextLengthW(hwnd) <= 0) return TRUE;

        BOOL cloaked = FALSE;
        DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        if (cloaked) return TRUE;

        result->second = hwnd;
        return FALSE;
    }

}

namespace platform {

    HWND findMainWindow(DWORD pid) {
        std::pair<DWORD, HWND> data = { pid, nullptr };
        EnumWindows(findWindowCallback, reinterpret_cast<LPARAM>(&data));
        return data.second;
    }

    bool getWindowBounds(HWND hwnd, RECT& out) {
        if (!IsWindow(hwnd)) return false;

        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &out, sizeof(out))))
            return true;

        return GetWindowRect(hwnd, &out) != 0;
    }

    bool queryWindowInfo(HWND hwnd, WindowInfo& out) {
        if (!getWindowBounds(hwnd, out.bounds)) return false;

        out.minimized = IsIconic(hwnd) != 0;
        out.size = { out.bounds.right - out.bounds.left, out.bounds.bottom - out.bounds.top };
        out.topmost = (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        return true;
    }

    bool isWindowValid(HWND hwnd) {
        return IsWindow(hwnd);
    }

    HWND calcInsertAfter(HWND target, HWND overlay, bool isTopmost) {
        HWND windowAboveTarget = GetWindow(target, GW_HWNDPREV);
        HWND insertAfter = windowAboveTarget;

        if (!windowAboveTarget || windowAboveTarget == overlay)
            insertAfter = HWND_TOP;

        if (isTopmost)
            if (!windowAboveTarget)
                insertAfter = HWND_TOPMOST;

        return insertAfter;
    }

}