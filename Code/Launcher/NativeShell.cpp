#include "EventSys.h"
#include "DACOM.H"
#include "HeapObj.h"
#include "HKEvent.h"
#include "IConnection.h"
#include "FrontierX/DataRoot.h"
#include "FrontierX/LegacyInput.h"
#include "FrontierX/NativeUserDefaults.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/NativeWindowManager.h"
#include "FrontierX/PlatformTime.h"
#include "FrontierX/SdlWindow.h"
#include "FrontierX/StartupDataRoot.h"
#include "FrontierX/StartupPreferences.h"
#include "SYSTEM.H"
#include "UserDefaults.h"
#include "WindowManager.h"

#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
#include "CQImage.h"
#include "DEffectOpts.h"
#include "FDump.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/RenderSmokeScene.h"
#include "FrontierX/SdlGpuRenderer.h"
#include "GlobalFlagTypes.h"
#include "IRenderPrimitive.h"
#include "IVertexBufferManager.h"
#include "RenderBatcherRegistration.h"
#endif

#include <charconv>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
GlobalFlags CQFLAGS = {};
GlobalRenderFlags CQRENDERFLAGS = {};
GlobalEffectsOptions CQEFFECTS = {};
IRenderPipeline *PIPE = nullptr;
IVertexBufferManager *VB_MANAGER = nullptr;

namespace
{
int DACOM_CDECL ShellRenderDiagnostic(ErrorCode, const C8 * format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    const int result = std::vfprintf(stderr, format, arguments);
    va_end(arguments);
    return result;
}
}

extern "C"
{
DA_ERROR_HANDLER FDUMP = ShellRenderDiagnostic;
}

bool ICQImage::Assert(const char * expression, void * file, unsigned line)
{
    std::fprintf(stderr, "Native render assertion: %s (%s:%u)\n",
        expression, static_cast<const char *>(file), line);
    std::abort();
}

bool DACOM_CDECL ICQImage::Bomb(const char * expression, ...)
{
    std::fputs("Native render fatal diagnostic: ", stderr);
    va_list arguments;
    va_start(arguments, expression);
    std::vfprintf(stderr, expression, arguments);
    va_end(arguments);
    std::fputc('\n', stderr);
    std::abort();
}
#endif

namespace
{
IEventSystem * shellEventSystem = nullptr;
bool shellCloseRequested = false;

void DACOM_CDECL ShellExit(int)
{
    shellCloseRequested = true;
}

LRESULT COMAPI ShellWindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == FrontierX::LegacyMessage::Close) shellCloseRequested = true;
    if (!shellEventSystem) return 0;

    if (message == FrontierX::LegacyMessage::Character)
        shellEventSystem->Send(FrontierX::LegacyMessage::UnicodeCharacter,
            reinterpret_cast<void *>(wparam & 0xffffU));
    FrontierX::LegacyWindowMessage record{
        static_cast<FrontierX::LegacyWindowToken>(window), message, wparam, lparam};
    shellEventSystem->Send(message, &record);
    return 0;
}

template <class Interface>
class InterfaceOwner
{
public:
    InterfaceOwner() = default;
    ~InterfaceOwner() { Reset(); }
    InterfaceOwner(const InterfaceOwner &) = delete;
    InterfaceOwner & operator=(const InterfaceOwner &) = delete;

    Interface ** Receive() { return &value_; }
    Interface * Get() const { return value_; }
    void Reset()
    {
        if (value_) value_->Release();
        value_ = nullptr;
    }

private:
    Interface * value_ = nullptr;
};

class ConnectionOwner
{
public:
    ~ConnectionOwner() { Disconnect(); }
    ConnectionOwner(const ConnectionOwner &) = delete;
    ConnectionOwner & operator=(const ConnectionOwner &) = delete;
    ConnectionOwner() = default;

    bool Connect(IDAConnectionPoint * point, IDAComponent * component)
    {
        if (!point || !component || point_) return false;
        U32 token = 0;
        if (point->Advise(component, &token) != GR_OK || token == 0) return false;
        point_ = point;
        token_ = token;
        return true;
    }

