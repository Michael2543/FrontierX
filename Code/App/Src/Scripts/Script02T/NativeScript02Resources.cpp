#include "NativeScript02Resources.h"
#include "NativeScript02Schema.inc"
namespace
{
// The extractor preserves source RT_STRING counts and IDs for this module.
struct NativeSolTrainStringEntry { std::uint32_t id; const wchar_t *counted; };
#include "NativeScript02Strings.inc"
unsigned char identity;
}
namespace FrontierX
{
const void *GetNativeScript02Schema(std::size_t &length) noexcept
{
    length = sizeof(script02SchemaBytes);
    return script02SchemaBytes;
}
HINSTANCE__ *GetNativeScript02ResourceHandle() noexcept
{
    return reinterpret_cast<HINSTANCE__ *>(&identity);
}
const wchar_t *GetNativeScript02CountedString(std::uint32_t id) noexcept
{
    std::size_t first = 0, last = sizeof(solTrainStringEntries)/sizeof(solTrainStringEntries[0]);
    while (first < last)
    {
        const auto middle = first + (last-first)/2;
        if (solTrainStringEntries[middle].id < id) first = middle+1;
        else last = middle;
    }
    return first < sizeof(solTrainStringEntries)/sizeof(solTrainStringEntries[0]) &&
        solTrainStringEntries[first].id == id ? solTrainStringEntries[first].counted : nullptr;
}
}
