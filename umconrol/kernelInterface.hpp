#pragma once
#include "communication.hpp"

class KernelInterface {
public:
    HANDLE hDriver;

    KernelInterface(LPCSTR RegistryPath) {
        hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    }

    // FIX 1: ULONGLONG + correct IOCTL usage
    ULONGLONG GetClientAddress() {
        if (hDriver == INVALID_HANDLE_VALUE) return 0;

        ULONGLONG addr = 0;  // Changed from ULONG
        DWORD bytes;
        DeviceIoControl(hDriver, IO_GET_CLIENTADDRESS, NULL, 0,
            &addr, sizeof(addr), &bytes, NULL);
        return addr;  // Now returns 7FFCCDBB0000
    }

    // FIX 2: Correct IOCTL usage
    ULONGLONG GetProcessID() {
        if (hDriver == INVALID_HANDLE_VALUE) return 0;

        ULONGLONG pid = 0;
        DWORD bytes;
        DeviceIoControl(hDriver, IO_REQUEST_PROCESSID, NULL, 0,
            &pid, sizeof(pid), &bytes, NULL);
        return pid;
    }

    // FIX 3: Remove unnecessary casts
    template<typename type>
    type ReadVirtualMemory(uint64_t processID, uint64_t address, size_t size) {
        type result{};
        KERNEL_READ_REQUEST request = { processID, address, &result, (ULONG)size };  // Direct assign

        DWORD bytesReturned = 0;
        if (DeviceIoControl(hDriver, IO_READ_REQUEST, &request, sizeof(request),
            &request, sizeof(request), &bytesReturned, NULL)) {
            return result;
        }

        DWORD error = GetLastError();
        printf("[READ FAIL] PID=0x%llX addr=0x%llX size=%zu kernel_error=0x%08X\n",
            processID, address, size, error);
        return {};
    }

    // FIX 4: uint64_t params + correct IOCTL
    template<typename type>
    bool WriteVirtualMemory(uint64_t ProcID, uint64_t writeAddr, type writeValue, size_t Size) {
        if (hDriver == INVALID_HANDLE_VALUE) return false;

        KERNEL_WRITE_REQUEST req = { ProcID, writeAddr, &writeValue, (ULONG)Size };
        DWORD bytes;
        return DeviceIoControl(hDriver, IO_WRITE_REQUEST, &req, sizeof(req),
            NULL, 0, &bytes, NULL);
    }
};