#include "device.h"
#include "../dispatch/dispatch.h"

namespace device {

    void destroy(PDRIVER_OBJECT driver_object) {
        UNICODE_STRING sym;
        RtlInitUnicodeString(&sym, KMD_SYMLINK);
        IoDeleteSymbolicLink(&sym);

        memory::cleanup();
        if (driver_object->DeviceObject)
            IoDeleteDevice(driver_object->DeviceObject);
    }

    NTSTATUS create(PDRIVER_OBJECT driver_object) {
        UNICODE_STRING dev, sym;
        RtlInitUnicodeString(&dev, KMD_NT_DEVICE);
        RtlInitUnicodeString(&sym, KMD_SYMLINK);

        PDEVICE_OBJECT device_object = nullptr;
        NTSTATUS status = IoCreateDevice(
            driver_object,
            0,
            &dev,
            FILE_DEVICE_UNKNOWN,
            FILE_DEVICE_SECURE_OPEN,
            FALSE,
            &device_object
        );
        if (!NT_SUCCESS(status))
            return status;

        status = IoCreateSymbolicLink(&sym, &dev);
        if (!NT_SUCCESS(status)) {
            IoDeleteDevice(device_object);
            return status;
        }

        device_object->Flags |= DO_BUFFERED_IO;

        driver_object->MajorFunction[IRP_MJ_CREATE] = dispatch::create;
        driver_object->MajorFunction[IRP_MJ_CLOSE] = dispatch::close;
        driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = dispatch::device_control;
        driver_object->DriverUnload = destroy;

        device_object->Flags &= ~DO_DEVICE_INITIALIZING;
        
        return STATUS_SUCCESS;
    }
}