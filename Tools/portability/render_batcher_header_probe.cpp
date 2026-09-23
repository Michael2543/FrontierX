// Verifies that the production ZBatcher header closure (Code/App/Src/RenderBatcher.h)
// compiles on non-Windows toolchains. The header is the declarative ZBatcher
// interface shared by the App-side RenderBatcher owner; this probe does not
// claim behavioral batching, only interface closure. Behavioral execution
// remains blocked on the App-side translation unit dependencies listed in
// docs/RENDER_BATCHER_PORTABILITY.md.
// Keep FDump first: its calling convention and export macros must not depend
// on RenderBatcher having already included DACOM.H.
#include "FDump.h"
#include "RenderBatcher.h"

int main()
{
    const PRIMITIVETYPEINFO info = primitive_type_info[0];
    (void) info.type;
    return 0;
}
