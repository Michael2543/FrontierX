#ifndef FRONTIERX_LEGACY_INPUT_H
#define FRONTIERX_LEGACY_INPUT_H

#include "FrontierX/LegacyMessage.h"
#include "FrontierX/WindowEvent.h"

#include <cstdint>
#include <vector>

namespace FrontierX
{
namespace LegacyMessage
{
constexpr std::uint32_t SetFocus = 0x0007;
constexpr std::uint32_t KillFocus = 0x0008;
constexpr std::uint32_t Size = 0x0005;
constexpr std::uint32_t Close = 0x0010;
constexpr std::uint32_t ActivateApp = 0x001c;
constexpr std::uint32_t KeyDown = 0x0100;
constexpr std::uint32_t KeyUp = 0x0101;
constexpr std::uint32_t Character = 0x0102;
constexpr std::uint32_t SystemKeyDown = 0x0104;
constexpr std::uint32_t SystemKeyUp = 0x0105;
constexpr std::uint32_t MouseMove = 0x0200;
constexpr std::uint32_t LeftButtonDown = 0x0201;
constexpr std::uint32_t LeftButtonUp = 0x0202;
constexpr std::uint32_t LeftButtonDoubleClick = 0x0203;
constexpr std::uint32_t RightButtonDown = 0x0204;
constexpr std::uint32_t RightButtonUp = 0x0205;
constexpr std::uint32_t RightButtonDoubleClick = 0x0206;
constexpr std::uint32_t MiddleButtonDown = 0x0207;
constexpr std::uint32_t MiddleButtonUp = 0x0208;
constexpr std::uint32_t MiddleButtonDoubleClick = 0x0209;
constexpr std::uint32_t MouseWheel = 0x020a;
constexpr std::uint32_t UnicodeCharacter = 0x1019;
}

namespace LegacyMouseMask
{
constexpr std::uintptr_t Left = 0x0001;
constexpr std::uintptr_t Right = 0x0002;
constexpr std::uintptr_t Shift = 0x0004;
constexpr std::uintptr_t Control = 0x0008;
constexpr std::uintptr_t Middle = 0x0010;
}

namespace LegacyVirtualKey
{
constexpr std::uint32_t Back = 0x08;
constexpr std::uint32_t Tab = 0x09;
constexpr std::uint32_t Return = 0x0d;
constexpr std::uint32_t Shift = 0x10;
constexpr std::uint32_t Control = 0x11;
constexpr std::uint32_t Menu = 0x12;
constexpr std::uint32_t Pause = 0x13;
constexpr std::uint32_t Capital = 0x14;
constexpr std::uint32_t Escape = 0x1b;
constexpr std::uint32_t Space = 0x20;
constexpr std::uint32_t Prior = 0x21;
constexpr std::uint32_t Next = 0x22;
constexpr std::uint32_t End = 0x23;
constexpr std::uint32_t Home = 0x24;
constexpr std::uint32_t Left = 0x25;
constexpr std::uint32_t Up = 0x26;
constexpr std::uint32_t Right = 0x27;
constexpr std::uint32_t Down = 0x28;
constexpr std::uint32_t Insert = 0x2d;
constexpr std::uint32_t Delete = 0x2e;
constexpr std::uint32_t LeftShift = 0xa0;
constexpr std::uint32_t RightShift = 0xa1;
constexpr std::uint32_t LeftControl = 0xa2;
constexpr std::uint32_t RightControl = 0xa3;
constexpr std::uint32_t LeftMenu = 0xa4;
constexpr std::uint32_t RightMenu = 0xa5;
constexpr std::uint32_t F10 = 0x79;
constexpr std::uint32_t NumLock = 0x90;
constexpr std::uint32_t Scroll = 0x91;
}

inline constexpr std::uint32_t LegacyKeyToggleBit = 1U << 25;

struct LegacyWindowMessage
{
    LegacyWindowToken window = 0;
    LegacyMessageId message = 0;
    LegacyWParam wparam = 0;
    LegacyLParam lparam = 0;
};

enum class LegacyInputPayload
{
    WindowMessage,
    ImmediateValue
};

struct LegacyInputEvent
{
    std::uint32_t message = 0;
    LegacyInputPayload payload = LegacyInputPayload::WindowMessage;
    LegacyWindowMessage window_message;
    std::uintptr_t immediate_value = 0;
};

class SdlLegacyInputAdapter
{
public:
    using Sink = void (*)(void * context, std::uint32_t message, void * parameter) noexcept;

    std::vector<LegacyInputEvent> Translate(const WindowEvent & event);
    void Dispatch(const WindowEvent & event, Sink sink, void * context = nullptr);

private:
    std::uint32_t mouse_buttons_ = 0;
    std::uint16_t key_modifiers_ = 0;
};
}

#endif
