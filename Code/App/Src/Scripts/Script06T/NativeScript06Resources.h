#ifndef FRONTIERX_NATIVE_SCRIPT06_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT06_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript06Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript06CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript06ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript06(void);
#endif
