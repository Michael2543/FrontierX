#ifndef FRONTIERX_NATIVE_APP_GLOBALS_H
#define FRONTIERX_NATIVE_APP_GLOBALS_H

#if !defined(_WIN32)
#include "FrontierX/LegacyFileTypes.h"
#include "FrontierX/LegacyWindowTypes.h"
#include "FrontierX/NativeAppMath.h"
#include "GlobalFlagTypes.h"
#include "AppPacketTypes.h"
#include "AppObjectContracts.h"

struct GENNODE;
typedef GENNODE *PGENTYPE;

// Native declarations of the App globals needed by the active mesh caller.
// The supported Windows declarations and definitions remain in Globals.h.
extern SINGLE LODPERCENT;
extern SINGLE ELAPSED_TIME;
extern SINGLE REALTIME_FRAMERATE;
#define DEF_ELAPSED_TIME (8.0F / 30.0F)
#define DEF_REALTIME_FRAMERATE (30.0F / 8)
#define RENDER_PERIOD (1.0F / 30.0F)
#define RENDER_FRAMERATE 30.0F
#define MAX_SYSTEMS 16
#define NUM_CIRCLE_SEGS 80
#define REAL2IDEALX(x) ((((x) * SCREEN_WIDTH) / (S32)SCREENRESX))
#define REAL2IDEALY(y) ((((y) * SCREEN_HEIGHT) / (S32)SCREENRESY))
#define IDEAL2REALX(x) ((((x) * ((S32)SCREENRESX) / SCREEN_WIDTH)))
#define IDEAL2REALY(y) ((((y) * ((S32)SCREENRESY) / SCREEN_HEIGHT)))
#define HYPER_SYSTEM_MASK 0x80
#define DBG_SOME -1
#define DBG_ALL 1
#define CQE_NEW_SELECTION 0x1000
#define CQE_HOTKEY 0x1003
#define CQE_KILL_FOCUS 0x1005
#define CQE_SET_FOCUS 0x1006
#define CQE_VIEW_OBJECT 0x1011
#define CQE_EDITOR_MODE 0x1013
#define CQE_PART_SELECTION 0x1024
#define CQE_DEBUG_HOTKEY 0x1026
#define CQE_HOST_MIGRATE 0x102F
#define CQE_GOTO_PART 0x1030
#define CQE_SELECTION_EVENT 0x1031
#define CQE_NETPACKET 0x100E
#define CQE_ENDFRAME 0x1001
#define CQE_NETSTARTUP 0x1009
#define CQE_NETSHUTDOWN 0x100A
#define CQE_ADDPLAYER 0x100C
#define CQE_NETADDPLAYER 0x1017
#define CQE_NETPAUSED 0x101B
#define CQE_LOCALPAUSED 0x101C
#define CQE_DPLAY_MSGWAITING 0x1023
#define CQE_PAUSE_WARNING 0x103C
#define CQE_NETDELETEPLAYER 0x1042
#define CQE_START3DMODE 0x105F
#define CQE_DELETEPLAYER 0x100D
#define CQE_UPDATE 0x1010
#define CQE_CAMERA_MOVED 0x100F
#define CQE_SYSTEM_CHANGED 0x1018
#define CQE_DLG_RESULT 0x1020
#define CQE_MISSION_ENDING 0x1025
#define CQE_GAME_ACTIVE 0x102B
#define CQE_MOVIE_MODE 0x1052
#define CQE_MISSION_PROG_ENDING 0x1050
#define CQE_PLAYER_RESIGN 0x1056
#define CQE_PLAYER_QUIT 0x1059
#define CQE_MISSION_ENDING_SPLASH 0x105D
#define CQE_ENTERING_INGAMEMODE 0x1062
#define CQE_LEAVING_INGAMEMODE 0x1061
#define CQE_MISSION_CLOSE 0x103A
#define CQE_PAUSE_CHANGE 0x103B
#define CQE_MISSION_LOAD_COMPLETE 0x103E
#define CQE_STREAMER 0x1016
#define CQE_NEWHOST 0x100B
#define CQE_IDLE_UNIT 0x1039
#define CQE_OBJECT_DESTROYED 0x1048
#define MISSION_END_RESIGN 1
#define MISSION_END_WON 2
#define MISSION_END_LOST 3
#define MISSION_END_QUIT 4
#define MISSION_END_SPLASH 5
extern S32 camMoved;
extern CQGlobalFlags CQFLAGS;
extern GlobalRenderFlags CQRENDERFLAGS;
extern struct ICOManager *DACOM;
extern struct IRenderPipeline *PIPE;
extern struct IRenderPrimitive *BATCH;
extern struct ICQBatch *CQBATCH;
extern struct IRenderer *REND;
extern struct IEngine *ENGINE;
extern struct ILightManager *LIGHT;
extern struct ILights *LIGHTS;
extern struct IDAComponent *GS;
extern struct IBaseCamera *CAMERA;
extern struct ICamera *MAINCAM;
extern struct ISector *SECTOR;
extern struct IObjectList *OBJLIST;
extern struct IGeneralData *GENDATA;
extern struct IMusicManager *MUSICMANAGER;
extern struct IStreamer *STREAMER;
extern struct IFileSystem *MUSICDIR;
extern struct IComponentFactory *PARSER;
extern struct IMapGen *MAPGEN;
extern struct ISFX *SFXMANAGER;
extern struct INuggetManager *NUGGETMANAGER;
extern struct IAnim2D *ANIM2D;
extern struct IMovieCameraManager *CAMERAMANAGER;
extern struct ISoundManager *SOUNDMANAGER;
extern struct ITeletype *TELETYPE;
extern struct ISubtitle *SUBTITLE;
extern struct ILineManager *LINEMAN;
extern struct IInterfaceManager *INTERMAN;
extern struct IBriefing *BRIEFING;
extern struct IGameProgress *GAMEPROGRESS;
extern struct IStringTable *STRINGTABLE;
extern struct IScripting *SCRIPTING;
extern struct ICQImage *CQIMAGE;
extern struct IFileSystem *SCRIPTSDIR;
extern struct IFileSystem *MSPEECHDIR;
extern struct IMouseScroll *MSCROLL;
extern struct IUnitComm *UNITCOMM;
extern struct IEventScheduler *SCHEDULER;
extern struct IMeshManager *MESHMAN;
extern struct IParticleManager *PARTMAN;
extern struct IFogOfWar *FOGOFWAR;
extern struct IDust *DUSTMANAGER;
extern struct IFieldManager *FIELDMGR;
extern struct IEffectPlayer *EFFECTPLAYER;
extern struct IUnbornMeshList *UNBORNMANAGER;
extern struct ISystemMap *SYSMAP;
extern struct IDebugFontDrawAgent *DEBUGFONT;
extern struct IHotkeyEvent *DBHOTKEY;
extern "C" COLORREF COLORTABLE[9];
extern "C" const COLORREF DEFCOLORTABLE[9];
extern struct IObjMap *OBJMAP;
extern struct PrimitiveBuilder2 PB;
void __stdcall DisableTextures();
S32 __fastcall ConvertFloatToDSound(SINGLE volume);
BOOL32 __stdcall RecursiveDelete(struct IFileSystem *file);
void AddToGlobalCleanupList(struct IDAComponent **component);
void CleanupGlobals(void);
void __stdcall CreateGlobalComponents(void);
void __stdcall CreateInProgressAnim(struct IPANIM **animation);
void __fastcall UnregisterWatchersForObject(struct IBaseObject *object);
void __fastcall UnregisterWatchersForObjectForPlayerMask(struct IBaseObject *object, U32 playerMask);
BOOL32 __stdcall RectIntersects(const RECT &left, const RECT &right);
struct _pane;
namespace DA {
void RectangleHash(const _pane *pane, S32 x0, S32 y0, S32 x1, S32 y1, COLORREF color);
void RectangleFill(const _pane *pane, S32 x0, S32 y0, S32 x1, S32 y1, COLORREF color);
}
void OrthoView(const _pane *pane = nullptr);
extern class ObjMapIterator *DEBUG_ITERATOR;
extern struct IEventSystem *EVENTSYS;
extern struct IWindowManager *WM;
extern struct IScrollingText *SCROLLTEXT;
extern struct IArchetypeList *ARCHLIST;
extern struct IMission *MISSION;
extern struct IFileSystem *SAVEDIR;
extern struct IFileSystem *NETOUTPUT;
extern struct IOpAgent *THEMATRIX;
extern struct IUserDefaults *DEFAULTS;
extern struct IFileTransfer *FILETRANSFER;
extern struct INetPacket *NETPACKET;
extern struct INetBuffer *NETBUFFER;
extern DWORD PLAYERID;
extern DWORD HOSTID;
extern DWORD SCREENRESX;
extern DWORD SCREENRESY;
extern S32 SCREEN_WIDTH;
extern S32 SCREEN_HEIGHT;
#define CQE_LOAD_TOOLBAR 0x102A
extern struct ITextureLibrary *TEXLIB;
extern struct ITManager *TMANAGER;
extern struct IMaterialManager *MATMAN;
extern struct IFileSystem *TEXTURESDIR;
extern struct IFileSystem *OBJECTDIR;
extern struct IFileSystem *SPMAPDIR;
extern struct IFileSystem *MATERIALDIR;
extern struct IFileSystem *GAMETYPESDIR;
extern struct IStatusBarResource *STATUS;
extern struct IHintResource *HINTBOX;
extern struct ICursorResource *CURSOR;
extern struct ICommTrack *COMMTRACK;
extern struct ICQ_VB_Manager *vb_mgr;
void __stdcall SetupDiffuseBlend(U32 texture_id, bool clamp);
U32 __stdcall GetBuildVersion(void);
U32 __stdcall GetBuildVersion(char *buffer, U32 bufferSize);
const char * __cdecl _localLoadString(U32 id);
const wchar_t * __cdecl _localLoadStringW(U32 id);
int __stdcall _localAnsiToWide(const char * input, wchar_t * output, U32 bufferSize);
int __stdcall _localWideToAnsi(const wchar_t * input, char * output, U32 bufferSize);

