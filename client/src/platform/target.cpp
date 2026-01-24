#include "target.h"
#include "window.h"
#include "../core/win.h"

namespace platform {

    bool Target::find(uint32_t pid) {
        _hwnd = findMainWindow(static_cast<DWORD>(pid));
        return _hwnd != nullptr;
    }

    bool Target::isValid() const {
        return _hwnd && IsWindow(static_cast<HWND>(_hwnd));
    }

    void Target::reset() {
        _hwnd = nullptr;
    }

}
