#pragma once
#include <ntifs.h>

namespace device {
    NTSTATUS create(PDRIVER_OBJECT);
    void destroy(PDRIVER_OBJECT);
}