//--------------------------------------------------------------------------//
//                                                                          //
//                                Mission.cpp                               //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Header: /Conquest/App/Src/Mission.cpp 322   10/18/02 2:36p Tmauer $

   Game state management
*/
//--------------------------------------------------------------------------//

#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppSchema.h"
#include "Include/NativeGlobalDataFormat.h"
#include "FrontierX/NativeDiskSpace.h"
#include "FrontierX/LegacyInput.h"
#include "FrontierX/PlatformTime.h"
#endif
#include <stdio.h>
#if !defined(_WIN32)
#include <cwchar>
#include <cstdlib>
#include <string>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <memory>


static void RequireNativeMissionFileTransfer()
{
	if (FILETRANSFER) return;
	std::fputs("Native mission file transfer is unavailable for a network mission\n", stderr);
	std::abort();
}

static bool NativeFormatPlayerMessage(wchar_t *output, size_t capacity,
                                      const wchar_t *format, const wchar_t *name)
{
	if (!output || !capacity || !format || !name) return false;
	std::wstring portable;
	for (const wchar_t *cursor = format; *cursor; ++cursor)
	{
		if (*cursor != L'%')
		{
			portable += *cursor;
			continue;
		}
		portable += *cursor;
		++cursor;
		if (*cursor == L'%') portable += *cursor;
		else if (*cursor == L's') portable += L"ls";
		else if (*cursor == L'l' && cursor[1] == L's')
		{
			portable += L"ls";
			++cursor;
		}
		else return false;
	}
	const int count = std::swprintf(output, capacity, portable.c_str(), name);
	return count >= 0 && static_cast<size_t>(count) < capacity;
}
constexpr U32 WM_CLOSE = FrontierX::LegacyMessage::Close;
using LARGE_INTEGER = S64;
static BOOL QueryPerformanceCounter(LARGE_INTEGER *counter)
{
	if (!counter) return FALSE;
	*counter = static_cast<S64>(FrontierX::Platform::MonotonicMicroseconds());
	return TRUE;
}
#endif

static FILE *g_mis_f = NULL;
#define IGM_LOG(s) do{FILE*_igf=fopen("debug/ingame_diag.txt","a");if(_igf){fputs((s),_igf);fclose(_igf);}}while(0)
#define MIS_LOG(s) do { if(!g_mis_f) g_mis_f=fopen("debug/mission_diag.txt","w"); if(g_mis_f){fputs((s),g_mis_f);fflush(g_mis_f);} } while(0)
#define MIS_LOGF(fmt,...) do { if(!g_mis_f) g_mis_f=fopen("debug/mission_diag.txt","w"); if(g_mis_f){fprintf(g_mis_f,(fmt),__VA_ARGS__);fflush(g_mis_f);} } while(0)

#include "Mission.h"
#include "resource.h"
#include "UserDefaults.h"
#include "ObjList.h"
#include "Sector.h"
#include "NetFileTransfer.h"
#include "NetPacket.h"
#include "Startup.h"
#include "Menu.h"
#include "CQTrace.h"
#if !defined(_WIN32)
#include <DGlobalData.h>
#include <ViewSymbolOperations.h>
#include <DScriptObject.h>
#include <DBuildPlat.h>
#include <DSupplyShipSave.h>
#include <DObjectGenerator.h>
#endif
#include "Cursor.h"
#include "Hotkeys.h"
#include "CQGame.h"
#include "BaseHotRect.h"
#include "MGlobals.h"
#include <DBaseData.h>
#include "OpAgent.h"
#include "IObject.h"
#include "Camera.h"
#if !defined(_WIN32)
#include "ICamera.h"
#endif
#include "MusicManager.h"
#include "CQLight.h"
#include "FogOfWar.h"
#include "MPart.h"
#include "ScrollingText.h"
#include "MapGen.h"
#include <MScript.h>
#include "CommPacket.h"
#include "IMovieCamera.h"
#include "INugget.h"
#include "ifabricator.h"
#include "ISPlayerAI.h"
#include "IGameProgress.h"
#include "InProgressAnim.h"
#include "IAnimate.h"
#include "IStatic.h"
#include "GenData.h"
#include "SysMap.h"
#include "DrawAgent.h"
#include "IShapeLoader.h"

#if defined(_WIN32)
#include <dplay.h>
#include <dplobby.h>
#include "ZoneLobby.h"
#endif
#include <TSmartPointer.h>
#include <EventSys.h>
#include <IConnection.h>
#include <TComponent.h>
#include <HeapObj.h>
#include <Filesys.h>
#include <ViewCnst.h>
#include <WindowManager.h>
#include <MemFile.h>
#include <IUTFWriter.h>

#include <DAnimate.h>
#include <DStatic.h>

#include <stdio.h>

#define NAME_REG_KEY   "CQPlayerName"
#define DEFERRED_MISSION_END_TIME 5000

using namespace CQGAMETYPES;

#if !defined(_WIN32)
static bool NativeSaveMissionSystemMap(IFileSystem *file)
{
	try
	{
		SYSMAP->Save(file);
		return true;
	}
	catch (const std::exception &error)
	{
		std::fprintf(stderr, "Native mission system-map save failed: %s\n", error.what());
		return false;
	}
}

static bool NativeLoadMissionSystemMap(IFileSystem *file)
{
	try
	{
		char script[MAX_PATH] = {};
		char filename[MAX_PATH + 4] = {};
		const LONG filenameLength = file ? file->GetFileName(filename, sizeof(filename)) : 0;
		if (!MGlobals::GetScriptName(script, sizeof(script)) || filenameLength <= 0 ||
			filenameLength >= S32(sizeof(filename)))
			throw std::runtime_error("cannot identify the loaded mission");
		std::string foldedScript = script;
		for (char &value : foldedScript)
			if (value >= 'A' && value <= 'Z') value = static_cast<char>(value + ('a' - 'A'));
		const bool regularTerran = foldedScript == "script01t.dll" || foldedScript == "script01.dll" ||
            foldedScript == "script02t.dll" || foldedScript == "script02.dll" ||
            foldedScript == "script03t.dll" || foldedScript == "script03.dll" ||
            foldedScript == "script04t.dll" || foldedScript == "script04.dll" ||
            foldedScript == "script05t.dll" || foldedScript == "script05.dll" ||
            foldedScript == "script06t.dll" || foldedScript == "script06.dll" ||
            foldedScript == "script07t.dll" || foldedScript == "script07.dll" ||
            foldedScript == "script08t.dll" || foldedScript == "script08.dll" ||
            foldedScript == "script09t.dll" || foldedScript == "script09.dll" ||
            foldedScript == "script10t.dll" || foldedScript == "script10.dll" ||
            foldedScript == "script11t.dll" || foldedScript == "script11.dll" ||
            foldedScript == "script12t.dll" || foldedScript == "script12.dll";
		const char *extension = std::strrchr(filename, '.');
		std::string foldedExtension = extension ? extension : "";
		for (char &value : foldedExtension)
			value = static_cast<char>(std::tolower(static_cast<unsigned char>(value)));
		FrontierX::LoadNativeSystemMap(SYSMAP, file, regularTerran && foldedExtension == ".mission");
		return true;
	}
	catch (const std::exception &error)
	{
		std::fprintf(stderr, "Native mission system-map load failed: %s\n", error.what());
		return false;
	}
}
#endif

static IViewConstructor2 * parser;

static bool g_bCheatsOn = false;

struct ENDGAME_PACKET : BASE_PACKET
{
	U8 allyMask;

	ENDGAME_PACKET (void)
	{
		type = PT_ENDGAME;
		dwSize = sizeof(*this);
	}
};

CQEXTERN SPLASHINFO CQ_SplashInfo; // in case bDeferredMissionEndNonFail is set to "true", do a splash screen

//--------------------------------------------------------------------------//
//
#define BEGIN_MAPPING(parent, name) \
	{								\
		HANDLE hFile, hMapping;		\
		DAFILEDESC fdesc = name;	\
		void *pImage;				\
		if ((hFile = parent->OpenChild(&fdesc)) == INVALID_HANDLE_VALUE)	\
			CQFILENOTFOUND(fdesc.lpFileName);	\
		hMapping = parent->CreateFileMapping(hFile);	\
		pImage = parent->MapViewOfFile(hMapping)

#define END_MAPPING(parent) \
		parent->UnmapViewOfFile(pImage); \
		parent->CloseHandle(hMapping); \
		parent->CloseHandle(hFile); }

//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE Mission : public IMission, 
										IEventCallback, 
										ISystemEventCallback,
										IResourceClient,
										IFileTransferCallback
{
	BEGIN_DACOM_MAP_INBOUND(Mission)
	DACOM_INTERFACE_ENTRY(IMission)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	DACOM_INTERFACE_ENTRY(IResourceClient)
	DACOM_INTERFACE_ENTRY(ISystemEventCallback)
	DACOM_INTERFACE_ENTRY(IFileTransferCallback)
	END_DACOM_MAP()

	//--------------------------------
	// data items go here
	//--------------------------------
	
	U32 eventHandle, systemEventHandle, fileTransferHandle;		// connection handles
	
  	//
	// mission save / load
	//
	COMPTR<IFileSystem> inFile;
	BOOL32 bUnsavedData, bDownloadNeeded, bRuseEnabled;
	HANDLE hPrevSymbols;
	FTCHANNEL * downloadChannel;		// remote download of mission file
	FTSTATUS downloadStatus;
	BOOL32 bHasFocus;
	BOOL32 bInterfaceLoaded;
	//
	// sinple player ai
	//
	COMPTR<ISPlayerAI> playerAI[MAX_PLAYERS];
	COMPTR<IPANIM> ipAnim; // the progress animation
	SINGLE baseObjPercent;		// starting percent for loading objlist
	SINGLE totalObjPercent;		// total percent for loading objlist


	U32    endCheckedCount;			// update count when last checked for end-of-game

	U32    deferredMissionWinners;	// ally mask of winning team
	S32    deferredMissionEndTime;
	BOOL32 bDeferredMissionEndNonFail;

	USER_DEFAULTS initialDefaults;	// check against for cheating
	bool bNetloadProgressAnimation;
	bool bOnlyHumanPlayerMsgNeeded;

	//--------------------------------
	// class methods
	//--------------------------------
	
	Mission (void);

	~Mission (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}
	
	/* IMission methods  */
	
	DEFMETHOD_(BOOL32,Load) (const C8 * fileName = 0, struct IComponentFactory * pParent = 0);

	DEFMETHOD_(BOOL32,Save) (const C8 * fileName = 0, struct IComponentFactory * pParent = 0);

	DEFMETHOD_(BOOL32,LoadFromDescription) (const wchar_t * fileDesc = 0, struct IComponentFactory * pParent = 0);

	DEFMETHOD_(BOOL32,SaveByDescription) (const wchar_t * fileDesc = 0, struct IComponentFactory * pParent = 0, const char * szOverwrite = 0, U32 saveKey = 0);

	virtual BOOL32 __stdcall LoadBriefing (const char * fileName);

	DEFMETHOD_(BOOL32,QuickSave) (const C8 * fileName = 0, struct IComponentFactory * pParent = 0);

	DEFMETHOD_(BOOL32,DynamicSave) (const C8 * fileName = 0, struct IComponentFactory * pParent = 0);

	DEFMETHOD_(BOOL32,QuickSaveFile) (void);

	DEFMETHOD_(BOOL32,QuickLoadFile) (void);

	DEFMETHOD_(BOOL32,New) ();
	
	DEFMETHOD_(void,Close) (BOOL32 bForceIt);

	DEFMETHOD_(BOOL32,SetUnsavedData) (void);
	
	DEFMETHOD(CorrelateSymbol) (const C8 *pSymbolName, void *pOldData, void *pNewData);

	DEFMETHOD_(void,Update) (void);

	DEFMETHOD_(void,SetWindowTitle) (void);

	DEFMETHOD_(void,StartNetDownload) (void);

	DEFMETHOD_(void,CancelNetDownload) (void);

	DEFMETHOD_(enum FTSTATUS,GetDownloadStatus) (void);

	virtual GENRESULT __stdcall GetFileSystem (struct IFileSystem **ppFile);

	virtual void __stdcall SetFileSystem (struct IFileSystem * pFile);

	virtual BOOL32 __stdcall Reload (void);	// reload current file

	virtual BOOL32 __stdcall ReloadMission (void);	// reload current file

	virtual void __stdcall InitializeNetGame (const struct FULLCQGAME & cqgame, U32 randomSeed);

	virtual void __stdcall GenerateMultiplayerMap (const struct FULLCQGAME & cqgame, U32 randomSeed);

	virtual void __stdcall EndMission (U32 allyMask, bool bNonWinLose);

	virtual U32 __stdcall GetNumAIPlayers (void);		// for accounting

	virtual U32 __stdcall GetFileDescription (const C8 * fileName, wchar_t * string, U32 numMaxChars);
	
	virtual U32 __stdcall GetFileDescription (IFileSystem * fileDir, const C8 * fileName, wchar_t * string, U32 numMaxChars);

	virtual U32 __stdcall GetFileMaxPlayers (IFileSystem * fileDir, const C8 * fileName, U32 & maxPlayers);

	virtual U32 __stdcall GetFileMaxPlayers (const C8 * fileName, U32 & maxPlayers);

	virtual void LoadInterface (void);

	virtual void __stdcall SetAIResign (U32 playerID)
	{
		CQASSERT(THEMATRIX->IsMaster());
		CQASSERT(IsComputerControlled(playerID));		// must be computer controlled

		AIRESIGN_PACKET packet;
		packet.dwSize = sizeof(packet);
		packet.playerID = playerID;
		NETPACKET->Send(0, NETF_ALLREMOTE | NETF_ECHO, &packet);
	}

	//AI CONTROLLING METHODS FOR MISSION SCRIPTING

	virtual bool __stdcall IsComputerControlled (U32 playerID);

	virtual void __stdcall EnableEnemyAI(U32 playerID, bool bOn, enum M_RACE race);
	
	virtual void __stdcall EnableEnemyAI(U32 playerID, bool bOn, const char * szPlayerAIType);

	virtual void __stdcall SetEnemyAIRules(U32 playerID, const struct AIPersonality & rules);

	virtual void __stdcall SetEnemyAITarget(U32 playerID, IBaseObject *obj, U32 range, U32 systemID = 0);

	virtual void __stdcall LaunchOffensive (U32 playerID, UNIT_STANCE stance);

	virtual void __stdcall SetInitialCheatState (void)
	{
		initialDefaults = *DEFAULTS->GetDefaults();
		memset(&initialDefaults, 0, daoffsetofmember(USER_DEFAULTS, gammaCorrection)+dasizeofmember(USER_DEFAULTS, gammaCorrection));
	}

	virtual bool __stdcall IsSinglePlayerGame (void)
	{
		return MGlobals::GetGameSettings().gameType == CQGAMETYPES::MISSION_DEFINED;
	}

	virtual void __stdcall SetFileDescription(const wchar_t * desc);

	virtual bool __stdcall IsMouseEnabled (void);

	virtual void __stdcall CancelProgressAnim (void)
	{
		ipAnim.free();
		bNetloadProgressAnimation = false;
	}

	virtual void __stdcall StartProgressAnim (struct IPANIM ** ppAnim);

	virtual void __stdcall SetObjlistLoadProgress (SINGLE percentLoaded)
	{
		if (percentLoaded >= 1.0)
			percentLoaded = 1;

		if (ipAnim!=0 && totalObjPercent!=0)
		{
			percentLoaded = (totalObjPercent*percentLoaded) + baseObjPercent;
			ipAnim->SetProgress(percentLoaded);
		}
	}

	virtual const M_RACE __stdcall GetSinglePlayerRace (void) const;

	virtual void __stdcall SetSinglePlayerRace (const enum M_RACE race);

	virtual const struct CQGAME & GetGameSettings (void);

	virtual void __stdcall ToggleCheating (void);

	virtual const bool __stdcall GetCheatsEnabled (void) const;

	virtual COLORREF __stdcall GetPlayerColorForDPID (U32 dpid);

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param = 0);

	/* IResourceClient methods */

	DEFMETHOD_(BOOL32,Notify) (struct IResource *res, U32 message, void *parm=0);

	/* ISystemEventCallback methods */

	DEFMETHOD_(BOOL32,OnAppClose) (void);

	/* IFileTransferCallback methods */

	DEFMETHOD(RemoteFileRequest) (const C8 *fileName, IFileSystem ** file);
	
	/* Mission methods  */

	void SetMissionMRUList (HMENU hMenu);

	BOOL32 SaveAs (const C8 * fileName=0, struct IComponentFactory * pParent = 0);

	BOOL32 QuickSaveAs (const C8 * fileName=0, struct IComponentFactory * pParent = 0);

	BOOL32 DynamicSaveAs (const C8 * fileName=0, struct IComponentFactory * pParent = 0);

	void LoadFromFilter();

	BOOL32 LoadMission (U32 id);

	BOOL32 SaveParseData (void);

	BOOL32 LoadParseData (IFileSystem * inFile);

	BOOL32 close (void);

	void updateNetFile (void);
	
	void unloadInterface (void);

	void enableSpectatorMode (void);

	void checkEndGame (void);
	
	void reloadGametypes (void);
	
	void destroyPlayerAI (void);
	
	void startPlayerAI (void);
	
	bool savePlayerAI (struct IFileSystem * outFile);
	
#if defined(_WIN32)
	void loadPlayerAI (struct IFileSystem * inFile);
	
	void loadPlayerAI (struct IFileSystem * inFile, U32 playerID);
#else
	bool loadPlayerAI (struct IFileSystem * inFile);
	bool loadPlayerAI (struct IFileSystem * inFile, U32 playerID);
