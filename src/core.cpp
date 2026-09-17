#include "core.h"

namespace Core
{
    void InjectPatches()
    {
        Hooking::MakeJMP((BYTE *)0xAE63CA, (DWORD)Floors::InitCameraVar, 5);
        Hooking::MakeJMP((BYTE *)0xAE4880, (DWORD)Floors::DetachCamerasOnShutdown, 6);
        Floors::FixVisibilityFilter();
        Hooking::MakeJMP((BYTE *)0xA62A10, (DWORD)Floors::ConsiderLevelZeroFloors, 6);
        Hooking::MakeJMP((BYTE *)0xA62A2E, (DWORD)Floors::UpdateCamerasOnLevelChange, 5);
        Hooking::MakeJMP((BYTE *)0xB62649, (DWORD)Floors::FindValidTileForPlane, 6);
    }
}