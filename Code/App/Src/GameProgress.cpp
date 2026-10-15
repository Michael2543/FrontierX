//--------------------------------------------------------------------------//
//                                                                          //
//                      GameProgress.cpp                                    //
//                                                                          //
//               COPYRIGHT (C) 2000 BY DIGITAL ANVIL, INC.                  //
//																			//
//	NOTE:  Ignore stupid stl warnings                                       //
//--------------------------------------------------------------------------//
/*
   $Author: Sbarton $
*/
//--------------------------------------------------------------------------//
 
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "CQTrace.h"
#include <TComponent.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#endif

#include "IGameProgress.h"
#include "Startup.h"
#include "UserDefaults.h"

#include <Filesys.h>
#include <TSmartPointer.h>

#define NAME_REG_KEY	"CQPlayerName"
#define GPF_FILE		"player.gpf"

static bool g_bForcedIntroMovie = false;
static U32  g_tempMissionsCompleted = 0;

struct PROGRESS_DATA
{
	U32 missionsCompleted;
	U32 moviesSeen;
	U32 missionsSeen;
};
#if !defined(_WIN32)
static_assert(sizeof(PROGRESS_DATA) == 12, "Original progress file layout");
#endif

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE GameProgress : public IGameProgress
{
	BEGIN_DACOM_MAP_INBOUND(GameProgress)
	DACOM_INTERFACE_ENTRY(IGameProgress)
	END_DACOM_MAP()

	COMPTR<IFileSystem> fileSystem;
	PROGRESS_DATA progressData;
	char szPlayerName[128];

	GameProgress(void)
	{
		init();
	}

	~GameProgress(void);
	
    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IGameProgress methods */

	virtual U32  GetMissionsCompleted (void);

	virtual void SetMissionCompleted (U32 missionID);

	virtual void SetMissionsSeen (U32 missionID);

	virtual U32  GetMissionsSeen (void);

	virtual U32  GetMoviesSeen (void);

	virtual void SetMovieSeen (U32 movieID);

	virtual void ReInitialize (void);

	virtual void ForcedIntroMovie (void)
	{
		g_bForcedIntroMovie = true;
	}

	virtual void SetTempMissionsCompleted (U32 missionID);

	/* GameProgress methods */

	void init (void);

	bool loadGameProgress (void);

	bool saveGameProgress (void);

	bool setFileSystem (void);

	IDAComponent * GetBase (void)
	{
		return (IGameProgress*) this;
	}
};
//--------------------------------------------------------------------------//
//
GameProgress::~GameProgress(void)
{
}
//----------------------------------------------------------------------------
//
void GameProgress::init (void)
{
	memset(&progressData, 0, sizeof(PROGRESS_DATA));
	g_tempMissionsCompleted = 0;
}
//----------------------------------------------------------------------------
//
bool GameProgress::setFileSystem (void)
{
#if !defined(_WIN32)
	fileSystem = SAVEDIR;
	return static_cast<IFileSystem *>(fileSystem) != nullptr;
#else
	char buffer[128];

	U32 res = DEFAULTS->GetStringFromRegistry(NAME_REG_KEY, szPlayerName, sizeof(szPlayerName));

	if (res && szPlayerName[0])
	{
		wsprintf(buffer, "SavedGame\\%s", szPlayerName);
	
		DAFILEDESC fdesc = buffer;
		
		if (DACOM->CreateInstance(&fdesc, fileSystem) != GR_OK)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
#endif
}
//--------------------------------------------------------------------------//
//
bool GameProgress::loadGameProgress (void)
{
	if (setFileSystem() == false)
	{
		CQBOMB0("Could not create the file system in GameProgress");
		return false;
	}

	// open up the file for reading
	if (fileSystem)
	{
		char szFile[128];
		DAFILEDESC fdesc;
		COMPTR<IFileSystem> file;
	
		DWORD dwRead;

#if defined(_WIN32)
		wsprintf(szFile, GPF_FILE);
#else
		std::snprintf(szFile, sizeof(szFile), "%s", GPF_FILE);
#endif
		fdesc.lpFileName = szFile;
		fdesc.lpImplementation = "DOS";
		fdesc.dwDesiredAccess = GENERIC_READ;
		fdesc.dwShareMode = 0;  // no sharing
		fdesc.dwCreationDistribution = OPEN_EXISTING;

		if (fileSystem->CreateInstance(&fdesc, file) != GR_OK)
		{
			// will fail the first time we play our game
			return false;
		}

		#if defined(_WIN32)
		file->ReadFile(0, &progressData, sizeof(progressData), &dwRead, 0);
#else
		PROGRESS_DATA loaded = {};
		if (!file->ReadFile(0, &loaded, sizeof(loaded), &dwRead, 0) ||
			dwRead != sizeof(loaded))
		{
			throw std::runtime_error("Native GameProgress could not read the complete player.gpf record");
		}
		progressData = loaded;
#endif
	}
	return true;
}
//--------------------------------------------------------------------------//
//
bool GameProgress::saveGameProgress (void)
{
	if (setFileSystem() == false)
	{
		CQBOMB0("Could not create the file system in GameProgress");
		return false;
	}

	// save off our structure into a file, create the file if it is not there
	if (fileSystem)
	{
		char szFile[128];
		DAFILEDESC fdesc;
		COMPTR<IFileSystem> file;
	
		DWORD dwWritten;

#if defined(_WIN32)
		wsprintf(szFile, GPF_FILE);
#else
		std::snprintf(szFile, sizeof(szFile), "%s", GPF_FILE);
#endif
		fdesc.lpFileName = szFile;
		fdesc.lpImplementation = "DOS";
		fdesc.dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
		fdesc.dwShareMode = 0;  // no sharing
		fdesc.dwCreationDistribution = CREATE_ALWAYS;

		if (fileSystem->CreateInstance(&fdesc, file) != GR_OK)
		{
			CQBOMB0("Could not create save file in GameProgress");
			return false;
		}

		#if defined(_WIN32)
		file->WriteFile(0, &progressData, sizeof(progressData), &dwWritten, 0);
#else
		if (!file->WriteFile(0, &progressData, sizeof(progressData), &dwWritten, 0) ||
			dwWritten != sizeof(progressData))
		{
			throw std::runtime_error("Native GameProgress could not write the complete player.gpf record");
		}
#endif
	}

	return true;
}
//--------------------------------------------------------------------------//
//
U32 GameProgress::GetMissionsCompleted (void)
{
	// the player may have changed names
	setFileSystem();
	
	// open the file for reading purposes
	// create the file if it is not there and set it's value to zero

	// there should be only one file of the type in the SAVEDIR directory

	// actually - we should just return a bitfield telling us what we've seen
	if (loadGameProgress() == false)
	{
		// try to create the file and save off whatever we have in progressData
		saveGameProgress();
	}
	
	return progressData.missionsCompleted | g_tempMissionsCompleted;
}
//--------------------------------------------------------------------------//
//
void GameProgress::SetMissionCompleted (U32 missionID)
{
	// set the missionID'th bit and save off
	progressData.missionsCompleted |= 1 << missionID;

	saveGameProgress();
}
//--------------------------------------------------------------------------//
//
void GameProgress::SetTempMissionsCompleted (U32 missionID)
{
	g_tempMissionsCompleted |= 1 << missionID;
}
//--------------------------------------------------------------------------//
//
void GameProgress::SetMissionsSeen (U32 missionID)
{
	progressData.missionsSeen |= 1 << missionID;

	saveGameProgress();
}
//--------------------------------------------------------------------------//
//
U32 GameProgress::GetMissionsSeen (void)
{
	// the player may have changed names
	setFileSystem();
	
	// open the file for reading purposes
	// create the file if it is not there and set it's value to zero

	// there should be only one file of the type in the SAVEDIR directory

	// actually - we should just return a bitfield telling us what we've seen
	if (loadGameProgress() == false)
	{
		// try to create the file and save off whatever we have in progressData
		saveGameProgress();
	}
	
	return progressData.missionsSeen;
}
//--------------------------------------------------------------------------//
//
U32 GameProgress::GetMoviesSeen (void)
{
	//  just return a bitfield telling us what we've seen
	if (loadGameProgress() == false)
	{
		// try to create the file and save off whatever we have in progressData
		saveGameProgress();
	}
	
	if (g_bForcedIntroMovie)
	{
		U32 moviebits = progressData.moviesSeen;
		moviebits |= 1;
		return moviebits;
	}

	return progressData.moviesSeen;
}
//--------------------------------------------------------------------------//
//
void GameProgress::SetMovieSeen (U32 movieID)
{
	// set the movieID'th bit and save off
	progressData.moviesSeen |= 1 << movieID;

	saveGameProgress();
}
//--------------------------------------------------------------------------//
//
void GameProgress::ReInitialize (void)
{
	init();
	loadGameProgress();
}
//--------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------//
//
#if !defined(_WIN32)
bool CheckNativeGameProgressPersistence(IFileSystem *directory)
{
	// Scratch-only regression for the production profile owner. This never
	// supplies progression to a mission or represents a campaign transition.
	if (!directory) return false;
	struct Scope
	{
		IFileSystem *previous = SAVEDIR;
		U32 temporary = g_tempMissionsCompleted;
		bool intro = g_bForcedIntroMovie;
		~Scope()
		{
			SAVEDIR = previous;
			g_tempMissionsCompleted = temporary;
			g_bForcedIntroMovie = intro;
		}
	} scope;
	SAVEDIR = directory;
	g_bForcedIntroMovie = false;
	DAComponent<GameProgress> owner;
	const PROGRESS_DATA expected = {2U, 4U, 8U};
	owner.progressData = expected;
	if (!owner.saveGameProgress()) return false;
	owner.ReInitialize();
	if (owner.GetMissionsCompleted() != expected.missionsCompleted ||
		owner.GetMoviesSeen() != expected.moviesSeen ||
		owner.GetMissionsSeen() != expected.missionsSeen) return false;
	DAFILEDESC descriptor = GPF_FILE;
	descriptor.lpImplementation = "DOS";
	descriptor.dwDesiredAccess = GENERIC_READ;
	descriptor.dwCreationDistribution = OPEN_EXISTING;
	COMPTR<IFileSystem> file;
	if (directory->CreateInstance(&descriptor, file) != GR_OK) return false;
	PROGRESS_DATA observed = {};
	DWORD count = 0;
	if (file->GetFileSize(0, nullptr) != sizeof(expected) ||
		!file->ReadFile(0, &observed, sizeof(observed), &count, nullptr) ||
		count != sizeof(observed) || std::memcmp(&observed, &expected, sizeof(expected))) return false;
	file.free();
	descriptor.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	descriptor.dwCreationDistribution = CREATE_ALWAYS;
	if (directory->CreateInstance(&descriptor, file) != GR_OK ||
		!file->WriteFile(0, &expected, sizeof(U32), &count, nullptr) || count != sizeof(U32)) return false;
	file.free();
	bool rejected = false;
	try { owner.GetMissionsCompleted(); }
	catch (const std::runtime_error &) { rejected = true; }
	descriptor.dwDesiredAccess = GENERIC_READ;
	descriptor.dwCreationDistribution = OPEN_EXISTING;
	if (!rejected || directory->CreateInstance(&descriptor, file) != GR_OK ||
		file->GetFileSize(0, nullptr) != sizeof(U32) ||
		std::memcmp(&owner.progressData, &expected, sizeof(expected))) return false;
	file.free();
	return directory->DeleteFile(GPF_FILE) != 0;
}
#endif

struct _gameProgress : GlobalComponent
{
	virtual void Startup (void)
	{
		GAMEPROGRESS = new DAComponent<GameProgress>;
		AddToGlobalCleanupList((IDAComponent **) &GAMEPROGRESS);
	}

	virtual void Initialize (void)
	{
	}
};

static _gameProgress startup;

//--------------------------------------------------------------------------//
//--------------------------End ScrollingText.cpp---------------------------//
//--------------------------------------------------------------------------//
