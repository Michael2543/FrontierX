#ifndef FRONTIERX_NATIVE_PARTICLE_MANAGER_H
#define FRONTIERX_NATIVE_PARTICLE_MANAGER_H
struct ICOManager;
struct ISystemContainer;
namespace FrontierX
{
bool RegisterNativeParticleManager(ICOManager * manager);
// Exercise one real filter factory from a successfully loaded App container.
bool ValidateNativeParticleManager(ISystemContainer * container, bool & present);
}
#endif
