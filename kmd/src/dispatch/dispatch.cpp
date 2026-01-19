#include "dispatch.h"

namespace dispatch {

    constexpr SIZE_T MAX_COPY = 1ull << 20;

    static NTSTATUS complete(PIRP irp, NTSTATUS status, ULONG_PTR info) {
        irp->IoStatus.Status = status;
        irp->IoStatus.Information = info;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return status;
    }

    NTSTATUS create(PDEVICE_OBJECT, PIRP irp) {
		return complete(irp, STATUS_SUCCESS, 0);
    }

    NTSTATUS close(PDEVICE_OBJECT, PIRP irp) {
        return complete(irp, STATUS_SUCCESS, 0);
    }

    NTSTATUS device_control(PDEVICE_OBJECT, PIRP irp) {
        auto stack = IoGetCurrentIrpStackLocation(irp);
        if (!stack || !irp->AssociatedIrp.SystemBuffer)
            return complete(irp, STATUS_INVALID_PARAMETER, 0);

        auto inLen = stack->Parameters.DeviceIoControl.InputBufferLength;
        auto outLen = stack->Parameters.DeviceIoControl.OutputBufferLength;
        if (inLen < sizeof(kmd::request) || outLen < sizeof(kmd::request))
            return complete(irp, STATUS_BUFFER_TOO_SMALL, 0);

        auto req = reinterpret_cast<kmd::request*>(irp->AssociatedIrp.SystemBuffer);
        if (!req)
            return complete(irp, STATUS_INVALID_PARAMETER, 0);

        NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
        const auto code = static_cast<kmd::ioctl>(stack->Parameters.DeviceIoControl.IoControlCode);

        switch (code) {
        case kmd::ioctl::attach:
            status = memory::attach(req->process_id);
            break;

        case kmd::ioctl::read:
            status = memory::read(req);
            break;

        case kmd::ioctl::write:
            status = memory::write(req);
            break;

        default:
            break;
        }

        ULONG_PTR info = NT_SUCCESS(status) ? sizeof(kmd::request) : 0;
        return complete(irp, status, info);
    }
}
