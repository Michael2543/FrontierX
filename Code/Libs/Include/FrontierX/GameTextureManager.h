#ifndef FRONTIERX_GAME_TEXTURE_MANAGER_H
#define FRONTIERX_GAME_TEXTURE_MANAGER_H

struct ICOManager;

namespace FrontierX
{
// Explicit native base-level policy; returns the previous selection.
bool SetNativeGameTextureBaseLevelOnly(bool enabled);
bool RegisterNativeGameTextureManager(ICOManager *manager);
}

#endif
