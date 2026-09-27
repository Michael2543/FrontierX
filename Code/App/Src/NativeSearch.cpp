#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include <cstdio>
#include <cstdlib>

// Scalar equivalent of the first-match search in Search.asm. Packet and
// object-set callers depend on the returned address, including duplicates.
const DWORD * __fastcall dwordsearch(DWORD len, DWORD value, const DWORD *buffer)
{
    if (len && !buffer)
    {
        std::fputs("Native packet search received a null nonempty range\n", stderr);
        std::abort();
    }
    for (DWORD index = 0; index < len; ++index)
        if (buffer[index] == value) return buffer + index;
    return nullptr;
}
#endif
