#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppMissionMedia.h"
#include <DACOM.H>
#include "ITeletype.h"
#include <TComponent.h>
#include <EventSys.h>
#include <IConnection.h>
#include <TSmartPointer.h>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <vector>

namespace
{
struct NativeMissionTeletype : ITeletype, IEventCallback
{
    BEGIN_DACOM_MAP_INBOUND(NativeMissionTeletype)
    DACOM_INTERFACE_ENTRY(ITeletype)
    DACOM_INTERFACE_ENTRY(IEventCallback)
    END_DACOM_MAP()

    struct Entry
    {
        U32 id;
        U32 lifetime;
        U32 elapsed = 0;
        bool dead = false;
        bool ignorePause = false;
    };

    ~NativeMissionTeletype()
    {
        if (EVENTSYS && eventHandle)
        {
            COMPTR<IDAConnectionPoint> connection;
            if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
                connection->Unadvise(eventHandle);
        }
    }

    bool Init()
    {
        COMPTR<IDAConnectionPoint> connection;
        return EVENTSYS &&
            EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK &&
            connection->Advise(static_cast<IEventCallback *>(this), &eventHandle) == GR_OK;
    }

    U32 CreateTeletypeDisplay(const wchar_t *text, RECT, U32, COLORREF, U32 lifetime,
                              U32 textTime, bool, bool) override
    {
        return Create(text, lifetime, textTime, false);
    }
    U32 CreateTeletypeDisplayEx(const wchar_t *text, RECT, U32, COLORREF, U32 lifetime,
                                U32 textTime, bool, bool ignorePause) override
    {
        return Create(text, lifetime, textTime, ignorePause);
    }
    BOOL32 IsPlaying(U32 id) override
    {
        if (id == 0) return FALSE;
        for (const Entry &entry : entries)
            if (entry.id == id) return TRUE;
        return FALSE;
    }
    void Flush() override { entries.clear(); }
    U32 GetLastTeletypeID() override { return lastID; }
    void SetLastTeletypeID(U32 id) override { lastID = id; }

    GENRESULT COMAPI Notify(U32 message, void *parameter) override
    {
        if (message == CQE_MISSION_CLOSE) { Flush(); return GR_OK; }
        if (message != CQE_UPDATE) return GR_OK;
        const std::uintptr_t encoded = reinterpret_cast<std::uintptr_t>(parameter);
        if (encoded > std::numeric_limits<U32>::max()) return GR_INVALID_PARMS;
        const U32 dt = static_cast<U32>(encoded) >> 10U;
        for (auto it = entries.begin(); it != entries.end();)
        {
            if (it->dead) it = entries.erase(it);
            else ++it;
        }
        for (Entry &entry : entries)
        {
            if (CQFLAGS.bGamePaused && !entry.ignorePause) continue;
            entry.elapsed += dt;
            if (entry.lifetime && entry.elapsed > entry.lifetime) entry.dead = true;
        }
        return GR_OK;
    }

    U32 Create(const wchar_t *text, U32 lifetime, U32 textTime, bool ignorePause)
    {
        if (!skipVisuals || !text || !textTime || lastID == std::numeric_limits<U32>::max())
        {
            std::fputs("Native mission teletype creation failed\n", stderr);
            std::abort();
        }
        const U32 id = ++lastID;
        entries.push_back({id, lifetime, 0, false, ignorePause});
        std::fprintf(stderr, "Native teletype visuals explicitly disabled; timed handle %u lifetime %u ms\n",
                     unsigned(id), unsigned(lifetime));
        return id;
    }

    bool skipVisuals = false;
    U32 eventHandle = 0;
    U32 lastID = 0;
    std::vector<Entry> entries;
};
}

namespace FrontierX
{
ITeletype *CreateNativeMissionTeletype(bool skipVisuals)
{
    if (!skipVisuals) return nullptr;
    auto *teletype = new DAComponent<NativeMissionTeletype>;
    teletype->skipVisuals = true;
    if (!teletype->Init())
    {
        teletype->Release();
        return nullptr;
    }
    return teletype;
}
}
