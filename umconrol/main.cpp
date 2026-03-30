#pragma warning(disable : 4996)  
#pragma warning(disable : 4005)  
#pragma warning(disable : 4081)

#include <iostream>
#include "kernelInterface.hpp"
#include "offsets.h" 
#include "clientdll.hpp"
#include <Windows.h>
#include <winternl.h>
#include <filesystem>
#pragma comment(lib, "ntdll.lib")
//preciate yeri diaz for moral support
#define ProcessProtectionLevelInformation ((PROCESSINFOCLASS)50)  // Fix 1: Cast to PROCESSINFOCLASS
#define PROCESS_PROTECTION_SYSTEM        2
#define PROCESS_PROTECTION_AUDIT         4

struct Vector3 {
    float x, y, z;
};
int main() {
    KernelInterface driver("\\\\.\\endr1337");
    uint64_t processID = driver.GetProcessID();
    uint64_t address = driver.GetClientAddress();

    printf("Counter Strike Address: 0x%p\n", address);
    printf("Proc ID:  0x%p\n", processID);  // Fix 2: %llX for uint64_t

    ULONGLONG localPlayerPawn = driver.ReadVirtualMemory<uint64_t>(processID, address + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn, sizeof(uint64_t));
    printf("localplayerPawn 0x%p\n", localPlayerPawn);
    ULONGLONG entityList = driver.ReadVirtualMemory<uint64_t>(processID, address + cs2_dumper::offsets::client_dll::dwEntityList, sizeof(uint64_t));
    printf("entity list = %p\n", entityList);
    ULONGLONG localPlayerController = driver.ReadVirtualMemory<uint64_t>(processID, address + cs2_dumper::offsets::client_dll::dwLocalPlayerController, sizeof(uint64_t));
    int localTeam = driver.ReadVirtualMemory<int>(
        processID,
        localPlayerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum,
        sizeof(int)
    );
 

    printf("=== SINGLE ITERATION TEST ===\n");
    FILE* log = fopen("C:\\temp\\cs2_debug.log", "w");
    fprintf(log, "PID=0x%llX LocalPawn=0x%llX EntityList=0x%llX\n",
        processID, localPlayerPawn, entityList);
    fclose(log);

#define LOG(fmt, ...) do { \
    FILE* f = fopen("C:\\temp\\cs2_debug.log", "a"); \
    if (f) { \
        fprintf(f, fmt, ##__VA_ARGS__); \
        fclose(f); \
    } \
} while(0)

    LOG("LocalPawn=0x%llX EntityList=0x%llX LocalTeam=%d\n", localPlayerPawn, entityList, localTeam);

    // SINGLE ITERATION i=1
    int i = 1;
    LOG("\n--- Testing player %d ---\n", i);

    uint32_t controllerIdx = (i & 0x7FFF) >> 9;
    LOG("controllerIdx=%u\n", controllerIdx);

    ULONGLONG controllerChunk = driver.ReadVirtualMemory<uint64_t>(
        processID, entityList + (controllerIdx * 8) + 0x10, sizeof(uint64_t)
    );
    LOG("controllerChunk=0x%llX", controllerChunk);
    if (!controllerChunk) {
        LOG(" (NULL - END)\n");
        printf("Test ended: NULL controllerChunk\n");
        return 0;
    }
    LOG(" (OK)\n");

    uint32_t controllerOffset = 120 * (i & 0x1FF);
    LOG("controllerOffset=0x%X\n", controllerOffset);

    ULONGLONG controller = driver.ReadVirtualMemory<uint64_t>(
        processID, controllerChunk + controllerOffset, sizeof(uint64_t)
    );
    LOG("controller=0x%llX", controller);
    if (!controller) {
        LOG(" (NULL - END)\n");
        printf("Test ended: NULL controller\n");
        return 0;
    }
    LOG(" (OK)\n");

    uint32_t pawnHandle = driver.ReadVirtualMemory<uint32_t>(
        processID, controller + 0x090C, sizeof(uint32_t)
    );
    LOG("pawnHandle=0x%08X", pawnHandle);
    if (!pawnHandle) {
        LOG(" (NULL - END)\n");
        printf("Test ended: NULL pawnHandle\n");
        return 0;
    }
    LOG(" (OK)\n");

    uint32_t pawnIdx = (pawnHandle & 0x7FFF) >> 9;
    LOG("pawnIdx=%u\n", pawnIdx);

    ULONGLONG pawnChunk = driver.ReadVirtualMemory<uint64_t>(
        processID, entityList + (pawnIdx * 8) + 0x10, sizeof(uint64_t)
    );
    LOG("pawnChunk=0x%llX", pawnChunk);
    if (!pawnChunk) {
        LOG(" (NULL - END)\n");
        printf("Test ended: NULL pawnChunk\n");
        return 0;
    }
    LOG(" (OK)\n");

    uint32_t pawnOffset = 120 * (pawnHandle & 0x1FF);
    ULONGLONG playerPawn = driver.ReadVirtualMemory<uint64_t>(
        processID, pawnChunk + pawnOffset, sizeof(uint64_t)
    );
    LOG("playerPawn=0x%llX\n", playerPawn);
    if (!playerPawn) {
        LOG("NULL playerPawn - END\n");
        printf("Test ended: NULL playerPawn\n");
        return 0;
    }

    int health = driver.ReadVirtualMemory<int>(
        processID, playerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth, sizeof(int)
    );
    int team = driver.ReadVirtualMemory<int>(
        processID, playerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum, sizeof(int)
    );

    LOG("health=%d team=%d (localTeam=%d)\n", health, team, localTeam);

    if (health < 1 || health > 100 || team == localTeam) {
        LOG("Filtered out (not valid enemy)\n");
    }
    else {
        Vector3 pos = driver.ReadVirtualMemory<Vector3>(
            processID, playerPawn + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin, sizeof(Vector3)
        );
        LOG("*** SUCCESS! Player %d: Health=%d Team=%d Pos=(%.1f,%.1f,%.1f) ***\n",
            i, health, team, pos.x, pos.y, pos.z);
    }

    LOG("=== SINGLE ITERATION COMPLETE ===\n");
    printf("Test complete - check C:\\temp\\cs2_debug.log\n");
}