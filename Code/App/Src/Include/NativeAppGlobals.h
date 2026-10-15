#ifndef FRONTIERX_NATIVE_APP_GLOBALS_H
#define FRONTIERX_NATIVE_APP_GLOBALS_H

#if !defined(_WIN32)
#include "FrontierX/LegacyFileTypes.h"
#include "FrontierX/LegacyWindowTypes.h"
#include "FrontierX/NativeAppMath.h"
#include "GlobalFlagTypes.h"
#include "AppPacketTypes.h"
#include "AppObjectContracts.h"

enum InterfaceRes
{
    IR_NO_RESOLUTION = 0,
    IR_IN_GAME_RESOLUTION,
    IR_FRONT_END_RESOLUTION,
};
InterfaceRes __stdcall GetCurrentInterfaceRes();
void __stdcall ChangeInterfaceRes(InterfaceRes resolution);

struct GENNODE;
struct BaseHotRect;
struct IMeshArchetype;
extern BaseHotRect *TOOLBAR;
extern BaseHotRect *FULLSCREEN;
bool NativeAppCheckOperationStorage();
bool NativeAppCheckObjectiveStorage();
void __stdcall CreateDrawAgent(struct IImageReader *reader, struct IDrawAgent **drawAgent,
    BOOL32 bHiRes = false, RECT *rect = nullptr);
typedef GENNODE *PGENTYPE;