#endif

	void createPlayerAI (M_RACE race, U32 playerID, ISPlayerAI ** ppPlayerAI);
	void calculateCtrlO_Scores (void);
	void explodePlayerBombs (void);

	void setAIResign (U32 playerID);

	void setDefaultCheatSettings (void);

	bool getDiscFreeSpace (LPCTSTR lpszPath, DWORDLONG* pnFree);

	IDAComponent * GetBase (void)
	{
		return (IMission *) this;
	}
};
//--------------------------------------------------------------------------
//
inline static void * GetPreprocessData (int & resourceSize)
{
#if !defined(_WIN32)
	return const_cast<void *>(FrontierX::GetNativeAppSchema(resourceSize));
#else
	HRSRC hRes;

	if ((hRes = FindResource(hResource, MAKEINTRESOURCE(IDR_PARSER1), "PARSER")) != 0)
	{
		HGLOBAL hGlobal;

		if ((hGlobal = LoadResource(hResource, hRes)) != 0)
		{
			void * ptr = LockResource(hGlobal);
			resourceSize = SizeofResource(hResource, hRes);
			return ptr;
		}
	}
	return 0;
#endif
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
Mission::Mission (void)
{
	// nothing else to init so far?
	bHasFocus = 1;
}
//--------------------------------------------------------------------------//
//
Mission::~Mission (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (GS && GS->QueryOutgoingInterface("ISystemEventCallback", connection) == GR_OK)
		connection->Unadvise(systemEventHandle);
	
#if defined(_WIN32)
	if (FULLSCREEN && FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
		connection->Unadvise(eventHandle);
#else
	if (FULLSCREEN && FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
        connection->Unadvise(eventHandle);
    else if (EVENTSYS && EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
        connection->Unadvise(eventHandle);
#endif

	if (FILETRANSFER && FILETRANSFER->QueryOutgoingInterface("IFileTransferCallback", connection) == GR_OK)
		connection->Unadvise(fileTransferHandle);

	if (parser)
	{
		if (hPrevSymbols)
			parser->DestroySymbols(hPrevSymbols);
		hPrevSymbols = 0;
		parser->Release();
	}
	parser = 0;

	if (downloadChannel && FILETRANSFER)
		FILETRANSFER->CloseChannel(downloadChannel);
	downloadChannel = 0;
}
//--------------------------------------------------------------------------//
//
void Mission::LoadInterface (void)
{
	if (bInterfaceLoaded==0)
	{
		CQASSERT(CQFLAGS.b3DEnabled!=0);

		M_RACE race;
		race = MGlobals::GetPlayerRace(MGlobals::GetThisPlayer());

		bInterfaceLoaded = 1;
		EVENTSYS->Send(CQE_LOAD_TOOLBAR, &race);
	}
}
//-------------------------------------------------------------------
//
void Mission::unloadInterface (void)
{
	if (bInterfaceLoaded)
	{
		bInterfaceLoaded = 0;
		EVENTSYS->Send(CQE_LOAD_TOOLBAR, NULL);
	}
}
//-------------------------------------------------------------------
//
void Mission::destroyPlayerAI (void)
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++)
		playerAI[i].free();
}
//-------------------------------------------------------------------
//
static bool isComputerPlayerAssigned (const struct CQGAME & cqgame, U32 playerID)
{
	int i, j = cqgame.activeSlots;

	for (i = 0; i < j; i++)
	{
		if (cqgame.slot[i].type == COMPUTER && MGlobals::GetPlayerIDFromSlot(i) == playerID)
			return true;
	}

	return false;
}
//-------------------------------------------------------------------
// assumes that cqgame struct has been initialized
// must delete some computer players that aren't needed (if they were loaded from disk)
//
void Mission::startPlayerAI (void)
{
	int i, j;
	const struct CQGAME & cqgame = MGlobals::GetGameSettings();
	j = cqgame.activeSlots;

	if (CQFLAGS.bAITest)		// for AI into all READY slots
	{
		for (i = 0; i < j; i++)
		{
			int race = cqgame.slot[i].race;
			if (cqgame.slot[i].state == READY)
			{
				U32 playerID = MGlobals::GetPlayerIDFromSlot(i);

				if (playerID && playerID <= MAX_PLAYERS && playerAI[playerID-1]==0)
				{
					createPlayerAI((M_RACE)race, playerID, playerAI[playerID-1]);
				}
			}
		}
	}
	else
	{
		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if (playerAI[i] != 0)
			{
				if (isComputerPlayerAssigned(cqgame, i+1) == false)
				{
					playerAI[i].free();
				}
			}
		}

		for (i = 0; i < j; i++)
		{
			int race = cqgame.slot[i].race;
			if (cqgame.slot[i].state == READY && cqgame.slot[i].type == COMPUTER)
			{
				U32 playerID = MGlobals::GetPlayerIDFromSlot(i);

				if (playerID && playerID <= MAX_PLAYERS && playerAI[playerID-1]==0)
				{
					createPlayerAI((M_RACE)race, playerID, playerAI[playerID-1]);
				}
			}
		}
	}
}
//--------------------------------------------------------------------------
//
#if defined(_WIN32)
void Mission::loadPlayerAI (struct IFileSystem * inFile, U32 playerID)
{
	char buffer[MAX_PATH];
	CQASSERT(playerID && playerID <= MAX_PLAYERS);

	sprintf(buffer, "\\PLAYERAI\\Player%02d", playerID);

	if (inFile->SetCurrentDirectory(buffer))
	{
		WIN32_FIND_DATA data;
		HANDLE hFF;

		if ((hFF = inFile->FindFirstFile("*.*", &data)) != INVALID_HANDLE_VALUE)
		{
			if (inFile->SetCurrentDirectory(data.cFileName))
			{
				ISPlayerAI::CreateInstance(data.cFileName, playerID, playerAI[playerID-1]);
				if (playerAI[playerID-1])
				{
					if (playerAI[playerID-1]->Load(inFile) == 0)
					{
						playerAI[playerID-1].free();
					}
				}
#ifdef _DEBUG
				if (playerAI[playerID-1] == 0)
				{
					const struct CQGAME & cqgame = MGlobals::GetGameSettings();
					if (cqgame.gameType == MISSION_DEFINED)
						CQERROR0("Failed load on PlayerAI");
				}
#endif
			}

			inFile->FindClose(hFF);
		}
	}
}
//--------------------------------------------------------------------------
//
void Mission::loadPlayerAI (struct IFileSystem * inFile)
{
	const struct CQGAME & cqgame = MGlobals::GetGameSettings();

	// single player mission
	if (cqgame.gameType == MISSION_DEFINED)
	{
		for(U32 i = 2; i <=MAX_PLAYERS; ++i)
		{
			loadPlayerAI(inFile, i);
		}
	}
	else
	{
		int i, j;
		const struct CQGAME & cqgame = MGlobals::GetGameSettings();
		j = cqgame.activeSlots;

		for (i = 0; i < j; i++)
		{
			int race = cqgame.slot[i].race;
			if (cqgame.slot[i].type == COMPUTER)
			{
				U32 playerID = MGlobals::GetPlayerIDFromSlot(i);

				if (playerID && playerID <= MAX_PLAYERS && playerAI[playerID-1]==0)
				{
					loadPlayerAI(inFile, playerID);
					if (playerAI[playerID-1]==0)		// if load failed
						createPlayerAI((M_RACE)race, playerID, playerAI[playerID-1]);
				}
			}
		}
	}

}
#else
bool Mission::loadPlayerAI (struct IFileSystem *inFile, U32 playerID)
{
	char buffer[MAX_PATH];
	CQASSERT(playerID && playerID <= MAX_PLAYERS);
	sprintf(buffer, "\\PLAYERAI\\Player%02d", playerID);
	// An absent AI is legitimate before the mission script starts that player.
	if (!inFile->SetCurrentDirectory(buffer)) return true;
	WIN32_FIND_DATA data = {};
	HANDLE search = inFile->FindFirstFile("*.*", &data);
	if (search == INVALID_HANDLE_VALUE) return false;
	bool loaded = false;
	if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
		inFile->SetCurrentDirectory(data.cFileName))
	{
		ISPlayerAI::CreateInstance(data.cFileName, playerID, playerAI[playerID-1]);
		loaded = playerAI[playerID-1] && playerAI[playerID-1]->Load(inFile);
		if (!loaded) playerAI[playerID-1].free();
	}
	inFile->FindClose(search);
	return loaded;
}

