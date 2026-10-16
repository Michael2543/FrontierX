#ifndef FRONTIERX_NATIVE_ANIMATION_H
#define FRONTIERX_NATIVE_ANIMATION_H

struct ICOManager;
struct IAnimation;
class Matrix;

namespace FrontierX
{
bool RegisterNativeAnimation(ICOManager * manager);
// Existing shell regression: source animation list release for all node kinds.
bool CheckNativeAnimationNodeRelease();
// Convert only the selected instance's authored root Object motion into the
// owning runtime frame. Shared archives and child Joint channels stay authored.
bool ConfigureNativeAnimationRootBasis(IAnimation *animation, int script,
                                      int root, const Matrix &basis);
}

#endif
