#pragma once
#include "../win.h"

namespace core::scoped {

    class ScopedHandle {
    public:
        ScopedHandle() = default;
        explicit ScopedHandle(HANDLE);
        ~ScopedHandle();

        ScopedHandle(ScopedHandle&&) noexcept;
        ScopedHandle& operator=(ScopedHandle&&) noexcept;

        ScopedHandle(const ScopedHandle&) = delete;
        ScopedHandle& operator=(const ScopedHandle&) = delete;

        HANDLE get() const { return _h; }
        bool valid() const { return _h && _h != INVALID_HANDLE_VALUE; }

        void reset(HANDLE h = INVALID_HANDLE_VALUE);

    private:
        HANDLE _h = INVALID_HANDLE_VALUE;
    };

}