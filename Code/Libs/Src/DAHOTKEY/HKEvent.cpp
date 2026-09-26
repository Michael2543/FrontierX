//--------------------------------------------------------------------------//
//                                                                          //
//                               HKEvent.cpp                                //
//                                                                          //
//                  COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Logfile: /Libs/dev/Src/DAHotkey/HKEvent.cpp $

    $Revision: 8 $

    $Date: 3/21/00 4:30p $

    $Author: Pbleisch $
*/			    
//--------------------------------------------------------------------------//

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else
#include "FrontierX/LegacyInput.h"
#include "FrontierX/LegacyJoystick.h"
#endif

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <limits>

#include "HKEvent.h"
#include "Hotkey.h"
#include "HkeyRec.h"
#include "EventSys.h"
#include "Filesys.h"
#include "TComponent.h"
#include "TConnContainer.h"
#include "TConnPoint.h"
#include "da_heap_utility.h"

#if !defined(_WIN32)
using BYTE = U8;
using BOOL = BOOL32;
using HWND = FrontierX::LegacyWindowToken;
using UINT = FrontierX::LegacyMessageId;
using WPARAM = FrontierX::LegacyWParam;
using LPARAM = FrontierX::LegacyLParam;
using LRESULT = std::intptr_t;
using JOYINFOEX = FrontierX::LegacyJoystickState;

inline constexpr U32 VK_SHIFT = FrontierX::LegacyVirtualKey::Shift;
inline constexpr U32 VK_CONTROL = FrontierX::LegacyVirtualKey::Control;
inline constexpr U32 VK_MENU = FrontierX::LegacyVirtualKey::Menu;
inline constexpr U32 VK_CAPITAL = FrontierX::LegacyVirtualKey::Capital;
inline constexpr U32 VK_NUMLOCK = FrontierX::LegacyVirtualKey::NumLock;
inline constexpr U32 VK_SCROLL = FrontierX::LegacyVirtualKey::Scroll;
inline constexpr U32 VK_LSHIFT = FrontierX::LegacyVirtualKey::LeftShift;
inline constexpr U32 VK_RSHIFT = FrontierX::LegacyVirtualKey::RightShift;
inline constexpr U32 VK_LCONTROL = FrontierX::LegacyVirtualKey::LeftControl;
inline constexpr U32 VK_RCONTROL = FrontierX::LegacyVirtualKey::RightControl;
inline constexpr U32 VK_LMENU = FrontierX::LegacyVirtualKey::LeftMenu;
inline constexpr U32 VK_RMENU = FrontierX::LegacyVirtualKey::RightMenu;

inline constexpr U32 WM_USER = 0x0400U;
inline constexpr U32 WM_SETFOCUS = FrontierX::LegacyMessage::SetFocus;
inline constexpr U32 WM_ACTIVATE = 0x0006U;
inline constexpr U32 WM_ACTIVATEAPP = FrontierX::LegacyMessage::ActivateApp;
inline constexpr U32 WM_KEYDOWN = FrontierX::LegacyMessage::KeyDown;
inline constexpr U32 WM_KEYUP = FrontierX::LegacyMessage::KeyUp;
inline constexpr U32 WM_SYSKEYDOWN = FrontierX::LegacyMessage::SystemKeyDown;
inline constexpr U32 WM_SYSKEYUP = FrontierX::LegacyMessage::SystemKeyUp;
inline constexpr U32 WM_MOUSEMOVE = FrontierX::LegacyMessage::MouseMove;
inline constexpr U32 WM_LBUTTONDOWN = FrontierX::LegacyMessage::LeftButtonDown;
inline constexpr U32 WM_LBUTTONUP = FrontierX::LegacyMessage::LeftButtonUp;
inline constexpr U32 WM_RBUTTONDOWN = FrontierX::LegacyMessage::RightButtonDown;
inline constexpr U32 WM_RBUTTONUP = FrontierX::LegacyMessage::RightButtonUp;
inline constexpr U32 WM_MBUTTONDOWN = FrontierX::LegacyMessage::MiddleButtonDown;
inline constexpr U32 WM_MBUTTONUP = FrontierX::LegacyMessage::MiddleButtonUp;

inline constexpr U32 MK_LBUTTON = FrontierX::LegacyMouseMask::Left;
inline constexpr U32 MK_RBUTTON = FrontierX::LegacyMouseMask::Right;
inline constexpr U32 MK_MBUTTON = FrontierX::LegacyMouseMask::Middle;

inline constexpr U32 JOY_POVCENTERED = FrontierX::LegacyJoystickPov::Centered;
inline constexpr U32 JOY_POVFORWARD = FrontierX::LegacyJoystickPov::Forward;
inline constexpr U32 JOY_POVRIGHT = FrontierX::LegacyJoystickPov::Right;
inline constexpr U32 JOY_POVBACKWARD = FrontierX::LegacyJoystickPov::Backward;
inline constexpr U32 JOY_POVLEFT = FrontierX::LegacyJoystickPov::Left;
inline constexpr U32 JOY_RETURNX = FrontierX::LegacyJoystickFlag::ReturnX;
inline constexpr U32 JOY_RETURNY = FrontierX::LegacyJoystickFlag::ReturnY;
inline constexpr U32 JOY_RETURNPOV = FrontierX::LegacyJoystickFlag::ReturnPov;
inline constexpr U32 JOY_RETURNBUTTONS = FrontierX::LegacyJoystickFlag::ReturnButtons;
inline constexpr U32 JOY_RETURNRAWDATA = FrontierX::LegacyJoystickFlag::ReturnRawData;
inline constexpr U32 JOY_RETURNPOVCTS = FrontierX::LegacyJoystickFlag::ReturnPovContinuous;

inline constexpr U32 JOY_BUTTON1 = FrontierX::LegacyJoystickButton(1);
inline constexpr U32 JOY_BUTTON2 = FrontierX::LegacyJoystickButton(2);
inline constexpr U32 JOY_BUTTON3 = FrontierX::LegacyJoystickButton(3);
inline constexpr U32 JOY_BUTTON4 = FrontierX::LegacyJoystickButton(4);
inline constexpr U32 JOY_BUTTON5 = FrontierX::LegacyJoystickButton(5);
inline constexpr U32 JOY_BUTTON6 = FrontierX::LegacyJoystickButton(6);
inline constexpr U32 JOY_BUTTON7 = FrontierX::LegacyJoystickButton(7);
inline constexpr U32 JOY_BUTTON8 = FrontierX::LegacyJoystickButton(8);
inline constexpr U32 JOY_BUTTON9 = FrontierX::LegacyJoystickButton(9);
inline constexpr U32 JOY_BUTTON10 = FrontierX::LegacyJoystickButton(10);
inline constexpr U32 JOY_BUTTON11 = FrontierX::LegacyJoystickButton(11);
inline constexpr U32 JOY_BUTTON12 = FrontierX::LegacyJoystickButton(12);
inline constexpr U32 JOY_BUTTON13 = FrontierX::LegacyJoystickButton(13);
inline constexpr U32 JOY_BUTTON14 = FrontierX::LegacyJoystickButton(14);

