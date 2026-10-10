#include "Include/NativeAppGlobals.h"
#include "DACOM.H"
#include "DMissionEnum.h"
#include "ISPlayerAI.h"

#include <cstdio>
#include <cstdlib>

#if !defined(_WIN32)
namespace
{
[[noreturn]] void RequireProductionPlayerAI()
{
    std::fputs("Native mission requested the unported production player-AI owner\n", stderr);
    std::abort();
}
}

GENRESULT ISPlayerAI::CreateInstance(const char *, U32, ISPlayerAI **)
{
    RequireProductionPlayerAI();
}

GENRESULT ISPlayerAI::CreateInstance(M_RACE, U32, ISPlayerAI **)
{
    RequireProductionPlayerAI();
}
#endif
