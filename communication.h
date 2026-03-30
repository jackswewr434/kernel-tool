#pragma once
#pragma once
#include <ntifs.h>
#include "message.h"
#include "data.h"
typedef unsigned long long uint64_t;
#define IO_GET_CLIENTADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x420,METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x421,METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x422,METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_REQUEST_PROCESSID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x423,METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

typedef struct _KERNEL_READ_REQUEST {
    ULONGLONG ProcId;     // 64-bit PID 
    uint64_t Addr;      
    PVOID pBuff;
    ULONG Size;          
} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST {
    ULONGLONG ProcId;     // 64-bit PID
    uint64_t Addr;        
    PVOID pBuff;
    ULONG Size;          
} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;