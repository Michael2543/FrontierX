//--------------------------------------------------------------------------//
//                                                                          //
//                      ScrollingText.cpp                                   //
//                                                                          //
//               COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.                  //
//																			//
//	NOTE:  Ignore stupid stl warnings                                       //
//--------------------------------------------------------------------------//
/*
   $Author: Jasony $

	Control that scrolls informative text to the user			

*/
//--------------------------------------------------------------------------//
 
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppFont.h"
#include "Include/NativeAppTextResources.h"
#include "CQTrace.h"
#include "IObject.h"
#include "ObjList.h"
#include <MGlobals.h>
#include <VFX.H>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cstdint>
#include <limits>
#include <vector>
#include <string>
using std::vector;
static std::uint64_t scrollingDraws = 0;
#endif

#include <wchar.h>

#include "resource.h"
#include "TResource.h"
#include "ScrollingText.h"
#if defined(_WIN32)
#include "BaseHotRect.h"
#endif
#include "EventPriority.h"
#include "DrawAgent.h"
#include "Startup.h"
#include "UserDefaults.h"
#if defined(_WIN32)
#include "VideoSurface.h"
#endif
#if defined(_WIN32)
#include "CQGame.h"
#endif
#include "SoundManager.h"
#include "SFX.h"
#if defined(_WIN32)
#include "IBanker.h"
#endif

#if defined(_WIN32)
#include "frame.h"
#endif
#include "Hotkeys.h"

#include <TComponent.h>
#include <IConnection.h>
#include <TConnContainer.h>
#include <TConnPoint.h>
#include <HeapObj.h>
#include <TSmartPointer.h>
#include <EventSys.h>
#include <IConnection.h>
#if defined(_WIN32)
#include <commctrl.h>
#endif

#include "CommPacket.h"
#if defined(_WIN32)
#include "NetPacket.h"
#endif
#if defined(_WIN32)
#include "MGlobals.h"
#endif
#if defined(_WIN32)
#include "Sector.h"
#endif
#if defined(_WIN32)
#include "ObjList.h"
#endif
#if defined(_WIN32)
#include "IAttack.h"
#endif

#pragma warning (disable : 4018 4245 4530 4663)

#if defined(_WIN32)
#include <da_vector>
using namespace da_std;
#endif

#if defined(_WIN32)
Frame * __stdcall CreateMenuChat (bool bChatAll);
Frame * __stdcall CreatePlayerMenu (void);
Frame * __stdcall CreateMenuDiplomacy (void);
Frame * __stdcall CreateMenuPlayerChat (void);
Frame * __stdcall CreateMenuObjectives (void);
#endif


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
typedef struct tagTextEntity
{
	DWORD lifetime;
	DWORD textID;
	U32 colorID;
	wchar_t name[MAX_NAME_LENGTH];
	wchar_t text[MAX_CHAT_LENGTH];
} TextEntity;


