#pragma once
#pragma warning (disable : 4047 4024)

#include "ntifs.h"
#include "message.h"

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProc,
	PVOID SourceAddr,
	PEPROCESS TargetProc,
	PVOID TargetAddr,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PrevMode,
	PSIZE_T ReturnSize
);

NTSTATUS KernelReadVirtualMemory(PEPROCESS Process, PVOID SourceAddr, PVOID TargetAddr, SIZE_T Size);
NTSTATUS KernelWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddr, PVOID TargetAddr, SIZE_T Size);