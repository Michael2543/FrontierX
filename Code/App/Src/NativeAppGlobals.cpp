#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include "DEffectOpts.h"
#include "IVertexBufferManager.h"
#include "EventSys.h"
#include "WindowManager.h"
#include "Filesys.h"
#include "Streamer.h"
#include "MusicManager.h"
#include "GenData.h"

#include <cstring>

SINGLE LODPERCENT = 0.0F;
SINGLE ELAPSED_TIME = DEF_ELAPSED_TIME;
SINGLE REALTIME_FRAMERATE = DEF_REALTIME_FRAMERATE;
S32 camMoved = 0;
CQGlobalFlags CQFLAGS = {};
GlobalRenderFlags CQRENDERFLAGS = {};
GlobalEffectsOptions CQEFFECTS = {};
ICOManager *DACOM = nullptr;
IRenderPipeline *PIPE = nullptr;
IRenderPrimitive *BATCH = nullptr;
ICQBatch *CQBATCH = nullptr;
IRenderer *REND = nullptr;
IEngine *ENGINE = nullptr;
ILightManager *LIGHT = nullptr;
ILights *LIGHTS = nullptr;
IDAComponent *GS = nullptr;
IEventSystem *EVENTSYS = nullptr;
IWindowManager *WM = nullptr;
IBaseCamera *CAMERA = nullptr;
ICamera *MAINCAM = nullptr;
ISector *SECTOR = nullptr;
IObjectList *OBJLIST = nullptr;
IArchetypeList *ARCHLIST = nullptr;
IMission *MISSION = nullptr;
IComponentFactory *PARSER = nullptr;
IFileTransfer *FILETRANSFER = nullptr;
IGeneralData *GENDATA = nullptr;
extern "C" {
const COLORREF DEFCOLORTABLE[9] = {
    RGB(100, 100, 100), RGB(255, 255, 0), RGB(240, 0, 0),
    RGB(56, 52, 255), RGB(255, 0, 255), RGB(18, 200, 0),
    RGB(255, 150, 0), RGB(128, 0, 255), RGB(85, 218, 240)
};
COLORREF COLORTABLE[9] = {};
}
ITextureLibrary *TEXLIB = nullptr;
ITManager *TMANAGER = nullptr;
IMaterialManager *MATMAN = nullptr;
IFileSystem *TEXTURESDIR = nullptr;
IFileSystem *OBJECTDIR = nullptr;
IFileSystem *SPMAPDIR = nullptr;
IFileSystem *MATERIALDIR = nullptr;
IFileSystem *GAMETYPESDIR = nullptr;
IFileSystem *NETOUTPUT = nullptr;
IFileSystem *SAVEDIR = nullptr;
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
ICQ_VB_Manager *vb_mgr = nullptr;
IVertexBufferManager *VB_MANAGER = nullptr;
ICursorResource *CURSOR = nullptr;
INuggetManager *NUGGETMANAGER = nullptr;
IFogOfWar *FOGOFWAR = nullptr;
IObjMap *OBJMAP = nullptr;
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
IFileSystem *MUSICDIR = nullptr;
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
    if (MATERIALDIR == materials_) MATERIALDIR = nullptr;
    if (SPMAPDIR == campaign_) SPMAPDIR = nullptr;
    if (OBJECTDIR == objects_) OBJECTDIR = nullptr;
    if (materials_) materials_->Release();
    if (campaign_) campaign_->Release();
    if (objects_) objects_->Release();
}

bool NativeAppDataSources::Bind(IFileSystem * objects, IFileSystem * campaign,
                                IFileSystem * materials)
{
    if (!objects || !campaign || !materials ||
        OBJECTDIR || SPMAPDIR || MATERIALDIR) return false;
    objects->AddRef();
    campaign->AddRef();
    materials->AddRef();
    objects_ = objects;
    campaign_ = campaign;
    materials_ = materials;
    OBJECTDIR = objects;
    SPMAPDIR = campaign;
    MATERIALDIR = materials;
    return true;
}

NativeAppAudioBinding::~NativeAppAudioBinding()
{
    if (MUSICDIR == music_) MUSICDIR = nullptr;
    if (STREAMER == streamer_) STREAMER = nullptr;
    if (music_) music_->Release();
    if (streamer_) streamer_->Release();
}

bool NativeAppAudioBinding::Bind(IStreamer *streamer, IFileSystem *music)
{
    if (!streamer || !music || streamer_ || music_ || STREAMER || MUSICDIR)
        return false;
    streamer->AddRef();
    music->AddRef();
    streamer_ = STREAMER = streamer;
    music_ = MUSICDIR = music;
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
