#ifndef FRONTIERX_NATIVE_SCRIPT11_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT11_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript11Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript11CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript11ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript11(void);
#endif
