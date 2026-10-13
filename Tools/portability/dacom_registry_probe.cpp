// Native Windows regression for the production DACOM/D3D provider registry.
// No retail data, graphics device, mission or replacement factory is used.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "DACOM.H"
#include <cstdio>
#include <cstring>

static BOOL32 DACOM_CDECL CountProvider(IComponentFactory *, const C8 *, U32, void *context)
{
    ++*static_cast<unsigned int *>(context);
    return TRUE;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char directory[MAX_PATH + 4];
    const char *path = nullptr;
    if (!std::strcmp(argv[1], "dot")) path = ".\\";
    else if (!std::strcmp(argv[1], "absolute"))
    {
        if (!GetModuleFileNameA(nullptr, directory, sizeof(directory))) return 2;
        char *slash = std::strrchr(directory, '\\');
        if (!slash) return 2;
        slash[1] = 0;
        path = directory;
    }
    else if (std::strcmp(argv[1], "unset")) return 2;
    if (!SetEnvironmentVariableA("DACOM", path)) return 2;
    ICOManager *manager = DACOM_Acquire();
    if (!manager || manager->SetINIConfig("[Libraries]\nD3DRenderPipe.dll\n", DACOM_INI_STRING) != GR_OK)
        return 1;
    unsigned int providers = 0;
    if (manager->EnumerateComponents("IRenderPipeline", CountProvider, &providers) != GR_OK || providers != 1)
    {
        std::fputs("FAIL: production render pipeline factory lost or duplicated\n", stderr);
        return 1;
    }
    manager->ShutDown();
    if (GetModuleHandleA("D3DRenderPipe.dll"))
    {
        std::fputs("FAIL: production provider remains pinned after shutdown\n", stderr);
        return 1;
    }
    std::puts("PASS: native DACOM provider registry and module release");
    return 0;
}
