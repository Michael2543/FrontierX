#ifndef FRONTIERX_M3_TRACE_H
#define FRONTIERX_M3_TRACE_H

// Opt-in observation/command access to the original ObjectList tick owner.
// No replacement simulation, RNG, speech completion or mission condition.
#include "MScript.h"
#include "MPartRef.h"
#include "Include/M3FrameInput.h"
#if !defined(_WIN32)
#include <FrontierX/NativeStreamer.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

namespace FrontierXM3
{
inline FILE * TraceFile()
{
    struct FileOwner
    {
        FILE *file = nullptr;
        ~FileOwner() { if (file) std::fclose(file); }
    };
    static FileOwner owner;
    FILE *&file = owner.file;
    static bool checked = false;
    if (!checked)
    {
        checked = true;
        const char *path = std::getenv("FRONTIERX_M3_TRACE");
        if (path && *path)
        {
            file = std::fopen(path, "wb");
            if (!file) { std::fputs("M3 trace open failed\n", stderr); std::abort(); }
        }
    }
    return file;
}

inline unsigned int FloatBits(SINGLE value)
{
    static_assert(sizeof(value) == sizeof(std::uint32_t), "M3 f32 contract");
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<unsigned int>(bits);
}

inline void __cdecl StateOutput(const char *bytes, U32 length, void *context)
{
    FILE *file = static_cast<FILE *>(context);
    if (std::fwrite(bytes, 1, length, file) != length)
    { std::fputs("M3 state output failed\n", stderr); std::abort(); }
}

inline void WriteVector(FILE *file, const Vector &value)
{
    std::fprintf(file, "[\"%08x\",\"%08x\",\"%08x\"]",
        FloatBits(value.x), FloatBits(value.y), FloatBits(value.z));
}

inline void WriteSnapshot(FILE *file, U32 tick, const char *boundary)
{
    // Original getters are read without executing Save or a simulation owner.
    const USER_DEFAULTS *defaults = DEFAULTS->GetDefaults();
    std::fprintf(file, "{\"schema\":6,\"coverage\":\"partial\",\"tick\":%u,\"boundary\":\"%s\","
        "\"globals\":{\"game_speed\":%d,\"frame_limit\":%u,\"constant_update_rate\":%u,"
        "\"game_active\":%u,\"paused\":%u,\"world_paused\":%u},\"mission_globals\":",
        static_cast<unsigned int>(tick), boundary, static_cast<int>(defaults->gameSpeed),
        defaults->bNoFrameLimit ? 0U : 1U, defaults->bConstUpdateRate ? 1U : 0U,
        CQFLAGS.bGameActive ? 1U : 0U, CQFLAGS.bGamePaused ? 1U : 0U,
        CQFLAGS.bRTGamePaused ? 1U : 0U);
    MGlobals::WriteComparisonState(StateOutput, file);
    std::fputs(",\"script\":", file);
    MScript::WriteComparisonState(StateOutput, file);
    std::fputs(",\"operations\":", file);
    WriteOperationComparisonState(StateOutput, file);
    std::fputs(",\"world\":[", file);
    bool worldFirst = true;
    for (IBaseObject *object = OBJLIST->GetObjectList(); object; object = object->next)
    {
        if (!worldFirst) std::fputc(',', file);
        worldFirst = false;
        const TRANSFORM &transform = object->GetTransform();
        std::fprintf(file, "{\"id\":%u,\"class\":%u,\"owner\":%u,\"system\":%u,\"transform_f32\":[",
            static_cast<unsigned int>(object->GetPartID()), static_cast<unsigned int>(object->objClass),
            static_cast<unsigned int>(object->GetPlayerID()), static_cast<unsigned int>(object->GetSystemID()));
        WriteVector(file, transform.get_i()); std::fputc(',', file);
        WriteVector(file, transform.get_j()); std::fputc(',', file);
        WriteVector(file, transform.get_k()); std::fputc(',', file);
        WriteVector(file, transform.translation);
        std::fputs("],\"velocity_f32\":", file); WriteVector(file, object->GetVelocity());
        std::fprintf(file, ",\"visibility\":[%u,%u],\"update_bin\":%u,\"flags\":[%u,%u,%u,%u,%u,%u,%u,%u]}",
            static_cast<unsigned int>(object->GetVisibilityFlags()),
            static_cast<unsigned int>(object->GetPendingVisibilityFlags()),
            static_cast<unsigned int>(object->updateBin),
            object->bHighlight ? 1U : 0U, object->bVisible ? 1U : 0U,
            object->bSelected ? 1U : 0U, object->bCloaked ? 1U : 0U,
            object->bSpecialRender ? 1U : 0U, object->bMimic ? 1U : 0U,
            object->bSpecialHighlight ? 1U : 0U, object->bExploding ? 1U : 0U);
    }
    std::fputc(']', file);
    std::fputs(",\"units\":[", file);
    bool first = true;
    for (IBaseObject *object = OBJLIST->GetObjectList(); object; object = object->next)
    {
        MPart part(object);
        if (!part.isValid()) continue;
        const Vector position = object->GetPosition();
        if (!first) std::fputc(',', file);
        first = false;
        std::fprintf(file,
            "{\"id\":%u,\"class\":%u,\"owner\":%u,\"system\":%u,"
            "\"position_f32\":[\"%08x\",\"%08x\",\"%08x\"],"
            "\"hull\":%u,\"supplies\":%u,\"selected\":%u,\"ready\":%u,"
            "\"pending_valid\":%u,\"pending_op\":%u,\"last_op\":%u,\"group\":%u}",
            static_cast<unsigned int>(part->dwMissionID),
            static_cast<unsigned int>(part->mObjClass),
            static_cast<unsigned int>(object->GetPlayerID()),
            static_cast<unsigned int>(object->GetSystemID()),
            FloatBits(position.x), FloatBits(position.y), FloatBits(position.z),
            static_cast<unsigned int>(part->hullPoints),
            static_cast<unsigned int>(part->supplies),
            static_cast<unsigned int>(MScript::IsSelected(MScript::GetPartByID(part->dwMissionID))),
            static_cast<unsigned int>(part->bReady),
            static_cast<unsigned int>(part->bPendingOpValid),
            static_cast<unsigned int>(part->pendingOp),
            static_cast<unsigned int>(part->lastOpCompleted),
            static_cast<unsigned int>(part->groupID));
    }
    std::fputs("],\"objectives\":[", file);
    const U32 count = MGlobals::GetNumberObjectives();
    for (U32 index = 0; index < count; ++index)
    {
        const U32 id = MGlobals::GetObjectiveStringID(index);
        if (index) std::fputc(',', file);
        std::fprintf(file, "{\"id\":%u,\"completed\":%u,\"failed\":%u,\"secondary\":%u}",
            static_cast<unsigned int>(id),
            static_cast<unsigned int>(MGlobals::IsObjectiveCompleted(id)),
            static_cast<unsigned int>(MGlobals::IsObjectiveFailed(id)),
            static_cast<unsigned int>(MGlobals::IsObjectiveSecondary(id)));
    }
    std::fprintf(file, "],\"agents\":%u,\"resources\":[%u,%u,%u]}\n",
        static_cast<unsigned int>(THEMATRIX->GetNumAgents()),
        static_cast<unsigned int>(MGlobals::GetCurrentGas(MGlobals::GetThisPlayer())),
        static_cast<unsigned int>(MGlobals::GetCurrentMetal(MGlobals::GetThisPlayer())),
        static_cast<unsigned int>(MGlobals::GetCurrentCrew(MGlobals::GetThisPlayer())));
    if (std::fflush(file) || std::ferror(file))
    { std::fputs("M3 trace write failed\n", stderr); std::abort(); }
}

inline void Snapshot(U32 tick)
{
    FILE *file = TraceFile();
    if (!file || tick > 1024) return;
#if !defined(_WIN32)
    FrontierX::SetNativeStreamerDiagnosticTick(tick);
#endif
    if (tick && FixedFrameInput()) return;
    WriteSnapshot(file, tick, tick ? "after-logic" : "initial");
    if (tick == 1024 && EVENTSYS->Post(CQE_PLAYER_QUIT) != GR_OK)
    { std::fputs("M3 original quit request failed\n", stderr); std::abort(); }
}

inline void SnapshotAfterPhysical(U32 tick)
{
    static U32 lastTick = 0;
    if (!TraceFile() || !FixedFrameInput() || tick == lastTick) return;
    // Reject gaps instead of inventing intermediate physical states.
    if (tick != lastTick + 1 || tick > 1024)
    { std::fputs("M3 physical boundary skipped an original tick\n", stderr); std::abort(); }
    lastTick = tick;
#if !defined(_WIN32)
    FrontierX::SetNativeStreamerDiagnosticTick(tick);
#endif
    WriteSnapshot(TraceFile(), tick, "after-physical");
    if (tick == 1024 && EVENTSYS->Post(CQE_PLAYER_QUIT) != GR_OK)
    { std::fputs("M3 original quit request failed\n", stderr); std::abort(); }
}

inline void BeforeTick(U32 tick)
{
    if (!TraceFile()) return;
#if !defined(_WIN32)
    FrontierX::SetNativeStreamerDiagnosticTick(tick);
#endif
    if (tick == 0) Snapshot(0);
    if (tick != 32 && tick != 64) return;
    const MPartRef forger = MScript::GetPartByName("Forger");
    if (!forger.isValid())
    { std::fputs("M3 Sol_Train Forger unavailable\n", stderr); std::abort(); }
    if (tick == 32) MScript::SelectUnit(forger);
    else
    {
        const MPartRef target = MScript::GetPartByName("TriggerPoint");
        if (!target.isValid())
        { std::fputs("M3 Sol_Train move target unavailable\n", stderr); std::abort(); }
        IBaseObject *targetObject = OBJLIST->FindObject(target->dwMissionID);
        if (!targetObject || !OBJLIST->GetSelectedList())
        { std::fputs("M3 original selected move prerequisites missing\n", stderr); std::abort(); }
        NETGRIDVECTOR destination;
        destination.init(targetObject->GetGridPosition(),
            targetObject->GetSystemID() & ~HYPER_SYSTEM_MASK);
        // Same player event as Sector/SysMap, including original ObjComm,
        // packet creation, UI feedback and movement voice. Script moves omit it.
        if (EVENTSYS->Send(0x102CU, &destination) != GR_OK)
        { std::fputs("M3 original selected move event failed\n", stderr); std::abort(); }
    }
}
}
#endif
