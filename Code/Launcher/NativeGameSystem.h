#pragma once

#include "SYSTEM.H"
#include "WindowManager.h"

struct IEngine;

namespace FrontierX
{
struct NativeGameSystem;

NativeGameSystem * StartNativeGameSystem(const char * appName,
                                       WMEXITCB exitCallback, U32 flags);
ISystemContainer * NativeGameSystemContainer(NativeGameSystem * system);
IEngine * NativeGameSystemEngine(NativeGameSystem * system);
void ReleaseNativeGameSystemEngine(NativeGameSystem * system);
void DestroyNativeGameSystem(NativeGameSystem * system);
}
