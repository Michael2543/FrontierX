#ifndef FRONTIERX_NATIVE_ANIMATION_H
#define FRONTIERX_NATIVE_ANIMATION_H

struct ICOManager;

namespace FrontierX
{
bool RegisterNativeAnimation(ICOManager * manager);
// Existing shell regression: source animation list release for all node kinds.
bool CheckNativeAnimationNodeRelease();
}

#endif
