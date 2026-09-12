#pragma once
#include "headers.h"
#include "hooking.h"
#include "TS2.h"

namespace Floors
{
    extern "C" void InitCameraVar();
    extern "C" void DetachCamerasOnShutdown();
    void FixVisibilityFilter();
    extern "C" void ConsiderLevelZeroFloors();
    extern "C" void UpdateCamerasOnLevelChange();
    extern "C" void FindValidTileForPlane();
}