#pragma once
#include <ntifs.h>
#pragma warning(disable : 4047)

ULONGLONG clientAddr;
ULONGLONG ProcessID;
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;