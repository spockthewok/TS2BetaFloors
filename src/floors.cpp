#include "floors.h"

namespace
{
    const DWORD SetLevelViewed_Exit_1 = 0xA62A20;
    const DWORD SetLevelViewed_Exit_2 = 0xA62A2E;
    const DWORD SetLevelViewed_Exit_3 = 0xA62A33;
    const DWORD SetLevelViewed_Exit_4 = 0xA62A36;
    const DWORD Shutdown_Exit = 0xAE4886;
    const DWORD Initialize_Exit = 0xAE63CF;
}

namespace Floors
{
    // cFloor::Initialize
    // cFloor::SetReflectionState checks one of the object's vars is not null before doing anything
    // Var never gets initialised by default so we set it during object instantiation
    void __declspec(naked) InitCameraVar()
    {
        __asm {
            mov ecx,ebx
            call [eax+0xC]
            mov [esi+0x80],0x1
            jmp Initialize_Exit
        }
    }

    // cFloor::Shutdown
    // Detaches cameras as part of object destruction when exiting a lot
    void __declspec(naked) DetachCamerasOnShutdown()
    {
        __asm {
            pushad
            mov ecx,esi
            push 0x0
            call cFloor::SetReflectionState
            popad
            mov [esi+0x80],0x0
            mov ecx,[esi+0x84]
            jmp Shutdown_Exit
        }
    }

    // cFloorReflectionVisibilityQueryFilter::TestNode
    // Patches out height range checks in visibility filter that cause reflection ghosting
    // It's possible to fix this by subtracting 0.75 from lower bound for certain node types
    // This is much simpler and achieves the same result
    void FixVisibilityFilter()
    {
        Hooking::Nop((BYTE *)0xB62256, 22);
    }

    // cFloorManager::SetLevelViewed
    // Floors at level 0 are skipped during processing
    // We need to alter this logic so reflection camera for level 0 floors gets updated correctly
    void __declspec(naked) ConsiderLevelZeroFloors()
    {
        __asm {
            mov eax,[ebx]
            push esi
            mov ecx,ebx
            call [eax+0x4C]
            test eax,eax
            jz LAB_Skip
            test esi,esi
            jz LAB_Level0
            jmp SetLevelViewed_Exit_1
        LAB_Level0:
            mov ecx,0x123A594 // "floorlocalgridblack"
            jmp SetLevelViewed_Exit_2
        LAB_Skip:
            jmp SetLevelViewed_Exit_4
        }
    }

    // cFloorManager::SetLevelViewed
    // Detaches and reattaches reflection cameras on floor level change to keep reflections in sync
    // Game can't handle multiple viewer nodes for one class so we only reattach camera for current level
    void __declspec(naked) UpdateCamerasOnLevelChange()
    {
        __asm {
            mov edx,[eax]
            pushad
            mov ecx,eax
            push 0x0
            call cFloor::SetReflectionState
            popad
            pushad
            mov ecx,eax
            call [edx+0x50] // cFloor::Level
            cmp eax,edi // EDI = currently viewed level
            jne LAB_Exit
            popad
            pushad
            mov ecx,eax
            push 0x1
            call cFloor::SetReflectionState
        LAB_Exit:
            popad
            push ecx
            mov ecx,eax
            jmp SetLevelViewed_Exit_3
        }
    }
}