#include "scoped.h"

namespace core {

    ScopedHandle::ScopedHandle(HANDLE h)
        : _h(h)
    {}

    ScopedHandle::ScopedHandle(ScopedHandle&& o) noexcept
        : _h(o._h)
    {
        o._h = INVALID_HANDLE_VALUE;
    }

    ScopedHandle::~ScopedHandle() {
        reset();
    }

    ScopedHandle& ScopedHandle::operator=(ScopedHandle&& o) noexcept {
        if (this != &o) { 
            reset(); 
            _h = o._h; 
            o._h = INVALID_HANDLE_VALUE; 
        }
        return *this;
    }

    void ScopedHandle::reset(HANDLE h) {
        if (valid()) 
            CloseHandle(_h);
        _h = h;
    }

    ScopedScHandle::ScopedScHandle(SC_HANDLE h) 
        : _h(h) 
    {}

    ScopedScHandle::ScopedScHandle(ScopedScHandle&& o) noexcept 
        : _h(o._h) 
    { 
        o._h = nullptr; 
    }

    ScopedScHandle::~ScopedScHandle() {
        if (_h)
            CloseServiceHandle(_h);
    }

    ScopedScHandle& ScopedScHandle::operator=(ScopedScHandle&& o) noexcept {
        if (this != &o) { 
            if (_h) CloseServiceHandle(_h); 
            _h = o._h; 
            o._h = nullptr; 
        }
        return *this;
    }

}