namespace FrontierX
{
class NativeAppSystemBinding
{
public:
    ~NativeAppSystemBinding();
    NativeAppSystemBinding() = default;
    NativeAppSystemBinding(const NativeAppSystemBinding &) = delete;
    NativeAppSystemBinding & operator=(const NativeAppSystemBinding &) = delete;
    bool Bind(IDAComponent * system);
private:
    IDAComponent *system_ = nullptr;
    IEventSystem *event_system_ = nullptr;
    IWindowManager *window_manager_ = nullptr;
};

class NativeAppGeneralDataBinding
{
public:
    ~NativeAppGeneralDataBinding();
    NativeAppGeneralDataBinding() = default;
    NativeAppGeneralDataBinding(const NativeAppGeneralDataBinding &) = delete;
    NativeAppGeneralDataBinding & operator=(const NativeAppGeneralDataBinding &) = delete;
    bool Bind(IGeneralData * data);
private:
    IGeneralData *data_ = nullptr;
};

class NativeAppDataSources
{
public:
    NativeAppDataSources() = default;
    ~NativeAppDataSources();
    NativeAppDataSources(const NativeAppDataSources &) = delete;
    NativeAppDataSources & operator=(const NativeAppDataSources &) = delete;

    bool Bind(IFileSystem * objects, IFileSystem * campaign,
              IFileSystem * materials);

private:
    IFileSystem *objects_ = nullptr;
    IFileSystem *campaign_ = nullptr;
    IFileSystem *materials_ = nullptr;
};

class NativeAppSaveBinding
{
public:
    NativeAppSaveBinding() = default;
    ~NativeAppSaveBinding();
    NativeAppSaveBinding(const NativeAppSaveBinding &) = delete;
    NativeAppSaveBinding & operator=(const NativeAppSaveBinding &) = delete;
    bool Bind(IFileSystem * saveDirectory);
private:
    IFileSystem *save_ = nullptr;
};

class NativeAppAudioBinding
{
public:
    NativeAppAudioBinding() = default;
    ~NativeAppAudioBinding();
    NativeAppAudioBinding(const NativeAppAudioBinding &) = delete;
    NativeAppAudioBinding & operator=(const NativeAppAudioBinding &) = delete;
    bool Bind(IStreamer *streamer, IFileSystem *music);
private:
    IStreamer *streamer_ = nullptr;
    IFileSystem *music_ = nullptr;
};

class NativeAppMusicBinding
{
public:
    NativeAppMusicBinding() = default;
    ~NativeAppMusicBinding();
    NativeAppMusicBinding(const NativeAppMusicBinding &) = delete;
    NativeAppMusicBinding & operator=(const NativeAppMusicBinding &) = delete;
    bool Bind(IMusicManager *manager);
private:
    IMusicManager *manager_ = nullptr;
};

class NativeAppGameTypesBinding
{
public:
    ~NativeAppGameTypesBinding();
    NativeAppGameTypesBinding() = default;
    NativeAppGameTypesBinding(const NativeAppGameTypesBinding &) = delete;
    NativeAppGameTypesBinding & operator=(const NativeAppGameTypesBinding &) = delete;
    bool Bind(IFileSystem * source);
private:
    IFileSystem *source_ = nullptr;
};
}
#endif

#endif
