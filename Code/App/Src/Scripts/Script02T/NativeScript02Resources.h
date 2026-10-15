#ifndef FRONTIERX_NATIVE_SCRIPT02_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT02_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript02Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript02CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript02ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript02(void);
#endif
