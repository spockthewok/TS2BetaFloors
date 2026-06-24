#include "core.h"

namespace Core
{
    void InjectPatches()
    {
        Hooking::MakeJMP((BYTE *)0xB62434, (DWORD)Floors::GetReflectiveFloorMaterial, 7);
        Hooking::MakeJMP((BYTE *)0xAE6D01, (DWORD)Floors::CheckReflective, 7);
        Hooking::MakeJMP((BYTE *)0xAE70CC, (DWORD)Floors::EnableFloorReflectionCamera, 7);
        Floors::FixVisibilityFilter();
        Hooking::MakeJMP((BYTE *)0xB62636, (DWORD)Floors::ClampReflectionsToCamLevel, 8);
        Hooking::MakeJMP((BYTE *)0xA62A10, (DWORD)Floors::ConsiderLevelZeroFloors, 6);
        Hooking::MakeJMP((BYTE *)0xA62A2E, (DWORD)Floors::UpdateCameraOnLevelChange, 5);
    }
}