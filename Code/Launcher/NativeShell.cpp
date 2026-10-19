// The legacy IArchetypeList overloads hide IDAComponent refcount methods.
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
#include "CQTrace.h"
extern bool NativeAppCheckAnimRandomization();
#endif
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
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
#include "FrontierX/NativeVideoSystem.h"
#include "VideoSystem.h"
U32 __stdcall MovieScreen(struct Frame *parent, const char *filename);
#endif
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
#include "IGameProgress.h"
#include "ISubtitle.h"
#include "StatusBar.h"
#include "Hintbox.h"
#include "MScroll.h"
#include "ScrollingText.h"
#include "resource.h"
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppFont.h"
#include "Include/NativeAppSfx.h"
#include "Include/NativeAppProfileParser.h"
#include "SFX.h"
#include "Include/NativeAppSchema.h"
#include "Include/NativeAppTextResources.h"
#include "Include/NativeAppImage.h"
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
#include "BaseHotRect.h"
#include "GenData.h"
#include <DMenu1.h>
struct Frame;
bool CheckNativeGameProgressPersistence(IFileSystem *directory);
bool CheckNativeUnitCatalog(IArchetypeList *objectList);
bool CheckNativeUnitCatalogMetadata();
U32 __stdcall DoMenu_Briefing (Frame * parent, const char * szFileName);
U32 __stdcall CreateMenuEndGame(bool won);
U32 __stdcall DoMenu_campaign(Frame *parent, const GT_MENU1 &data, bool skip);
#endif
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
namespace FrontierX {
bool CheckNativeTerranDroneArrayRelease();
bool CheckNativeSpiderDroneArrayRelease();
bool CheckNativeAnimationBorder();
}
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
#include "rendpipeline.h"
#include "renderer.h"
#include "ICamera.h"
#include "IVertexBufferManager.h"
#include "ITextureLibrary.h"
#include "IMaterialManager.h"
#include "IMeshManager.h"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnontrivial-memcall"
#endif
#include "IParticleManager.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include "IAnim.h"
#include "IHardPoint.h"
#include "TManager.h"
#include "StringTable.h"
#include "lightman.h"
#include "RenderBatcherRegistration.h"
#include "Include/NativeAppMissionMedia.h"
#include "SoundManager.h"
#include "StringData.h"
#include "ITeletype.h"
#include "UnitComm.h"
#include "ObjList.h"
#include <DMBaseData.h>
#include "Mission.h"
#include "IObject.h"
#include "Sector.h"
#include "Camera.h"
#include "MGlobals.h"
#endif

#include <charconv>
#include <cstring>
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
constexpr U32 NativeFlushMessagesEvent = 0x1044U; // Globals.h CQE_FLUSHMESSAGES
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

struct AppHotkeyGlobal
{
    ~AppHotkeyGlobal() { HOTKEY = nullptr; }
};

struct AppDefaultsGlobal
{
    ~AppDefaultsGlobal() { DEFAULTS = nullptr; }
};

#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
struct AppEngineGlobals
{
    IComponentFactory *previousSearchPath = nullptr;
    bool objectSearchBound = false;
    USER_DEFAULTS *subtitleDefaults = nullptr;
    bool savedSubtitles = false;
    void EnableSubtitles()
    {
        subtitleDefaults = DEFAULTS->GetDefaults();
        savedSubtitles = subtitleDefaults->bSubtitles;
        subtitleDefaults->bSubtitles = 1;
    }
    ~AppEngineGlobals()
    {
        if (objectSearchBound && ENGINE)
            ENGINE->set_search_path2(previousSearchPath);
        if (previousSearchPath) previousSearchPath->Release();
        if (subtitleDefaults) subtitleDefaults->bSubtitles = savedSubtitles;
        ENGINE = nullptr;
        REND = nullptr;
        PIPE = nullptr;
        BATCH = nullptr;
        SOUNDMANAGER = nullptr;
        UNITCOMM = nullptr;
        TELETYPE = nullptr;
        SUBTITLE = nullptr;
        CQBATCH = nullptr;
        LIGHT = nullptr;
        TEXLIB = nullptr;
        MATMAN = nullptr;
        MESHMAN = nullptr;
        TMANAGER = nullptr;
        VB_MANAGER = nullptr;
        STRINGTABLE = nullptr;
        STRINGDATA = nullptr;
    }
};

// Conquest.cpp::Cleanup closes the original App owners while their dependencies
// and the rendering backend are still bound. Never let GenData teardown run
// after GENDATA has been cleared with live UI archetypes.
struct AppGlobalCleanup
{
    bool created = false;
    bool vertexBuffersInitialized = false;
    ~AppGlobalCleanup()
    {
        if (!created)
        {
            if (vertexBuffersInitialized && VB_MANAGER) VB_MANAGER->cleanup();
            return;
        }
        CQFLAGS.bGameActive = 0;
        FrontierX::LegacyWindowMessage close{WM ? WM->GetWindowHandle() : HWND{},
            FrontierX::LegacyMessage::Close, 0, 0};
        if (EVENTSYS) EVENTSYS->Send(FrontierX::LegacyMessage::Close, &close);
        if (FULLSCREEN) FULLSCREEN->Notify(CQE_UPDATE, nullptr);
        if (MISSION) MISSION->Close(TRUE);
        CleanupGlobals();
        if (GENDATA) GENDATA->FlushUnusedArchetypes();
        if (vertexBuffersInitialized && VB_MANAGER && VB_MANAGER->cleanup() != GR_OK)
            std::cerr << "Native mission vertex-buffer cleanup failed\n";
    }
};

struct MissionGpuUnbind
{
    IRenderPrimitive * primitive = nullptr;
    IRenderPipeline * pipeline = nullptr;
    bool appBuilderBound = false;

    void Disconnect()
    {
        if (appBuilderBound) BindNativeAppPrimitive(nullptr);
        if (pipeline) FrontierX::UnbindNativeRenderPipeline(pipeline);
        if (primitive) FrontierX::UnbindNativeRenderPrimitive(primitive);
        pipeline = nullptr;
        primitive = nullptr;
        appBuilderBound = false;
    }
    ~MissionGpuUnbind() { Disconnect(); }
};
#endif

IEventSystem * shellEventSystem = nullptr;
bool shellCloseRequested = false;

#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
struct NativeMissionModalExit final : IEventCallback
{
    GENRESULT COMAPI QueryInterface(const C8 *name, void **instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!name || (std::strcmp(name, "IEventCallback") &&
                      std::strcmp(name, IID_IEventCallback)))
            return GR_INTERFACE_UNSUPPORTED;
        *instance = static_cast<IEventCallback *>(this);
        AddRef();
        return GR_OK;
    }
    U32 COMAPI AddRef() override { return ++references; }
    U32 COMAPI Release() override { return references ? --references : 0; }

    U32 references = 0;
    std::uint32_t started = 0;
    std::uint32_t duration = 0;
    unsigned int frames = 0;
    bool frameStats = false;
    std::uint32_t statsStarted = 0;
    unsigned int statsFrames = 0;
    FrontierX::SdlGpuRenderer * timingRenderer = nullptr;
    FrontierX::SdlGpuRenderer::Timings lastTimings{};
    bool requested = false;
    U32 observedObjectives = 0;
    U32 observedCompletedObjectives = 0;

    GENRESULT COMAPI Notify(U32 message, void *parameter) override
    {
        if (message == CQE_HOTKEY)
            std::cout << "Native App hotkey event "
                      << reinterpret_cast<std::uintptr_t>(parameter) << '\n' << std::flush;
        if (message != CQE_ENDFRAME) return GR_OK;
        ++frames;
        if (frameStats)
        {
            const auto now = FrontierX::Platform::MonotonicMilliseconds();
            if (!statsStarted) { statsStarted = now; statsFrames = frames; }
            const auto elapsed = now - statsStarted;
            if (elapsed >= 5000U)
            {
                const double count = frames - statsFrames;
                std::cout << "Native App frame stats: frames=" << frames - statsFrames
                          << "; milliseconds=" << elapsed
                          << "; fps=" << (frames - statsFrames) * 1000.0 / elapsed
                          << '\n' << std::flush;
                if (timingRenderer && count)
                {
                    const auto current = timingRenderer->GetTimings();
                    std::cout << "Native App backend ms/frame: submit=" << (current.submissions - lastTimings.submissions) / count
                              << "; sampler=" << (current.samplers - lastTimings.samplers) / count
                              << "; retire=" << (current.retirement - lastTimings.retirement) / count
                              << "; present=" << (current.presentation - lastTimings.presentation) / count
                              << "; draws=" << (current.submissionCount - lastTimings.submissionCount) / count
                              << "; sampler-changes=" << (current.samplerChanges - lastTimings.samplerChanges) / count
                              << "; acquire=" << (current.acquisition - lastTimings.acquisition) / count
                              << "; draw=" << (current.drawing - lastTimings.drawing) / count
                              << "; queue=" << (current.queueSubmit - lastTimings.queueSubmit) / count
                              << "; reused=" << (current.reusedUploads - lastTimings.reusedUploads) / count
                              << "; texture-upload=" << (current.textureUploads - lastTimings.textureUploads) / count
                              << "; texture-updates=" << (current.textureUploadCount - lastTimings.textureUploadCount) / count
                              << '\n' << std::flush;
                    lastTimings = current;
                }
                statsStarted = now;
                statsFrames = frames;
            }
        }
        // Sol_T/resource.h: original objective IDs 46..69. Observation only.
        U32 listed = 0, completed = 0;
        for (U32 index = 0; index < 24; ++index)
        {
            const U32 bit = 1U << index;
            if (MGlobals::IsObjectiveInList(46U + index)) listed |= bit;
            if (MGlobals::IsObjectiveCompleted(46U + index)) completed |= bit;
            if ((listed & bit) != (observedObjectives & bit) ||
                (completed & bit) != (observedCompletedObjectives & bit))
                std::cout << "Native training objective " << index + 1
                          << ": listed=" << ((listed & bit) != 0)
                          << " completed=" << ((completed & bit) != 0) << '\n' << std::flush;
        }
        observedObjectives = listed;
        observedCompletedObjectives = completed;
        if (!requested && (shellCloseRequested ||
            (duration && FrontierX::Platform::MonotonicMilliseconds() - started >= duration)))
        {
            requested = EVENTSYS->Post(CQE_PLAYER_QUIT) == GR_OK;
            if (!requested) return GR_GENERIC;
        }
        return GR_OK;
    }
};

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

    if (message == FrontierX::LegacyMessage::KeyDown ||
        message == FrontierX::LegacyMessage::KeyUp)
        std::cout << "Native App SDL key " << message << " virtual "
                  << wparam << '\n' << std::flush;

    if (message == FrontierX::LegacyMessage::LeftButtonDown ||
        message == FrontierX::LegacyMessage::LeftButtonUp ||
        message == FrontierX::LegacyMessage::RightButtonDown ||
        message == FrontierX::LegacyMessage::RightButtonUp)
        std::cout << "Native App SDL mouse button " << message << " at "
                  << (lparam & 0xffff) << ',' << ((lparam >> 16) & 0xffff)
                  << '\n' << std::flush;

    if (message == FrontierX::LegacyMessage::Character)
        shellEventSystem->Send(FrontierX::LegacyMessage::UnicodeCharacter,
            reinterpret_cast<void *>(wparam & 0xffffU));
    FrontierX::LegacyWindowMessage record{
        static_cast<FrontierX::LegacyWindowToken>(window), message, wparam, lparam};
    shellEventSystem->Send(message, &record);
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    if ((message == FrontierX::LegacyMessage::LeftButtonUp ||
         message == FrontierX::LegacyMessage::RightButtonUp) && OBJLIST)
        std::cout << "Native App mouse dispatch highlighted="
                  << (OBJLIST->GetHighlightedList() ? "yes" : "no")
                  << " selected=" << (OBJLIST->GetSelectedList() ? "yes" : "no")
                  << '\n' << std::flush;
