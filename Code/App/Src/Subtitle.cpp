//--------------------------------------------------------------------------//
//                                                                          //
//                      Subtitle.cpp                                        //
//                                                                          //
//               COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.                  //
//																			//
//--------------------------------------------------------------------------//
/*
   $Author: Tmauer $

	Control that scrolls informative text to the user			

*/
//--------------------------------------------------------------------------//
 
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppFont.h"
#include "Include/NativeAppMissionMedia.h"
#include "resource.h"
#include "UserDefaults.h"
#include "CQTrace.h"
#include <VFX.H>
#include <EventSys.h>
#include <IConnection.h>
#include <TComponent.h>
#include <TSmartPointer.h>
#include <cstdlib>
#include <cwchar>
#include <cstdint>
#include <limits>
#endif

#include "ISubtitle.h"
#if defined(_WIN32)
#include "BaseHotRect.h"
#endif
#include "EventPriority.h"
#include "DrawAgent.h"
#include "Startup.h"
#include "SoundManager.h"

#if defined(_WIN32)
#include "frame.h"
#endif

#if !defined(_WIN32)
static std::uint64_t subtitleDraws = 0;
#endif

#define SUB_POSX1 150
#define SUB_POSX2 635
#define SUB_POSY1 110
#define SUB_POSY2 350

#define B_SUB_POSX1 70
#define B_SUB_POSX2 745
#define B_SUB_POSY1 320
#define B_SUB_POSY2 550

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE Subtitle : public IEventCallback, ISubtitle
{
	BEGIN_DACOM_MAP_INBOUND(Subtitle)
	DACOM_INTERFACE_ENTRY(ISubtitle)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	END_DACOM_MAP()

	U32 handle;			// connection handle
	
	U32 soundHandle;
	wchar_t textBuffer[1024];

	bool bBriefing;

	COMPTR<IFontDrawAgent> fontAgent;

	Subtitle (void);

	~Subtitle (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* ISubtitle methods */

	virtual void NewSubtitle(wchar_t * subtitle,U32 _soundHandle);

	virtual void NewBriefingSubtitle(wchar_t * subtitle,U32 _soundHandle);

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param = 0);

	/* Subtitle methods */

	void reloadFonts (bool bLoad);

	void draw (void);

	IDAComponent * GetBase (void)
	{
		return (ISubtitle *) this;
	}

	void onUpdate (U32 dt)			// dt is milliseconds
	{
		if(soundHandle && (!SOUNDMANAGER->IsPlaying(soundHandle)))
		{
			soundHandle = 0;
		}

	}
};

