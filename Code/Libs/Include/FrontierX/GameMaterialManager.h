#ifndef FRONTIERX_GAME_MATERIAL_MANAGER_H
#define FRONTIERX_GAME_MATERIAL_MANAGER_H

struct ICOManager;
struct IMaterialManager;

namespace FrontierX
{
bool RegisterNativeGameMaterialManager(ICOManager *manager);
// Release borrowed scene resources after all material consumers have stopped.
bool ResetNativeGameMaterialManager(IMaterialManager *manager);
}

#endif
