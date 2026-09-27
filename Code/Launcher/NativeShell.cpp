#include "EventSys.h"
#include "DACOM.H"
#include "HeapObj.h"
#include "HKEvent.h"
#include "IConnection.h"
#include "IProfileParser.h"
#include "FrontierX/DataRoot.h"
#include "FrontierX/LegacyInput.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeUserDefaults.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/NativeWritableUtfArchive.h"
#include "FrontierX/NativeWindowManager.h"
#include "FrontierX/NativeStreamer.h"
#include "FrontierX/NativeParticleManager.h"
#include "FrontierX/NativeMeshManager.h"
#include "FrontierX/PlatformTime.h"
#include "FrontierX/SdlWindow.h"
#include "FrontierX/StartupDataRoot.h"
#include "FrontierX/StartupPreferences.h"
#include "FDump.h"
#include "SYSTEM.H"
#include "UserDefaults.h"
#include "WindowManager.h"
#include "Streamer.h"
#include "MusicManager.h"
#include "InProgressAnim.h"
#include "StatusBar.h"
#include "Hintbox.h"
#include "ScrollingText.h"
#include "resource.h"
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppSchema.h"
#include "Include/NativeAppTextResources.h"
#include "Include/NativeMusicManager.h"
#include "Include/NativeGenData.h"
#include "ViewCnst.h"

#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
#include "CQBatch.h"
#include "DEffectOpts.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeEngine.h"
#include "FrontierX/NativeAnimation.h"
#include "FrontierX/NativeChannel.h"
#include "FrontierX/NativeHardpoint.h"
#include "FrontierX/NativeOptics.h"
#include "FrontierX/NativePolyMesh.h"
#include "FrontierX/NativeRenderManager.h"
#include "FrontierX/NativeMathEngine.h"
#include "FrontierX/GameMaterialManager.h"
#include "FrontierX/GameTextureManager.h"
#include "FrontierX/VertexBufferManagerRegistration.h"
#include "FrontierX/StringTableRegistration.h"
#include "FrontierX/RenderSmokeScene.h"
#include "FrontierX/SdlGpuRenderer.h"
#include "NativeGameScene.h"
#include "NativeGameSystem.h"
#include "NativeAnimationValidation.h"
#include "GlobalFlagTypes.h"
#include "IRenderPrimitive.h"
#include "IVertexBufferManager.h"
#include "RenderBatcherRegistration.h"
#endif

#include <charconv>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cwchar>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

extern ICOManager * DACOM;
void RegisterContainerFactory(void);
void RegisterViewConstructor(ICOManager * manager);
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
bool RegisterNativeTextureLibrary(ICOManager * manager);
bool RegisterNativeLightManager(ICOManager * manager);
#endif

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

