#include "EventSys.h"
#include "FrontierX/NativeComponentManager.h"
#include "IConnection.h"
#include "SYSTEM.H"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

struct RejectingFactory final : IComponentFactory
{
    GENRESULT QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interfaceName && std::strcmp(interfaceName, IID_IComponentFactory) == 0)
        {
            *instance = static_cast<IComponentFactory *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }
    GENRESULT CreateInstance(DACOMDESC *, void ** instance) override
    {
        if (instance) *instance = nullptr;
        ++createCalls;
        return GR_GENERIC;
    }

    U32 references = 0;
    U32 createCalls = 0;
};

struct Enumeration
{
    std::vector<IComponentFactory *> factories;
    std::vector<U32> priorities;
    bool stopAfterFirst = false;
};

BOOL32 DACOM_CDECL CaptureRegistration(IComponentFactory * factory, const C8 *,
                                       U32 priority, void * context)
{
    auto & enumeration = *static_cast<Enumeration *>(context);
    enumeration.factories.push_back(factory);
    enumeration.priorities.push_back(priority);
    return enumeration.stopAfterFirst && enumeration.factories.size() == 1 ? FALSE : TRUE;
}

struct ProbeCallback final : IEventCallback
{
    GENRESULT QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interfaceName && (std::strcmp(interfaceName, "IEventCallback") == 0 ||
            std::strcmp(interfaceName, IID_IEventCallback) == 0))
        {
            *instance = static_cast<IEventCallback *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }

    GENRESULT Notify(U32 message, void * parameter) override
    {
        messages.push_back(message);
        parameters.push_back(parameter);
        if (message == 10 && eventSystem)
            return eventSystem->Post(11, reinterpret_cast<void *>(0x11));
        return GR_OK;
    }

    IEventSystem * eventSystem = nullptr;
    std::vector<U32> messages;
    std::vector<void *> parameters;
    U32 references = 0;
};
}

