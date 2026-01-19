#pragma once
#include <ntifs.h>
#include <ioctls.h>
#include "../memory/memory.h"

namespace dispatch {
    NTSTATUS create(PDEVICE_OBJECT, PIRP);
    NTSTATUS close(PDEVICE_OBJECT, PIRP);
    NTSTATUS device_control(PDEVICE_OBJECT, PIRP);
}