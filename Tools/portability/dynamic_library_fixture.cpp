#include "DACOM.H"

#include <cstring>

namespace
{
U32 liveInstances = 0;
bool failNextRegistration = false;
IDAComponent * failedRegistrationObject = nullptr;

class FixtureComponent final : public IDAComponent
{
public:
    FixtureComponent()
    {
        ++liveInstances;
    }

    ~FixtureComponent()
    {
        --liveInstances;
    }

    GENRESULT COMAPI QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!interfaceName || (std::strcmp(interfaceName, "INativeFixture") != 0 &&
            std::strcmp(interfaceName, IID_IDAComponent) != 0))
            return GR_INTERFACE_UNSUPPORTED;
        *instance = static_cast<IDAComponent *>(this);
        AddRef();
        return GR_OK;
    }

    U32 COMAPI AddRef() override
    {
        return ++references_;
    }

    U32 COMAPI Release() override
    {
        if (references_ > 0) --references_;
        const U32 remaining = references_;
        if (remaining == 0) delete this;
        return remaining;
    }

private:
    U32 references_ = 1;
};

class FixtureFactory final : public IComponentFactory
{
public:
    GENRESULT COMAPI QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!interfaceName || std::strcmp(interfaceName, IID_IComponentFactory) != 0)
            return GR_INTERFACE_UNSUPPORTED;
        *instance = static_cast<IComponentFactory *>(this);
        AddRef();
        return GR_OK;
    }

    U32 COMAPI AddRef() override
    {
        return ++references_;
    }

    U32 COMAPI Release() override
    {
        if (references_ > 1) --references_;
        return references_;
    }

    GENRESULT COMAPI CreateInstance(DACOMDESC * descriptor, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!descriptor || !descriptor->interface_name ||
            std::strcmp(descriptor->interface_name, "INativeFixture") != 0)
            return GR_INTERFACE_UNSUPPORTED;
        FixtureComponent * component = new FixtureComponent;
        *instance = static_cast<IDAComponent *>(component);
        return GR_OK;
    }

private:
    U32 references_ = 1;
};

FixtureFactory factory;
}

extern "C" int frontierx_fixture_value()
{
    return 73;
}

extern "C" GENRESULT FrontierXRegisterComponents(ICOManager * manager)
{
    if (!manager) return GR_INVALID_PARMS;
    if (failNextRegistration)
    {
        failNextRegistration = false;
        failedRegistrationObject = new FixtureComponent;
        return GR_GENERIC;
    }
    return manager->RegisterComponent(&factory, "INativeFixture", DACOM_NORMAL_PRIORITY);
}

extern "C" BOOL32 FrontierXCanUnloadComponents()
{
    return liveInstances == 0 ? TRUE : FALSE;
}

extern "C" void frontierx_fixture_fail_next_registration()
{
    failNextRegistration = true;
}

extern "C" void frontierx_fixture_release_failed_registration_object()
{
    if (!failedRegistrationObject) return;
    failedRegistrationObject->Release();
    failedRegistrationObject = nullptr;
}
