//--------------------------------------------------------------------------//
//                                                                          //
//                               HotButton.cpp                              //
//                                                                          //
//                  COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/Src/HotButton.cpp 43    9/25/00 9:55p Tmauer $
*/			    
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
#ifdef _WIN32
#include "pch.h"
#include <globals.h>
#else
#include "NativeAppGlobals.h"
#include <SYSTEM.H>
#include "NativeAppUiInput.h"
#include "NativeAppTextResources.h"
#include "MyVertex.h"
#include <IRenderPrimitive.h>
#include <VFX.H>
#include <algorithm>
#include <cwchar>
#include <cstring>
#include <cstdlib>
#include <cstdio>
using namespace FrontierX::NativeAppUi;
#define _wcsdup DuplicateWide
#define _ltow DecimalToWide
#include <TManager.h>
#endif

#include "IHotButton.h"
#include "Startup.h"
#include "BaseHotRect.h"
#include "GenData.h"
#include <DHotButton.h>
#include "DrawAgent.h"
#include "IShapeLoader.h"
#include "SFX.h"
#include "IInterfaceManager.h"
#include <DSounds.h>

#include <cstdint>
#include <TComponent.h>
#include <Filesys.h>
#include <HKEvent.h>

#if !defined(_WIN32)
#include "Include/NativeRetailHudHotkey.h"

namespace FrontierX
{
bool CheckNativeHotButtonCommands()
{
    AGGDESC descriptor("IEventSystem", "EventSystem");
    COMPTR<IAggregateComponent> aggregate;
    COMPTR<IEventSystem> events;
    COMPTR<ISystemComponent> eventOwner;
    if (!DACOM || DACOM->CreateInstance(&descriptor, aggregate) != GR_OK ||
        aggregate->QueryInterface(IID_IEventSystem, events) != GR_OK ||
        aggregate->QueryInterface(IID_ISystemComponent, eventOwner) != GR_OK ||
        aggregate->Initialize() != GR_OK || events->Initialize(0) != GR_OK)
    {
        std::fprintf(stderr, "Native hot-button isolated event owner unavailable\n");
        return false;
    }
    struct Command { U32 resource; U32 expected; };
    // Independent representative bindings cover every HUD command family,
    // including resources without text and mutable multi-button states.
    const Command commands[] = {
        {0, 0}, {0xFFFFFFFFU, 0xFFFFFFFFU},
        {6, IDH_CANCEL_COMMAND}, {12, IDH_CONTEXT_A}, {37, IDH_CONTEXT_Z},
        {45, IDH_FABRICATOR}, {46, IDH_FIGHTER_NORMAL}, {47, IDH_FIGHTER_PATROL},
        {54, IDH_FORM_FLEET}, {56, IDH_GAME_OPTIONS},
        {78, IDH_MISSION_OBJ}, {81, IDH_PATROL},
        {84, IDH_QUICK_LOAD}, {85, IDH_QUICK_SAVE}, {86, IDH_RALLY_POINT},
        {88, IDH_REPAIR}, {89, IDH_REPAIR_FLEET}, {92, IDH_RESUPPLY_FLEET},
        {93, IDH_ROTATE_0_WORLD}, {109, IDH_SELECT_ALL_FLEET},
        {112, IDH_SELL}, {113, IDH_SPECIAL_ABILITY},
        {114, IDH_SPECIAL_AEGIS}, {115, IDH_SPECIAL_ASSAULT},
        {118, IDH_SPECIAL_CLOAK}, {122, IDH_SPECIAL_MIMIC},
        {127, IDH_SPECIAL_REPULSOR}, {131, IDH_SPECIAL_TEMPEST},
        {133, IDH_SPECIAL_WORMHOLE},
        {134, IDH_STANCE_ATTACK}, {135, IDH_STANCE_DEFENSIVE},
        {136, IDH_STANCE_IDLE}, {137, IDH_STANCE_STANDGROUND},
        {138, IDH_SUPPLY_STANCE_FULLYAUTO}, {139, IDH_SUPPLY_STANCE_NOAUTO},
        {140, IDH_SUPPLY_STANCE_RESUPPLY}, {143, IDH_TOGGLE_TOOLBAR},
        {147, IDH_ZOOM_IN}, {148, IDH_ZOOM_OUT}
    };
    for (const auto &command : commands)
    {
        const U32 stored = NativeRetailHudHotkey(command.resource);
        if (stored != command.expected || PostNativeHudHotkey(events, stored) != GR_OK)
            return false;
    }
    // Already canonical commands, including physical F8, are posted unchanged.
    const U32 canonical[] = {IDH_QUICK_LOAD, IDH_SELL, IDH_REPAIR,
                             IDH_SPECIAL_ABILITY, IDH_FLEET_SPORDER_1};
    for (const U32 command : canonical)
        if (PostNativeHudHotkey(events, command) != GR_OK) return false;
    eventOwner->Update();
    for (U32 index = 0; index < sizeof(commands)/sizeof(commands[0]); ++index)
    {
        U32 message = 0;
        void *parameter = nullptr;
        if (events->Peek(index, &message, &parameter) != GR_OK ||
            message != CQE_HOTKEY || reinterpret_cast<std::uintptr_t>(parameter) != commands[index].expected)
        {
            std::fprintf(stderr, "Native hot-button command %u: expected %u, received %zu (message %u)\n",
                index, commands[index].expected, reinterpret_cast<std::uintptr_t>(parameter), message);
            return false;
        }
    }
    for (U32 index = 0; index < sizeof(canonical)/sizeof(canonical[0]); ++index)
    {
        U32 message = 0;
        void *parameter = nullptr;
        if (events->Peek(sizeof(commands)/sizeof(commands[0]) + index, &message, &parameter) != GR_OK ||
            message != CQE_HOTKEY || reinterpret_cast<std::uintptr_t>(parameter) != canonical[index])
            return false;
    }
    return true;
}
}
#endif

