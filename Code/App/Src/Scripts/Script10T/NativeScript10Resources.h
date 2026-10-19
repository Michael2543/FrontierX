#ifndef FRONTIERX_NATIVE_SCRIPT10_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT10_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript10Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript10CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript10ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript10(void);
#endif