// Native declarations of the App globals needed by the active mesh caller.
// The supported Windows declarations and definitions remain in Globals.h.
extern SINGLE LODPERCENT;
extern SINGLE ELAPSED_TIME;
extern SINGLE REALTIME_FRAMERATE;
extern U32 TEXMEMORYUSED;
extern bool NATIVE_SKIP_UNIT_LABELS;
extern bool NATIVE_SKIP_PLANET_UI;
extern bool NATIVE_SKIP_SUBTITLES;
#define DEF_ELAPSED_TIME (8.0F / 30.0F)
#define DEF_REALTIME_FRAMERATE (30.0F / 8)
#define RENDER_PERIOD (1.0F / 30.0F)
#define RENDER_FRAMERATE 30.0F
#define BASE_MAX_CREW 250
#define BASE_MAX_METAL 220
#define BASE_MAX_GAS 180
#define MAX_SYSTEMS 16
#define MAX_NUGGETS 3
#define LOWER_EXP_BOUND 5
#define UPPER_EXP_BOUND 10
#define NUM_CIRCLE_SEGS 80
#define FAB_MAX_QUEUE_SIZE 15
#define CQE_BUILDQUEUE_REMOVE 0x103F
#define CQE_TABCONTROL_CHANGE 0x1055
#define GAS_MULTIPLIER 25
#define METAL_MULTIPLIER 25
#define CREW_MULTIPLIER 10
#define SCREEN_HRIDEAL_WIDTH 1024
#define SCREEN_HRIDEAL_HEIGHT 768
#define HRIDEAL2IDEALX(x) (((x) * (S32)SCREEN_WIDTH) / SCREEN_HRIDEAL_WIDTH)
#define HRIDEAL2IDEALY(y) (((y) * (S32)SCREEN_HEIGHT) / SCREEN_HRIDEAL_HEIGHT)
#define HRIDEAL2REALX(x) (((x) * (S32)SCREENRESX) / SCREEN_HRIDEAL_WIDTH)
#define HRIDEAL2REALY(y) (((y) * (S32)SCREENRESY) / SCREEN_HRIDEAL_HEIGHT)
#define IDEAL2HRIDEALX(x) (((x) * SCREEN_HRIDEAL_WIDTH) / (S32)SCREEN_WIDTH)
#define IDEAL2HRIDEALY(y) (((y) * SCREEN_HRIDEAL_HEIGHT) / (S32)SCREEN_HEIGHT)
#define REAL2IDEALX(x) ((((x) * SCREEN_WIDTH) / (S32)SCREENRESX))
#define REAL2IDEALY(y) ((((y) * SCREEN_HEIGHT) / (S32)SCREENRESY))
#define IDEAL2REALX(x) ((((x) * ((S32)SCREENRESX) / SCREEN_WIDTH)))
#define IDEAL2REALY(y) ((((y) * ((S32)SCREENRESY) / SCREEN_HEIGHT)))
#define HYPER_SYSTEM_MASK 0x80
#define DBG_SOME -1
#define DBG_ALL 1
#define CQE_UNICHAR 0x1019
#define CQE_EDIT_CHANGED 0x1032
#define CQE_LDBLHOTBUTTON 0x1029
#define CQE_LOAD_TOOLBAR 0x102A
#define CQE_ADMIRALBAR_ACTIVE 0x1041
#define CQE_PANEL_OWNED 0x1002
#define CQE_BUTTON 0x1007
#define CQE_UNALERT 0x1008
#define CQE_KEYBOARD_FOCUS 0x101A
#define CQE_DELETE_HOTRECT 0x101E
#define CQE_LIST_SELECTION 0x1021
#define CQE_LIST_CARET_MOVED 0x1022
#define CQE_LHOTBUTTON 0x1027
#define CQE_RHOTBUTTON 0x1028
#define CQE_SLIDER 0x1037
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
#define CQE_LOAD_INTERFACE 0x101D
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
#define CQE_INTERFACE_RES_CHANGE 0x105E
#define CQE_START3DMODE 0x105F
#define CQE_END3DMODE 0x1060
#define CQE_UNIT_UNDER_ATTACK 0x1049
#define CQE_DELETEPLAYER 0x100D
#define CQE_UPDATE 0x1010
#define CQE_RENDER_LAST3D 0x1043
#define CQE_CAMERA_MOVED 0x100F
#define CQE_SYSTEM_CHANGED 0x1018
#define CQE_DLG_RESULT 0x1020
#define CQE_DELETE_OBJECTIVES_MENU 0x1047
#define CQE_OBJECT_PRECAPTURE 0x104C
#define CQE_OBJECT_POSTCAPTURE 0x104D
#define CQE_OBJECTIVES_CHANGE 0x1053
#define CQE_OBJECTIVE_ADDED 0x1054
#define CQE_FLUSHMESSAGES 0x1044
#define CQE_MISSION_ENDING 0x1025
#define CQE_GAME_ACTIVE 0x102B
#define CQE_MOVIE_MODE 0x1052
#define CQE_MISSION_PROG_ENDING 0x1050
#define CQE_FABRICATOR_FINISHED 0x1051
#define CQE_PLAYER_RESIGN 0x1056
#define CQE_PLAYER_QUIT 0x1059
#define CQE_MISSION_ENDING_SPLASH 0x105D
#define CQE_ENTERING_INGAMEMODE 0x1062
#define CQE_LEAVING_INGAMEMODE 0x1061
#define CQE_LEAVING_FRONTENDMODE 0x1063
#define CQE_ENTERING_FRONTENDMODE 0x1064
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
extern S32 EXPCOUNT;
extern CQGlobalFlags CQFLAGS;
extern bool NATIVE_SKIP_LEGACY_LIGHTING;
extern bool NATIVE_USE_ORIGINAL_LEGACY_MATERIALS;
extern GlobalRenderFlags CQRENDERFLAGS;
extern struct ICOManager *DACOM;
extern struct IRenderPipeline *PIPE;
extern struct IRenderPrimitive *BATCH;
extern struct ICQBatch *CQBATCH;
extern struct IRenderer *REND;
extern struct IEngine *ENGINE;
extern struct ILightManager *LIGHT;
extern struct ILights *LIGHTS;
extern struct CQLight *MAINLIGHT;
extern struct CQLight *CAMERALIGHT;
extern struct IDAComponent *GS;
extern struct IBaseCamera *CAMERA;
extern struct ICamera *MAINCAM;
extern struct ISector *SECTOR;
extern struct IObjectList *OBJLIST;
extern struct IGeneralData *GENDATA;
extern struct IMusicManager *MUSICMANAGER;
extern struct IStreamer *STREAMER;
extern struct IFileSystem *MUSICDIR;
extern struct IFileSystem *SFXDIR;
extern struct IComponentFactory *PARSER;
extern struct IHotkeyEvent *HOTKEY;
extern struct IMapGen *MAPGEN;
extern struct ISFX *SFXMANAGER;
extern struct INuggetManager *NUGGETMANAGER;
extern struct IAnim2D *ANIM2D;
extern struct IAnimation *ANIM;
extern struct IMovieCameraManager *CAMERAMANAGER;
extern struct ISoundManager *SOUNDMANAGER;
extern struct ITeletype *TELETYPE;
extern struct ISubtitle *SUBTITLE;
extern struct ILineManager *LINEMAN;
extern struct IInterfaceManager *INTERMAN;
extern struct IBriefing *BRIEFING;
extern struct IGameProgress *GAMEPROGRESS;
extern struct IStringTable *STRINGTABLE;
extern struct IStringData *STRINGDATA;
extern struct IScripting *SCRIPTING;
extern struct ICQImage *CQIMAGE;
extern struct IFileSystem *SCRIPTSDIR;
extern struct IFileSystem *MSPEECHDIR;
extern struct IFileSystem *SPEECHDIR;
extern struct IMouseScroll *MSCROLL;
extern struct IUnitComm *UNITCOMM;
extern struct IEventScheduler *SCHEDULER;
extern struct IMeshManager *MESHMAN;
extern struct IHardpoint *HARDPOINT;
extern struct IVertexBufferManager *VB_MANAGER;
extern struct IParticleManager *PARTMAN;
extern struct IFogOfWar *FOGOFWAR;
extern struct IDust *DUSTMANAGER;
extern struct IFieldManager *FIELDMGR;
struct IBaseObject * __stdcall CreateBlast(PARCHETYPE pArchetype,
    const class TRANSFORM &orientation, U32 systemID, SINGLE animScale=1.0F);
