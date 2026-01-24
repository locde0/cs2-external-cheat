#pragma once
#include <string>

namespace core::path {

    std::wstring getDir();
    std::wstring makeFullPath(const std::wstring&);

}