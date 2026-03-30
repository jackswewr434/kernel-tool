#include <ntifs.h>
#pragma warning (disable : 4100 4047 4024)
#include "global.h"
#include "message.h"
#include "events.h"
#include "communication.h"
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) {
	UNREFERENCED_PARAMETER(pRegistryPath);
	pDriverObject->DriverUnload = UnloadDriver;
	RtlInitUnicodeString(&dev, L"\\Device\\endr1337");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\endr1337");


	DebugMessage("loaded upppp!\n");


	PsSetLoadImageNotifyRoutine(ImageLoadCallback);



	IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&dos, &dev);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	pDeviceObject->Flags &= ~DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}


NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject) {

	UNREFERENCED_PARAMETER(pDriverObject);

	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);

	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriverObject->DeviceObject);
	DebugMessage("Unloaded!\n");
	return STATUS_SUCCESS;
}