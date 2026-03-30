#pragma warning (disable : 4022)
#pragma warning(disable : 4996)  
#pragma warning(disable : 4005)  // macro redefinition
#pragma warning(disable : 4083)
#include "communication.h"
#include "memory.h"
NTSYSAPI
PCHAR
PsGetProcessImageFileName(
    IN PEPROCESS Process
);
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    ULONG   ByteID = 0;

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;
    ULONG inLen = stack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outLen = stack->Parameters.DeviceIoControl.OutputBufferLength;

    switch (ControlCode)
    {
    case IO_GET_CLIENTADDRESS:
        if (outLen < sizeof(ULONG)) {
            Status = STATUS_BUFFER_TOO_SMALL;
            ByteID = 0;
            goto Complete;
        }
        break;

    case IO_REQUEST_PROCESSID:
        if (outLen < sizeof(ULONG)) {
            Status = STATUS_BUFFER_TOO_SMALL;
            ByteID = 0;
            goto Complete;
        }
        break;

    case IO_READ_REQUEST:
        if (inLen < sizeof(KERNEL_READ_REQUEST)) {
            Status = STATUS_BUFFER_TOO_SMALL;
            ByteID = 0;
            goto Complete;
        }
        break;

    case IO_WRITE_REQUEST:
        if (inLen < sizeof(KERNEL_WRITE_REQUEST)) {
            Status = STATUS_BUFFER_TOO_SMALL;
            ByteID = 0;
            goto Complete;
        }
        break;

    default:
        // Unknown IOCTL
        Status = STATUS_INVALID_DEVICE_REQUEST;
        ByteID = 0;
        goto Complete;
    }

    if (ControlCode == IO_GET_CLIENTADDRESS) {

        PULONGLONG Output = (PULONGLONG)Irp->AssociatedIrp.SystemBuffer;
        *Output = clientAddr;   // or keep as pointer if you want

        DebugMessage("clientAddr Requested\n");

        Status = STATUS_SUCCESS;
        ByteID = sizeof(*Output);
    }
    else if (ControlCode == IO_REQUEST_PROCESSID) {

        PULONGLONG Output = (PULONGLONG)Irp->AssociatedIrp.SystemBuffer;
        *Output = ProcessID;

        DebugMessage("process id Requested\n");

        Status = STATUS_SUCCESS;
        ByteID = sizeof(*Output);
    }
    else if (ControlCode == IO_READ_REQUEST) {
        PKERNEL_READ_REQUEST req = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;

        PEPROCESS process;
        if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)req->ProcId, &process))) {
            Irp->IoStatus.Status = STATUS_INVALID_CID;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            return STATUS_INVALID_CID;
        }

        SIZE_T bytesRead = 0;
        NTSTATUS status = KernelReadVirtualMemory(process, req->Addr, req->pBuff, req->Size);

        ObDereferenceObject(process);

        if (!NT_SUCCESS(status)) {
            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            return status;
        }

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = bytesRead;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }
    else if (ControlCode == IO_WRITE_REQUEST) {

        PKERNEL_WRITE_REQUEST WriteInput =
            (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;

        PEPROCESS Process = NULL;
        if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)WriteInput->ProcId, &Process)))
        {
            KernelWriteVirtualMemory(Process,
                Irp->AssociatedIrp.SystemBuffer,
                WriteInput->Addr,
                WriteInput->Size);

            ObDereferenceObject(Process);

            Status = STATUS_SUCCESS;
            ByteID = sizeof(KERNEL_WRITE_REQUEST);
        }
        else {
            Status = STATUS_INVALID_CID;
            ByteID = 0;
        }
    }

Complete:
    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = ByteID;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Status;
}
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("Connection Terminated\n");
	return STATUS_SUCCESS;
}

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("CreateCall: connection established\n");
	return STATUS_SUCCESS;
}
