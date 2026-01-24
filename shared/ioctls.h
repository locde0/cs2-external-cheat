#pragma once

#if defined(_KERNEL_MODE) 
    #include <ntddk.h>
#else
    #include <Windows.h>
    #include <winioctl.h>
#endif

#define KMD_DEVICE_NAME L"\\\\.\\kmd"
#define KMD_NT_DEVICE L"\\Device\\kmd"
#define KMD_SYMLINK L"\\DosDevices\\kmd"

namespace kmd {

    enum ioctl : ULONG {
        attach = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS),
        read = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS),
        write = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS),
    };

    struct request {
        HANDLE process_id;

        void* target;
        void* buffer;
        
        SIZE_T size;
        SIZE_T transferred;
    };

}