    void Disconnect()
    {
        if (point_ && token_) point_->Unadvise(token_);
        point_ = nullptr;
        token_ = 0;
    }

private:
    IDAConnectionPoint * point_ = nullptr;
    U32 token_ = 0;
};

struct Options
{
    std::filesystem::path configPath;
    std::filesystem::path dataRoot;
    std::filesystem::path preferencesPath;
    std::filesystem::path hotkeysPath;
    std::filesystem::path vertexShaderPath;
    std::filesystem::path fragmentShaderPath;
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
            std::cout << "Usage: frontierx_native_shell [--config FILE] [--preferences FILE] "
                         "[--data-root DIRECTORY] [--hotkeys FILE] "
                         "[--render-smoke VERTEX_SPV FRAGMENT_SPV] "
                         "[--smoke-ms MILLISECONDS [--hidden]]\n"
                         "Opens the native startup shell; the optional render smoke draws a "
                         "repository fixture, not the game.\n";
            std::exit(0);
        }
        if (option == "--hidden")
        {
            if (options.hidden) throw std::invalid_argument("duplicate option: --hidden");
            options.hidden = true;
            continue;
        }
        if (option == "--render-smoke")
        {
            if (!options.vertexShaderPath.empty() || !options.fragmentShaderPath.empty())
                throw std::invalid_argument("duplicate option: --render-smoke");
            if (index + 2 >= argc || !argv[index + 1][0] || !argv[index + 2][0] ||
                std::string(argv[index + 1]).rfind("--", 0) == 0 ||
                std::string(argv[index + 2]).rfind("--", 0) == 0)
                throw std::invalid_argument(
                    "--render-smoke requires vertex and fragment SPIR-V files");
            options.vertexShaderPath = argv[++index];
            options.fragmentShaderPath = argv[++index];
            continue;
        }
        if ((option != "--config" && option != "--preferences" && option != "--hotkeys" &&
             option != "--data-root" && option != "--smoke-ms") ||
            index + 1 == argc)
            throw std::invalid_argument(
                "expected --config, --preferences, --data-root, --hotkeys, --smoke-ms or --hidden");
        const char * value = argv[++index];
        if (!value[0] || std::string(value).rfind("--", 0) == 0)
            throw std::invalid_argument("missing option value");
        if (option == "--config")
        {
            if (!options.configPath.empty())
                throw std::invalid_argument("duplicate option: --config");
            options.configPath = value;
        }
        else if (option == "--preferences")
        {
            if (!options.preferencesPath.empty())
                throw std::invalid_argument("duplicate option: --preferences");
            options.preferencesPath = value;
        }
        else if (option == "--hotkeys")
        {
            if (!options.hotkeysPath.empty())
                throw std::invalid_argument("duplicate option: --hotkeys");
            options.hotkeysPath = value;
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

    const char * xdgConfigEnvironment = std::getenv("XDG_CONFIG_HOME");
    const char * homeEnvironment = std::getenv("HOME");
    FrontierX::StartupPreferences preferences;
    std::string preferencesError;
    if (!FrontierX::LoadStartupPreferences(
        options.preferencesPath,
        xdgConfigEnvironment ? xdgConfigEnvironment : "",
        homeEnvironment ? homeEnvironment : "",
        preferences, &preferencesError))
    {
        std::cerr << "Preferences error: " << preferencesError << '\n';
        return 5;
    }
    std::cout << "Preferences source: "
              << FrontierX::SettingsPathSourceName(preferences.selection.source)
              << "; status: "
              << FrontierX::PreferenceLoadResultName(preferences.storeResult)
              << "; entries: " << preferences.store.Size()
              << "; user-defaults: "
              << FrontierX::UserSettingsLoadResultName(preferences.userSettingsResult)
              << "; settings-source: "
              << FrontierX::NativeUserSettingsSourceName(
                     preferences.userSettings.Source()) << '\n';
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

    ICOManager * manager = DACOM_Acquire();
    if (!manager)
    {
        std::cerr << "Native component-manager acquisition failed\n";
        return 3;
    }
    if (InitializeDAHeap(0x10000, 0x4000,
            DAHEAPFLAG_MULTITHREADED | DAHEAPFLAG_GROWHEAP |
            DAHEAPFLAG_NOMSGS) == FALSE)
    {
        std::cerr << "Native shared-heap initialization failed\n";
        return 3;
    }
    RegisterTheEventSystem(manager);
    RegisterHotkeyEventServer(manager);
    FrontierX::RegisterNativeUserDefaults(
        manager, preferences.selection.path, preferences);
    FrontierX::RegisterNativeWindowManager(manager);
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    if (!options.vertexShaderPath.empty())
        FrontierX::RegisterNativeRenderPrimitive(manager);
#else
    if (!options.vertexShaderPath.empty())
    {
        std::cerr << "Native render smoke is unavailable in this build\n";
        return 7;
    }
#endif
    if (!options.configPath.empty())
    {
        const std::string configName = options.configPath.string();
        const GENRESULT configResult = manager->SetINIConfig(configName.c_str());
        if (configResult != GR_OK && configResult != GR_DATA_NOT_FOUND)
        {
            std::cerr << "Native component configuration failed\n";
            return 3;
        }
        if (configResult == GR_OK)
            std::cout << "Native component library configuration applied\n";
    }
    AGGDESC descriptor("IEventSystem", "EventSystem");
    InterfaceOwner<IAggregateComponent> aggregate;
    if (manager->CreateInstance(&descriptor,
        reinterpret_cast<void **>(aggregate.Receive())) != GR_OK || !aggregate.Get())
    {
        std::cerr << "Native event-system creation failed\n";
        return 3;
    }
    InterfaceOwner<IEventSystem> eventSystem;
    InterfaceOwner<ISystemComponent> eventSystemOwner;
    InterfaceOwner<IEventCallback> eventSystemCallback;
    if (aggregate.Get()->QueryInterface(IID_IEventSystem,
        reinterpret_cast<void **>(eventSystem.Receive())) != GR_OK || !eventSystem.Get() ||
        aggregate.Get()->QueryInterface(IID_ISystemComponent,
            reinterpret_cast<void **>(eventSystemOwner.Receive())) != GR_OK ||
        !eventSystemOwner.Get() ||
        aggregate.Get()->QueryInterface(IID_IEventCallback,
            reinterpret_cast<void **>(eventSystemCallback.Receive())) != GR_OK ||
        !eventSystemCallback.Get() ||
        aggregate.Get()->Initialize() != GR_OK || eventSystem.Get()->Initialize(0) != GR_OK)
    {
        std::cerr << "Native event-system initialization failed\n";
        return 3;
    }

    InterfaceOwner<IFileSystem> hotkeyFile;
    InterfaceOwner<IAggregateComponent> hotkeyAggregate;
    InterfaceOwner<IHotkeyEvent> hotkey;
    InterfaceOwner<IEventCallback> hotkeyInput;
    InterfaceOwner<IDAConnectionPoint> rawEventPoint;
    InterfaceOwner<IDAConnectionPoint> hotkeyEventPoint;
    ConnectionOwner rawEventConnection;
    ConnectionOwner hotkeyEventConnection;
    if (!options.hotkeysPath.empty())
    {
        std::string hotkeyError;
        if (!FrontierX::OpenNativeUtfFileSystem(
                options.hotkeysPath, hotkeyFile.Receive(), hotkeyError))
        {
            std::cerr << "Native hotkey table error: " << hotkeyError << '\n';
            return 6;
        }
        HKEVENTDESC hotkeyDescriptor;
        hotkeyDescriptor.file = hotkeyFile.Get();
        hotkeyDescriptor.hotkeyMessage = 0x10000U;
        if (manager->CreateInstance(&hotkeyDescriptor,
                reinterpret_cast<void **>(hotkeyAggregate.Receive())) != GR_OK ||
            !hotkeyAggregate.Get() ||
            hotkeyAggregate.Get()->QueryInterface(IID_IHotkeyEvent,
                reinterpret_cast<void **>(hotkey.Receive())) != GR_OK || !hotkey.Get() ||
            hotkeyAggregate.Get()->QueryInterface(IID_IEventCallback,
                reinterpret_cast<void **>(hotkeyInput.Receive())) != GR_OK || !hotkeyInput.Get() ||
            hotkeyAggregate.Get()->Initialize() != GR_OK ||
            eventSystem.Get()->QueryOutgoingInterface(
                "IEventCallback", rawEventPoint.Receive()) != GR_OK || !rawEventPoint.Get() ||
            hotkey.Get()->QueryOutgoingInterface(
                "IEventCallback", hotkeyEventPoint.Receive()) != GR_OK || !hotkeyEventPoint.Get() ||
            !rawEventConnection.Connect(rawEventPoint.Get(), hotkeyInput.Get()) ||
            !hotkeyEventConnection.Connect(hotkeyEventPoint.Get(), eventSystemCallback.Get()))
        {
            std::cerr << "Native hotkey service initialization failed\n";
            return 6;
        }
        std::cout << "Native hotkey table loaded and event pipeline connected\n";
    }

    DACOMDESC defaultsDescriptor("IUserDefaults");
    InterfaceOwner<IUserDefaults> userDefaults;
    if (manager->CreateInstance(&defaultsDescriptor,
        reinterpret_cast<void **>(userDefaults.Receive())) != GR_OK || !userDefaults.Get())
    {
        std::cerr << "Native user-defaults creation failed\n";
        return 3;
    }

    AGGDESC windowDescriptor("IWindowManager", "WindowManager");
    InterfaceOwner<IAggregateComponent> windowAggregate;
    if (manager->CreateInstance(&windowDescriptor,
        reinterpret_cast<void **>(windowAggregate.Receive())) != GR_OK || !windowAggregate.Get())
    {
        std::cerr << "Native window-manager creation failed\n";
        return 4;
    }
    InterfaceOwner<IWindowManager> windowManager;
    if (windowAggregate.Get()->QueryInterface(IID_IWindowManager,
        reinterpret_cast<void **>(windowManager.Receive())) != GR_OK || !windowManager.Get() ||
        windowAggregate.Get()->Initialize() != GR_OK ||
        windowManager.Get()->Startup(nullptr, "FrontierX native shell (game not started)",
            ShellExit, WMF_PREVENT_MULTIPLE, nullptr) != 1)
    {
        std::cerr << "Native window-manager startup failed\n";
        return 4;
    }
    shellEventSystem = eventSystem.Get();
    shellCloseRequested = false;
    windowManager.Get()->SetCallback(ShellWindowCallback);
    // Wayland compositors own top-level placement, so the portable shell does not
    // make successful startup depend on the legacy centering request.
    const U32 windowFlags = options.hidden ? 0U : WMF_SHOW;
    if (!windowManager.Get()->SetWindowPos(800, 600, windowFlags))
    {
        windowManager.Get()->SetCallback(nullptr);
        shellEventSystem = nullptr;
        std::cerr << "Native window-manager configuration failed\n";
        return 4;
    }
    const std::string driver = FrontierX::CurrentSdlVideoDriverName();
    if (driver.empty())
    {
        windowManager.Get()->SetCallback(nullptr);
        shellEventSystem = nullptr;
        std::cerr << "Native window startup failed: SDL reported no video driver\n";
        return 4;
    }
    std::cout << "Native shell opened; video driver: " << driver << '\n';

#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    FrontierX::SdlGpuRenderer renderer;
    InterfaceOwner<IRenderPrimitive> renderPrimitive;
    std::uint32_t presentedFrames = 0U;
    FrontierX::SdlGpuReadback directReadback;
    if (!options.vertexShaderPath.empty())
    {
        const HWND token = windowManager.Get()->GetWindowHandle();
        if (!token || token > std::numeric_limits<std::uint32_t>::max())
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native render smoke received an invalid SDL window token\n";
            return 7;
        }
        const FrontierX::SdlGpuTextureView texture{
            FrontierX::RenderSmokeTextureRgba.data(),
            FrontierX::RenderSmokeTextureWidth,
            FrontierX::RenderSmokeTextureHeight,
            FrontierX::RenderSmokeTextureRgba.size()};
        std::string renderError;
        if (!renderer.Initialize(static_cast<std::uint32_t>(token),
                options.vertexShaderPath.string().c_str(),
                options.fragmentShaderPath.string().c_str(), texture, renderError))
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native render initialization failed: " << renderError << '\n';
            return 7;
        }
        FrontierX::NativeRenderPrimitiveDesc renderDescriptor(
            FrontierX::SdlGpuRenderer::SubmissionCallback, &renderer, nullptr,
            FrontierX::SdlGpuRenderer::RenderStateCallback,
            nullptr, nullptr, FrontierX::SdlGpuRenderer::TextureStageCallback,
            FrontierX::SdlGpuRenderer::TextureBindCallback);
        if (manager->CreateInstance(&renderDescriptor,
                reinterpret_cast<void **>(renderPrimitive.Receive())) != GR_OK ||
            !renderPrimitive.Get())
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native render-scene owner creation failed\n";
            return 7;
        }
        FrontierX::NativeRenderPipeline renderPipeline(renderPrimitive.Get(),
            {&renderer, FrontierX::SdlGpuRenderer::TextureLookupCallback,
             FrontierX::SdlGpuRenderer::TextureCreateCallback,
             FrontierX::SdlGpuRenderer::TextureUploadCallback,
             FrontierX::SdlGpuRenderer::TextureDestroyCallback});
        PIPE = &renderPipeline;
        CQFLAGS.b3DEnabled = -1;
        RegisterTheRenderBatcher(manager);
        AGGDESC batchDescriptor("IRenderPrimitive");
        InterfaceOwner<IRenderPrimitive> renderBatcher;
        if (manager->CreateInstance(&batchDescriptor,
                reinterpret_cast<void **>(renderBatcher.Receive())) != GR_OK ||
            !renderBatcher.Get())
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native render-batcher creation failed\n";
            return 7;
        }
        InterfaceOwner<IAggregateComponent> batchAggregate;
        if (renderBatcher.Get()->QueryInterface(IID_IAggregateComponent,
                reinterpret_cast<void **>(batchAggregate.Receive())) != GR_OK ||
            !batchAggregate.Get() || batchAggregate.Get()->Initialize() != GR_OK)
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native render-batcher initialization failed\n";
            return 7;
        }
        batchAggregate.Reset();
        if (FrontierX::ConfigureRenderSmokeState(renderBatcher.Get()) != GR_OK ||
            renderBatcher.Get()->set_texture_stage_texture(
                0U, renderer.FixtureTextureHandle()) != GR_OK ||
            renderBatcher.Get()->set_texture_stage_texture(1U, 0U) != GR_OK ||
            FrontierX::SubmitRenderSmokeScene(renderBatcher.Get()) != GR_OK ||
            renderer.VertexCount() != FrontierX::RenderSmokeVertexCount ||
            renderer.IndexCount() != 0U ||
            !renderer.Readback64(directReadback, renderError) ||
            !directReadback.structureValid)
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native render-scene submission failed\n";
            return 7;
        }
        const PixelFormat rgbaFormat(PF_RGBA8);
        U32 alternateHandle = 0U;
        FrontierX::SdlGpuReadback alternateReadback;
        FrontierX::SdlGpuReadback reboundReadback;
        if (renderPipeline.create_texture(2, 2, rgbaFormat, 0, 0U,
                alternateHandle) != GR_OK ||
            renderPipeline.set_texture_level_data(alternateHandle, 0, 2, 2, 8,
                rgbaFormat, FrontierX::RenderSmokeAlternateTextureRgba.data(),
                nullptr, nullptr) != GR_OK ||
            renderBatcher.Get()->set_texture_stage_texture(0U, alternateHandle) != GR_OK ||
            !renderer.Readback64(alternateReadback, renderError) ||
            alternateReadback.changedPixels != directReadback.changedPixels ||
            alternateReadback.checksum == directReadback.checksum ||
            renderBatcher.Get()->set_texture_stage_texture(
                0U, renderer.FixtureTextureHandle()) != GR_OK ||
            !renderer.Readback64(reboundReadback, renderError) ||
            reboundReadback.checksum != directReadback.checksum ||
            renderPipeline.destroy_texture(alternateHandle) != GR_OK ||
            FrontierX::SubmitIndexedRenderSmokeScene(renderBatcher.Get()) != GR_OK ||
            renderer.VertexCount() != FrontierX::RenderSmokeIndexedVertexCount ||
            renderer.IndexCount() != FrontierX::RenderSmokeIndexCount)
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native render-scene submission failed\n";
            return 7;
        }
        renderBatcher.Reset();
        PIPE = nullptr;
        std::cout << "Native render smoke initialized; backend: "
                  << renderer.DriverName()
                  << "; source: PrimitiveBuilder and indexed fixture via production "
                     "RenderBatcher and IRenderPipeline\n";
    }