//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE HotButton : BaseHotRect, IHotButton 
{
	//
	// incoming interface map
	//
	BEGIN_DACOM_MAP_INBOUND(HotButton)
	DACOM_INTERFACE_ENTRY(IResourceClient)
	DACOM_INTERFACE_ENTRY(IHotButton)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	DACOM_INTERFACE_ENTRY(BaseHotRect)
	DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
	DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
	END_DACOM_MAP()

	//
	// data items
	// 
	PGENTYPE pArchetype;
	HSOUND hSound;
	COMPTR<IDrawAgent> shapes[GTHBSHP_MAX_SHAPES];

	U32 controlID;					// set by owner
	HBTNTXT::BUTTON_TEXT buttonText;	
	wchar_t *szButtonText;
	U32 hotkeyID;
	S32 numericValue;

	bool bDisabled:1;
	bool bPushState:1;			// used for selection state
	bool bPostMessageBlocked:1;
	bool bContextBehavior:1;
	bool bHighlight:1;

	//
	// class methods
	//

	HotButton (void) : BaseHotRect ( (IDAComponent *)NULL )
	{
	}

	virtual ~HotButton (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IHotButton methods  */

	virtual void InitHotButton (const HOTBUTTON_DATA & data, BaseHotRect * parent, IShapeLoader * loader); 

	virtual void InitBuildButton (const BUILDBUTTON_DATA & data, BaseHotRect * parent, IShapeLoader * loader); 

	virtual void InitResearchButton (const RESEARCHBUTTON_DATA & data, BaseHotRect * parent, IShapeLoader * loader); 

	virtual void InitMultiHotButton (const MULTIHOTBUTTON_DATA & data, BaseHotRect * parent, IShapeLoader * loader); 

	virtual void EnableButton (bool bEnable);

	virtual bool GetEnableState (void);

	virtual void SetVisible (bool bVisible);

	virtual bool GetVisible (void);
	
	virtual void SetTextID (U32 textID);	// set resource ID for text

	virtual void SetTextString (const wchar_t * szString);

	virtual void SetPushState (bool bPressed);

	virtual bool GetPushState (void);

	virtual void SetControlID (U32 id);

	virtual U32 GetControlID (void);

	virtual void SetNumericValue (S32 value);

	virtual S32 GetNumericValue (void);

	virtual void EnableContextMenuBehavior (void)
	{
		bContextBehavior = true;
	}

	virtual void SetHighlightState (bool _bHighlight)
	{
		bHighlight = _bHighlight;
	}

	/* BaseHotRect methods */
	virtual void setStatus (void);


	/* HotButton methods */

	virtual GENRESULT __stdcall	Notify (U32 message, void *param);

	void init (PGENTYPE _pArchetype, HSOUND _hSound);

	void draw (void);

	IDAComponent * getBase (void)
	{
		return static_cast<IEventCallback *> (this);
	}

	void onLeftButtonDown (void)
	{
		if (bDisabled == false)
		{
			if (bAlert)
				doLPushAction();
		}
	}

	void onRightButtonDown (void)
	{
		if (bDisabled == false)
		{
			if (bAlert)
				doRPushAction();
		}
	}

	void doLPushAction (void)
	{
		if (bPostMessageBlocked==false)
		{
			if (hotkeyID != 0)
				{
#if defined(_WIN32)
                EVENTSYS->Post(CQE_HOTKEY, (void *)hotkeyID);
#else
                FrontierX::PostNativeHudHotkey(EVENTSYS, hotkeyID);
#endif
            }
			parent->PostMessage(CQE_LHOTBUTTON, (void*)controlID);
			SFXMANAGER->Play(hSound);
			bPostMessageBlocked = true;
		}
	}

	void doRPushAction (void)
	{
		if (bPostMessageBlocked==false)
		{
			parent->PostMessage(CQE_RHOTBUTTON, (void*)controlID);
			bPostMessageBlocked = true;
		}
	}

	void doDblPushAction (void)
	{
		parent->PostMessage(CQE_LDBLHOTBUTTON, (void*)controlID);
	}
};
//--------------------------------------------------------------------------//
//
HotButton::~HotButton (void)
{
	GENDATA->Release(pArchetype);
	::free(szButtonText);
}
//--------------------------------------------------------------------------//
//
void HotButton::InitHotButton (const HOTBUTTON_DATA & data, BaseHotRect * _parent, IShapeLoader * loader)
{
	if (parent == 0)
	{
		parent = _parent;
		lateInitialize(_parent->GetBase());
		resPriority = _parent->resPriority+1;
	}

	if ((buttonText = data.buttonText) != 0)
		szButtonText = _wcsdup(_localLoadStringW(buttonText));
	//	wcsncpy(szButtonText, _localLoadStringW(buttonText), sizeof(szButtonText)/sizeof(wchar_t));

#if defined(_WIN32)
	hotkeyID = data.hotkey;
#else
    hotkeyID = FrontierX::NativeRetailHudHotkey(data.hotkey);
#endif
	statusTextID = data.buttonInfo;
	hintboxID = data.buttonHint;

	EnableButton(data.bDisabled==false);

	//
	// load the shapes
	//
	int base;
	if ((base = data.baseImage) == 0)
		base++;
	
	int i;
	for (i = 0; i < GTHBSHP_MAX_SHAPES; i++)
		loader->CreateDrawAgent(i+base, shapes[i]);

	U16 width, height;
	if (shapes[0])
		shapes[0]->GetDimensions(width, height);
	else
	{
		width = height = 50;
	}

	screenRect.left = IDEAL2REALX(data.xOrigin) + parent->screenRect.left;
	screenRect.top = IDEAL2REALY(data.yOrigin) + parent->screenRect.top;
	screenRect.bottom = screenRect.top + height - 1;
	screenRect.right = screenRect.left + width - 1;

	{
		static FILE* _hbf = nullptr;
		if (!_hbf) _hbf = fopen("debug/hotbutton_diag.txt", "w");
		if (_hbf) {
			fprintf(_hbf, "HotButton: baseImage=%u xOrig=%d yOrig=%d w=%d h=%d parentL=%d parentT=%d -> L=%d T=%d R=%d B=%d txt=%d\n",
				(unsigned)data.baseImage, (int)data.xOrigin, (int)data.yOrigin,
				(int)width, (int)height,
				(int)parent->screenRect.left, (int)parent->screenRect.top,
				(int)screenRect.left, (int)screenRect.top,
				(int)screenRect.right, (int)screenRect.bottom,
				(int)data.buttonText);
			fflush(_hbf);
		}
	}

	//
	// calculate text offset
	//

	if (controlID == 0)
		controlID = buttonText;
}
//--------------------------------------------------------------------------//
//
void HotButton::InitBuildButton (const BUILDBUTTON_DATA & data, BaseHotRect * parent, IShapeLoader * loader)
{
	CQBOMB0("Wrong init called.");
}
//--------------------------------------------------------------------------//
//
void HotButton::InitResearchButton (const RESEARCHBUTTON_DATA & data, BaseHotRect * _parent, IShapeLoader * loader)
{
	CQBOMB0("Wrong init called.");
}
//--------------------------------------------------------------------------//
//
void HotButton::InitMultiHotButton (const MULTIHOTBUTTON_DATA & data, BaseHotRect * _parent, IShapeLoader * loader)
{
	CQBOMB0("Wrong init called.");
}
//--------------------------------------------------------------------------//
//
void HotButton::EnableButton (bool bEnable)
{
	if (bEnable == bDisabled)	// if we are changing state
	{
		bDisabled = !bEnable;
		bPushState = false;
	}
}
//--------------------------------------------------------------------------//
//
bool HotButton::GetEnableState (void)
{
	return !bDisabled;
}
//--------------------------------------------------------------------------//
//
void HotButton::SetVisible (bool bVisible)
{
	bInvisible = !bVisible;

	if (bInvisible)
	{
		desiredOwnedFlags = 0;
		releaseResources();
	}
}
//--------------------------------------------------------------------------//
//
bool HotButton::GetVisible (void)
{
	return !bInvisible;
}
//--------------------------------------------------------------------------//
//
void HotButton::SetTextID (U32 textID)
{
	buttonText = static_cast<HBTNTXT::BUTTON_TEXT>(textID);
//	wcsncpy(szButtonText, _localLoadStringW(buttonText), sizeof(szButtonText)/sizeof(wchar_t));
	if (szButtonText)
	{
		::free(szButtonText);
	}
	szButtonText = _wcsdup(_localLoadStringW(buttonText));
}
//--------------------------------------------------------------------------//
//
void HotButton::SetTextString (const wchar_t * szString)
{
	buttonText = HBTNTXT::NOTEXT;
//	wcsncpy(szButtonText, szString, sizeof(szButtonText)/sizeof(wchar_t));
	if (szButtonText)
	{
		::free(szButtonText);
		szButtonText = 0;
	}
	if (szString && szString[0])
		szButtonText = _wcsdup(szString);
}
//--------------------------------------------------------------------------//
//
void HotButton::SetPushState (bool bPressed)
{
	bPushState = bPressed;
}
//--------------------------------------------------------------------------//
//
bool HotButton::GetPushState (void)
{
	return bPushState;
}
//--------------------------------------------------------------------------//
//
void HotButton::SetControlID (U32 id)
{
	controlID = id;
}
//--------------------------------------------------------------------------//
//
U32 HotButton::GetControlID (void)
{
	return controlID;
}
//--------------------------------------------------------------------------//
//
void HotButton::SetNumericValue (S32 value)
{
	numericValue = value;	
}
//--------------------------------------------------------------------------//
//
S32 HotButton::GetNumericValue (void)
{
	return numericValue;
}
//--------------------------------------------------------------------------//
//
GENRESULT HotButton::Notify (U32 message, void *param)
{
#ifdef _WIN32
    MSG *msg = (MSG *) param;
#else
    MessageFields fields;
    if (!DecodeMessage(message, param, fields)) return GR_INVALID_PARMS;
    const MessageFields *msg = &fields;
#endif

	if (bInvisible==0)
	switch (message)
	{
	case CQE_ENDFRAME:
		if (bContextBehavior && (CQFLAGS.bGamePaused || DEFAULTS->GetDefaults()->bEditorMode))	// do not allow buttons to work in this mode
			bAlert = 0;
		draw();
		break;

	case WM_MOUSEMOVE:
		BaseHotRect::Notify(message, param);		// switch focus, if needed
		if (bContextBehavior && (CQFLAGS.bGamePaused || DEFAULTS->GetDefaults()->bEditorMode))	// do not allow buttons to work in this mode
			bAlert = 0;
		if (bAlert)
		{
			desiredOwnedFlags = (RF_CURSOR|RF_STATUS|RF_HINTBOX);
			grabAllResources();
		}
		else
		if (actualOwnedFlags)
		{
			desiredOwnedFlags = 0;
			releaseResources();
		}
		break;

	case WM_LBUTTONDBLCLK:
		BaseHotRect::Notify(message, param);		// switch focus, if needed
		if (bContextBehavior && (CQFLAGS.bGamePaused || DEFAULTS->GetDefaults()->bEditorMode))	// do not allow buttons to work in this mode
			bAlert = 0;
		if (bAlert)
		{
			doDblPushAction();
			return GR_GENERIC;
		}
		return GR_OK;

	case WM_LBUTTONDOWN:
		BaseHotRect::Notify(message, param);		// switch focus, if needed
		if (bContextBehavior && (CQFLAGS.bGamePaused || DEFAULTS->GetDefaults()->bEditorMode))	// do not allow buttons to work in this mode
			bAlert = 0;
		onLeftButtonDown();
		if (bAlert)
			return GR_GENERIC;
		return GR_OK;

	case WM_RBUTTONDOWN:
		BaseHotRect::Notify(message, param);		// switch focus, if needed
		if (bContextBehavior && (CQFLAGS.bGamePaused || DEFAULTS->GetDefaults()->bEditorMode))	// do not allow buttons to work in this mode
			bAlert = 0;
		onRightButtonDown();
		if (bAlert)
			return GR_GENERIC;
		return GR_OK;

	case CQE_UPDATE:
		bPostMessageBlocked = false;
		break;

//	case WM_KEYDOWN:
//	case CQE_SLIDER:
//	case CQE_LIST_CARET_MOVED:
//	case CQE_LIST_SELECTION:
//	case CQE_BUTTON:
	case CQE_LHOTBUTTON:
	case CQE_RHOTBUTTON:
		parent->Notify(message, param);		// forward the message upward
		break;

/*	case CQE_SET_FOCUS:
		if (isInvisibleMenu())
		{
			onSetFocus(true);
			return GR_OK;
		}
		break;

	case CQE_KILL_FOCUS:
		if (isInvisibleMenu())
		{
			onSetFocus(false);
			return GR_OK;
		}
		break;
*/
	}

//	if (isInvisibleMenu() && message == CQE_ENDFRAME)
//		return GR_OK;

	return BaseHotRect::Notify(message, param);
}
//--------------------------------------------------------------------------//
//
void HotButton::draw (void)
{
	BATCH->set_state(RPR_BATCH,FALSE);
	U32 state;

    bool bFlash = INTERMAN->RenderHotkeyFlashing(hotkeyID);
	
	if(bFlash)
	{
		state = GTHBSHP_MOUSE_FOCUS;
	}
	else
	if (bDisabled)
	{
		state = GTHBSHP_DISABLED;
	}
	else
	{
		state = GTHBSHP_NORMAL;
		if (bAlert)
			state = GTHBSHP_MOUSE_FOCUS;
	}

	if (shapes[state])
		shapes[state]->Draw(0, screenRect.left, screenRect.top);
	if ((bPushState || bHighlight) && shapes[GTHBSHP_SELECTED]!=0)
		shapes[GTHBSHP_SELECTED]->Draw(0, screenRect.left, screenRect.top);
}
//--------------------------------------------------------------------------//
//
void HotButton::setStatus (void)
{
	wchar_t buffer[256];
	wchar_t ext[128];
	char keytext[128];
	wchar_t * ptr;
	int len;

	if (szButtonText)
	{
		ptr = szButtonText;
		len = wcslen(ptr);
		CQASSERT1(len < 128, "String too long: \"%S\"", ptr);
		memcpy(buffer, ptr, (len+1)*sizeof(*ptr));
		
		if (hotkeyID != 0 && (ptr = wcschr(buffer, '!')) != 0)
		{
			int i;

			wcscpy(ext, ptr+1);		// copy everything after the mark
            len = HOTKEY->GetHotkeyText(hotkeyID, keytext, sizeof(keytext));
			#ifdef _WIN32

			i = MultiByteToWideChar(CP_ACP, 0, keytext, len, ptr, sizeof(buffer)/sizeof(buffer[0]));
#else

			i = ConvertHotkeyToWide(keytext, len, ptr, sizeof(buffer)/sizeof(buffer[0]));
#endif
			ptr += i;
			wcscpy(ptr, ext);
			CQASSERT(wcslen(buffer) < sizeof(buffer)/sizeof(buffer[0]));
		}

		if ((ptr = wcschr(buffer, '#')) != 0)
		{
//			int i=0;

			wcscpy(ext, ptr+1);		// copy everything after the mark
			_ltow(numericValue, ptr, 10);
			wcscat(ptr, ext);
		}

		STATUS->SetTextString(buffer, STM_TOOLTIP);
	}
	else
	if (statusTextID)
		STATUS->SetText(statusTextID, STM_DEFAULT);
	else
		STATUS->SetDefaultState();

	STATUS->SetRect(screenRect);
}
//--------------------------------------------------------------------------//
//
void HotButton::init (PGENTYPE _pArchetype, HSOUND _hSound)
{
	pArchetype = _pArchetype;
	hSound = _hSound;
	cursorID = IDC_CURSOR_DEFAULT;
}
//--------------------------------------------------------------------------//
//-----------------------HotButton Factory class-------------------------------//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE HotButtonFactory : public ICQFactory
{
	//----------------------------------------------------------------------------
	//
	U32 factoryHandle;		// handles to callback

	PGENTYPE pArchetype;
	HSOUND hSound;		// sound played when hotbutton is pressed

	//
	// Interface mapping
	//

	BEGIN_DACOM_MAP_INBOUND(HotButtonFactory)
	DACOM_INTERFACE_ENTRY(ICQFactory)
	END_DACOM_MAP()

	HotButtonFactory (void) { }

	~HotButtonFactory (void);

	void init (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* ICQFactory methods */

	virtual HANDLE CreateArchetype (PGENTYPE pArchetype, GENBASE_TYPE objClass, void *data);

	virtual BOOL32 DestroyArchetype (HANDLE hArchetype);

	virtual GENRESULT CreateInstance (HANDLE hArchetype, IDAComponent ** pInstance);

	/* FontFactory methods */

	IDAComponent * getBase (void)
	{
		return static_cast<ICQFactory *> (this);
	}
};
//-----------------------------------------------------------------------------------------//
//
HotButtonFactory::~HotButtonFactory (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (GENDATA && GENDATA->QueryOutgoingInterface("ICQFactory", connection) == GR_OK)
		connection->Unadvise(factoryHandle);
}
//-----------------------------------------------------------------------------------------//
//
void HotButtonFactory::init (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (GENDATA->QueryOutgoingInterface("ICQFactory", connection) == GR_OK)
		connection->Advise(this, &factoryHandle);
}
//-----------------------------------------------------------------------------------------//
//
HANDLE HotButtonFactory::CreateArchetype (PGENTYPE _pArchetype, GENBASE_TYPE objClass, void *_data)
{
	{
		const char* _n = GENDATA->GetArchName(_pArchetype);
		#if defined(_WIN32)
		if (_n && strstr(_n, "Tab"))
#else
		if (objClass == GBT_HOTBUTTON && _n && strstr(_n, "Tab"))
#endif
		{
			GT_HOTBUTTON * _d = (GT_HOTBUTTON *) _data;
			FILE* _dbgf = fopen("debug/factory_debug.txt", "a");
			if (_dbgf) {
				fprintf(_dbgf, "HotButtonFactory::CreateArchetype name='%s' objClass=%d buttonType=%d\n",
					_n, (int)objClass, (int)_d->buttonType);
				fclose(_dbgf);
			}
		}
	}
	if (objClass == GBT_HOTBUTTON)
	{
		GT_HOTBUTTON * data = (GT_HOTBUTTON *) _data;
		if (data->buttonType == HOTBUTTONTYPE::HOTBUTTON)
		{
			CQASSERT(pArchetype == 0);

			pArchetype = _pArchetype;
			hSound = SFXMANAGER->Open(SFXMANAGER->GetGlobalSounds().defaultButton);

			return (HANDLE) 1;
		}
	}

	return 0;
}
//-----------------------------------------------------------------------------------------//
//
BOOL32 HotButtonFactory::DestroyArchetype (HANDLE hArchetype)
{
	CQASSERT(reinterpret_cast<std::uintptr_t>(hArchetype) == 1 && pArchetype != 0);

	pArchetype = 0;
	SFXMANAGER->CloseHandle(hSound);
	hSound = 0;
	return 1;
}
//-----------------------------------------------------------------------------------------//
//
GENRESULT HotButtonFactory::CreateInstance (HANDLE hArchetype, IDAComponent ** pInstance)
{
	CQASSERT(reinterpret_cast<std::uintptr_t>(hArchetype) == 1 && pArchetype != 0);
	HotButton * result = new DAComponent<HotButton>;

	result->init(pArchetype, hSound);
	*pInstance = result->getBase();
	return GR_OK;
}
//-----------------------------------------------------------------------------------------//
//
struct _hotbuttonfactory : GlobalComponent
{
	HotButtonFactory * factory;

	virtual void Startup (void)
	{
		factory = new DAComponent<HotButtonFactory>;
		#ifdef _WIN32
        AddToGlobalCleanupList(&factory);
#else
        AddToGlobalCleanupList(reinterpret_cast<IDAComponent **>(&factory));
#endif
	}

	virtual void Initialize (void)
	{
		factory->init();
	}
};

static _hotbuttonfactory startup;

//-----------------------------------------------------------------------------------------//
//----------------------------------End HotButton.cpp--------------------------------------//
//-----------------------------------------------------------------------------------------//
