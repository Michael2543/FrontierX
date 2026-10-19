#ifndef FRONTIERX_NATIVE_SCRIPT15_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT15_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript15Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript15CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript15ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript15(void);
#endif