//--------------------------------------------------------------------------//
//
Subtitle::Subtitle (void)
{
	soundHandle = 0;
}
//--------------------------------------------------------------------------//
//
Subtitle::~Subtitle (void)
{
	fontAgent.free();
#if !defined(_WIN32)
    if (EVENTSYS && handle)
    {
        COMPTR<IDAConnectionPoint> connection;
        if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
            connection->Unadvise(handle);
    }
#endif
}
//--------------------------------------------------------------------------//
//
void Subtitle::NewBriefingSubtitle(wchar_t * subtitle,U32 _soundHandle)
{
	if(subtitle)
	{
		CQASSERT(subtitle[0]-1 < 1024);
		soundHandle = _soundHandle;
		wcsncpy(textBuffer,subtitle+1,(subtitle[0] < 1023 ? subtitle[0] : 1023));
		textBuffer[(subtitle[0] < 1023 ? subtitle[0] : 1023)] = 0;

		if(!fontAgent)
			reloadFonts(true);

		PANE pane;
		pane.window = NULL;
		pane.x0 = IDEAL2REALX(B_SUB_POSX1);
		pane.x1 = IDEAL2REALX(B_SUB_POSX2);
		pane.y0 = IDEAL2REALY(B_SUB_POSY1);
		pane.y1 = IDEAL2REALY(B_SUB_POSY2);
		
		bBriefing = true;
	}
}
//--------------------------------------------------------------------------//
//
void Subtitle::NewSubtitle(wchar_t * subtitle,U32 _soundHandle)
{
	if(subtitle)
	{
		CQASSERT(subtitle[0]-1 < 1024);
		soundHandle = _soundHandle;
		wcsncpy(textBuffer,subtitle+1,(subtitle[0] < 1023 ? subtitle[0] : 1023));
		textBuffer[(subtitle[0] < 1023 ? subtitle[0] : 1023)] = 0;

		if(!fontAgent)
			reloadFonts(true);

		PANE pane;
		pane.window = NULL;
		pane.x0 = IDEAL2REALX(SUB_POSX1);
		pane.x1 = IDEAL2REALX(SUB_POSX2);
		pane.y0 = IDEAL2REALY(SUB_POSY1);
		pane.y1 = IDEAL2REALY(SUB_POSY2);
		
		bBriefing = false;
	}
}
//--------------------------------------------------------------------------//
//
void Subtitle::draw (void)
{
	if(soundHandle && fontAgent && DEFAULTS->GetDefaults()->bSubtitles && !CQFLAGS.bGamePaused)
	{
		bool bFrameLocked = (CQFLAGS.bFrameLockEnabled != 0);

		#if !defined(_WIN32)
        CQASSERT(!bFrameLocked);
#else
        if (bFrameLocked)
		{
			if (SURFACE->Lock() == false)
			{
				return;
			}
		}

#endif
#if !defined(_WIN32)
        ++subtitleDraws;
#endif
		if(bBriefing)
		{
			PANE pane;
			pane.window = NULL;

			pane.x0 = IDEAL2REALX(B_SUB_POSX1);
			pane.x1 = IDEAL2REALX(B_SUB_POSX2);
			pane.y0 = IDEAL2REALY(B_SUB_POSY1);
			pane.y1 = IDEAL2REALY(B_SUB_POSY2);
			fontAgent->StringDraw(&pane,  0, 0, textBuffer);
		}
		else
		{
			PANE pane;
			pane.window = NULL;

			pane.x0 = IDEAL2REALX(SUB_POSX1);
			pane.x1 = IDEAL2REALX(SUB_POSX2);
			pane.y0 = IDEAL2REALY(SUB_POSY1);
			pane.y1 = IDEAL2REALY(SUB_POSY2);
			fontAgent->StringDraw(&pane,  0, 0, textBuffer);
		}

#if defined(_WIN32)
		if (bFrameLocked)
		{
			SURFACE->Unlock();
		}
#endif
	}
}
//--------------------------------------------------------------------------//
// receive notifications from event system
//
GENRESULT Subtitle::Notify (U32 message, void *param)
{
	switch (message)
	{
	#if !defined(_WIN32)
    case CQE_MISSION_CLOSE:
        soundHandle = 0;
        textBuffer[0] = 0;
        break;
#endif
    case CQE_START3DMODE:
		reloadFonts(true);
		break;
	case CQE_END3DMODE:
		reloadFonts(false);
		break;

	case CQE_ENDFRAME:
		draw();
		break;


	case CQE_UPDATE:
		#if defined(_WIN32)
        onUpdate(S32(param) >> 10);
#else
        if (reinterpret_cast<std::uintptr_t>(param) > std::numeric_limits<U32>::max()) return GR_INVALID_PARMS;
        onUpdate(static_cast<U32>(reinterpret_cast<std::uintptr_t>(param)) >> 10);
#endif
		break;
	}

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
void Subtitle::reloadFonts (bool bLoad)
{
	if (bLoad)
	{
#if defined(_WIN32)
		HFONT hFont;
		COLORREF pen, background;

		pen			= RGB(255,255,255) | 0xFF000000;		
		background	= RGB(0,0,0);
		hFont = CQCreateFont(IDS_SUBTITLE_FONT);

		CreateMultilineFontDrawAgent(0,hFont, pen, background, fontAgent);
#else
        *static_cast<IFontDrawAgent **>(fontAgent) = FrontierX::CreateNativeFontDrawAgent(IDS_SUBTITLE_FONT, RGB(255,255,255) | 0xFF000000, true);
        CQASSERT(fontAgent);
#endif
	}
	else
	{
		fontAgent.free();
	}
}

//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
struct _subtitle : GlobalComponent
{
	Subtitle * SSubTitle;
	
	virtual void Startup (void)
	{
		SUBTITLE = SSubTitle = new DAComponent<Subtitle>;
		AddToGlobalCleanupList((IDAComponent **) &SUBTITLE);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;
	
		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
		{
			connection->Advise(SSubTitle->GetBase(), &SSubTitle->handle);
			FULLSCREEN->SetCallbackPriority(SSubTitle, EVENT_PRIORITY_TEXTCHAT);
		}
	}
};

static _subtitle sub_startup;
#else
namespace FrontierX
{
std::uint64_t NativeSubtitleDrawCount() { return subtitleDraws; }
ISubtitle *CreateNativeSourceSubtitle()
{
    if (!EVENTSYS) return nullptr;
    auto *owner = new DAComponent<Subtitle>;
    COMPTR<IDAConnectionPoint> connection;
    if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
        connection->Advise(owner->GetBase(), &owner->handle) != GR_OK)
    { owner->Release(); return nullptr; }
    return owner;
}
// Extend the existing font lifecycle check: the mission script supplies counted
// strings and sound handles, which the former disabled path could not retain.
bool CheckNativeSourceSubtitle()
{
    const unsigned fonts = NativeFontLiveCount();
    auto *interface = CreateNativeSourceSubtitle();
    if (!interface) return false;
    auto *owner = static_cast<Subtitle *>(interface);
    wchar_t text[] = {2,L'A',L'B'};
    owner->NewSubtitle(text, 42);
    PANE pane{}; pane.x1 = owner->fontAgent->GetStringWidth(L"A") - 1; pane.y1 = 100;
    bool valid = owner->soundHandle == 42 && !owner->bBriefing &&
        std::wcscmp(owner->textBuffer, L"AB") == 0 && owner->fontAgent->IsMultiLine() &&
        owner->fontAgent->GetNumRows(L"AB", &pane) == 2 &&
        owner->fontAgent->GetNumRows(L"A\r\nB", &pane) == 2;
    IFileSystem *savedSpeech = SPEECHDIR, *savedMissionSpeech = MSPEECHDIR;
    // No file is read for an unknown handle; reuse the generated archive.
    SPEECHDIR = MSPEECHDIR = SPMAPDIR;
    ISoundManager *savedSound = SOUNDMANAGER;
    SOUNDMANAGER = CreateNativeMissionSoundManager(true, true);
    valid = valid && SOUNDMANAGER;
    if (SOUNDMANAGER)
    {
        owner->Notify(CQE_UPDATE, reinterpret_cast<void *>(1024));
        valid = valid && owner->soundHandle == 0;
        SOUNDMANAGER->Release();
    }
    SOUNDMANAGER = savedSound;
    SPEECHDIR = savedSpeech; MSPEECHDIR = savedMissionSpeech;
    IFontDrawAgent *duplicate = nullptr;
    valid = valid && owner->fontAgent->CreateDuplicate(&duplicate) == GR_OK && duplicate && duplicate->IsMultiLine();
    if (duplicate) duplicate->Release();
    owner->NewBriefingSubtitle(text, 0);
    valid = valid && owner->bBriefing && owner->soundHandle == 0;
    owner->Notify(CQE_MISSION_CLOSE);
    valid = valid && !owner->textBuffer[0];
    owner->Notify(CQE_END3DMODE);
    interface->Release();
    return valid && NativeFontLiveCount() == fonts;
}
}
#endif

//--------------------------------------------------------------------------//
//--------------------------End Subtitle.cpp--------------------------------//
//--------------------------------------------------------------------------//