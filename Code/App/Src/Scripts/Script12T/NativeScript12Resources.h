#ifndef FRONTIERX_NATIVE_SCRIPT12_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT12_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript12Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript12CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript12ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript12(void);
#endif
