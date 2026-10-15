#ifndef FRONTIERX_NATIVE_SCRIPT01_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT01_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript01Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript01CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript01ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript01(void);
#endif