bool Mission::loadPlayerAI (struct IFileSystem *inFile)
{
	const struct CQGAME &cqgame = MGlobals::GetGameSettings();
	if (cqgame.gameType == MISSION_DEFINED)
	{
		for (U32 i = 2; i <= MAX_PLAYERS; ++i)
			if (!loadPlayerAI(inFile, i)) return false;
	}
	else
	{
		for (int i = 0; i < cqgame.activeSlots; ++i)
		{
			if (cqgame.slot[i].type == COMPUTER)
			{
				const U32 playerID = MGlobals::GetPlayerIDFromSlot(i);
				if (playerID && playerID <= MAX_PLAYERS && playerAI[playerID-1] == 0)
				{
					if (!loadPlayerAI(inFile, playerID)) return false;
					if (playerAI[playerID-1] == 0)
						createPlayerAI((M_RACE)cqgame.slot[i].race, playerID, playerAI[playerID-1]);
				}
			}
		}
	}
	return true;
}
#endif
//--------------------------------------------------------------------------
//
bool Mission::savePlayerAI (struct IFileSystem * outFile)
{
	if (PLAYERID==0 && GetNumAIPlayers())
	{
		char buffer[MAX_PATH];

		outFile->CreateDirectory("\\PLAYERAI");
		
		for (U32 i = 0; i < MAX_PLAYERS; i++)
		{		
			if (playerAI[i] != 0)
			{
				sprintf(buffer, "\\PLAYERAI\\Player%02d", i+1);
				if (outFile->CreateDirectory(buffer) == 0)
				{
					return false;
				}
				if (outFile->SetCurrentDirectory(buffer) == 0)
				{
					return false;
				}

				if (outFile->CreateDirectory(playerAI[i]->getSaveLoadName()) == 0)
				{
					return false;
				}
				if (outFile->SetCurrentDirectory(playerAI[i]->getSaveLoadName()) == 0)
				{
					return false;
				}

				if (playerAI[i]->Save(outFile) == 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}
//-------------------------------------------------------------------
//
BOOL32 Mission::SetUnsavedData (void)
{
	bUnsavedData = 1;
	return 1;
}
//-------------------------------------------------------------------
//
GENRESULT Mission::CorrelateSymbol (const C8 *pSymbolName, void *pOldData, void *pNewData)
{
	SYMBOL oldSymbol, newSymbol;

	oldSymbol = parser->GetSymbol(hPrevSymbols, pSymbolName);
	newSymbol = parser->GetSymbol(0, pSymbolName);

	if (oldSymbol && newSymbol)
	{
#if defined(_WIN32)
		parser->CorrelateSymbol(oldSymbol, pOldData, newSymbol, pNewData);
		return GR_OK;
#else
		const bool correlated = !std::strcmp(pSymbolName, "MT_GlobalData") ?
			CorrelateNativeMissionGlobals(parser, oldSymbol, pOldData, newSymbol, pNewData) :
			CorrelateNativeViewSymbols(parser, oldSymbol, pOldData, newSymbol, pNewData);
		return correlated ? GR_OK : GR_GENERIC;
#endif
	}
	else
		return GR_GENERIC;
}
#if !defined(_WIN32)
bool LoadNativeMissionRecord(IMission *mission, const char *name, IFileSystem *file,
	void *destination, U32 destinationBytes)
{
	// The saved schema is authoritative for input extent. Older legitimate
	// layouts remain correlated by the original owner rather than read as the
	// current C++ struct or silently accepted after a short read.
	auto *owner = dynamic_cast<Mission *>(mission);
	if (!owner || !parser || !name || !file || !destination || !destinationBytes)
		return false;
	SYMBOL oldSymbol = parser->GetSymbol(owner->hPrevSymbols, name);
	SYMBOL newSymbol = parser->GetSymbol(0, name);
	if (!oldSymbol || !newSymbol) return false;
	oldSymbol = ResolveViewSymbolType(oldSymbol);
	newSymbol = ResolveViewSymbolType(newSymbol);
	if (!oldSymbol || !newSymbol || oldSymbol->kind != RECORDSYM || newSymbol->kind != RECORDSYM ||
		oldSymbol->size <= 0 || newSymbol->size <= 0)
		return false;
	U32 inputBytes = static_cast<U32>(oldSymbol->size);
	// Original SupplyShip Save writes sizeof(SUPPLYSHIP_SAVELOAD): its empty
	// trailing sup_SL occupies one C++ byte plus alignment, but zero schema bytes.
	// Recognize that exact contract, keeping every real field in normal correlation.
	const auto hasEmptySupplyTail = [](SYMBOL record) {
		if (record->size != 464) return false;
		SYMBOL field = record->datatype;
		if (!field) return false;
		while (field->link) field = field->link;
		if (!field->name || std::strcmp(field->name, "sup_SL") || field->offset != 464 ||
			!field->datatype) return false;
		SYMBOL type = ResolveViewSymbolType(field->datatype);
		return type && type->kind == RECORDSYM && type->size == 0 && !type->datatype &&
			type->name && !std::strcmp(type->name, "SUP_SAVELOAD");
	};
	const bool supplyTail = !std::strcmp(name, "SUPPLYSHIP_SAVELOAD") &&
		destinationBytes == sizeof(SUPPLYSHIP_SAVELOAD) && sizeof(SUPPLYSHIP_SAVELOAD) == 468 &&
		sizeof(SUP_SAVELOAD) == 1 &&
		hasEmptySupplyTail(oldSymbol) && hasEmptySupplyTail(newSymbol);
	if (supplyTail) inputBytes = sizeof(SUPPLYSHIP_SAVELOAD);
	if ((!supplyTail && static_cast<U32>(newSymbol->size) != destinationBytes) ||
		file->GetFileSize() != inputBytes)
		return false;
	try
	{
		std::vector<U8> input(inputBytes);
		std::vector<U8> output(destinationBytes, 0);
		U32 read = 0;
		if (!file->ReadFile(0, input.data(), input.size(), &read, 0) || read != input.size() ||
			owner->CorrelateSymbol(name, input.data(), output.data()) != GR_OK)
			return false;
		std::memcpy(destination, output.data(), destinationBytes);
		return true;
	}
	catch (const std::bad_alloc &)
	{
		std::fprintf(stderr, "Native mission record allocation failed: %s\n", name);
		return false;
	}
}

bool CheckNativeMissionRecordPersistence(IFileSystem *directory)
{
	// Source ScriptObject/SpaceShip Load call this exact parser-owned path.
	// Exercise saved-schema extents and genuine correlation before any actor
	// mutation, using the source record and original App schema, not a serializer.
	const auto fail = [](const char *stage) {
		std::fprintf(stderr, "Native mission record regression failed: %s\n", stage);
		return false;
	};
	if (!directory || MISSION || parser || !GS) return fail("fixture lifecycle prerequisites");
	COMPTR<IViewConstructor> constructor;
	COMPTR<IViewConstructor2> provider;
	if (GS->QueryInterface("IViewConstructor", constructor) != GR_OK ||
		constructor->QueryInterface("IViewConstructor2", provider) != GR_OK) return fail("genuine provider lookup");
	if ((!provider->GetSymbol(0, "SCRIPTOBJECT_SAVELOAD") ||
		!provider->GetSymbol(0, "BUILDPLAT_SAVELOAD")) &&
		!FrontierX::ParseNativeAppSchema(provider)) return fail("current App schema parse");
	std::unique_ptr<DAComponent<Mission>> owner(new DAComponent<Mission>);
	parser = provider;
	parser->AddRef(); // The actual Mission destructor owns this reference.
	int schemaLength = 0;
	const char *schema = static_cast<const char *>(FrontierX::GetNativeAppSchema(schemaLength));
	if (!schema || schemaLength <= 0) return fail("original schema resource");
	std::vector<char> schemaText(schema, schema+schemaLength);
	schemaText.push_back(0);
	owner->hPrevSymbols = parser->ParseNewMemory(schemaText.data());
	if (!owner->hPrevSymbols) return fail("saved App schema parse");
	DAFILEDESC descriptor = "MissionRecordRegression.mission";
	descriptor.lpImplementation = "UTF";
	descriptor.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	descriptor.dwCreationDistribution = CREATE_ALWAYS;
	COMPTR<IFileSystem> archive;
	if (directory->CreateInstance(&descriptor, archive) != GR_OK) return fail("physical archive create");
	SCRIPTOBJECT_SAVELOAD saved = {};
	saved.bTowed = true;
	saved.towerID = 0x12345678;
	saved.exploredFlags = 3;
	const auto write = [&](const void *data, U32 size) {
		DAFILEDESC record = "SCRIPTOBJECT_SAVELOAD";
		record.lpImplementation = "DOS";
		record.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		record.dwCreationDistribution = CREATE_ALWAYS;
		COMPTR<IFileSystem> file;
		U32 count = 0;
		return archive->CreateInstance(&record, file) == GR_OK &&
			file->WriteFile(0, data, size, &count, 0) && count == size;
	};
	const auto read = [&](IFileSystem *source, SCRIPTOBJECT_SAVELOAD &output) {
		DAFILEDESC record = "SCRIPTOBJECT_SAVELOAD";
		record.lpImplementation = "DOS";
		COMPTR<IFileSystem> file;
		return source->CreateInstance(&record, file) == GR_OK &&
			LoadNativeMissionRecord(owner.get(), record.lpFileName, file, &output, sizeof(output));
	};
	SCRIPTOBJECT_SAVELOAD output = {};
	if (!write(&saved, sizeof(saved))) return fail("canonical ScriptObject write");
	COMPTR<IUTFWriter> writer;
	if (archive->QueryInterface(IID_IUTFWriter, writer) != GR_OK) return fail("source UTF writer query");
	writer->Flush();
	writer.free();
	descriptor.dwCreationDistribution = OPEN_EXISTING;
	descriptor.dwDesiredAccess = GENERIC_READ;
	COMPTR<IFileSystem> reopened;
	if (directory->CreateInstance(&descriptor, reopened) != GR_OK)
		return fail("physical read-only archive reopen");
	if (!read(reopened, output)) return fail("physical ScriptObject canonical record load");
	if (std::memcmp(&output, &saved, sizeof(saved)))
		return fail("ScriptObject canonical correlated bytes");
	output.towerID = 0x87654321;
	if (!write(&saved, sizeof(saved)-1) || read(archive, output) || output.towerID != 0x87654321)
		return fail("ScriptObject truncated rejection/transaction");
	std::vector<U8> oversized(sizeof(saved)+1, 0);
	std::memcpy(oversized.data(), &saved, sizeof(saved));
	if (!write(oversized.data(), oversized.size()) || read(archive, output) ||
		output.towerID != 0x87654321) return fail("ScriptObject oversized rejection/transaction");
	// Construction platforms use the same production loader with their real
	// nested source record, including a live queue/working operation payload.
	BUILDPLAT_SAVELOAD platform = {};
	platform.buildPlanetID = 0x10203040;
	platform.buildSlot = 3;
	platform.buildPlatSaveload.workingID = 71;
	platform.buildPlatSaveload.constructionID = 72;
	platform.buildPlatSaveload.buidTimeSpent = 1.5f;
	DAFILEDESC platformDescriptor = "BUILDPLAT_SAVELOAD";
	platformDescriptor.lpImplementation = "DOS";
	platformDescriptor.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	platformDescriptor.dwCreationDistribution = CREATE_ALWAYS;
	COMPTR<IFileSystem> platformFile;
	U32 platformWritten = 0;
	if (archive->CreateInstance(&platformDescriptor, platformFile) != GR_OK ||
		!platformFile->WriteFile(0, &platform, sizeof(platform), &platformWritten, 0) ||
		platformWritten != sizeof(platform)) return fail("Platform canonical record write");
	BUILDPLAT_SAVELOAD platformOutput = {};
	platformFile.free();
	platformDescriptor.dwDesiredAccess = GENERIC_READ;
	platformDescriptor.dwCreationDistribution = OPEN_EXISTING;
	if (archive->CreateInstance(&platformDescriptor, platformFile) != GR_OK)
		return fail("Platform canonical fresh child open");
	if (!LoadNativeMissionRecord(owner.get(), platformDescriptor.lpFileName, platformFile,
		&platformOutput, sizeof(platformOutput)))
		return fail("Platform canonical record load/correlation");
	if (std::memcmp(&platformOutput, &platform, sizeof(platform)))
		return fail("Platform canonical correlated bytes");
	platformFile.free();
	platformDescriptor.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	platformDescriptor.dwCreationDistribution = CREATE_ALWAYS;
	if (archive->CreateInstance(&platformDescriptor, platformFile) != GR_OK ||
		!platformFile->WriteFile(0, &platform, sizeof(platform)-1, &platformWritten, 0) ||
		platformWritten != sizeof(platform)-1) return fail("Platform truncated record write");
	platformFile.free();
	platformDescriptor.dwDesiredAccess = GENERIC_READ;
	platformDescriptor.dwCreationDistribution = OPEN_EXISTING;
	if (archive->CreateInstance(&platformDescriptor, platformFile) != GR_OK ||
		LoadNativeMissionRecord(owner.get(), platformDescriptor.lpFileName, platformFile,
			&platformOutput, sizeof(platformOutput))) return fail("Platform truncated record rejection");
	// The real SupplyShip writer includes the empty trailing C++ member/padding;
	// its parser record is four bytes shorter. Exercise the same file adapter.
	SUPPLYSHIP_SAVELOAD supply = {};
	supply.mission.dwMissionID = 0x12345671;
	supply.baseSaveLoad.mode = SUP_MOVING_TARGETED_SUPPLY;
	supply.baseSaveLoad.workingID = 97;
	supply.baseSaveLoad.targetedTargetID = 0x12345681;
	supply.baseSaveLoad.supplyTimer = 0.5f;
	supply.baseSaveLoad.supplyStance = SUP_STANCE_RESUPPLY;
	supply.baseSaveLoad.bNeedToSendMoveToTarget = true;
	const auto writeSupply = [&](U32 size) {
		DAFILEDESC record = "SUPPLYSHIP_SAVELOAD";
		record.lpImplementation = "DOS";
		record.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		record.dwCreationDistribution = CREATE_ALWAYS;
		COMPTR<IFileSystem> file;
		std::vector<U8> bytes(size, 0);
		std::memcpy(bytes.data(), &supply, std::min<size_t>(size, sizeof(supply)));
		U32 written = 0;
		return archive->CreateInstance(&record, file) == GR_OK &&
			file->WriteFile(0, bytes.data(), size, &written, 0) && written == size;
	};
	const auto readSupply = [&](SUPPLYSHIP_SAVELOAD &output) {
		DAFILEDESC record = "SUPPLYSHIP_SAVELOAD";
		record.lpImplementation = "DOS";
		COMPTR<IFileSystem> file;
		return archive->CreateInstance(&record, file) == GR_OK &&
			LoadNativeMissionRecord(owner.get(), record.lpFileName, file, &output, sizeof(output));
	};
	SUPPLYSHIP_SAVELOAD supplyOutput = {};
	if (!writeSupply(sizeof(supply)) || !readSupply(supplyOutput) ||
		std::memcmp(&supplyOutput, &supply, sizeof(supply)))
		return fail("SupplyShip original trailing empty member record");
	for (U32 size : {464U, 467U, 469U})
	{
		supplyOutput.baseSaveLoad.workingID = 101;
		if (!writeSupply(size) || readSupply(supplyOutput) || supplyOutput.baseSaveLoad.workingID != 101)
			return fail("SupplyShip malformed extent rejection/transaction");
	}
	if (!writeSupply(sizeof(supply))) return fail("SupplyShip original record restore");
	SYMBOL supplySymbol = ResolveViewSymbolType(parser->GetSymbol(owner->hPrevSymbols, "SUPPLYSHIP_SAVELOAD"));
	SYMBOL tail = supplySymbol ? supplySymbol->datatype : nullptr;
	while (tail && tail->link) tail = tail->link;
	if (!tail || tail->offset != 464) return fail("SupplyShip synthetic schema tail");
	tail->offset = 463;
	const bool acceptedWrongTail = readSupply(supplyOutput);
	tail->offset = 464;
	if (acceptedWrongTail) return fail("SupplyShip malformed empty tail rejection");
	// TM5's original wave programs use ObjectGenerator::Load through this
	// boundary. Check its live countdown/type/enable record, not just sizeof.
	OBJECT_GENERATOR_SAVELOAD generator{};
	generator.baseSave.mean = 90.5F;
	generator.baseSave.minDiff = 12.25F;
	generator.baseSave.timer = 37.5F;
	generator.baseSave.nextTime = 86.75F;
	generator.baseSave.archID = 0x12345678;
	generator.baseSave.bGenEnabled = true;
	const auto writeGenerator = [&](U32 size) {
		DAFILEDESC record = "OBJECT_GENERATOR_SAVELOAD";
		record.lpImplementation = "DOS";
		record.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		record.dwCreationDistribution = CREATE_ALWAYS;
		COMPTR<IFileSystem> file;
		std::vector<U8> bytes(size, 0);
		std::memcpy(bytes.data(), &generator, std::min<size_t>(size, sizeof(generator)));
		U32 written = 0;
		return archive->CreateInstance(&record, file) == GR_OK &&
			file->WriteFile(0, bytes.data(), size, &written, nullptr) && written == size;
	};
	const auto readGenerator = [&](OBJECT_GENERATOR_SAVELOAD &output) {
		DAFILEDESC record = "OBJECT_GENERATOR_SAVELOAD";
		record.lpImplementation = "DOS";
		COMPTR<IFileSystem> file;
		return archive->CreateInstance(&record, file) == GR_OK &&
			LoadNativeMissionRecord(owner.get(), record.lpFileName, file, &output, sizeof(output));
	};
	OBJECT_GENERATOR_SAVELOAD generatorOutput{};
	if (!writeGenerator(sizeof(generator)) || !readGenerator(generatorOutput) ||
		std::memcmp(&generatorOutput, &generator, sizeof(generator)))
		return fail("ObjectGenerator countdown/type/enable correlation");
	for (U32 size : {167U, 169U})
	{
		generatorOutput.baseSave.timer = 101.25F;
		if (!writeGenerator(size) || readGenerator(generatorOutput) ||
			generatorOutput.baseSave.timer != 101.25F)
			return fail("ObjectGenerator malformed extent rejection/transaction");
	}
	parser->DestroySymbols(owner->hPrevSymbols);
	owner->hPrevSymbols = parser->ParseNewMemory(
		"struct SCRIPTOBJECT_SAVELOAD { unsigned int towerID; };");
	const U32 incomplete = 7;
	if (!owner->hPrevSymbols || !write(&incomplete, sizeof(incomplete)) ||
		read(archive, output) || output.towerID != 0x87654321) return fail("incomplete schema rejection/transaction");
    // The original load menu lists descriptions without loading mission state.
    // Its disk wchar_t fields remain UTF-16 even on the LP64 host.
    MT_GlobalData metadata{};
    std::wcscpy(metadata.gameDescription.string, L"Menu checkpoint");
    FrontierX::GlobalDataDiskBytes metadataBytes{};
    if (!FrontierX::EncodeGlobalData(metadata, metadataBytes) ||
        !archive->CreateDirectory("\\MT_GlobalData") ||
        !archive->SetCurrentDirectory("\\MT_GlobalData")) return fail("menu metadata setup");
    DAFILEDESC globalsDescriptor = "Globals";
    globalsDescriptor.lpImplementation = "DOS";
    globalsDescriptor.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    globalsDescriptor.dwCreationDistribution = CREATE_ALWAYS;
    COMPTR<IFileSystem> metadataFile;
    U32 written = 0;
    if (archive->CreateInstance(&globalsDescriptor, metadataFile) != GR_OK ||
        !metadataFile->WriteFile(0, metadataBytes.data(), metadataBytes.size(), &written, nullptr) ||
        written != metadataBytes.size()) return fail("menu metadata write");
    metadataFile.free();
    if (archive->QueryInterface("IUTFWriter", writer) != GR_OK || !writer)
        return fail("menu metadata writer");
    writer->Flush();
    wchar_t description[32]{};
    wchar_t bounded[4] = {L'?', L'?', L'?', L'!'};
    if (MGlobals::GetFileDescription(directory, descriptor.lpFileName, description, 32) != 15 ||
        std::wcscmp(description, L"Menu checkpoint") ||
        MGlobals::GetFileDescription(directory, descriptor.lpFileName, bounded, 3) != 2 ||
        bounded[0] != L'M' || bounded[1] != L'e' || bounded[2] || bounded[3] != L'!' ||
        MGlobals::GetFileDescription(directory, descriptor.lpFileName, bounded, 0) != 0)
        return fail("menu description UTF-16 decode/bounds");
	return true;
}
#endif

//-------------------------------------------------------------------
//
BOOL32 Mission::Load (const C8 * fileName, IComponentFactory * pParent)
{
	BOOL32 result=0;
	char buffer[MAX_PATH+4];
	DAFILEDESC fdesc = buffer;

	if (pParent == 0)
		pParent = DACOM;
	fdesc.lpImplementation = "UTF";
	buffer[0] = 0;
	if (fileName)
		strcpy(buffer, fileName);
	if (buffer[0] || DEFAULTS->GetInputFilename(buffer, IDS_MISSION_FILTER))
	{
		if (close() == 0)
			goto Done;

		if (pParent->CreateInstance(&fdesc, inFile) == GR_OK)
		{
			result = Reload();
		}
		else
			SetWindowTitle();

		bUnsavedData=0;
	}

Done:
	return result;
}
//--------------------------------------------------------------------------//
//
bool Mission::getDiscFreeSpace (LPCTSTR lpszPath, DWORDLONG* pnFree)
{
#ifndef _WIN32
	if (!pnFree) return false;
	std::uint64_t available = 0;
	if (!FrontierX::Platform::AvailableDiskBytes(lpszPath, available))
		return false;
	*pnFree = available;
	return true;
#else
	// the typedef for the function pointer to get the available disk space
	typedef bool (CALLBACK* MYFUNCPTR)(LPCTSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER); 

	bool bRet = false;

	// We need to determine whether GetDiskFreeSpaceEx is available by calling LoadLibrary
	// or LoadLibraryEx, to load Kernel32.DLL, and then calling the GetProcAddress to
	// obtain an address for GetDiskFreeSpaceEx.  If GetProcAddress fails, or if
	// GetDiskFreeSpaceEx fails with the ERROR_CALL_NOT_IMPLEMENTED code, we use the
	// GetDiskFreeSpace function instead.	
	HINSTANCE hInstance = LoadLibrary("KERNEL32.DLL");

	if (hInstance)	// If we got the library
	{		
		MYFUNCPTR lpfnDLLProc = NULL;
		
		lpfnDLLProc = (MYFUNCPTR)GetProcAddress(hInstance, "GetDiskFreeSpaceExA");
		
		if (lpfnDLLProc)	// If we got an address to the function
		{	
			ULARGE_INTEGER nTotalBytes, nTotalFreeBytes, nTotalAvailable;

			if (lpfnDLLProc(lpszPath, &nTotalAvailable, &nTotalBytes, &nTotalFreeBytes))
			{
				*pnFree = nTotalAvailable.QuadPart;
				bRet = true;
			}		
		}

		FreeLibrary(hInstance);
	}

	if (!bRet)	// We have to try and use GetDiskFreeSpace()	
	{
		ULONG secpercluster, bytespersec, nooffreeclusters, totalnoofclusters;	

		if(GetDiskFreeSpace(lpszPath, &secpercluster, &bytespersec,
			&nooffreeclusters, &totalnoofclusters))
		{
			*pnFree = (nooffreeclusters * secpercluster * bytespersec);
			bRet = true;
		}				
	}

	return bRet;
#endif
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::Save (const C8 * fileName, struct IComponentFactory * pParent)
{
	// make sure we have enough memory
	DWORDLONG dwlDiskSpace=0;
	const bool diskSpaceKnown = getDiscFreeSpace(NULL, &dwlDiskSpace);
	#ifndef _WIN32
	if (!diskSpaceKnown)
		return FALSE;
	#endif
	if (diskSpaceKnown)
	{
		// we need to have at least one meg of memory around in order to save our game...
		if (dwlDiskSpace < 1024*1024)
		{
			// tell the user that they don't have enough available memory to save the game
			#ifdef _WIN32
			CQMessageBox(IDS_HELP_NODISCSPACE, IDS_ERROR, MB_OK);
			#else
			fprintf(stderr, "Mission save requires at least one MiB of available disk space\n");
			#endif
			return FALSE;
		}
	}
	
	BOOL32 result = 0;
	COMPTR<IUTFWriter> pWriter;
	char buffer[MAX_PATH+4];

	if (inFile)
		inFile->GetFileName(buffer, sizeof(buffer));
	else
		buffer[0] = 0;
	
	#ifdef _WIN32
	if (inFile == 0 || (fileName != 0 && stricmp(fileName,buffer)!=0))	// no current file, or it's a different file
	#else
	if (inFile == 0 || (fileName != 0 && strcmp(fileName,buffer)!=0))
	#endif
		return SaveAs(fileName, pParent);

	if (strstr(buffer, ".qmission"))
		return SaveAs();

	CURSOR->SetBusy(1);

	MGlobals::SetTerrainFilename("");

	if ((inFile->GetAccessType() & GENERIC_WRITE) == 0)
	{
		// file is not writeable
		COMPTR<IFileSystem> parentFile;
		if (inFile->GetParentSystem(parentFile) == GR_OK)
		{
			COMPTR<IComponentFactory> factory;
			inFile.free();
			DAFILEDESC fdesc = buffer;
			fdesc.lpImplementation = "UTF";
			fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
			fdesc.dwShareMode = 0;		// no sharing, faster reads/ writes
			fdesc.dwCreationDistribution = CREATE_ALWAYS;
			if (buffer[1] == ':') // file was opened through GDI 
				factory = DACOM;
			else
				factory = parentFile;
			parentFile.free();

			if (factory->CreateInstance(&fdesc, inFile) != GR_OK)
			{
				CQERROR0("Failed to create save file.");
				goto Done;		// failed
			}
		}
		else
		{
			CQERROR0("Failed to create get parent file system.");
			goto Done;		// failed
		}
	}

#if !defined(_WIN32)
	if (!inFile->SetCurrentDirectory("\\") || !RecursiveDelete(inFile))
	{
		std::fputs("Native mission save could not clear the writable archive\n", stderr);
		goto Done;
	}
#else
	inFile->SetCurrentDirectory("\\");
	RecursiveDelete(inFile);
#endif
#if defined(_DEBUG) || !defined(_WIN32)
	if (SaveParseData() == 0)
	{
		CQERROR0("Failed write to save game");
		#if !defined(_WIN32)
		goto Done;
		#endif
	}
#endif

	// any one of these saves can fail, no?
#if defined(_WIN32)
	SECTOR->Save(inFile);
	savePlayerAI(inFile);
	CAMERAMANAGER->Save(inFile);
#else
	if (!SECTOR->Save(inFile) || !NativeSaveMissionSystemMap(inFile) ||
		!savePlayerAI(inFile) || !CAMERAMANAGER->Save(inFile))
		goto Done;
#endif
	NUGGETMANAGER->Save(inFile);
	#if !defined(_WIN32)
	if (!OBJLIST->Save(inFile))
	{
		std::fputs("Native mission object-list save failed\n", stderr);
		goto Done;
	}
	#else
	OBJLIST->Save(inFile);
	#endif
#if defined(_WIN32)
	THEMATRIX->Save(inFile);
#else
	if (!THEMATRIX->Save(inFile)) goto Done;
#endif
	MUSICMANAGER->Save(inFile);
#if defined(_WIN32)
	FOGOFWAR->Save(inFile);
#else
	if (!FOGOFWAR->Save(inFile)) goto Done;
#endif
	MScript::Save(inFile);
#if defined(_WIN32)
	MGlobals::Save(inFile);		// needs to save last, load first because it's referenced elsewhere
#else
	if (!MGlobals::Save(inFile)) goto Done;
#endif


#if defined(_WIN32)
	if (inFile!=0 && inFile->QueryInterface("IUTFWriter", pWriter)==GR_OK)
	{
		pWriter->Flush();
		pWriter.free();
	}
#else
	if (!inFile || inFile->QueryInterface("IUTFWriter", pWriter) != GR_OK) goto Done;
	pWriter->Flush();
	pWriter.free();
#endif

	// re-open the file for read-only
	{
		COMPTR<IFileSystem> parentFile;
		if (inFile->GetParentSystem(parentFile) == GR_OK)
		{
			COMPTR<IComponentFactory> factory;
			inFile.free();
			DAFILEDESC fdesc = buffer;
			if (buffer[1] == ':') // file was opened through GDI 
				factory = DACOM;
			else
				factory = parentFile;
			parentFile.free();

			if (factory->CreateInstance(&fdesc, inFile) != GR_OK)
			{
				CQERROR0("Failed to re-open save file.");
				goto Done;		// failed
			}
		}
		else
		{
			CQERROR0("Failed to create get parent file system.");
			goto Done;		// failed
		}
	}


	SetWindowTitle();
	bUnsavedData=0;
	result=1;
Done:
	CURSOR->SetBusy(0);
	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::LoadFromDescription (const wchar_t * fileDesc, IComponentFactory *pParent)
{
	return TRUE;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::SaveByDescription (const wchar_t * fileDesc, IComponentFactory *pParent, const char * szOverwrite, U32 saveKey)
{
	// set the new game description
	MGlobals::SetFileDescription(fileDesc);

	if (szOverwrite == NULL)
	{
		char szFile[MAX_PATH];

		if (IsSinglePlayerGame())
		{
#if defined(_WIN32)
			wsprintf(szFile, "f%07d", saveKey);
#else
			if (std::snprintf(szFile, sizeof(szFile), "f%07u", saveKey) >=
				static_cast<int>(sizeof(szFile))) return FALSE;
#endif
		}
		else
		{
#if defined(_WIN32)
			wsprintf(szFile, "m%07d", saveKey);
#else
			if (std::snprintf(szFile, sizeof(szFile), "m%07u", saveKey) >=
				static_cast<int>(sizeof(szFile))) return FALSE;
#endif
		}
		
		return Save(szFile, pParent);
	}
	else
	{
		return Save(szOverwrite, pParent);
	}
}
//--------------------------------------------------------------------------//
//
U32 Mission::GetFileDescription (const C8 * fileName, wchar_t * string, U32 numMaxChars)
{
	return MGlobals::GetFileDescription(fileName, string, numMaxChars);
}
//--------------------------------------------------------------------------//
//
U32 Mission::GetFileDescription (IFileSystem * fileDir, const C8 * fileName, wchar_t * string, U32 numMaxChars)
{
	return MGlobals::GetFileDescription(fileDir, fileName, string, numMaxChars);
}
//--------------------------------------------------------------------------//
//
U32 Mission::GetFileMaxPlayers (IFileSystem * fileDir, const C8 * fileName, U32 & maxPlayers)
{
	return MGlobals::GetFileMaxPlayers (fileDir,fileName,maxPlayers);
}
//--------------------------------------------------------------------------//
//
U32 Mission::GetFileMaxPlayers (const C8 * fileName, U32 & maxPlayers)
{
	return MGlobals::GetFileMaxPlayers (fileName,maxPlayers);
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::QuickSave (const C8 * fileName, struct IComponentFactory * pParent)
{
	BOOL32 result = 0;
	COMPTR<IUTFWriter> pWriter;
 
	if (inFile == 0 || fileName != 0)
		return QuickSaveAs(fileName, pParent);

	CURSOR->SetBusy(1);

	if ((inFile->GetAccessType() & GENERIC_WRITE) == 0)
	{
		// file is not writeable
		COMPTR<IFileSystem> parentFile;
		if (inFile->GetParentSystem(parentFile) == GR_OK)
		{
			COMPTR<IComponentFactory> factory;
			char buffer[MAX_PATH+4];
			inFile->GetFileName(buffer, sizeof(buffer));
			inFile.free();
			DAFILEDESC fdesc = buffer;
			fdesc.lpImplementation = "UTF";
			fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
			fdesc.dwShareMode = 0;		// no sharing, faster reads/ writes
			fdesc.dwCreationDistribution = CREATE_ALWAYS;
			if (buffer[1] == ':') // file was opened through GDI 
				factory = DACOM;
			else
				factory = parentFile;
			parentFile.free();

			if (factory->CreateInstance(&fdesc, inFile) != GR_OK)
			{
				CQERROR0("Failed to create save file.");
				goto Done;		// failed
			}
		}
		else
		{
			CQERROR0("Failed to create get parent file system.");
			goto Done;		// failed
		}
	}

#if !defined(_WIN32)
	if (!inFile->SetCurrentDirectory("\\") || !RecursiveDelete(inFile))
	{
		std::fputs("Native mission quicksave could not clear the writable archive\n", stderr);
		goto Done;
	}
#else
	inFile->SetCurrentDirectory("\\");
	RecursiveDelete(inFile);
#endif
	if (SaveParseData() == 0)
	{
		CQERROR0("Failed write to save game");
		goto Done;
	}
#if defined(_WIN32)
	SECTOR->QuickSave(inFile);
	OBJLIST->QuickSave(inFile);
#else
	if (!SECTOR->QuickSave(inFile) || !NativeSaveMissionSystemMap(inFile) ||
		!OBJLIST->QuickSave(inFile)) goto Done;
#endif
	MUSICMANAGER->Save(inFile);
#if defined(_WIN32)
	MGlobals::QuickSave(inFile);
#else
	if (!MGlobals::QuickSave(inFile)) goto Done;
#endif

#if defined(_WIN32)
	if (inFile!=0 && inFile->QueryInterface("IUTFWriter", pWriter)==GR_OK)
		pWriter->Flush();
#else
	if (!inFile || inFile->QueryInterface("IUTFWriter", pWriter) != GR_OK) goto Done;
	pWriter->Flush();
#endif

	SetWindowTitle();
	bUnsavedData=0;
	result=1;
Done:
	CURSOR->SetBusy(0);
	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::QuickSaveFile (void)
{
	BOOL32 result;

	if (IsSinglePlayerGame())
	{
		SetFileDescription(_localLoadStringW(IDS_SAVE_QSMISSION));
		result = Save("f0000000.mission", SAVEDIR);
	}
	else
	{
		SetFileDescription(_localLoadStringW(IDS_SAVE_QSQUICKBATTLE));
		result = Save("m0000000.mission", SAVEDIR);
	}

	if (result == TRUE)
	{
		SCROLLTEXT->SetText(IDS_QUICKSAVE_SUCCESS);
	}
#if !defined(_WIN32)
	else
	{
		SCROLLTEXT->SetTextString(L"Quicksave failed.");
	}
#endif

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::QuickLoadFile (void)
{
	// are we quick loading a quick battle or a single player game?
	char szDescription[32];
	memset(szDescription, 0, sizeof(szDescription));

	if (IsSinglePlayerGame())
	{
		strcpy(szDescription, "f0000000.mission");
	}
	else
	{
		strcpy(szDescription, "m0000000.mission");
	}
	
	// first make sure that the file exists!
	WIN32_FIND_DATA data;
	const HANDLE findHandle = SAVEDIR ? SAVEDIR->FindFirstFile(szDescription, &data) : INVALID_HANDLE_VALUE;
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		SCROLLTEXT->SetText(IDS_QUICKLOAD_FAILED);
		return FALSE;
	}
	else
	{
//		SCROLLTEXT->SetText(IDS_QUICKLOAD_WAIT);
		SAVEDIR->FindClose(findHandle);
		BOOL32 result = MISSION->Load(szDescription, SAVEDIR);
#if defined(_WIN32)
		SCROLLTEXT->SetText(IDS_QUICKLOAD_SUCCESS);
#else
		SCROLLTEXT->SetText(result ? IDS_QUICKLOAD_SUCCESS : IDS_QUICKLOAD_FAILED);
#endif
		return result;
	}
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::DynamicSave (const C8 * fileName, struct IComponentFactory * pParent)
{
	BOOL32 result = 0;
	COMPTR<IUTFWriter> pWriter;
 
	if (inFile == 0 || fileName != 0)
		return DynamicSaveAs(fileName, pParent);

	CURSOR->SetBusy(1);

	if ((inFile->GetAccessType() & GENERIC_WRITE) == 0)
	{
		// file is not writeable
		COMPTR<IFileSystem> parentFile;
		if (inFile->GetParentSystem(parentFile) == GR_OK)
		{
			COMPTR<IComponentFactory> factory;
			char buffer[MAX_PATH+4];
			inFile->GetFileName(buffer, sizeof(buffer));
			inFile.free();
			DAFILEDESC fdesc = buffer;
			fdesc.lpImplementation = "UTF";
			fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
			fdesc.dwShareMode = 0;		// no sharing, faster reads/ writes
			fdesc.dwCreationDistribution = CREATE_ALWAYS;
			if (buffer[1] == ':') // file was opened through GDI 
				factory = DACOM;
			else
				factory = parentFile;
			parentFile.free();

			if (factory->CreateInstance(&fdesc, inFile) != GR_OK)
			{
				CQERROR0("Failed to create save file.");
				goto Done;		// failed
			}
		}
		else
		{
			CQERROR0("Failed to create get parent file system.");
			goto Done;		// failed
		}
	}

#if !defined(_WIN32)
	if (!inFile->SetCurrentDirectory("\\") || !RecursiveDelete(inFile))
	{
		std::fputs("Native mission quicksave could not clear the writable archive\n", stderr);
		goto Done;
	}
#else
	inFile->SetCurrentDirectory("\\");
	RecursiveDelete(inFile);
#endif
	if (SaveParseData() == 0)
	{
		CQERROR0("Failed write to save game");
		goto Done;
	}
#if defined(_WIN32)
	OBJLIST->QuickSave(inFile,true);
	MGlobals::QuickSave(inFile);
#else
	if (!NativeSaveMissionSystemMap(inFile) || !OBJLIST->QuickSave(inFile,true) ||
		!MGlobals::QuickSave(inFile)) goto Done;
#endif

#if defined(_WIN32)
	if (inFile!=0 && inFile->QueryInterface("IUTFWriter", pWriter)==GR_OK)
		pWriter->Flush();
#else
	if (!inFile || inFile->QueryInterface("IUTFWriter", pWriter) != GR_OK) goto Done;
	pWriter->Flush();
#endif

	SetWindowTitle();
	bUnsavedData=0;
	result=1;
Done:
	CURSOR->SetBusy(0);
	return result;
}
//-------------------------------------------------------------------
// receive notifications from event system
//
GENRESULT Mission::Notify (U32 message, void *param)
{
#if defined(_WIN32)
	MSG *msg = (MSG *) param;
	const U32 param32 = U32(param);
#else
	const auto *msg = static_cast<const FrontierX::LegacyWindowMessage *>(param);
	const std::uintptr_t encoded = reinterpret_cast<std::uintptr_t>(param);
	if ((message == CQE_UPDATE || message == CQE_HOTKEY ||
		message == CQE_DELETEPLAYER || message == CQE_MISSION_PROG_ENDING) &&
		encoded > UINT32_MAX) return GR_INVALID_PARMS;
	const U32 param32 = static_cast<U32>(encoded);
#endif

	switch (message)
	{
	case CQE_GAME_ACTIVE:
		setDefaultCheatSettings();
		break;

	case CQE_NETPACKET:
		if (CQFLAGS.bGameActive)
		{
			BASE_PACKET * packet = (BASE_PACKET *) param;
			if (packet->type == PT_ENDGAME && packet->fromID == HOSTID)
			{
				ENDGAME_PACKET * p = (ENDGAME_PACKET *) packet;
				deferredMissionEndTime = DEFERRED_MISSION_END_TIME;
				deferredMissionWinners = p->allyMask;
			}
			else
			if (packet->type == PT_AIRESIGN)
			{
				AIRESIGN_PACKET * p = (AIRESIGN_PACKET *) packet;

				setAIResign(p->playerID);
			}
			else
			if (packet->type == PT_RESIGN)
			{
				U32 playerID = MGlobals::GetPlayerIDFromDPID(packet->fromID);
				U32 slotID = MGlobals::GetSlotIDFromDPID(packet->fromID);

				if (slotID < MAX_PLAYERS)
				{
					// tell the user that someone has resigned
					wchar_t msg[255];
					wchar_t name[64];

					// if the player is already marked as 'resigned' then don't print any messages
					// they've already been labelled has resigned if they have been defeated
					if (MGlobals::HasPlayerResigned(slotID) == false)
					{
						MGlobals::GetPlayerNameFromDPID(packet->fromID, name, sizeof(name));
#if defined(_WIN32)
						swprintf(msg, _localLoadStringW(IDS_FORMAT_PLAYERRESIGNED), name);
#else
						if (!NativeFormatPlayerMessage(msg, 255,
							_localLoadStringW(IDS_FORMAT_PLAYERRESIGNED), name))
							return GR_GENERIC;
#endif
						SCROLLTEXT->SetTextString(msg);
						CQTRACE11("RESIGNATION -- %S", msg);
					}

					if (slotID < MAX_PLAYERS)
						MGlobals::SetPlayerResignedBySlot(slotID);
					MGlobals::RemoveDPIDFromPlayerID(packet->fromID, playerID);

#if defined(_WIN32)
					if (ZONESCORE)
					{
						U32 slots[MAX_PLAYERS];
						U32 numSlots = MGlobals::GetSlotIDForPlayerID(playerID, slots);
						U32 allResigning = (MGlobals::GetPlayerDPIDForPlayerID(playerID) == 0);
						U32 flags;
						U32 i;

						for (i = 0; i < numSlots; i++)
						{
							if (allResigning || slotID == slots[i])
							{
								flags = ZONESCORE_RESIGNED|ZONESCORE_LOSER;
								if (slotID != slots[i])   // assume computer player
									flags |= ZONESCORE_COMPUTERPLAYER;
								ZONESCORE->SetPlayerFlags(MGlobals::GetZoneSeatFromSlot(slots[i]), flags);
							}
						}
					}
#endif

					if (THEMATRIX->IsMaster())
					{
						if (MGlobals::GetPlayerDPIDForPlayerID(playerID) == 0)	// no more humans for this player
							THEMATRIX->TerminatePlayerUnits(playerID);
					}
				}
			}
			else
			if (packet->type == PT_PLAYERLOST && packet->fromID == HOSTID)
			{
				PLAYERLOST_PACKET * p;
				p = (PLAYERLOST_PACKET *) packet;

				// scrolling text message
				wchar_t msg[255];
				wchar_t name[64];

				// arrrgh!  Go through all the slots, see if there is a match for the playerID, get the
				// name(s) and post the message
				const CQGAME & cqgame = MGlobals::GetGameSettings();
				for (U32 i = 0; i < cqgame.activeSlots; i++)
				{
					U32 id = MGlobals::GetPlayerIDFromSlot(i);

					if (id == p->playerID)
					{
						U32 result = MGlobals::GetPlayerNameBySlot(i, name, sizeof(name));

						// if there was no name, then the player must be a computer
						if (result == 0)
						{
							wcsncpy(name, _localLoadStringW(IDS_COMPUTER_NAME), 32);
						}

#if defined(_WIN32)
						swprintf(msg, _localLoadStringW(IDS_FORMAT_PLAYEROUT), name);
#else
						if (!NativeFormatPlayerMessage(msg, 255,
							_localLoadStringW(IDS_FORMAT_PLAYEROUT), name))
							return GR_GENERIC;
#endif
						SCROLLTEXT->SetTextString(msg);
						CQTRACE11("LOSING -- %S", msg);

						MGlobals::SetPlayerResignedBySlot(i);

#if defined(_WIN32)
						if (ZONESCORE)
						{
							U32 flags = ZONESCORE_LOSER;
							if (result == 0 || cqgame.slot[i].dpid==0)
								flags |= ZONESCORE_COMPUTERPLAYER;
							ZONESCORE->SetPlayerFlags(MGlobals::GetZoneSeatFromSlot(i), flags);
						}
#endif
					}
				}
				
				// if you are not spectating, then leave the game, otherwise have some nice big teletype text come
				// up telling the user that he lost so he knows for sure that he's a loser and is just spectating
				if (p->playerID == MGlobals::GetThisPlayer())
				{
					if (IsSinglePlayerGame() == false)
					{
						// if we are allowed to spectate, then spectate dammit
						if (DEFAULTS->GetDefaults()->bSpectatorModeAllowed)
						{
							enableSpectatorMode();
						}
						else
						{
							// end the game for us
							deferredMissionEndTime = DEFERRED_MISSION_END_TIME;
							deferredMissionWinners = ~MGlobals::GetAllyMask(MGlobals::GetThisPlayer());
						}
					}
					else
					{
						// we are playing a single player game, end the game
						deferredMissionWinners = ~MGlobals::GetAllyMask(MGlobals::GetThisPlayer());
						deferredMissionEndTime = 1;
					}
				}
			}
		}
		break;
	case CQE_KILL_FOCUS:
		if ((Mission *)param != this)
			bHasFocus = FALSE;
		break;
	case CQE_SET_FOCUS:
		bHasFocus = TRUE;
		break;

	case CQE_UPDATE:
		// is it time to put a an endmission message box?
		if (CQFLAGS.bGamePaused == false && deferredMissionEndTime!=0 && bHasFocus)
		{
			deferredMissionEndTime -= static_cast<S32>(param32)>>10;
			if (deferredMissionEndTime <= 0)
			{
				deferredMissionEndTime = 0;
				bOnlyHumanPlayerMsgNeeded = 0;
				EndMission(deferredMissionWinners,bDeferredMissionEndNonFail != false);
			}
		}
		updateNetFile();
		if (CQFLAGS.bGameActive)
		{
			const U32 updateCount = MGlobals::GetUpdateCount();
			const U32 updateDiff = updateCount - endCheckedCount;			// number of times we have updated AI
			if (updateDiff > U32(RENDER_FRAMERATE)*5)
			{
				endCheckedCount = updateCount;
				checkEndGame();
			}
			if (PLAYERID!=0)
			{
				if (updateCount > ((U32)RENDER_FRAMERATE)*60*10)		// 10 minutes
				{
					const USER_DEFAULTS & defaults = *DEFAULTS->GetDefaults();
							// uh-oh! someone's illegally cheating!
							// make game not-so-good
					if (defaults.fogMode != initialDefaults.fogMode ||
						(defaults.bSpectatorModeOn==0 && defaults.bVisibilityRulesOff != initialDefaults.bVisibilityRulesOff) ||
						defaults.gameSpeed != initialDefaults.gameSpeed ||
						defaults.bCheatsEnabled != initialDefaults.bCheatsEnabled || 
						defaults.bLockDiplomacy != initialDefaults.bLockDiplomacy ||
						defaults.bSpectatorModeAllowed != initialDefaults.bSpectatorModeAllowed ||
						(defaults.bSpectatorModeOn && initialDefaults.bSpectatorModeAllowed==0) )
					{
						EVENTSYS->Post(CQE_HOTKEY, (void *)IDH_GAME_OPTIONS);
					}
				}
			}
			if (bOnlyHumanPlayerMsgNeeded && bHasFocus)
			{
				bOnlyHumanPlayerMsgNeeded = false;
				// notify the player he is the only human left playing
				if (deferredMissionEndTime==0)		// don't do this if mission is ending
				{
#if defined(_WIN32)
					CQMessageBox(IDS_HELP_NOHUMANS, IDS_APP_NAMETM, MB_OK);
#else
					return GR_INTERFACE_UNSUPPORTED;
#endif
				}
			}
		}
		break;


	case CQE_HOTKEY:
		switch (param32)
		{
		case IDH_WIN_CLOSE:
#if defined(_WIN32)
			PostMessage(hMainWindow, WM_CLOSE, 0, 0);
#else
			return GR_INTERFACE_UNSUPPORTED;
#endif
			break;
		} 
		break; // end case CQE_HOTKEY

	case WM_CLOSE:
		ipAnim.free();		// make sure background drawing is shutdown
		break;

	case CQE_ENTERING_INGAMEMODE:
		IGM_LOG("Mission: entered\n");
		EVENTSYS->Send(CQE_LOAD_TOOLBAR, 0);		// unload interface
		IGM_LOG("Mission: after Send(CQE_LOAD_TOOLBAR)\n");
		break;

	case CQE_DELETEPLAYER:
		if (param32 != PLAYERID)		// ignore booting case
		{
			U32 playerID = MGlobals::GetPlayerIDFromDPID(param32);
			if (playerID)
			{
				// notify everyone that a player has left the building
				// has left the game
				// scrolling text message

				U32 slotID = MGlobals::GetSlotIDFromDPID(param32);
				if (slotID < MAX_PLAYERS)
				{
					wchar_t msg[255];
					wchar_t name[64];

					U32 nPlayers;
					nPlayers = MGlobals::GetPlayerDPIDForPlayerID(playerID, NULL);

					U32 result = MGlobals::GetPlayerNameBySlot(slotID, name, sizeof(name));

					CQASSERT(result);		// cannot be a computer player here!
#if defined(_WIN32)
					swprintf(msg, _localLoadStringW(IDS_FORMAT_PLAYERLEFT), name);
#else
					if (!NativeFormatPlayerMessage(msg, 255,
						_localLoadStringW(IDS_FORMAT_PLAYERLEFT), name))
						return GR_GENERIC;
#endif
					SCROLLTEXT->SetTextString(msg);
					CQTRACE11("QUITTING -- %S", msg);

					MGlobals::RemoveDPIDFromPlayerID(param32, playerID);
					if (MGlobals::HasPlayerResigned(slotID) == false)
					{
#if defined(_WIN32)
						if (ZONESCORE)
							ZONESCORE->SetPlayerFlags(MGlobals::GetZoneSeatFromSlot(slotID), ZONESCORE_DISCONNECTED);
#endif
						
						if (nPlayers == 1)
						{
#if defined(_WIN32)
							if (ZONESCORE)
							{
								//
								// add a new player for the zone to track
								//
								char buffer[64];
								U32 dwSeat;
								strncpy(buffer, _localLoadString(IDS_COMPUTER_NAME), sizeof(buffer));
								ZONESCORE->AddNewPlayer(buffer, 0, MGlobals::GetAllyMask(playerID), ZONESCORE_COMPUTERPLAYER, &dwSeat);
								MGlobals::SetZoneSeatFromSlot(slotID, dwSeat);
							}
#endif
							// no more human players, create a computer player
							createPlayerAI(MGlobals::GetPlayerRace(playerID), playerID, playerAI[playerID-1]);
						}
					}
				}
			}
			// are there any human oppopents left?
			if (CQFLAGS.bGameActive && deferredMissionEndTime==0 && MGlobals::IsHostOnlyPlayerLeft())
			{
				bOnlyHumanPlayerMsgNeeded = true;
			}
		}
		break;

	case CQE_MISSION_PROG_ENDING:
		deferredMissionEndTime = DEFERRED_MISSION_END_TIME;
		deferredMissionWinners = param32;
		break;

	case CQE_MISSION_ENDING_SPLASH:
		deferredMissionEndTime = 500; // end 1/2 second latter
		deferredMissionWinners = 0;
		bDeferredMissionEndNonFail = true;
		memcpy( &CQ_SplashInfo, (SPLASHINFO*)param, sizeof(CQ_SplashInfo) );
		break;

	case CQE_PLAYER_RESIGN:
		EVENTSYS->Send(CQE_DLG_RESULT, (void *)MISSION_END_RESIGN);			// go back to shell
		EVENTSYS->Send(CQE_MISSION_ENDING, (void *)MISSION_END_RESIGN);		// send result code
		destroyPlayerAI();
		break;
	case CQE_PLAYER_QUIT:
		EVENTSYS->Send(CQE_DLG_RESULT, (void *)MISSION_END_QUIT);			// go back to shell
		EVENTSYS->Send(CQE_MISSION_ENDING, (void *)MISSION_END_QUIT);		// send result code
		destroyPlayerAI();
		break;

#if defined(_WIN32)
	case WM_CHAR:
		if (bHasFocus)
		switch (TCHAR(msg->wParam))
		{
		case 27:		//ESC
			if (CQFLAGS.bGameActive)
			{
				U32 igResult = CreateMenu_igoptions();
				if(igResult)		// player wants to resign
				{
					if(igResult == 1)
					{
						EVENTSYS->Send(CQE_DLG_RESULT, (void *)MISSION_END_RESIGN);			// go back to shell
						EVENTSYS->Send(CQE_MISSION_ENDING, (void *)MISSION_END_RESIGN);		// send result code
						destroyPlayerAI();
					}
					else
					{
						EVENTSYS->Send(CQE_DLG_RESULT, (void *)MISSION_END_QUIT);			// go back to shell
						EVENTSYS->Send(CQE_MISSION_ENDING, (void *)MISSION_END_QUIT);		// send result code
						destroyPlayerAI();
					}
				}
			}
			break;
		}
		break;  // end case WM_CHAR

	case WM_INITMENUPOPUP:
		if (HIWORD(msg->lParam)==0)
		{
			if (LOWORD(msg->lParam) == MENUPOS_MRU_MISSION)
				SetMissionMRUList((HMENU)msg->wParam);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(msg->wParam))
		{
		case IDM_RELOAD_DATABASE:
			reloadGametypes();
			break;

		case IDM_MRU0_MISSION:
		case IDM_MRU1_MISSION:
		case IDM_MRU2_MISSION:
		case IDM_MRU3_MISSION:
			EVENTSYS->Send(CQE_NEW_SELECTION, this);
			LoadMission(LOWORD(msg->wParam));
			break;
		
		case IDM_OPEN_MISSION:
			EVENTSYS->Send(CQE_NEW_SELECTION, this);
			if (Load()==0)
				New();
			break;
		
		case IDM_LOAD_TERRAIN:
			EVENTSYS->Send(CQE_NEW_SELECTION, this);
			if (Load()==0)
				New();
			if (inFile)
			{
				char buffer[MAX_PATH+4];
				inFile->GetFileName(buffer,sizeof(buffer));
				char * ptr = strrchr(buffer,'\\');
				if(!ptr)
					ptr = buffer;
				MGlobals::SetTerrainFilename(ptr);
				inFile.free();
				SetWindowTitle();
			}
			break;

		/*
		case IDM_CLOSE_MISSION:
			EVENTSYS->Send(CQE_NEW_SELECTION, this);
			close();
			break;
		*/

		case IDM_NEW_MISSION:
			EVENTSYS->Send(CQE_NEW_SELECTION, this);
			if (close())
			{
				New();
			}
			break;

		case IDM_ENABLE_RUSE:
			if (CQFLAGS.bGameActive && QuickSave() && inFile!=0)
			{
				bRuseEnabled = 1;
				CQFLAGS.bGameActive = 0;
				ActivateRUSE(inFile);
			}
			break;

 		case IDM_RUSE_EXITMAPEDITOR:
			if (bRuseEnabled && DeactivateRUSE())
			{
				bRuseEnabled = 0;
				CQFLAGS.bGameActive = 1;
				Reload();
//				SendMessage(hMainWindow, WM_COMMAND, IDM_MRU0_MISSION, 0);
			}
			break;
		
		case IDM_SAVE_MISSION:
			Save();
			break;

		case IDM_SAVEAS_MISSION:
			SaveAs();
			break;

		case IDM_QUICK_SAVE:
			{
				char buffer[MAX_PATH+4];
				MGlobals::GetTerrainFilename(buffer,sizeof(buffer));
				if(buffer[0])
					DynamicSave();
				else
					QuickSave();
			}
			break;

		case IDM_QUICK_SAVE_AS:
			{
				char buffer[MAX_PATH+4];
				MGlobals::GetTerrainFilename(buffer,sizeof(buffer));
				if(buffer[0])
					DynamicSaveAs();
				else
					QuickSaveAs();
			}
			break;

		case IDM_LOAD_FILTER:
			LoadFromFilter();
			break;

		case IDM_ALL_TECH:
			{
				TECHNODE node;
				memset(&node, 0, sizeof(node));
				node.InitLevel(TECHTREE::FULL_TREE);
				for(U32 i = 1; i <=MAX_PLAYERS; ++i)
				{
					MGlobals::SetCurrentTechLevel(node,i);
				}
			}
			break;

		case IDM_FIX_CP:
			{
				for(U32 i = 1; i <= MAX_PLAYERS; ++i)
				{
					MGlobals::SetCurrentUsedComPts(i,0);
					if(MGlobals::GetMaxControlPoints(i) == 0)
						MGlobals::SetMaxControlPoints(i,100);
				}
				IBaseObject * obj = OBJLIST->GetTargetList();
				while(obj)
				{
					MPart part(obj);
					if(part.isValid() && part->bUnderCommand)
					{
						U32 playerID = obj->GetPlayerID();
						if(playerID)
							MGlobals::SetCurrentUsedComPts(playerID,MGlobals::GetCurrentUsedComPts(playerID)+part.pInit->resourceCost.commandPt);
					}
					obj = obj->nextTarget;
				}
			}
			break;
		}
		break;
#else
	case FrontierX::LegacyMessage::Character:
		if (!msg || msg->message != message) return GR_INVALID_PARMS;
		if (bHasFocus && msg->wparam == 27) return GR_INTERFACE_UNSUPPORTED;
		break;
#endif

	}

	MScript::Notify(message, param);

	return GR_OK;
}
//-------------------------------------------------------------------
//
void Mission::setAIResign (U32 playerID)
{
	U32 slots[MAX_PLAYERS];
	U32 numSlots = MGlobals::GetSlotIDForPlayerID(playerID, slots);

	if (numSlots)
	{
		U32 index;
		const struct CQGAME & cqgame = MGlobals::GetGameSettings();

		for (index = 0; index < numSlots; index++)
		{
			if (MGlobals::HasPlayerResigned(slots[index]) == 0)
			{
				// tell the user that someone has resigned
				wchar_t msg[255];
				wchar_t name[64];

				MGlobals::GetPlayerNameBySlot(slots[index], name, sizeof(name));
#if defined(_WIN32)
				swprintf(msg, _localLoadStringW(IDS_FORMAT_PLAYERRESIGNED), name);
#else
				if (!NativeFormatPlayerMessage(msg, 255,
					_localLoadStringW(IDS_FORMAT_PLAYERRESIGNED), name))
					CQBOMB0("Unsupported native player-status format.");
#endif
				SCROLLTEXT->SetTextString(msg);
				CQTRACE11("RESIGNATION -- %S", msg);

				MGlobals::SetPlayerResignedBySlot(slots[index]);
				MGlobals::RemoveDPIDFromPlayerID(cqgame.slot[slots[index]].dpid, playerID);
			}
		}

		if (THEMATRIX->IsMaster())
		{
			THEMATRIX->TerminatePlayerUnits(playerID);
		}

		playerAI[playerID-1].free();
	}
}
//-------------------------------------------------------------------
//
void Mission::setDefaultCheatSettings (void)
{
	g_bCheatsOn = false;
	CQFLAGS.bInstantBuilding = false;
	CQFLAGS.bEverythingFree = false;
}
//-------------------------------------------------------------------
// receive notifications from resources
//
BOOL32 Mission::Notify (struct IResource *res, U32 message, void *parm)
{
	return GR_OK;
}

//-------------------------------------------------------------------
//
void Mission::Update()
{
	static U32 tracedUpdates = 0;
	const bool trace = tracedUpdates++ < 4;
	if (trace)
		MIS_LOGF("Mission::Update: paused=%u rtPaused=%u matrix=%p\n",
			CQFLAGS.bGamePaused, CQFLAGS.bRTGamePaused, (void *)THEMATRIX);
	if(CQFLAGS.bGamePaused==0 && CQFLAGS.bRTGamePaused==0)
	{
		S64 pretick, posttick;
		QueryPerformanceCounter((LARGE_INTEGER *)&pretick);
		if (trace) MIS_LOG("Mission::Update: before matrix\n");
		THEMATRIX->Update();
		if (trace) MIS_LOG("Mission::Update: after matrix\n");
		QueryPerformanceCounter((LARGE_INTEGER *)&posttick);
		OBJLIST->AddTimerTicks(TIMING_OPAGENT, posttick-pretick);
		if (trace) MIS_LOG("Mission::Update: before script\n");
		MScript::Update();
		if (trace) MIS_LOG("Mission::Update: after script\n");
		if (THEMATRIX->IsMaster())
		{
			if (trace) MIS_LOG("Mission::Update: before scores\n");
			calculateCtrlO_Scores();
			if (trace) MIS_LOG("Mission::Update: after scores\n");
		}


		if (THEMATRIX->IsMaster())
		{
			int i;
			QueryPerformanceCounter((LARGE_INTEGER *)&pretick);
			for (i = 0; i < MAX_PLAYERS; i++)
			{
				if (playerAI[i])
				{
					if (trace) MIS_LOGF("Mission::Update: before playerAI[%d]\n", i);
					playerAI[i]->Update();
					if (trace) MIS_LOGF("Mission::Update: after playerAI[%d]\n", i);
				}
			}
			QueryPerformanceCounter((LARGE_INTEGER *)&posttick);
			OBJLIST->AddTimerTicks(TIMING_PLAYERAI, posttick-pretick);
		}
	}
	else if(CQFLAGS.bGamePaused==0 && CQFLAGS.bRTGamePaused!=0)
	{
		if (trace) MIS_LOG("Mission::Update: before paused script\n");
		MScript::Update();
		if (trace) MIS_LOG("Mission::Update: after paused script\n");
	}
	if (trace) MIS_LOG("Mission::Update: done\n");
}
//-------------------------------------------------------------------
//
BOOL32 Mission::OnAppClose (void)
{
#if !defined(_WIN32)
	// Until native save and confirmation UI exist, refuse an unsafe close.
	if (bUnsavedData || CQFLAGS.bGameActive) return TRUE;
	return close() == 0;
#else
	USER_DEFAULTS * defaults = DEFAULTS->GetDefaults();
	BOOL32 result=0;

//	if (bHasFocus==0)
//		return 0;
	if (CQFLAGS.bInProgressAnimActive)
		return TRUE;
	if (CQFLAGS.bFullScreenMovie)
		return FALSE;			// always exit without comment during a movie

	if (bRuseEnabled)
	{
		return (DeactivateRUSE() == 0);		// cancel if ruse did not deactivate
	}

	if (CQFLAGS.bNoExitConfirm == 0)
	{
		static bool bRecursion;
		if (bRecursion)
			return 1;						// cancel if recursion
		bRecursion = true;
		if (CQMessageBox(IDS_CONFIRM_QUIT, IDS_CONFIRM, MB_OKCANCEL) == 0)
		{
			bRecursion = false;
			return 1;							// cancel if user changed his mind
		}
		bRecursion = false;
	}

	if (CQFLAGS.bGameActive && PLAYERID)
	{
		CQFLAGS.bGameActive = 0;
		EVENTSYS->Send(CQE_GAME_ACTIVE, (void*)0);
		DoMenu_nul(false);	// do net unloading
	}

	if (defaults->bNoAutoSave)
	{
		bUnsavedData = false;
		goto Done;
	}

Done:
	result = (close() == 0);		// do not end program if save failed
	return result;
#endif
}
//--------------------------------------------------------------------------//
//
GENRESULT Mission::RemoteFileRequest (const C8 *fileName, IFileSystem ** file)
{
	GENRESULT result = GR_GENERIC;

	if (strcmp(fileName, "??MISSION") == 0)
	{
		if (inFile!=0)
		{
			MEMFILEDESC mdesc = "??MISSION";
			DWORD dwRead;
			mdesc.dwDesiredAccess  = GENERIC_READ;
			mdesc.dwCreationDistribution = OPEN_EXISTING;

			mdesc.dwBufferSize = inFile->GetFileSize();
			mdesc.lpBuffer = malloc(mdesc.dwBufferSize);
			mdesc.dwFlags = CMF_DONT_COPY_MEMORY | CMF_OWN_MEMORY;
			inFile->SetFilePointer(0,0);
			inFile->ReadFile(0, mdesc.lpBuffer, mdesc.dwBufferSize, &dwRead, 0);
			if ((result = DACOM->CreateInstance(&mdesc, (void **) file)) != GR_OK)
				free(mdesc.lpBuffer);
		}
	}
	return result;
}
//--------------------------------------------------------------------------//
//
void Mission::SetMissionMRUList (HMENU hMenu)
{
#if !defined(_WIN32)
	CQBOMB0("Native mission MRU menu is unavailable.");
#else
	MENUITEMINFO minfo;
	DWORD i;
	char buffer[MAX_PATH+4];

	for (i = 0; i < 4; i++)
	{
		::RemoveMenu(hMenu, IDM_MRU0_MISSION+i, MF_BYCOMMAND);
	}

	for (i = 0; i < 4; i++)
	{
		memset(&minfo, 0, sizeof(minfo));
		minfo.cbSize = sizeof(minfo);
		minfo.fMask = MIIM_ID | MIIM_TYPE;
		minfo.fType = MFT_STRING;
		minfo.wID = IDM_MRU0_MISSION+i;
		minfo.dwTypeData = buffer;

		if (DEFAULTS->GetNameInMRU(buffer, IDS_MISSION_FILTER, i) == 0)
			break;

		minfo.cch = strlen(buffer);
		
		InsertMenuItem(hMenu, 0x7FFE, 1, &minfo);
	}
#endif
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::SaveAs (const C8 * fileName, struct IComponentFactory * pParent)
{
	BOOL32 result = 0;
	COMPTR<IFileSystem> oldFile = inFile;
	char buffer[MAX_PATH+4];

	buffer[0] = 0;
	if (fileName)
		strcpy(buffer, fileName);
	if (pParent == 0)
		pParent = DACOM;
	if (buffer[0] || DEFAULTS->GetOutputFilename(buffer, IDS_MISSION_FILTER))
	{
		DAFILEDESC fdesc = buffer;

		if (strchr(buffer, '.') == 0)
		{
			strcat(buffer, ".mission");
			DEFAULTS->SetNameInMRU(buffer, IDS_MISSION_FILTER, 0);
		}
		fdesc.lpImplementation = "UTF";
		fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
		fdesc.dwShareMode = 0;		// no sharing, faster reads/ writes
		fdesc.dwCreationDistribution = CREATE_ALWAYS;

		if (pParent->CreateInstance(&fdesc, inFile) != GR_OK)
		{
			inFile = oldFile;
			CQERROR0("Failed to create save file.");
			goto Done;
		}
		else
		if ((result=Save(0,pParent))==0)
		{
			inFile = oldFile;
			goto Done;
		}

	}

Done:
	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::QuickSaveAs (const C8 * fileName, struct IComponentFactory * pParent)
{
	BOOL32 result = 0;
	COMPTR<IFileSystem> oldFile = inFile;
	char buffer[MAX_PATH+4];

	buffer[0] = 0;
	if (fileName)
		strcpy(buffer, fileName);
	if (pParent == 0)
		pParent = DACOM;
	if (buffer[0] || DEFAULTS->GetOutputFilename(buffer, IDS_QUICK_MISSION_FILTER))
	{
		DAFILEDESC fdesc = buffer;

		if (strchr(buffer, '.') == 0)
		{
			strcat(buffer, ".qmission");
			DEFAULTS->SetNameInMRU(buffer, IDS_QUICK_MISSION_FILTER, 0);
		}
		fdesc.lpImplementation = "UTF";
		fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
		fdesc.dwShareMode = 0;		// no sharing, faster reads/ writes
		fdesc.dwCreationDistribution = CREATE_ALWAYS;

		if (pParent->CreateInstance(&fdesc, inFile) != GR_OK)
		{
			inFile = oldFile;
			CQERROR0("Failed to create save file.");
			goto Done;
		}
		else
		if ((result=QuickSave(0,pParent))==0)
		{
			inFile = oldFile;
			goto Done;
		}
	}

Done:
	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::DynamicSaveAs (const C8 * fileName, struct IComponentFactory * pParent)
{
	BOOL32 result = 0;
	COMPTR<IFileSystem> oldFile = inFile;
	char buffer[MAX_PATH+4];

	buffer[0] = 0;
	if (fileName)
		strcpy(buffer, fileName);
	if (pParent == 0)
		pParent = DACOM;
	if (buffer[0] || DEFAULTS->GetOutputFilename(buffer, IDS_DYNAMIC_MISSION_FILTER))
	{
		DAFILEDESC fdesc = buffer;

		if (strchr(buffer, '.') == 0)
		{
			strcat(buffer, ".dmission");
			DEFAULTS->SetNameInMRU(buffer, IDS_DYNAMIC_MISSION_FILTER, 0);
		}
		fdesc.lpImplementation = "UTF";
		fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
		fdesc.dwShareMode = 0;		// no sharing, faster reads/ writes
		fdesc.dwCreationDistribution = CREATE_ALWAYS;

		if (pParent->CreateInstance(&fdesc, inFile) != GR_OK)
		{
			inFile = oldFile;
			CQERROR0("Failed to create save file.");
			goto Done;
		}
		else
		if ((result=DynamicSave(0,pParent))==0)
		{
			inFile = oldFile;
			goto Done;
		}
	}

Done:
	return result;
}
//--------------------------------------------------------------------------//
//
void Mission::LoadFromFilter()
{
	char buffer[MAX_PATH+4];
	DAFILEDESC fdesc = buffer;

	fdesc.lpImplementation = "UTF";
	buffer[0] = 0;
	if (DEFAULTS->GetInputFilename(buffer, IDS_MISSION_FILTER))
	{
		COMPTR<IFileSystem> filterFile;
		if (DACOM->CreateInstance(&fdesc, filterFile) == GR_OK)
		{
			baseObjPercent = 0;
			totalObjPercent = 0;
			OBJLIST->Load(filterFile,false,true);
			OBJLIST->ResolveAssociations();
		}
	}

}
//--------------------------------------------------------------------------//
//
void Mission::SetWindowTitle (void)
{
#if defined(_WIN32)
	// setup program needs to search on the official title
	if (CQFLAGS.bNoGDI==0)
	{
		char buffer[MAX_PATH+4+36];
		char *ptr;
		int len;

		::GetWindowText(hMainWindow, buffer, sizeof(buffer));

		len = strlen(buffer);
		if ((ptr = strchr(buffer, '[')) == 0)
		{
			buffer[len++] = ' ';
			ptr = buffer+len;
			buffer[len++] = '[';
			buffer[len] = 0;
		}

		ptr++;
		*ptr=0;

		if (inFile)
			inFile->GetFileName(ptr, sizeof(buffer)-(ptr-buffer));
		strcat(buffer, "] ");

		::SetWindowText(hMainWindow, buffer);
	}
#endif
}
//--------------------------------------------------------------------------//
//
void Mission::StartNetDownload (void)
{
#if !defined(_WIN32)
	RequireNativeMissionFileTransfer();
#endif
	CQASSERT(HOSTID != PLAYERID);		//can't download if we are the host
	CQASSERT(downloadChannel==0);

	if (close() == 0)
	{
  		CQBOMB0("Close failed!?");
	}
	bDownloadNeeded = 1;
	downloadStatus = FTS_INITIALIZING;
}
//--------------------------------------------------------------------------//
//
void Mission::CancelNetDownload (void)
{
	if (downloadChannel && FILETRANSFER)
		FILETRANSFER->CloseChannel(downloadChannel);
	downloadStatus = FTS_INVALIDCHANNEL;
	downloadChannel = 0;
	bDownloadNeeded = 0;
}	
//--------------------------------------------------------------------------//
//
enum FTSTATUS Mission::GetDownloadStatus (void)
{
	return downloadStatus;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::close (void)
{
	BOOL32 result = 0;

	// todo(aaj-5/6/2004): big hack to get the first splash screen up first
	if (bDeferredMissionEndNonFail)
	{
#if !defined(_WIN32)
		// The native game renderer does not yet own the mission-ending slide.
		return 0;
#else
		// do the loading
		COMPTR<IShapeLoader> loader;
		COMPTR<IDAComponent> pComp;
		COMPTR<IDrawAgent> slide;

		GENDATA->CreateInstance(CQ_SplashInfo.vfxName, pComp);
		pComp->QueryInterface("IShapeLoader", loader);

		if( loader->CreateDrawAgent(0,slide) == GR_OK )
		{
			// start the drawing
			PIPE->clear_buffers(RP_CLEAR_COLOR_BIT|RP_CLEAR_DEPTH_BIT,0);
			PIPE->begin_scene();

			// do the drawing
			if(slide)
			{
				PANE src;
				src.window = NULL; // VFX_WINDOW*
				PIPE->get_viewport( (int*)&src.x0, (int*)&src.y0, (int*)&src.x1, (int*)&src.y1 );

				U16 width, height;
				slide->GetDimensions(width,height);

				PANE dst;
				dst.window = NULL; // VFX_WINDOW*
				dst.x0 = 0;
				dst.y0 = 0;
				dst.x1 = width;
				dst.y1 = height;

				slide->Draw( &src, &dst );
			}

			// end the drawing
			PIPE->end_scene();
			PIPE->swap_buffers();
		}

		// do the cleanup
		loader.free();
		slide.free();
#endif
	}

#if !defined(_WIN32)
	// Without a native save prompt, refuse to discard an unsaved mission.
	if (bUnsavedData) return 0;
#else
	if (bUnsavedData && CQFLAGS.bNoGDI==0)
	{
		switch (MessageBox(hMainWindow, "Save mission data?", "Unsaved Data", MB_YESNOCANCEL|MB_ICONEXCLAMATION))
		{
		case IDYES:
			if (Save() == 0)
				return close();
			break;
		case IDCANCEL:
			goto Done;
		}
	}
#endif
	
	EVENTSYS->Send(CQE_MISSION_CLOSE, 0);

	inFile.free();
	SetWindowTitle();
#if defined(_WIN32)
	MENU->EnableMenuItem(IDM_CLOSE_MISSION, MF_BYCOMMAND | MF_GRAYED); 		
#endif

	destroyPlayerAI();
	SECTOR->Close();
	OBJLIST->Close();
	THEMATRIX->Close();
	MScript::Close();
	MGlobals::Close();

	bUnsavedData=0;
	endCheckedCount = 0;
	deferredMissionWinners = 0;
	deferredMissionEndTime = 0;
	bOnlyHumanPlayerMsgNeeded = 0;
	bDeferredMissionEndNonFail = false;

	memcpy(COLORTABLE, DEFCOLORTABLE, sizeof(COLORTABLE));		// restore the color table

	EVENTSYS->Send(CQE_MISSION_CLOSE, (void *)1);

	unloadInterface();

	result=1;

Done:
	return result;
}
//--------------------------------------------------------------------------//
//
void Mission::Close (BOOL32 bForceIt)
{
	if (bForceIt)
		bUnsavedData = 0;
	close();
}
//--------------------------------------------------------------------------//
//
void Mission::reloadGametypes (void)
{
	if (PLAYERID==0 && CQFLAGS.bGameActive)		// can't do this during a network session
	{
		const USER_DEFAULTS * const defaults = DEFAULTS->GetDefaults();
		if (defaults->bNoAutoSave)
			bUnsavedData = false;
		if (bUnsavedData && Save()==0)
			return;
		COMPTR<IFileSystem> file = inFile;
		if (close())
		{
			OBJLIST->DEBUG_reloadDatabase();
			inFile = file;
			if (file)
				Reload();
			else
				New();
		}
	}
	else
		CQERROR0("Can't reload database right now.");
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
BOOL32 Mission::LoadMission (U32 id)
{
	char buffer[MAX_PATH+4];
	BOOL32 result=0;

	if (DEFAULTS->GetNameInMRU(buffer, IDS_MISSION_FILTER, id-IDM_MRU0_MISSION))
	{
		DEFAULTS->RemoveNameFromMRU(IDS_MISSION_FILTER, id-IDM_MRU0_MISSION);

		if ((result = Load(buffer)) != 0)
		{
			DEFAULTS->InsertNameIntoMRU(buffer, IDS_MISSION_FILTER);
		}
		else
			New();
	}

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::New (void)
{
	BOOL32 result;

	if ((result = close()) != 0)
	{
		MGlobals::New();
		//warning, stupid code follows.  I didn't write it.  -rmarr
		result = SECTOR->New();
		result = OBJLIST->New();
		result = THEMATRIX->New();
		result = LIGHTS->New();
		result = FOGOFWAR->New();

		LoadInterface();

		EVENTSYS->Send(CQE_MISSION_LOAD_COMPLETE, 0);
	}

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Mission::SaveParseData (void)
{
	BOOL32 result = 0;
	COMPTR<IFileSystem> file;
	DAFILEDESC fdesc = "Data.h";
	DWORD len, dwWritten;

	inFile->CreateDirectory("\\ParseData");
	if (inFile->SetCurrentDirectory("\\ParseData") == 0)
		goto Done;

	fdesc.lpImplementation = "DOS";
	fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
	fdesc.dwShareMode = 0;  // no sharing
	fdesc.dwCreationDistribution = CREATE_ALWAYS;
	if (inFile->CreateInstance(&fdesc, file) != GR_OK)
		goto Done;

	char * ptr2;
	{
		void * pPreprocessBlock;
		int length=0;

		if ((pPreprocessBlock = ::GetPreprocessData(length)) == 0)
			goto Done;

		ptr2 = (char *) malloc(length + 1);
		memcpy(ptr2, pPreprocessBlock, length);
		ptr2[length] = 0;
		len = length;
	}
	
	file->WriteFile(0, ptr2, len+1, &dwWritten, 0);
	::free(ptr2);

	inFile->SetCurrentDirectory("..");
	result = 1;
Done:
	return result;
}
//--------------------------------------------------------------------------
//
BOOL32 Mission::LoadParseData (IFileSystem * inFile)
{
#ifndef _WIN32
	if (hPrevSymbols)
		parser->DestroySymbols(hPrevSymbols);
	hPrevSymbols = 0;
	return FrontierX::LoadNativeMissionSchema(inFile, parser, hPrevSymbols);
#else
	BOOL32 result = 0;
	DAFILEDESC fdesc = "\\ParseData\\Data.h";
	DWORD len, dwRead;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	C8 * pTemp = 0;

	if (hPrevSymbols)
		parser->DestroySymbols(hPrevSymbols);
	hPrevSymbols = 0;
	if ((hFile = inFile->OpenChild(&fdesc)) == INVALID_HANDLE_VALUE)
		goto Done;
	len = inFile->GetFileSize(hFile);
	if (len == 0 || len == DWORD(-1))
		goto Done;
#ifndef _WIN32
	if (len > 4U * 1024U * 1024U)
		goto Done;
#endif
	if ((pTemp = (C8 *) malloc (len+1)) == 0)
		goto Done;
	if (inFile->ReadFile(hFile, (void *)pTemp, len, &dwRead, 0) == 0 || dwRead != len)
		goto Done;
	pTemp[len] = 0;

	hPrevSymbols = parser->ParseNewMemory(pTemp);

	result = (hPrevSymbols != 0);
Done:
	if (hFile != INVALID_HANDLE_VALUE)
		inFile->CloseHandle(hFile);
	::free(pTemp);

	return result;
#endif
}
//--------------------------------------------------------------------------
//
void Mission::updateNetFile (void)
{
	if (downloadChannel || (bDownloadNeeded && HOSTID && HOSTID != PLAYERID))
	{
#if !defined(_WIN32)
		RequireNativeMissionFileTransfer();
#endif
		if (bDownloadNeeded)
		{
			if (downloadChannel)		// remote download of mission file
				FILETRANSFER->CloseChannel(downloadChannel);
			downloadChannel = FILETRANSFER->GetFile(HOSTID, "??MISSION");
		}

		if (downloadChannel)
		{
			U32 bytesReceived=0, totalBytes=0;
			COMPTR<IFileSystem> file;

			bDownloadNeeded = 0;
			FILETRANSFER->GetTransferProgress(downloadChannel, &bytesReceived, &totalBytes);

			switch (downloadStatus = FILETRANSFER->CreateFile(downloadChannel, file))
			{
			case FTS_INITIALIZING:
			case FTS_INPROGRESS:
				break;		// do nothing, connection still pending

			case FTS_SUCCESS:
				FILETRANSFER->CloseChannel(downloadChannel);
				downloadChannel = 0;
				bUnsavedData = 0;
				close();
				inFile = file;

				LoadParseData(inFile);
				MGlobals::Load(inFile);
				SECTOR->Load(inFile);
				SYSMAP->Load(inFile);
				CAMERAMANAGER->Load(inFile);
				NUGGETMANAGER->Load(inFile);
				baseObjPercent=0;
				totalObjPercent=0;
				OBJLIST->Load(inFile);
				THEMATRIX->Load(inFile);
				FOGOFWAR->Load(inFile);
				MUSICMANAGER->Load(inFile);
				LIGHTS->Load(inFile);
				MScript::Load(inFile);
				OBJLIST->ResolveAssociations();
				MUSICMANAGER->LoadFinish();
				EVENTSYS->Send(CQE_MISSION_LOAD_COMPLETE, 0);

				SetWindowTitle();
				inFile.free();
				bUnsavedData = 0;
				break;

			default:	// else some sort of error occurred
				switch (downloadStatus)
				{
				case FTS_INVALIDFILE:
					CQERROR0("Remote side has nothing loaded.");
					break;
				case FTS_INVALIDCHANNEL:
					if (PLAYERID)
						CQERROR0("An error occurred in data file transfer!");
					break;
				case FTS_CLOSED:				// remote side closed the connection
					CQERROR0("FTP Channel closed by host machine.");
					break;
				}

				FILETRANSFER->CloseChannel(downloadChannel);
				downloadChannel = 0;
				break;
			} // end switch
		}
	}
}
//--------------------------------------------------------------------------
//
GENRESULT Mission::GetFileSystem (struct IFileSystem **ppFile)
{
	if ((*ppFile = inFile) != 0)
	{
		inFile->AddRef();
		return GR_OK;
	}
	return GR_GENERIC;
}
//--------------------------------------------------------------------------
//
void Mission::SetFileSystem (struct IFileSystem * pFile)
{
	inFile = pFile;
}
//--------------------------------------------------------------------------
//
BOOL32 Mission::ReloadMission (void)
{
	// must be in single player mode to call this.
	U32 uMissionID = MGlobals::GetMissionID();

	char fileName[32];
	memset(fileName, 0, sizeof(fileName));

	if (uMissionID == 0)
	{
		// either were are about to reload mission 1 or one of the training missions
		enum M_RACE race = MGlobals::GetPlayerRace(MGlobals::GetThisPlayer());
		if (race == M_TERRAN)
		{
			// reload mission 1
			strcpy(fileName, "T_Mission1");
		}
		else if (race == M_MANTIS)
		{
			// reload the mantis training mission
			strcpy(fileName, "Mantis_Train");
		}
		else if (race == M_SOLARIAN)
		{
			// reload the solarian training mission
			strcpy(fileName, "Sol_Train");
		}
	}
	else
	{
		// loading on of the missions 2 - 16
		#ifdef _WIN32
		wsprintf(fileName, "T_Mission%d", uMissionID+1); 
		#else
		const int written = snprintf(fileName, sizeof(fileName), "T_Mission%u", uMissionID+1);
		if (written < 0 || written >= S32(sizeof(fileName)))
			return FALSE;
		#endif
	}
	#ifndef _WIN32
	if (fileName[0] == 0 || !SPMAPDIR)
		return FALSE;
	#endif

	char fileNameEx[32];
	memset(fileNameEx, 0, sizeof(fileNameEx));

	// okay, first see if we can load a *.dmission file
	// if that fails, load a *.qmission file
	#ifdef _WIN32
	wsprintf(fileNameEx, "%s%s", fileName, ".dmission");
	#else
	const int missionNameSize = snprintf(fileNameEx, sizeof(fileNameEx), "%s.dmission", fileName);
	if (missionNameSize < 0 || missionNameSize >= S32(sizeof(fileNameEx)))
		return FALSE;
	#endif

	DAFILEDESC fdesc = fileNameEx;
	if (SPMAPDIR->CreateInstance(&fdesc, inFile) != GR_OK)
	{
		// we couldn't find the dmission, load the qmission instead
		memset(fileNameEx, 0, sizeof(fileNameEx));
		#ifdef _WIN32
		wsprintf(fileNameEx, "%s%s", fileName, ".qmission");
		#else
		const int fallbackNameSize = snprintf(fileNameEx, sizeof(fileNameEx), "%s.qmission", fileName);
		if (fallbackNameSize < 0 || fallbackNameSize >= S32(sizeof(fileNameEx)))
			return FALSE;
		#endif

		if (SPMAPDIR->CreateInstance(&fdesc, inFile) != GR_OK)
		{
			CQBOMB0("Could not restart the mission");
			return FALSE;
		}
	}

	return Reload();
}
//--------------------------------------------------------------------------
//
BOOL32 Mission::Reload (void)
{
	COMPTR<IFileSystem> temp = inFile;
	BOOL32 result = 0;
	bool bMouseBusy = false;
#if !defined(_WIN32)
	const bool wasGameActive = CQFLAGS.bGameActive != 0;
	bool reloadStarted = false;
#endif

	MIS_LOG("Reload: entered\n");
	if (close() == 0)
		{ MIS_LOG("Reload: close() failed\n"); goto Done; }
#if !defined(_WIN32)
	reloadStarted = true;
#endif

	CURSOR->SetBusy(1);
	bMouseBusy = 1;

	inFile = temp;		// restore original file
	CQASSERT(inFile != 0);

	if (DEFAULTS->GetDefaults()->bSpectatorModeOn)
	{
		DEFAULTS->GetDefaults()->bSpectatorModeOn = false;
		DEFAULTS->GetDefaults()->bVisibilityRulesOff = false;
	}

	// don't create the progress animation if its already around
	MIS_LOG("Reload: before CreateInProgressAnim\n");
	if (ipAnim == 0)
	{
		CreateInProgressAnim(ipAnim);
	}
	MIS_LOG("Reload: after CreateInProgressAnim\n");

	ipAnim->UpdateString(IDS_PROG_INITMAP);
	MIS_LOG("Reload: before LoadParseData\n");
	if (LoadParseData(inFile) == 0)
		goto Done;
	ipAnim->SetProgress(0.05f);
	MIS_LOG("Reload: before MGlobals::Load\n");
	if (MGlobals::Load(inFile) == 0)
		goto Done;
	ipAnim->SetProgress(0.08f);

	if (PLAYERID==0 && MGlobals::IsSinglePlayer())		// we may want to store this data in the map!
	{
		DEFAULTS->GetDefaults()->fogMode = FOGOWAR_NORMAL;
	}
		
	if (MGlobals::GetGameSettings().version && MGlobals::GetGameSettings().version != GetBuildVersion())
	{
#ifdef _DEBUG
		CQERROR0("Mission file is from an incompatible version. I'll let it go, this time...");
#else
		CQERROR0("Mission file is from an incompatible version. DO NOT LOG BUGS FOR THIS SESSION!");
#endif
	}
	
	//detect if it is a dynamic load
	char buffer[MAX_PATH+4];
	MGlobals::GetTerrainFilename(buffer,sizeof(buffer));

	MIS_LOGF("Reload: terrain buffer='%s'\n", buffer);
	if (buffer[0])
	{
		COMPTR<IFileSystem> terrainFile;

		DAFILEDESC fdesc = buffer;

		fdesc.lpImplementation = "UTF";

		MIS_LOG("Reload: opening terrain file\n");
		if (SPMAPDIR->CreateInstance(&fdesc, terrainFile) == GR_OK)
		{
			HANDLE _hPrevSymbols = hPrevSymbols;		// save old parse info
			hPrevSymbols = NULL;

			MIS_LOG("Reload: terrain LoadParseData\n");
			LoadParseData(terrainFile);

			ipAnim->SetProgress(0.15f);
			MIS_LOG("Reload: SECTOR->Load\n");
			SECTOR->Load(terrainFile);
			MIS_LOG("Reload: SYSMAP->Load\n");
#if defined(_WIN32)
			SYSMAP->Load(terrainFile);
#else
			if (!NativeLoadMissionSystemMap(terrainFile)) goto Done;
#endif

			ipAnim->SetProgress(0.20f);
			ipAnim->UpdateString(IDS_PROG_CAMERA);
			MIS_LOG("Reload: CAMERAMANAGER->Load\n");
			CAMERAMANAGER->Load(terrainFile);

			ipAnim->SetProgress(0.22f);
			ipAnim->UpdateString(IDS_PROG_OBJECTS);
			MIS_LOG("Reload: NUGGETMANAGER->Load\n");
			NUGGETMANAGER->Load(terrainFile);

			ipAnim->SetProgress(0.25f);
			baseObjPercent=ipAnim->GetProgress();
			totalObjPercent=0.40-baseObjPercent;
			MIS_LOG("Reload: OBJLIST->Load(terrainFile)\n");
#if defined(_WIN32)
			OBJLIST->Load(terrainFile,true);
#else
			if (!OBJLIST->Load(terrainFile,true)) goto Done;
#endif
			MIS_LOG("Reload: OBJLIST->Load done\n");

			ipAnim->SetProgress(0.40f);
			ipAnim->UpdateString(IDS_PROG_MATRIX);
			MIS_LOG("Reload: THEMATRIX->Load\n");
			THEMATRIX->Load(terrainFile);

			ipAnim->SetProgress(0.43f);
			MIS_LOG("Reload: FOGOFWAR->Load\n");
			FOGOFWAR->Load(terrainFile);

			ipAnim->SetProgress(0.47f);
			MIS_LOG("Reload: MUSICMANAGER->Load\n");
			MUSICMANAGER->Load(terrainFile);

			ipAnim->SetProgress(0.50f);
			MIS_LOG("Reload: LIGHTS->Load\n");
			LIGHTS->Load(terrainFile);

			ipAnim->SetProgress(0.53f);
			if (hPrevSymbols)
			{
				parser->DestroySymbols(hPrevSymbols);
			}
			hPrevSymbols = _hPrevSymbols;
		}

#if !defined(_WIN32)
		// Terrain supplies the initial map; the dynamic checkpoint owns saved alerts.
		if (!NativeLoadMissionSystemMap(inFile)) goto Done;
#endif

		ipAnim->UpdateString(IDS_PROG_OBJECTS);
		baseObjPercent=ipAnim->GetProgress();
		totalObjPercent=0.80 - baseObjPercent;
		MIS_LOG("Reload: OBJLIST->Load(inFile)\n");
#if defined(_WIN32)
		OBJLIST->Load(inFile);
#else
		if (!OBJLIST->Load(inFile)) goto Done;
#endif

		ipAnim->SetProgress(0.80f);
		ipAnim->UpdateString(IDS_PROG_SCRIPTS);
		MIS_LOG("Reload: MScript::Load\n");
#if defined(_WIN32)
		MScript::Load(NULL);
#else
		if (!LoadNativeMissionScripts(NULL)) goto Done;
#endif		// should not have any save/load info here anyway
		ipAnim->SetProgress(0.90f);
	}
	else
	{
		// initialize the load
		MIS_LOG("Reload: else branch - SECTOR->Load\n");
		SECTOR->Load(inFile);
		MIS_LOG("Reload: SYSMAP->Load\n");
#if defined(_WIN32)
		SYSMAP->Load(inFile);
#else
		if (!NativeLoadMissionSystemMap(inFile)) goto Done;
#endif
		ipAnim->SetProgress(0.10f);

		// load the artificial intelligence
		ipAnim->UpdateString(IDS_PROG_CREATEAI);
		MIS_LOG("Reload: loadPlayerAI\n");
#if defined(_WIN32)
		loadPlayerAI(inFile);
#else
		if (!loadPlayerAI(inFile)) goto Done;
#endif
		ipAnim->SetProgress(0.15f);

		ipAnim->UpdateString(IDS_PROG_CAMERA);
		CAMERAMANAGER->Load(inFile);
		ipAnim->SetProgress(0.20f);

		ipAnim->UpdateString(IDS_PROG_OBJECTS);
		NUGGETMANAGER->Load(inFile);
		ipAnim->SetProgress(0.25f);

		baseObjPercent=ipAnim->GetProgress();
		totalObjPercent=0.80 - baseObjPercent;
#if defined(_WIN32)
		OBJLIST->Load(inFile);
#else
		if (!OBJLIST->Load(inFile)) goto Done;
#endif
		ipAnim->SetProgress(0.80f);

		ipAnim->UpdateString(IDS_PROG_MATRIX);
		THEMATRIX->Load(inFile);
		ipAnim->SetProgress(0.82f);

		FOGOFWAR->Load(inFile);
		ipAnim->SetProgress(0.84f);

		MUSICMANAGER->Load(inFile);
		ipAnim->SetProgress(0.86f);

		LIGHTS->Load(inFile);
		ipAnim->SetProgress(0.88f);

		ipAnim->UpdateString(IDS_PROG_SCRIPTS);
#if defined(_WIN32)
		MScript::Load(inFile);
#else
		if (!LoadNativeMissionScripts(inFile)) goto Done;
#endif
		ipAnim->SetProgress(0.90f);
	}

	OBJLIST->ResolveAssociations();
	MUSICMANAGER->LoadFinish();
	EVENTSYS->Send(CQE_MISSION_LOAD_COMPLETE, 0);

	SetWindowTitle();
	bUnsavedData=0;
	
	ipAnim->UpdateString(IDS_PROG_LOADINTERFACE);
	if (PLAYERID==0 && MGlobals::IsSinglePlayer())
	{
		LoadInterface();
		DEFAULTS->GetStringFromRegistry(NAME_REG_KEY, buffer, sizeof(buffer));
		MGlobals::AssignThisPlayer(buffer);
	}
//	else if (CQFLAGS.bGameActive)
else // AAJ - the proper way fix this is to have IsSinglePlayer() report correctly for Non-Mission DLL scripted missions
	{
		LoadInterface();
	}

	ipAnim->SetProgress(1.00f);

	if (hPrevSymbols)
	{
		parser->DestroySymbols(hPrevSymbols);
		hPrevSymbols = 0;
	}

	result = 1;

	if (CQFLAGS.bGameActive)
	{
		EVENTSYS->Send(CQE_GAME_ACTIVE, (void *)1);
	}

Done:
	if (bMouseBusy)
		CURSOR->SetBusy(0);

	// do not free the animation if we are going to call initnetgame
	if (ipAnim != 0 && bNetloadProgressAnimation == false)
	{
		ipAnim->SetProgress(1.0f);
		ipAnim.free();
		ipAnim = NULL;
	}
#if !defined(_WIN32)
	if (!result && reloadStarted)
	{
		// A rejected object record may already have restored live actors, while
		// their operations/scripts have not loaded. Never resume that partial graph.
		std::fputs("Native mission reload failed; closing partial mission\n", stderr);
		CQFLAGS.bGameActive = 0;
		EVENTSYS->Send(CQE_GAME_ACTIVE, nullptr);
		Close(TRUE);
		if (wasGameActive)
		{
			EVENTSYS->Send(CQE_DLG_RESULT, (void *)MISSION_END_QUIT);
			EVENTSYS->Send(CQE_MISSION_ENDING, (void *)MISSION_END_QUIT);
		}
	}
#endif
	return result;
}
//--------------------------------------------------------------------------
//
BOOL32 Mission::LoadBriefing (const char * fileName)
{
	BOOL32 result = 0;
	DAFILEDESC fdesc = fileName;

	MIS_LOGF("LoadBriefing: %s\n", fileName ? fileName : "(null)");
	if (close() == 0)
		{ MIS_LOG("LoadBriefing: close() failed\n"); goto Done; }

	if (SPMAPDIR->CreateInstance(&fdesc, inFile) != GR_OK)
		{ MIS_LOG("LoadBriefing: CreateInstance failed\n"); goto Done; }
	MIS_LOG("LoadBriefing: file opened\n");

	LoadParseData(inFile);
	MIS_LOG("LoadBriefing: ParseData done\n");
	MGlobals::Load(inFile);
	MIS_LOG("LoadBriefing: MGlobals done\n");
	if (PLAYERID==0 && MGlobals::IsSinglePlayer())		// we may want to store this data in the map!
		DEFAULTS->GetDefaults()->fogMode = FOGOWAR_NORMAL;

	if (MGlobals::GetGameSettings().version && MGlobals::GetGameSettings().version != GetBuildVersion())
	{
#ifdef _DEBUG
		CQERROR0("Mission file is from an incompatible version. I'll let it go, this time...");
#else
		CQERROR0("Mission file is from an incompatible version. DO NOT LOG BUGS FOR THIS SESSION!");
#endif
	}

	MScript::LoadScript();
	MIS_LOG("LoadBriefing: LoadScript done\n");
	SetWindowTitle();
	bUnsavedData=0;
	result = 1;

Done:
	MIS_LOGF("LoadBriefing: done result=%d\n", result);
	return result;
}
//--------------------------------------------------------------------------
//
void Mission::InitializeNetGame (const struct FULLCQGAME & cqgame, U32 randomSeed)
{
	// if this gets called you know you are playing a quickbattle or multiplayer game
	// the animation is already going
	CQASSERT(ipAnim);

	ipAnim->UpdateString(IDS_PROG_ASSIGNPLAYERS);
	MGlobals::AssignPlayers(cqgame, randomSeed);
	ipAnim->SetProgress(0.8f);

	ipAnim->UpdateString(IDS_PROG_LOADINTERFACE);
	LoadInterface();
	
	//
	// look at this player's ships
	//
	ipAnim->SetProgress(0.9f);
	ipAnim->UpdateString(IDS_PROG_INIT3D);
	IBaseObject * obj = OBJLIST->GetTargetList(), *end=0;
	const U32 playerID = MGlobals::GetThisPlayer();

	while (obj)
	{
		if (obj->GetPlayerID() == playerID)
			break;
		obj = obj->nextTarget;
	}

	// set camera to look at obj
	if (obj)
	{
		SECTOR->SetCurrentSystem(obj->GetSystemID());
		CAMERA->SetLookAtPosition(obj->GetTransform().translation);
	}

	explodePlayerBombs();

	ipAnim->SetProgress(0.95f);
	
	//
	// recalculate visibility
	//
	obj=OBJLIST->GetObjectList();

	while (obj)
	{
		obj->CastVisibleArea();
		end = obj;
		obj = obj->next;
	}

	obj = end;
	while (obj)
	{
		obj->UpdateVisibilityFlags();
		obj = obj->prev;
	}

	FOGOFWAR->Update();

	ipAnim->SetProgress(0.98f);
	ipAnim->UpdateString(IDS_PROG_STARTAI);
	startPlayerAI();

	//
	// initialize money
	//
	if (MGlobals::GetUpdateCount() == 0)
	{
		U32 gas = 0;
		U32 metal = 0;
		U32 crew = 0;
		U32 maxControlPoints = 100;
#ifdef _DEMO
		if (cqgame.commandLimit == COMMAND_LOW)
			maxControlPoints = 50;
		else
		if (cqgame.commandLimit == COMMAND_MID)
			maxControlPoints = 150;
		else
		if (cqgame.commandLimit == COMMAND_HIGH)
			maxControlPoints = 200;
#else
		if (cqgame.commandLimit == COMMAND_NORMAL)
			maxControlPoints = 150;
		else
		if (cqgame.commandLimit == COMMAND_MID)
			maxControlPoints = 200;
		else
		if (cqgame.commandLimit == COMMAND_HIGH)
			maxControlPoints = 300;		
#endif

		if(cqgame.money == LOW_MONEY)
		{
			gas = atoi(_localLoadString(IDS_GAS_LOW));
			metal = atoi(_localLoadString(IDS_METAL_LOW));
			crew = atoi(_localLoadString(IDS_CREW_LOW));
		}
		else if(cqgame.money == MEDIUM_MONEY)
		{
			gas = atoi(_localLoadString(IDS_GAS_MEDIUM));
			metal = atoi(_localLoadString(IDS_METAL_MEDIUM));
			crew = atoi(_localLoadString(IDS_CREW_MEDIUM));
		}
		else if(cqgame.money == HIGH_MONEY)
		{
			gas = atoi(_localLoadString(IDS_GAS_HIGH));
			metal = atoi(_localLoadString(IDS_METAL_HIGH));
			crew = atoi(_localLoadString(IDS_CREW_HIGH));
		}
		
		for (U32 i = 1; i <= MAX_PLAYERS; i++)
		{
			MGlobals::SetCurrentCrew(i,crew);
			MGlobals::SetCurrentGas(i,gas);
			MGlobals::SetCurrentMetal(i,metal);
			MGlobals::SetMaxControlPoints(i, maxControlPoints);
		}
	}

	THEMATRIX->ResetSyncItems();

	if (cqgame.mapType == RANDOM_MAP)
	{
		MUSICMANAGER->InitPlaylist(MGlobals::GetPlayerRace(playerID));
		EVENTSYS->Send(CQE_MISSION_LOAD_COMPLETE, 0);
	}

	// destroy what we need to
	ipAnim.free();
	ipAnim = NULL;
}
//--------------------------------------------------------------------------
//
void Mission::GenerateMultiplayerMap (const struct FULLCQGAME & cqgame, U32 randomSeed)
{
	// get ready for the load animation...
	if (ipAnim==0)
		CreateInProgressAnim(ipAnim);

	bUnsavedData = 0;
	close();
	MGlobals::New();		// doesn't have a close() method

	CAMERA->SetCameraToDefaults();		// always use default values for camera
	OBJLIST->New();
	THEMATRIX->New();
	LIGHTS->New();
	FOGOFWAR->New();
	
	// generate the map
	MAPGEN->GenerateMap(cqgame, randomSeed, ipAnim);
}
//--------------------------------------------------------------------------
// start up a menu that says -"hey you lost" and it has an okay button to let you continue
// DO NOT CALL THIS IN RESPONSE TO A NETWORK PACKET OR YOU WILL STALL OTHER PLAYERS!
// if top bit of allyMask is set, local player has lost, but game is to continue....
//
void Mission::EndMission (U32 allyMask, bool bNonWinLose)
{
#if defined(_WIN32)
	if (ZONESCORE && ((allyMask & 0x80000000) == 0))
	{
		const CQGAME & cqgame = MGlobals::GetGameSettings();
		U32 i;

		for (i = 0; i < cqgame.activeSlots; i++)
		{
			U32 playerID = MGlobals::GetPlayerIDFromSlot(i);

			if (playerID && playerID <= MAX_PLAYERS)
			{
				if ((allyMask & (1 << (playerID-1))) != 0)		// is player among the winners
				{
					U32 flags = ZONESCORE_WINNER;
					if (cqgame.slot[i].dpid==0)
					{
						if (MGlobals::HasPlayerResigned(i))
							flags |= ZONESCORE_RESIGNED;
						else
							flags |= ZONESCORE_COMPUTERPLAYER;
					}
					ZONESCORE->SetPlayerFlags(MGlobals::GetZoneSeatFromSlot(i), flags);
				}
			}
		}

		ZONESCORE->SendGameState(DPLOBBY, ZSTATE_END);
		ZONESCORE->SendFinalScore(DPLOBBY);
	}
#endif

	bool bWon = ((allyMask & (1 << (MGlobals::GetThisPlayer()-1))) != 0);		// are we allied with winners?
	U32 result = bWon ? MISSION_END_WON : MISSION_END_LOST;

	if (bNonWinLose)
	{
		result = MISSION_END_SPLASH;
	}
	
	// if recursing, this will close the previous dialog message!
	EVENTSYS->Send(CQE_DLG_RESULT, (void *)result);			// go back to shell
	EVENTSYS->Send(CQE_MISSION_ENDING, (void *)result);		// send result code

	if (THEMATRIX->IsMaster())
	{
		THEMATRIX->FlushSyncData();	// called by Host before shutdown of net session
		if (((allyMask & 0x80000000) == 0))		// is the game really ending!?
		{
			ENDGAME_PACKET packet;
			packet.allyMask = allyMask;
			NETPACKET->Send(0, NETF_ALLREMOTE, &packet);
		}
	}

	BOOL bOldGamePaused = CQFLAGS.bGamePaused;
	// only do the pause thing if the game is really ending!
	// otherwise the clients will be paused for no good reason
	if (bOldGamePaused==0 && ((allyMask & 0x80000000) == 0))
	{
		CQFLAGS.bGamePaused = TRUE;
		EVENTSYS->Send(CQE_PAUSE_CHANGE, (void *)(CQFLAGS.bGamePaused));
	}

	if (result == MISSION_END_WON)
	{
		CQMessageBox(IDS_MISSION_PRESSOK, IDS_MISSION_SUCCESS, MB_OK);
	}
	else if(result == MISSION_END_LOST)
	{
		CQMessageBox(IDS_MISSION_PRESSOK, IDS_MISSION_FAILURE, MB_OK);
	}

	if (bOldGamePaused==0 && ((allyMask & 0x80000000) == 0))
	{
		CQFLAGS.bGamePaused = FALSE;
		EVENTSYS->Send(CQE_PAUSE_CHANGE, (void *)(CQFLAGS.bGamePaused));
	}
}
//--------------------------------------------------------------------------
//
U32 Mission::GetNumAIPlayers (void)
{
	U32 result = 0;
	int i;

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (playerAI[i] != 0)
			result++;
	}

	return result;
}

//--------------------------------------------------------------------------
// AI CONTROLLING METHODS FOR MISSION SCRIPTING
//--------------------------------------------------------------------------
//
bool Mission::IsComputerControlled (U32 playerID)
{
	CQASSERT(playerID && playerID <= MAX_PLAYERS);
	return (playerAI[playerID-1]);
}
//--------------------------------------------------------------------------
//
void Mission::EnableEnemyAI (U32 playerID, bool bOn, enum M_RACE race)
{
	CQASSERT(playerID && playerID <= MAX_PLAYERS);
	if (bOn)
	{  
		if (!playerAI[playerID-1])
			createPlayerAI(race, playerID, playerAI[playerID-1]);
		
		playerAI[playerID-1]->Activate(true);
	}
	else
	{
		if (playerAI[playerID-1])
			playerAI[playerID-1]->Activate(false);
	}
}
//--------------------------------------------------------------------------
//
void Mission::EnableEnemyAI (U32 playerID, bool bOn, const char * szPlayerAIType)
{
	CQASSERT(playerID && playerID <= MAX_PLAYERS);
	if (bOn)
	{  
		if (!playerAI[playerID-1])
			ISPlayerAI::CreateInstance(szPlayerAIType, playerID, playerAI[playerID-1]);
		
		playerAI[playerID-1]->Activate(true);
	}
	else
	{
		if (playerAI[playerID-1])
			playerAI[playerID-1]->Activate(false);
	}
}
//--------------------------------------------------------------------------
//
void Mission::SetEnemyAIRules (U32 playerID, const struct AIPersonality & rules)
{
	CQASSERT(playerID && playerID <= MAX_PLAYERS);
	if (playerAI[playerID-1])
	{
		playerAI[playerID-1]->SetPersonality(rules);
	}
}
//--------------------------------------------------------------------------
//
void Mission::SetEnemyAITarget (U32 playerID, IBaseObject *obj, U32 range, U32 systemID)
{
	CQASSERT(playerID && playerID <= MAX_PLAYERS);
	if (playerAI[playerID-1])
	{
		playerAI[playerID-1]->SetStrategicTarget(obj, range, systemID);
	}
}
//--------------------------------------------------------------------------
//
void Mission::SetFileDescription(const wchar_t * desc)
{
	MGlobals::SetFileDescription(desc);
}
//--------------------------------------------------------------------------
//
static void playerHasLost (U32 playerID)
{
	PLAYERLOST_PACKET packet;

	packet.playerID = playerID;
	packet.dwSize = sizeof(packet);

	NETPACKET->Send(0, NETF_ALLREMOTE|NETF_ECHO, &packet);
}
//--------------------------------------------------------------------------
//
static U32 getTeamRemaining (const CQGAME & cqgame)
{
	U32 result = 0;
	U32 i;

	for (i = 0; i < cqgame.activeSlots; i++)
	{
		if (cqgame.slot[i].state == READY && MGlobals::HasPlayerResigned(i)==false)
		{
			U32 playerID = MGlobals::GetPlayerIDFromSlot(i);
			result |= (1 << playerID);
		}
	}

	result >>= 1;

	return result;
}
//--------------------------------------------------------------------------
// 
void Mission::enableSpectatorMode (void)
{
	DEFAULTS->GetDefaults()->bSpectatorModeOn = true;
	DEFAULTS->GetDefaults()->bVisibilityRulesOff = true;
}
//--------------------------------------------------------------------------
// called periodically
//
void Mission::checkEndGame (void)
{
	const CQGAME & cqgame = MGlobals::GetGameSettings();
	if (cqgame.gameType != MISSION_DEFINED && DEFAULTS->GetDefaults()->bNoWinningConditions == 0 && THEMATRIX->IsMaster())
	{
		U32 playerMask = 0;
		MPart part = OBJLIST->GetTargetList();
		BOOL32 bEnd = false;
		U32 numUnits[MAX_PLAYERS+1];
		U32 numHQs[MAX_PLAYERS+1];
		U32 numPlats[MAX_PLAYERS+1];
		U32 numFabs[MAX_PLAYERS+1];
		VOLPTR(IPlatform) pPlatform;

		memset(numUnits, 0, sizeof(numUnits));
		memset(numHQs, 0, sizeof(numHQs));
		memset(numPlats, 0, sizeof(numPlats));
		memset(numFabs, 0, sizeof(numFabs));

		while (part.obj)
		{
			CQASSERT(part.isValid());

			if (part->bReady)
			{
				const U32 playerID = part->playerID;

				if (playerID && playerID <= MAX_PLAYERS)
				{
					if (part.obj->objClass != OC_PLATFORM || (pPlatform=part.obj)==0 || pPlatform->IsReallyDead()==0)
					{
						numUnits[playerID]++;

						if (MGlobals::IsHQ(part->mObjClass))
							numHQs[playerID]++;
						if (part.obj->objClass == OC_PLATFORM && MGlobals::IsGunPlat(part->mObjClass)==0)
							numPlats[playerID]++;
						if (MGlobals::IsFabricator(part->mObjClass))
							numFabs[playerID]++;
					}
				}
			}

			part = part.obj->nextTarget;
		}

		if (cqgame.gameType == KILL_UNITS)
		{
			U32 i;
			U32 losers=0;
			for (i = 0; i < cqgame.activeSlots; i++)
			{
				if (cqgame.slot[i].state == READY && MGlobals::HasPlayerResigned(i)==false)
				{
					U32 playerID = MGlobals::GetPlayerIDFromSlot(i);
					if (numUnits[playerID] == 0)
					{
						losers |= (1 << playerID);
						MGlobals::SetPlayerResignedBySlot(i);
					}
				}
			}

			losers>>=1;
			for (i = 1; i <= MAX_PLAYERS; i++, losers>>=1)
			{
				if (losers & 1)
				{
					playerHasLost(i);
				}
			}

			playerMask = getTeamRemaining(cqgame);
			if (playerMask)
			{
				for (i = 0; i < MAX_PLAYERS; i++)
				{
					if ((playerMask & (1 << i)) != 0)
					{
						if ((MGlobals::GetAllyMask(i+1) & playerMask) != playerMask)		// remaining players are not allied
							goto Done;	// keep playing
					}
				}
			}
		}
		else  // last platform
		if (cqgame.gameType == KILL_HQ_PLATS)
		{
			U32 i;
			U32 losers=0;
			for (i = 0; i < cqgame.activeSlots; i++)
			{
				if (cqgame.slot[i].state == READY && MGlobals::HasPlayerResigned(i)==false)
				{
					U32 playerID = MGlobals::GetPlayerIDFromSlot(i);
					if (numHQs[playerID] == 0)
					{
						losers |= (1 << playerID);
						MGlobals::SetPlayerResignedBySlot(i);
					}
				}
			}

			losers>>=1;
			for (i = 1; i <= MAX_PLAYERS; i++, losers>>=1)
			{
				if (losers & 1)
				{
					playerHasLost(i);
					THEMATRIX->TerminatePlayerUnits(i);
				}
			}

			playerMask = getTeamRemaining(cqgame);
			if (playerMask)
			{
				for (i = 0; i < MAX_PLAYERS; i++)
				{
					if ((playerMask & (1 << i)) != 0)
					{
						if ((MGlobals::GetAllyMask(i+1) & playerMask) != playerMask)		// remaining players are not allied
							goto Done;	// keep playing
					}
				}
			}
		}
		else  // last platform
		if (cqgame.gameType == KILL_PLATS_FABS)
		{
			U32 i;
			U32 losers=0;
			for (i = 0; i < cqgame.activeSlots; i++)
			{
				if (cqgame.slot[i].state == READY && MGlobals::HasPlayerResigned(i)==false)
				{
					U32 playerID = MGlobals::GetPlayerIDFromSlot(i);
					if (numPlats[playerID]+numFabs[playerID] == 0)
					{
						losers |= (1 << playerID);
						MGlobals::SetPlayerResignedBySlot(i);
					}
				}
			}

			losers>>=1;
			for (i = 1; i <= MAX_PLAYERS; i++, losers>>=1)
			{
				if (losers & 1)
				{
					playerHasLost(i);
					THEMATRIX->TerminatePlayerUnits(i);
				}
			}

			playerMask = getTeamRemaining(cqgame);
			if (playerMask)
			{
				for (i = 0; i < MAX_PLAYERS; i++)
				{
					if ((playerMask & (1 << i)) != 0)
					{
						if ((MGlobals::GetAllyMask(i+1) & playerMask) != playerMask)		// remaining players are not allied
							goto Done;	// keep playing
					}
				}
			}
		}

		bEnd = true;
Done:
		if (bEnd)
		{
			if (deferredMissionEndTime==0)
				deferredMissionEndTime = DEFERRED_MISSION_END_TIME;
			deferredMissionWinners = playerMask;
		}
	}
}
//--------------------------------------------------------------------------
//
void Mission::StartProgressAnim (struct IPANIM ** ppAnim)
{
	bNetloadProgressAnimation = true;
	if (ipAnim == 0)
	{
		CreateInProgressAnim(ipAnim);
	}
	ipAnim->AddRef();
	*ppAnim = ipAnim;
}
//--------------------------------------------------------------------------
//
const M_RACE Mission::GetSinglePlayerRace (void) const
{
	return MGlobals::GetPlayerRace(MGlobals::GetThisPlayer());
}
//--------------------------------------------------------------------------
//
void Mission::SetSinglePlayerRace (const enum M_RACE race)
{
	MGlobals::SetPlayerRace(MGlobals::GetThisPlayer(), race);
}
//--------------------------------------------------------------------------
//
const struct CQGAME & Mission::GetGameSettings (void)
{
	return MGlobals::GetGameSettings();
}
//--------------------------------------------------------------------------
//
void Mission::ToggleCheating (void)
{
	g_bCheatsOn = !g_bCheatsOn;

	if (g_bCheatsOn == false)
	{
		setDefaultCheatSettings();
	}
}
//--------------------------------------------------------------------------
//
const bool Mission::GetCheatsEnabled (void) const
{
	return g_bCheatsOn;
}
//--------------------------------------------------------------------------
//
bool Mission::IsMouseEnabled (void)
{
	return !MGlobals::GetScriptUIControl();
}
//--------------------------------------------------------------------------
//
void Mission::createPlayerAI (M_RACE race, U32 playerID, ISPlayerAI ** ppPlayerAI)
{
	ISPlayerAI::CreateInstance(race, playerID, ppPlayerAI);
}
//--------------------------------------------------------------------------
//
void Mission::LaunchOffensive (U32 playerID, UNIT_STANCE stance)
{
	CQASSERT(playerID && playerID <= MAX_PLAYERS);
	if (playerAI[playerID-1])
			playerAI[playerID-1]->LaunchOffensive(stance);
}
//--------------------------------------------------------------------------
//
COLORREF Mission::GetPlayerColorForDPID (U32 dpid)
{
	U32 playerID = MGlobals::GetPlayerIDFromDPID(dpid);
	COLORREF color = RGB(255,255,255);

	if (playerID && playerID <= MAX_PLAYERS)
		color = COLORTABLE[MGlobals::GetColorID(playerID)];

	return color;
}

//--------------------------------------------------------------------------
// points should add up to around 256
#define CALC_PLAT_POINTS 60
#define CALC_SHIP_POINTS 60
#define CALC_EXPLORED_POINTS 10
#define CALC_JUMPGATE_POINTS 100 
#define CALC_MONEY_POINTS 30

#define MAX_POSSIBLE_UNITS 50
#define MAX_POSSIBLE_MONEY 750

//--------------------------------------------------------------------------
//
void Mission::calculateCtrlO_Scores (void)
{
	U32 playerID;

	for (playerID = 1; playerID <= MAX_PLAYERS; playerID++)
	{
		if (MGlobals::IsPlayerInGame(playerID))
		{
			S32 total = 0;
			// add up plat units
			S32 score = MGlobals::GetNumPlatformsBuilt(playerID);
			score -= MGlobals::GetPlatformsLost(playerID);
			if (score < 0)
				score = 0;
			else
			if (score < MAX_POSSIBLE_UNITS)
				score = (score * CALC_PLAT_POINTS) / MAX_POSSIBLE_UNITS;
			else
				score = CALC_PLAT_POINTS;
			total += score;

			// add up plat units
			score = MGlobals::GetNumUnitsBuilt(playerID);
			score -= MGlobals::GetUnitsLost(playerID);
			if (score < 0)
				score = 0;
			else
			if (score < MAX_POSSIBLE_UNITS)
				score = (score * CALC_SHIP_POINTS) / MAX_POSSIBLE_UNITS;
			else
				score = CALC_SHIP_POINTS;

			total += score;
			
			// add up exploration
			score = (MGlobals::GetExploredSystemsRatio(playerID) * CALC_EXPLORED_POINTS);
			total += score;

			// add up jumpgate control for allies of player
			score = 0;
			{
				U32 allyMask = MGlobals::GetAllyMask(playerID) << 1;
				U32 i;
				for (i = 1; i <= MAX_PLAYERS; i++)
				{
					if ((allyMask & (1 << i)) != 0)
						score += MGlobals::GetNumJumpgatesControlled(i);
				}
			}
			if (score < SECTOR->GetNumSystems())
				score = (score * CALC_JUMPGATE_POINTS) / SECTOR->GetNumSystems();
			else
				score = CALC_JUMPGATE_POINTS;
			total += score;

			// add up money
			score = MGlobals::GetCurrentCrew(playerID);
			score += MGlobals::GetCurrentMetal(playerID);
			score += MGlobals::GetCurrentGas(playerID);

			if (score < MAX_POSSIBLE_MONEY)
				score = (score * CALC_MONEY_POINTS) / MAX_POSSIBLE_MONEY;
			else
				score = CALC_MONEY_POINTS;
			total += score;

			MGlobals::SetPlayerScore(playerID, total);
		}
	}
}
//--------------------------------------------------------------------------
//
void Mission::explodePlayerBombs (void)
{
	IBaseObject * obj = OBJLIST->GetTargetList();

	while (obj)
	{
		VOLPTR(IPlayerBomb) pBomb = obj;

		if (pBomb)
			pBomb->ExplodePlayerBomb();
		
		obj = obj->nextTarget;
	}

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//
static BOOL32 CreateParser (void)
{
	if (GS->QueryInterface("IViewConstructor", (void **)&PARSER) == GR_OK)
	{
		AddToGlobalCleanupList((IDAComponent **) &PARSER);
		return 1;
	}
	return 0;
}
//--------------------------------------------------------------------------//
//
static BOOL32 ParseDefFile (void)
{
	BOOL32 result=0;

	if (PARSER->QueryInterface("IViewConstructor2", (void **) &parser) == GR_OK)
	{
#if !defined(_WIN32)
		result = FrontierX::ParseNativeAppSchema(parser) ? 1 : 0;
#else
		void * pPreprocessBlock;
		char * ptr2;
		int len=0;

		if ((pPreprocessBlock = ::GetPreprocessData(len)) == 0)
			goto Done;

		ptr2 = (char *) malloc(len + 1);
		memcpy(ptr2, pPreprocessBlock, len);
		ptr2[len] = 0;

		result = (parser->ParseMemory(ptr2) == GR_OK);
		::free(ptr2);
#endif
	}
	else
		result = 0;

Done:
	return result;
}
extern "C" void _heap_init (void);
//--------------------------------------------------------------------------//
//
struct _mission : GlobalComponent
{
	Mission * mission;
#if defined(_WIN32)
	HINSTANCE hInstance;
#endif

	virtual void Startup (void)
	{
#if defined(_WIN32)
		_heap_init();
#endif

		if (!GS || CreateParser() == 0 || ParseDefFile() == 0)
		{
#if !defined(_WIN32)
			std::fputs("Native mission requires a working Data schema parser\n", stderr);
			std::abort();
#else
			CQBOMB0("Failed parse of the Data definition file: ..\\dinclude\\Data.h");
#endif
		}

		memcpy(COLORTABLE, DEFCOLORTABLE, sizeof(COLORTABLE));		// init the color table
		
		MISSION = mission = new DAComponent<Mission>;
		AddToGlobalCleanupList((IDAComponent **) &MISSION);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;
#if defined(_WIN32)
		DWORD dwLen;
		char buffer[260];
		
		dwLen = GetModuleFileName(hInstance, buffer, sizeof(buffer));
#endif

#if !defined(_WIN32)
		if (GS->QueryOutgoingInterface("ISystemEventCallback", connection) != GR_OK ||
			connection->Advise(MISSION, &mission->systemEventHandle) != GR_OK)
		{
			std::fputs("Native mission requires the system event callback\n", stderr);
			std::abort();
		}
#else
		if (GS->QueryOutgoingInterface("ISystemEventCallback", connection) == GR_OK)
			connection->Advise(MISSION, &mission->systemEventHandle);
#endif
#if !defined(_WIN32)
		// The offline mission has no remote transfer channel. Network missions
		// still require the production file-transfer service.
		if (PLAYERID || HOSTID) RequireNativeMissionFileTransfer();
		if (FILETRANSFER && FILETRANSFER->QueryOutgoingInterface("IFileTransferCallback", connection) == GR_OK)
			connection->Advise(MISSION, &mission->fileTransferHandle);
#else
		if (FILETRANSFER->QueryOutgoingInterface("IFileTransferCallback", connection) == GR_OK)
			connection->Advise(MISSION, &mission->fileTransferHandle);
#endif
#if defined(_WIN32)
		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
		{
			connection->Advise(MISSION, &mission->eventHandle);
			FULLSCREEN->SetCallbackPriority(mission, EVENT_PRIORITY_MISSION);
		}

		SendMessage(hMainWindow, CQE_DATA_CHECKSUM, 3, CreateZBufferAnalog(buffer));
#else
        if (FULLSCREEN)
        {
            if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
                connection->Advise(MISSION, &mission->eventHandle) != GR_OK)
                std::abort();
            FULLSCREEN->SetCallbackPriority(mission, EVENT_PRIORITY_MISSION);
        }
        else if (!EVENTSYS || EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
            connection->Advise(MISSION, &mission->eventHandle) != GR_OK)
		{
			std::fputs("Native mission requires the game event callback\n", stderr);
			std::abort();
		}
#endif
	}
};

static _mission mission;

#if defined(_WIN32)
void main (void)
{
}
//--------------------------------------------------------------------------
//  
static void SetDllHeapMsg (HINSTANCE hInstance)
{
	DWORD dwLen;
	char buffer[260];
	
	dwLen = GetModuleFileName(hInstance, buffer, sizeof(buffer));
	mission.hInstance = hInstance;
 
	while (dwLen > 0)
	{
		if (buffer[dwLen] == '\\')
		{
			dwLen++;
			break;
		}
		dwLen--;
	}

	SetDefaultHeapMsg(buffer+dwLen);
}
//--------------------------------------------------------------------------//
//
BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			HEAP = HEAP_Acquire();
			SetDllHeapMsg(hInstance);
		}
		break;
	}

	return 1;
}
#endif
//--------------------------------------------------------------------------//
//
void __stdcall MissionResetHeap (IHeap * heap)
{
	HEAP = heap;
}
#if !defined(_WIN32)
BOOL32 LoadNativeSelectedMission(const C8 *name, IComponentFactory *source)
{
	return MISSION && name && source && MISSION->Load(name, source);
}
#endif
//--------------------------------------------------------------------------//
//
//------------------------------------------------------------------------------------//
//---------------------------------------End Mission.cpp------------------------------//
//------------------------------------------------------------------------------------//
