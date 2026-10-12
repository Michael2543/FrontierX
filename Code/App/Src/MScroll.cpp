//--------------------------------------------------------------------------//
//                                                                          //
//                                MScroll.cpp                               //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Author: Jasony $

   $Header: /Conquest/App/Src/MScroll.cpp 10    7/14/00 11:27a Jasony $


   Mouse scrolling component
*/
//--------------------------------------------------------------------------//

 
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppTextResources.h"
#include <3dmath.h>
#include <TSmartPointer.h>
#include <HeapObj.h>
#include <IConnection.h>
#include <FrontierX/LegacyInput.h>
#include <cstdlib>
#endif

#include "MScroll.h"
#include "resource.h"
#include "Cursor.h"
#include "StatusBar.h"
#include "SysMap.h"
#include "IResource.h"
#include "Startup.h"
#include "TResClient.h"
#include "Hotkeys.h"
#include "DBHotkeys.h"

#include <TSmartPointer.h>
#include <EventSys.h>
#include <IConnection.h>
#include <HeapObj.h>
#include <TComponent.h>
#include <WindowManager.h>
#include <HKEvent.h>

#if defined(_WIN32)
#include <commctrl.h>
#endif

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

#define MSCROLL_PRIORITY      (RES_PRIORITY_HIGH-0x1000)
#define EDGE_WIDTH 1.5

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

struct DACOM_NO_VTABLE MScroll : public IMouseScroll, IEventCallback, ResourceClient<>
{
	BEGIN_DACOM_MAP_INBOUND(MScroll)
	DACOM_INTERFACE_ENTRY(IMouseScroll)
	DACOM_INTERFACE_ENTRY(IResourceClient)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	END_DACOM_MAP()

	BOOL32 bActive:1;
	BOOL32 bHasFocus:1;
	U32 eventHandle;
#if !defined(_WIN32)
	bool hasMousePosition;
#endif

	RECT inner, outer;
	S32 outerThirdX, outerThirdY;

 	//------------------------

	MScroll (void);

	~MScroll (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IMouseScroll methods */

	DEFMETHOD_(BOOL32,SetArea) (struct tagRECT *inner, struct tagRECT *outer);

	DEFMETHOD_(BOOL32,SetActive) (BOOL32 activate);	

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param = 0);

	/* MScroll methods */

	void update (void);