inline constexpr U32 MM_JOY1BUTTONDOWN = 0x03b5U;
inline constexpr U32 MM_JOY1BUTTONUP = 0x03b7U;
#endif


static char interface_name[] = "IHotkeyEvent";


//--------------------------------------------------------------------------//
//
#if defined(_MSC_VER)
#pragma warning(disable: 4200)		// zero length array
#endif

struct KTEXT
{
	U32 numStrings;
	U32 sizeOfString;
#if defined(_MSC_VER)
	char strings[];
#else
	char strings[1];
#endif

	char * GetText (U32 index)
	{
#ifdef _DEBUG
		if (index==0 || index > numStrings)
			return 0;
#endif
		return strings + ((index-1) * sizeOfString);
	}

	void remove_elements (U32 startIndex, U32 numToRemove)
	{
		if (startIndex >= numStrings)
			return;
		if ((startIndex + numToRemove) > numStrings)
			numToRemove = numStrings - startIndex;

		char * src, * dst;

	  	src = getText(startIndex+numToRemove);
		dst = getText(startIndex);

		std::memmove(dst, src, getText(numStrings) - src);
		numStrings -= numToRemove;
	}

	void remove_element (U32 index)
	{
		if (index >= numStrings)
			return;

		char * src, * dst;

		dst = getText(index);
	  	src = dst + sizeOfString;

		std::memmove(dst, src, strings + (numStrings * sizeOfString) - src);
		numStrings--;
	}

private:
	char * getText (U32 index)
	{
#ifdef _DEBUG
		if (index >= numStrings)
			return 0;
#endif
		return strings + (index * sizeOfString);
	}
};

//--------------------------------------------------------------------------//
//
static int DACOM_FASTCALL BitsSet (DWORD dwNumber)		// return number of bits set in number
{
	int result=0;
	DWORD i=1;

	do
	{
		if (dwNumber & i)
			result++;
		i <<= 1;
	} while (i);


	return result;
}
//--------------------------------------------------------------------------//
//----------------------------CmpHotKey Methods-----------------------------//
//--------------------------------------------------------------------------//
// Return 0 if equal, < 0 if we are smaller, > 0 if we are greater
// 	  ranking:
//			CTRL-ALT-A beats
//			ALT-A beats
//			release events beats
//			press events beats
//			A (single key events)
//			special_vkeys beats
//
S32 DAHOTKEY_STRUCT::compare (DAHOTKEY_STRUCT & rec)
{
	S32 result;

//	if ((result = rec.vKEY - vKEY) == 0)
		if ((result = BitsSet(getModifiers()) - BitsSet(rec.getModifiers())) == 0)
			if ((result = bType - rec.bType) == 0)
				if (vKEY >= HKR_VKEY_ALPHA || rec.vKEY >= HKR_VKEY_ALPHA)
				{
					if (vKEY < HKR_VKEY_ALPHA)
						result = 1;
					else
					if (rec.vKEY < HKR_VKEY_ALPHA)
						result = -1;
					else
						result = vKEY - rec.vKEY;
				}

	return result;
}

//--------------------------------------------------------------------------//
//---------------------HotkeyEvent class declaration------------------------//
//--------------------------------------------------------------------------//


struct DACOM_NO_VTABLE HotkeyEvent : public IHotkeyEvent, 
											ConnectionPointContainer<HotkeyEvent>,
											IEventCallback
{
	BEGIN_DACOM_MAP_INBOUND(HotkeyEvent)
	DACOM_INTERFACE_ENTRY(IHotkeyEvent)
	DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	DACOM_INTERFACE_ENTRY(IAggregateComponent)
	DACOM_INTERFACE_ENTRY2(IID_IHotkeyEvent,IHotkeyEvent)
	DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer,IDAConnectionPointContainer)
	DACOM_INTERFACE_ENTRY2(IID_IEventCallback,IEventCallback)
	DACOM_INTERFACE_ENTRY2(IID_IAggregateComponent,IAggregateComponent)
	END_DACOM_MAP()

	BEGIN_DACOM_MAP_OUTBOUND(HotkeyEvent)
	DACOM_INTERFACE_ENTRY_AGGREGATE("IEventMessageFilter", messageFilter)
	DACOM_INTERFACE_ENTRY_AGGREGATE("IEventCallback", eventCallback)
	END_DACOM_MAP()

	//------------------------------------

	BOOL					bEnabled;
	DWORD					dwJoyMessage;
	U32						dwHotkeyMessage;

	DWORD					dwPrevJoyButtons;
	DWORD					dwPrevMouseButtons;
	DWORD					dwPrevPOV;
	DWORD					dwPrevDIR;
	KB_MODIFIERS			kbMods;
	DWORD					dwCurrentModifiers;

	BYTE					vkey_state[(HKR_VKEY_LAST+7)/8];		// bitfield

	DAHOTKEY				pKeyData;
	KTEXT                   *pKeyText;
	DWORD					dwNumKeys;
	DWORD					dwSizeOfKey;

	BOOL					bFilterMessage;		// true if we are to filter the current msg

	//------------------------------
	// outgoing interface connection points
	//
	ConnectionPoint<HotkeyEvent> messageFilter;
	ConnectionPoint<HotkeyEvent> eventCallback;
	
	//------------------------------
	
	HotkeyEvent (void);
	~HotkeyEvent (void);

	DA_HEAP_DEFINE_NEW_OPERATOR(HotkeyEvent);

	GENRESULT init (HKEVENTDESC * info);
	
	/* IAggregateComponent members */

	DEFMETHOD(Initialize) (void);
	
	/* IHotkeyEvent members */
	
	DEFMETHOD_(BOOL32,GetHotkeyState) (U32 hotkey);

	DEFMETHOD_(BOOL32,GetVkeyState) (U32 vkey);

	DEFMETHOD(Enable) (void);

	DEFMETHOD(Disable) (void);

    virtual void DACOM_CDECL SystemMessage ( FrontierX::LegacyWindowToken hwnd,
                                         FrontierX::LegacyMessageId message,
                                         FrontierX::LegacyWParam wParam,
                                         FrontierX::LegacyLParam lParam,
                                         S32   unused1=0,
                                         S32   unused2=0);

	DEFMETHOD_(U32,GetHotkeyText) (U32 hotkey, C8 *outBuffer, U32 bufferSize);

	DEFMETHOD_(BOOL32,IsAnyHotkeyPressed) (void);

	/* IEventCallback members */

	DEFMETHOD(Notify) (U32 message, void *param = 0);
	
	
	/* HotkeyEvent members */

	LRESULT WndCallback (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void UpdateModifiers (void);

	BOOL32 UpdateKeyboard (DWORD dwVirtKey, BOOL bKeyPress);

	BOOL32 UpdateJoystick (DWORD dwButtons, DWORD dwJoyPOV=JOY_POVCENTERED, DWORD dwJoyDIR=0);

	BOOL32 KeyPressedEvent (DWORD dwVirtKey);

	BOOL32 KeyReleasedEvent (DWORD dwVirtKey);

	void SetVkeyState (DWORD dwVirtKey);

	void ClearVkeyState (DWORD dwVirtKey);

	BOOL32 UpdateMouse (DWORD dwMouseButtons);

	static DWORD DACOM_FASTCALL ConvertToModifier (DWORD dwVirtKey);

	static BOOL32 DACOM_FASTCALL IsAModifier (DWORD dwVirtKey, DWORD dwKeyMods);
	
	static BOOL32 DACOM_FASTCALL IsAMatch (DWORD dwVirtKey, DWORD dwMatchKey, DWORD dwKeyMods);

	BOOL32 AddPressedEvent (DWORD dwHotkey);

	BOOL32 AddReleasedEvent (DWORD dwHotkey);
	
	void setOtherDirtyFlags (DWORD dwHotkey);

	void initKeyState (void);

	void SendEvent (U32 eventNum);

	GENRESULT PreMessageFilter  (FrontierX::LegacyWindowToken * hwnd,
	                             FrontierX::LegacyMessageId * message,
	                             FrontierX::LegacyWParam * wParam,
	                             FrontierX::LegacyLParam * lParam);

	GENRESULT PostMessageFilter (FrontierX::LegacyWindowToken hwnd,
	                             FrontierX::LegacyMessageId message,
	                             FrontierX::LegacyWParam wParam,
	                             FrontierX::LegacyLParam lParam);
};

