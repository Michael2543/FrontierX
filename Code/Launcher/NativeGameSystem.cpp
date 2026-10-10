#include "NativeGameSystem.h"

#include "ICamera.h"
#include "gamesys.H"
#include "renderer.h"

#include <new>

namespace FrontierX
{
struct NativeGameSystem
{
    GameSystem game;
};

NativeGameSystem * StartNativeGameSystem(const char * appName,
                                       WMEXITCB exitCallback, U32 flags)
{
    NativeGameSystem * system = new (std::nothrow) NativeGameSystem;
    if (!system) return nullptr;
    if (!system->game.startup(nullptr, appName, exitCallback, flags, nullptr))
    {
        delete system;
        return nullptr;
    }
    return system;
}

ISystemContainer * NativeGameSystemContainer(NativeGameSystem * system)
{
    return system ? system->game.GS.ptr : nullptr;
}

IEngine * NativeGameSystemEngine(NativeGameSystem * system)
{
    return system ? system->game.ENG.ptr : nullptr;
}

bool LoadNativeGameSystemEngineComponents(NativeGameSystem * system,
                                          ISystemContainer * container,
                                          IRenderer ** renderer)
{
    if (!system || !system->game.ENG.ptr || !container || !renderer || *renderer)
        return false;
    return system->game.ENG.ptr->load_engine_components(container) == GR_OK &&
        system->game.ENG.ptr->QueryInterface(IID_IRenderer,
            reinterpret_cast<void **>(renderer)) == GR_OK && *renderer;
}

void ReleaseNativeGameSystemEngine(NativeGameSystem * system)
{
    if (system) system->game.ENG.free();
}

void DestroyNativeGameSystem(NativeGameSystem * system)
{
    if (!system) return;
    system->game.shut_down();
    delete system;
}
}