//--------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE ScrollingText : public IEventCallback, IScrollingText
{
	BEGIN_DACOM_MAP_INBOUND(ScrollingText)
	DACOM_INTERFACE_ENTRY(IScrollingText)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	END_DACOM_MAP()

	typedef vector<TextEntity> TE_VECTOR;

	COMPTR<IFontDrawAgent> fontAgentText[MAX_LINES_TEXT];
	COMPTR<IFontDrawAgent> fontAgentPlayer[MAX_LINES_TEXT];

	U32 handle;		// connection handle
	U32 dwHeight;		// height of the control, in pixels

	U32 dwTextID[MAX_LINES_TEXT];

	BOOL32 bInMenuLoop;
	S32 lastFontWidth;
	U32 toolbarHeight;
	bool bHasFocus;

#if defined(_WIN32)
	// our chat dlg pointer
	Frame * pChatDlg;
	Frame * pPlayerMenu;
	Frame * pDiplomacyDlg;
	Frame * pChatFilterDlg;
	Frame * pObjectivesDlg;
#else
    bool skipNotificationCue = false;
#endif

	int y_positionChat;
	int bottomAdmiralEdge;

	// our notification sound, letting us know a message is up to me read
	HSOUND hsndMessage;

	// the array of 'text entities', they expire after a predefined amount of time
	TE_VECTOR teVector;

	ScrollingText (void);

	~ScrollingText (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IScrollingText methods */

	DEFMETHOD_(void,Redraw) (void);

	DEFMETHOD(SetText) (U32 dwResourceID);

	DEFMETHOD(SetTextString) (const wchar_t *string);

	DEFMETHOD(SetTextStringEx) (const wchar_t *string, U32 playerID, U32 dplayID = 0);

	DEFMETHOD_(U32,GetText) (void);

	DEFMETHOD_(U32,GetHeight) (void);

	DEFMETHOD_(void,SetToolbarHeight) (U32 height);

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param = 0);

	/* ScrollingText methods */

	void reloadFonts (bool bLoad);

	void unDraw (void);

	void OnNoOwner (void)
	{
		SetText(0);		// empty text for now
	}

	IDAComponent * GetBase (void)
	{
		return (IScrollingText *) this;
	}

	void onUpdate (U32 dt)			// dt is milliseconds
	{
		U32 i;
		// _todo_
		for (i = 0; i < teVector.size(); i++)
		{
			teVector[i].lifetime += dt;

			if (teVector[i].lifetime > TEXT_LIFETIME)
			{
				teVector.erase(teVector.begin()+i);
			}
		}

	}

#if defined(_WIN32)
	void addStringToPlayerChat (U32 dwResourceID);

	void addStringToPlayerChat (const wchar_t * string);

	void addStringToPlayerChat (const wchar_t * string, const U32 playerID, const U32 dplayID);

	void deleteChatMenu (void)
	{
		if (pChatDlg)
		{
			pChatDlg->parent->PostMessage(CQE_DELETE_HOTRECT, static_cast<BaseHotRect *>(pChatDlg));
			pChatDlg = NULL;
		}
	}

	void deletePlayerMenu (void)
	{
		if (pPlayerMenu)
		{
			pPlayerMenu->parent->PostMessage(CQE_DELETE_HOTRECT, static_cast<BaseHotRect *>(pPlayerMenu));
			pPlayerMenu = NULL;
		}
	}

	void deleteDiplomacyMenu (void)
	{
		if (pDiplomacyDlg)
		{
			pDiplomacyDlg->parent->PostMessage(CQE_DELETE_HOTRECT, static_cast<BaseHotRect *>(pDiplomacyDlg));
			pDiplomacyDlg = NULL;
//			EVENTSYS->Send(CQE_SET_FOCUS, 0);
		}
	}

	void deleteChatFilterMenu (void)
	{
		if (pChatFilterDlg)
		{
			pChatFilterDlg->parent->PostMessage(CQE_DELETE_HOTRECT, static_cast<BaseHotRect *>(pChatFilterDlg));
			pChatFilterDlg = NULL;
//			EVENTSYS->Send(CQE_SET_FOCUS, 0);
		}
	}

	void deleteObjectivesMenu (void)
	{
		if (pObjectivesDlg)
		{
			pObjectivesDlg->parent->PostMessage(CQE_DELETE_HOTRECT, static_cast<BaseHotRect *>(pObjectivesDlg));
			pObjectivesDlg = NULL;
		}
	}

	void handleAlliancePacket (ALLIANCE_PACKET * ap);
#endif
};
//--------------------------------------------------------------------------//
//
ScrollingText::ScrollingText (void)
{
	bHasFocus = 1;
}
//--------------------------------------------------------------------------//
//
ScrollingText::~ScrollingText (void)
{
#if defined(_WIN32)
	if (FULLSCREEN)
	{
		COMPTR<IDAConnectionPoint> connection;
		
		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Unadvise(handle);
	}
#else
    if (EVENTSYS && handle)
    {
        COMPTR<IDAConnectionPoint> connection;
        if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
            connection->Unadvise(handle);
    }
#endif

	// close the  sound handles
	if (SFXMANAGER && hsndMessage)
	{
		SFXMANAGER->CloseHandle(hsndMessage);
		hsndMessage = NULL;
	}
}
//--------------------------------------------------------------------------//
//
void ScrollingText::Redraw (void)
{
	//
	// NOTE:
	// might not want to lock the surface during the menu loop because of WIN16 trouble,
	// especially now that we are use GDI to draw the fonts
	//
	if (CQFLAGS.bGameActive)
	{
#if !defined(_WIN32)
        if (!fontAgentText[0]) reloadFonts(true);
        const auto drawsBefore = FrontierX::NativeFontDrawCount();
#endif
		y_positionChat = bottomAdmiralEdge + 6;
		
		S32 font_height = fontAgentText[0]->GetFontHeight();
		bool bFrameLocked = (CQFLAGS.bFrameLockEnabled != 0);

#if defined(_WIN32)
		if (bFrameLocked)
		{
			if (SURFACE->Lock() == false)
			{
				return;
			}
		}
#else
        CQASSERT(!bFrameLocked);
#endif

		S32 y = font_height;
		S32 w;

		PANE pane;
		pane.window = 0;
		
		for (U32 i = 0; i < teVector.size(); i++)
		{
			pane.x0 = IDEAL2REALX(XPOS_TEXT);
			pane.x1 = IDEAL2REALX(600);
			pane.y0 = y;
			pane.y1 = y + IDEAL2REALY(40);

			if (teVector[i].textID)
			{
				fontAgentText[i]->StringDraw(&pane, 0, 0, teVector[i].textID);
				w = fontAgentText[i]->GetStringWidth(teVector[i].textID);
			}
			else
			{
				w = fontAgentText[i]->GetStringWidth(teVector[i].name);
				fontAgentPlayer[i]->SetFontColor(COLORTABLE[teVector[i].colorID] | 0xFF000000, 0x0);

				fontAgentPlayer[i]->StringDraw(&pane, 0, 0, teVector[i].name);

				pane.x0 += w;
				fontAgentText[i]->StringDraw(&pane, 0, 0, teVector[i].text);
				
				w = fontAgentText[i]->GetStringWidth(teVector[i].text);
			}

			int multiply = (w > pane.x1 - pane.x0) ? 2 : 1;
			y_positionChat = y + (font_height + 6) * multiply;
			y += (font_height + 6) * multiply;
		}

#if defined(_WIN32)
		if (pChatDlg)
		{
			pChatDlg->PostMessage(CQE_CHATDLG_SETYPOS, (void*)y_positionChat);
		}
#else
        scrollingDraws += FrontierX::NativeFontDrawCount() - drawsBefore;
#endif

#if defined(_WIN32)
		if (bFrameLocked)
		{
			SURFACE->Unlock();
		}
#endif
	}
}
//--------------------------------------------------------------------------//
// undraw the previous text (this operation is slow)
void ScrollingText::unDraw (void)
{
	if (CQFLAGS.bGameActive)
	{
		S32 font_height = fontAgentText[0]->GetFontHeight();
		bool bFrameLocked=(CQFLAGS.bFrameLockEnabled!=0);

#if defined(_WIN32)
		if (bFrameLocked)
		{
			if (SURFACE->Lock() == false)
			{
				return;
			}
		}
#else
        CQASSERT(!bFrameLocked);
#endif

		// _todo_
		for (U32 i = 0; i < teVector.size(); i++)
		{
			S32 y = font_height * i;
			S32 w;
			
			if (teVector[i].textID)
			{
				w = fontAgentText[i]->GetStringWidth(teVector[i].textID);
			}
			else
			{
				w = fontAgentText[i]->GetStringWidth(teVector[i].text);
			}

			DA::RectangleFill(0, 10, y, w, y + font_height, 0);
		}

#if defined(_WIN32)
		if (bFrameLocked)
			SURFACE->Unlock();
#endif
	}
}
//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
void ScrollingText::addStringToPlayerChat (U32 dwResourceID)
{
	wchar_t buffer[256];
	wcsncpy(buffer, _localLoadStringW(dwResourceID), CHARCOUNT(buffer));
	addStringToPlayerChat(buffer);
}
//--------------------------------------------------------------------------//
//
void ScrollingText::addStringToPlayerChat (const wchar_t * string)
{
	CQASSERT(pChatFilterDlg && "Chat Menu is not active");

	COMPTR<IPlayerChat> playerChat;
	pChatFilterDlg->GetBase()->QueryInterface("IPlayerChat", playerChat);
	CQASSERT(playerChat != NULL);

	playerChat->AddTextString(string);
}
//--------------------------------------------------------------------------//
//
void ScrollingText::addStringToPlayerChat (const wchar_t * string, const U32 playerID, const U32 dplayID)
{
	CQASSERT(pChatFilterDlg && "Chat Menu is not active");

	COMPTR<IPlayerChat> playerChat;
	pChatFilterDlg->GetBase()->QueryInterface("IPlayerChat", playerChat);
	CQASSERT(playerChat != NULL);

	playerChat->AddTextString(string, playerID, dplayID);
}
//--------------------------------------------------------------------------//
//
#endif
U32 ScrollingText::GetText (void)
{
	return dwTextID[0];
}
//--------------------------------------------------------------------------//
//
GENRESULT ScrollingText::SetText (U32 dwResourceID)
{
	if (!dwResourceID)
	{
		return GR_INVALID_PARMS;
	}
#if defined(_WIN32)
	if (pChatFilterDlg)
	{
		// display the text in the player chat menu
		addStringToPlayerChat(dwResourceID);
		return GR_OK;
	}
#endif

	TextEntity te;
	memset(&te, 0, sizeof(TextEntity));
	te.lifetime = 0;
	te.textID = dwResourceID;

	// _todo_
	teVector.push_back(te);

	// get rid of any extra instances of a text entity
	if (teVector.size() > MAX_LINES_TEXT)
	{
		teVector.erase(teVector.begin());
	}

	// play the sound

#if !defined(_WIN32)
        if (!skipNotificationCue)
        {
            CQASSERT(SFXMANAGER);
#endif
	if (hsndMessage == NULL && SFXMANAGER)
	{
		// load the sounds
//		hsndMessage = SFXMANAGER->Open(SFX::TELETYPE);
		hsndMessage = SFXMANAGER->Open(SFX::BEACON);
	}
	SFXMANAGER->Play(hsndMessage, 0, 0);
#if !defined(_WIN32)
        }
#endif

	if (bInMenuLoop && CQFLAGS.bFrameLockEnabled)
	{
		unDraw();
		Redraw();
#if defined(_WIN32)
		InvalidateRect(hMainWindow, 0, 0);
#endif
	}
	
	return GR_OK;
}
//--------------------------------------------------------------------------//
//	 set our primary string and remember the last couple of strings
//
GENRESULT ScrollingText::SetTextString (const wchar_t *string)
{
	if (string && *string)
	{
#if defined(_WIN32)
		if (pChatFilterDlg)
		{
			// display the text in the player chat menu
			addStringToPlayerChat(string);
			return GR_OK;
		}
#endif

		TextEntity te;
		memset(&te, 0, sizeof(TextEntity));
		te.lifetime = 0;
		te.textID = 0;
		te.colorID = 0;
		te.name[0] = 0;

		wcsncpy(te.text, string, CHARCOUNT(te.text));
#if !defined(_WIN32)
        te.text[CHARCOUNT(te.text)-1] = 0;
#endif

		// _todo_
		teVector.push_back(te);

		// get rid of any extra instances of a text entity
		if (teVector.size() > MAX_LINES_TEXT)
		{
			teVector.erase(teVector.begin());
		}

		// play the sound

#if !defined(_WIN32)
        if (!skipNotificationCue)
        {
            CQASSERT(SFXMANAGER);
#endif
		if (hsndMessage == NULL && SFXMANAGER)
		{
			// load the sounds
//			hsndMessage = SFXMANAGER->Open(SFX::TELETYPE);
			hsndMessage = SFXMANAGER->Open(SFX::BEACON);
		}
		SFXMANAGER->Play(hsndMessage, 0, 0);
#if !defined(_WIN32)
        }
#endif

	}
	return GR_OK;
}
//--------------------------------------------------------------------------//
//	 set our primary string and remember the last couple of strings
//
GENRESULT ScrollingText::SetTextStringEx (const wchar_t *string, U32 playerID, U32 dplayID)
{
#if defined(_WIN32)
	if (string && *string)
	{
		if (pChatFilterDlg)
		{
			// display the text in the player chat menu
			addStringToPlayerChat(string, playerID, dplayID);
			return GR_OK;
		}

		// get the slotID for this player
		U32 slotIDs[8];
		U32 nSlots;
		
		nSlots = MGlobals::GetSlotIDForPlayerID(playerID, slotIDs); 

		TextEntity te;
		memset(&te, 0, sizeof(TextEntity));
		te.colorID = MGlobals::GetColorID(playerID);
		te.lifetime = 0;
		te.textID = 0;

		// get the player's name
		if (nSlots > 1 && dplayID != 0)
		{
			MGlobals::GetPlayerNameFromDPID(dplayID, te.name, (MAX_NAME_LENGTH-3)*sizeof(wchar_t));
		}
		else
		{
			MGlobals::GetPlayerNameBySlot(slotIDs[0], te.name, (MAX_NAME_LENGTH-3)*sizeof(wchar_t));
		}

		// add a ': ' to the end of the player name
		U32 sName = wcslen(te.name);
		te.name[sName++] = ':';
		te.name[sName++] = ' ';
		te.name[sName++] = '\0';

		wcsncpy(te.text, string, CHARCOUNT(te.text));

		// add the text entity to the list
		teVector.push_back(te);

		// get rid of any extra instances of a text entity
		if (teVector.size() > MAX_LINES_TEXT)
		{
			teVector.erase(teVector.begin());
		}

		// play the sound
		if (hsndMessage == NULL && SFXMANAGER)
		{
			// load the sounds
//			hsndMessage = SFXMANAGER->Open(SFX::TELETYPE);
			hsndMessage = SFXMANAGER->Open(SFX::BEACON);
		}
		SFXMANAGER->Play(hsndMessage, 0, 0);

	}
	return GR_OK;

#else
    (void)string; (void)playerID; (void)dplayID;
    std::fputs("Native attributed chat is not part of the single-player text closure\n", stderr);
    return GR_GENERIC;
#endif
}
//--------------------------------------------------------------------------//
//
U32 ScrollingText::GetHeight (void)
{
	return dwHeight;
}
//--------------------------------------------------------------------------//
//
void ScrollingText::SetToolbarHeight (U32 height)
{
	toolbarHeight = height;
}
//--------------------------------------------------------------------------//
// receive notifications from event system
//
GENRESULT ScrollingText::Notify (U32 message, void *param)
{
	switch (message)
	{
	case CQE_START3DMODE:
		reloadFonts(true);
		break;
	case CQE_END3DMODE:
		reloadFonts(false);
		break;

#if defined(_WIN32)
	case CQE_ADMIRALBAR_ACTIVE:
		bottomAdmiralEdge = (int)param;
		break;

	case WM_ENTERMENULOOP:
		bInMenuLoop=1;
		break;
	case WM_EXITMENULOOP:
		bInMenuLoop=0;
		break;
#endif
	case CQE_ENDFRAME:
		Redraw();
		break;

#if defined(_WIN32)
	case CQE_MISSION_ENDING:
	case CQE_GAME_ACTIVE:
		deletePlayerMenu();
		deleteChatMenu();
		deleteDiplomacyMenu();
		deleteChatFilterMenu();
		deleteObjectivesMenu();
		break;

	case CQE_DELETE_CHAT_MENU:
		deleteChatMenu();
		break;

	case CQE_DELETE_PLAYER_MENU:
		deletePlayerMenu();
		break;

	case CQE_DELETE_DIPLOMACY_MENU:
		deleteDiplomacyMenu();
		break;

	case CQE_DELETE_CHATFILTER_MENU:
		deleteChatFilterMenu();
		break;

	case CQE_DELETE_OBJECTIVES_MENU:
		deleteObjectivesMenu();
		break;

	case CQE_HOTKEY:
		if (bHasFocus)
		switch ((U32) param)
		{
		case IDH_CHATALL:
			if (DEFAULTS->GetDefaults()->bSpectatorModeOn == false && pChatDlg == NULL && CQFLAGS.bGameActive)
			{
				pChatDlg = CreateMenuChat(true);
				pChatDlg->PostMessage(CQE_CHATDLG_SETYPOS, (void*)y_positionChat);
			}
			break;

		case IDH_CHATTEAM:
			if (DEFAULTS->GetDefaults()->bSpectatorModeOn == false && pChatDlg == NULL && CQFLAGS.bGameActive)
			{
				pChatDlg = CreateMenuChat(false);
				pChatDlg->PostMessage(CQE_CHATDLG_SETYPOS, (void*)y_positionChat);
			}
			break;

		case IDH_CHAT_MENU:
			if (DEFAULTS->GetDefaults()->bSpectatorModeOn == false && pChatFilterDlg == NULL && CQFLAGS.bGameActive && MGlobals::GetGameSettings().gameType != CQGAMETYPES::MISSION_DEFINED)
			{
				pChatFilterDlg = CreateMenuPlayerChat();
			}
			break;
			
		case IDH_DIPLOMACY:
			if ((pDiplomacyDlg == NULL) && CQFLAGS.bGameActive && DEFAULTS->GetDefaults()->bSpectatorModeOn == false && MGlobals::GetGameSettings().gameType != CQGAMETYPES::MISSION_DEFINED )
			{
				pDiplomacyDlg = CreateMenuDiplomacy();
			}
			break;

		case IDH_MULTIPLAYER_SCORE:
			if (pPlayerMenu == NULL && CQFLAGS.bGameActive && MGlobals::GetGameSettings().gameType != CQGAMETYPES::MISSION_DEFINED) // PLAYERID != 0)
			{
				pPlayerMenu = CreatePlayerMenu();	
			}
			else if (MGlobals::GetGameSettings().gameType != CQGAMETYPES::MISSION_DEFINED)
			{
				deletePlayerMenu();
			}
			break;

		case IDH_MISSION_OBJ:
			if (pObjectivesDlg == NULL && CQFLAGS.bGameActive && MGlobals::GetGameSettings().gameType == CQGAMETYPES::MISSION_DEFINED) // PLAYERID != 0)
			{
				pObjectivesDlg = CreateMenuObjectives();	
			}
			else if (MGlobals::GetGameSettings().gameType == CQGAMETYPES::MISSION_DEFINED)
			{
				deleteObjectivesMenu();
			}
			break;
		}
		break;

#else
    case CQE_MISSION_CLOSE:
        teVector.clear();
        break;
    case CQE_HOTKEY:
        switch (static_cast<U32>(reinterpret_cast<std::uintptr_t>(param)))
        {
        case IDH_CHATALL: case IDH_CHATTEAM: case IDH_CHAT_MENU:
        case IDH_DIPLOMACY: case IDH_MULTIPLAYER_SCORE: case IDH_MISSION_OBJ:
            std::fputs("Native scrolling-text menus are not ported\n", stderr);
            return GR_GENERIC;
        }
        break;
#endif
	case CQE_KILL_FOCUS:
		bHasFocus = 0;
		break;

	case CQE_SET_FOCUS:
		bHasFocus = 1;
		break;

#if defined(_WIN32)
	case CQE_NETPACKET:
		if (CQFLAGS.bGameActive)
		{
			BASE_PACKET * packet = (BASE_PACKET *) param;

			if (packet->type == PT_NETTEXT)
			{
				NETTEXT * nt = (NETTEXT*) packet;

				U32 playerMask = 1 << (MGlobals::GetThisPlayer() - 1);

				if (nt->toID == 0xff || (nt->toID & playerMask))
				{
					// if the player chat menu is up, then put the text there, otherwise display the chat text
					// with the scrolling text stuff
					SetTextStringEx(nt->chatText, nt->playerID, nt->fromID);
				}
			}
			else if (packet->type == PT_ALLIANCEFLAGS)
			{
				handleAlliancePacket((ALLIANCE_PACKET*)packet);
			}
			else if (packet->type == PT_GIFTCREW)
			{
				GIFTCREW_PACKET * gp = (GIFTCREW_PACKET*) packet;

				//check to make sure we can realy gift this
				U32 currentValue = MGlobals::GetCurrentCrew(gp->giverID);
				if(currentValue < gp->amount)
					gp->amount = currentValue;//gave too much
				MGlobals::SetCurrentCrew(gp->giverID, currentValue - gp->amount);

				BANKER->AddCrew(gp->recieveID,gp->amount);
			}
			else if (packet->type == PT_GIFTORE)
			{
				GIFTORE_PACKET * gp = (GIFTORE_PACKET*) packet;
				U32 currentValue = MGlobals::GetCurrentMetal(gp->giverID);
				if(currentValue < gp->amount)
					gp->amount = currentValue;
				MGlobals::SetCurrentMetal(gp->giverID, currentValue - gp->amount);

				BANKER->AddMetal(gp->recieveID,gp->amount);
			}
			else if (packet->type == PT_GIFTGAS)
			{
				GIFTGAS_PACKET * gp = (GIFTGAS_PACKET*) packet;

				U32 currentValue = MGlobals::GetCurrentGas(gp->giverID);
				if(currentValue < gp->amount)
					gp->amount = currentValue;
				MGlobals::SetCurrentGas(gp->giverID, currentValue - gp->amount);

				BANKER->AddGas(gp->recieveID,gp->amount);
			}
		}
		break;

#endif
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
#if defined(_WIN32)
void ScrollingText::handleAlliancePacket (ALLIANCE_PACKET * ap)
{
	const U32 oldOneWayMask = MGlobals::GetOneWayAllyMask(ap->playerID);

	U32 i;
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		U32 playID = i+1;

		MGlobals::SetAlly(ap->playerID, playID, (ap->allianceFlags & (1 << i)) != 0);
		SECTOR->ComputeSupplyForAllPlayers();
	}

	const U32 allyMask = MGlobals::GetAllyMask(ap->playerID);

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		U32 id = i;
		
		// only broadcast the message if there is a new alliance (2 ways)
		if (id+1 != ap->playerID)
		{
			if ((1 << id) & allyMask)
			{
				OBJLIST->BroadcastAllianceForPlayers(ap->playerID, id+1);
			}
		}
	}

	// if someone has changed their alliance to you, than send a message
	const U32 thisID = MGlobals::GetThisPlayer() - 1;
	const U32 newOneWayMask = MGlobals::GetOneWayAllyMask(ap->playerID);

	if ((oldOneWayMask & (1 << thisID)) != (newOneWayMask & (1 << thisID)))
	{
		wchar_t buffer[128];
		wchar_t name[64];
		
		memset(name, 0, sizeof(name));
		memset(buffer, 0, sizeof(buffer));

		U32 enemySlot;
		U32 slots[MAX_PLAYERS];
		
		MGlobals::GetSlotIDForPlayerID(ap->playerID, slots); 
		enemySlot = slots[0]; 
		MGlobals::GetPlayerNameBySlot(enemySlot, name, sizeof(name));

		if (name[0])
		{
			// are we friend of foe now?
			if (allyMask & (1 << thisID))
			{
				swprintf(buffer, L"%s %s", name, _localLoadStringW(IDS_MESSAGE_ALLY_BOTH)); 
				SCROLLTEXT->SetTextString(buffer);
			}
			else if (newOneWayMask & (1 << thisID))
			{
				// the other guy wishes to be friends
				swprintf(buffer, L"%s %s", name, _localLoadStringW(IDS_MESSAGE_ALLY_FRIEND)); 
				SCROLLTEXT->SetTextString(buffer);
			}
			else
			{
				// the other guy doesn't care for you at all
				swprintf(buffer, L"%s %s", name, _localLoadStringW(IDS_MESSAGE_ALLY_ENEMY));
				SCROLLTEXT->SetTextString(buffer);
			}
		}
	}
}
#endif
//--------------------------------------------------------------------------//
//
void ScrollingText::reloadFonts (bool bLoad)
{
	int i;

	if (bLoad)
	{
#if defined(_WIN32)
		HFONT hFont;
#endif
		COLORREF pen, background;

		pen			= RGB(255,255,255)   | 0xFF000000;		
		background	= RGB(0,0,0);	   //| 0xFF000000;		// black	(background color)
#if defined(_WIN32)
		hFont = CQCreateFont(IDS_TOOLBAR_MONEY_FONT);

		CreateMultilineFontDrawAgent(0, hFont, pen, background, fontAgentText[0]);
//		CreateFontDrawAgent(hFont, 1, pen, background, fontAgentText[0]);
		CreateFontDrawAgent(hFont, 0, pen, background, fontAgentPlayer[0]);
#else
        *static_cast<IFontDrawAgent **>(fontAgentText[0]) = FrontierX::CreateNativeFontDrawAgent(IDS_TOOLBAR_MONEY_FONT, pen, true);
        *static_cast<IFontDrawAgent **>(fontAgentPlayer[0]) = FrontierX::CreateNativeFontDrawAgent(IDS_TOOLBAR_MONEY_FONT, pen);
        CQASSERT(fontAgentText[0] && fontAgentPlayer[0]);
#endif
		
		for (i = 1; i < MAX_LINES_TEXT; i++)
		{
			fontAgentText[0]->CreateDuplicate(fontAgentText[i]);
		}
		for (i = 1; i < MAX_LINES_TEXT; i++)
		{
			fontAgentPlayer[0]->CreateDuplicate(fontAgentPlayer[i]);
		}

	}
	else
	{
		for (i = 0; i < MAX_LINES_TEXT; i++)
		{
			fontAgentText[i].free();
			fontAgentPlayer[i].free();
		}
	}
}


