#include "NativeSolTrainStrings.h"

#include <cstddef>

namespace
{
struct NativeSolTrainStringEntry
{
    std::uint32_t id;
    const wchar_t *counted;
};

unsigned char solTrainResourceIdentity = 0;

#include "NativeSolTrainStrings.inc"
}

namespace FrontierX
{
HINSTANCE__ *GetNativeSolTrainResourceHandle() noexcept
{
    return reinterpret_cast<HINSTANCE__ *>(&solTrainResourceIdentity);
}

const wchar_t *GetNativeSolTrainCountedString(std::uint32_t id) noexcept
{
    std::size_t first = 0;
    std::size_t last = sizeof(solTrainStringEntries) /
                       sizeof(solTrainStringEntries[0]);
    while (first < last)
    {
        const std::size_t middle = first + (last - first) / 2;
        if (solTrainStringEntries[middle].id < id) first = middle + 1;
        else last = middle;
    }
    const std::size_t count = sizeof(solTrainStringEntries) /
                              sizeof(solTrainStringEntries[0]);
    return first < count && solTrainStringEntries[first].id == id
        ? solTrainStringEntries[first].counted : nullptr;
}
}
