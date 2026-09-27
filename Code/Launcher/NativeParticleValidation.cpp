#include "FrontierX/NativeParticleManager.h"
#include "IParticleManager.h"
#include "SYSTEM.H"

namespace FrontierX
{
bool ValidateNativeParticleManager(ISystemContainer * container, bool & present)
{
    present = false;
    if (!container) return false;
    IParticleManager * manager = nullptr;
    const GENRESULT result = container->QueryInterface(IID_IParticleManager,
        reinterpret_cast<void **>(&manager));
    if (result == GR_INTERFACE_UNSUPPORTED) return true;
    if (result != GR_OK || !manager) return false;
    present = true;
    IParticleProgramer * program = manager->CreateParticleProgramer(PE_POINT_EMMITER);
    const bool valid = manager->GetFilterNumber() == 29 && program &&
        manager->GetFilterName(PE_POINT_EMMITER);
    if (program) manager->ReleaseInstance(program);
    manager->Release();
    return valid;
}
}
