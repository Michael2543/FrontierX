#ifndef FRONTIERX_NATIVE_ENGINE_H
#define FRONTIERX_NATIVE_ENGINE_H

struct ICOManager;

namespace FrontierX
{
bool RegisterNativeEngine(ICOManager *manager);

// Native static owners distinguish a failed declared component from absence.
// The legacy BOOL32 component interface cannot express that distinction.
struct NativeArchetypeLoadFailure {};
}

#endif
