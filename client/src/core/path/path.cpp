#include "path.h"
#include "../win.h"
#include <filesystem>

namespace core::path {

    std::wstring getDir() {
        wchar_t buf[MAX_PATH];
        DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);

        if (n == 0 || n == MAX_PATH) return L"";
        return std::filesystem::path(buf).parent_path().wstring();
    }

    std::wstring makeFullPath(const std::wstring& filename) {
        std::filesystem::path path = std::filesystem::path(getDir()) / filename;
        return path.lexically_normal().wstring();
    }

}
