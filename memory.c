
#include "memory.h"

NTSTATUS KernelReadVirtualMemory(
    PEPROCESS Process,
    PVOID SourceAddr,
    PVOID TargetAddr,
    SIZE_T Size)
{
    SIZE_T Bytes = 0;

    NTSTATUS status = MmCopyVirtualMemory(
        Process,
        SourceAddr,
        PsGetCurrentProcess(),
        TargetAddr,
        Size,
        KernelMode,
        &Bytes
    );

    if (!NT_SUCCESS(status)) {
        DebugMessage("READ FAIL: 0x%08X bytes=%zu addr=0x%p size=%zu\n",
            status, Bytes, SourceAddr, Size);
        return status;
    }

    if (Bytes != Size) {
        DebugMessage("READ PARTIAL: %zu/%zu bytes addr=0x%p\n",
            Bytes, Size, SourceAddr);
    }

    return STATUS_SUCCESS;
}
NTSTATUS KernelWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddr, PVOID TargetAddr, SIZE_T Size) {
    SIZE_T Bytes = 0;
    NTSTATUS status = MmCopyVirtualMemory(PsGetCurrentProcess(), SourceAddr, Process, TargetAddr, Size, KernelMode, &Bytes);

    if (!NT_SUCCESS(status)) {
        DebugMessage("WRITE FAIL: 0x%08X bytes=%zu addr=0x%p size=%zu\n", status, Bytes, SourceAddr, Size);
        return status;
    }

    if (Bytes != Size) {
        ("WRITE PARTIAL: %zu/%zu bytes addr=0x%p\n", Bytes, Size, SourceAddr);
    }

    return STATUS_SUCCESS;
}