struct IBaseObject * __stdcall CreateTrail(PARCHETYPE pArchetype,
    struct IBaseObject *jumper, Vector target, U32 systemID);
struct IBaseObject * __stdcall CreateDebris(SINGLE lifeTime, BOOL32 explode,
    S32 instance, PARCHETYPE blastType, U32 systemID, struct PHYS_CHUNK *chunk);
struct IBaseObject * __stdcall CreateDebris(SINGLE lifeTime, BOOL32 explode,
    struct MeshChain *mesh, PARCHETYPE blastType, U32 systemID,
    struct PHYS_CHUNK *chunk);
void __stdcall EnumerateHardpoints(S32 parent, const char *pathName,
    struct IHPEnumerator *callback);
BOOL32 __stdcall FindHardpoint(const char *pathname, S32 &index,
    struct HardpointInfo &info, S32 parent);
extern struct IEffectPlayer *EFFECTPLAYER;
extern struct IGamePositionCallback *GAMEPOSCALLBACK;
extern struct IUnbornMeshList *UNBORNMANAGER;
extern U32 BUILDARCHEID;
extern struct ISystemMap *SYSMAP;
extern struct IDebugFontDrawAgent *DEBUGFONT;
extern struct IHotkeyEvent *DBHOTKEY;
extern "C" COLORREF COLORTABLE[9];
extern "C" const COLORREF DEFCOLORTABLE[9];
extern "C" COLORREF SECTORCOLORTABLE[16];
extern struct IObjMap *OBJMAP;
extern struct IBackground *BACKGROUND;
extern struct PrimitiveBuilder2 PB;
void __stdcall DisableTextures();
S32 __fastcall ConvertFloatToDSound(SINGLE volume);
BOOL32 __stdcall RecursiveDelete(struct IFileSystem *file);
void AddToGlobalCleanupList(struct IDAComponent **component);
void CleanupGlobals(void);
void __stdcall CreateGlobalComponents(void);
U32 __stdcall CQDoModal(BaseHotRect *dialog);
void BindNativeAppPrimitive(struct IRenderPrimitive *primitive);
BOOL32 LoadNativeSelectedMission(const C8 *name, struct IComponentFactory *source);
void __stdcall CreateInProgressAnim(struct IPANIM **animation);
void __fastcall UnregisterWatchersForObject(struct IBaseObject *object);
void __fastcall UnregisterSystemVolatileWatchersForObject(struct IBaseObject *object);
void __fastcall UnregisterWatchersForObjectForPlayerMask(struct IBaseObject *object, U32 playerMask);
BOOL32 __stdcall RectIntersects(const RECT &left, const RECT &right);
BOOL32 __stdcall RectIntersects(const RECT &rect, struct ViewPoint *points, S32 numPoints);
struct _pane;
namespace DA {
void LineDraw(const _pane *pane, S32 x0, S32 y0, S32 x1, S32 y1,
              COLORREF color, BOOL32 bSmooth = 0);
void RectangleHash(const _pane *pane, S32 x0, S32 y0, S32 x1, S32 y1, COLORREF color);
void RectangleFill(const _pane *pane, S32 x0, S32 y0, S32 x1, S32 y1, COLORREF color);
}
void OrthoView(const _pane *pane = nullptr);
namespace DA {
void DrawTexture(const _pane *pane, S32 x0, S32 y0, S32 x1, S32 y1,
                 U32 textureHandle, SINGLE u0, SINGLE v0, SINGLE u1, SINGLE v1);
}
extern class ObjMapIterator *DEBUG_ITERATOR;
extern struct IEventSystem *EVENTSYS;
extern struct IWindowManager *WM;
extern struct IScrollingText *SCROLLTEXT;
extern struct IArchetypeList *ARCHLIST;
extern struct IMission *MISSION;
extern struct IFileSystem *SAVEDIR;
extern struct IFileSystem *NETOUTPUT;
extern struct IOpAgent *THEMATRIX;
extern struct IBanker *BANKER;
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
extern struct IVideoSystem *VIDEOSYS;
extern struct IFileSystem *TEXTURESDIR;
extern struct IFileSystem *OBJECTDIR;
extern struct IFileSystem *INTERFACEDIR;
extern struct IFileSystem *SPMAPDIR;
extern struct IFileSystem *MATERIALDIR;
extern struct IFileSystem *PROFILEDIR;
extern struct IFileSystem *GAMETYPESDIR;
extern struct IStatusBarResource *STATUS;
extern struct IHintResource *HINTBOX;
extern struct ICursorResource *CURSOR;
extern struct ICommTrack *COMMTRACK;
extern struct ICQ_VB_Manager *vb_mgr;
extern class ISQRT SQRT;
S32 __stdcall FindChild(const char *pathname, S32 parent);
void __stdcall SetupDiffuseBlend(U32 texture_id, bool clamp);
U32 __stdcall GetBuildVersion(void);
U32 __stdcall GetBuildVersion(char *buffer, U32 bufferSize);
const char * __cdecl _localLoadString(U32 id);
const wchar_t * __cdecl _localLoadStringW(U32 id);
inline constexpr U32 MB_OK = 0;
inline constexpr U32 MB_OKCANCEL = 1;
inline constexpr U32 MB_YESNO = 4;
U32 __stdcall CQMessageBox(wchar_t *message, wchar_t *title, U32 type, BaseHotRect *parent = nullptr);
U32 __stdcall CQMessageBox(U32 message, U32 title, U32 type, BaseHotRect *parent = nullptr);
U32 __stdcall CQMessageBox(U32 message, wchar_t *title, U32 type, BaseHotRect *parent = nullptr);
U32 __stdcall CQMessageBox(wchar_t *message, U32 title, U32 type, BaseHotRect *parent = nullptr);
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

