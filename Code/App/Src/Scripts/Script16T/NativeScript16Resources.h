#ifndef FRONTIERX_NATIVE_SCRIPT16_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT16_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript16Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript16CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript16ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript16(void);
#endif
