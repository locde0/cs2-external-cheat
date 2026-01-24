#pragma once
#include <cstdint>
#include "../core/win.h"

namespace driver {

    DWORD findPid(const wchar_t*);
    std::uintptr_t findModuleBase(DWORD, const wchar_t*);

}