#endif
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
    std::filesystem::path stringPackPath;
    std::filesystem::path uiFontPath;
    std::filesystem::path uiFontMappingPath;
    std::filesystem::path fontSmokePath;
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
    std::string movieSourceName;
    std::string profileSmokeName;
    std::uint32_t smokeMilliseconds = 0;
    bool noFrameLimit = false;
    bool frameStats = false;
    bool hidden = false;
    bool skipMovies = false;
    bool skipNotificationCue = false;
    bool skipSectorMap = false;
    bool skipSpaceBackground = false;
    bool skipUnitLabels = false;
    bool skipPlanetUi = false;
    bool skipAnimatedPortraits = false;
    bool skipSubtitles = false;
    bool baseTexturesOnly = false;
    bool skipLegacyLighting = false;
    bool skipTrainingSpeech = false;
    bool skipUnitVoice = false;
    bool skipUnitAlerts = false;
    bool missionAlertSmoke = false;
    bool missionHintCheck = false;
    bool skipMissionSfx = false;
    bool skipTeletypeVisuals = false;
    bool skipMissionMusic = false;
    bool sfxSmoke = false;
    bool missionEngine = false;
    bool missionGpu = false;
    bool missionStart = false;
    bool unitCatalogCheck = false;
    bool missionBriefing = false;
    bool missionCampaign = false;
    bool HasUiFont() const { return !uiFontPath.empty() || !uiFontMappingPath.empty(); }
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
                         "[--data-root DIRECTORY] [--material-root DIRECTORY] [--save-root DIRECTORY] [--gen-data FILE] [--game-types FILE] [--string-pack FILE] [--ui-font FILE | --ui-font-map FILE] [--font-smoke FILE] [--hotkeys FILE] "
                         "[--render-smoke VERTEX_SPV FRAGMENT_SPV] "
                         "[--game-scene MATERIAL_SOURCE MESH_UTF VERTEX_SPV "
                         "FRAGMENT_SPV EXTENDED_SPV GAME_SPV] "
                         "[--mission-gpu VERTEX_SPV FRAGMENT_SPV EXTENDED_SPV GAME_SPV] "
                         "[--capture-ppm FILE] [--skip-movies] [--skip-notification-cue] [--skip-sector-map] [--skip-space-background] [--skip-unit-labels] [--skip-planet-ui] [--skip-animated-portraits] [--skip-subtitles] [--base-textures-only] [--skip-legacy-lighting] [--skip-training-speech] [--skip-unit-voice] [--skip-unit-alerts] [--mission-alert-smoke] [--mission-hint-check] [--skip-mission-sfx] [--skip-teletype-visuals] [--skip-mission-music] [--sfx-smoke] [--no-frame-limit] [--frame-stats] [--profile-smoke FILE] [--mission-engine] [--mission-start] [--unit-catalog-check] [--mission-briefing] [--mission-campaign] "
                         "[--animation-object OBJECT_NAME SCRIPT_NAME] "
                         "[--mesh-object OBJECT_NAME] "
                         "[--movie-source FILE (separate optional movie diagnostic)] "
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
        if (option == "--skip-sector-map")
        {
            if (options.skipSectorMap)
                throw std::invalid_argument("duplicate option: --skip-sector-map");
            options.skipSectorMap = true;
            continue;
        }
        if (option == "--skip-space-background")
        {
            if (options.skipSpaceBackground)
                throw std::invalid_argument("duplicate option: --skip-space-background");
            options.skipSpaceBackground = true;
            continue;
        }
        if (option == "--skip-unit-labels")
        {
            if (options.skipUnitLabels)
                throw std::invalid_argument("duplicate option: --skip-unit-labels");
            options.skipUnitLabels = true;
            continue;
        }
        if (option == "--skip-planet-ui")
        {
            if (options.skipPlanetUi)
                throw std::invalid_argument("duplicate option: --skip-planet-ui");
            options.skipPlanetUi = true;
            continue;
        }
        if (option == "--skip-animated-portraits")
        {
            if (options.skipAnimatedPortraits)
                throw std::invalid_argument("duplicate option: --skip-animated-portraits");
            options.skipAnimatedPortraits = true;
            continue;
        }
        if (option == "--skip-subtitles")
        {
            if (options.skipSubtitles)
                throw std::invalid_argument("duplicate option: --skip-subtitles");
            options.skipSubtitles = true;
            continue;
        }
        if (option == "--base-textures-only")
        {
            if (options.baseTexturesOnly)
                throw std::invalid_argument("duplicate option: --base-textures-only");
            options.baseTexturesOnly = true;
            continue;
        }
        if (option == "--skip-legacy-lighting")
        {
            if (options.skipLegacyLighting)
                throw std::invalid_argument("duplicate option: --skip-legacy-lighting");
            options.skipLegacyLighting = true;
            continue;
        }
        if (option == "--skip-training-speech")
        {
            if (options.skipTrainingSpeech)
                throw std::invalid_argument("duplicate option: --skip-training-speech");
            options.skipTrainingSpeech = true;
            continue;
        }
        if (option == "--skip-mission-sfx")
        {
            if (options.skipMissionSfx) throw std::invalid_argument("duplicate option: --skip-mission-sfx");
            options.skipMissionSfx = true;
            continue;
        }
        if (option == "--mission-hint-check")
        {
            if (options.missionHintCheck) throw std::invalid_argument("duplicate option: --mission-hint-check");
            options.missionHintCheck = true;
            continue;
        }
        if (option == "--mission-alert-smoke")
        {
            if (options.missionAlertSmoke) throw std::invalid_argument("duplicate option: --mission-alert-smoke");
            options.missionAlertSmoke = true;
            continue;
        }
        if (option == "--skip-unit-alerts")
        {
            if (options.skipUnitAlerts) throw std::invalid_argument("duplicate option: --skip-unit-alerts");
            options.skipUnitAlerts = true;
            continue;
        }
        if (option == "--skip-unit-voice")
        {
            if (options.skipUnitVoice)
                throw std::invalid_argument("duplicate option: --skip-unit-voice");
            options.skipUnitVoice = true;
            continue;
        }
        if (option == "--skip-teletype-visuals")
        {
            if (options.skipTeletypeVisuals)
                throw std::invalid_argument("duplicate option: --skip-teletype-visuals");
            options.skipTeletypeVisuals = true;
            continue;
        }
        if (option == "--skip-mission-music")
        {
            if (options.skipMissionMusic)
                throw std::invalid_argument("duplicate option: --skip-mission-music");
            options.skipMissionMusic = true;
            continue;
        }
        if (option == "--sfx-smoke")
        {
            if (options.sfxSmoke) throw std::invalid_argument("duplicate option: --sfx-smoke");
            options.sfxSmoke = true;
            continue;
        }
        if (option == "--mission-engine")
        {
            if (options.missionEngine)
                throw std::invalid_argument("duplicate option: --mission-engine");
            options.missionEngine = true;
            continue;
        }
        if (option == "--no-frame-limit")
        {
            if (options.noFrameLimit)
                throw std::invalid_argument("duplicate option: --no-frame-limit");
            options.noFrameLimit = true;
            continue;
        }
        if (option == "--frame-stats")
        {
            if (options.frameStats)
                throw std::invalid_argument("duplicate option: --frame-stats");
            options.frameStats = true;
            continue;
        }
        if (option == "--mission-start")
        {
            if (options.missionStart)
                throw std::invalid_argument("duplicate option: --mission-start");
            options.missionStart = true;
            continue;
        }
        if (option == "--unit-catalog-check")
        {
            if (options.unitCatalogCheck)
                throw std::invalid_argument("duplicate option: --unit-catalog-check");
            options.unitCatalogCheck = true;
            continue;
        }
        if (option == "--mission-briefing")
        {
            if (options.missionBriefing)
                throw std::invalid_argument("duplicate option: --mission-briefing");
            options.missionBriefing = true;
            continue;
        }
        if (option == "--mission-campaign")
        {
            if (options.missionCampaign)
                throw std::invalid_argument("duplicate option: --mission-campaign");
            options.missionCampaign = true;
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
        if (option == "--mission-gpu")
        {
            if (options.missionGpu || !options.vertexShaderPath.empty() || index + 4 >= argc)
                throw std::invalid_argument("--mission-gpu requires four SPIR-V files and no other render mode");
            for (int offset = 1; offset <= 4; ++offset)
                if (!argv[index + offset][0] ||
                    std::string(argv[index + offset]).rfind("--", 0) == 0)
                    throw std::invalid_argument("--mission-gpu requires four SPIR-V files");
            options.vertexShaderPath = argv[++index];
            options.fragmentShaderPath = argv[++index];
            options.gameExtendedShaderPath = argv[++index];
            options.gameVertexShaderPath = argv[++index];
            options.missionGpu = true;
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
            bool validObjectPath = !objectName.empty() &&
                objectName.rfind("--", 0) != 0 && objectName.front() != '/' &&
                objectName.find_first_of("\\:") == std::string::npos;
            for (std::size_t begin = 0; validObjectPath && begin <= objectName.size();)
            {
                const std::size_t end = objectName.find('/', begin);
                const std::string component = objectName.substr(begin,
                    end == std::string::npos ? end : end - begin);
                validObjectPath = !component.empty() && component != "." && component != "..";
                if (end == std::string::npos)
                    break;
                begin = end + 1;
            }
            if (!validObjectPath)
                throw std::invalid_argument("--mesh-object requires one relative object path");
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
        if (option == "--movie-source")
        {
#if !defined(FRONTIERX_HAVE_VIDEO_DECODER)
            throw std::invalid_argument("--movie-source requires the optional native movie build");
#else
            if (!options.movieSourceName.empty() || index + 1 >= argc)
                throw std::invalid_argument("--movie-source requires one movie filename");
            const std::string name(argv[++index]);
            if (name.empty() || name.rfind("--", 0) == 0 ||
                name.find_first_of("/\\:") != std::string::npos || name == "." || name == "..")
                throw std::invalid_argument("--movie-source requires one movie filename");
            options.movieSourceName = name;
            continue;
#endif
        }
        if (option == "--profile-smoke")
        {
            if (!options.profileSmokeName.empty() || index + 1 >= argc)
                throw std::invalid_argument("--profile-smoke requires one profile name");
            const std::string name(argv[++index]);
            if (name.empty() || name.rfind("--", 0) == 0 ||
                name.find_first_of("/\\") != std::string::npos ||
                name == "." || name == "..")
                throw std::invalid_argument("--profile-smoke requires one profile name");
            options.profileSmokeName = name;
            continue;
        }
        if ((option != "--config" && option != "--system-profile" &&
             option != "--preferences" && option != "--hotkeys" &&
             option != "--data-root" && option != "--material-root" && option != "--save-root" && option != "--gen-data" && option != "--game-types" && option != "--string-pack" && option != "--ui-font" && option != "--ui-font-map" && option != "--font-smoke" && option != "--smoke-ms" &&
             option != "--capture-ppm") ||
            index + 1 == argc)
            throw std::invalid_argument(
                "expected --config, --system-profile, --preferences, --data-root, --material-root, --save-root, --gen-data, --game-types, --string-pack, --ui-font, --ui-font-map, --font-smoke, --hotkeys, --smoke-ms, "
                "--capture-ppm, --animation-object, --mesh-object, --mission-source, --mission-engine, --mission-gpu, --mission-start, --skip-movies, --skip-notification-cue, --skip-sector-map, --skip-space-background, --skip-unit-labels, --skip-planet-ui, --skip-animated-portraits, --skip-subtitles, --base-textures-only, --skip-legacy-lighting, --skip-training-speech, --skip-unit-voice, --skip-unit-alerts, --mission-alert-smoke, --mission-hint-check, --skip-mission-sfx, --skip-teletype-visuals, --skip-mission-music, --sfx-smoke or --hidden");
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
        else if (option == "--font-smoke")
        {
            if (!options.fontSmokePath.empty()) throw std::invalid_argument("duplicate option: --font-smoke");
            options.fontSmokePath = value;
        }
        else if (option == "--ui-font")
        {
            if (options.HasUiFont()) throw std::invalid_argument("duplicate option: --ui-font");
            options.uiFontPath = value;
        }
        else if (option == "--ui-font-map")
        {
            if (options.HasUiFont()) throw std::invalid_argument("duplicate or conflicting font option");
            options.uiFontMappingPath = value;
        }
        else if (option == "--string-pack")
        {
            if (!options.stringPackPath.empty()) throw std::invalid_argument("duplicate option: --string-pack");
            options.stringPackPath = value;
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
    if (options.noFrameLimit && !options.missionStart)
        throw std::invalid_argument("--no-frame-limit requires --mission-start");
    if (options.frameStats && !options.missionStart)
        throw std::invalid_argument("--frame-stats requires --mission-start");
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
    if (options.unitCatalogCheck && (!options.missionStart || options.missionCampaign ||
        options.missionBriefing || !options.movieSourceName.empty()))
        throw std::invalid_argument("--unit-catalog-check requires exclusive --mission-start without campaign/briefing/movie");
    if (options.missionBriefing && (!options.missionStart || options.hidden))
        throw std::invalid_argument("--mission-briefing requires visible --mission-start");
    if (options.missionCampaign && (!options.missionStart || options.hidden ||
        options.missionBriefing || (options.missionSourceName != "Sol_Train.dmission" &&
        options.missionSourceName != "T_Mission1.dmission")))
        throw std::invalid_argument("--mission-campaign requires visible Sol_Train or T_Mission1 --mission-start and excludes --mission-briefing");
    if (options.skipSectorMap && !options.missionStart)
        throw std::invalid_argument("--skip-sector-map requires --mission-start");
    if (options.skipSpaceBackground && !options.missionStart)
        throw std::invalid_argument("--skip-space-background requires --mission-start");
    if (options.skipUnitLabels && !options.missionStart)
        throw std::invalid_argument("--skip-unit-labels requires --mission-start");
    if (options.skipPlanetUi && !options.missionStart)
        throw std::invalid_argument("--skip-planet-ui requires --mission-start");
    if (options.skipAnimatedPortraits && !options.missionStart)
        throw std::invalid_argument("--skip-animated-portraits requires --mission-start");
    if (options.skipSubtitles && !options.missionStart)
        throw std::invalid_argument("--skip-subtitles requires --mission-start");
    if (options.baseTexturesOnly && !options.missionStart)
        throw std::invalid_argument("--base-textures-only requires --mission-start");
    if (options.skipLegacyLighting && !options.missionStart)
        throw std::invalid_argument("--skip-legacy-lighting requires --mission-start");
    if (options.skipTrainingSpeech && !options.missionStart)
        throw std::invalid_argument("--skip-training-speech requires --mission-start");
    if (options.skipUnitVoice && !options.missionStart)
        throw std::invalid_argument("--skip-unit-voice requires --mission-start");
    if (options.skipTeletypeVisuals && !options.missionStart)
        throw std::invalid_argument("--skip-teletype-visuals requires --mission-start");
    if (options.skipMissionSfx && !options.missionStart)
        throw std::invalid_argument("--skip-mission-sfx requires --mission-start");
    if (options.missionHintCheck && (!options.missionStart || !options.HasUiFont()))
        throw std::invalid_argument("--mission-hint-check requires a font-enabled mission");
    if (options.missionAlertSmoke && (!options.missionStart || options.skipUnitVoice || options.skipUnitAlerts ||
        options.skipTeletypeVisuals || !options.HasUiFont()))
        throw std::invalid_argument("--mission-alert-smoke requires a font-enabled mission with unit voice, alerts and teletype");
    if (options.skipUnitAlerts && !options.missionStart)
        throw std::invalid_argument("--skip-unit-alerts requires --mission-start");
    if (!options.stringPackPath.empty() && !options.missionStart)
        throw std::invalid_argument("--string-pack requires --mission-start");
    if (options.missionStart && !options.skipUnitVoice &&
        options.stringPackPath.empty())
        throw std::invalid_argument("native unit voice requires --string-pack");
    if (options.missionStart && !options.skipUnitVoice && !options.skipUnitAlerts && !options.HasUiFont())
        throw std::invalid_argument("native unit alerts require --ui-font or explicit --skip-unit-alerts");
    if (options.missionStart && !options.skipSubtitles && !options.HasUiFont())
        throw std::invalid_argument("native source subtitles require --ui-font or explicit --skip-subtitles");
    if (options.missionStart && !options.skipTeletypeVisuals && !options.HasUiFont())
        throw std::invalid_argument("native source teletype requires --ui-font or explicit --skip-teletype-visuals");
    if (!options.fontSmokePath.empty() && (!options.sfxSmoke || options.missionStart))
        throw std::invalid_argument("--font-smoke requires --sfx-smoke without --mission-start");
    if (options.HasUiFont() && !options.missionStart &&
        (options.uiFontMappingPath.empty() || options.fontSmokePath.empty()))
        throw std::invalid_argument("UI font requires --mission-start or a mapped --font-smoke");
    if (options.skipMissionMusic && !options.missionStart)
        throw std::invalid_argument("--skip-mission-music requires --mission-start");
    if (options.sfxSmoke && options.missionSourceName.empty())
        throw std::invalid_argument("--sfx-smoke requires --mission-source");
    if (options.missionEngine &&
        (options.missionSourceName.empty() || options.systemProfilePath.empty()))
        throw std::invalid_argument("--mission-engine requires --mission-source and --system-profile");
    if (options.missionGpu && (!options.missionEngine || options.hidden))
        throw std::invalid_argument("--mission-gpu requires --mission-engine and a visible window");
    if (options.missionGpu && !options.capturePpmPath.empty())
        throw std::invalid_argument("--capture-ppm is only available for a render fixture");
    if (options.missionStart &&
        (!options.missionGpu || options.genDataPath.empty() ||
         options.gameTypesPath.empty() || options.saveRoot.empty() ||
         (!options.skipUnitLabels && !options.HasUiFont())))
        throw std::invalid_argument("--mission-start requires --mission-gpu, --gen-data, --game-types, --save-root and either --ui-font or --skip-unit-labels");
    if (!options.profileSmokeName.empty() && options.missionSourceName.empty())
        throw std::invalid_argument("--profile-smoke requires --mission-source");
    if (!options.movieSourceName.empty() &&
        (!options.missionStart || options.skipMovies || options.missionCampaign || options.missionBriefing))
        throw std::invalid_argument("--movie-source requires --mission-start without movie exclusion or campaign/briefing entry");
    if (!options.movieSourceName.empty() && options.smokeMilliseconds)
        throw std::invalid_argument("--movie-source uses original EOF/Esc/close; --smoke-ms applies to mission or render smokes");
    if (!options.saveRoot.empty() && options.missionSourceName.empty())
        throw std::invalid_argument("--save-root requires --mission-source");
    return options;
}

int Run(int argc, char ** argv)
{
    const Options options = ParseOptions(argc, argv);
    struct TexturePolicyRestore
    {
        bool previous;
        ~TexturePolicyRestore() { FrontierX::SetNativeGameTextureBaseLevelOnly(previous); }
    } texturePolicyRestore{FrontierX::SetNativeGameTextureBaseLevelOnly(options.baseTexturesOnly)};
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
    AppDefaultsGlobal appDefaultsGlobal;
    struct FrameLimitRestore
    {
        USER_DEFAULTS * defaults;
        U32 saved;
        bool active;
        void Restore()
        {
            if (active) defaults->bNoFrameLimit = saved;
            active = false;
        }
        ~FrameLimitRestore() { Restore(); }
    } frameLimitRestore{userDefaults.Get()->GetDefaults(),
        userDefaults.Get()->GetDefaults()->bNoFrameLimit, options.noFrameLimit};
    if (options.missionStart)
    {
        if (DEFAULTS)
        {
            std::cerr << "Native App user-defaults owner already bound\n";
            return 3;
        }
        DEFAULTS = userDefaults.Get();
        if (options.noFrameLimit)
        {
            if (DEFAULTS->GetDefaults()->gameSpeed != 0)
            {
                std::cerr << "--no-frame-limit requires original normal game speed (0); "
                             "the original option bypasses the game-speed multiplier\n";
                return 2;
            }
            // Original Windows menu option; keep its source timing semantics.
            DEFAULTS->GetDefaults()->bNoFrameLimit = 1;
            std::cout << "Native App original No Frame limiting enabled for this launch; "
                         "original elapsed-time and game-speed policy applies\n";
        }
    }
    std::cout << "Native user defaults initialized before GameSystem startup\n";
    FrontierX::RegisterNativeWindowManager(manager);
    if (!FrontierX::RegisterNativeStreamer(manager))
        throw std::runtime_error("native streamer registration failed");
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
    if (!FrontierX::RegisterNativeVideoSystem(manager))
        throw std::runtime_error("native video registration failed");
#endif
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
    CQFLAGS.bNoGDI = options.skipSectorMap ? 1 : 0;
    // Original toolbar is now linked independently of the excluded sector-map owner.
    CQFLAGS.bNoToolbar = 0;
    if (options.skipSpaceBackground)
    {
        CQEFFECTS.bBackground = GlobalEffectsOptions::off;
        CQEFFECTS.bExpensiveTerrain = GlobalEffectsOptions::off;
        std::cout << "Native optional space background explicitly disabled\n";
    }
    NATIVE_SKIP_UNIT_LABELS = options.skipUnitLabels;
    if (options.skipUnitLabels)
        std::cout << "Native optional unit labels explicitly disabled\n";
    NATIVE_SKIP_PLANET_UI = options.skipPlanetUi;
    if (options.skipPlanetUi)
        std::cout << "Native optional planet resource panel explicitly disabled\n";
    NATIVE_SKIP_SUBTITLES = options.skipSubtitles;
    if (options.skipSubtitles)
        std::cout << "Native optional speech subtitles explicitly disabled\n";
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
    FrontierX::NativeAppParticleBinding appParticleBinding;
    FrontierX::NativeAppTextBinding appTextBinding;
    FrontierX::NativeAppSaveBinding appSaveBinding;
    FrontierX::NativeAppAudioBinding appAudioBinding;
    FrontierX::NativeAppMusicBinding appMusicBinding;
    FrontierX::NativeAppSfxBinding appSfxBinding;
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    InterfaceOwner<IRenderer> appRenderer;
    InterfaceOwner<IRenderPipeline> appPipeline;
    InterfaceOwner<IRenderPrimitive> appPrimitive;
    InterfaceOwner<IRenderPrimitive> appRenderBatcher;
    InterfaceOwner<ICQBatch> appPoolBatch;
    InterfaceOwner<ILightManager> appLight;
    InterfaceOwner<ITextureLibrary> appTextureLibrary;
    InterfaceOwner<IMaterialManager> appMaterials;
    InterfaceOwner<IMeshManager> appMeshes;
    InterfaceOwner<IAnimation> appAnimation;
    InterfaceOwner<IHardpoint> appHardpoint;
    InterfaceOwner<ITManager> appTextureManager;
    InterfaceOwner<IVertexBufferManager> appVertexBuffers;
    InterfaceOwner<IStringTable> appStrings;
    InterfaceOwner<ISoundManager> appSoundManager;
    InterfaceOwner<IUnitComm> appUnitComm;
    InterfaceOwner<IStringData> appStringData;
    InterfaceOwner<ITeletype> appTeletype;
    InterfaceOwner<ISubtitle> appSubtitle;
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
    InterfaceOwner<IFileSystem> appMovieSource;
#endif
    AppEngineGlobals appEngineGlobals;
#endif
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
    if (options.missionEngine)
    {
        if (!FrontierX::NativeGameSystemEngine(gameSystem.get()) || ENGINE || REND)
        {
            std::cerr << "Native App mission Engine unavailable or already bound\n";
            return 6;
        }
        nativeManager->SetProfileParser(profile.Get());
        if (!FrontierX::LoadNativeGameSystemEngineComponents(gameSystem.get(),
                systemContainer, appRenderer.Receive()))
        {
            nativeManager->SetProfileParser(nullptr);
            std::cerr << "Native App mission Engine components or IRenderer unavailable\n";
            return 6;
        }
        if (systemContainer->QueryInterface(IID_IRenderPipeline,
                reinterpret_cast<void **>(appPipeline.Receive())) != GR_OK ||
            !appPipeline.Get() ||
            systemContainer->QueryInterface(IID_IRenderPrimitive,
                reinterpret_cast<void **>(appPrimitive.Receive())) != GR_OK ||
            !appPrimitive.Get())
        {
            nativeManager->SetProfileParser(nullptr);
            std::cerr << "Native App mission render interfaces unavailable\n";
            return 6;
        }
        if (options.missionStart &&
            (systemContainer->QueryInterface(IID_ILightManager,
                reinterpret_cast<void **>(appLight.Receive())) != GR_OK || !appLight.Get() ||
             systemContainer->QueryInterface(IID_ITextureLibrary,
                reinterpret_cast<void **>(appTextureLibrary.Receive())) != GR_OK || !appTextureLibrary.Get() ||
             systemContainer->QueryInterface(IID_IMaterialManager,
                reinterpret_cast<void **>(appMaterials.Receive())) != GR_OK || !appMaterials.Get() ||
             systemContainer->QueryInterface(IID_IMeshManager,
                reinterpret_cast<void **>(appMeshes.Receive())) != GR_OK || !appMeshes.Get() ||
             systemContainer->QueryInterface(IID_ITManager,
                reinterpret_cast<void **>(appTextureManager.Receive())) != GR_OK || !appTextureManager.Get() ||
             systemContainer->QueryInterface(IID_IVertexBufferManager,
                reinterpret_cast<void **>(appVertexBuffers.Receive())) != GR_OK || !appVertexBuffers.Get() ||
             systemContainer->QueryInterface(IID_IStringTable,
                reinterpret_cast<void **>(appStrings.Receive())) != GR_OK || !appStrings.Get()))
        {
            nativeManager->SetProfileParser(nullptr);
            std::cerr << "Native App required profile services unavailable\n";
            return 6;
        }
        if (options.missionStart &&
            (FrontierX::NativeGameSystemEngine(gameSystem.get())->QueryInterface(
                IID_IAnimation, reinterpret_cast<void **>(appAnimation.Receive())) != GR_OK ||
             !appAnimation.Get() ||
             FrontierX::NativeGameSystemEngine(gameSystem.get())->QueryInterface(
                IID_IHardpoint, reinterpret_cast<void **>(appHardpoint.Receive())) != GR_OK ||
             !appHardpoint.Get()))
        {
            std::cerr << "Native App required animation or hardpoint service unavailable\n";
            return 6;
        }
        if (options.missionStart)
        {
            ANIM = appAnimation.Get();
            HARDPOINT = appHardpoint.Get();
        }
        nativeManager->SetProfileParser(nullptr);
        ENGINE = FrontierX::NativeGameSystemEngine(gameSystem.get());
        REND = appRenderer.Get();
        PIPE = appPipeline.Get();
        BATCH = appPrimitive.Get();
        if (options.missionStart)
        {
            LIGHT = appLight.Get();
            TEXLIB = appTextureLibrary.Get();
            MATMAN = appMaterials.Get();
            MESHMAN = appMeshes.Get();
            TMANAGER = appTextureManager.Get();
            VB_MANAGER = appVertexBuffers.Get();
            STRINGTABLE = appStrings.Get();
        }
        std::cout << "Native App mission Engine and render interfaces bound; GPU not initialized\n";
    }
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
        if (!appTextBinding.Bind(EVENTSYS, options.missionStart && options.HasUiFont()))
        {
            std::cerr << "Native App text-resource binding failed\n";
            return 6;
        }
        if (!appTextBinding.BindScrollingText(options.skipNotificationCue))
        {
            std::cerr << "Native App scrolling-text binding failed\n";
            return 6;
        }
        if (options.skipNotificationCue && !options.missionStart)
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
        std::cout << "Native App text resources initialized" <<
            (options.missionStart && options.HasUiFont() ? "; original visual owners selected" : "; terminal-only owners") << '\n';
        InterfaceOwner<IViewConstructor> viewConstructor;
        InterfaceOwner<IViewConstructor2> viewConstructor2;
        if (GS->QueryInterface(IID_IViewConstructor,
                reinterpret_cast<void **>(viewConstructor.Receive())) != GR_OK ||
            !viewConstructor.Get() ||
            viewConstructor.Get()->QueryInterface(IID_IViewConstructor2,
                reinterpret_cast<void **>(viewConstructor2.Receive())) != GR_OK ||
            !viewConstructor2.Get() ||
            (!options.missionStart &&
             !FrontierX::ParseNativeAppSchema(viewConstructor2.Get())))
        {
            std::cerr << "Native App parser schema initialization failed\n";
            return 6;
        }
        if (!options.missionStart)
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
        InterfaceOwner<IFileSystem> physicalInterface;
        InterfaceOwner<IFileSystem> physicalTextures;
        InterfaceOwner<IFileSystem> physicalCampaign;
        InterfaceOwner<IFileSystem> physicalMaterials;
        InterfaceOwner<IFileSystem> physicalProfiles;
        const auto resolveSource = [&](const char * name,
                                       InterfaceOwner<IFileSystem> & physical,
                                       FrontierX::NativePhysicalFileLimit limit =
                                           FrontierX::NativePhysicalFileLimit::Standard) -> IFileSystem * {
            for (const auto & mount : archiveMounts)
                if (mount.first == name) return mount.second->Get();
            for (const auto & entry : startupRoot.entries)
                if (entry.expectedPath == name &&
                    entry.status == FrontierX::Platform::DataRootEntryStatus::Present)
                {
                    std::string error;
                    if (FrontierX::OpenNativeDirectoryFileSystem(
                            startupRoot.path / entry.actualPath,
                            physical.Receive(), error, limit)) return physical.Get();
                }
            return nullptr;
        };
        IFileSystem * objects = resolveSource("Objects", physicalObjects);
        IFileSystem * campaign = resolveSource("Maps/Campaign", physicalCampaign);
        IFileSystem * materials = materialSource.Get() ? materialSource.Get() :
            resolveSource("Mat", physicalMaterials);
        IFileSystem * profiles = resolveSource("Profiles", physicalProfiles);
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
        InterfaceOwner<IFileSystem> physicalMovies;
        if (options.missionStart && !options.skipMovies)
        {
            IFileSystem * movies = resolveSource("Movies", physicalMovies,
                FrontierX::NativePhysicalFileLimit::Movies);
            if (!movies)
            {
                std::cerr << "Native movie directory unavailable\n";
                return 6;
            }
            movies->AddRef();
            *appMovieSource.Receive() = movies;
        }
#endif
        if (!appDataSources.Bind(objects, campaign, materials, profiles))
        {
            std::cerr << "Native App mission data sources unavailable\n";
            return 6;
        }
        if (IFileSystem * interfaceDirectory = resolveSource("Interface", physicalInterface))
        {
            if (!appDataSources.BindInterface(interfaceDirectory)) return 6;
        }
        IFileSystem * textures = resolveSource("Textures", physicalTextures);
        if (textures)
        {
            if (!appDataSources.BindTextures(textures)) return 6;
        }
        else if (options.missionStart && !options.skipSectorMap)
        {
            std::cerr << "Native App original map texture directory unavailable\n";
            return 6;
        }
        if (!options.profileSmokeName.empty())
        {
            InterfaceOwner<IProfileParser2> profileParser;
            if (CreateProfileParser(options.profileSmokeName.c_str(),
                    profileParser.Receive(), nullptr) != GR_OK || !profileParser.Get())
            {
                std::cerr << "Native App profile parser could not open supplied profile\n";
                return 6;
            }
            const HANDLE section = profileParser.Get()->CreateSection("NativeSmoke");
            char value[16] = {};
            if (!section || profileParser.Get()->ReadKeyValue(section, "Value",
                    value, sizeof(value)) == 0 ||
                !profileParser.Get()->CloseSection(section) ||
                std::string(value) != "ready")
            {
                std::cerr << "Native App profile parser smoke content invalid\n";
                return 6;
            }
            std::cout << "Native App PROFILEDIR CreateProfileParser checked\n";
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
        if (!options.missionStart &&
            !FrontierX::LoadNativeMissionSchema(mission.Get(),
                viewConstructor2.Get(), missionSymbols, &parseBytes))
        {
            std::cerr << "Native App mission ParseData invalid\n";
            return 6;
        }
        if (missionSymbols) viewConstructor2.Get()->DestroySymbols(missionSymbols);
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
        std::cout << "Native App mission source opened through SPMAPDIR; Globals bytes: "
                  << globalsBytes;
        if (options.missionStart)
            std::cout << "; parser ownership reserved for Mission::Startup\n";
        else
            std::cout << "; ParseData bytes: " << parseBytes
                      << "; schema parsed; mission not started\n";
        if (!appParticleBinding.Bind(GS))
        {
            std::cerr << "Native App required particle manager unavailable\n";
            return 6;
        }
        InterfaceOwner<IStreamer> streamer;
        InterfaceOwner<IFileSystem> physicalMusic;
        InterfaceOwner<IFileSystem> physicalEffects;
        InterfaceOwner<IFileSystem> physicalSpeech;
        InterfaceOwner<IFileSystem> physicalMissionSpeech;
        IFileSystem *music = resolveSource("Music", physicalMusic);
        IFileSystem *effects = resolveSource("SFX", physicalEffects);
        IFileSystem *speech = resolveSource("Speech", physicalSpeech);
        IFileSystem *missionSpeech = resolveSource("MSpeech", physicalMissionSpeech);
        if (!music || !effects ||
            (options.missionStart && (!speech || !missionSpeech)) ||
            GS->QueryInterface(IID_IStreamer,
                reinterpret_cast<void **>(streamer.Receive())) != GR_OK ||
            !streamer.Get() || !appAudioBinding.Bind(streamer.Get(), music,
                effects, speech, missionSpeech))
        {
            std::cerr << "Native App mission audio source or IStreamer unavailable\n";
            return 6;
        }
    }
    InterfaceOwner<IFileSystem> physicalMeshDiagnosticRoot;
    IFileSystem * meshDiagnosticRoot = nullptr;
    const bool deferMeshDiagnostic = options.missionGpu && !options.missionStart;
    if (!options.animationObjectName.empty() || !options.meshObjectName.empty())
    {
        IFileSystem * objectRoot = OBJECTDIR;
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
                            physicalMeshDiagnosticRoot.Receive(), error))
                        objectRoot = physicalMeshDiagnosticRoot.Get();
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
            if (!options.missionStart)
            {
                if (!FrontierX::CheckNativeAnimationNodeRelease())
                {
                    std::cerr << "Native source animation node release failed\n";
                    return 6;
                }
                std::cout << "Native source animation node kinds released with allocated types\n";
                std::cout << "Native source animation channel stream types released polymorphically\n";
            }
        }
        meshDiagnosticRoot = objectRoot;
        if (!options.meshObjectName.empty() && !deferMeshDiagnostic)
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
    AppHotkeyGlobal appHotkeyGlobal;
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
        hotkeyDescriptor.hotkeyMessage = CQE_HOTKEY;
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
        HOTKEY = hotkey.Get();
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
        std::cout << "Native App mission IStreamer initialized with Music and SFX sources\n";
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
        InterfaceOwner<ISFX> soundEffects;
        *soundEffects.Receive() = FrontierX::CreateNativeSfxManager();
        if (!soundEffects.Get() || !appSfxBinding.Bind(soundEffects.Get()))
        {
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            std::cerr << "Native App sound-effect manager initialization failed\n";
            return 6;
        }
        std::cout << "Native App ISFX initialized from source ID table\n";
        if (options.skipMissionSfx)
        {
            SFXMANAGER->Enable(FALSE);
            std::cout << "Native mission SFX explicitly disabled through ISFX::Enable(FALSE)\n";
        }
        if (options.sfxSmoke)
        {
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
            if (!options.missionStart && saveDirectory.Get())
            {
                InterfaceOwner<IFileSystem> aiCheckDirectory;
                std::string aiCheckError;
                std::error_code aiDirectoryError;
                std::filesystem::create_directories(options.saveRoot / "NativeAIRegression",
                                                    aiDirectoryError);
                if (aiDirectoryError || !FrontierX::OpenNativeSaveDirectory(
                        options.saveRoot / "NativeAIRegression", aiCheckDirectory.Receive(),
                        aiCheckError) ||
                    !CheckNativePlayerAISaveLifecycle(aiCheckDirectory.Get()))
                {
                    std::cerr << "Native production player AI save lifecycle check failed\n";
                    return 6;
                }
                std::cout << "Native production player AI save lifecycle checked\n";
            }
            if (!options.missionStart && saveDirectory.Get())
            {
                InterfaceOwner<IFileSystem> mapCheckDirectory;
                std::string mapCheckError;
                std::error_code mapDirectoryError;
                std::filesystem::create_directories(options.saveRoot / "NativeMapRegression",
                                                    mapDirectoryError);
                if (mapDirectoryError || !FrontierX::OpenNativeSaveDirectory(
                        options.saveRoot / "NativeMapRegression", mapCheckDirectory.Receive(),
                        mapCheckError) ||
                    !FrontierX::CheckNativeSystemMapPersistence(mapCheckDirectory.Get(),
                                                               userDefaults.Get()))
                    throw std::runtime_error("Native source system-map persistence check failed");
                std::cout << "Native source system-map mission animations saved and restored\n";
            }
            if (!options.missionStart && saveDirectory.Get())
            {
                InterfaceOwner<IFileSystem> pingCheckDirectory;
                std::string pingCheckError;
                std::error_code pingDirectoryError;
                std::filesystem::create_directories(options.saveRoot / "NativePingRegression",
                                                    pingDirectoryError);
                if (pingDirectoryError || !FrontierX::OpenNativeSaveDirectory(
                        options.saveRoot / "NativePingRegression", pingCheckDirectory.Receive(),
                        pingCheckError) ||
                    !CheckNativeStatelessPingRestart(pingCheckDirectory.Get()) ||
                    !CheckNativeGunboatLauncherFailure(pingCheckDirectory.Get()))
                    throw std::runtime_error("Native source stateless Ping restart check failed");
                std::cout << "Native source stateless Ping restart validated against real launcher owner\n";
                std::cout << "Native source Gunboat failed launcher load propagated\n";
                if (!FrontierX::CheckNativeTrailTerminalFade())
                    throw std::runtime_error("Native source warp trail terminal fade check failed");
                std::cout << "Native source warp trail terminal fade validated\n";
            }
            if (!options.missionStart && saveDirectory.Get())
            {
                InterfaceOwner<IFileSystem> abilityCheckDirectory;
                std::string abilityCheckError;
                std::error_code abilityDirectoryError;
                std::filesystem::create_directories(options.saveRoot / "NativeAbilityRegression",
                                                    abilityDirectoryError);
                if (abilityDirectoryError || !FrontierX::OpenNativeSaveDirectory(
                        options.saveRoot / "NativeAbilityRegression", abilityCheckDirectory.Receive(),
                        abilityCheckError) ||
                    !CheckNativeMissionAbilityPersistence(abilityCheckDirectory.Get()))
                    throw std::runtime_error("Native source mission ability persistence check failed");
                std::cout << "Native source mission ability records saved and invalid records rejected\n";
            }
            if (!options.missionStart && saveDirectory.Get())
            {
                InterfaceOwner<IFileSystem> recordCheckDirectory;
                std::string recordCheckError;
                std::error_code recordDirectoryError;
                std::filesystem::create_directories(options.saveRoot / "NativeMissionRecordRegression",
                                                    recordDirectoryError);
                if (recordDirectoryError || !FrontierX::OpenNativeSaveDirectory(
                        options.saveRoot / "NativeMissionRecordRegression", recordCheckDirectory.Receive(),
                        recordCheckError) ||
                    !CheckNativeMissionRecordPersistence(recordCheckDirectory.Get()))
                    throw std::runtime_error("Native source mission record persistence check failed");
                std::cout << "Native source mission records correlated and invalid records rejected\n";
            }
            if (!options.missionStart && saveDirectory.Get())
            {
                InterfaceOwner<IFileSystem> progressDirectory;
                std::string progressError;
                std::filesystem::create_directories(options.saveRoot / "NativeProgressRegression");
                if (!FrontierX::OpenNativeSaveDirectory(options.saveRoot / "NativeProgressRegression",
                        progressDirectory.Receive(), progressError) ||
                    !CheckNativeGameProgressPersistence(progressDirectory.Get()))
                    throw std::runtime_error("Native original progress persistence check failed");
                std::cout << "Native original progress scratch roundtrip and malformed-read preservation passed\n";
            }
            if (!options.missionStart && !CheckNativeUnitCatalogMetadata())
            {
                std::cerr << "Native unit catalogue metadata classification failed\n";
                return 6;
            }
            if (!options.missionStart && !CheckNativeCampaignScriptPrograms())
            {
                std::cerr << "Native source campaign script registration check failed\n";
                return 6;
            }
            if (!options.missionStart && !FrontierX::CheckNativeHotButtonCommands())
            {
                std::cerr << "Native original hot-button command check failed\n";
                return 6;
            }
            if (!options.missionStart)
            {
                if (!FrontierX::CheckNativeTerranDroneArrayRelease() ||
                    !FrontierX::CheckNativeSpiderDroneArrayRelease())
                    throw std::runtime_error("Native builder-drone array ownership check failed");
                std::cout << "Native source builder-drone arrays released by their allocating owners\n";
            }
            if (!options.missionStart && !NativeAppCheckAnimRandomization())
            {
                std::cerr << "Native effect-animation randomization contract failed\n";
                return 6;
            }
            if (!options.missionStart)
                std::cout << "Native effect-animation randomization and RNG consumption checked\n";
            if (!NativeAppCheckOperationStorage())
            {
                std::cerr << "Native operation storage contract failed\n";
                return 6;
            }
            std::cout << "Native operation storage alignment/copy/load checked\n";
            if (!NativeAppCheckObjectiveStorage())
            {
                std::cerr << "Native objective storage contract failed\n";
                return 6;
            }
            std::cout << "Native objective storage compaction/state checked\n";
#endif
            if (SFXMANAGER->Preload(SFX::INVALID) ||
                SFXMANAGER->Open(SFX::INVALID) ||
                !SFXMANAGER->Preload(SFX::BUTTONPRESS1))
            {
                std::cerr << "Native ISFX BUTTONPRESS1 preload failed\n";
                return 6;
            }
            HSOUND sound = SFXMANAGER->Open(SFX::BUTTONPRESS1);
            if (!sound || SFXMANAGER->DEBUG_GetNumHandles() != 1)
            {
                std::cerr << "Native ISFX BUTTONPRESS1 open failed\n";
                return 6;
            }
            HSOUND copy(sound);
            SINGLE volume = 0;
            if (!SFXMANAGER->SetVolume(sound, 0.5F) ||
                !SFXMANAGER->GetVolume(copy, &volume) || volume != 0.5F ||
                !SFXMANAGER->Play(sound) ||
                !SFXMANAGER->Play(sound, 0, nullptr, SFXPLAYF_NORESTART) ||
                !SFXMANAGER->Stop(sound) ||
                !SFXMANAGER->Play(sound, 0, nullptr, SFXPLAYF_LOOPING) ||
                !SFXMANAGER->Stop(sound))
            {
                std::cerr << "Native ISFX BUTTONPRESS1 playback contract failed\n";
                return 6;
            }
            // Unit voice shares IStreamer with SFX. Disabled effects retain only
            // their source-style logical handles and never take audio streams.
            if (!options.missionStart && !FrontierX::CheckNativeSfxPool())
            {
                std::cerr << "Native source SFX pool/cache contract failed\n";
                return 6;
            }
            if (!options.missionStart)
                std::cout << "Native source SFX pool/cache ownership and RNG consumption checked\n";
            SFXMANAGER->Enable(FALSE);
            const Vector mutedPosition(0.0F, 0.0F, 0.0F);
            HSOUND mutedSound = SFXMANAGER->Open(SFX::BUTTONPRESS1);
            if (!mutedSound || !SFXMANAGER->Play(mutedSound, 1, &mutedPosition) ||
                SFXMANAGER->Preload(SFX::BUTTONPRESS1))
            {
                std::cerr << "Native ISFX disabled spatial/resource contract failed\n";
                return 6;
            }
            mutedSound = 0;
            SFXMANAGER->Enable(TRUE);
            sound = 0;
            if (SFXMANAGER->DEBUG_GetNumHandles() != 1)
            {
                std::cerr << "Native ISFX shared handle ownership failed\n";
                return 6;
            }
            copy = 0;
            if (SFXMANAGER->DEBUG_GetNumHandles() != 0)
            {
                std::cerr << "Native ISFX final handle release failed\n";
                return 6;
            }
            std::cout << "Native ISFX BUTTONPRESS1 preload/open/play/stop checked\n";
            if (!options.missionStart)
            {
                if (!FrontierX::CheckNativeMissionSoundCompletion())
                {
                    std::cerr << "Native mission sound EOF/completion contract failed\n";
                    return 6;
                }
                std::cout << "Native mission sound retains EOF until actual completion and owner cleanup checked\n";
            }
        }
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
#if defined(FRONTIERX_HAS_SDL_GPU_RENDERER)
    if (!options.missionStart)
    {
        if (!FrontierX::CheckNativeAnimationBorder())
        { std::cerr << "Native animation border resize check failed\n"; return 6; }
    }
    if (!options.fontSmokePath.empty())
    {
        if (!options.uiFontMappingPath.empty())
        {
            std::string fontError;
            if (!FrontierX::SetNativeFontMapping(options.uiFontMappingPath.string().c_str(), fontError))
            { std::cerr << "Native external font mapping failed: " << fontError << '\n'; return 6; }
        }
        else if (!FrontierX::SetNativeFontFile(options.fontSmokePath.string().c_str()))
        { std::cerr << "Native font smoke face unavailable\n"; return 6; }
        if (!FrontierX::CheckNativeSourceTeletype() || !FrontierX::CheckNativeSourceSubtitle() || !FrontierX::CheckNativeSourceTextResources() || !FrontierX::CheckNativeUnitAlerts() || !FrontierX::CheckNativeMouseScroll() || !FrontierX::CheckNativeHotRect() || !FrontierX::CheckNativeVfxReader())
        { std::cerr << "Native source text lifecycle check failed\n"; return 6; }
        std::cout << "Native source teletype counted string, pause, expiry and flush checked; subtitle wrapping and release checked; status/hint/scrolling lifecycle checked; unit alert lifecycle checked; hot-rectangle input/post/lifecycle checked; original VFX image reader checked\n";
    }
#endif
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
    MissionGpuUnbind missionGpuUnbind;
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
    InterfaceOwner<IVideoSystem> appVideo;
    struct VideoShutdown
    {
        InterfaceOwner<IVideoSystem> &owner;
        ~VideoShutdown()
        {
            if (VIDEOSYS == owner.Get()) VIDEOSYS = nullptr;
            if (owner.Get()) owner.Get()->Shutdown();
        }
    } videoShutdown{appVideo};
#endif
    AppGlobalCleanup appGlobalCleanup;
    std::uint32_t presentedFrames = 0U;
    FrontierX::SdlGpuReadback directReadback;
    if (options.missionGpu)
    {
        const HWND token = windowManager.Get()->GetWindowHandle();
        if (!token || token > std::numeric_limits<std::uint32_t>::max())
        {
            std::cerr << "Native mission GPU received an invalid SDL window token\n";
            return 7;
        }
        const FrontierX::SdlGpuTextureView initialTexture{
            FrontierX::RenderSmokeTextureRgba.data(),
            FrontierX::RenderSmokeTextureWidth,
            FrontierX::RenderSmokeTextureHeight,
            FrontierX::RenderSmokeTextureRgba.size()};
        std::string renderError;
        if (!renderer.Initialize(static_cast<std::uint32_t>(token),
                options.vertexShaderPath.string().c_str(),
                options.fragmentShaderPath.string().c_str(), initialTexture,
                renderError, options.gameExtendedShaderPath.string().c_str(),
                options.gameVertexShaderPath.string().c_str()))
        {
            std::cerr << "Native mission GPU initialization failed: " << renderError << '\n';
            return 7;
        }
        FrontierX::NativeRenderPrimitiveDesc descriptor(
            FrontierX::SdlGpuRenderer::SubmissionCallback, &renderer,
            FrontierX::SdlGpuRenderer::Submission2Callback,
            FrontierX::SdlGpuRenderer::RenderStateCallback,
            nullptr, nullptr,
            FrontierX::SdlGpuRenderer::TextureStageCallback,
            FrontierX::SdlGpuRenderer::TextureBindCallback,
            FrontierX::SdlGpuRenderer::ModelviewCallback,
            FrontierX::SdlGpuRenderer::ProjectionCallback,
            FrontierX::SdlGpuRenderer::Submission3Callback,
            FrontierX::SdlGpuRenderer::Submission4Callback,
            FrontierX::SdlGpuRenderer::SamplerStateCallback,
            FrontierX::SdlGpuRenderer::ViewportCallback);
        if (FrontierX::BindNativeRenderPrimitive(appPrimitive.Get(), descriptor) != GR_OK)
        {
            std::cerr << "Native mission profile primitive GPU binding failed\n";
            return 7;
        }
        missionGpuUnbind.primitive = appPrimitive.Get();
        const FrontierX::NativeTextureOperations textures{
            &renderer, FrontierX::SdlGpuRenderer::TextureLookupCallback,
            FrontierX::SdlGpuRenderer::TextureCreateCallback,
            FrontierX::SdlGpuRenderer::TextureUploadCallback,
            FrontierX::SdlGpuRenderer::TextureDestroyCallback,
             FrontierX::SdlGpuRenderer::TextureCreateMipsCallback, FrontierX::SdlGpuRenderer::TextureUploadMipsCallback};
        if (FrontierX::BindNativeRenderPipeline(appPipeline.Get(), appPrimitive.Get(),
                textures, FrontierX::SdlGpuRenderer::VertexConstantsCallback,
                &renderer) != GR_OK)
        {
            std::cerr << "Native mission profile pipeline GPU binding failed\n";
            return 7;
        }
        missionGpuUnbind.pipeline = appPipeline.Get();
        if (FrontierX::BindNativeRenderFrames(appPipeline.Get(),
                {&renderer, FrontierX::SdlGpuRenderer::ClearFrameCallback,
                 FrontierX::SdlGpuRenderer::PresentFrameCallback}) != GR_OK)
            return 7;
        if (PIPE->set_viewport(0, 0, static_cast<int>(SCREENRESX),
                static_cast<int>(SCREENRESY)) != GR_OK ||
            PIPE->set_ortho(0.0F, static_cast<float>(SCREENRESX),
                static_cast<float>(SCREENRESY), 0.0F, -1.0F, 1.0F) != GR_OK)
        {
            std::cerr << "Native mission GPU viewport or projection failed\n";
            return 7;
        }
        std::cout << "Native mission profile render interfaces bound to SDL GPU; mission not started\n";
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
        if (options.missionStart && !options.skipMovies)
        {
            if (systemContainer->QueryInterface(IID_IVideoSystem,
                    reinterpret_cast<void **>(appVideo.Receive())) != GR_OK ||
                !appVideo.Get() ||
                appVideo.Get()->Initialize(appMovieSource.Get(), PIPE) != GR_OK)
            {
                std::cerr << "Native movie owner initialization failed\n";
                return 6;
            }
            VIDEOSYS = appVideo.Get();
            std::cout << "Native movie owner initialized with original external movie directory\n";
        }
#endif
        if (deferMeshDiagnostic && !options.meshObjectName.empty())
        {
            // This separate owner diagnostic needs real GPU texture upload.
            // It never runs alongside a selected mission's simulation.
            unsigned int faceGroups = 0;
            if (!FrontierX::ValidateMeshObject(systemContainer,
                    FrontierX::NativeGameSystemEngine(gameSystem.get()),
                    meshDiagnosticRoot, MATERIALDIR ? MATERIALDIR : materialSource.Get(),
                    options.meshObjectName.c_str(), faceGroups))
            {
                std::cerr << "Native mesh archetype load failed\n";
                return 6;
            }
            std::cout << "Native mesh instance created and destroyed; face groups: "
                      << faceGroups << '\n';
            std::cout << "Native separate GPU mesh load/create/destroy diagnostic completed; rendering untested\n";
            windowManager.Get()->SetCallback(nullptr);
            shellEventSystem = nullptr;
            return 0;
        }
        if (options.missionStart)
        {
            // Sol_T/resource.h: IDS_SOLTRAIN_OBJECTIVE1 and OBJECTIVE2.
            constexpr U32 kSelectForgerObjective = 46U;
            constexpr U32 kMoveForgerObjective = 47U;
            if (options.missionSourceName == "T_Mission1.dmission" ||
                options.missionSourceName == "T_Mission2.dmission" ||
                options.missionSourceName == "T_Mission3.dmission" ||
                options.missionSourceName == "T_Mission4.dmission" ||
                options.missionSourceName == "T_Mission5.dmission" ||
                options.missionSourceName == "T_Mission6.dmission" ||
                options.missionSourceName == "T_Mission7.dmission" ||
                options.missionSourceName == "T_Mission8.dmission" ||
                options.missionSourceName == "T_Mission9.dmission" ||
                options.missionSourceName == "T_Mission10.dmission" ||
                options.missionSourceName == "T_Mission11.dmission" ||
                options.missionSourceName == "T_Mission12.dmission")
            {
                InitializeNativeEffectsOptions();
                if (options.skipSpaceBackground)
                {
                    CQEFFECTS.bBackground = GlobalEffectsOptions::off;
                    CQEFFECTS.bExpensiveTerrain = GlobalEffectsOptions::off;
                }
                std::cout << "Native App Terran source effects settings initialized\n";
            }
            NATIVE_SKIP_LEGACY_LIGHTING = options.skipLegacyLighting;
            NATIVE_USE_ORIGINAL_LEGACY_MATERIALS =
                (options.missionSourceName == "T_Mission1.dmission" ||
                 options.missionSourceName == "T_Mission2.dmission" ||
                options.missionSourceName == "T_Mission3.dmission" ||
                options.missionSourceName == "T_Mission4.dmission" ||
                options.missionSourceName == "T_Mission5.dmission" ||
                options.missionSourceName == "T_Mission6.dmission" ||
                options.missionSourceName == "T_Mission7.dmission" ||
                options.missionSourceName == "T_Mission8.dmission" ||
                options.missionSourceName == "T_Mission9.dmission" ||
                options.missionSourceName == "T_Mission10.dmission" ||
                options.missionSourceName == "T_Mission11.dmission" ||
                options.missionSourceName == "T_Mission12.dmission") && !options.skipLegacyLighting;
            if (NATIVE_USE_ORIGINAL_LEGACY_MATERIALS)
                std::cout << "Native Terran legacy materials use original scalar lighting, "
                             "diffuse/emission colors and authored texture UV pairs\n";
            if (options.skipLegacyLighting)
                std::cout << "Native App legacy lighting explicitly disabled; "
                             "light objects remain in simulation\n";
            if (!VB_MANAGER || VB_MANAGER->initialize(nullptr) != GR_OK)
            {
                std::cerr << "Native mission vertex-buffer initialization failed\n";
                return 6;
            }
            appGlobalCleanup.vertexBuffersInitialized = true;
            RegisterTheRenderBatcher(manager);
            AGGDESC batchDescriptor("IRenderPrimitive");
            InterfaceOwner<IAggregateComponent> batchAggregate;
            if (manager->CreateInstance(&batchDescriptor,
                    reinterpret_cast<void **>(appRenderBatcher.Receive())) != GR_OK ||
                !appRenderBatcher.Get() ||
                appRenderBatcher.Get()->QueryInterface(IID_IAggregateComponent,
                    reinterpret_cast<void **>(batchAggregate.Receive())) != GR_OK ||
                !batchAggregate.Get() || batchAggregate.Get()->Initialize() != GR_OK ||
                appRenderBatcher.Get()->QueryInterface(IID_ICQBatch,
                    reinterpret_cast<void **>(appPoolBatch.Receive())) != GR_OK ||
                !appPoolBatch.Get())
            {
                std::cerr << "Native App production render batcher unavailable\n";
                return 6;
            }
            BATCH = appRenderBatcher.Get();
            CQBATCH = appPoolBatch.Get();
            std::cout << "Native App entering production global startup\n" << std::flush;
            appGlobalCleanup.created = true;
            CreateGlobalComponents();
            if (options.missionCampaign && !MGlobals::New())
            {
                std::cerr << "Native original menu global initialization failed\n";
                return 6;
            }
            if (!MISSION || !OBJLIST || !MAINCAM || !GAMEPROGRESS)
            {
                std::cerr << "Native App required mission, object, camera or progress owner unavailable\n";
                return 6;
            }
            std::cout << "Native App original progress loaded; completed: "
                      << GAMEPROGRESS->GetMissionsCompleted() << "; seen: "
                      << GAMEPROGRESS->GetMissionsSeen() << '\n' << std::flush;
            *appSoundManager.Receive() = FrontierX::CreateNativeMissionSoundManager(
                options.skipTrainingSpeech, options.skipAnimatedPortraits);
            if (!options.uiFontMappingPath.empty())
            {
                std::string fontError;
                if (!FrontierX::SetNativeFontMapping(options.uiFontMappingPath.string().c_str(), fontError))
                { std::cerr << "Native external font mapping failed: " << fontError << '\n'; return 6; }
            }
            else if (!options.uiFontPath.empty() &&
                     !FrontierX::SetNativeFontFile(options.uiFontPath.string().c_str()))
            { std::cerr << "Native external UI font unavailable; enable FRONTIERX_BUILD_NATIVE_FONTS\n"; return 6; }
            *appTeletype.Receive() = options.skipTeletypeVisuals ?
                FrontierX::CreateNativeMissionTeletype(true) : FrontierX::CreateNativeSourceTeletype();
            if (options.skipUnitVoice)
                *appUnitComm.Receive() = FrontierX::CreateNativeDisabledUnitComm();
            if (!appSoundManager.Get() || !appTeletype.Get())
            {
                std::cerr << "Native App mission sound or teletype owner unavailable\n";
                return 6;
            }
            SOUNDMANAGER = appSoundManager.Get();
            TELETYPE = appTeletype.Get();
            if (!options.skipSubtitles)
            {
                *appSubtitle.Receive() = FrontierX::CreateNativeSourceSubtitle();
                if (!appSubtitle.Get())
                { std::cerr << "Native source subtitle owner unavailable\n"; return 6; }
                SUBTITLE = appSubtitle.Get();
                appEngineGlobals.EnableSubtitles();
                std::cout << "Native source mission subtitles enabled\n";
            }
            if (!options.skipUnitVoice)
            {
                InterfaceOwner<IFileSystem> stringPackArchive;
                std::string stringError;
                if (!FrontierX::OpenNativeUtfFileSystem(options.stringPackPath,
                        stringPackArchive.Receive(), stringError))
                {
                    std::cerr << "Native StringPack archive invalid: " << stringError << '\n';
                    return 6;
                }
                *appStringData.Receive() = FrontierX::CreateNativeStringData(stringPackArchive.Get());
                if (!appStringData.Get())
                {
                    std::cerr << "Native StringPack records invalid\n";
                    return 6;
                }
                STRINGDATA = appStringData.Get();
                *appUnitComm.Receive() = FrontierX::CreateNativeUnitComm(options.skipUnitAlerts);
            }
            if (!appUnitComm.Get())
            {
                std::cerr << "Native UnitComm owner unavailable\n";
                return 6;
            }
            UNITCOMM = appUnitComm.Get();
            std::cout << "Native App mission sound and teletype contracts bound"
                      << (options.skipTrainingSpeech ? "; training speech explicitly disabled" : "")
                      << (options.skipAnimatedPortraits ? "; animated portraits explicitly disabled" : "")
                      << (options.skipUnitVoice ? "; unit voice explicitly disabled" : "; production UnitComm voice enabled")
                      << (options.skipUnitAlerts ? "; unit alert text explicitly disabled" : "")
                      << (options.skipTeletypeVisuals ? "; teletype visuals explicitly disabled with source-timed handles" : "")
                      << '\n';
            if (!PARTMAN || !GAMEPOSCALLBACK || !MESHMAN || !TMANAGER ||
                !MATMAN || !OBJECTDIR || !MATERIALDIR)
            {
                std::cerr << "Native App mission initialization dependencies unavailable\n";
                return 6;
            }
            IParticleManager::InitInfo particleInfo{};
            ENGINE->get_search_path2(&appEngineGlobals.previousSearchPath);
            ENGINE->set_search_path2(OBJECTDIR); // menu.cpp::initDataPath
            appEngineGlobals.objectSearchBound = true;
            particleInfo.camera = MAINCAM;
            particleInfo.posCallback = GAMEPOSCALLBACK;
            particleInfo.MATMAN = MATMAN;
            particleInfo.MESHMAN = MESHMAN;
            PARTMAN->InitParticles(particleInfo);
            IMeshManager::InitInfo meshInfo{};
            meshInfo.ENGINE = ENGINE;
            meshInfo.ANIM = appAnimation.Get();
            meshInfo.OBJDIR = OBJECTDIR;
            meshInfo.REND = REND;
            meshInfo.PIPE = PIPE;
            meshInfo.CAMERA = MAINCAM;
            meshInfo.HARDPOINT = appHardpoint.Get();
            meshInfo.MATMAN = MATMAN;
            MESHMAN->InitManager(meshInfo);
            ITManager::InitInfo textureInfo{PIPE};
            TMANAGER->Initialize(textureInfo);
            IMaterialManager::InitInfo materialInfo{};
            materialInfo.PIPE = PIPE;
            materialInfo.TMANAGER = TMANAGER;
            materialInfo.MATDIR = MATERIALDIR;
            if (MATMAN->Initialize(materialInfo) != GR_OK)
            {
                std::cerr << "Native App mission material initialization failed\n";
                return 6;
            }
            BindNativeAppPrimitive(BATCH);
            missionGpuUnbind.appBuilderBound = true;
            CQFLAGS.b3DEnabled = true;
            if (!BACKGROUND)
            {
                std::cerr << "Native App mission requires the production SpaceEnvironment background owner\n";
                return 6;
            }
            if (options.baseTexturesOnly)
            {
                if (!TEXLIB || TEXLIB->set_library_state(
                        ITL_STATE_TEXTURE_LOD_LOAD, 0U) != GR_OK)
                {
                    std::cerr << "Native App base texture selection failed\n";
                    return 6;
                }
                std::cout << "Native App texture mip loading explicitly disabled; "
                             "SDL GPU samples the base level\n";
            }
            ChangeInterfaceRes((options.missionBriefing || options.missionCampaign)
                ? IR_FRONT_END_RESOLUTION : IR_IN_GAME_RESOLUTION);
            std::cout << "Native App in-game interface: real=" << SCREENRESX << ',' << SCREENRESY
                      << "; ideal=" << SCREEN_WIDTH << ',' << SCREEN_HEIGHT << '\n';
            if (options.skipMissionMusic)
            {
                MUSICMANAGER->Enable(FALSE);
                std::cout << "Native App inherited menu music explicitly disabled\n";
            }
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
            if (!options.movieSourceName.empty())
            {
                std::cout << "Native separate original MovieScreen diagnostic; mission not started\n" << std::flush;
                const U32 movieResult = MovieScreen(nullptr, options.movieSourceName.c_str());
                if (CQFLAGS.bFullScreenMovie)
                    throw std::runtime_error("Native MovieScreen did not release its full-screen state");
                std::cout << "Native original MovieScreen returned: " << movieResult
                          << "; mission gameplay untested\n";
                windowManager.Get()->SetCallback(nullptr);
                shellEventSystem = nullptr;
                return 0;
            }
#endif
            if (options.unitCatalogCheck)
            {
                std::cout << "Native separate unit-catalog diagnostic; no mission simulation will start\n" << std::flush;
                if (!CheckNativeRetailAdmiralBonuses())
                    throw std::runtime_error("Native retail admiral bonus queries failed");
                std::cout << "Native retail admiral bonuses and passive traits validated\n" << std::flush;
                const bool complete = CheckNativeUnitCatalog(ARCHLIST);
                windowManager.Get()->SetCallback(nullptr);
                shellEventSystem = nullptr;
                return complete ? 0 : 6;
            }
            if (options.missionCampaign)
            {
                const auto *menu = static_cast<const GT_MENU1 *>(GENDATA->GetArchetypeData("Menu1"));
                if (!menu) throw std::runtime_error("Original training-menu resource unavailable");
                std::cout << "Native App entering original campaign selector\n" << std::flush;
                if (!DoMenu_campaign(nullptr, *menu, false)) return 0;
            }
            else if (options.missionBriefing)
            {
                std::cout << "Native App selected mission entering original briefing\n" << std::flush;
                if (!DoMenu_Briefing(nullptr, options.missionSourceName.c_str()))
                {
                    std::cout << "Native App original briefing cancelled\n";
                    return 0;
                }
            }
            else if (!LoadNativeSelectedMission(options.missionSourceName.c_str(), SPMAPDIR))
            {
                std::cerr << "Native App production mission load failed\n";
                return 6;
            }
        missionLoaded:
            std::cout << "Native App production mission records loaded\n" << std::flush;
            char selectedScript[MAX_PATH] = {};
            if (!MGlobals::GetScriptName(selectedScript, sizeof(selectedScript))) return 6;
            const bool trainingMission = std::strcmp(selectedScript, "Sol_T.dll") == 0;
            const bool firstTerranMission = std::strcmp(selectedScript, "Script01T.dll") == 0 ||
                std::strcmp(selectedScript, "script01.dll") == 0;
            const bool secondTerranMission = std::strcmp(selectedScript, "Script02T.dll") == 0 ||
                std::strcmp(selectedScript, "script02.dll") == 0 ||
                std::strcmp(selectedScript, "Script02.dll") == 0;
            const bool thirdTerranMission = std::strcmp(selectedScript, "Script03T.dll") == 0 ||
                std::strcmp(selectedScript, "script03.dll") == 0 ||
                std::strcmp(selectedScript, "Script03.dll") == 0;
            const bool fourthTerranMission = std::strcmp(selectedScript, "Script04T.dll") == 0 ||
                std::strcmp(selectedScript, "script04.dll") == 0 ||
                std::strcmp(selectedScript, "Script04.dll") == 0;
            const bool fifthTerranMission = std::strcmp(selectedScript, "Script05T.dll") == 0 ||
                std::strcmp(selectedScript, "script05.dll") == 0 ||
                std::strcmp(selectedScript, "Script05.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT05.dll") == 0;
            const bool sixthTerranMission = std::strcmp(selectedScript, "Script06T.dll") == 0 ||
                std::strcmp(selectedScript, "script06.dll") == 0 ||
                std::strcmp(selectedScript, "Script06.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT06.dll") == 0;
            const bool seventhTerranMission = std::strcmp(selectedScript, "Script07T.dll") == 0 ||
                std::strcmp(selectedScript, "script07.dll") == 0 ||
                std::strcmp(selectedScript, "Script07.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT07.dll") == 0;
            const bool eighthTerranMission = std::strcmp(selectedScript, "Script08T.dll") == 0 ||
                std::strcmp(selectedScript, "script08.dll") == 0 ||
                std::strcmp(selectedScript, "Script08.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT08.dll") == 0;
            const bool ninthTerranMission = std::strcmp(selectedScript, "Script09T.dll") == 0 ||
                std::strcmp(selectedScript, "script09.dll") == 0 ||
                std::strcmp(selectedScript, "Script09.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT09.dll") == 0;
            const bool terran10Mission = std::strcmp(selectedScript, "Script10T.dll") == 0 ||
                std::strcmp(selectedScript, "script10.dll") == 0 ||
                std::strcmp(selectedScript, "Script10.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT10.dll") == 0;
            const bool terran11Mission = std::strcmp(selectedScript, "Script11T.dll") == 0 ||
                std::strcmp(selectedScript, "script11.dll") == 0 ||
                std::strcmp(selectedScript, "Script11.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT11.dll") == 0;
            const bool terran12Mission = std::strcmp(selectedScript, "Script12T.dll") == 0 ||
                std::strcmp(selectedScript, "script12.dll") == 0 ||
                std::strcmp(selectedScript, "Script12.dll") == 0 ||
                std::strcmp(selectedScript, "SCRIPT12.dll") == 0;
            if (!trainingMission && !firstTerranMission && !secondTerranMission &&
                !thirdTerranMission && !fourthTerranMission && !fifthTerranMission && !sixthTerranMission &&
                !seventhTerranMission && !eighthTerranMission && !ninthTerranMission && !terran10Mission && !terran11Mission && !terran12Mission) return 6;
            std::cout << "Native App selected source script: " << selectedScript << '\n';
            unsigned int forgers = 0;
            Vector initialPlayerForgerPosition{};
            bool initialPlayerForgerFound = false;
            for (IBaseObject *object = OBJLIST->GetObjectList(); object;
                 object = object->next)
            {
                const char *name = object->pArchetype
                    ? ARCHLIST->GetArchName(object->pArchetype) : nullptr;
                if (object->objClass == OC_SPACESHIP && name &&
                    std::strcmp(name, "FAB!!S_Forger") == 0) ++forgers;
            }
            if (trainingMission && forgers == 0)
            {
                std::cerr << "Native App mission has no Forger instance; "
                             "required object factory is absent\n";
                return 6;
            }
            std::cout << "Native App production Forger instances: " << forgers << '\n'
                      << std::flush;
            std::cout << "Native App selected system: " << SECTOR->GetCurrentSystem()
                      << "; player: " << MGlobals::GetThisPlayer() << '\n';
            const Vector cameraLookAt = CAMERA->GetLookAtPosition();
            std::cout << "Native App camera look-at: " << cameraLookAt.x << ','
                      << cameraLookAt.y << ',' << cameraLookAt.z << '\n';
            for (IBaseObject *object = OBJLIST->GetObjectList(); object;
                 object = object->next)
            {
                const char *name = object->pArchetype
                    ? ARCHLIST->GetArchName(object->pArchetype) : nullptr;
                if (name && std::strcmp(name, "FAB!!S_Forger") == 0)
                {
                    const Vector position = object->GetPosition();
                    if (object->GetPlayerID() == MGlobals::GetThisPlayer())
                    {
                        initialPlayerForgerPosition = position;
                        initialPlayerForgerFound = true;
                    }
                    std::cout << "Native App Forger system: " << object->GetSystemID()
                              << "; position: " << position.x << ',' << position.y
                              << ',' << position.z << "; owner: "
                              << object->GetPlayerID() << "; visible-to-player: "
                              << object->IsVisibleToPlayer(MGlobals::GetThisPlayer()) << '\n';
                }
            }
            CQFLAGS.bGameActive = -1;
            std::cout << "Native App sending source game-active transition\n" << std::flush;
            EVENTSYS->Send(CQE_GAME_ACTIVE, reinterpret_cast<void *>(1));
            MSCROLL->SetActive(1);
            std::cout << "Native App entering one source-time mission update\n" << std::flush;
            EVENTSYS->Send(CQE_UPDATE, reinterpret_cast<void *>(34133));
            std::cout << "Native App system after first update: "
                      << SECTOR->GetCurrentSystem() << '\n';
            const Vector cameraPosition = CAMERA->GetPosition();
            if (!CAMERA->SetPosition(&cameraPosition))
            {
                std::cerr << "Native App source camera initialization failed\n";
                return 7;
            }
            for (IBaseObject *object = OBJLIST->GetObjectList(); object;
                 object = object->next)
            {
                const char *name = object->pArchetype
                    ? ARCHLIST->GetArchName(object->pArchetype) : nullptr;
                if (name && std::strcmp(name, "FAB!!S_Forger") == 0 &&
                    object->GetPlayerID() == MGlobals::GetThisPlayer())
                {
                    S32 screenX = 0;
                    S32 screenY = 0;
                    const auto result = CAMERA->PointToScreen(object->GetPosition(),
                        &screenX, &screenY, nullptr);
                    std::cout << "Native App player Forger screen: " << screenX << ','
                              << screenY << "; projection: " << result << '\n';
                    break;
                }
            }
            for (IBaseObject *object = OBJLIST->GetObjectList(); object;
                 object = object->next)
            {
                if (object->objClass != OC_PLANETOID) continue;
                IBaseObject::MDATA planet{};
                if (!object->GetMissionData(planet) || !planet.pSaveData) continue;
                S32 screenX = 0, screenY = 0;
                const Vector position = object->GetPosition();
                const auto projected = CAMERA->PointToScreen(position, &screenX, &screenY, nullptr);
                std::cout << "Native App planet observation: " << planet.pSaveData->partName.string
                          << "; system=" << object->GetSystemID()
                          << "; position=" << position.x << ',' << position.y
                          << "; screen=" << screenX << ',' << screenY
                          << "; projection=" << projected << '\n';
            }
            if (options.missionAlertSmoke)
            {
                // Explicit source-caller diagnostic; the training script does not
                // naturally generate an alert in the selected early checkpoint.
                bool sent = false;
                for (IBaseObject *object = OBJLIST->GetObjectList(); object; object = object->next)
                {
                    const char *name = object->pArchetype ? ARCHLIST->GetArchName(object->pArchetype) : nullptr;
                    if (name && std::strcmp(name, "FAB!!S_Forger") == 0 && object->GetPlayerID() == MGlobals::GetThisPlayer())
                    {
                        FrontierX::SubmitNativeMissionAlertSmoke(object);
                        sent = true;
                        break;
                    }
                }
                if (!sent) { std::cerr << "Native source alert diagnostic has no player Forger\n"; return 7; }
                std::cout << "Native explicit MScript alert diagnostic submitted\n";
            }
            BATCH->set_state(RPR_BATCH, TRUE);
            std::string missionRenderError;
            if (PIPE->clear_buffers(RP_CLEAR_COLOR_BIT | RP_CLEAR_DEPTH_BIT, nullptr) != GR_OK ||
                PIPE->begin_scene() != GR_OK)
            {
                std::cerr << "Native App mission frame start failed: "
                          << renderer.LastError() << '\n';
                return 7;
            }
            std::cout << "Native App entering production object render\n" << std::flush;
            OBJLIST->Render();
            for (IBaseObject *object = OBJLIST->GetObjectList(); object;
                 object = object->next)
            {
                const char *name = object->pArchetype
                    ? ARCHLIST->GetArchName(object->pArchetype) : nullptr;
                if (name && std::strcmp(name, "FAB!!S_Forger") == 0)
                    std::cout << "Native App Forger visibility after source render: "
                              << object->bVisible << '\n';
            }
            EVENTSYS->Send(CQE_RENDER_LAST3D, reinterpret_cast<void *>(34133));
            std::cout << "Native App production draw submissions: "
                      << renderer.FrameSubmissionCount() << '\n';
            if (PIPE->end_scene() != GR_OK || PIPE->swap_buffers() != GR_OK)
            {
                std::cerr << "Native App mission presentation failed: " << renderer.LastError() << '\n';
                return 7;
            }
            FrontierX::SdlGpuReadback missionReadback;
            if (!renderer.Readback64(missionReadback, missionRenderError))
            {
                std::cerr << "Native App mission readback failed: " << missionRenderError << '\n';
                return 7;
            }
            std::cout << "Native App production object frame presented; changed-pixels: "
                      << missionReadback.changedPixels << "; checksum: "
                      << missionReadback.checksum << '\n' << std::flush;
            InterfaceOwner<IDAConnectionPoint> modalPoint;
            NativeMissionModalExit modalExit;
            modalExit.frameStats = options.frameStats;
            if (options.frameStats)
            {
                std::cout << "Native App GPU present mode: " << renderer.PresentModeName() << '\n';
                renderer.EnableTimings(true);
                modalExit.timingRenderer = &renderer;
                modalExit.lastTimings = renderer.GetTimings();
            }
            ConnectionOwner modalExitConnection;
            if (EVENTSYS->QueryOutgoingInterface("IEventCallback", modalPoint.Receive()) != GR_OK ||
                !modalExitConnection.Connect(modalPoint.Get(), &modalExit))
            {
                std::cerr << "Native App mission modal exit connection failed\n";
                return 7;
            }
            modalExit.started = FrontierX::Platform::MonotonicMilliseconds();
            modalExit.duration = options.smokeMilliseconds;
            const U32 modalResult = CQDoModal(nullptr);
            const unsigned int missionFrames = modalExit.frames;
            modalExitConnection.Disconnect();
            std::cout << "Native App selected source mission modal frames: "
                      << missionFrames << '\n';
            if (firstTerranMission && options.smokeMilliseconds &&
                !shellCloseRequested && modalResult == MISSION_END_QUIT && missionFrames == 0)
            {
                std::cerr << "Native App Terran bounded smoke executed no source frames\n";
                return 7;
            }
            if (firstTerranMission && options.smokeMilliseconds && missionFrames > 0 &&
                !shellCloseRequested && modalResult == MISSION_END_QUIT && modalExit.requested &&
                FrontierX::Platform::MonotonicMilliseconds() - modalExit.started >=
                    options.smokeMilliseconds)
                std::cout << "Native App first Terran bounded source loop completed\n";
            if (modalResult != MISSION_END_QUIT && modalResult != MISSION_END_RESIGN &&
                modalResult != MISSION_END_WON && modalResult != MISSION_END_LOST &&
                modalResult != MISSION_END_SPLASH)
            {
                std::cerr << "Native App source modal returned unexpected result "
                          << modalResult << "; renderer: " << renderer.LastError() << '\n';
                return 7;
            }
            std::cout << "Native App source modal returned " << modalResult << '\n';
            if (shellCloseRequested || modalResult != MISSION_END_QUIT || !trainingMission)
            {
                CQFLAGS.bGameActive = 0;
                EVENTSYS->Send(CQE_GAME_ACTIVE, nullptr);
                MISSION->Close(TRUE);
                std::cout << "Native App mission closed after modal exit\n";
                FrontierX::NativeMissionAudioStats closedAudio;
                if (!FrontierX::GetNativeMissionAudioStats(appSoundManager.Get(), closedAudio) ||
                    closedAudio.activeStreams != 0U ||
                    (options.HasUiFont() &&
                     (!FrontierX::NativeSourceHintIdle() || !FrontierX::NativeFieldHintIdle())))
                {
                    std::cerr << "Native App mission resources survived regular close\n";
                    return 7;
                }
                std::cout << "Native App regular close: mission audio streams=0; hint ownership="
                          << (!options.HasUiFont() ? "unchecked (font disabled)" : "idle")
                          << '\n';
                std::cout << "Native App source portraits: started=" << closedAudio.portraitsStarted
                          << "; frame changes=" << closedAudio.portraitFrameChanges
                          << "; draws=" << closedAudio.portraitDraws << '\n';
                if (!shellCloseRequested &&
                    (modalResult == MISSION_END_WON || modalResult == MISSION_END_LOST))
                {
                    ChangeInterfaceRes(IR_FRONT_END_RESOLUTION);
                    FULLSCREEN->Notify(CQE_UPDATE, nullptr);
                    GENDATA->FlushUnusedArchetypes();
                    const U32 endResult = CreateMenuEndGame(modalResult == MISSION_END_WON);
                    std::cout << "Native App original end menu returned " << endResult << '\n';
                }
                // Match the original single-player main loop: Continue, defeat,
                // resign and quit return through the campaign's mission selector.
                // Completion is owned exclusively by MScript::EndMissionVictory.
                MSCROLL->SetActive(0);
                GENDATA->FlushUnusedArchetypes();
                if (options.missionCampaign && !options.smokeMilliseconds && !shellCloseRequested &&
                    modalResult != MISSION_END_SPLASH)
                {
                    ChangeInterfaceRes(IR_FRONT_END_RESOLUTION);
                    FULLSCREEN->Notify(CQE_UPDATE, nullptr);
                    const auto *menu = static_cast<const GT_MENU1 *>(GENDATA->GetArchetypeData("Menu1"));
                    if (!menu) throw std::runtime_error("Original campaign-menu resource unavailable");
                    if (DoMenu_campaign(nullptr, *menu, MGlobals::IsSinglePlayer())) goto missionLoaded;
                }
                return 0;
            }
            std::cout << "Native App source mission loop frames: " << missionFrames
                      << "; simulation updates: " << MGlobals::GetUpdateCount()
                      << "; player selection: "
                      << (OBJLIST->GetSelectedList() ? "present" : "empty") << '\n'
                      << std::flush;
            std::cout << "Native App training objectives complete: select="
                      << MGlobals::IsObjectiveCompleted(kSelectForgerObjective)
                      << " move=" << MGlobals::IsObjectiveCompleted(kMoveForgerObjective)
                      << '\n';
            Vector savedForgerPosition{};
            U32 savedForgerOwner = 0U;
            bool foundSavedForger = false;
            for (IBaseObject *object = OBJLIST->GetObjectList(); object;
                 object = object->next)
            {
                const char *name = object->pArchetype
                    ? ARCHLIST->GetArchName(object->pArchetype) : nullptr;
                if (name && std::strcmp(name, "FAB!!S_Forger") == 0 &&
                    object->GetPlayerID() == MGlobals::GetThisPlayer())
                {
                    savedForgerPosition = object->GetPosition();
                    savedForgerOwner = object->GetPlayerID();
                    foundSavedForger = true;
                    break;
                }
            }
            if (!foundSavedForger || !MISSION->Save("native_slice.mission", SAVEDIR))
            {
                std::cerr << "Native App source mission save failed\n";
                return 7;
            }
            std::cout << "Native App source mission save completed\n" << std::flush;
            const bool missionMoved = initialPlayerForgerFound &&
                (savedForgerPosition.x != initialPlayerForgerPosition.x ||
                 savedForgerPosition.y != initialPlayerForgerPosition.y);
            const bool selectedObjective =
                MGlobals::IsObjectiveCompleted(kSelectForgerObjective);
            const bool movedObjective =
                MGlobals::IsObjectiveCompleted(kMoveForgerObjective);
            std::cout << "Native App player Forger moved in plane: " << missionMoved
                      << "; from " << initialPlayerForgerPosition.x << ','
                      << initialPlayerForgerPosition.y << " to "
                      << savedForgerPosition.x << ',' << savedForgerPosition.y << '\n';
            if (!MISSION->Load("native_slice.mission", SAVEDIR))
            {
                std::cerr << "Native App source mission reload failed\n";
                return 7;
            }
            if (options.HasUiFont() &&
                (!FrontierX::NativeSourceHintIdle() || !FrontierX::NativeFieldHintIdle()))
            { std::cerr << "Native source hint ownership survived mission reload\n"; return 7; }
            bool restoredForger = false;
            for (IBaseObject *object = OBJLIST->GetObjectList(); object;
                 object = object->next)
            {
                const char *name = object->pArchetype
                    ? ARCHLIST->GetArchName(object->pArchetype) : nullptr;
                if (name && std::strcmp(name, "FAB!!S_Forger") == 0 &&
                    object->GetPlayerID() == savedForgerOwner)
                {
                    const Vector position = object->GetPosition();
                    restoredForger = position.x == savedForgerPosition.x &&
                        position.y == savedForgerPosition.y &&
                        position.z == savedForgerPosition.z;
                    break;
                }
            }
            if (!restoredForger)
            {
                std::cerr << "Native App mission reload changed the player Forger state\n";
                return 7;
            }
            std::cout << "Native App source mission reload preserved player Forger owner "
                      << savedForgerOwner << " and position "
                      << savedForgerPosition.x << ',' << savedForgerPosition.y << ','
                      << savedForgerPosition.z << '\n' << std::flush;
            if (!missionMoved || !selectedObjective || !movedObjective ||
                !MGlobals::IsObjectiveCompleted(kSelectForgerObjective) ||
                !MGlobals::IsObjectiveCompleted(kMoveForgerObjective))
            {
                std::cerr << "Native App selected training checkpoint was not preserved\n";
                return 6;
            }
            std::cout << "Native App selected training checkpoint preserved after source reload\n";
            std::cout << "Native App source status/hint draws: " << FrontierX::NativeStatusDrawCount()
                      << '/' << FrontierX::NativeHintDrawCount() << '\n';
            std::cout << "Native App source unit-alert draws: " << FrontierX::NativeUnitAlertDrawCount() << '\n';
            if (options.missionAlertSmoke && !FrontierX::NativeUnitAlertDrawCount())
            { std::cerr << "Native source alert diagnostic did not draw\n"; return 7; }
            std::cout << "Native App source scrolling-text draws: " << FrontierX::NativeScrollingTextDrawCount() << '\n';
            if (options.HasUiFont() && !options.skipUnitVoice && !options.skipSubtitles &&
                !FrontierX::NativeScrollingTextDrawCount())
            { std::cerr << "Native source unit scrolling text did not draw\n"; return 7; }
            std::cout << "Native App source subtitle draws: " << FrontierX::NativeSubtitleDrawCount() << '\n';
            if (!options.skipSubtitles && !options.skipTrainingSpeech && !FrontierX::NativeSubtitleDrawCount())
            { std::cerr << "Native source subtitles did not draw\n"; return 7; }
            std::cout << "Native App font draws (mission text owners): " << FrontierX::NativeFontDrawCount() << '\n';
            if (!options.skipTeletypeVisuals && options.skipTrainingSpeech && !FrontierX::NativeFontDrawCount())
            { std::cerr << "Native source teletype did not draw\n"; return 7; }
            FrontierX::NativeMissionAudioStats audioStats;
            if (!FrontierX::GetNativeMissionAudioStats(appSoundManager.Get(), audioStats)) return 7;
            std::cout << "Native App unit voice streams: started=" << audioStats.unitStarted
                      << " completed=" << audioStats.unitCompleted << " stopped=" << audioStats.unitStopped
                      << " active=" << audioStats.activeStreams << '\n';
            if (!options.skipUnitVoice && (audioStats.unitStarted < 2 ||
                    audioStats.unitCompleted + audioStats.unitStopped < 2))
            {
                std::cerr << "Native App unit voice selection/move acceptance failed\n";
                return 7;
            }
            if (options.missionHintCheck)
            {
                if (!FrontierX::NativeHintDrawCount() ||
                    FrontierX::NativeHintDrawResource() != IDS_HINT_FIELD_CYGNUS)
                { std::cerr << "Native gameplay Cygnus hint was not drawn\n"; return 7; }
                const auto beforeReloadHover = FrontierX::NativeHintDrawCount();
                CQFLAGS.bGameActive = -1;
                EVENTSYS->Send(CQE_GAME_ACTIVE, reinterpret_cast<void *>(1));
                MSCROLL->SetActive(1);
                modalExit.started = FrontierX::Platform::MonotonicMilliseconds();
                modalExit.duration = 6000;
                modalExit.frames = 0;
                modalExit.requested = false;
                if (!modalExitConnection.Connect(modalPoint.Get(), &modalExit)) return 7;
                std::cout << "Native App reloaded hint observation started; use normal hover\n" << std::flush;
                const U32 reloadedResult = CQDoModal(nullptr);
                modalExitConnection.Disconnect();
                if (reloadedResult != MISSION_END_QUIT || shellCloseRequested ||
                    FrontierX::NativeHintDrawCount() <= beforeReloadHover ||
                    FrontierX::NativeHintDrawResource() != IDS_HINT_FIELD_CYGNUS)
                { std::cerr << "Native reloaded gameplay hint was not drawn\n"; return 7; }
                std::cout << "Native App reloaded gameplay hint drawn through source modal\n";
            }
            CQFLAGS.bGameActive = 0;
            EVENTSYS->Send(CQE_GAME_ACTIVE, nullptr);
            MISSION->Close(TRUE);
            if (options.HasUiFont() &&
                (!FrontierX::NativeSourceHintIdle() || !FrontierX::NativeFieldHintIdle()))
            { std::cerr << "Native source hint ownership survived mission close\n"; return 7; }
            std::cout << "Native App source hint text, timers and ownership clear after reload/close\n";
            std::cout << "Native App source mission closed after checkpoint\n";
            return 0;
        }
    }
    if (!options.vertexShaderPath.empty() && !options.missionGpu)
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
            nullptr, nullptr, FrontierX::SdlGpuRenderer::SamplerStateCallback,
            FrontierX::SdlGpuRenderer::ViewportCallback);
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
             FrontierX::SdlGpuRenderer::TextureDestroyCallback,
             FrontierX::SdlGpuRenderer::TextureCreateMipsCallback, FrontierX::SdlGpuRenderer::TextureUploadMipsCallback});
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
    if (options.missionGpu)
    {
        FrontierX::SdlGpuReadback readback;
        std::string renderError;
        if (!presentedFrames || !renderer.Readback64(readback, renderError) ||
            !readback.structureValid)
        {
            std::cerr << "Native mission GPU readback failed: " << renderError << '\n';
            return 7;
        }
        std::cout << "Native mission GPU presented blank context; frames: "
                  << presentedFrames << "; changed-pixels: "
                  << readback.changedPixels << "; mission not started\n";
        missionGpuUnbind.Disconnect();
        renderer.Shutdown();
    }
    else if (!options.vertexShaderPath.empty())
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
    frameLimitRestore.Restore();
    if (userDefaults.Get()->StoreDefaults() == FALSE)
    {
        std::cerr << "Native preference save failed\n";
        return 5;
    }
    if (options.missionGpu)
        std::cout << "Native shell closed cleanly; mission GPU context ran, game startup not attempted\n";
    else if (options.vertexShaderPath.empty())
        std::cout << "Native shell closed cleanly; retail game startup and rendering were not attempted\n";
    else
        std::cout << "Native shell closed cleanly; repository render fixture completed; "
                     "game startup was not attempted\n";
    return 0;
}
}

int main(int argc, char ** argv)
{
    struct SdlRuntimeCleanup
    {
        ~SdlRuntimeCleanup() { FrontierX::ShutdownSdlRuntime(); }
    } sdlRuntimeCleanup;
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
