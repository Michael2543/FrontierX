#ifndef FRONTIERX_NATIVE_COMPONENT_MANAGER_H
#define FRONTIERX_NATIVE_COMPONENT_MANAGER_H

#include "DACOM.H"
#include "FrontierX/DynamicLibrary.h"
#include "FrontierX/NativeComponentModule.h"

#include <cstddef>
#include <string>
#include <vector>

namespace FrontierX
{
class NativeComponentManager final : public ICOManager
{
public:
    NativeComponentManager() = default;
    ~NativeComponentManager();
    NativeComponentManager(const NativeComponentManager &) = delete;
    NativeComponentManager & operator=(const NativeComponentManager &) = delete;

    GENRESULT COMAPI QueryInterface(const C8 * interfaceName, void ** instance) override;
    U32 COMAPI AddRef() override;
    U32 COMAPI Release() override;
    GENRESULT COMAPI CreateInstance(DACOMDESC * descriptor, void ** instance) override;
    GENRESULT COMAPI RegisterComponent(IComponentFactory * component,
                                       const C8 * interfaceName,
                                       U32 priority = DACOM_NORMAL_PRIORITY) override;
    GENRESULT COMAPI UnregisterComponent(IComponentFactory * component,
                                         const C8 * interfaceName = nullptr) override;
    GENRESULT COMAPI EnumerateComponents(const C8 * interfaceName,
                                         DACOMENUMCALLBACK callback,
                                         void * context = nullptr) override;
    GENRESULT COMAPI AddLibrary(const C8 * filename) override;
    GENRESULT COMAPI RemoveLibrary(const C8 * filename) override;
    GENRESULT COMAPI ShutDown() override;
    GENRESULT COMAPI SetINIConfig(const C8 * info, U32 flags = 0) override;

    std::size_t RegisteredCount() const noexcept;

private:
    struct Registration
    {
        IComponentFactory * component = nullptr;
        const C8 * interfaceName = nullptr;
        U32 priority = 0;
        std::size_t libraryId = 0;
    };

    struct LoadedLibrary
    {
        std::string filename;
        DynamicLibrary library;
        NativeComponentCanUnload canUnload = nullptr;
        std::size_t id = 0;
        U32 references = 1;
    };

    void RemoveRegistrations(std::size_t libraryId) noexcept;
    static bool CanUnload(const LoadedLibrary & loaded) noexcept;
    GENRESULT ConfigureLibraries(const C8 * info, U32 flags);

    std::vector<Registration> registrations_;
    std::vector<LoadedLibrary> libraries_;
    std::size_t activeLibraryId_ = 0;
    std::size_t nextLibraryId_ = 1;
    bool configured_ = false;
};
}

#endif
