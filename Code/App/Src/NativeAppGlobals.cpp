#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include "DEffectOpts.h"
#include "IVertexBufferManager.h"
#include "EventSys.h"
#include "WindowManager.h"
#include "Filesys.h"
#include "Streamer.h"
#include "MusicManager.h"
#include "SFX.h"
#include "GenData.h"
#include "IParticleManager.h"

#include <inv_sqrt.h>
#include <cstring>

ISQRT SQRT;

static InterfaceRes nativeInterfaceRes = IR_NO_RESOLUTION;

InterfaceRes __stdcall GetCurrentInterfaceRes()
{
    return nativeInterfaceRes;
}

void __stdcall ChangeInterfaceRes(InterfaceRes resolution)
{
    if (nativeInterfaceRes == resolution) return;
    switch (nativeInterfaceRes)
    {
    case IR_IN_GAME_RESOLUTION: EVENTSYS->Send(CQE_LEAVING_INGAMEMODE); break;
    case IR_FRONT_END_RESOLUTION: EVENTSYS->Send(CQE_LEAVING_FRONTENDMODE); break;
    default: break;
    }
    nativeInterfaceRes = resolution;
    switch (resolution)
    {
    case IR_IN_GAME_RESOLUTION:
        // Original System 3D entry restores the HUD after mission movie mode.
        CQFLAGS.bNoToolbar = CQFLAGS.bMovieMode = 0;
        SCREEN_WIDTH = 640;
        SCREEN_HEIGHT = 480;
        EVENTSYS->Send(CQE_ENTERING_INGAMEMODE);
        break;
    case IR_FRONT_END_RESOLUTION:
        SCREEN_WIDTH = 800;
        SCREEN_HEIGHT = 600;
        EVENTSYS->Send(CQE_ENTERING_FRONTENDMODE);
        break;
    default: break;
    }
    EVENTSYS->Send(CQE_INTERFACE_RES_CHANGE);
}


// The selected training mission uses ASCII names. Reject other code pages
// until their Windows CP_ACP mapping can be specified for native data paths.
int __stdcall _localAnsiToWide(const char *input, wchar_t *output, U32 bufferSize)
{
    if (!input || !output || bufferSize < sizeof(wchar_t)) return 0;
    const size_t capacity = bufferSize / sizeof(wchar_t);
    size_t index = 0;
    for (; index < capacity; ++index)
    {
        const unsigned char value = static_cast<unsigned char>(input[index]);
        if (value > 0x7f) { output[0] = L'\0'; return 0; }
        output[index] = static_cast<wchar_t>(value);
        if (!value) return static_cast<int>(index + 1);
    }
    output[0] = L'\0';
    return 0;
}

int __stdcall _localWideToAnsi(const wchar_t *input, char *output, U32 bufferSize)
{
    if (!input || !output || !bufferSize) return 0;
    for (size_t index = 0; index < bufferSize; ++index)
    {
        const wchar_t value = input[index];
        if (value < 0 || value > 0x7f) { output[0] = '\0'; return 0; }
        output[index] = static_cast<char>(value);
        if (!value) return static_cast<int>(index + 1);
    }
    output[0] = '\0';
    return 0;
}

