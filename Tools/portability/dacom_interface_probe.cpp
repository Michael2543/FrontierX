#include "TComponent.h"
#include "TConnContainer.h"
#include "TConnPoint.h"
#include "IProfileParser.h"
#include "UserDefaults.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

struct ProbeComponent final : IDAComponent
{
    GENRESULT QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interfaceName && std::strcmp(interfaceName, IID_IDAComponent) == 0)
        {
            *instance = this;
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override
    {
        return ++references;
    }

    U32 Release() override
    {
        return references ? --references : 0;
    }

    U32 references = 0;
};

struct ProbeInterface : IDAComponent
{
    virtual U32 Value() = 0;
};

struct ProbeOwner : ProbeInterface
{
    BEGIN_DACOM_MAP_INBOUND(ProbeOwner)
    DACOM_INTERFACE_ENTRY(ProbeInterface)
    END_DACOM_MAP()

    U32 Value() override
    {
        return 42;
    }
};

struct ProbeSink : IDAComponent
{
    virtual U32 Marker() = 0;
};

struct ProbeSinkClient final : ProbeSink
{
    GENRESULT QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interfaceName && std::strcmp(interfaceName, "ProbeSink") == 0)
        {
            *instance = this;
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }
    U32 Marker() override { return marker; }

    U32 references = 0;
    U32 marker = 7;
};

struct ConnectionOwner final : ConnectionPointContainer<ConnectionOwner>
{
    BEGIN_DACOM_MAP_OUTBOUND(ConnectionOwner)
    DACOM_INTERFACE_ENTRY_AGGREGATE("ProbeSink", point)
    END_DACOM_MAP()

    ConnectionOwner() : point(0) {}

    GENRESULT QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interfaceName && std::strcmp(interfaceName,
            IID_IDAConnectionPointContainer) == 0)
        {
            *instance = static_cast<IDAConnectionPointContainer *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }

    ConnectionPoint<ConnectionOwner, ProbeSink> point;
    U32 references = 0;
};

BOOL32 COMAPI CountConnection(IDAConnectionPoint *, IDAComponent * client, void * context)
{
    auto * count = static_cast<U32 *>(context);
    if (client) ++*count;
    return TRUE;
}

struct ProbeProfileCallback final : IProfileCallback
{
    BOOL32 COMAPI ProfileCallback(IProfileParser2 *, const C8 * name,
                                  const C8 * value, void * context) override
    {
        auto * count = static_cast<U32 *>(context);
        if (name && value) ++*count;
        return TRUE;
    }
};
}

int main()
{
    static_assert(sizeof(U32) == 4, "DACOM U32 must remain fixed-width");
    static_assert(sizeof(S32) == 4, "DACOM S32 must remain fixed-width");
    static_assert(std::has_virtual_destructor<IDAComponent>::value == false,
        "IDAComponent ABI unexpectedly gained a virtual destructor");
    static_assert(std::is_base_of<IDAComponent, IUserDefaults>::value,
        "IUserDefaults no longer follows the DACOM component boundary");

    try
    {
        ProbeComponent component;
        void * instance = nullptr;
        Require(component.QueryInterface(IID_IDAComponent, &instance) == GR_OK,
            "IDAComponent query failed");
        Require(instance == &component && component.references == 1,
            "IDAComponent query changed pointer or reference behavior");
        Require(component.Release() == 0, "IDAComponent release count changed");
        Require(component.QueryInterface("missing", &instance) == GR_INTERFACE_UNSUPPORTED &&
            instance == nullptr, "unsupported interface query changed failure semantics");

        DAComponent<ProbeOwner> mappedComponent;
        ProbeInterface * mappedInterface = nullptr;
        Require(mappedComponent.QueryInterface("ProbeInterface",
            reinterpret_cast<void **>(&mappedInterface)) == GR_OK,
            "DAComponent interface-map query failed");
        Require(mappedInterface == static_cast<ProbeInterface *>(&mappedComponent) &&
            mappedInterface->Value() == 42,
            "DAComponent map changed interface adjustment or dispatch");
        Require(mappedInterface->Release() == 1,
            "DAComponent mapped query changed reference behavior");

        ConnectionOwner connectionOwner;
        IDAConnectionPoint * point = nullptr;
        Require(connectionOwner.FindConnectionPoint("ProbeSink", &point) == GR_OK &&
            point == &connectionOwner.point,
            "connection container did not recover its member point");
        point->Release();

        IDAConnectionPointContainer * container = nullptr;
        Require(point->GetContainer(&container) == GR_OK &&
            container == static_cast<IDAConnectionPointContainer *>(&connectionOwner),
            "connection point did not recover its owner container");
        container->Release();

        ProbeSinkClient firstClient;
        ProbeSinkClient secondClient;
        U32 firstToken = 0;
        U32 secondToken = 0;
        Require(point->Advise(&firstClient, &firstToken) == GR_OK && firstToken == 1,
            "first connection did not receive token one");
        Require(point->Advise(&secondClient, &secondToken) == GR_OK && secondToken == 2,
            "second connection did not receive token two");
        U32 connectionCount = 0;
        Require(point->EnumerateConnections(CountConnection, &connectionCount) &&
            connectionCount == 2, "connection enumeration changed subscriber order/count");
        Require(point->Unadvise(firstToken) == GR_OK,
            "connection token did not remove its subscriber");
        U32 reusedToken = 0;
        Require(point->Advise(&firstClient, &reusedToken) == GR_OK && reusedToken == 1,
            "connection token did not deterministically reuse its lowest gap");
        Require(point->Unadvise(secondToken) == GR_OK &&
            point->Unadvise(reusedToken) == GR_OK,
            "connection cleanup failed");

        ProbeProfileCallback profileCallback;
        U32 callbackCount = 0;
        Require(profileCallback.ProfileCallback(nullptr, "name", "value", &callbackCount) &&
            callbackCount == 1, "profile callback calling/context contract changed");

        std::cout << "portable DACOM interface, connection point, and profile callback passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "DACOM interface probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