//--------------------------------------------------------------------------//
//---------------------------HotkeyEvent Methods----------------------------//
//--------------------------------------------------------------------------//
//																			//
HotkeyEvent::HotkeyEvent (void) :
	bEnabled(1),
	dwJoyMessage(0),
	dwHotkeyMessage(0),
	dwPrevJoyButtons(0),
	dwPrevMouseButtons(0),
	dwPrevPOV(JOY_POVCENTERED),
	dwPrevDIR(0),
	kbMods{},
	dwCurrentModifiers(0),
	vkey_state{},
	pKeyData(nullptr),
	pKeyText(nullptr),
	dwNumKeys(0),
	dwSizeOfKey(0),
	bFilterMessage(0),
	messageFilter(0),
	eventCallback(1)
{
}
//--------------------------------------------------------------------------//
//
HotkeyEvent::~HotkeyEvent (void)
{
	DA_HEAP_FREE(HEAP, pKeyData);
	pKeyData = 0;
	DA_HEAP_FREE(HEAP, pKeyText);
	pKeyText = 0;
}
//--------------------------------------------------------------------------//
//
GENRESULT HotkeyEvent::Initialize (void)
{
	return GR_OK;
}
//--------------------------------------------------------------------------//
//
void HotkeyEvent::UpdateModifiers (void)
{
	DAHOTKEY_STRUCT tmp;
	
	tmp.mKEY = ((((kbMods.bLeftAlt|kbMods.bRightAlt) * 2) + 
		(kbMods.bLeftControl|kbMods.bRightControl)) * 2) + 
		(kbMods.bLeftShift|kbMods.bRightShift);
	
	tmp.mJOY = dwPrevJoyButtons;
	
	switch (dwPrevPOV)
	{
	case JOY_POVFORWARD:
		tmp.mHAT = 1;
		break;
		
	case JOY_POVRIGHT:
		tmp.mHAT = 4;
		break;
		
	case JOY_POVBACKWARD:
		tmp.mHAT = 2;
		break;
		
	case JOY_POVLEFT:
		tmp.mHAT = 8;
		break;
		
	case JOY_POVCENTERED:
	default:
		tmp.mHAT = 0;
		break;
	}
	
	switch (dwPrevDIR)
	{
	case HKR_VKEY_DIR_U:
		tmp.mDIR = 1;
		break;
	case HKR_VKEY_DIR_UR:
		tmp.mDIR = 2;
		break;
	case HKR_VKEY_DIR_R:
		tmp.mDIR = 4;
		break;
	case HKR_VKEY_DIR_DR:
		tmp.mDIR = 8;
		break;
	case HKR_VKEY_DIR_D:
		tmp.mDIR = 16;
		break;
	case HKR_VKEY_DIR_DL:
		tmp.mDIR = 32;
		break;
	case HKR_VKEY_DIR_L:
		tmp.mDIR = 64;
		break;
	case HKR_VKEY_DIR_UL:
		tmp.mDIR = 128;
		break;
	default:
		tmp.mDIR = 0;
		break;
	}
	
	tmp.mMOUSE = dwPrevMouseButtons;
	
	dwCurrentModifiers = tmp.getModifiers();
}
//--------------------------------------------------------------------------//
//
void HotkeyEvent::initKeyState (void)
{
	DWORD i;
#if defined(_WIN32)
	BYTE buffer[256];
#endif

	kbMods.reset();
	memset(vkey_state, 0, sizeof(vkey_state));
	dwCurrentModifiers=0;
	for (i=0; i < dwNumKeys; i++)
		pKeyData[i].bPressed=0;
#if defined(_WIN32)
	if (GetKeyboardState(buffer))
	{
		vkey_state[VK_NUMLOCK >> 3] |= ((buffer[VK_NUMLOCK] & 0x81) != 0) << (VK_NUMLOCK & 7);
		vkey_state[VK_SCROLL >> 3] |= ((buffer[VK_SCROLL] & 0x81) != 0) << (VK_SCROLL & 7);
		vkey_state[VK_CAPITAL >> 3] |= ((buffer[VK_CAPITAL] & 0x81) != 0) << (VK_CAPITAL & 7);
	}
#endif
}
//----------------------------------------------------------------------------
//
inline void HotkeyEvent::SetVkeyState (DWORD dwVirtKey)
{
	vkey_state[dwVirtKey >> 3] |= 1 << (dwVirtKey & 7);
}
//----------------------------------------------------------------------------
//
inline void HotkeyEvent::ClearVkeyState (DWORD dwVirtKey)
{
	vkey_state[dwVirtKey >> 3] &= ~(1 << (dwVirtKey & 7));
}
//----------------------------------------------------------------------------
//
BOOL32 HotkeyEvent::GetVkeyState (U32 dwVirtKey)
{
	if (dwVirtKey > HKR_VKEY_LAST)
		return 0;

	BOOL32 result = (vkey_state[dwVirtKey >> 3] >> (dwVirtKey & 7));

	return result & 1;
}
//----------------------------------------------------------------------------
//
BOOL32 HotkeyEvent::UpdateKeyboard (DWORD dwVirtKey, BOOL bKeyPress)
{
	BOOL result;
	if (bKeyPress != GetVkeyState(dwVirtKey))
	{
	 	if (bKeyPress)
		{
			result = KeyPressedEvent(dwVirtKey);
//			SetVkeyState(dwVirtKey);
		}
		else
		{
			result = KeyReleasedEvent(dwVirtKey);
//			ClearVkeyState(dwVirtKey);
		}
	}
	else
	{
		bFilterMessage = 1;		// filter repeat messages
		result=0;
	}
	return result;
}
//--------------------------------------------------------------------------//
//
inline GENRESULT HotkeyEvent::PreMessageFilter (FrontierX::LegacyWindowToken * hwnd,
	FrontierX::LegacyMessageId * message, FrontierX::LegacyWParam * wParam,
	FrontierX::LegacyLParam * lParam)
{
	CONNECTION_NODE<IEventMessageFilter> *pNode = (CONNECTION_NODE<IEventMessageFilter> *) messageFilter.pClientList;
	GENRESULT result = GR_OK;

	while (pNode)
	{
		if ((result = pNode->client->PreMessageFilter(hwnd, message, wParam, lParam)) != GR_OK)
			break;
		pNode = pNode->pNext;
	}

	return result;
}

