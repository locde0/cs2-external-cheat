#pragma once
#include <ntifs.h>

extern "C" {
    NTKERNELAPI NTSTATUS IoCreateDriver(
        PUNICODE_STRING,
        PDRIVER_INITIALIZE
    );
}

extern "C" DRIVER_INITIALIZE DriverEntry;