#endif

    const std::uint32_t started = FrontierX::Platform::MonotonicMilliseconds();
    while (!shellCloseRequested)
    {
        windowManager.Get()->ServeMessageQueue(WMF_BACKGROUND);
        eventSystemOwner.Get()->Update();
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
        if (!options.vertexShaderPath.empty() && !shellCloseRequested)
        {
            bool presented = false;
            std::string renderError;
            if (!renderer.Present(presented, renderError))
            {
                windowManager.Get()->SetCallback(nullptr);
                shellEventSystem = nullptr;
                std::cerr << "Native render presentation failed: " << renderError << '\n';
                return 7;
            }
            if (presented) ++presentedFrames;
        }
#endif
        if (!shellCloseRequested && options.smokeMilliseconds)
        {
            const std::uint32_t elapsed =
                FrontierX::Platform::MonotonicMilliseconds() - started;
            shellCloseRequested = elapsed >= options.smokeMilliseconds;
        }
        if (!shellCloseRequested) FrontierX::Platform::SleepForMilliseconds(1);
    }
    windowManager.Get()->SetCallback(nullptr);
    shellEventSystem = nullptr;
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    if (!options.vertexShaderPath.empty())
    {
        FrontierX::SdlGpuReadback readback;
        std::string renderError;
        if (!presentedFrames || !renderer.Readback64(readback, renderError) ||
            !readback.structureValid)
        {
            std::cerr << "Native render readback failed: " << renderError << '\n';
            return 7;
        }
        if (readback.changedPixels != directReadback.changedPixels ||
            readback.checksum != directReadback.checksum)
        {
            std::cerr << "Native direct and indexed fixture readbacks differ\n";
            return 7;
        }
        std::cout << "Native render smoke passed; presented-frames: " << presentedFrames
                  << "; vertices: " << renderer.VertexCount()
                  << "; changed-pixels: " << readback.changedPixels
                  << "; pixel-checksum: " << readback.checksum << '\n';
        renderPrimitive.Reset();
        renderer.Shutdown();
    }
#endif
    hotkeyEventConnection.Disconnect();
    rawEventConnection.Disconnect();
    if (userDefaults.Get()->StoreDefaults() == FALSE)
    {
        std::cerr << "Native preference save failed\n";
        return 5;
    }
    if (options.vertexShaderPath.empty())
        std::cout << "Native shell closed cleanly; game startup and rendering were not attempted\n";
    else
        std::cout << "Native shell closed cleanly; repository render fixture completed; "
                     "game startup was not attempted\n";
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