//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
struct _scrollingText : GlobalComponent
{
	ScrollingText * STEXT;
	
	virtual void Startup (void)
	{
		SCROLLTEXT = STEXT = new DAComponent<ScrollingText>;
		AddToGlobalCleanupList((IDAComponent **) &SCROLLTEXT);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;
	
		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
		{
			connection->Advise(STEXT->GetBase(), &STEXT->handle);
			FULLSCREEN->SetCallbackPriority(STEXT, EVENT_PRIORITY_TEXTCHAT);
		}
	
		STEXT->OnNoOwner();
	}
};

static _scrollingText startup;
#else
namespace FrontierX
{
std::uint64_t NativeScrollingTextDrawCount() { return scrollingDraws; }
IScrollingText *CreateNativeSourceScrollingText(bool skipNotificationCue)
{
    if (!EVENTSYS) return nullptr;
    auto *owner = new DAComponent<ScrollingText>;
    owner->skipNotificationCue = skipNotificationCue;
    COMPTR<IDAConnectionPoint> connection;
    if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
        connection->Advise(owner->GetBase(), &owner->handle) != GR_OK)
    { owner->Release(); return nullptr; }
    return owner;
}
// Original capacity/strict expiry/erase order were absent from terminal-only
// messages. This check enables the selected single-player production owner.
bool CheckNativeSourceScrollingText()
{
    const unsigned fonts = NativeFontLiveCount();
    auto *interface = CreateNativeSourceScrollingText(true);
    if (!interface) return false;
    auto *owner = static_cast<ScrollingText *>(interface);
    bool valid = owner->SetText(0) == GR_INVALID_PARMS;
    for (unsigned i = 0; i < MAX_LINES_TEXT+1; ++i) owner->SetText(IDS_DISABLED);
    valid = valid && owner->teVector.size() == MAX_LINES_TEXT;
    owner->onUpdate(TEXT_LIFETIME);
    valid = valid && owner->teVector.size() == MAX_LINES_TEXT;
    owner->onUpdate(1);
    valid = valid && owner->teVector.size() == 2; // Source erase advances past the next entry.
    owner->onUpdate(0);
    valid = valid && owner->teVector.size() == 2 && owner->teVector[0].lifetime == TEXT_LIFETIME;
    owner->onUpdate(1); owner->onUpdate(1);
    valid = valid && owner->teVector.empty();
    std::wstring longText(MAX_CHAT_LENGTH+1, L'A');
    owner->SetTextString(longText.c_str());
    valid = valid && owner->teVector.size() == 1 &&
        std::wcslen(owner->teVector[0].text) == MAX_CHAT_LENGTH-1;
    owner->reloadFonts(true);
    valid = valid && NativeFontLiveCount() == fonts + 2*MAX_LINES_TEXT;
    owner->Notify(CQE_MISSION_CLOSE);
    valid = valid && owner->teVector.empty();
    owner->Notify(CQE_END3DMODE);
    interface->Release();
    return valid && NativeFontLiveCount() == fonts;
}
}
#endif

//--------------------------------------------------------------------------//
//--------------------------End ScrollingText.cpp---------------------------//
//--------------------------------------------------------------------------//