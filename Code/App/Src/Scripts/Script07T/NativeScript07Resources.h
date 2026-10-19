#ifndef FRONTIERX_NATIVE_SCRIPT07_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT07_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript07Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript07CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript07ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript07(void);
#endif
