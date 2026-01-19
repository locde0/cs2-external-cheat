#include "memory.h"

namespace memory {

    static PEPROCESS target_process = nullptr;

    void cleanup() {
        if (target_process) {
            ObDereferenceObject(target_process);
            target_process = nullptr;
        }
    }

    NTSTATUS attach(HANDLE pid) {
		cleanup();
        return PsLookupProcessByProcessId(pid, &target_process);
    }

    NTSTATUS read(kmd::request* req) {
        if (!target_process) return STATUS_INVALID_HANDLE;

        return MmCopyVirtualMemory(
            target_process,
            req->target,
            PsGetCurrentProcess(),
            req->buffer,
            req->size,
            KernelMode,
            &req->transferred
        );
    }

    NTSTATUS write(kmd::request* req) {
        if (!target_process) return STATUS_INVALID_HANDLE;

        return MmCopyVirtualMemory(
            PsGetCurrentProcess(),
            req->buffer,
            target_process,
            req->target,
            req->size,
            KernelMode,
            &req->transferred
        );
    }
}
