#pragma once

#include "SYSTEM.H"
#include "WindowManager.h"

struct IEngine;
struct IRenderer;

namespace FrontierX
{
struct NativeGameSystem;

NativeGameSystem * StartNativeGameSystem(const char * appName,
                                       WMEXITCB exitCallback, U32 flags);
ISystemContainer * NativeGameSystemContainer(NativeGameSystem * system);
IEngine * NativeGameSystemEngine(NativeGameSystem * system);
bool LoadNativeGameSystemEngineComponents(NativeGameSystem * system,
                                          ISystemContainer * container,
                                          IRenderer ** renderer);
void ReleaseNativeGameSystemEngine(NativeGameSystem * system);
void DestroyNativeGameSystem(NativeGameSystem * system);
}
