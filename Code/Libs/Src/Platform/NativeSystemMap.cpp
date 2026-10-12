#include "Include/NativeAppGlobals.h"
#include "FrontierX/NativeSystemMap.h"
#include <3dmath.h>
#include "SysMap.h"
#include "Sector.h"
#include "Startup.h"
#include "Camera.h"
#include "UserDefaults.h"
#include <EventSys.h>
#include <IConnection.h>
#include <TSmartPointer.h>
#include <cmath>
#include <cstdint>

#include <Filesys.h>
#include <TComponent.h>

#include <cstdio>
#include <cstdlib>

void ISystemMap::DrawHashSquare(Vector, SINGLE, COLORREF)
{
    std::fputs("Native system map operation is not implemented: DrawHashSquare\n", stderr);
    std::abort();
}

namespace
{
[[noreturn]] void Unsupported(const char *operation)
{
    std::fprintf(stderr, "Native system map operation is not implemented: %s\n", operation);
    std::abort();
}

struct NativeSystemMap : ISystemMap, IEventCallback
{
    BEGIN_DACOM_MAP_INBOUND(NativeSystemMap)
    DACOM_INTERFACE_ENTRY(ISystemMap)
    DACOM_INTERFACE_ENTRY(IEventCallback)
    END_DACOM_MAP()