BaseHotRect *TOOLBAR = nullptr;
BaseHotRect *FULLSCREEN = nullptr;
SINGLE LODPERCENT = 0.0F;
SINGLE ELAPSED_TIME = DEF_ELAPSED_TIME;
SINGLE REALTIME_FRAMERATE = DEF_REALTIME_FRAMERATE;
U32 TEXMEMORYUSED = 0;
bool NATIVE_SKIP_UNIT_LABELS = false;
bool NATIVE_SKIP_PLANET_UI = false;
bool NATIVE_SKIP_SUBTITLES = false;
S32 camMoved = 0;
S32 EXPCOUNT = 0;
CQGlobalFlags CQFLAGS = {};
bool NATIVE_SKIP_LEGACY_LIGHTING = false;
bool NATIVE_USE_ORIGINAL_LEGACY_MATERIALS = false;
GlobalRenderFlags CQRENDERFLAGS = {};
GlobalEffectsOptions CQEFFECTS = {};
ICOManager *DACOM = nullptr;
IRenderPipeline *PIPE = nullptr;
IRenderPrimitive *BATCH = nullptr;
ICQBatch *CQBATCH = nullptr;
ICQ_VB_Manager *vb_mgr = nullptr;
IRenderer *REND = nullptr;
IEngine *ENGINE = nullptr;
ILightManager *LIGHT = nullptr;
ILights *LIGHTS = nullptr;
CQLight *MAINLIGHT = nullptr;
CQLight *CAMERALIGHT = nullptr;
IDAComponent *GS = nullptr;
IEventSystem *EVENTSYS = nullptr;
IWindowManager *WM = nullptr;
IBaseCamera *CAMERA = nullptr;
ICamera *MAINCAM = nullptr;
ISector *SECTOR = nullptr;
IBackground *BACKGROUND = nullptr;
IObjectList *OBJLIST = nullptr;
IArchetypeList *ARCHLIST = nullptr;
IMission *MISSION = nullptr;
IComponentFactory *PARSER = nullptr;
IFileTransfer *FILETRANSFER = nullptr;
IGeneralData *GENDATA = nullptr;
IEffectPlayer *EFFECTPLAYER = nullptr;
IFieldManager *FIELDMGR = nullptr;
IMapGen *MAPGEN = nullptr;
IDebugFontDrawAgent *DEBUGFONT = nullptr;
IHotkeyEvent *HOTKEY = nullptr;
IHotkeyEvent *DBHOTKEY = nullptr;
IAnim2D *ANIM2D = nullptr;
IAnimation *ANIM = nullptr;
IHardpoint *HARDPOINT = nullptr;
IBanker *BANKER = nullptr;
IDust *DUSTMANAGER = nullptr;
IGamePositionCallback *GAMEPOSCALLBACK = nullptr;
extern "C" {
const COLORREF DEFCOLORTABLE[9] = {
    RGB(100, 100, 100), RGB(255, 255, 0), RGB(240, 0, 0),
    RGB(56, 52, 255), RGB(255, 0, 255), RGB(18, 200, 0),
    RGB(255, 150, 0), RGB(128, 0, 255), RGB(85, 218, 240)
};
COLORREF COLORTABLE[9] = {};
COLORREF SECTORCOLORTABLE[16] = {
    RGB(255,255,255), RGB(255,0,0), RGB(0,255,0), RGB(0,0,255),
    RGB(255,255,0), RGB(0,255,255), RGB(255,0,255), RGB(0,128,255),
    RGB(255,0,128), RGB(128,255,0), RGB(128,0,255), RGB(255,128,0),
    RGB(0,255,128), RGB(128,128,255), RGB(255,128,128), RGB(128,255,128)
};
}
ITextureLibrary *TEXLIB = nullptr;
ITManager *TMANAGER = nullptr;
IMaterialManager *MATMAN = nullptr;
IVideoSystem *VIDEOSYS = nullptr;
IFileSystem *TEXTURESDIR = nullptr;
IFileSystem *OBJECTDIR = nullptr;
IFileSystem *INTERFACEDIR = nullptr;
IFileSystem *SPMAPDIR = nullptr;
IFileSystem *MATERIALDIR = nullptr;
IFileSystem *PROFILEDIR = nullptr;
IFileSystem *GAMETYPESDIR = nullptr;
IFileSystem *NETOUTPUT = nullptr;
IFileSystem *SAVEDIR = nullptr;
U32 BUILDARCHEID = 0;
IOpAgent *THEMATRIX = nullptr;
IUserDefaults *DEFAULTS = nullptr;
DWORD PLAYERID = 0;
DWORD HOSTID = 0;
DWORD SCREENRESX = 0;
DWORD SCREENRESY = 0;
S32 SCREEN_WIDTH = 0;
S32 SCREEN_HEIGHT = 0;
INetBuffer *NETBUFFER = nullptr;
INetPacket *NETPACKET = nullptr;
U32 VBMEMORYUSED = 0;
IVertexBufferManager *VB_MANAGER = nullptr;
ICursorResource *CURSOR = nullptr;
INuggetManager *NUGGETMANAGER = nullptr;
IFogOfWar *FOGOFWAR = nullptr;
IObjMap *OBJMAP = nullptr;
IUnbornMeshList *UNBORNMANAGER = nullptr;
ICommTrack *COMMTRACK = nullptr;
IParticleManager *PARTMAN = nullptr;
ObjMapIterator *DEBUG_ITERATOR = nullptr;
IEventScheduler *SCHEDULER = nullptr;
IMeshManager *MESHMAN = nullptr;
IMovieCameraManager *CAMERAMANAGER = nullptr;
ISystemMap *SYSMAP = nullptr;
ISoundManager *SOUNDMANAGER = nullptr;
ITeletype *TELETYPE = nullptr;
ISubtitle *SUBTITLE = nullptr;
ILineManager *LINEMAN = nullptr;
IInterfaceManager *INTERMAN = nullptr;
IBriefing *BRIEFING = nullptr;
IGameProgress *GAMEPROGRESS = nullptr;
IStringTable *STRINGTABLE = nullptr;
IScripting *SCRIPTING = nullptr;
ICQImage *CQIMAGE = nullptr;
IFileSystem *SCRIPTSDIR = nullptr;
IFileSystem *MSPEECHDIR = nullptr;
IFileSystem *SPEECHDIR = nullptr;
IFileSystem *MUSICDIR = nullptr;
IFileSystem *SFXDIR = nullptr;
ISFX *SFXMANAGER = nullptr;
IStreamer *STREAMER = nullptr;
IMusicManager *MUSICMANAGER = nullptr;

