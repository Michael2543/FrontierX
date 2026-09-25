// MyVertex reaches CQTrace before FDump in the production App include order.
// Keep this in a separate translation unit from the FDump-first regression.
#include "MyVertex.h"
#include "GlobalFlagTypes.h"
#include "DEffectOpts.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

static_assert(sizeof(Vertex2) == 32, "production RPVERTEX2 stride must remain 32 bytes");
static_assert(sizeof(CQGlobalFlags) == 8, "production global flags must retain their two-word layout");
static_assert(sizeof(GlobalRenderFlags) == 4, "production render flags must retain their word layout");

namespace
{
char lastTrace[256] = {};
ErrorKind lastKind = ERR_MISSION;
SeverityLevel lastSeverity = SEV_FATAL;
unsigned assertionCalls = 0U;

int DACOM_CDECL CaptureTrace(ErrorCode code, const C8 * format, ...)
{
    lastKind = code.kind;
    lastSeverity = code.severity;
    std::va_list args;
    va_start(args, format);
    const int length = std::vsnprintf(lastTrace, sizeof(lastTrace), format, args);
    va_end(args);
    return length;
}
}

DA_ERROR_HANDLER FDUMP = CaptureTrace;

bool ICQImage::Assert(const char * expression, void * file, unsigned line)
{
    if (expression && file && line != 0U) ++assertionCalls;
    return false;
}

bool CheckBatcherTraceFormat()
{
    CQTRACE10("batcher diagnostic");
    const bool traceValid = lastKind == ERR_GENERAL && lastSeverity == SEV_TRACE_1 &&
        std::strstr(lastTrace, ") : batcher diagnostic\n") != nullptr;
    int checked = 0;
    CQASSERT(++checked == 1);
    CQASSERT(++checked == 1);
    return traceValid && checked == 2 && assertionCalls == 1U;
}
