#include "FrontierX/DynamicLibrary.h"
#include "FrontierX/NativeComponentManager.h"
#include "EventSys.h"
#include "SYSTEM.H"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

struct ScratchDirectory
{
    std::filesystem::path path;

    ScratchDirectory()
    {
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        for (int attempt = 0; attempt < 100; ++attempt)
        {
            path = std::filesystem::temp_directory_path() /
                ("frontierx-module-config-" + std::to_string(stamp) + "-" +
                 std::to_string(attempt));
            if (std::filesystem::create_directory(path)) return;
        }
        throw std::runtime_error("unable to create module-config scratch directory");
    }

    ~ScratchDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(path, error);
    }
};
}

int main(int argc, char ** argv)
{
    try
    {
        Require(argc == 3, "expected the fixture and production event-module paths");
        std::string error;
        FrontierX::DynamicLibrary library;
        Require(!library.Open(nullptr, error) && !error.empty() && !library.IsOpen(),
            "empty library path did not fail transactionally");
        Require(!library.Open("/frontierx/no/such/library.so", error) && !error.empty() &&
            !library.IsOpen(), "missing library did not fail transactionally");
        Require(library.Symbol("frontierx_fixture_value", error) == nullptr && !error.empty(),
            "symbol lookup on a closed library did not fail");
        Require(library.Open(argv[1], error) && error.empty() && library.IsOpen(),
            "fixture library did not open");
        Require(!library.Open(argv[1], error) && !error.empty(),
            "second open silently replaced an owned library");

        void * symbol = library.Symbol("frontierx_fixture_value", error);
        Require(symbol && error.empty(), "fixture symbol lookup failed");
        using FixtureValue = int (*)();
        static_assert(sizeof(FixtureValue) == sizeof(symbol),
            "function and loader symbol pointers differ in size");
        FixtureValue fixtureValue = nullptr;
        std::memcpy(&fixtureValue, &symbol, sizeof(fixtureValue));
        Require(fixtureValue && fixtureValue() == 73,
            "loaded fixture symbol returned the wrong value");
        Require(library.Symbol("frontierx_missing_symbol", error) == nullptr && !error.empty() &&
            library.IsOpen(), "missing symbol lookup damaged library ownership");
        Require(library.Symbol(nullptr, error) == nullptr && !error.empty() && library.IsOpen(),
            "empty symbol name damaged library ownership");

        FrontierX::DynamicLibrary moved(std::move(library));
        Require(!library.IsOpen() && moved.IsOpen(), "move construction duplicated ownership");
        FrontierX::DynamicLibrary assigned;
        assigned = std::move(moved);
        Require(!moved.IsOpen() && assigned.IsOpen(), "move assignment duplicated ownership");
        Require(assigned.Close(error) && error.empty() && !assigned.IsOpen(),
            "explicit library close failed");
        Require(assigned.Close(error) && error.empty(), "repeated close was not idempotent");

        FrontierX::NativeComponentManager manager;
        Require(manager.AddLibrary(nullptr) == GR_INVALID_PARMS,
            "component manager accepted a null module path");
        Require(manager.AddLibrary("") == GR_INVALID_PARMS,
            "component manager accepted an empty module path");
        Require(manager.AddLibrary("/frontierx/no/such/module.so") == GR_GENERIC,
            "component manager accepted a missing module");
        Require(manager.AddLibrary(argv[1]) == GR_OK && manager.RegisteredCount() == 1,
            "native module did not register its factory");
        Require(manager.AddLibrary(argv[1]) == GR_OK && manager.RegisteredCount() == 1,
            "duplicate module load repeated registration");

        DACOMDESC descriptor("INativeFixture");
        IDAComponent * first = nullptr;
        Require(manager.CreateInstance(&descriptor, reinterpret_cast<void **>(&first)) == GR_OK &&
            first, "registered native factory did not create an instance");
        Require(manager.RemoveLibrary(argv[1]) == GR_OK && manager.RegisteredCount() == 1,
            "first balanced remove unloaded a multiply referenced module");
        Require(manager.RemoveLibrary(argv[1]) == GR_GENERIC && manager.RegisteredCount() == 1,
            "module unloaded while one of its objects was alive");
        first->Release();
        Require(manager.RemoveLibrary(argv[1]) == GR_OK && manager.RegisteredCount() == 0,
            "idle module did not unregister and unload");

        void * missing = reinterpret_cast<void *>(1);
        Require(manager.CreateInstance(&descriptor, &missing) == GR_INTERFACE_UNSUPPORTED &&
            missing == nullptr, "unloaded module factory remained available");
        Require(manager.RemoveLibrary(argv[1]) == GR_GENERIC,
            "component manager removed an unknown module");

        Require(manager.AddLibrary(argv[1]) == GR_OK && manager.RegisteredCount() == 1,
            "native module did not reload after a complete unload");
        IDAComponent * second = nullptr;
        Require(manager.CreateInstance(&descriptor, reinterpret_cast<void **>(&second)) == GR_OK &&
            second, "reloaded native factory did not create an instance");
        Require(manager.ShutDown() == GR_GENERIC && manager.RegisteredCount() == 1,
            "shutdown unloaded a module with a live object");
        second->Release();
        Require(manager.ShutDown() == GR_OK && manager.RegisteredCount() == 0,
            "shutdown did not release registrations before unloading modules");

        const std::string inlineConfiguration =
            std::string("[Libraries]\n\"") + argv[1] + "\"\n";
        FrontierX::NativeComponentManager inlineManager;
        Require(inlineManager.SetINIConfig(inlineConfiguration.c_str(), DACOM_INI_STRING) ==
            GR_OK && inlineManager.RegisteredCount() == 1,
            "inline library configuration did not load the fixture module");
        Require(inlineManager.SetINIConfig(inlineConfiguration.c_str(), DACOM_INI_STRING) ==
            GR_OK && inlineManager.RegisteredCount() == 1,
            "repeated library configuration registered the module twice");
        Require(inlineManager.ShutDown() == GR_OK,
            "inline-configured module did not shut down");
        Require(inlineManager.SetINIConfig(inlineConfiguration.c_str(), DACOM_INI_STRING) ==
            GR_OK && inlineManager.RegisteredCount() == 1,
            "manager could not configure again after a complete shutdown");
        Require(inlineManager.ShutDown() == GR_OK,
            "reconfigured module did not shut down");

        FrontierX::NativeComponentManager removalManager;
        Require(removalManager.AddLibrary(argv[1]) == GR_OK,
            "removal-config manager did not load the fixture");
        const std::string removalConfiguration =
            std::string("[Libraries]\n- \"") + argv[1] + "\"\n";
        Require(removalManager.SetINIConfig(removalConfiguration.c_str(), DACOM_INI_STRING) ==
            GR_OK && removalManager.RegisteredCount() == 0,
            "configured removal did not unload the named module");

        ScratchDirectory scratch;
        const std::filesystem::path copiedLibrary = scratch.path / "fixture module.so";
        std::filesystem::copy_file(std::filesystem::absolute(argv[1]), copiedLibrary);
        const std::filesystem::path configPath = scratch.path / "components.ini";
        {
            std::ofstream config(configPath, std::ios::binary);
            config << "[Libraries]\n\"fixture module.so\"\n";
            Require(bool(config), "unable to write module configuration fixture");
        }
        FrontierX::NativeComponentManager fileManager;
        const std::string configName = configPath.string();
        Require(fileManager.SetINIConfig(configName.c_str()) == GR_OK &&
            fileManager.RegisteredCount() == 1,
            "file configuration did not resolve a module relative to its directory");
        IDAComponent * configured = nullptr;
        Require(fileManager.CreateInstance(&descriptor,
            reinterpret_cast<void **>(&configured)) == GR_OK && configured,
            "file-configured factory did not create an instance");
        configured->Release();
        Require(fileManager.ShutDown() == GR_OK,
            "file-configured module did not shut down");

        FrontierX::NativeComponentManager retryManager;
        Require(retryManager.SetINIConfig("[Libraries]\n\"unterminated\n",
            DACOM_INI_STRING) == GR_INVALID_PARMS,
            "unterminated configured filename was accepted");
        const std::string failingConfiguration = inlineConfiguration +
            "/frontierx/no/such/configured-module.so\n";
        Require(retryManager.SetINIConfig(failingConfiguration.c_str(), DACOM_INI_STRING) ==
            GR_GENERIC && retryManager.RegisteredCount() == 0,
            "failed multi-module configuration did not roll back earlier loads");
        Require(retryManager.SetINIConfig(inlineConfiguration.c_str(), DACOM_INI_STRING) ==
            GR_OK && retryManager.RegisteredCount() == 1,
            "failed configuration prevented a valid retry");
        Require(retryManager.ShutDown() == GR_OK,
            "retried module configuration did not shut down");

        const std::string productionConfiguration =
            std::string("[Libraries]\n\"") + argv[2] + "\"\n";
        FrontierX::NativeComponentManager productionManager;
        Require(productionManager.SetINIConfig(productionConfiguration.c_str(),
            DACOM_INI_STRING) == GR_OK && productionManager.RegisteredCount() == 1,
            "production event-system ELF module did not configure");
        AGGDESC eventDescriptor("IEventSystem", "EventSystem");
        void * rawEventAggregate = nullptr;
        Require(productionManager.CreateInstance(&eventDescriptor, &rawEventAggregate) == GR_OK &&
            rawEventAggregate, "production ELF factory did not create the event aggregate");
        auto * eventAggregate = static_cast<IAggregateComponent *>(rawEventAggregate);
        IEventSystem * dynamicEventSystem = nullptr;
        ISystemComponent * dynamicSystem = nullptr;
        Require(eventAggregate->QueryInterface(IID_IEventSystem,
            reinterpret_cast<void **>(&dynamicEventSystem)) == GR_OK && dynamicEventSystem &&
            eventAggregate->QueryInterface(IID_ISystemComponent,
            reinterpret_cast<void **>(&dynamicSystem)) == GR_OK && dynamicSystem,
            "production ELF event interfaces were unavailable");
        Require(eventAggregate->Initialize() == GR_OK &&
            dynamicEventSystem->Initialize(0) == GR_OK,
            "production ELF event aggregate did not initialize");
        Require(productionManager.RemoveLibrary(argv[2]) == GR_GENERIC,
            "production ELF module unloaded while its event instance was alive");
        Require(dynamicEventSystem->Post(91, reinterpret_cast<void *>(0x91)) == GR_OK,
            "production ELF event instance did not accept a message");
        dynamicSystem->Update();
        U32 dynamicMessage = 0;
        void * dynamicParameter = nullptr;
        Require(dynamicEventSystem->Peek(0, &dynamicMessage, &dynamicParameter) == GR_OK &&
            dynamicMessage == 91 && dynamicParameter == reinterpret_cast<void *>(0x91),
            "production ELF event instance did not execute its queue");
        dynamicSystem->Release();
        dynamicEventSystem->Release();
        eventAggregate->Release();
        Require(productionManager.RemoveLibrary(argv[2]) == GR_OK &&
            productionManager.RegisteredCount() == 0,
            "production event-system ELF module did not unload after final release");

        IDAComponent * escaped = nullptr;
        {
            FrontierX::NativeComponentManager scopedManager;
            Require(scopedManager.AddLibrary(argv[1]) == GR_OK,
                "scoped manager did not load the fixture module");
            Require(scopedManager.CreateInstance(&descriptor,
                reinterpret_cast<void **>(&escaped)) == GR_OK && escaped,
                "scoped manager did not create the escaping fixture object");
        }
        Require(escaped->Release() == 0,
            "manager destruction unmapped a module with a live object");

        FrontierX::DynamicLibrary failureControl;
        Require(failureControl.Open(argv[1], error),
            "unable to open fixture failure-control handle");
        void * failSymbol = failureControl.Symbol(
            "frontierx_fixture_fail_next_registration", error);
        void * releaseSymbol = failureControl.Symbol(
            "frontierx_fixture_release_failed_registration_object", error);
        using FixtureControl = void (*)();
        static_assert(sizeof(FixtureControl) == sizeof(failSymbol),
            "fixture control pointer has an incompatible representation");
        FixtureControl failRegistration = nullptr;
        FixtureControl releaseFailedObject = nullptr;
        std::memcpy(&failRegistration, &failSymbol, sizeof(failRegistration));
        std::memcpy(&releaseFailedObject, &releaseSymbol, sizeof(releaseFailedObject));
        Require(failRegistration && releaseFailedObject,
            "fixture failure-control symbols did not resolve");
        failRegistration();
        {
            FrontierX::NativeComponentManager failedManager;
            Require(failedManager.AddLibrary(argv[1]) == GR_GENERIC &&
                failedManager.RegisteredCount() == 0,
                "failed registration with a live object was accepted");
        }
        releaseFailedObject();
        Require(failureControl.Close(error),
            "fixture failure-control handle did not close");

        std::cout << "native dynamic-library ownership, registration and safe unload passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "dynamic-library probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