	IDAComponent * GetBase (void)
	{
		return (IMouseScroll *) this;
	}
};
//--------------------------------------------------------------------------//
//
MScroll::MScroll (void)
{
	// nothing to init so far?
	bHasFocus = 1;
	resPriority = MSCROLL_PRIORITY;
#if !defined(_WIN32)
	hasMousePosition = false;
#endif
}
//--------------------------------------------------------------------------//
//
MScroll::~MScroll (void)
{
	COMPTR<IDAConnectionPoint> connection;

	#if defined(_WIN32)
	if (GS && GS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
#else
	if (EVENTSYS && EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
#endif
		connection->Unadvise(eventHandle);
}
//--------------------------------------------------------------------------//
//
void MScroll::update (void)
{
	if (bActive)
	{
		S32 x, y;
		BOOL32 bInRegion=0;
		
		WM->GetCursorPos(x, y);

		//
		// detect when mouse is in one of the scrolling regions
		//

		bool bUp     = HOTKEY->GetHotkeyState(IDH_SCROLL_UP) || 
					   HOTKEY->GetHotkeyState(IDH_SCROLL_UPLEFT) || 
					   HOTKEY->GetHotkeyState(IDH_SCROLL_UPRIGHT)|| 
					   (DBHOTKEY && DBHOTKEY->GetHotkeyState(IDH_SCROLL_UP_DB));
		bool bDown   = HOTKEY->GetHotkeyState(IDH_SCROLL_DOWN) || 
					   HOTKEY->GetHotkeyState(IDH_SCROLL_DOWNLEFT) || 
					   HOTKEY->GetHotkeyState(IDH_SCROLL_DOWNRIGHT) || 
					   (DBHOTKEY && DBHOTKEY->GetHotkeyState(IDH_SCROLL_DOWN_DB));

		bool bLeft   = HOTKEY->GetHotkeyState(IDH_SCROLL_LEFT) ||
					   HOTKEY->GetHotkeyState(IDH_SCROLL_UPLEFT) ||
					   HOTKEY->GetHotkeyState(IDH_SCROLL_DOWNLEFT) ||
					   (DBHOTKEY && DBHOTKEY->GetHotkeyState(IDH_SCROLL_LEFT_DB));

		bool bRight  = HOTKEY->GetHotkeyState(IDH_SCROLL_RIGHT) ||
					   HOTKEY->GetHotkeyState(IDH_SCROLL_UPRIGHT) ||
					   HOTKEY->GetHotkeyState(IDH_SCROLL_DOWNRIGHT) ||
					   (DBHOTKEY && DBHOTKEY->GetHotkeyState(IDH_SCROLL_RIGHT_DB));

		if (bUp | bDown | bLeft | bRight)
			goto OK;

		if (x > inner.left && x < inner.right)
		{
			if (y > inner.top && y < inner.bottom)
				goto Done;
		}
		if (x < outer.left || x > outer.right)
			goto Done;
		if (y < outer.top || y > outer.bottom)
			goto Done;
		bInRegion=1;
OK:
		// we have a winner!

		desiredOwnedFlags = RF_CURSOR;

		if (grabAllResources())
		{
			if (bInRegion)
			{
				bUp = bDown = bRight = bLeft = 0;

				if (x <= outer.left+outerThirdX)
				{
					if (y <= outer.top+outerThirdY)
					{
						bUp = bLeft = true;
					}
					else
					if (y <= outer.bottom-outerThirdY)
					{
						bLeft = true;
					}
					else
					{
						bLeft = bDown = true;
					}
				}
				else
				if (x <= outer.right-outerThirdX)
				{
					if (y <= inner.top)
					{
						bUp = true;
					}
					else
					{
						bDown = true;
					}
				}
				else
				{
					if (y <= outer.top+outerThirdY)
					{
						bUp = bRight = true;
					}
					else
					if (y <= outer.bottom-outerThirdY)
					{
						bRight=  true;
					}
					else
					{
						bRight = bDown = true;
					}					   
				}
			}

			// begin hotkey input section
			if (bLeft)
			{
				if (bUp)
				{
					if (cursorID != IDC_CURSOR_UPLEFT)
					{
						cursorID = IDC_CURSOR_UPLEFT;
						setCursor();
					}
					SYSMAP->ScrollUp(0);
					SYSMAP->ScrollLeft(0);
				}
				else
				if (bDown)
				{
					if (cursorID != IDC_CURSOR_DOWNLEFT)
					{
						cursorID = IDC_CURSOR_DOWNLEFT;
						setCursor();
					}
					SYSMAP->ScrollLeft(0);
					SYSMAP->ScrollDown(0);
				}
				else
				{
					if (cursorID != IDC_CURSOR_LEFT)
					{
						cursorID = IDC_CURSOR_LEFT;
						setCursor();
					}
					SYSMAP->ScrollLeft(0);
				}
			}
			else
			if (bRight)
			{
				if (bUp)
				{
					if (cursorID != IDC_CURSOR_UPRIGHT)
					{
						cursorID = IDC_CURSOR_UPRIGHT;
						setCursor();
					}
					SYSMAP->ScrollUp(0);
					SYSMAP->ScrollRight(0);
				}
				else
				if (bDown)
				{
					if (cursorID != IDC_CURSOR_DOWNRIGHT)
					{
						cursorID = IDC_CURSOR_DOWNRIGHT;
						setCursor();
					}
					SYSMAP->ScrollRight(0);
					SYSMAP->ScrollDown(0);
				}					   
				else
				{
					if (cursorID != IDC_CURSOR_RIGHT)
					{
						cursorID = IDC_CURSOR_RIGHT;
						setCursor();
					}
					SYSMAP->ScrollRight(0);
				}
			}
			else
			{
				if (bUp)
				{
					if (cursorID != IDC_CURSOR_UP)
					{
						cursorID = IDC_CURSOR_UP;
						setCursor();
					}
					SYSMAP->ScrollUp(0);
				}
				else
				if (bDown)
				{
					if (cursorID != IDC_CURSOR_DOWN)
					{
						cursorID = IDC_CURSOR_DOWN;
						setCursor();
					}
					SYSMAP->ScrollDown(0);
				}
			}
		}
		return;
Done:	// not in a scroll position, so release resources
		desiredOwnedFlags = 0;
		releaseResources();
	}
}
//--------------------------------------------------------------------------//
//
BOOL32 MScroll::SetArea (struct tagRECT *_inner, struct tagRECT *_outer)
{
	inner = *_inner;
	outer = *_outer;

	outerThirdY = (outer.bottom - outer.top + 1) / 4;
	outerThirdX = (outer.right - outer.left + 1) / 4;

	return 1;
}
//--------------------------------------------------------------------------//
//
BOOL32 MScroll::SetActive (BOOL32 activate)
{
	BOOL32 result = bActive;

	if (bActive == 0 && activate)
	{
		bActive = 1;
	}
	else
	if (bActive && activate == 0)
	{
		bActive = 0;
		desiredOwnedFlags = 0;
		releaseResources();
	}

	return result;
}
//--------------------------------------------------------------------------//
// receive notifications from event system
//
GENRESULT MScroll::Notify (U32 message, void *param)
{
//	MSG *msg = (MSG *) param;

	switch (message)
	{
#if !defined(_WIN32)
	case FrontierX::LegacyMessage::MouseMove:
        if (!param || static_cast<const FrontierX::LegacyWindowMessage *>(param)->message != message)
            return GR_INVALID_PARMS;
		hasMousePosition = true;
		break;
	case CQE_MISSION_CLOSE:
		hasMousePosition = false;
		desiredOwnedFlags = 0;
		releaseResources();
		break;
#endif
	case CQE_INTERFACE_RES_CHANGE:
		{
			RECT inner, outer;
			inner.left  = IDEAL2REALX(0 + EDGE_WIDTH);
			inner.right = IDEAL2REALX(SCREEN_WIDTH - EDGE_WIDTH);
			inner.top   = IDEAL2REALY(0 + EDGE_WIDTH);
			inner.bottom= IDEAL2REALY(SCREEN_HEIGHT - EDGE_WIDTH);

			outer.left  = IDEAL2REALX(0);
			outer.right = IDEAL2REALX(SCREEN_WIDTH)-1;
			outer.top   = IDEAL2REALY(0);
			outer.bottom= IDEAL2REALY(SCREEN_HEIGHT)-1;

			SetArea(&inner, &outer);

			if (param == 0)		// going to 2D mode
			{
				desiredOwnedFlags = 0;
				releaseResources();
			}
		}
		break;
	
	case CQE_UPDATE:
		if (bHasFocus && CQFLAGS.bGameActive
#if !defined(_WIN32)
            && hasMousePosition
#endif
        )
			update();
		break;

	case CQE_KILL_FOCUS:
		desiredOwnedFlags = 0;
		releaseResources();
		bHasFocus = 0;
		break;

	case CQE_SET_FOCUS:
		bHasFocus = 1;
		break;
	}

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
#if !defined(_WIN32)
namespace FrontierX
{
bool CheckNativeMouseScroll()
{
    const auto active = CQFLAGS.bGameActive;
    auto *owner = new DAComponent<MScroll>;
    owner->SetActive(1);
    CQFLAGS.bGameActive = -1;
    owner->Notify(CQE_UPDATE, nullptr); // No SDL position: must not scroll at cached (0,0).
    bool valid = !owner->hasMousePosition && !owner->actualOwnedFlags &&
        !owner->desiredOwnedFlags && owner->resPriority == MSCROLL_PRIORITY;
    FrontierX::LegacyWindowMessage motion{};
    motion.message = FrontierX::LegacyMessage::MouseMove;
    valid = valid && owner->Notify(motion.message, nullptr) == GR_INVALID_PARMS;
    owner->Notify(motion.message, &motion);
    valid = valid && owner->hasMousePosition;
    owner->Notify(CQE_MISSION_CLOSE, nullptr);
    valid = valid && !owner->hasMousePosition && owner->bActive &&
        !owner->actualOwnedFlags && !owner->desiredOwnedFlags;
    owner->Notify(CQE_UPDATE, nullptr); // Reload must still wait for fresh mouse input.
    valid = valid && owner->bActive && !owner->actualOwnedFlags &&
        !owner->desiredOwnedFlags;
    CQFLAGS.bGameActive = active;
    owner->Release();
    return valid;
}
}
#endif

struct _mscroll : GlobalComponent
{
	MScroll * mscroll;

	virtual void Startup (void)
	{
		MSCROLL = mscroll = new DAComponent<MScroll>;
		AddToGlobalCleanupList((IDAComponent **) &MSCROLL);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;
		RECT inner, outer;

		inner.left  = 0 + EDGE_WIDTH;
		inner.right = SCREEN_WIDTH - EDGE_WIDTH;
		inner.top   = 0 + EDGE_WIDTH;
		inner.bottom= SCREEN_HEIGHT - EDGE_WIDTH;

		outer.left  = 0;
		outer.right = SCREEN_WIDTH-1;
		outer.top   = 0;
		outer.bottom= SCREEN_HEIGHT-1;

		MSCROLL->SetArea(&inner, &outer);

#if defined(_WIN32)
		if (GS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Advise(MSCROLL, &mscroll->eventHandle);
#else
		if (!EVENTSYS || EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
			connection->Advise(MSCROLL, &mscroll->eventHandle) != GR_OK)
			std::abort();
#endif

		mscroll->initializeResources();
	}
};

static _mscroll mscroll;

//--------------------------------------------------------------------------//







//--------------------------------------------------------------------------//
//----------------------------End MScroll.cpp-------------------------------//
//--------------------------------------------------------------------------//
