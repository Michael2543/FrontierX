#ifndef FRONTIERX_NATIVE_SCRIPT09_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT09_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript09Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript09CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript09ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript09(void);
#endif