int main()
{
    try
    {
        FrontierX::NativeComponentManager manager;
        RejectingFactory rejectingFactory;
        RejectingFactory equalPriorityFactory;
        Require(manager.RegisterComponent(&rejectingFactory, "IEventSystem",
            DACOM_HIGH_PRIORITY) == GR_OK && rejectingFactory.references == 1,
            "high-priority rejecting factory registration failed");
        RegisterTheEventSystem(&manager);
        Require(manager.RegisterComponent(&equalPriorityFactory, "IEventSystem",
            DACOM_NORMAL_PRIORITY) == GR_OK && equalPriorityFactory.references == 1,
            "equal-priority rejecting factory registration failed");
        Require(manager.RegisteredCount() == 3,
            "event-system factory was not registered with the native manager");

        Enumeration enumeration;
        Require(manager.EnumerateComponents("IEventSystem", CaptureRegistration,
            &enumeration) == GR_OK && enumeration.factories.size() == 3 &&
            enumeration.factories[0] == &rejectingFactory &&
            enumeration.factories[1] == &equalPriorityFactory &&
            enumeration.priorities[0] == DACOM_HIGH_PRIORITY &&
            enumeration.priorities[1] == DACOM_NORMAL_PRIORITY &&
            enumeration.priorities[2] == DACOM_NORMAL_PRIORITY,
            "component priority ordering or enumeration changed");
        Enumeration stoppedEnumeration;
        stoppedEnumeration.stopAfterFirst = true;
        Require(manager.EnumerateComponents(nullptr, CaptureRegistration,
            &stoppedEnumeration) == GR_OK && stoppedEnumeration.factories.size() == 1 &&
            stoppedEnumeration.factories[0] == &rejectingFactory,
            "component enumeration did not stop when its callback returned false");

        AGGDESC descriptor("IEventSystem", "EventSystem");
        void * rawAggregate = nullptr;
        Require(manager.CreateInstance(&descriptor, &rawAggregate) == GR_OK && rawAggregate,
            "event-system aggregate creation failed");
        Require(rejectingFactory.createCalls == 1 && equalPriorityFactory.createCalls == 1,
            "manager did not continue after higher/equal-priority factories rejected creation");
        auto * aggregate = static_cast<IAggregateComponent *>(rawAggregate);

        IEventSystem * eventSystem = nullptr;
        ISystemComponent * system = nullptr;
        Require(aggregate->QueryInterface(IID_IEventSystem,
            reinterpret_cast<void **>(&eventSystem)) == GR_OK && eventSystem,
            "IEventSystem query failed");
        Require(aggregate->QueryInterface(IID_ISystemComponent,
            reinterpret_cast<void **>(&system)) == GR_OK && system,
            "ISystemComponent query failed");
        Require(aggregate->Initialize() == GR_OK, "aggregate initialization failed");
        Require(eventSystem->Initialize(0) == GR_OK &&
            eventSystem->Initialize(DAEVENTFLAG_MULTITHREADED) == GR_GENERIC,
            "event-system initialization flags changed");

        IDAConnectionPoint * point = nullptr;
        Require(eventSystem->QueryOutgoingInterface("IEventCallback", &point) == GR_OK && point,
            "event callback connection point query failed");
        ProbeCallback callback;
        callback.eventSystem = eventSystem;
        U32 token = 0;
        Require(point->Advise(&callback, &token) == GR_OK && token == 1,
            "event callback subscription failed");

        void * firstParameter = reinterpret_cast<void *>(0x10);
        Require(eventSystem->Post(10, firstParameter) == GR_OK,
            "event post failed");
        system->Update();
        Require(callback.messages.size() == 2 && callback.messages[0] == 10 &&
            callback.messages[1] == 11 && callback.parameters[0] == firstParameter &&
            callback.parameters[1] == reinterpret_cast<void *>(0x11),
            "callback-time post did not run later in the same update");

        U32 message = 0;
        void * parameter = nullptr;
        Require(eventSystem->Peek(0, &message, &parameter) == GR_OK &&
            message == 10 && parameter == firstParameter,
            "first delivered event was not retained for Peek");
        Require(eventSystem->Peek(1, &message, &parameter) == GR_OK &&
            message == 11 && parameter == reinterpret_cast<void *>(0x11),
            "callback-posted event was not retained for Peek");
        Require(eventSystem->Peek(2, &message, &parameter) == GR_GENERIC,
            "Peek did not stop at the delivered batch boundary");

        Require(eventSystem->Send(12, reinterpret_cast<void *>(0x12)) == GR_OK &&
            callback.messages.size() == 3 && callback.messages.back() == 12,
            "synchronous event send failed");
        system->Update();
        Require(eventSystem->Peek(0, &message, &parameter) == GR_GENERIC,
            "empty update did not retire the previous delivered batch");

        Require(point->Unadvise(token) == GR_OK, "event callback unsubscribe failed");
        point->Release();
        system->Release();
        eventSystem->Release();
        aggregate->Release();

        Require(manager.UnregisterComponent(&equalPriorityFactory, "IEventSystem") == GR_OK &&
            equalPriorityFactory.references == 0 &&
            manager.UnregisterComponent(&rejectingFactory, "IEventSystem") == GR_OK &&
            rejectingFactory.references == 0 && manager.RegisteredCount() == 1,
            "native manager unregistration or reference release failed");
        Require(manager.AddLibrary(nullptr) == GR_INVALID_PARMS &&
            manager.AddLibrary("/frontierx/no/such/module.so") == GR_GENERIC &&
            manager.RemoveLibrary("/frontierx/no/such/module.so") == GR_GENERIC &&
            manager.SetINIConfig("config", DACOM_INI_STRING | DACOM_INI_WRITABLE) ==
                GR_INVALID_PARMS &&
            manager.SetINIConfig("config", 0) == GR_INVALID_PARMS &&
            manager.SetINIConfig("[Other]\nValue=1\n", DACOM_INI_STRING) ==
                GR_DATA_NOT_FOUND &&
            manager.SetINIConfig("config", DACOM_INI_WRITABLE) == GR_NOT_IMPLEMENTED,
            "native manager loader/configuration failure contracts changed");
        Require(manager.ShutDown() == GR_OK && manager.RegisteredCount() == 0,
            "native manager shutdown did not release all registrations");

        std::cout << "native DACOM manager and production event-system owner passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "event-system probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
