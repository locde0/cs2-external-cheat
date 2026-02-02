#include "scoped.h"

namespace core::scoped {

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

}