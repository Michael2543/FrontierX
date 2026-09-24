// RenderBatcher.cpp uses this production arena owner for vertex/index pools.
// Exercise growth, exact-end allocation, rejection and destruction before
// linking the App-side batcher into a native module. The header-only probe
// cannot detect an arena pointer being deleted or a copy past the arena.
#include "BMPoolJr.h"
#include "CQImage.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>

bool ICQImage::Assert(const char *, void *, unsigned)
{
    return false;
}

namespace
{
constexpr U32 kArenaBytes = 1536U * 1024U;

bool Expect(bool value, const char * message)
{
    if (!value) std::fprintf(stderr, "%s\n", message);
    return value;
}
}

int main()
{
    TheBigPointer = 0U;
    {
        BMPOOLJR pool;
        pool.set_size(16U);
        if (!Expect(pool.allocate(8U, true) == 0U && pool.pool == TheBigBuffer &&
                        pool.pool_used_cnt == 8U && TheBigPointer == 16U,
                    "initial arena allocation")) return 1;
        std::memset(pool.pool, 0x5a, 8U);
        if (!Expect(pool.allocate(12U, true) == 8U && pool.pool_used_cnt == 20U &&
                        pool.pool != TheBigBuffer && pool.pool[0] == 0x5a &&
                        pool.pool[7] == 0x5a,
                    "growth preserves occupied bytes")) return 1;
        pool.set_pool_used_cnt(1000U);
        if (!Expect(pool.pool_used_cnt == 20U,
                    "invalid used-count rollback leaves owner unchanged")) return 1;
        pool.set_pool_used_cnt(4U);
        if (!Expect(pool.pool_used_cnt == 12U, "used-count rollback")) return 1;
        const U32 beforeMove = TheBigPointer;
        pool.reallocate();
        if (!Expect(pool.pool == TheBigBuffer + beforeMove &&
                        TheBigPointer == beforeMove + 12U &&
                        pool.pool[0] == 0x5a && pool.pool[7] == 0x5a,
                    "reallocation preserves live bytes")) return 1;
    }

    TheBigPointer = kArenaBytes - 8U;
    {
        BMPOOLJR pool;
        pool.set_size(8U);
        if (!Expect(pool.allocate(8U) == 0U && TheBigPointer == kArenaBytes &&
                        pool.pool == TheBigBuffer + kArenaBytes - 8U,
                    "exact arena end allocation")) return 1;
    }
    {
        BMPOOLJR pool;
        pool.set_size(8U);
        if (!Expect(pool.allocate(8U) == std::numeric_limits<U32>::max() &&
                        pool.pool == nullptr && pool.pool_used_cnt == 0U &&
                        TheBigPointer == kArenaBytes,
                    "full arena rejection leaves owner unchanged")) return 1;
    }

    TheBigPointer = 0U;
    {
        BMPOOLJR pool;
        pool.set_size(8U);
        if (!Expect(pool.allocate(std::numeric_limits<U32>::max()) ==
                        std::numeric_limits<U32>::max() &&
                        pool.pool == nullptr && TheBigPointer == 0U,
                    "oversized request rejects before arithmetic overflow")) return 1;
    }
    return 0;
}
