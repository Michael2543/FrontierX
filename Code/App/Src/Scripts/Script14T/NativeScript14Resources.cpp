#include "NativeScript14Resources.h"
#include "NativeScript14Schema.inc"
namespace
{
// The extractor preserves source RT_STRING counts and IDs for this module.
struct NativeSolTrainStringEntry { std::uint32_t id; const wchar_t *counted; };
#include "NativeScript14Strings.inc"
unsigned char identity;
}
namespace FrontierX
{
const void *GetNativeScript14Schema(std::size_t &length) noexcept
{
    length = sizeof(script14SchemaBytes);
    return script14SchemaBytes;
}
HINSTANCE__ *GetNativeScript14ResourceHandle() noexcept
{
    return reinterpret_cast<HINSTANCE__ *>(&identity);
}
const wchar_t *GetNativeScript14CountedString(std::uint32_t id) noexcept
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
