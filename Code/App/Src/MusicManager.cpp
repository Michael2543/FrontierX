//                                                                          //
//                            MusicManager.cpp                              //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Author: Tmauer $

   $Header: /Conquest/App/Src/MusicManager.cpp 24    4/18/01 8:58a Tmauer $

   High level music player. Streams .WAV files in the background.
*/
//--------------------------------------------------------------------------//

#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeMusicDataFormat.h"
#include "FrontierX/LegacyInput.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

[[noreturn]] static void NativeMusicFailure(const char *reason)
{
	std::fprintf(stderr, "Native MusicManager: %s\n", reason);
	std::abort();
}
#endif

#include "MusicManager.h"
#if !defined(_WIN32)
#include "Include/NativeMusicManager.h"
#endif
#include "CQTrace.h"
#include "EventPriority.h"
#if defined(_WIN32)
#include "TDocClient.h"
#include "UserDefaults.h"
#include "Resource.h"
#include "Menu.h"
#endif
#include "Startup.h"
#include <DMusic.h>
#if defined(_WIN32)
#include "BaseHotRect.h"
#endif
#include "Mission.h"
#include <DMissionEnum.h>
#include "SoundManager.h"

#include <Streamer.h>
#include <TComponent.h>
#include <IConnection.h>
#include <HeapObj.h>
#include <TSmartPointer.h>
#include <IConnection.h>
#if defined(_WIN32)
#include <Viewer.h>
#include <ViewCnst.h>
#include <Document.h>
#include <IDocClient.h>
#endif
#include <EventSys.h>
#if defined(_WIN32)
#include <MemFile.h>
#else
#include <Filesys.h>
#endif

static char szKey[] = "Music";


struct DACOM_NO_VTABLE MManager : IMusicManager, IEventCallback
#if defined(_WIN32)
    , DocumentClient
