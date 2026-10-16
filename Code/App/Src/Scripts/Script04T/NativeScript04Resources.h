#ifndef FRONTIERX_NATIVE_SCRIPT04_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT04_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript04Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript04CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript04ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript04(void);
#endif
