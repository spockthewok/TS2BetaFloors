#include "floors.h"

namespace
{
    struct FloorNode
    {
        const char *name;
        struct FloorNode *next;
    };
    FloorNode *nodeHead = nullptr;

    const DWORD SetLevelViewed_Exit = 0xA62A33;
    const DWORD SetTile_Exit_1 = 0xAE6D08;
    const DWORD SetTile_Exit_2 = 0xAE70D3;
    const DWORD SetReflectionStateMaterialOverrides_Exit = 0xB6243B;
    const DWORD ConfigureReflectionCamera_Exit = 0xB6263E;

    int camLevel = 0;
}

namespace Floors
{
    // Gets material name of a floor defined as "FloorReflective"
    void __declspec(naked) GetReflectiveFloorMaterial()
    {
        __asm {
            pushad
            mov eax,[esp+0x64]
            push eax
            call AddFloorToList
            add esp,0x4
            popad
            mov edx,[edi]
            mov ecx,edi
            call [edx+0x4C]
            jmp SetReflectionStateMaterialOverrides_Exit
        }
    }

    // Stores material name of reflective floor in linked list
    void AddFloorToList(const char *matName)
    {
        if (!matName)
            return;
        FloorNode *node = new FloorNode;
        node->name = matName;
        node->next = nodeHead;
        nodeHead = node;
    }

    // Checks if passed material name is stored in linked list
    bool IsFloorReflective(const char *matName)
    {
        if (!matName)
            return false;

        for (FloorNode *node = nodeHead; node; node = node->next)
        {
            if (_stricmp(matName, node->name) == 0)
                return true;
        }
        return false;
    }

    // Marks current cFloor object as reflective if its material name is in linked list
    void __declspec(naked) CheckReflective()
    {
        __asm {
            mov edx,[esp+0xF8]
            pushad
            push edx
            call IsFloorReflective
            test al,al
            jz LAB_Exit
            mov [edi+0x80],0x1 // Camera checks this is not null before doing anything, not initialised by default
        LAB_Exit:
            add esp,0x4
            popad
            jmp SetTile_Exit_1
        }
    }

    // Patches in call to method responsible for setting up reflection camera
    void __declspec(naked) EnableFloorReflectionCamera()
    {
        __asm {
            mov byte ptr [esp+0xE0],bl
            cmp [edi+0x80],0x0
            je LAB_Exit // Skip if reflective flag not set for object
            pushad
            mov ecx,edi
            push 0x1 // Attach reflection camera
            call cFloor::SetReflectionState
            popad
        LAB_Exit:
            jmp SetTile_Exit_2
        }
    }

    // Patches out checks in visibility filter that cause reflection ghosting for some reason
    void FixVisibilityFilter()
    {
        Hooking::Nop((BYTE *)0xB62256, 22);
    }

    // Clamps reflection height calculations to current camera level
    void __declspec(naked) ClampReflectionsToCamLevel()
    {
        __asm {
            mov eax,[esp+0x14]
            cmp eax,[camLevel]
            je LAB_Exit
            mov eax,[camLevel]
            mov [esp+0x14],eax
        LAB_Exit:
            mov [esp+0x24],eax
            jmp ConfigureReflectionCamera_Exit
        }
    }

    // cFloorManager::SetLevelViewed skips processing for floors on level 0
    // We need to patch this out so reflective floors at level 0 have their cameras updated correctly
    void ConsiderLevelZeroFloors()
    {
        Hooking::Nop((BYTE *)0xA62A10, 4);
    }

    // Detaches and reattaches reflection cameras on floor level change to keep reflections in sync
    void __declspec(naked) UpdateCameraOnLevelChange()
    {
        __asm {
            mov [camLevel],edi
            mov edx,[eax]
            pushad
            cmp [eax+0x80],0x0 // Skip if floor not reflective
            je LAB_Exit
            mov ecx,eax
            push 0x0 // Detach reflection cameras
            call cFloor::SetReflectionState
            popad
            pushad
            mov ecx,eax
            push 0x1 // Reattach reflection cameras
            call cFloor::SetReflectionState
        LAB_Exit:
            popad
            push ecx
            mov ecx,eax
            jmp SetLevelViewed_Exit
        }
    }
}