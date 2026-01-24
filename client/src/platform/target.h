#pragma once
#include <cstdint>

namespace platform {

    class Target {
    public:
        bool find(uint32_t pid);
        bool isValid() const;
        void reset();

        void* handle() const { return _hwnd; }
        
        operator bool() const { return _hwnd != nullptr; }

    private:
        void* _hwnd = nullptr;
    };

}