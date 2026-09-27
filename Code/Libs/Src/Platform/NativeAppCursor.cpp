#include "Include/NativeAppGlobals.h"
#include "Cursor.h"
#include "Startup.h"
#include "TResource.h"
#include "resource.h"

#include <EventSys.h>
#include <IConnection.h>
#include <TComponent.h>
#include <TConnContainer.h>

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstdio>
#include <stdexcept>

namespace
{
SDL_SystemCursor CursorShape(U32 id)
{
	switch (id)
	{
	case IDC_CURSOR_DEFAULT:
	case IDC_CURSOR_ARROW: return SDL_SYSTEM_CURSOR_DEFAULT;
	case IDC_CURSOR_MOVEOBJECT: return SDL_SYSTEM_CURSOR_MOVE;
	case IDC_CURSOR_ATTACK: return SDL_SYSTEM_CURSOR_CROSSHAIR;
	case IDC_CURSOR_BAN: return SDL_SYSTEM_CURSOR_NOT_ALLOWED;
	default: throw std::invalid_argument("unsupported native cursor resource");
	}
}

struct NativeAppCursor : Resource<NativeAppCursor, ICursorResource>,
	ConnectionPointContainer<NativeAppCursor>, IEventCallback
{
	BEGIN_DACOM_MAP_INBOUND(NativeAppCursor)
	DACOM_INTERFACE_ENTRY(IResource)
	DACOM_INTERFACE_ENTRY(ICursorResource)
	DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
	END_DACOM_MAP()

	~NativeAppCursor()
	{
		if (event_handle_ && EVENTSYS)
		{
			IDAConnectionPoint *connection = nullptr;
			if (EVENTSYS->QueryOutgoingInterface("IEventCallback", &connection) == GR_OK)
			{
				connection->Unadvise(event_handle_);
				connection->Release();
			}
		}
		if (cursor_) SDL_DestroyCursor(cursor_);
		if (busy_cursor_) SDL_DestroyCursor(busy_cursor_);
	}

	BOOL32 SetCursor(U32 id) override
	{
		SDL_SystemCursor shape;
		try { shape = CursorShape(id); }
		catch (const std::invalid_argument &) { return FALSE; }
		SDL_Cursor *next = SDL_CreateSystemCursor(shape);
		if (!next) return FALSE;
		if (!SDL_SetCursor(busy_count_ ? BusyCursor() : next))
		{
			SDL_DestroyCursor(next);
			return FALSE;
		}
		if (cursor_) SDL_DestroyCursor(cursor_);
		cursor_ = next;
		resource_id_ = id;
		return TRUE;
	}

	U32 GetCursor() override { return resource_id_; }

	void SetBusy(BOOL busy) override
	{
		if (busy)
		{
			if (busy_count_ == 0 && !SDL_SetCursor(BusyCursor())) std::abort();
			++busy_count_;
		}
		else
		{
			if (busy_count_ == 0) std::abort();
			--busy_count_;
			if (busy_count_ == 0 && !SDL_SetCursor(cursor_)) std::abort();
		}
	}

	// SDL renders the hardware cursor; game frame drawing must not duplicate it.
	void DrawMouse(S32, S32) override {}
	BOOL32 IsValid() const override { return cursor_ != nullptr && busy_count_ == 0; }
	BOOL32 SetDefaultCursor() override { return SetCursor(IDC_CURSOR_DEFAULT); }
	// SDL window events already carry absolute client coordinates; the legacy
	// speed applies only to Cursor.cpp's exclusive DirectInput delta path.
	void SetCursorSpeed(S32) override {}
	void EnableCursor(bool enabled) override
	{
		if (!(enabled ? SDL_ShowCursor() : SDL_HideCursor())) std::abort();
		enabled_ = enabled;
	}
	bool IsCursorEnabled() const override { return enabled_; }
	HCURSOR GetWindowsCursor() override { return nullptr; }
	GENRESULT SetDefaultState() override
	{
		return SetDefaultCursor() ? GR_OK : GR_GENERIC;
	}
	void OnNoOwner() { if (!SetDefaultCursor()) std::abort(); }
	IDAComponent *GetBase() { return static_cast<ICursorResource *>(this); }
	GENRESULT Notify(U32 message, void *) override
	{
		if (message == CQE_UPDATE) updateResource();
		return GR_OK;
	}
	bool Subscribe()
	{
		if (!EVENTSYS) return false;
		IDAConnectionPoint *connection = nullptr;
		if (EVENTSYS->QueryOutgoingInterface("IEventCallback", &connection) != GR_OK)
			return false;
		const bool subscribed = connection->Advise(static_cast<IEventCallback *>(this),
			&event_handle_) == GR_OK;
		connection->Release();
		return subscribed;
	}

private:
	SDL_Cursor *BusyCursor()
	{
		if (!busy_cursor_) busy_cursor_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		if (!busy_cursor_) std::abort();
		return busy_cursor_;
	}
	SDL_Cursor *cursor_ = nullptr;
	SDL_Cursor *busy_cursor_ = nullptr;
	U32 resource_id_ = 0;
	U32 busy_count_ = 0;
	bool enabled_ = true;
	U32 event_handle_ = 0;
};

struct CursorStartup : GlobalComponent
{
	void Startup() override
	{
		cursor_ = new DAComponent<NativeAppCursor>;
		CURSOR = cursor_;
		AddToGlobalCleanupList(reinterpret_cast<IDAComponent **>(&CURSOR));
	}
	void Initialize() override
	{
		if (!CURSOR || !CURSOR->SetDefaultCursor())
		{
			std::fprintf(stderr, "Native App cursor initialization failed: %s\n", SDL_GetError());
			std::abort();
		}
		if (EVENTSYS && !cursor_->Subscribe())
			std::abort();
	}
	NativeAppCursor *cursor_ = nullptr;
};

CursorStartup startup;
}