S32 __fastcall ConvertFloatToDSound(SINGLE volume)
{
    if (volume > 1.0F) volume = 1.0F;
    else if (volume < 0.0F) volume = 0.0F;
    if (volume == 0.0F) return -10000;
    return static_cast<S32>((1.0F - volume) * -3000.0F);
}
IMouseScroll *MSCROLL = nullptr;
IUnitComm *UNITCOMM = nullptr;
IStringData *STRINGDATA = nullptr;

BOOL32 __stdcall RecursiveDelete(IFileSystem * file)
{
    // Saving must never erase the mounted retail archives, which are read-only.
    if (!file || !(file->GetAccessType() & GENERIC_WRITE)) return FALSE;

    WIN32_FIND_DATA data = {};
    HANDLE search = file->FindFirstFile("*.*", &data);
    if (search == INVALID_HANDLE_VALUE)
        return file->GetLastError() == ERROR_FILE_NOT_FOUND ||
               file->GetLastError() == ERROR_NO_MORE_FILES;

    BOOL32 result = TRUE;
    do
    {
        if (std::strcmp(data.cFileName, ".") == 0 ||
            std::strcmp(data.cFileName, "..") == 0) continue;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (!file->SetCurrentDirectory(data.cFileName)) result = FALSE;
            else
            {
                const BOOL32 childResult = RecursiveDelete(file);
                const BOOL32 restored = file->SetCurrentDirectory("..");
                if (!childResult || !restored ||
                    !file->RemoveDirectory(data.cFileName)) result = FALSE;
            }
        }
        else if (!file->DeleteFile(data.cFileName)) result = FALSE;
    } while (file->FindNextFile(search, &data));

    if (file->GetLastError() != ERROR_NO_MORE_FILES || !file->FindClose(search))
        result = FALSE;
    return result;
}