#endif
{
	BEGIN_DACOM_MAP_INBOUND(MManager)
	DACOM_INTERFACE_ENTRY(IMusicManager)
#if defined(_WIN32)
	DACOM_INTERFACE_ENTRY(IDocumentClient)
	DACOM_INTERFACE_ENTRY_REF("IViewer", viewer)
#endif
	DACOM_INTERFACE_ENTRY(IEventCallback)
	END_DACOM_MAP()

#if defined(_WIN32)
	COMPTR<IViewer> viewer;
	COMPTR<IDocument> doc;
#endif

	U32 menuID;
	U32 eventHandle;
	BOOL32 bIgnoreUpdate, bActive;
	SINGLE musicVolume;

	MT_MUSIC_DATA data;

	HSTREAM loadTimeHandle;

	char currentMusic[MAX_PATH];

	//------------------------

	MManager (void);

	~MManager (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IMusicManager methods */

	DEFMETHOD_(void,Enable) (BOOL32 bEnable);

	DEFMETHOD_(void,SetVolume) (SINGLE volume);

	virtual void __stdcall Load (struct IFileSystem * inFile);
	
	virtual void __stdcall LoadFinish ();

	virtual void __stdcall Save (struct IFileSystem * outFile);

	virtual void __stdcall InitPlaylist (enum M_RACE race);

	virtual void __stdcall PlayMusic (const char * fileName, bool bSmoothTransition);

	virtual void __stdcall GetMusicFileName (struct M_STRING & string);

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param = 0);

	/* IDocumentClient methods */

#if defined(_WIN32)
	DEFMETHOD(OnUpdate) (struct IDocument *doc, const C8 *message = 0, void *parm = 0);
#else
	GENRESULT OnUpdate ();
#endif

	/* MManager methods */

#if defined(_WIN32)
	BOOL32 CreateViewer (void);
#endif

	void handleStreamerNotify (HSTREAM hStream, IStreamer::STATUS state);

	IDAComponent * getBase (void)
	{
		return static_cast<IMusicManager *>(this);
	}


};
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
MManager::MManager (void)
{
	musicVolume = 1.0;
	loadTimeHandle = 0;
}
//--------------------------------------------------------------------------//
//
MManager::~MManager (void)
{
#if !defined(_WIN32)
	if (STREAMER)
		for (int i = 0; i < NUM_SONGS; ++i)
			if (data[i].handle) STREAMER->CloseHandle(data[i].handle);
#endif
	if(loadTimeHandle)
	{
		STREAMER->CloseHandle(loadTimeHandle);
		loadTimeHandle = 0;
	}
#if defined(_WIN32)
	if (FULLSCREEN)
	{
		COMPTR<IDAConnectionPoint> connection;
		
		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Unadvise(eventHandle);
	}
#else
	if (EVENTSYS && eventHandle)
	{
		COMPTR<IDAConnectionPoint> connection;
		if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Unadvise(eventHandle);
	}
#endif
}
//--------------------------------------------------------------------------//
//
GENRESULT MManager::Notify (U32 message, void *param)
{
#if defined(_WIN32)
	MSG *msg = (MSG *) param;
#else
	const FrontierX::LegacyWindowMessage *msg =
		static_cast<const FrontierX::LegacyWindowMessage *>(param);
#endif

	switch (message)
	{
	case CQE_STREAMER:
		if (!msg) break;
#if defined(_WIN32)
		handleStreamerNotify((HSTREAM)msg->lParam, (IStreamer::STATUS)msg->wParam);
#else
		handleStreamerNotify(reinterpret_cast<HSTREAM>(msg->lparam),
			static_cast<IStreamer::STATUS>(msg->wparam));
#endif
		break;

#if defined(_WIN32)
	case WM_CLOSE:
		if (viewer)
			viewer->set_display_state(0);
		break;

#if 0		
	case WM_CHAR:
		if (msg->wParam == 'p')
			STREAMER->Open(data[1].filename, MUSICDIR);
		break;
#endif

	case WM_COMMAND:
		if (LOWORD(msg->wParam) == menuID)
		{
			if (viewer)
				viewer->set_display_state(1);
		}
		break;
#endif
	};

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
void MManager::Enable (BOOL32 bEnable)
{
	if (bEnable == bActive)
		return;				// no change

	if ((bActive = bEnable) == 0)
	{
		// disable all streams
		int i;

		for (i = 0; i < NUM_SONGS; i++)
			if (data[i].handle)
			{
#if defined(_WIN32)
				STREAMER->Stop(data[i].handle);
#else
				if (!STREAMER->Stop(data[i].handle))
					NativeMusicFailure("stopping music stream failed");
#endif
			}
	}
	else
#if defined(_WIN32)
	if (doc)		// might not be initialized yet
		OnUpdate(doc);
#else
	OnUpdate();
#endif
}
//--------------------------------------------------------------------------//
//
void MManager::SetVolume (SINGLE volume)
{
#if !defined(_WIN32)
	if (!std::isfinite(volume)) NativeMusicFailure("music volume is not finite");
#endif
	if (musicVolume != volume)
	{
		int i;

		musicVolume = volume;

		for (i = 0; i < NUM_SONGS; i++)
			if (data[i].handle)
			{
#if defined(_WIN32)
				STREAMER->SetVolume(data[i].handle, ConvertFloatToDSound(data[i].volume * musicVolume));
#else
				if (!STREAMER->SetVolume(data[i].handle,
					ConvertFloatToDSound(data[i].volume * musicVolume)))
					NativeMusicFailure("setting music volume failed");
#endif
			}
	}
}
//--------------------------------------------------------------------------//
//
void MManager::Load (struct IFileSystem * inFile)
{
#if !defined(_WIN32)
	if (!inFile || !MISSION || !STREAMER) NativeMusicFailure("required load owner missing");
	std::memset(currentMusic, 0, sizeof(currentMusic));
	if (!inFile->SetCurrentDirectory("\\MT_MUSIC_DATA")) return;
	DAFILEDESC descriptor("Music");
	const HANDLE file = inFile->OpenChild(&descriptor);
	if (file == INVALID_HANDLE_VALUE) return;
	const DWORD size = inFile->GetFileSize(file, nullptr);
	if (size == 0)
	{
		if (!inFile->CloseHandle(file)) NativeMusicFailure("closing empty music save failed");
		return;
	}
	if (size != FrontierX::MusicDataDiskSize) NativeMusicFailure("invalid music save length");
	FrontierX::MusicDataDiskBytes saved = {};
	FrontierX::MusicDataDiskBytes correlated = {};
	DWORD read = 0;
	const BOOL32 loaded = inFile->ReadFile(file, saved.data(), saved.size(), &read, nullptr);
	const BOOL32 closed = inFile->CloseHandle(file);
	if (!loaded || read != saved.size() || !closed ||
		MISSION->CorrelateSymbol("MT_MUSIC_DATA", saved.data(), correlated.data()) != GR_OK)
		NativeMusicFailure("music save read or symbol correlation failed");
	MT_MUSIC_DATA decoded = {};
	if (!FrontierX::DecodeMusicData(correlated, decoded))
		NativeMusicFailure("invalid saved music song");
	for (int i = 0; i < NUM_SONGS; ++i)
	{
		if (data[i].handle)
		{
			if (data[i].playing && !loadTimeHandle) loadTimeHandle = data[i].handle;
			else if (!STREAMER->CloseHandle(data[i].handle))
				NativeMusicFailure("closing prior music stream failed");
		}
		data[i] = decoded[i];
	}
#else
#ifndef _DEMO_
	HANDLE hFile=INVALID_HANDLE_VALUE;
	DAFILEDESC fdesc = "Music";
	DWORD dwRead;
	char buffer[1024];
	int i;

	if (inFile->SetCurrentDirectory("\\MT_MUSIC_DATA") == 0)
		goto Done;

	if ((hFile = inFile->OpenChild(&fdesc)) == INVALID_HANDLE_VALUE)
		goto Done;

	if (inFile->GetFileSize(hFile) == 0)
		goto Done;

//	SOUNDMANAGER->TweekMusicVolume();

	for (i = 0; i < NUM_SONGS; i++)
		if (data[i].handle)
		{
			if(data[i].playing && !loadTimeHandle)
			{
				loadTimeHandle = data[i].handle;
			}
			else
			{
				STREAMER->CloseHandle(data[i].handle);
			}
		}

	inFile->ReadFile(hFile, buffer, sizeof(buffer), &dwRead, 0);
	
	MISSION->CorrelateSymbol("MT_MUSIC_DATA", buffer, &data);

	for (i = 0; i < NUM_SONGS; i++)
		data[i].handle = 0;

Done:
	if (hFile != INVALID_HANDLE_VALUE)
		inFile->CloseHandle(hFile);

	// null out or current file so that the right music gets a chance to be played
	memset(currentMusic, 0, sizeof(currentMusic));
#endif  // !_DEMO_
#endif
}

//--------------------------------------------------------------------------//
//

void MManager::LoadFinish ()
{
	if(loadTimeHandle)
	{
#if defined(_WIN32)
		STREAMER->CloseHandle(loadTimeHandle);
#else
		if (!STREAMER->CloseHandle(loadTimeHandle))
			NativeMusicFailure("closing prior load-time stream failed");
#endif
		loadTimeHandle = 0;
	}
#if defined(_WIN32)
	doc->UpdateAllClients();
#else
	OnUpdate();
#endif
}

//--------------------------------------------------------------------------//
//
void MManager::Save (struct IFileSystem * outFile)
{
#if !defined(_WIN32)
	if (!outFile) NativeMusicFailure("save filesystem missing");
	DAFILEDESC descriptor("Music");
	descriptor.dwDesiredAccess |= GENERIC_WRITE;
	descriptor.dwShareMode = 0;
	descriptor.dwCreationDistribution = CREATE_ALWAYS;
	outFile->CreateDirectory("\\MT_MUSIC_DATA");
	if (!outFile->SetCurrentDirectory("\\MT_MUSIC_DATA"))
		NativeMusicFailure("music save directory unavailable");
	const HANDLE file = outFile->OpenChild(&descriptor);
	if (file == INVALID_HANDLE_VALUE) NativeMusicFailure("opening music save failed");
	bool playing = false;
	for (int i = 0; i < NUM_SONGS; ++i) playing |= data[i].playing;
	if (playing)
	{
		FrontierX::MusicDataDiskBytes saved = {};
		DWORD written = 0;
		if (!FrontierX::EncodeMusicData(data, saved) ||
			!outFile->WriteFile(file, saved.data(), saved.size(), &written, nullptr) ||
			written != saved.size()) NativeMusicFailure("writing music save failed");
	}
	if (!outFile->CloseHandle(file)) NativeMusicFailure("closing music save failed");
#else
	HANDLE hFile=INVALID_HANDLE_VALUE;
	DAFILEDESC fdesc = "Music";
	DWORD dwWritten;
	int i;

	fdesc.dwDesiredAccess |= GENERIC_WRITE;
	fdesc.dwShareMode = 0;
	fdesc.dwCreationDistribution = CREATE_ALWAYS;

	outFile->CreateDirectory("\\MT_MUSIC_DATA");

	if (outFile->SetCurrentDirectory("\\MT_MUSIC_DATA") == 0)
		goto Done;

	if ((hFile = outFile->OpenChild(&fdesc)) == INVALID_HANDLE_VALUE)
		goto Done;

	for (i = 0; i < NUM_SONGS; i++)
		if (data[i].playing)
		{
			// only save struct if at least one song is playing
			outFile->WriteFile(hFile, &data, sizeof(data), &dwWritten, 0);
			break;
		}

Done:
	if (hFile != INVALID_HANDLE_VALUE)
		outFile->CloseHandle(hFile);
#endif
}
//--------------------------------------------------------------------------//
//
void MManager::InitPlaylist (enum M_RACE race)
{
	const char *fileName = 0;
	
	switch (race)
	{
	case M_TERRAN:
		fileName = "terrangame14.wav";
		break;
	case M_MANTIS:
		fileName = "mantisgame14.wav";
		break;
	case M_SOLARIAN:
		fileName = "solarian_Game14.wav";
		break;
	}

	PlayMusic(fileName, true);
}
//--------------------------------------------------------------------------//
//
void MManager::PlayMusic (const char * fileName, bool bSmoothTransition)
{
#if !defined(_WIN32)
	if (fileName && (std::strlen(fileName) >= sizeof(data[0].filename) ||
		std::strlen(fileName) >= sizeof(currentMusic)))
		NativeMusicFailure("music filename exceeds saved field");
#endif
#ifdef _DEMO_
	if (fileName)
		fileName = "demomusic.wav";		// override filename for demo
#endif
	if (fileName)
	{
		CQTRACE11("Attempting to play music file   %s", fileName);

		// if the filename is the same as what is already playing, then ignore it
		if (strcmp(fileName, currentMusic) == 0)
		{
			return;
		}
		else
		{
			strcpy(currentMusic, fileName);
		}

		int i;
		if (bSmoothTransition)
		{
#if defined(_WIN32)
			SOUNDMANAGER->TweekMusicVolume();
#else
			if (SOUNDMANAGER) SOUNDMANAGER->TweekMusicVolume();
#endif
		}
		
		for (i = 0; i < NUM_SONGS; i++)
		{
			if (data[i].handle)
			{
				STREAMER->CloseHandle(data[i].handle);
				data[i].handle = 0;
				data[0].looping = 0;
				data[0].playing = 0;
				data[0].filename[0] = 0;
			}
		}

		strncpy(data[0].filename, fileName, sizeof(data[0].filename));
		data[0].volume = 1;
		data[0].playing = 1;
		data[0].looping = 1;

#if defined(_WIN32)
			doc->UpdateAllClients();
#else
			OnUpdate();
#endif
	}
	else	// just shutdown all music
	{
		// zero out the current music file
		memset(currentMusic, 0, sizeof(currentMusic));

		int i;
		if (bSmoothTransition)
		{
#if defined(_WIN32)
			SOUNDMANAGER->TweekMusicVolume();
#else
			if (SOUNDMANAGER) SOUNDMANAGER->TweekMusicVolume();
#endif
		}
		
		for (i = 0; i < NUM_SONGS; i++)
		{
			if (data[i].handle)
			{
				STREAMER->CloseHandle(data[i].handle);
				data[i].handle = 0;
				data[0].looping = 0;
				data[0].playing = 0;
				data[0].filename[0] = 0;
			}
		}

#if defined(_WIN32)
		doc->UpdateAllClients();
#else
		OnUpdate();
#endif
	}
}
//--------------------------------------------------------------------------//
//
void MManager::GetMusicFileName (struct M_STRING & string)
{
	int i;
	string.string[0] = 0;

	for (i = 0; i < NUM_SONGS; i++)
		if (data[i].playing)
		{
			string = data[i].filename;
			break;
		}

}
//--------------------------------------------------------------------------//
// data has changed. do something about it
//
#if defined(_WIN32)
GENRESULT MManager::OnUpdate (struct IDocument *doc, const C8 *message, void *parm)
#else
GENRESULT MManager::OnUpdate ()
#endif
{
	if (bActive && bIgnoreUpdate == 0)
	{
		int i;

		bIgnoreUpdate++;

		for (i = 0; i < NUM_SONGS; i++)
		{
			if (data[i].playing == 0)
			{
				if (data[i].handle)
				{
					STREAMER->CloseHandle(data[i].handle);
					data[i].handle = 0;
					data[0].looping = 0;
				}
			}
			else // user wants to play this one
			{
				if (data[i].handle==0)
				{
					U32 flags = (data[i].looping) ? (STRMFL_PLAY|STRMFL_LOOPING) : STRMFL_PLAY;
					data[i].handle = STREAMER->Open(data[i].filename, MUSICDIR, flags);
#if !defined(_WIN32)
					if (!data[i].handle) NativeMusicFailure("required music stream open failed");
#endif
				}
				else	// currently has a valid handle, is it playing?
				{
					switch (STREAMER->GetStatus(data[i].handle))
					{
					case IStreamer::PLAYING:
					case IStreamer::EOFREACHED:
						break;

					case IStreamer::STOPPED:
						#if defined(_WIN32)
						STREAMER->Restart(data[i].handle);
						#else
						if (!STREAMER->Restart(data[i].handle))
							NativeMusicFailure("restarting music stream failed");
						#endif
						break;
					}
				}

				if (data[i].handle)
				{
#if defined(_WIN32)
					STREAMER->SetVolume(data[i].handle, ConvertFloatToDSound(data[i].volume * musicVolume));
#else
					if (!STREAMER->SetVolume(data[i].handle,
						ConvertFloatToDSound(data[i].volume * musicVolume)))
						NativeMusicFailure("setting active music volume failed");
#endif
				}
			}
		}

#if defined(_WIN32)
		doc->UpdateAllClients(0);
#endif
		bIgnoreUpdate--;
	}
	return GR_OK;
}
//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
BOOL32 MManager::CreateViewer (void)
{
	DOCDESC ddesc = "MusicData";
	COMPTR<IFileSystem> pMemFile;

	// create a memory file

	MEMFILEDESC mdesc = ddesc.lpFileName;
	mdesc.dwDesiredAccess  = GENERIC_WRITE | GENERIC_READ;
	mdesc.lpBuffer = &data;
	mdesc.dwBufferSize = sizeof(data);
	mdesc.dwFlags = CMF_DONT_COPY_MEMORY;
	if (DACOM->CreateInstance(&mdesc, pMemFile) != GR_OK)
		return 0;

	pMemFile->AddRef();
	ddesc.lpParent = pMemFile;
	ddesc.dwDesiredAccess  = GENERIC_WRITE | GENERIC_READ;
	ddesc.dwCreationDistribution = CREATE_ALWAYS;
	ddesc.lpImplementation = "DOS";

	if (DACOM->CreateInstance(&ddesc, doc) == GR_OK)
	{
		VIEWDESC vdesc;
		HWND hwnd;

		vdesc.className = "MT_MUSIC_DATA";
		vdesc.doc = doc;
		vdesc.hOwnerWindow = hMainWindow;
		
		if (PARSER->CreateInstance(&vdesc, viewer) == GR_OK)
		{
			COMPTR<IDAConnectionPoint> connection;

			viewer->get_main_window((void **)&hwnd);
			MoveWindow(hwnd, 100, 100, 400, 200, 1);

			viewer->set_instance_name("Music data");

			MakeConnection(doc);
		}
	}

	return (viewer != 0);
}
#endif
//--------------------------------------------------------------------------//
//
void MManager::handleStreamerNotify (HSTREAM hStream, IStreamer::STATUS state)
{
	int i;
	BOOL32 bDataChanged=0;
	S32 index = -1;
	BOOL32 bPlaying = 0;

#if defined(_WIN32)
	CQASSERT(hStream);
#else
	if (!hStream) std::abort();
#endif
	//
	// close streams that have completed
	//
	for (i = 0; i < NUM_SONGS; i++)
	{
		if (data[i].handle == hStream)
		{
			switch (state)
			{
			case IStreamer::EOFREACHED:
				index = i;
				break;

	 		case IStreamer::INVALID:
				data[i].filename[0] = 0;
				// fall through intentional
	 		case IStreamer::COMPLETED:
				STREAMER->CloseHandle(data[i].handle);
				data[i].playing = data[i].looping = 0;
				data[i].handle = 0;
				bDataChanged = 1;
				break;
			}
		}
		else
		if (data[i].playing)
			bPlaying = 1;
	}

	//
	// if active and not currently playing a tune, start a new one
	//

	if (bActive && bPlaying == 0)
	{
		int j=++index;
		if (j >= NUM_SONGS)
			j = index = 0;
		do
		{
			if (data[j].filename[0])
			{
				if (data[j].playing == 0)
				{
					data[j].playing = 1;
					bDataChanged = 1;
				}
				break;
			}
			j++;
			if (j >= NUM_SONGS)
				j = 0;
		} while (index != j);
	}

	if (bDataChanged)
	{
#if defined(_WIN32)
		OnUpdate(doc);
#else
		OnUpdate();
#endif
	}
}
//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
struct _music : GlobalComponent
{
	MManager * manager;

	virtual void Startup (void)
	{
		MUSICMANAGER = manager = new DAComponent<MManager>;
		AddToGlobalCleanupList((IDAComponent **)&MUSICMANAGER);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;
#if defined(_WIN32)
		HMENU hMenu = MENU->GetSubMenu(MENUPOS_VIEW);
		MENUITEMINFO minfo;

		manager->CreateViewer(); // viewer is a debug tool; failure is non-fatal

		memset(&minfo, 0, sizeof(minfo));
		minfo.cbSize = sizeof(minfo);
		minfo.fMask = MIIM_ID | MIIM_TYPE;
		minfo.fType = MFT_STRING;
		minfo.wID = IDS_VIEWMUSIC;
		minfo.dwTypeData = "Music";
		minfo.cch = 5;	// length of string "Music"
			
		if (InsertMenuItem(hMenu, 0x7FFE, 1, &minfo))
			manager->menuID = IDS_VIEWMUSIC;

		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Advise(manager->getBase(), &manager->eventHandle);
		FULLSCREEN->SetCallbackPriority(manager,EVENT_PRIORITY_MUSIC);
#else
		if (!STREAMER || !MUSICDIR || !EVENTSYS ||
			EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
			connection->Advise(manager->getBase(), &manager->eventHandle) != GR_OK)
		{
			std::fputs("Native music manager requires streamer, Music source and event system\n", stderr);
			std::abort();
		}
		manager->Enable(TRUE);
#endif
	}
};

static _music music;
#else
namespace FrontierX
{
IMusicManager *CreateNativeMusicManager()
{
	if (!STREAMER || !MUSICDIR || !EVENTSYS) return nullptr;
	MManager *manager = new DAComponent<MManager>;
	COMPTR<IDAConnectionPoint> connection;
	if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
		connection->Advise(manager->getBase(), &manager->eventHandle) != GR_OK)
	{
		manager->getBase()->Release();
		return nullptr;
	}
	manager->Enable(TRUE);
	return manager;
}
}
#endif
//--------------------------------------------------------------------------//
//-------------------------END MusicManager.cpp-----------------------------//
//--------------------------------------------------------------------------//