    GENRESULT SetRect(tagRECT *rect) override
    {
        if (!rect || rect->right <= rect->left || rect->top <= rect->bottom)
            return GR_INVALID_PARMS;
        const S32 padding = static_cast<S32>(TEX_PAD_RATIO *
            static_cast<SINGLE>(rect->right - rect->left));
        systemRect_ = *rect;
        sysMapRect_.left = rect->left - padding;
        sysMapRect_.right = rect->right + padding;
        sysMapRect_.top = rect->top + padding;
        sysMapRect_.bottom = rect->bottom - padding;
        hasRect_ = true;
        return GR_OK;
    }
    void GetSysMapRect(int system, tagRECT *rect) override
    {
        tagRECT systemRect{};
        if (!rect || system <= 0 || !SECTOR ||
            !SECTOR->GetSystemRect(static_cast<U32>(system), &systemRect, false))
            Unsupported("GetSysMapRect invalid system");
        const S32 padding = static_cast<S32>(TEX_PAD_RATIO *
            static_cast<SINGLE>(systemRect.right - systemRect.left));
        rect->left = systemRect.left - padding;
        rect->right = systemRect.right + padding;
        rect->top = systemRect.top + padding;
        rect->bottom = systemRect.bottom - padding;
    }
    ~NativeSystemMap()
    {
        COMPTR<IDAConnectionPoint> connection;
        if (eventHandle_ && EVENTSYS && EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
            connection->Unadvise(eventHandle_);
    }
    void Initialize()
    {
        COMPTR<IDAConnectionPoint> connection;
        if (!EVENTSYS || EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
            connection->Advise(static_cast<IEventCallback *>(this), &eventHandle_) != GR_OK)
            Unsupported("scroll callback subscription");
    }
    // SysMap.cpp's in-game camera branch; fullscreen map scrolling is unported.
    GENRESULT Scroll(BOOL32 singleStep, bool horizontal, S32 direction)
    {
        if (CQFLAGS.bFullScreenMap) Unsupported("fullscreen scrolling");
        if (!CAMERA || !DEFAULTS || !CAMERA->PaneToPoints(p_[0], p_[1], p_[2], p_[3]))
            return GR_GENERIC;
        Vector pos = CAMERA->GetRotatedPosition();
        U32 amount;
        if (singleStep)
        {
            amount = 100;
            scrollRemainderX_ = scrollRemainderY_ = 0;
        }
        else
        {
            U32 &remainder = horizontal ? scrollRemainderX_ : scrollRemainderY_;
            const U32 rate = DEFAULTS->GetDefaults()->scrollRate *
                std::fabs(horizontal ? p_[1].x-p_[0].x : p_[2].y-p_[1].y);
            const U32 value = (lastFrameTime_ >> 10) * rate + remainder;
            amount = value >> 10;
            remainder = value & 1023;
        }
        if (horizontal) pos.x += direction * static_cast<S32>(amount);
        else pos.y += direction * static_cast<S32>(amount);
        CAMERA->SetRotatedPosition(&pos, 1);
        return GR_OK;
    }
    GENRESULT ScrollUp(BOOL32 step) override { return Scroll(step, false, 1); }
    GENRESULT ScrollDown(BOOL32 step) override { return Scroll(step, false, -1); }
    GENRESULT ScrollLeft(BOOL32 step) override { return Scroll(step, true, -1); }
    GENRESULT ScrollRight(BOOL32 step) override { return Scroll(step, true, 1); }
    GENRESULT ZoneOn(const Vector &) override { Unsupported("ZoneOn"); }
    void RegisterAttack(U32, GRIDVECTOR, U32) override { Unsupported("RegisterAttack"); }
    S32 GetLastFrameTime() override { return static_cast<S32>(lastFrameTime_); }
    U64 GetMapTiming() override { return 0; }
    const Transform &GetMapTrans() override { Unsupported("GetMapTrans"); }
    void SetScrollRate(SINGLE) override { Unsupported("SetScrollRate"); }
    SINGLE GetScrollRate() override { Unsupported("GetScrollRate"); }
    void BeginFullScreen() override { Unsupported("BeginFullScreen"); }
    GENRESULT Notify(U32 message, void *param) override
    {
        if (message == CQE_RENDER_LAST3D && CQFLAGS.bGameActive)
        {
            const auto value = reinterpret_cast<std::uintptr_t>(param);
            if (value > UINT32_MAX) return GR_INVALID_PARMS;
            lastFrameTime_ = static_cast<U32>(value);
        }
        if (message == CQE_MISSION_CLOSE)
            lastFrameTime_ = scrollRemainderX_ = scrollRemainderY_ = 0;
        return GR_OK;
    }
    void InvalidateMap(U32 systemID) override
    {
        if (!systemID || systemID > MAX_SYSTEMS)
            Unsupported("InvalidateMap invalid system");
        invalidatedSystems_ |= 1U << (systemID - 1U);
    }
    void DrawCircle(Vector, SINGLE, COLORREF) override { Unsupported("DrawCircle"); }
    void DrawRing(Vector, SINGLE, U32, COLORREF) override { Unsupported("DrawRing"); }
    void DrawSquare(Vector, SINGLE, COLORREF) override { Unsupported("DrawSquare"); }
    void DrawHashSquare(Vector, SINGLE, COLORREF) override { Unsupported("DrawHashSquare"); }
    void DrawArc(Vector, SINGLE, SINGLE, SINGLE, U32, COLORREF) override { Unsupported("DrawArc"); }
    void DrawWaypoint(Vector, U32) override { Unsupported("DrawWaypoint"); }
    void DrawMissionAnim(Vector, U32) override { Unsupported("DrawMissionAnim"); }
    void DrawIcon(Vector, SINGLE, U32) override { Unsupported("DrawIcon"); }
    void DrawPlayerIcon(Vector, SINGLE, U32, U32) override { Unsupported("DrawPlayerIcon"); }
    U32 RegisterIcon(char *filename) override
    {
        if (!CQFLAGS.bNoGDI || !filename || !filename[0])
            Unsupported("RegisterIcon");
        // The original owner returns this failure value when its map-only BMP
        // cannot be loaded. The explicit no-GDI slice has no map icon target.
        return static_cast<U32>(-1);
    }
    U32 RegisterPlayerIcon(char *filename) override
    {
        if (!CQFLAGS.bNoGDI || !filename || !filename[0])
            Unsupported("RegisterPlayerIcon");
        return static_cast<U32>(-1);
    }
    U32 GetPadding(U32) override { Unsupported("GetPadding"); }
    void PingSystem(U32, Vector, U32) override { Unsupported("PingSystem"); }
    U32 CreateMissionAnim(U32, GRIDVECTOR) override { Unsupported("CreateMissionAnim"); }
    void StopMissionAnim(U32) override { Unsupported("StopMissionAnim"); }
    void FlushMissionAnims() override {}

    // The source owner writes no SysMapSaveload child when it has no mission animations.
    void Save(IFileSystem *outFile) override
    {
        if (!outFile) Unsupported("Save without filesystem");
    }

    void Load(IFileSystem *inFile) override
    {
        if (!inFile) Unsupported("Load without filesystem");
        if (!inFile->SetCurrentDirectory("\\SysMapSaveload")) return;
        DAFILEDESC descriptor("SysMap");
        IFileSystem *saved = nullptr;
        const GENRESULT result = inFile->CreateInstance(
            &descriptor, reinterpret_cast<void **>(&saved));
        if (result == GR_DATA_NOT_FOUND) return;
        if (result != GR_OK || !saved) Unsupported("Load system-map child");
        saved->Release();
        Unsupported("Load saved mission animations");
    }

    U32 eventHandle_ = 0;
    U32 lastFrameTime_ = 0;
    U32 scrollRemainderX_ = 0, scrollRemainderY_ = 0;
    Vector p_[4]{};
    tagRECT systemRect_{};
    tagRECT sysMapRect_{};
    bool hasRect_ = false;
    U32 invalidatedSystems_ = 0U;
};

struct SystemMapStartup : GlobalComponent
{
    void Startup() override
    {
        if (!CQFLAGS.bNoGDI) return; // The original SysMap owns enabled presentation.
        SYSMAP = new DAComponent<NativeSystemMap>;
        AddToGlobalCleanupList(reinterpret_cast<IDAComponent **>(&SYSMAP));
    }
    void Initialize() override
    {
        if (CQFLAGS.bNoGDI) static_cast<NativeSystemMap *>(SYSMAP)->Initialize();
    }
};

SystemMapStartup startup;
}

namespace FrontierX
{
ISystemMap *CreateNativeSystemMap()
{
    return new DAComponent<NativeSystemMap>;
}
}
