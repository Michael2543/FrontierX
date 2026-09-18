#include "EventSys.h"
#include "FrontierX/NativeComponentModule.h"

void RegisterTheEventSystem(ICOManager * manager);
extern "C" BOOL32 FrontierXEventSystemModuleCanUnload();

extern "C" GENRESULT FrontierXRegisterComponents(ICOManager * manager)
{
    if (!manager) return GR_INVALID_PARMS;
    RegisterTheEventSystem(manager);
    return GR_OK;
}

extern "C" BOOL32 FrontierXCanUnloadComponents()
{
    return FrontierXEventSystemModuleCanUnload();
}