namespace FrontierX
{
NativeAppSaveBinding::~NativeAppSaveBinding()
{
    if (SAVEDIR == save_) SAVEDIR = nullptr;
    if (save_) save_->Release();
}

bool NativeAppSaveBinding::Bind(IFileSystem * saveDirectory)
{
    if (!saveDirectory || SAVEDIR || save_ ||
        !(saveDirectory->GetAccessType() & GENERIC_WRITE)) return false;
    saveDirectory->AddRef();
    save_ = saveDirectory;
    SAVEDIR = saveDirectory;
    return true;
}

NativeAppSystemBinding::~NativeAppSystemBinding()
{
    if (WM == window_manager_) WM = nullptr;
    if (window_manager_) window_manager_->Release();
    if (EVENTSYS == event_system_) EVENTSYS = nullptr;
    if (event_system_) event_system_->Release();
    if (GS == system_) GS = nullptr;
    if (system_) system_->Release();
}

bool NativeAppSystemBinding::Bind(IDAComponent * system)
{
    if (!system || GS || EVENTSYS || WM || system_ || event_system_ ||
        window_manager_) return false;
    IEventSystem *event = nullptr;
    const GENRESULT result = system->QueryInterface(IID_IEventSystem,
        reinterpret_cast<void **>(&event));
    if (result != GR_OK || !event)
    {
        if (event) event->Release();
        return false;
    }
    IWindowManager *window = nullptr;
    if (system->QueryInterface(IID_IWindowManager,
            reinterpret_cast<void **>(&window)) != GR_OK || !window)
    {
        if (window) window->Release();
        event->Release();
        return false;
    }
    system->AddRef();
    system_ = system;
    GS = system;
    event_system_ = event;
    EVENTSYS = event;
    window_manager_ = window;
    WM = window;
    return true;
}

NativeAppGeneralDataBinding::~NativeAppGeneralDataBinding()
{
    if (GENDATA == data_) GENDATA = nullptr;
    if (data_) static_cast<IDAComponent *>(data_)->Release();
}

NativeAppParticleBinding::~NativeAppParticleBinding()
{
    if (PARTMAN == manager_) PARTMAN = nullptr;
    if (manager_) manager_->Release();
}

bool NativeAppParticleBinding::Bind(IDAComponent * system)
{
    if (!system || PARTMAN || manager_) return false;
    IParticleManager *manager = nullptr;
    if (system->QueryInterface(IID_IParticleManager,
            reinterpret_cast<void **>(&manager)) != GR_OK || !manager)
    {
        if (manager) manager->Release();
        return false;
    }
    manager_ = manager;
    PARTMAN = manager;
    return true;
}

bool NativeAppGeneralDataBinding::Bind(IGeneralData * data)
{
    if (!data || GENDATA || data_) return false;
    static_cast<IDAComponent *>(data)->AddRef();
    data_ = data;
    GENDATA = data;
    return true;
}

NativeAppDataSources::~NativeAppDataSources()
{
    if (TEXTURESDIR == textures_) TEXTURESDIR = nullptr;
    if (textures_) textures_->Release();
    if (INTERFACEDIR == interface_) INTERFACEDIR = nullptr;
    if (interface_) interface_->Release();
    if (PROFILEDIR == profiles_) PROFILEDIR = nullptr;
    if (MATERIALDIR == materials_) MATERIALDIR = nullptr;
    if (SPMAPDIR == campaign_) SPMAPDIR = nullptr;
    if (OBJECTDIR == objects_) OBJECTDIR = nullptr;
    if (materials_) materials_->Release();
    if (profiles_) profiles_->Release();
    if (campaign_) campaign_->Release();
    if (objects_) objects_->Release();
}

bool NativeAppDataSources::BindInterface(IFileSystem * interfaceDirectory)
{
    if (!interfaceDirectory || interface_ || INTERFACEDIR) return false;
    interfaceDirectory->AddRef();
    interface_ = INTERFACEDIR = interfaceDirectory;
    return true;
}

bool NativeAppDataSources::BindTextures(IFileSystem * textureDirectory)
{
    if (!textureDirectory || textures_ || TEXTURESDIR) return false;
    textureDirectory->AddRef();
    textures_ = TEXTURESDIR = textureDirectory;
    return true;
}

bool NativeAppDataSources::Bind(IFileSystem * objects, IFileSystem * campaign,
                                IFileSystem * materials, IFileSystem * profiles)
{
    if (!objects || !campaign || !materials || !profiles ||
        OBJECTDIR || SPMAPDIR || MATERIALDIR || PROFILEDIR) return false;
    objects->AddRef();
    campaign->AddRef();
    materials->AddRef();
    profiles->AddRef();
    objects_ = objects;
    campaign_ = campaign;
    materials_ = materials;
    profiles_ = profiles;
    OBJECTDIR = objects;
    SPMAPDIR = campaign;
    MATERIALDIR = materials;
    PROFILEDIR = profiles;
    return true;
}

NativeAppAudioBinding::~NativeAppAudioBinding()
{
    if (MSPEECHDIR == missionSpeech_) MSPEECHDIR = nullptr;
    if (SPEECHDIR == speech_) SPEECHDIR = nullptr;
    if (SFXDIR == effects_) SFXDIR = nullptr;
    if (MUSICDIR == music_) MUSICDIR = nullptr;
    if (STREAMER == streamer_) STREAMER = nullptr;
    if (effects_) effects_->Release();
    if (missionSpeech_) missionSpeech_->Release();
    if (speech_) speech_->Release();
    if (music_) music_->Release();
    if (streamer_) streamer_->Release();
}

bool NativeAppAudioBinding::Bind(IStreamer *streamer, IFileSystem *music,
                                IFileSystem *effects, IFileSystem *speech,
                                IFileSystem *missionSpeech)
{
    if (!streamer || !music || !effects ||
        streamer_ || music_ || effects_ || speech_ || missionSpeech_ ||
        STREAMER || MUSICDIR || SFXDIR || SPEECHDIR || MSPEECHDIR)
        return false;
    streamer->AddRef();
    music->AddRef();
    effects->AddRef();
    if (speech) speech->AddRef();
    if (missionSpeech) missionSpeech->AddRef();
    streamer_ = STREAMER = streamer;
    music_ = MUSICDIR = music;
    effects_ = SFXDIR = effects;
    speech_ = SPEECHDIR = speech;
    missionSpeech_ = MSPEECHDIR = missionSpeech;
    return true;
}

NativeAppMusicBinding::~NativeAppMusicBinding()
{
    if (MUSICMANAGER == manager_) MUSICMANAGER = nullptr;
    if (manager_) manager_->Release();
}

bool NativeAppMusicBinding::Bind(IMusicManager *manager)
{
    if (!manager || manager_ || MUSICMANAGER || !STREAMER || !MUSICDIR)
        return false;
    manager->AddRef();
    manager_ = MUSICMANAGER = manager;
    return true;
}

NativeAppSfxBinding::~NativeAppSfxBinding()
{
    if (SFXMANAGER == manager_) SFXMANAGER = nullptr;
    if (manager_) manager_->Release();
}

bool NativeAppSfxBinding::Bind(ISFX *manager)
{
    if (!manager || manager_ || SFXMANAGER || !STREAMER || !SFXDIR)
        return false;
    static_cast<IDAComponent *>(manager)->AddRef();
    manager_ = SFXMANAGER = manager;
    return true;
}

NativeAppGameTypesBinding::~NativeAppGameTypesBinding()
{
    if (GAMETYPESDIR == source_) GAMETYPESDIR = nullptr;
    if (source_) source_->Release();
}

bool NativeAppGameTypesBinding::Bind(IFileSystem * source)
{
    if (!source || source_ || GAMETYPESDIR) return false;
    source->AddRef();
    source_ = source;
    GAMETYPESDIR = source;
    return true;
}
}
#endif

struct IMenuResource *MENU = nullptr;
