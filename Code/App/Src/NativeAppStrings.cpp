#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace
{
struct NativeAppStringEntry
{
    U32 id;
    const wchar_t *text;
};

#include "NativeAppStrings.inc"

const wchar_t *FindNativeAppString(U32 id)
{
    // The source LoadString returns an empty buffer for the zero display ID
    // used by invisible mission triggers.
    if (id == 0) return L"";
    std::size_t first = 0;
    std::size_t last = sizeof(appStringEntries) / sizeof(appStringEntries[0]);
    while (first < last)
    {
        const std::size_t middle = first + (last - first) / 2;
        if (appStringEntries[middle].id < id) first = middle + 1;
        else last = middle;
    }
    if (first < sizeof(appStringEntries) / sizeof(appStringEntries[0]) &&
        appStringEntries[first].id == id)
        return appStringEntries[first].text;
    std::fprintf(stderr, "Native App string resource %u is missing\n", id);
    std::abort();
}
}

const wchar_t * __cdecl _localLoadStringW(U32 id)
{
    return FindNativeAppString(id);
}

const char * __cdecl _localLoadString(U32 id)
{
    const wchar_t *wide = FindNativeAppString(id);
    static thread_local std::string narrow;
    narrow.clear();
    for (; *wide; ++wide)
    {
        // The source English resources use CP1252. Required non-Latin text
        // needs an explicit codepage conversion before it can be used here.
        if (*wide > 0xff)
        {
            std::fprintf(stderr, "Native App string resource %u needs CP1252 conversion\n", id);
            std::abort();
        }
        narrow.push_back(static_cast<char>(*wide));
    }
    return narrow.c_str();
}
#endif
