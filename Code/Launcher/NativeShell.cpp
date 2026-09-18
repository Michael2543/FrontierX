#include "EventSys.h"
#include "FrontierX/DataRoot.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/PlatformTime.h"
#include "FrontierX/SdlEventPump.h"
#include "FrontierX/SdlWindow.h"
#include "FrontierX/StartupDataRoot.h"

#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
template <class Interface>
class InterfaceOwner
{
public:
    InterfaceOwner() = default;
    ~InterfaceOwner() { if (value_) value_->Release(); }
    InterfaceOwner(const InterfaceOwner &) = delete;
    InterfaceOwner & operator=(const InterfaceOwner &) = delete;

    Interface ** Receive() { return &value_; }
    Interface * Get() const { return value_; }

private:
    Interface * value_ = nullptr;
};

struct Options
{
    std::filesystem::path configPath;
    std::filesystem::path dataRoot;
    std::uint32_t smokeMilliseconds = 0;
    bool hidden = false;
};

std::uint32_t ParseDuration(const char * text)
{
    std::uint32_t value = 0;
    const std::string input(text ? text : "");
    const auto result = std::from_chars(input.data(), input.data() + input.size(), value);
    if (input.empty() || result.ec != std::errc{} || result.ptr != input.data() + input.size() ||
        value == 0 || value > 60000)
        throw std::invalid_argument("--smoke-ms requires an integer from 1 through 60000");
    return value;
}

Options ParseOptions(int argc, char ** argv)
{
    Options options;
    for (int index = 1; index < argc; ++index)
    {
        const std::string option(argv[index]);
        if (option == "--help" && argc == 2)
        {
            std::cout << "Usage: frontierx_native_shell [--config FILE] [--data-root DIRECTORY] "
                         "[--smoke-ms MILLISECONDS [--hidden]]\n"
                         "Opens the native startup shell; it does not start or render the game.\n";
            std::exit(0);
        }
        if (option == "--hidden")
        {
            if (options.hidden) throw std::invalid_argument("duplicate option: --hidden");
            options.hidden = true;
            continue;
        }
        if ((option != "--config" && option != "--data-root" && option != "--smoke-ms") ||
            index + 1 == argc)
            throw std::invalid_argument("expected --config, --data-root, --smoke-ms or --hidden");
        const char * value = argv[++index];
        if (!value[0] || std::string(value).rfind("--", 0) == 0)
            throw std::invalid_argument("missing option value");
        if (option == "--config")
        {
            if (!options.configPath.empty())
                throw std::invalid_argument("duplicate option: --config");
            options.configPath = value;
        }
        else if (option == "--data-root")
        {
            if (!options.dataRoot.empty())
                throw std::invalid_argument("duplicate option: --data-root");
            options.dataRoot = value;
        }
        else
        {
            if (options.smokeMilliseconds)
                throw std::invalid_argument("duplicate option: --smoke-ms");
            options.smokeMilliseconds = ParseDuration(value);
        }
    }
    if (options.hidden && !options.smokeMilliseconds)
        throw std::invalid_argument("--hidden requires --smoke-ms");
    return options;
}

int Run(int argc, char ** argv)
{
    const Options options = ParseOptions(argc, argv);
    std::filesystem::path configuredRoot;
    if (!options.configPath.empty())
    {
        std::string configurationError;
        if (!FrontierX::LoadConfiguredDataRoot(
            options.configPath, configuredRoot, configurationError))
        {
            std::cerr << "Configuration error: " << configurationError << '\n';
            return 2;
        }
    }
    const char * environment = std::getenv("FRONTIERX_DATA_ROOT");
    const std::filesystem::path environmentRoot = environment ? environment : "";
    const auto startupRoot = FrontierX::ResolveStartupDataRoot(
        options.dataRoot, environmentRoot, configuredRoot);
    if (!startupRoot.directoryAccessible)
    {
        std::cerr << "Data root is not an accessible directory; set --data-root or "
                     "FRONTIERX_DATA_ROOT\n";
        return 2;
    }
    if (!FrontierX::Platform::IsDataRootUsable(startupRoot.entries))
    {
        for (const auto & entry : startupRoot.entries)
            if (entry.required &&
                entry.status != FrontierX::Platform::DataRootEntryStatus::Present)
                std::cerr << entry.expectedPath.generic_string() << ": "
                          << FrontierX::Platform::DataRootEntryStatusName(entry.status) << '\n';
        std::cerr << "Required data directories are missing or have incompatible names/types\n";
        return 2;
    }
    std::cout << "Data root source: "
              << FrontierX::StartupDataRootSourceName(startupRoot.source)
              << "; required directories passed\n";

    FrontierX::NativeComponentManager manager;
    RegisterTheEventSystem(&manager);
    AGGDESC descriptor("IEventSystem", "EventSystem");
    InterfaceOwner<IAggregateComponent> aggregate;
    if (manager.CreateInstance(&descriptor,
        reinterpret_cast<void **>(aggregate.Receive())) != GR_OK || !aggregate.Get())
    {
        std::cerr << "Native event-system creation failed\n";
        return 3;
    }
    InterfaceOwner<IEventSystem> eventSystem;
    if (aggregate.Get()->QueryInterface(IID_IEventSystem,
        reinterpret_cast<void **>(eventSystem.Receive())) != GR_OK || !eventSystem.Get() ||
        aggregate.Get()->Initialize() != GR_OK || eventSystem.Get()->Initialize(0) != GR_OK)
    {
        std::cerr << "Native event-system initialization failed\n";
        return 3;
    }

    FrontierX::SdlWindow window;
    std::string error;
    if (!window.Open("FrontierX native shell (game not started)", 800, 600,
        options.hidden, error) || !window.StartTextInput(error))
    {
        std::cerr << "Native window startup failed: " << error << '\n';
        return 4;
    }
    const std::string driver = window.VideoDriverName();
    if (driver.empty())
    {
        std::cerr << "Native window startup failed: SDL reported no video driver\n";
        return 4;
    }
    std::cout << "Native shell opened; video driver: " << driver << '\n';

    FrontierX::SdlEventPump pump(window, *eventSystem.Get());
    const std::uint32_t started = FrontierX::Platform::MonotonicMilliseconds();
    bool closeRequested = false;
    while (!closeRequested)
    {
        const auto batch = pump.Pump();
        closeRequested = batch.close_requested || batch.quit_requested;
        if (!closeRequested && options.smokeMilliseconds)
        {
            const std::uint32_t elapsed =
                FrontierX::Platform::MonotonicMilliseconds() - started;
            closeRequested = elapsed >= options.smokeMilliseconds;
        }
        if (!closeRequested) FrontierX::Platform::SleepForMilliseconds(1);
    }
    window.Close();
    std::cout << "Native shell closed cleanly; game startup and rendering were not attempted\n";
    return 0;
}
}

int main(int argc, char ** argv)
{
    try
    {
        return Run(argc, argv);
    }
    catch (const std::invalid_argument & error)
    {
        std::cerr << "Usage error: " << error.what() << '\n';
        return 1;
    }
    catch (const std::exception & error)
    {
        std::cerr << "Native shell failed: " << error.what() << '\n';
        return 4;
    }
}
