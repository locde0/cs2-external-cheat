#include "driver.h"
#include "../device/device.h"

extern "C"
NTSTATUS DriverEntry(
    PDRIVER_OBJECT driver_object,
    PUNICODE_STRING registry_path
) {
    UNREFERENCED_PARAMETER(registry_path);
    return device::create(driver_object);
}
