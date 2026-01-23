#pragma once
#include <Windows.h>

namespace core {

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

    class ScopedScHandle {
    public:
        explicit ScopedScHandle(SC_HANDLE h);
        ~ScopedScHandle();

        ScopedScHandle(ScopedScHandle&&) noexcept;
        ScopedScHandle& operator=(ScopedScHandle&&) noexcept;

        ScopedScHandle(const ScopedScHandle&) = delete;
        ScopedScHandle& operator=(const ScopedScHandle&) = delete;

        SC_HANDLE get() const { return _h; }
        operator bool() const { return _h != nullptr; }

    private:
        SC_HANDLE _h = nullptr;
    };

}