class NativeAppParticleBinding
{
public:
    NativeAppParticleBinding() = default;
    ~NativeAppParticleBinding();
    NativeAppParticleBinding(const NativeAppParticleBinding &) = delete;
    NativeAppParticleBinding & operator=(const NativeAppParticleBinding &) = delete;
    bool Bind(IDAComponent * system);
private:
    IParticleManager *manager_ = nullptr;
};

class NativeAppDataSources
{
public:
    NativeAppDataSources() = default;
    ~NativeAppDataSources();
    NativeAppDataSources(const NativeAppDataSources &) = delete;
    NativeAppDataSources & operator=(const NativeAppDataSources &) = delete;

    bool Bind(IFileSystem * objects, IFileSystem * campaign,
              IFileSystem * materials, IFileSystem * profiles);
    bool BindInterface(IFileSystem * interfaceDirectory);
    bool BindTextures(IFileSystem * textureDirectory);

private:
    IFileSystem *objects_ = nullptr;
    IFileSystem *campaign_ = nullptr;
    IFileSystem *materials_ = nullptr;
    IFileSystem *profiles_ = nullptr;
    IFileSystem *interface_ = nullptr;
    IFileSystem *textures_ = nullptr;
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
    bool Bind(IStreamer *streamer, IFileSystem *music, IFileSystem *effects,
              IFileSystem *speech, IFileSystem *missionSpeech);
private:
    IStreamer *streamer_ = nullptr;
    IFileSystem *music_ = nullptr;
    IFileSystem *effects_ = nullptr;
    IFileSystem *speech_ = nullptr;
    IFileSystem *missionSpeech_ = nullptr;
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

class NativeAppSfxBinding
{
public:
    NativeAppSfxBinding() = default;
    ~NativeAppSfxBinding();
    NativeAppSfxBinding(const NativeAppSfxBinding &) = delete;
    NativeAppSfxBinding & operator=(const NativeAppSfxBinding &) = delete;
    bool Bind(ISFX *manager);
private:
    ISFX *manager_ = nullptr;
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

// Native module registrations must keep training and campaign start flags separate.
void InitializeNativeEffectsOptions();
bool CheckNativeCampaignScriptPrograms();
bool CheckNativePlayerAISaveLifecycle(IFileSystem *directory);
bool IsNativePersistentGunboat(IBaseObject *object);
bool IsNativePersistentSpaceShip(IBaseObject *object);
bool IsNativePersistentScriptObject(IBaseObject *object);
bool LoadNativeMissionRecord(IMission *mission, const char *name, IFileSystem *file,
                             void *destination, U32 destinationBytes);
bool LoadNativeMissionScripts(IFileSystem *file);
bool CheckNativeMissionRecordPersistence(IFileSystem *directory);
BOOL32 LoadNativePersistedLauncher(struct ISaveLoad *saved, const struct BASIC_DATA *data,
    IBaseObject *launcher, PARCHETYPE archetype, IBaseObject *owner,
    IFileSystem *file, const char *directoryName);
bool CheckNativeStatelessPingRestart(IFileSystem *directory);
bool CheckNativeGunboatLauncherFailure(IFileSystem *file);
bool CheckNativeMimicPersistence(IFileSystem *directory);
bool CheckNativeRepellentCloudPersistence(IFileSystem *directory);
bool CheckNativeStasisBoltPersistence(IFileSystem *directory);
bool CheckNativeMissionAbilityPersistence(IFileSystem *directory);
namespace FrontierX
{
void LoadNativeSystemMap(ISystemMap *map, IFileSystem *file, bool requireRecord);
bool CheckNativeSystemMapPersistence(IFileSystem *directory, IUserDefaults *defaults);
}
#endif
