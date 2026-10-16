#ifndef FRONTIERX_NATIVE_SCRIPT05_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT05_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript05Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript05CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript05ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript05(void);
#endif
