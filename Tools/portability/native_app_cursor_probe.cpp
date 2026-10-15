#include "Include/NativeAppGlobals.h"
#include "Cursor.h"
#include "Startup.h"
#include "resource.h"

#include <EventSys.h>
#include <IConnection.h>
#include <SDL3/SDL.h>

#include <cstring>
#include <iostream>

namespace
{
struct CursorClient : IResourceClient
{
	GENRESULT QueryInterface(const C8 *name, void **instance) override
	{
		if (!instance) return GR_INVALID_PARMS;
		*instance = nullptr;
		if (std::strcmp(name, "IResourceClient") != 0) return GR_INTERFACE_UNSUPPORTED;
		*instance = static_cast<IResourceClient *>(this);
		AddRef();
		return GR_OK;
	}
	U32 AddRef() override { return ++references; }
	U32 Release() override { return --references; }
	BOOL32 Notify(IResource *, U32 message, void *) override
	{
		if (message == RESMSG_RES_UNOWNED) ++unowned;
		return TRUE;
	}
	U32 references = 1;
	U32 unowned = 0;
};
}

int main()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "Native cursor skipped: " << SDL_GetError() << '\n';
		return 77;
	}
	const char *driver = SDL_GetCurrentVideoDriver();
	if (!driver || std::strcmp(driver, "dummy") == 0)
	{
		std::cout << "Native cursor skipped: no system-cursor video driver\n";
		SDL_Quit();
		return 77;
	}

	CreateGlobalComponents();
	if (!CURSOR || !CURSOR->IsValid() || CURSOR->GetCursor() != IDC_CURSOR_DEFAULT ||
		!CURSOR->SetCursor(IDC_CURSOR_MOVEOBJECT) ||
		CURSOR->GetCursor() != IDC_CURSOR_MOVEOBJECT ||
		!CURSOR->SetCursor(IDC_CURSOR_BUILD) ||
		CURSOR->GetCursor() != IDC_CURSOR_BUILD ||
		CURSOR->SetCursor(0xFFFFFFFFU) ||
		CURSOR->GetCursor() != IDC_CURSOR_BUILD)
	{
		std::cerr << "Native cursor startup, original build resource or transactional rejection failed\n";
		return 1;
	}
	// Original ObjComm, SysMap and MScroll requests must retain their resource ID.
	const U32 required[] = {
		IDC_CURSOR_DEFAULT, IDC_CURSOR_ARROW, IDC_CURSOR_BUILD,
		IDC_CURSOR_MOVEOBJECT, IDC_CURSOR_ATTACK, IDC_CURSOR_BAN,
		IDC_CURSOR_HARVEST, IDC_CURSOR_JUMP, IDC_CURSOR_FABREPAIR,
		IDC_CURSOR_REPAIR, IDC_CURSOR_RESUPPLY, IDC_CURSOR_SELLOBJECT,
		IDC_CURSOR_DEFEND, IDC_CURSOR_PATROL, IDC_CURSOR_RALLY,
		IDC_CURSOR_SPECIAL_ATTACK, IDC_CURSOR_MICRO_MOVE,
		IDC_CURSOR_MICRO_GOTO, IDC_CURSOR_MICRO_RALLY,
		IDC_CURSOR_LEFT, IDC_CURSOR_RIGHT, IDC_CURSOR_UP, IDC_CURSOR_DOWN,
		IDC_CURSOR_UPLEFT, IDC_CURSOR_UPRIGHT,
		IDC_CURSOR_DOWNLEFT, IDC_CURSOR_DOWNRIGHT
	};
	for (const U32 id : required)
	{
		if (!CURSOR->SetCursor(id) || CURSOR->GetCursor() != id) return 10;
	}
	CURSOR->SetBusy(TRUE);
	CURSOR->SetBusy(TRUE);
	if (CURSOR->IsValid() || !CURSOR->SetCursor(IDC_CURSOR_HARVEST) ||
		CURSOR->GetCursor() != IDC_CURSOR_HARVEST || CURSOR->SetCursor(0xFFFFFFFFU) ||
		CURSOR->GetCursor() != IDC_CURSOR_HARVEST) return 2;
	CURSOR->SetBusy(FALSE);
	if (CURSOR->IsValid()) return 3;
	CURSOR->SetBusy(FALSE);
	if (!CURSOR->IsValid() || CURSOR->SetDefaultState() != GR_OK ||
		CURSOR->GetCursor() != IDC_CURSOR_DEFAULT) return 4;
	IDAConnectionPoint *connection = nullptr;
	IEventCallback *events = nullptr;
	CursorClient client;
	U32 handle = 0;
	if (CURSOR->QueryOutgoingInterface("IResourceClient", &connection) != GR_OK ||
		CURSOR->QueryInterface("IEventCallback", reinterpret_cast<void **>(&events)) != GR_OK ||
		connection->Advise(&client, &handle) != GR_OK ||
		!CURSOR->GetOwnership(&client, RES_PRIORITY_MEDIUM) ||
		!CURSOR->ReleaseOwnership(&client)) return 8;
	events->Notify(CQE_UPDATE, nullptr);
	if (client.unowned != 1 || connection->Unadvise(handle) != GR_OK) return 9;
	events->Release();
	connection->Release();
	CURSOR->EnableCursor(false);
	if (CURSOR->IsCursorEnabled()) return 5;
	CURSOR->EnableCursor(true);
	if (!CURSOR->IsCursorEnabled()) return 6;
	CleanupGlobals();
	if (CURSOR) return 7;
	SDL_Quit();
	std::cout << "Native App cursor: 27 original resources, busy nesting, transactional rejection and cleanup\n";
	return 0;
}
