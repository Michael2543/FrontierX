#ifndef FRONTIERX_NATIVE_SCRIPT13_RESOURCES_H
#define FRONTIERX_NATIVE_SCRIPT13_RESOURCES_H
#include <cstddef>
#include <cstdint>
struct HINSTANCE__;
namespace FrontierX
{
const void *GetNativeScript13Schema(std::size_t &length) noexcept;
const wchar_t *GetNativeScript13CountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeScript13ResourceHandle() noexcept;
}
extern "C" void FrontierXInitializeScript13(void);
#endif