namespace
{
struct AppDacomGlobal
{
    explicit AppDacomGlobal(ICOManager * manager) { DACOM = manager; }
    ~AppDacomGlobal() { DACOM = nullptr; }
};

IEventSystem * shellEventSystem = nullptr;
bool shellCloseRequested = false;

#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
struct GameSceneLoop
{
    IWindowManager * window;
    ISystemComponent * events;
    std::uint32_t started;
    std::uint32_t duration;
};

bool ContinueGameScene(void * context)
{
    auto & loop = *static_cast<GameSceneLoop *>(context);
    loop.window->ServeMessageQueue(WMF_BACKGROUND);
    loop.events->Update();
    if (!shellCloseRequested && loop.duration)
        shellCloseRequested =
            FrontierX::Platform::MonotonicMilliseconds() - loop.started >=
            loop.duration;
    if (!shellCloseRequested) FrontierX::Platform::SleepForMilliseconds(1);
    return !shellCloseRequested;
}
#endif

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
    std::filesystem::path systemProfilePath;
    std::filesystem::path dataRoot;
    std::filesystem::path materialRoot;
    std::filesystem::path saveRoot;
    std::filesystem::path genDataPath;
    std::filesystem::path gameTypesPath;
    std::filesystem::path preferencesPath;
    std::filesystem::path hotkeysPath;
    std::filesystem::path vertexShaderPath;
    std::filesystem::path fragmentShaderPath;
    std::filesystem::path capturePpmPath;
    std::filesystem::path gameMaterialPath;
    std::filesystem::path gameMeshPath;
    std::filesystem::path gameExtendedShaderPath;
    std::filesystem::path gameVertexShaderPath;
    std::string animationObjectName;
    std::string animationScriptName;
    std::string meshObjectName;
    std::string missionSourceName;
    std::uint32_t smokeMilliseconds = 0;
    bool hidden = false;
    bool skipMovies = false;
    bool skipNotificationCue = false;
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
            std::cout << "Usage: frontierx_native_shell [--config FILE] [--system-profile FILE] [--preferences FILE] "
                         "[--data-root DIRECTORY] [--material-root DIRECTORY] [--save-root DIRECTORY] [--gen-data FILE] [--game-types FILE] [--hotkeys FILE] "
                         "[--render-smoke VERTEX_SPV FRAGMENT_SPV] "
                         "[--game-scene MATERIAL_SOURCE MESH_UTF VERTEX_SPV "
                         "FRAGMENT_SPV EXTENDED_SPV GAME_SPV] "
                         "[--capture-ppm FILE] [--skip-movies] [--skip-notification-cue] "
                         "[--animation-object OBJECT_NAME SCRIPT_NAME] "
                         "[--mesh-object OBJECT_NAME] "
                         "[--mission-source MISSION_NAME] "
                         "[--smoke-ms MILLISECONDS [--hidden]]\n"
                         "Opens the native startup shell. The render smoke draws a "
                         "repository fixture; game-scene renders one supplied UTF mesh "
                         "through production Engine/RenderMgr/PolyMesh.\n";
            std::exit(0);
        }
        if (option == "--skip-movies")
        {
            if (options.skipMovies) throw std::invalid_argument("duplicate option: --skip-movies");
            options.skipMovies = true;
            continue;
        }
        if (option == "--skip-notification-cue")
        {
            if (options.skipNotificationCue)
                throw std::invalid_argument("duplicate option: --skip-notification-cue");
            options.skipNotificationCue = true;
            continue;
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
        if (option == "--game-scene")
        {
            if (!options.gameMeshPath.empty() || !options.vertexShaderPath.empty())
                throw std::invalid_argument("--game-scene cannot follow another render mode");
            if (index + 6 >= argc)
                throw std::invalid_argument("--game-scene requires a material source, a mesh UTF and four SPIR-V files");
            for (int offset = 1; offset <= 6; ++offset)
                if (!argv[index + offset][0] ||
                    std::string(argv[index + offset]).rfind("--", 0) == 0)
                    throw std::invalid_argument("--game-scene requires six file paths");
            options.gameMaterialPath = argv[++index];
            options.gameMeshPath = argv[++index];
            options.vertexShaderPath = argv[++index];
            options.fragmentShaderPath = argv[++index];
            options.gameExtendedShaderPath = argv[++index];
            options.gameVertexShaderPath = argv[++index];
            continue;
        }
        if (option == "--animation-object")
        {
            if (!options.animationObjectName.empty() || index + 2 >= argc)
                throw std::invalid_argument("--animation-object requires an object and script name");
            const std::string objectName(argv[++index]);
            const std::string scriptName(argv[++index]);
            if (objectName.empty() || scriptName.empty() ||
                objectName.rfind("--", 0) == 0 || scriptName.rfind("--", 0) == 0 ||
                objectName.find_first_of("/\\") != std::string::npos ||
                objectName == "." || objectName == "..")
                throw std::invalid_argument("--animation-object requires one object and script name");
            options.animationObjectName = objectName;
            options.animationScriptName = scriptName;
            continue;
        }
        if (option == "--mesh-object")
        {
            if (!options.meshObjectName.empty() || index + 1 >= argc)
                throw std::invalid_argument("--mesh-object requires one object name");
            const std::string objectName(argv[++index]);
            if (objectName.empty() || objectName.rfind("--", 0) == 0 ||
                objectName.find_first_of("/\\") != std::string::npos ||
                objectName == "." || objectName == "..")
                throw std::invalid_argument("--mesh-object requires one object name");
            options.meshObjectName = objectName;
            continue;
        }
        if (option == "--mission-source")
        {
            if (!options.missionSourceName.empty() || index + 1 >= argc)
                throw std::invalid_argument("--mission-source requires one mission name");
            const std::string missionName(argv[++index]);
            if (missionName.empty() || missionName.rfind("--", 0) == 0 ||
                missionName.find_first_of("/\\") != std::string::npos ||
                missionName == "." || missionName == ".." ||
                missionName.size() <= sizeof(".dmission") - 1 ||
                missionName.compare(missionName.size() - (sizeof(".dmission") - 1),
                    sizeof(".dmission") - 1, ".dmission") != 0)
                throw std::invalid_argument("--mission-source requires one .dmission name");
            options.missionSourceName = missionName;
            continue;
        }
        if ((option != "--config" && option != "--system-profile" &&
             option != "--preferences" && option != "--hotkeys" &&
             option != "--data-root" && option != "--material-root" && option != "--save-root" && option != "--gen-data" && option != "--game-types" && option != "--smoke-ms" &&
             option != "--capture-ppm") ||
            index + 1 == argc)
            throw std::invalid_argument(
                "expected --config, --system-profile, --preferences, --data-root, --material-root, --save-root, --gen-data, --game-types, --hotkeys, --smoke-ms, "
                "--capture-ppm, --animation-object, --mesh-object, --mission-source, --skip-movies, --skip-notification-cue or --hidden");
        const char * value = argv[++index];
        if (!value[0] || std::string(value).rfind("--", 0) == 0)
            throw std::invalid_argument("missing option value");
        if (option == "--config")
        {
            if (!options.configPath.empty())
                throw std::invalid_argument("duplicate option: --config");
            options.configPath = value;
        }
        else if (option == "--system-profile")
        {
            if (!options.systemProfilePath.empty())
                throw std::invalid_argument("duplicate option: --system-profile");
            options.systemProfilePath = value;
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
        else if (option == "--material-root")
        {
            if (!options.materialRoot.empty())
                throw std::invalid_argument("duplicate option: --material-root");
            options.materialRoot = value;
        }
        else if (option == "--save-root")
        {
            if (!options.saveRoot.empty())
                throw std::invalid_argument("duplicate option: --save-root");
            options.saveRoot = value;
        }
        else if (option == "--gen-data")
        {
            if (!options.genDataPath.empty())
                throw std::invalid_argument("duplicate option: --gen-data");
            options.genDataPath = value;
        }
        else if (option == "--game-types")
        {
            if (!options.gameTypesPath.empty())
                throw std::invalid_argument("duplicate option: --game-types");
            options.gameTypesPath = value;
        }
        else if (option == "--capture-ppm")
        {
            if (!options.capturePpmPath.empty())
                throw std::invalid_argument("duplicate option: --capture-ppm");
            options.capturePpmPath = value;
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
    if (!options.gameMeshPath.empty() && options.hidden)
        throw std::invalid_argument("--game-scene requires a visible window");
    if (!options.capturePpmPath.empty() && options.vertexShaderPath.empty())
        throw std::invalid_argument("--capture-ppm requires a render mode");
    if (!options.genDataPath.empty() && options.missionSourceName.empty())
        throw std::invalid_argument("--gen-data requires --mission-source");
    if (!options.gameTypesPath.empty() && options.missionSourceName.empty())
        throw std::invalid_argument("--game-types requires --mission-source");
    if (options.skipNotificationCue && options.missionSourceName.empty())
        throw std::invalid_argument("--skip-notification-cue requires --mission-source");
    if (!options.saveRoot.empty() && options.missionSourceName.empty())
        throw std::invalid_argument("--save-root requires --mission-source");
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
    auto startupRoot = FrontierX::ResolveStartupDataRoot(
        options.dataRoot, environmentRoot, configuredRoot);
    if (!startupRoot.directoryAccessible)
    {
        std::cerr << "Data root is not an accessible directory; set --data-root or "
                     "FRONTIERX_DATA_ROOT\n";
        return 2;
    }
    InterfaceOwner<IFileSystem> materialSource;
    if (!options.materialRoot.empty())
    {
        std::string error;
        if (!FrontierX::OpenNativeDirectoryFileSystem(
                options.materialRoot, materialSource.Receive(), error))
        {
            std::cerr << "Native material source open failed: " << error << '\n';
            return 2;
        }
        for (const char *name : {"shaders", "materials", "textures"})
        {
            InterfaceOwner<IFileSystem> child;
            DAFILEDESC descriptor(name);
            if (materialSource.Get()->CreateInstance(&descriptor,
                    reinterpret_cast<void **>(child.Receive())) != GR_OK ||
                !child.Get() || !child.Get()->SetCurrentDirectory("/"))
            {
                std::cerr << "Native material source lacks directory: " << name << '\n';
                return 2;
            }
        }
        for (auto &entry : startupRoot.entries)
            if (entry.expectedPath == "Mat")
            {
                entry.actualPath = options.materialRoot;
                entry.archiveDirectory.clear();
                entry.status = FrontierX::Platform::DataRootEntryStatus::Present;
            }
        std::cout << "Native material source: explicit directory; required children opened\n";
    }
    if (!FrontierX::Platform::IsDataRootUsable(startupRoot.entries))
    {
        for (const auto & entry : startupRoot.entries)
            if (entry.required &&
                !FrontierX::Platform::IsDataRootEntryUsable(entry))
                std::cerr << entry.expectedPath.generic_string() << ": "
                          << FrontierX::Platform::DataRootEntryStatusName(entry.status) << '\n';
        std::cerr << "Required data sources are missing or have incompatible names/types\n";
        return 2;
    }
    std::cout << "Data root source: "
              << FrontierX::StartupDataRootSourceName(startupRoot.source)
              << "; required data sources passed\n";

    // Retain packed sources through startup and the scene, as the App does for
    // its directory shortcuts. Physical directory sources retain their existing path.
    std::vector<std::pair<std::filesystem::path,
        std::unique_ptr<InterfaceOwner<IFileSystem>>>> archiveMounts;
    for (const auto & entry : startupRoot.entries)
    {
        if (entry.status != FrontierX::Platform::DataRootEntryStatus::PresentArchive) continue;
        auto mounted = std::make_unique<InterfaceOwner<IFileSystem>>();
        std::string error;
        if (!FrontierX::Platform::OpenDataRootArchive(
                startupRoot.path, entry, mounted->Receive(), error))
        {
            std::cerr << "Native data archive mount failed: "
                      << entry.expectedPath.generic_string() << ": " << error << '\n';
            return 2;
        }
        archiveMounts.emplace_back(entry.expectedPath, std::move(mounted));
    }
    std::cout << "Native data archives mounted: " << archiveMounts.size() << '\n';

    ICOManager * manager = DACOM_Acquire();
    if (!manager)
    {
        std::cerr << "Native component-manager acquisition failed\n";
        return 3;
    }
    AppDacomGlobal appDacomGlobal(manager);
    if (InitializeDAHeap(0x10000, 0x4000,
            DAHEAPFLAG_MULTITHREADED | DAHEAPFLAG_GROWHEAP |
            DAHEAPFLAG_NOMSGS) == FALSE)
    {
        std::cerr << "Native shared-heap initialization failed\n";
        return 3;
    }
    RegisterTheEventSystem(manager);
    RegisterContainerFactory();
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    if (!FrontierX::RegisterNativeEngine(manager))
    {
        std::cerr << "Native game-engine registration failed\n";
        return 3;
    }
#endif
    RegisterHotkeyEventServer(manager);
    RegisterViewConstructor(manager);
    FrontierX::RegisterNativeUserDefaults(
        manager, preferences.selection.path, preferences);
    DACOMDESC defaultsDescriptor("IUserDefaults");
    InterfaceOwner<IUserDefaults> userDefaults;
    if (manager->CreateInstance(&defaultsDescriptor,
            reinterpret_cast<void **>(userDefaults.Receive())) != GR_OK ||
        !userDefaults.Get() || userDefaults.Get()->LoadDefaults() == FALSE)
    {
        std::cerr << "Native user-defaults initialization failed\n";
        return 3;
    }
    std::cout << "Native user defaults initialized before GameSystem startup\n";
    FrontierX::RegisterNativeWindowManager(manager);
    if (!FrontierX::RegisterNativeStreamer(manager))
        throw std::runtime_error("native streamer registration failed");
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    if (!FrontierX::RegisterNativeParticleManager(manager))
        throw std::runtime_error("native particle manager registration failed");
    if (!FrontierX::RegisterNativeMeshManager(manager))
        throw std::runtime_error("native mesh manager registration failed");
    if (!FrontierX::RegisterNativeAnimation(manager))
        throw std::runtime_error("native animation registration failed");
    if (!FrontierX::RegisterNativeChannel(manager))
        throw std::runtime_error("native channel registration failed");
    if (!FrontierX::RegisterNativeHardpoint(manager))
        throw std::runtime_error("native hardpoint registration failed");
    if (!FrontierX::RegisterNativeOptics(manager))
        throw std::runtime_error("native optics registration failed");
    if (!FrontierX::RegisterNativePolyMesh(manager) ||
        !FrontierX::RegisterNativeRenderManager(manager))
        throw std::runtime_error("native renderer registration failed");
    FrontierX::RegisterNativeRenderPrimitive(manager);
    FrontierX::RegisterNativeRenderPipeline(manager);
    if (!RegisterNativeTextureLibrary(manager))
    {
        std::cerr << "Native texture-library registration failed\n";
        return 3;
    }
    if (!FrontierX::RegisterNativeMathEngine(manager) ||
        !RegisterNativeLightManager(manager))
    {
        std::cerr << "Native light-manager registration failed\n";
        return 3;
    }
    RegisterTheVertexBufferManager(manager);
    if (!FrontierX::RegisterNativeGameMaterialManager(manager) ||
        !FrontierX::RegisterNativeGameTextureManager(manager))
    {
        std::cerr << "Native game material/texture manager registration failed\n";
        return 3;
    }
    if (!RegisterTheStringTable(manager))
    {
        std::cerr << "Native string-table registration failed\n";
        return 3;
    }
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
    PROFPARSEDESC2 profileDescriptor;
    InterfaceOwner<IAggregateComponent> profileAggregate;
    InterfaceOwner<IProfileParser2> profile2;
    InterfaceOwner<IProfileParser> profile;
    constexpr char defaultSystemProfile[] =
        "[System]\nIEventSystem\nIWindowManager = WindowManager\n[Engine]\n";
    std::string systemProfile(defaultSystemProfile,
        sizeof(defaultSystemProfile) - 1U);
    if (!options.systemProfilePath.empty())
    {
        std::ifstream profile(options.systemProfilePath, std::ios::binary);
        if (!profile)
        {
            std::cerr << "Native system profile read failed\n";
            return 3;
        }
        profile.seekg(0, std::ios::end);
        const std::streampos length = profile.tellg();
        if (length <= 0 || length > 1024 * 1024)
        {
            std::cerr << "Native system profile size is invalid\n";
            return 3;
        }
        systemProfile.resize(static_cast<std::size_t>(length));
        profile.seekg(0, std::ios::beg);
        profile.read(systemProfile.data(),
            static_cast<std::streamsize>(systemProfile.size()));
        if (!profile)
        {
            std::cerr << "Native system profile read failed\n";
            return 3;
        }
    }
    if (manager->CreateInstance(&profileDescriptor,
            reinterpret_cast<void **>(profileAggregate.Receive())) != GR_OK ||
        !profileAggregate.Get() ||
        profileAggregate.Get()->QueryInterface(IID_IProfileParser2,
            reinterpret_cast<void **>(profile2.Receive())) != GR_OK || !profile2.Get() ||
        profile2.Get()->Initialize2(systemProfile.data(),
            static_cast<U32>(systemProfile.size())) != GR_OK ||
        profileAggregate.Get()->QueryInterface(IID_IProfileParser,
            reinterpret_cast<void **>(profile.Receive())) != GR_OK || !profile.Get())
    {
        std::cerr << "Native App system profile initialization failed\n";
        return 3;
    }
    auto *nativeManager = static_cast<FrontierX::NativeComponentManager *>(manager);
    nativeManager->SetProfileParser(profile.Get());
    // Use the production MovieScreen guard; never substitute a successful video stub.
    CQFLAGS.bSkipMovies = options.skipMovies ? 1 : 0;
    if (options.skipMovies)
        std::cout << "Native full-screen movies explicitly disabled; audio and animation remain required\n";
    shellCloseRequested = false;
    FrontierX::NativeAppDataSources appDataSources;
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    using GameSystemOwner = std::unique_ptr<FrontierX::NativeGameSystem,
        decltype(&FrontierX::DestroyNativeGameSystem)>;
    GameSystemOwner gameSystem(FrontierX::StartNativeGameSystem(
        "FrontierX native shell (game not started)", ShellExit,
        WMF_PREVENT_MULTIPLE), FrontierX::DestroyNativeGameSystem);
    ISystemContainer * systemContainer =
        FrontierX::NativeGameSystemContainer(gameSystem.get());
#else
    AGGDESC systemDescriptor("ISystemContainer");
    InterfaceOwner<ISystemContainer> systemContainerOwner;
    const GENRESULT createResult = manager->CreateInstance(&systemDescriptor,
        reinterpret_cast<void **>(systemContainerOwner.Receive()));
    ISystemContainer * systemContainer = systemContainerOwner.Get();
    const GENRESULT loadResult = createResult == GR_OK && systemContainer ?
        systemContainer->LoadSystemComponents() : GR_GENERIC;
#endif
    // Release aggregate interface references before GameSystem tears down its container.
    FrontierX::NativeAppSystemBinding appSystemBinding;
    FrontierX::NativeAppTextBinding appTextBinding;
    FrontierX::NativeAppSaveBinding appSaveBinding;
    FrontierX::NativeAppAudioBinding appAudioBinding;
    FrontierX::NativeAppMusicBinding appMusicBinding;
    nativeManager->SetProfileParser(nullptr);
    bool systemReady = systemContainer != nullptr;
#if !defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    systemReady = systemReady && loadResult == GR_OK;
#endif
    if (!systemReady)
    {
        std::cerr << "Native game-system startup failed\n";
        return 3;
    }
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    std::cout << "Native GameSystem startup completed with "
              << (options.systemProfilePath.empty() ? "synthetic" : "supplied")
              << " profile\n";
    InterfaceOwner<IFileSystem> genDataArchive;
    std::unique_ptr<IGeneralData, decltype(&FrontierX::ReleaseNativeGeneralData)>
        generalData(nullptr, FrontierX::ReleaseNativeGeneralData);
    FrontierX::NativeAppGeneralDataBinding generalDataBinding;
    InterfaceOwner<IFileSystem> gameTypesArchive;
    InterfaceOwner<IFileSystem> saveDirectory;
    FrontierX::NativeAppGameTypesBinding gameTypesBinding;
    if (!options.missionSourceName.empty())
    {
        if (!options.saveRoot.empty())
        {
            std::error_code pathError;
            std::string saveError;
            const auto retailRoot = std::filesystem::canonical(startupRoot.path, pathError);
            if (pathError)
            {
                std::cerr << "Native App retail data root canonicalization failed\n";
                return 6;
            }
            const auto requestedSaveRoot = std::filesystem::canonical(options.saveRoot, pathError);
            const auto retailPrefix = retailRoot.string() + "/";
            if (pathError || requestedSaveRoot == retailRoot ||
                requestedSaveRoot.string().rfind(retailPrefix, 0) == 0 ||
                !FrontierX::OpenNativeSaveDirectory(
                    options.saveRoot, saveDirectory.Receive(), saveError) ||
                !appSaveBinding.Bind(saveDirectory.Get()))
            {
                std::cerr << "Native App save-directory binding failed: " << saveError << '\n';
                return 6;
            }
            std::cout << "Native App writable save directory initialized outside retail data\n";
        }
        else
            std::cout << "Native App save directory unavailable; mission save not enabled\n";
        InterfaceOwner<IPANIM> loadProgress;
        CreateInProgressAnim(loadProgress.Receive());
        if (!loadProgress.Get())
        {
            std::cerr << "Native App mission progress owner unavailable\n";
            return 6;
        }
        loadProgress.Get()->UpdateString(IDS_PROG_INITMAP);
        loadProgress.Get()->SetProgress(0.05F);
        if (loadProgress.Get()->GetProgress() != 0.05F ||
            !CQFLAGS.bInProgressAnimActive)
        {
            std::cerr << "Native App mission progress state mismatch\n";
            return 6;
        }
        loadProgress.Get()->Stop();
        loadProgress.Reset();
        if (CQFLAGS.bInProgressAnimActive)
        {
            std::cerr << "Native App mission progress lifetime mismatch\n";
            return 6;
        }
        std::cout << "Native App mission progress owner initialized\n";
        if (std::wcscmp(_localLoadStringW(IDS_DISABLED), L"Disabled") != 0 ||
            std::string(_localLoadString(IDS_DISABLED)) != "Disabled")
        {
            std::cerr << "Native App source string resource mismatch\n";
            return 6;
        }
        std::cout << "Native App source string resources initialized\n";
        if (!appSystemBinding.Bind(systemContainer))
        {
            std::cerr << "Native App system binding failed\n";
            return 6;
        }
        if (!appTextBinding.Bind(EVENTSYS))
        {
            std::cerr << "Native App text-resource binding failed\n";
            return 6;
        }
        if (!appTextBinding.BindScrollingText(options.skipNotificationCue))
        {
            std::cerr << "Native App scrolling-text binding failed\n";
            return 6;
        }
        if (options.skipNotificationCue)
        {
            if (SCROLLTEXT->SetText(IDS_DISABLED) != GR_OK ||
                SCROLLTEXT->GetText() != 0 ||
                SCROLLTEXT->SetText(0) != GR_INVALID_PARMS ||
                SCROLLTEXT->SetTextString(L"Native message") != GR_OK)
            {
                std::cerr << "Native App scrolling-text contract failed\n";
                return 6;
            }
            std::cout << "Native App scrolling text initialized; notification cue explicitly disabled\n";
        }
        InterfaceOwner<IDAConnectionPoint> statusClients;
        InterfaceOwner<IDAConnectionPoint> hintClients;
        if (STATUS->QueryOutgoingInterface("IResourceClient", statusClients.Receive()) != GR_OK ||
            !statusClients.Get() ||
            HINTBOX->QueryOutgoingInterface("IResourceClient", hintClients.Receive()) != GR_OK ||
            !hintClients.Get() ||
            STATUS->SetText(IDS_DISABLED) != GR_OK ||
            STATUS->GetText() != IDS_DISABLED ||
            HINTBOX->GetText() != 0)
        {
            std::cerr << "Native App text-resource contract failed\n";
            return 6;
        }
        HINTBOX->SetText(IDS_DISABLED);
        if (HINTBOX->GetText() != IDS_DISABLED ||
            STATUS->SetDefaultState() != GR_OK || STATUS->GetText() != 0 ||
            HINTBOX->SetDefaultState() != GR_OK || HINTBOX->GetText() != 0)
        {
            std::cerr << "Native App text-resource reset failed\n";
            return 6;
        }
        std::cout << "Native App text resources initialized\n";
        InterfaceOwner<IViewConstructor> viewConstructor;
        InterfaceOwner<IViewConstructor2> viewConstructor2;
        if (GS->QueryInterface(IID_IViewConstructor,
                reinterpret_cast<void **>(viewConstructor.Receive())) != GR_OK ||
            !viewConstructor.Get() ||
            viewConstructor.Get()->QueryInterface(IID_IViewConstructor2,
                reinterpret_cast<void **>(viewConstructor2.Receive())) != GR_OK ||
            !viewConstructor2.Get() ||
            !FrontierX::ParseNativeAppSchema(viewConstructor2.Get()))
        {
            std::cerr << "Native App parser schema initialization failed\n";
            return 6;
        }
        std::cout << "Native App parser schema loaded from embedded data.i\n";
        if (!options.gameTypesPath.empty())
        {
            std::string error;
            if (!FrontierX::OpenNativeUtfFileSystem(options.gameTypesPath,
                    gameTypesArchive.Receive(), error))
            {
                std::cerr << "Native App game-types archive invalid: " << error << '\n';
                return 6;
            }
            DAFILEDESC typeDirectory("BT_FABRICATOR_DATA");
            InterfaceOwner<IFileSystem> fabricatorTypes;
            if (gameTypesArchive.Get()->CreateInstance(&typeDirectory,
                    reinterpret_cast<void **>(fabricatorTypes.Receive())) != GR_OK ||
                !fabricatorTypes.Get())
            {
                std::cerr << "Native App Forger game-types directory unavailable\n";
                return 6;
            }
            DAFILEDESC forgerType("FAB!!S_Forger");
            const HANDLE forgerRecord = fabricatorTypes.Get()->OpenChild(&forgerType);
            if (forgerRecord == INVALID_HANDLE_VALUE)
            {
                std::cerr << "Native App Forger game-types record unavailable\n";
                return 6;
            }
            const DWORD forgerBytes = fabricatorTypes.Get()->GetFileSize(forgerRecord, nullptr);
            const BOOL forgerClosed = fabricatorTypes.Get()->CloseHandle(forgerRecord);
            if (!forgerClosed || forgerBytes == 0)
            {
                std::cerr << "Native App Forger game-types record invalid\n";
                return 6;
            }
            if (!gameTypesBinding.Bind(gameTypesArchive.Get()))
            {
                std::cerr << "Native App game-types binding failed\n";
                return 6;
            }
            std::cout << "Native App GameTypes archive opened for ObjectList; Forger type bytes: "
                      << forgerBytes << "; mission not started\n";
        }
        if (!options.genDataPath.empty())
        {
            std::string error;
            IGeneralData *created = nullptr;
            unsigned int globalValueBytes = 0;
            if (!FrontierX::OpenNativeUtfFileSystem(options.genDataPath,
                    genDataArchive.Receive(), error))
            {
                std::cerr << "Native App general data archive invalid: " << error << '\n';
                return 6;
            }
            if (!FrontierX::CreateNativeGeneralData(genDataArchive.Get(),
                    &created, &globalValueBytes))
            {
                std::cerr << "Native App general data contents invalid\n";
                return 6;
            }
            generalData.reset(created);
            if (!generalDataBinding.Bind(generalData.get()))
            {
                std::cerr << "Native App general data binding failed\n";
                return 6;
            }
            std::cout << "Native App GenData loaded through production owner; GlobalValues bytes: "
                      << globalValueBytes << '\n';
        }
        InterfaceOwner<IFileSystem> physicalObjects;
        InterfaceOwner<IFileSystem> physicalCampaign;
        InterfaceOwner<IFileSystem> physicalMaterials;
        const auto resolveSource = [&](const char * name,
                                       InterfaceOwner<IFileSystem> & physical) -> IFileSystem * {
            for (const auto & mount : archiveMounts)
                if (mount.first == name) return mount.second->Get();
            for (const auto & entry : startupRoot.entries)
                if (entry.expectedPath == name &&
                    entry.status == FrontierX::Platform::DataRootEntryStatus::Present)
                {
                    std::string error;
                    if (FrontierX::OpenNativeDirectoryFileSystem(
                            startupRoot.path / entry.actualPath,
                            physical.Receive(), error)) return physical.Get();
                }
            return nullptr;
        };
        IFileSystem * objects = resolveSource("Objects", physicalObjects);
        IFileSystem * campaign = resolveSource("Maps/Campaign", physicalCampaign);
        IFileSystem * materials = materialSource.Get() ? materialSource.Get() :
            resolveSource("Mat", physicalMaterials);
        if (!appDataSources.Bind(objects, campaign, materials))
        {
            std::cerr << "Native App mission data sources unavailable\n";
            return 6;
        }
        DAFILEDESC missionDescriptor(options.missionSourceName.c_str());
        InterfaceOwner<IFileSystem> mission;
        if (SPMAPDIR->CreateInstance(&missionDescriptor,
                reinterpret_cast<void **>(mission.Receive())) != GR_OK ||
            !mission.Get())
        {
            std::cerr << "Native App mission source open failed\n";
            return 6;
        }
        HANDLE missionSymbols = nullptr;
        U32 parseBytes = 0;
        if (!FrontierX::LoadNativeMissionSchema(mission.Get(),
                viewConstructor2.Get(), missionSymbols, &parseBytes))
        {
            std::cerr << "Native App mission ParseData invalid\n";
            return 6;
        }
        viewConstructor2.Get()->DestroySymbols(missionSymbols);
        if (!mission.Get()->SetCurrentDirectory("\\MT_GlobalData"))
        {
            std::cerr << "Native App mission global data unavailable\n";
            return 6;
        }
        DAFILEDESC globalsDescriptor("Globals");
        const HANDLE globalsFile = mission.Get()->OpenChild(&globalsDescriptor);
        if (globalsFile == INVALID_HANDLE_VALUE)
        {
            std::cerr << "Native App mission global record unavailable\n";
            return 6;
        }
        const DWORD globalsBytes = mission.Get()->GetFileSize(globalsFile, nullptr);
        const BOOL globalsClosed = mission.Get()->CloseHandle(globalsFile);
        if (!globalsClosed || globalsBytes == 0)
        {
            std::cerr << "Native App mission global record invalid\n";
            return 6;
        }
        std::cout << "Native App mission source opened through SPMAPDIR; ParseData bytes: "
                  << parseBytes << "; schema parsed; Globals bytes: " << globalsBytes
                  << "; mission not started\n";
        InterfaceOwner<IStreamer> streamer;
        InterfaceOwner<IFileSystem> physicalMusic;
        IFileSystem *music = resolveSource("Music", physicalMusic);
        if (!music || GS->QueryInterface(IID_IStreamer,
                reinterpret_cast<void **>(streamer.Receive())) != GR_OK ||
            !streamer.Get() || !appAudioBinding.Bind(streamer.Get(), music))
        {
            std::cerr << "Native App mission music source or IStreamer unavailable\n";
            return 6;
        }
    }
    if (!options.animationObjectName.empty() || !options.meshObjectName.empty())
    {
        IFileSystem * objectRoot = OBJECTDIR;
        InterfaceOwner<IFileSystem> physicalObjectRoot;
        if (!objectRoot)
            for (const auto & mount : archiveMounts)
                if (mount.first == "Objects") objectRoot = mount.second->Get();
        if (!objectRoot)
            for (const auto & entry : startupRoot.entries)
                if (entry.expectedPath == "Objects" &&
                    entry.status == FrontierX::Platform::DataRootEntryStatus::Present)
                {
                    std::string error;
                    if (FrontierX::OpenNativeDirectoryFileSystem(
                            startupRoot.path / entry.actualPath,
                            physicalObjectRoot.Receive(), error))
                        objectRoot = physicalObjectRoot.Get();
                }
        if (!objectRoot)
        {
            std::cerr << "Native object root unavailable\n";
            return 6;
        }
        IEngine * engine = FrontierX::NativeGameSystemEngine(gameSystem.get());
        if (!options.animationObjectName.empty())
        {
            const auto animation = FrontierX::ValidateAnimationObject(
                engine, objectRoot, options.animationObjectName.c_str(),
                options.animationScriptName.c_str());
            if (animation.status != FrontierX::AnimationValidationStatus::Ok)
            {
                const char * message = animation.status == FrontierX::AnimationValidationStatus::OpenFailed ?
                    "Native animation object open failed" :
                    animation.status == FrontierX::AnimationValidationStatus::LoadFailed ?
                    "Native animation script-set load failed" :
                    "Native animation script validation failed";
                std::cerr << message << '\n';
                return 6;
            }
            std::cout << "Native animation script loaded; scripts: " << animation.scripts
                      << "; duration: " << animation.duration << " seconds\n";
        }
        if (!options.meshObjectName.empty())
        {
            unsigned int faceGroups = 0;
            if (!FrontierX::ValidateMeshObject(systemContainer, engine,
                    objectRoot, MATERIALDIR ? MATERIALDIR : materialSource.Get(),
                    options.meshObjectName.c_str(), faceGroups))
            {
                std::cerr << "Native mesh archetype load failed\n";
                return 6;
            }
            std::cout << "Native mesh instance created and destroyed; face groups: "
                      << faceGroups << '\n';
        }
    }
#else
    if (!options.animationObjectName.empty() || !options.meshObjectName.empty() ||
        !options.missionSourceName.empty())
    {
        std::cerr << "Native game data diagnostics require the GPU-enabled Engine build\n";
        return 6;
    }
#endif
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    bool particlePresent = false;
    if (!FrontierX::ValidateNativeParticleManager(systemContainer, particlePresent))
    {
        std::cerr << "Native production particle owner failed validation\n";
        return 3;
    }
    if (particlePresent)
        std::cout << "Native production particle owner: 29 filters; point-emitter program created\n";
#endif
    InterfaceOwner<IEventSystem> eventSystem;
    InterfaceOwner<ISystemComponent> eventSystemOwner;
    InterfaceOwner<IEventCallback> eventSystemCallback;
    if (systemContainer->QueryInterface(IID_IEventSystem,
        reinterpret_cast<void **>(eventSystem.Receive())) != GR_OK || !eventSystem.Get() ||
        systemContainer->QueryInterface(IID_ISystemComponent,
            reinterpret_cast<void **>(eventSystemOwner.Receive())) != GR_OK ||
        !eventSystemOwner.Get() ||
        systemContainer->QueryInterface(IID_IEventCallback,
            reinterpret_cast<void **>(eventSystemCallback.Receive())) != GR_OK ||
        !eventSystemCallback.Get() ||
        (!options.missionSourceName.empty() && EVENTSYS != eventSystem.Get()) ||
        eventSystem.Get()->Initialize(0) != GR_OK)
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

    InterfaceOwner<IWindowManager> windowManager;
    bool windowReady = systemContainer->QueryInterface(IID_IWindowManager,
        reinterpret_cast<void **>(windowManager.Receive())) == GR_OK && windowManager.Get();
#if !defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    if (windowReady)
        windowReady = windowManager.Get()->Startup(nullptr,
            "FrontierX native shell (game not started)", ShellExit,
            WMF_PREVENT_MULTIPLE, nullptr) == 1;
#endif
    if (!windowReady)
    {
        std::cerr << "Native window-manager startup failed\n";
        return 4;
    }
    if (!options.missionSourceName.empty() && WM != windowManager.Get())
    {
        std::cerr << "Native App window-manager identity mismatch\n";
        return 6;
    }
    shellEventSystem = eventSystem.Get();
    windowManager.Get()->SetCallback(ShellWindowCallback);
    if (!options.missionSourceName.empty())
    {
        STREAMERDESC streamerDescription;
        streamerDescription.hMainWindow = windowManager.Get()->GetWindowHandle();
        streamerDescription.uMsg = CQE_STREAMER;
        streamerDescription.readBufferTime = 4.0F;
        streamerDescription.soundBufferTime = 1.0F;
        if (!STREAMER || !STREAMER->Init(&streamerDescription))
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native App mission IStreamer device initialization failed\n";
            return 6;
        }
        std::cout << "Native App mission IStreamer initialized with Music source\n";
        InterfaceOwner<IMusicManager> musicManager;
        *musicManager.Receive() = FrontierX::CreateNativeMusicManager();
        if (!musicManager.Get() || !appMusicBinding.Bind(musicManager.Get()))
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native App music manager initialization failed\n";
            return 6;
        }
        std::cout << "Native App MusicManager initialized\n";
    }
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
    WM_WINAREA clientArea = {};
    windowManager.Get()->GetClientArea(clientArea);
    if (clientArea.w <= 0 || clientArea.h <= 0)
    {
        windowManager.Get()->SetCallback(nullptr);
        shellEventSystem = nullptr;
        std::cerr << "Native App client area is invalid\n";
        return 4;
    }
    SCREENRESX = static_cast<DWORD>(clientArea.w);
    SCREENRESY = static_cast<DWORD>(clientArea.h);
    SCREEN_WIDTH = 800;
    SCREEN_HEIGHT = 600;
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
    if (!options.gameMeshPath.empty())
    {
        const HWND token = windowManager.Get()->GetWindowHandle();
        FrontierX::SdlGpuReadback image;
        std::string sceneError;
        std::uint32_t sceneFrames = 0U;
        GameSceneLoop sceneLoop{windowManager.Get(), eventSystemOwner.Get(),
            FrontierX::Platform::MonotonicMilliseconds(), options.smokeMilliseconds};
        const bool rendered = token &&
            token <= std::numeric_limits<std::uint32_t>::max() &&
            FrontierX::RenderNativeGameScene(manager, gameSystem.get(),
                static_cast<std::uint32_t>(token), options.gameMaterialPath,
                options.gameMeshPath, options.vertexShaderPath.string().c_str(),
                options.fragmentShaderPath.string().c_str(),
                options.gameExtendedShaderPath.string().c_str(),
                options.gameVertexShaderPath.string().c_str(),
                ContinueGameScene, &sceneLoop, sceneFrames, image, sceneError);
        windowManager.Get()->SetCallback(nullptr);
        shellEventSystem = nullptr;
        if (!rendered)
        {
            std::cerr << "Native game scene failed: " << sceneError << '\n';
            return 7;
        }
        std::cout << "Native game scene presented; frames: " << sceneFrames
                  << "; changed-pixels: "
                  << image.changedPixels << "; pixel-checksum: "
                  << image.checksum << '\n';
        if (!options.capturePpmPath.empty())
        {
            std::ofstream capture(options.capturePpmPath, std::ios::binary);
            capture << "P6\n" << FrontierX::SdlGpuReadback::Width << ' '
                    << FrontierX::SdlGpuReadback::Height << "\n255\n";
            capture.write(reinterpret_cast<const char *>(image.rgb8.data()),
                static_cast<std::streamsize>(image.rgb8.size()));
            capture.close();
            if (!capture)
            {
                std::cerr << "Native game-scene capture write failed\n";
                return 7;
            }
        }
        return 0;
    }
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
            FrontierX::SdlGpuRenderer::TextureBindCallback,
            FrontierX::SdlGpuRenderer::ModelviewCallback,
            FrontierX::SdlGpuRenderer::ProjectionCallback,
            nullptr, nullptr, FrontierX::SdlGpuRenderer::SamplerStateCallback);
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
        FrontierX::SdlGpuReadback indexedReadback;
        FrontierX::SdlGpuReadback pooledReadback;
        InterfaceOwner<ICQBatch> poolBatch;
        if (!renderer.Readback64(indexedReadback, renderError) ||
            renderBatcher.Get()->QueryInterface(IID_ICQBatch,
                reinterpret_cast<void **>(poolBatch.Receive())) != GR_OK ||
            !poolBatch.Get())
        {
            std::cerr << "Native render-pool setup failed\n";
            return 7;
        }
        poolBatch.Get()->SetIdentityModelview();
        poolBatch.Reset();
        if (renderBatcher.Get()->set_state(RPR_STATE_FLAGS, 0U) != GR_OK ||
            renderBatcher.Get()->set_state(RPR_STATE_ID, 1U) != GR_OK ||
            renderBatcher.Get()->set_state(RPR_BATCH, 1U) != GR_OK ||
            FrontierX::SubmitIndexedRenderSmokeScene(renderBatcher.Get()) != GR_OK ||
            renderBatcher.Get()->flush(U32(-1)) != GR_OK ||
            !renderer.Readback64(pooledReadback, renderError) ||
            pooledReadback.checksum != indexedReadback.checksum ||
            pooledReadback.changedPixels != indexedReadback.changedPixels)
        {
            std::cerr << "Native render-pool submission failed\n";
            return 7;
        }
        renderBatcher.Reset();
        PIPE = nullptr;
        std::cout << "Native render smoke initialized; backend: "
                  << renderer.DriverName()
                  << "; source: PrimitiveBuilder, indexed and pooled fixture via production "
                     "RenderBatcher and IRenderPipeline; pooled checksum: "
                  << pooledReadback.checksum << '\n';
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
        if (!options.capturePpmPath.empty())
        {
            if (!readback.rgbValid)
            {
                std::cerr << "Native render capture has an unsupported color format\n";
                return 7;
            }
            std::ofstream capture(options.capturePpmPath, std::ios::binary);
            capture << "P6\n" << FrontierX::SdlGpuReadback::Width << ' '
                    << FrontierX::SdlGpuReadback::Height << "\n255\n";
            capture.write(reinterpret_cast<const char *>(readback.rgb8.data()),
                static_cast<std::streamsize>(readback.rgb8.size()));
            capture.close();
            if (!capture)
            {
                std::cerr << "Native render capture write failed\n";
                return 7;
            }
            std::cout << "Native render fixture captured: " << options.capturePpmPath << '\n';
        }
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
        std::cout << "Native shell closed cleanly; retail game startup and rendering were not attempted\n";
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
