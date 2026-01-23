#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

namespace driver {
    DWORD findPId(const wchar_t*);
    std::uintptr_t findMBase(DWORD, const wchar_t*);
}