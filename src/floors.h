#pragma once
#include "headers.h"
#include "hooking.h"
#include "TS2.h"

namespace Floors
{
    extern "C" void GetReflectiveFloorMaterial();
    void AddFloorToList(const char *name);
    bool IsFloorReflective(const char *matName);
    extern "C" void CheckReflective();
    extern "C" void EnableFloorReflectionCamera();
    void FixVisibilityFilter();
    extern "C" void ClampReflectionsToCamLevel();
    extern "C" void ConsiderLevelZeroFloors();
    extern "C" void UpdateCameraOnLevelChange();
}