#ifndef FRONTIERX_NATIVE_SCRIPT14_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT14_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript14Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript14CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript14ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript14(void);
#endif
