#ifndef FRONTIERX_M3_TRACE_H
#define FRONTIERX_M3_TRACE_H

// Opt-in observation/command access to the original ObjectList tick owner.
// No replacement clock, RNG, speech completion or mission condition.
#include "MScript.h"
#include "MPartRef.h"
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

inline void Snapshot(U32 tick)
{
    FILE *file = TraceFile();
    if (!file || tick > 1024) return;
    // Order IDs are observed, but the queued operation payload, script state
    // and CRT RNG state are NOT yet available through a semantic interface.
    std::fprintf(file, "{\"schema\":1,\"coverage\":\"partial\",\"tick\":%u,\"units\":[",
        static_cast<unsigned int>(tick));
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
    if (tick == 1024 && EVENTSYS->Post(CQE_PLAYER_QUIT) != GR_OK)
    { std::fputs("M3 original quit request failed\n", stderr); std::abort(); }
}

inline void BeforeTick(U32 tick)
{
    if (!TraceFile()) return;
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
        MScript::OrderMoveTo(forger, target, false);
    }
}
}
#endif
