#ifndef FRONTIERX_NATIVE_MESH_MANAGER_H
#define FRONTIERX_NATIVE_MESH_MANAGER_H

#include <cstdint>

struct ICOManager;
struct IMeshInstance;

namespace FrontierX
{
bool RegisterNativeMeshManager(ICOManager * manager);
// Inspect the production native owner without adding a shared interface slot.
std::int32_t GetNativeMeshInstanceIndex(const IMeshInstance * instance);
}

#endif
