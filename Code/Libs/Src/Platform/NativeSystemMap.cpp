#include "Include/NativeAppGlobals.h"
#include "FrontierX/NativeSystemMap.h"
#include <3dmath.h>
#include "SysMap.h"
#include "Sector.h"
#include "Startup.h"

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

struct NativeSystemMap : ISystemMap
{
    BEGIN_DACOM_MAP_INBOUND(NativeSystemMap)
    DACOM_INTERFACE_ENTRY(ISystemMap)
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
    GENRESULT ScrollUp(BOOL32) override { Unsupported("ScrollUp"); }
    GENRESULT ScrollDown(BOOL32) override { Unsupported("ScrollDown"); }
    GENRESULT ScrollLeft(BOOL32) override { Unsupported("ScrollLeft"); }
    GENRESULT ScrollRight(BOOL32) override { Unsupported("ScrollRight"); }
    GENRESULT ZoneOn(const Vector &) override { Unsupported("ZoneOn"); }
    void RegisterAttack(U32, GRIDVECTOR, U32) override { Unsupported("RegisterAttack"); }
    S32 GetLastFrameTime() override { return 0; }
    U64 GetMapTiming() override { return 0; }
    const Transform &GetMapTrans() override { Unsupported("GetMapTrans"); }
    void SetScrollRate(SINGLE) override { Unsupported("SetScrollRate"); }
    SINGLE GetScrollRate() override { Unsupported("GetScrollRate"); }
    void BeginFullScreen() override { Unsupported("BeginFullScreen"); }
    GENRESULT Notify(U32, void *) override { Unsupported("Notify"); }
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

    tagRECT systemRect_{};
    tagRECT sysMapRect_{};
    bool hasRect_ = false;
    U32 invalidatedSystems_ = 0U;
};

struct SystemMapStartup : GlobalComponent
{
    void Startup() override
    {
        SYSMAP = new DAComponent<NativeSystemMap>;
        AddToGlobalCleanupList(reinterpret_cast<IDAComponent **>(&SYSMAP));
    }
    void Initialize() override {}
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
