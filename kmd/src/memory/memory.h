#pragma once
#include <ntifs.h>
#include <ioctls.h>

extern "C" {
	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(
		PEPROCESS,
		PVOID,
		PEPROCESS,
		PVOID,
		SIZE_T,
		KPROCESSOR_MODE,
		PSIZE_T
	);
}

namespace memory {
	void cleanup();

    NTSTATUS attach(HANDLE);
    NTSTATUS read(kmd::request*);
    NTSTATUS write(kmd::request*);
}