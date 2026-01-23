#pragma once
#include <stdexcept>
#include <string>
#include <Windows.h>

inline void throwIfFailed(HRESULT hr, const char* what) {
    if (FAILED(hr)) {
        char buf[256];
        sprintf_s(buf, "%s (hr=0x%08X)", what, (unsigned)hr);
        throw std::runtime_error(buf);
    }
}