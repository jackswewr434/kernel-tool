#pragma warning (disable : 4047 4311)

#include "events.h"
#include "message.h"
#include "data.h"
PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{	
	//DebugMessage("Dll Loaded, %ls \n", FullImageName->Buffer);
	//Dll Loaded, \Device\HarddiskVolume3\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\bin\win64\steam_api64.dll 
	if (wcsstr(FullImageName->Buffer, L"\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\game\\csgo\\bin\\win64\\client.dll")) {
		DebugMessage("DLL Found!\n");
		clientAddr = ImageInfo->ImageBase;
		ProcessID = (ULONG)ProcessId;
		DebugMessage("ProcessID: %d \n", (int)ProcessId);
	}
	return STATUS_SUCCESS;
}
