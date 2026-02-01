#include "driver.h"
#include <ioctls.h>
#include "../device/device.h"

NTSTATUS driver_main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
    UNREFERENCED_PARAMETER(registry_path);
    return device::create(driver_object);
}

extern "C"
NTSTATUS DriverEntry(
    PDRIVER_OBJECT driver_object,
    PUNICODE_STRING registry_path
) {
    UNREFERENCED_PARAMETER(driver_object);
    UNREFERENCED_PARAMETER(registry_path);

    UNICODE_STRING driver_name;
    RtlInitUnicodeString(&driver_name, KMD_DRIVER_NAME);

    return IoCreateDriver(&driver_name, &driver_main);
}
