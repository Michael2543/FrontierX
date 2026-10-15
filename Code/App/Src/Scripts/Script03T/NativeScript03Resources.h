#ifndef FRONTIERX_NATIVE_SCRIPT03_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT03_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript03Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript03CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript03ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript03(void);
#endif
