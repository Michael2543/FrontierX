#include "FrontierX/DynamicLibrary.h"
#include "FrontierX/NativeComponentManager.h"

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
        Require(argc == 2, "expected the fixture library path");
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
        Require(retryManager.SetINIConfig(inlineConfiguration.c_str(), DACOM_INI_STRING) ==
            GR_OK && retryManager.RegisteredCount() == 1,
            "failed configuration prevented a valid retry");
        Require(retryManager.ShutDown() == GR_OK,
            "retried module configuration did not shut down");

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

        std::cout << "native dynamic-library ownership, registration and safe unload passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "dynamic-library probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
