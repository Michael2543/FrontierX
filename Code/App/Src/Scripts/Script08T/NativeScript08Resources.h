#ifndef FRONTIERX_NATIVE_SCRIPT08_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT08_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript08Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript08CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript08ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript08(void);
#endif