//--------------------------------------------------------------------------//
//
inline GENRESULT HotkeyEvent::PostMessageFilter (FrontierX::LegacyWindowToken hwnd,
	FrontierX::LegacyMessageId message, FrontierX::LegacyWParam wParam,
	FrontierX::LegacyLParam lParam)
{
	CONNECTION_NODE<IEventMessageFilter> *pNode = (CONNECTION_NODE<IEventMessageFilter> *) messageFilter.pClientList;
	GENRESULT result = GR_OK;

	while (pNode)
	{
		if ((result = pNode->client->PostMessageFilter(hwnd, message, wParam, lParam)) != GR_OK)
			break;
		pNode = pNode->pNext;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
void HotkeyEvent::SystemMessage (FrontierX::LegacyWindowToken hwnd,
	FrontierX::LegacyMessageId message, FrontierX::LegacyWParam wParam,
	FrontierX::LegacyLParam lParam, S32 unused1, S32 unused2)
{
	(void)unused1;
	(void)unused2;
	if (PreMessageFilter(&hwnd, &message, &wParam, &lParam) != GR_OK)
		return;

	bFilterMessage = 0;

	if (dwJoyMessage && message == dwJoyMessage)
	{
		JOYINFOEX *joy = FrontierX::PointerFromLegacyLParam<JOYINFOEX>(lParam);
		DWORD dwJoyDIR = 0, dwPOV=JOY_POVCENTERED, dwButtons=0;
		if (joy == nullptr)
			return;

		if (joy->dwFlags & JOY_RETURNBUTTONS)
			dwButtons = joy->dwButtons;
		if ((joy->dwFlags & (JOY_RETURNPOV|JOY_RETURNPOVCTS)) == (JOY_RETURNPOV))
			dwPOV = joy->dwPOV;
		if ((joy->dwFlags & (JOY_RETURNX|JOY_RETURNY|JOY_RETURNRAWDATA)) == (JOY_RETURNX|JOY_RETURNY))
		{
			if (joy->dwXpos > 0xC000)
			{
				if (joy->dwYpos < 0x4000)
					dwJoyDIR = HKR_VKEY_DIR_UR;
				else
				if (joy->dwYpos > 0xC000)
					dwJoyDIR = HKR_VKEY_DIR_DR;
				else
					dwJoyDIR = HKR_VKEY_DIR_R;
			}
			else
			if (joy->dwXpos < 0x4000)
			{
				if (joy->dwYpos < 0x4000)
					dwJoyDIR = HKR_VKEY_DIR_UL;
				else
				if (joy->dwYpos > 0xC000)
					dwJoyDIR = HKR_VKEY_DIR_DL;
				else
					dwJoyDIR = HKR_VKEY_DIR_L;
			}
			else
			{
				if (joy->dwYpos < 0x4000)
					dwJoyDIR = HKR_VKEY_DIR_U;
				else
				if (joy->dwYpos > 0xC000)
					dwJoyDIR = HKR_VKEY_DIR_D;
			}
		}

		UpdateJoystick(dwButtons, dwPOV, dwJoyDIR);
	}
	else
		WndCallback(
#if defined(_WIN32)
			reinterpret_cast<HWND>(hwnd), message,
#else
			static_cast<HWND>(hwnd), message,
#endif
			static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));

	if (bFilterMessage == 0)
		PostMessageFilter(hwnd, message, wParam, lParam);
}
//--------------------------------------------------------------------------//
//
U32 HotkeyEvent::GetHotkeyText (U32 hotkey, C8 *outBuffer, U32 bufferSize)
{
	C8 * src;
	U32 result = 0;

	if (pKeyText && hotkey > 0 && hotkey <= pKeyText->numStrings && bufferSize &&
		(src = pKeyText->GetText(hotkey)) != 0)
	{
		result = strlen(src);
		if (bufferSize < result+1)
			result = bufferSize-1;
		
		if (outBuffer)
		{
			memcpy(outBuffer, src, result);
			outBuffer[result] = 0;
		}
		return result;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
GENRESULT HotkeyEvent::Notify (U32 message, void *param)
{
	if (dwJoyMessage && message == dwJoyMessage)
		SystemMessage(0, message, 0, FrontierX::LegacyLParamFromPointer(param));
	else
	if (message < WM_USER)
	{
		if (param == nullptr)
			return GR_INVALID_PARMS;
#if defined(_WIN32)
		MSG *msg = (MSG *) param;
		SystemMessage(FrontierX::LegacyWindowTokenFromPointer(msg->hwnd),
			msg->message, msg->wParam, msg->lParam);
#else
		auto *msg = static_cast<FrontierX::LegacyWindowMessage *>(param);
		SystemMessage(msg->window, msg->message, msg->wparam, msg->lparam);
#endif
	}

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
LRESULT HotkeyEvent::WndCallback (HWND _hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)_hwnd;
	switch (message)
	{
		case MM_JOY1BUTTONDOWN:
		case MM_JOY1BUTTONUP:
			UpdateJoystick(wParam & 0xFF);
			break;
	
	
		case WM_SETFOCUS:
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
			initKeyState();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			switch ((BYTE)wParam)
			{
			 	case VK_SHIFT:
					if ((lParam>>24) & 1)	// if right button
					{
						kbMods.bRightShift = ((((unsigned)lParam)>>31)^1);
						UpdateModifiers();
						UpdateKeyboard(VK_RSHIFT, ((((unsigned)lParam)>>31)^1));
					}
					else
					{
						kbMods.bLeftShift = ((((unsigned)lParam)>>31)^1);
						UpdateModifiers();
						UpdateKeyboard(VK_LSHIFT, ((((unsigned)lParam)>>31)^1));
					}
					break;

			 	case VK_CONTROL:
					if ((lParam>>24) & 1)	// if right button
					{
						kbMods.bRightControl = ((((unsigned)lParam)>>31)^1);
						UpdateModifiers();
						UpdateKeyboard(VK_RCONTROL, ((((unsigned)lParam)>>31)^1));
					}
					else
					{
						kbMods.bLeftControl = ((((unsigned)lParam)>>31)^1);
						UpdateModifiers();
						UpdateKeyboard(VK_LCONTROL, ((((unsigned)lParam)>>31)^1));
					}
					break;
					
			 	case VK_MENU:
					if ((lParam>>24) & 1)	// if right button
					{
						kbMods.bRightAlt = ((((unsigned)lParam)>>31)^1);
						UpdateModifiers();
						UpdateKeyboard(VK_RMENU, ((((unsigned)lParam)>>31)^1));
					}
					else
					{
						kbMods.bLeftAlt = ((((unsigned)lParam)>>31)^1);
						UpdateModifiers();
						UpdateKeyboard(VK_LMENU, ((((unsigned)lParam)>>31)^1));
					}
					break;

				case VK_CAPITAL:
				case VK_SCROLL:
				case VK_NUMLOCK:
					if (((((unsigned)lParam)>>31)^1) == 0)		// if key release
					{
#if defined(_WIN32)
						if (GetKeyState(static_cast<int>(wParam)) & 1)		// is it toggled?
#else
						if ((static_cast<U32>(lParam) & FrontierX::LegacyKeyToggleBit) != 0)
#endif
							break;		// do not process this message
					}

					[[fallthrough]];

				default:
					UpdateModifiers();
					UpdateKeyboard((BYTE)wParam, ((((unsigned)lParam)>>31)^1));
			}
			break;
		
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
//		case WM_MOUSEMOVE:
			{
				DWORD dwMouseButtons;

				dwMouseButtons = 0;
				if (wParam & MK_LBUTTON)
					dwMouseButtons |= 1;
				if (wParam & MK_RBUTTON)
					dwMouseButtons |= 2;
				if (wParam & MK_MBUTTON)
					dwMouseButtons |= 4;
				dwMouseButtons ^= dwPrevMouseButtons;
				dwPrevMouseButtons ^= dwMouseButtons;
				if (dwMouseButtons)		// something changed!
				{
					UpdateModifiers();
					UpdateMouse(dwMouseButtons);
				}
				else
					bFilterMessage = 1;

			}
			break;

	}

	return 0;
}
//----------------------------------------------------------------------------
// returns modifier bitfield
//
DWORD HotkeyEvent::ConvertToModifier (DWORD dwVirtKey)
{
	DWORD result;

	switch (dwVirtKey)
	{
		case VK_SHIFT:
		case VK_LSHIFT:
		case VK_RSHIFT:
			result = HKRF_SHIFT;
			break;

		case VK_CONTROL:
		case VK_LCONTROL:
		case VK_RCONTROL:
			result = HKRF_CTRL;
			break;

		case VK_MENU:
		case VK_LMENU:
		case VK_RMENU:
			result = HKRF_ALT;
			break;

		case HKR_VKEY_JOY0:
		case HKR_VKEY_JOY1:
		case HKR_VKEY_JOY2:
		case HKR_VKEY_JOY3:
		case HKR_VKEY_JOY4:
		case HKR_VKEY_JOY5:
		case HKR_VKEY_JOY6:
		case HKR_VKEY_JOY7:
		case HKR_VKEY_JOY8:
		case HKR_VKEY_JOY9:
		case HKR_VKEY_JOY10:
		case HKR_VKEY_JOY11:
		case HKR_VKEY_JOY12:
		case HKR_VKEY_JOY13:
			result = HKRF_MOD_JOY0 << (dwVirtKey - HKR_VKEY_JOY0);
			break;

		case HKR_VKEY_DIR_U:
		case HKR_VKEY_DIR_UR:
		case HKR_VKEY_DIR_R:
		case HKR_VKEY_DIR_DR:
		case HKR_VKEY_DIR_D:
		case HKR_VKEY_DIR_DL:
		case HKR_VKEY_DIR_L:
		case HKR_VKEY_DIR_UL:
			result = HKRF_MOD_DIR_U << (dwVirtKey - HKR_VKEY_DIR_U);
			break;

		case HKR_VKEY_POVUP:
		case HKR_VKEY_POVDOWN:
		case HKR_VKEY_POVRIGHT:
		case HKR_VKEY_POVLEFT:
			result = HKRF_MOD_POVUP << (dwVirtKey - HKR_VKEY_POVUP);
			break;

		case HKR_VKEY_MOUSELEFT:
		case HKR_VKEY_MOUSERIGHT:
		case HKR_VKEY_MOUSEMIDDLE:
			result = HKRF_MOD_MOUSELEFT << (dwVirtKey - HKR_VKEY_MOUSELEFT);
			break;

		default:
			result = 0;
	}

	return result;
}
//----------------------------------------------------------------------------
// return TRUE if dwVirtKey is actually one of the modifier keys
//
BOOL32 HotkeyEvent::IsAModifier (DWORD dwVirtKey, DWORD dwKeyMods)
{
	return ((ConvertToModifier(dwVirtKey) & dwKeyMods) != 0);
}
//----------------------------------------------------------------------------
// return TRUE if vKeys match
//
BOOL32 HotkeyEvent::IsAMatch (DWORD dwVirtKey, DWORD dwMatchKey, DWORD dwKeyMods)
{
	switch (dwVirtKey)
	{
		case 0:
			return IsAModifier(dwMatchKey, dwKeyMods);
		case HKR_VKEY_ALPHA:
			return (dwMatchKey >= 'A' && dwMatchKey <= 'Z') * 2;
		case HKR_VKEY_NUMBER:
			return (dwMatchKey >= '0' && dwMatchKey <= '9') * 2;
		case HKR_VKEY_OTHER:
			return (dwMatchKey < '0' || (dwMatchKey > 'Z' && dwMatchKey < 256)) * 2;
		case HKR_VKEY_SINGLE:
			return (dwMatchKey < 256) * 2;
	}

	return (dwVirtKey == dwMatchKey);
}
//----------------------------------------------------------------------------
// One new key was pressed. Input is the virt key version. (can also be mouse, joystick, etc)
//
//
BOOL32 HotkeyEvent::KeyPressedEvent (DWORD dwVirtKey)
{
	BOOL32 bEventAdded=0;
	DWORD i, dwKeyMods;
	int iMatchResult, iSpecialKeyPressed=-1;

	for (i = 0; i < dwNumKeys && bEventAdded==0; i++)
	{
 		dwKeyMods = pKeyData[i].getModifiers();
		if ((dwKeyMods & dwCurrentModifiers) == dwKeyMods)	// all modifiers pressed
		{
			if ((iMatchResult = IsAMatch(pKeyData[i].vKEY, dwVirtKey, dwKeyMods)) != 0)
			{
				if (pKeyData[i].bType==HKR_PRESSED)
				{
					if (iMatchResult==1)		// if regular vkey
					{
						if (iSpecialKeyPressed < 0 || dwKeyMods!=0)	   // if no special key was pressed || key is not single
						{
							if ((bEventAdded = pKeyData[i].bPressed)==0)
							{
								AddPressedEvent(i);
								pKeyData[i].bPressed = bEventAdded = 1;
							}
						}
					}
					else		// one of the special vkeys
					{
						if (iSpecialKeyPressed < 0)
					 		iSpecialKeyPressed = i;
					}
				}
				else
				{
					if (iSpecialKeyPressed < 0 || dwKeyMods!=0)	   // if no special key was pressed || key is not single
					{
						if (pKeyData[i].bPressed==0)
							pKeyData[i].bDirty = 0;
						setOtherDirtyFlags(i);
						pKeyData[i].bPressed = 1;
					}
				}
			}
		}
	}

	//
	// else if all necessary keys are pressed, set the pressed flag
	//
	for (i = 0; i < dwNumKeys; i++)
	{
		if (pKeyData[i].bPressed==0)
		{
 			dwKeyMods = pKeyData[i].getModifiers();
			if ((dwKeyMods & dwCurrentModifiers) == dwKeyMods)	// all modifiers pressed
			{
				if (pKeyData[i].vKEY == 0 || pKeyData[i].vKEY == dwVirtKey || GetVkeyState(pKeyData[i].vKEY))
				{
					pKeyData[i].bDirty =
					pKeyData[i].bPressed = 1;
				}
			}
		}
	}

	//
	// if a more complicated hotkey is pressed than us, unset the pressed flag
	//

	for (i = 0; i < dwNumKeys; i++)
	{
		DWORD j, vKEY;

		if (pKeyData[i].bPressed && (vKEY = pKeyData[i].vKEY) != 0)
		{
			j = i+1;

			while (j < dwNumKeys)
			{
				if (vKEY == pKeyData[j].vKEY)
					pKeyData[j].bDirty = pKeyData[j].bPressed = 0;
				j++;
			}
		}
	}

	if (bEventAdded==0 && iSpecialKeyPressed >= 0)
	{
		AddPressedEvent(iSpecialKeyPressed);
		pKeyData[iSpecialKeyPressed].bPressed = bEventAdded = 1;
	}

	//
	// if someone cares, check to see if state changed
	//

	if (bEventAdded)
		bFilterMessage=1;

	//
	// set the virtual key state to true
	//
	SetVkeyState(dwVirtKey);
	
	
	return bEventAdded;
}
//----------------------------------------------------------------------------
//
void HotkeyEvent::setOtherDirtyFlags (DWORD dwHotkey)
{
	DWORD i, dwMods;

	dwMods   = pKeyData[dwHotkey].getModifiers();

	// if any of the modifiers of this key is the vkey of another hotkey
	// mark other hotkey as dirty

	for (i = 0; i < dwNumKeys; i++)
	{
		if (i != dwHotkey && pKeyData[i].bType==HKR_RELEASED)
		{
			if ((dwMods & ConvertToModifier(pKeyData[i].vKEY)) != 0)
				pKeyData[i].bDirty = 1;
		}
	}
}
//----------------------------------------------------------------------------
// Input is hotkey index number
//
BOOL32 HotkeyEvent::AddPressedEvent (DWORD dwHotkey)
{
	setOtherDirtyFlags(dwHotkey);

	if (bEnabled)
		SendEvent(pKeyData[dwHotkey].symbol);
	
	return 1;
}
//----------------------------------------------------------------------------
// One key was released. Input is the virt key version. (can also be mouse, joystick, etc)
//
BOOL32 HotkeyEvent::KeyReleasedEvent (DWORD dwVirtKey)
{
	BOOL32 bEventAdded=0;
	DWORD i, dwKeyMods, dwVMods;

	dwVMods = ConvertToModifier(dwVirtKey);	 	// modifier button released?

	for (i = 0; i < dwNumKeys; i++)
	{
		if (pKeyData[i].bPressed==0 && pKeyData[i].vKEY < HKR_VKEY_ALPHA)
			continue;

		// if all modifiers still pressed && vkey && vkey != key that was released, move on
 		dwKeyMods = pKeyData[i].getModifiers();
		if (IsAMatch(pKeyData[i].vKEY, dwVirtKey, dwKeyMods)==0 && // lpKeyData[i].vKEY != dwVirtKey && 
			((dwKeyMods & dwCurrentModifiers) == dwKeyMods))	// all modifiers pressed
		{
		 	continue;
		}
		// else one of the relavent keys was released
		pKeyData[i].bPressed = 0;
		// can this hotkey generate an event?
		if (bEventAdded!=0 ||
			pKeyData[i].bType==HKR_PRESSED || 
			pKeyData[i].bDirty != 0 )
		{
			// nothing to do?
		}
		else
		if ((dwKeyMods & (dwCurrentModifiers|dwVMods)) == dwKeyMods)	// all modifiers pressed
		{
			if (bEventAdded==0)
			{
				AddReleasedEvent(i);
				bEventAdded=1;
			}
		}
	}

	// begin new things

	//
	// else if all necessary keys are pressed, set the pressed flag
	//
	for (i = 0; i < dwNumKeys; i++)
	{
		if (pKeyData[i].bPressed==0)
		{
 			dwKeyMods = pKeyData[i].getModifiers();
			if ((dwKeyMods & dwCurrentModifiers) == dwKeyMods)	// all modifiers pressed
			{
				if (pKeyData[i].vKEY == 0 || (pKeyData[i].vKEY != dwVirtKey && GetVkeyState(pKeyData[i].vKEY)))
				{
					pKeyData[i].bDirty =
					pKeyData[i].bPressed = 1;
				}
			}
		}
	}

	//
	// if a more complicated hotkey is pressed than us, unset the pressed flag
	//

	for (i = 0; i < dwNumKeys; i++)
	{
		DWORD j, vKEY;

		if (pKeyData[i].bPressed && (vKEY = pKeyData[i].vKEY) != 0)
		{
			j = i+1;

			while (j < dwNumKeys)
			{
				if (vKEY == pKeyData[j].vKEY)
					pKeyData[j].bDirty = pKeyData[j].bPressed = 0;
				j++;
			}
		}
	}

	// end new things

	//
	// if someone cares, check to see if state changed
	//

	if (bEventAdded)
		bFilterMessage=1;
	//
	// set the virtual key state to false
	//
	ClearVkeyState(dwVirtKey);

	return bEventAdded;
}
//----------------------------------------------------------------------------
// dwMouseButtons= bitfield of buttons that have changed
// For each button that was pressed, call KeyPressedEvent(),
// call KeyReleasedEvent() for each button that was released.
//
BOOL32 HotkeyEvent::UpdateMouse (DWORD dwMouseButtons)
{
	DWORD i,j;
	BOOL result = 0;

	i = 1;
	j = HKR_VKEY_MOUSELEFT;
	do
	{
	 	if (dwMouseButtons & i)
		{
			if (dwPrevMouseButtons & i)		// mouse button was pressed
 				result |= KeyPressedEvent(j);
			else	// else button was released
				result |= KeyReleasedEvent(j);	
		}
		i <<= 1;
		j++;
	} while (i);

	return result;
}
//----------------------------------------------------------------------------
// Input is hotkey index number
//
BOOL32 HotkeyEvent::AddReleasedEvent (DWORD dwHotkey)
{
	if (bEnabled)
		SendEvent(pKeyData[dwHotkey].symbol);

	return 1;
}
//----------------------------------------------------------------------------
//
BOOL32 HotkeyEvent::UpdateJoystick (DWORD dwButtons, DWORD dwJoyPOV, DWORD dwJoyDIR)
{
	DWORD i,j, dwJoyButtons;
	BOOL32 result = 0;

	dwJoyButtons  = (dwButtons & JOY_BUTTON1) ? 1:0;
	dwJoyButtons |= (dwButtons & JOY_BUTTON2) ? 2:0;
	dwJoyButtons |= (dwButtons & JOY_BUTTON3) ? 4:0;
	dwJoyButtons |= (dwButtons & JOY_BUTTON4) ? 8:0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON5&~JOY_BUTTON1)) ? 16:0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON6&~JOY_BUTTON1)) ? 32:0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON7&~JOY_BUTTON1)) ? 64:0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON8&~JOY_BUTTON1)) ? 128:0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON9&~JOY_BUTTON1)) ? (1<<8):0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON10&~JOY_BUTTON1)) ? (1<<9):0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON11&~JOY_BUTTON1)) ? (1<<10):0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON12&~JOY_BUTTON1)) ? (1<<11):0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON13&~JOY_BUTTON1)) ? (1<<12):0;
	dwJoyButtons |= (dwButtons & (JOY_BUTTON14&~JOY_BUTTON1)) ? (1<<13):0;

	// handle change in joystick buttons
	dwJoyButtons ^= dwPrevJoyButtons;
	dwPrevJoyButtons ^= dwJoyButtons;
	
	if (dwJoyButtons)
	{
		UpdateModifiers();
		i = 1;
		j = HKR_VKEY_JOY0;
		do
		{
		 	if (dwJoyButtons & i)
			{
				if (dwPrevJoyButtons & i)		// joystick button was pressed
	 				result |= KeyPressedEvent(j);
				else	// else button was released
					result |= KeyReleasedEvent(j);	
			}
			i <<= 1;
			j++;
		} while (i);
	}

	// handle change in joy POV
	if (dwPrevPOV != dwJoyPOV)
	{
		i = dwPrevPOV;
		dwPrevPOV = dwJoyPOV;
		UpdateModifiers();

	 	switch (i)
		{
			case JOY_POVFORWARD:
				result |= KeyReleasedEvent(HKR_VKEY_POVUP);
				break;
			case JOY_POVRIGHT:
				result |= KeyReleasedEvent(HKR_VKEY_POVRIGHT);
				break;
			case JOY_POVBACKWARD:
				result |= KeyReleasedEvent(HKR_VKEY_POVDOWN);
				break;
			case JOY_POVLEFT:
				result |= KeyReleasedEvent(HKR_VKEY_POVLEFT);
				break;
		}

	 	switch (dwJoyPOV)
		{
			case JOY_POVFORWARD:
				result |= KeyPressedEvent(HKR_VKEY_POVUP);
				break;
			case JOY_POVRIGHT:
				result |= KeyPressedEvent(HKR_VKEY_POVRIGHT);
				break;
			case JOY_POVBACKWARD:
				result |= KeyPressedEvent(HKR_VKEY_POVDOWN);
				break;
			case JOY_POVLEFT:
				result |= KeyPressedEvent(HKR_VKEY_POVLEFT);
				break;
		}
	}

	// handle change in joy DIR
	if (dwPrevDIR != dwJoyDIR)
	{
		i = dwPrevDIR;
		dwPrevDIR = dwJoyDIR;
		UpdateModifiers();

		if (i)
			result |= KeyReleasedEvent(i);
		if (dwJoyDIR)
			result |= KeyPressedEvent(dwJoyDIR);
	}

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 HotkeyEvent::GetHotkeyState (U32 hotkey)
{
	BOOL32 result=0;
	DWORD i;
	
	for (i=0; i < dwNumKeys; i++)
		if (pKeyData[i].symbol == hotkey)
			if ((result = pKeyData[i].bPressed) != 0)
				break;

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 HotkeyEvent::IsAnyHotkeyPressed (void)
{
	BOOL32 result=0;
	DWORD i;
	
	for (i=0; i < dwNumKeys; i++)
		if ((result = pKeyData[i].bPressed) != 0)
			break;

	return result;
}
//--------------------------------------------------------------------------//
//
GENRESULT HotkeyEvent::Enable (void)
{
	bEnabled=1;
	return GR_OK;
}
//--------------------------------------------------------------------------//
//
GENRESULT HotkeyEvent::Disable (void)
{
	bEnabled=0;
	return GR_OK;
}
//--------------------------------------------------------------------------//
//
void HotkeyEvent::SendEvent (U32 eventNum)
{
	CONNECTION_NODE<IEventCallback> *pNode = (CONNECTION_NODE<IEventCallback> *) eventCallback.pClientList;

	while (pNode)
	{
		pNode->client->Notify(dwHotkeyMessage,
			reinterpret_cast<void *>(static_cast<std::uintptr_t>(eventNum)));
		pNode = pNode->pNext;
	}
}
//--------------------------------------------------------------------------//
//
static void NormalizeKeyText (KTEXT & keyText, DAHOTKEY hotkeys)
{
	if (keyText.numStrings < 2)
		return;

	U32 i=0;
	DAHOTKEY compare = hotkeys++;

	while (i+1 < keyText.numStrings)
	{
		if (compare->symbol == hotkeys->symbol)
		{
			// remove the entry that is larger
			// set compare to point to smaller
			if (*compare > *hotkeys)
			{
				keyText.remove_element(i);
				compare = hotkeys;
			}
			else  // current entry is smaller or the same size
			{
				keyText.remove_element(i+1);
			}
		}
		else
		{
			i++;
			compare = hotkeys;
		}

		hotkeys++;
	}
}
//--------------------------------------------------------------------------//
// perform bubble sort
// 
static BOOL32 DACOM_FASTCALL BubbleSortHotKeyList (DAHOTKEY lpKeyData, DWORD dwNum)
{
	DWORD i=0;
	BOOL32 result=0;
	DAHOTKEY_STRUCT tmp;
	
	dwNum--;
	while (i < dwNum)
	{
		if (lpKeyData[i] < lpKeyData[i+1])
		{
			//switch them
			tmp = lpKeyData[i];
			lpKeyData[i] = lpKeyData[i+1];
			lpKeyData[i+1] = tmp;
			
			result=1;
		}
		i++;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
static BOOL32 ReadExact (IFileSystem * file, HANDLE handle, void * buffer, DWORD size)
{
	DWORD bytesRead = 0;
	return file->ReadFile(handle, buffer, size, &bytesRead, nullptr) && bytesRead == size;
}
//--------------------------------------------------------------------------//
//
GENRESULT HotkeyEvent::init (HKEVENTDESC *lpDesc)
{
	GENRESULT result = GR_OK;
	HANDLE hFile1 = INVALID_HANDLE_VALUE;
	DAFILEDESC desc = "Version";
	DWORD dwVersion = 0, dwAddedMemory = 0, dwKeyTextSize = 0, binarySize = 0;
	DWORD dwSortKeys;
	constexpr DWORD binaryHeaderSize = sizeof(U32) * 2U;
	constexpr DWORD textHeaderSize = sizeof(U32) * 2U;

	if (lpDesc == nullptr || lpDesc->file == nullptr)
	{
		result = GR_INVALID_PARMS;
		goto Done;
	}

	// check the version

	if ((hFile1 = lpDesc->file->OpenChild(&desc)) == INVALID_HANDLE_VALUE)
	{
		result = GR_GENERIC;
		goto Done;
	}

	if (lpDesc->file->GetFileSize(hFile1) != sizeof(dwVersion) ||
		!ReadExact(lpDesc->file, hFile1, &dwVersion, sizeof(dwVersion)) ||
		dwVersion != HKEVENT_VERSION)
	{
		result = GR_GENERIC;
		goto Done;
	}

	lpDesc->file->CloseHandle(hFile1);
	hFile1 = INVALID_HANDLE_VALUE;

	desc.lpFileName = "Binary data";
	if ((hFile1 = lpDesc->file->OpenChild(&desc)) == INVALID_HANDLE_VALUE)
	{
		result = GR_GENERIC;
		goto Done;
	}

	// find out how much memory to allocate for the instance

	binarySize = lpDesc->file->GetFileSize(hFile1);
	if (binarySize < binaryHeaderSize ||
		!ReadExact(lpDesc->file, hFile1, &dwNumKeys, sizeof(dwNumKeys)) ||
		!ReadExact(lpDesc->file, hFile1, &dwSizeOfKey, sizeof(dwSizeOfKey)) ||
		dwSizeOfKey != sizeof(DAHOTKEY_STRUCT) ||
		dwNumKeys > ((std::numeric_limits<DWORD>::max)() - binaryHeaderSize) /
			dwSizeOfKey)
	{
		result = GR_GENERIC;
		goto Done;
	}

	dwAddedMemory = dwNumKeys * dwSizeOfKey;
	if (binarySize != binaryHeaderSize + dwAddedMemory)
	{
		result = GR_GENERIC;
		goto Done;
	}

	if (dwAddedMemory != 0)
	{
		pKeyData = static_cast<DAHOTKEY>(DA_HEAP_MALLOC(HEAP, dwAddedMemory, "Hotkey data"));
		if (pKeyData == nullptr)
		{
			result = GR_OUT_OF_MEMORY;
			goto Done;
		}
		if (!ReadExact(lpDesc->file, hFile1, pKeyData, dwAddedMemory))
		{
			result = GR_GENERIC;
			goto Done;
		}
	}

	lpDesc->file->CloseHandle(hFile1);
	hFile1 = INVALID_HANDLE_VALUE;
	// get the size of key text
	
	desc.lpFileName = "Keyboard text";
	if ((hFile1 = lpDesc->file->OpenChild(&desc)) == INVALID_HANDLE_VALUE)
	{
		result = GR_GENERIC;
		goto Done;
	}

	dwKeyTextSize = lpDesc->file->GetFileSize(hFile1);
	if (dwKeyTextSize < textHeaderSize)
	{
		result = GR_GENERIC;
		goto Done;
	}
	if ((pKeyText = static_cast<KTEXT *>(DA_HEAP_MALLOC(HEAP, dwKeyTextSize,
		"Hotkey Text"))) == nullptr)
	{
		result = GR_OUT_OF_MEMORY;
		goto Done;
	}
	if (!ReadExact(lpDesc->file, hFile1, pKeyText, dwKeyTextSize) ||
		pKeyText->numStrings != dwNumKeys ||
		(pKeyText->numStrings != 0 &&
			(pKeyText->sizeOfString == 0 || pKeyText->numStrings >
				((std::numeric_limits<DWORD>::max)() - textHeaderSize) /
				pKeyText->sizeOfString)) ||
		dwKeyTextSize != textHeaderSize + pKeyText->numStrings * pKeyText->sizeOfString)
	{
		result = GR_GENERIC;
		goto Done;
	}
	for (DWORD index = 0; index < pKeyText->numStrings; ++index)
	{
		const char * text = pKeyText->strings + index * pKeyText->sizeOfString;
		if (std::memchr(text, '\0', pKeyText->sizeOfString) == nullptr)
		{
			result = GR_GENERIC;
			goto Done;
		}
	}

	// done with the file
	lpDesc->file->CloseHandle(hFile1);
	hFile1 = INVALID_HANDLE_VALUE;

	// create proper ktext array (remove duplicates)

	NormalizeKeyText(*pKeyText, pKeyData);

	// sort the keys

	dwSortKeys = dwNumKeys;
	while (dwSortKeys>1 && BubbleSortHotKeyList(pKeyData, dwSortKeys))
		dwSortKeys--;

	dwJoyMessage = lpDesc->joyMessage;
	dwHotkeyMessage = lpDesc->hotkeyMessage;

Done:
	if (hFile1 != INVALID_HANDLE_VALUE)
		lpDesc->file->CloseHandle(hFile1);
	
	return result;
}
//--------------------------------------------------------------------------//
//
void RegisterHotkeyEventServer (ICOManager *DACOM)
{
	IComponentFactory * pServer = new DAComponentFactory2<DAComponentAggregate<HotkeyEvent>, HKEVENTDESC> (interface_name);

	if (pServer)
	{
		DACOM->RegisterComponent(pServer, interface_name);
		pServer->Release();
	}
}
//----------------------------------------------------------------------------
//--------------------------END HKEvent.cpp-----------------------------------
//----------------------------------------------------------------------------
