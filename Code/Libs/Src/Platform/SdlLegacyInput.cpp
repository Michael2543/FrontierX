#include "FrontierX/LegacyInput.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>

namespace FrontierX
{
bool NativeShiftPressed()
{
    return (SDL_GetModState() & SDL_KMOD_SHIFT) != 0;
}

namespace
{
constexpr auto vkBack = LegacyVirtualKey::Back;
constexpr auto vkTab = LegacyVirtualKey::Tab;
constexpr auto vkReturn = LegacyVirtualKey::Return;
constexpr auto vkShift = LegacyVirtualKey::Shift;
constexpr auto vkControl = LegacyVirtualKey::Control;
constexpr auto vkMenu = LegacyVirtualKey::Menu;
constexpr auto vkPause = LegacyVirtualKey::Pause;
constexpr auto vkCapital = LegacyVirtualKey::Capital;
constexpr auto vkEscape = LegacyVirtualKey::Escape;
constexpr auto vkSpace = LegacyVirtualKey::Space;
constexpr auto vkPrior = LegacyVirtualKey::Prior;
constexpr auto vkNext = LegacyVirtualKey::Next;
constexpr auto vkEnd = LegacyVirtualKey::End;
constexpr auto vkHome = LegacyVirtualKey::Home;
constexpr auto vkLeft = LegacyVirtualKey::Left;
constexpr auto vkUp = LegacyVirtualKey::Up;
constexpr auto vkRight = LegacyVirtualKey::Right;
constexpr auto vkDown = LegacyVirtualKey::Down;
constexpr auto vkInsert = LegacyVirtualKey::Insert;
constexpr auto vkDelete = LegacyVirtualKey::Delete;
constexpr auto vkF10 = LegacyVirtualKey::F10;
constexpr auto vkNumLock = LegacyVirtualKey::NumLock;
constexpr auto vkScroll = LegacyVirtualKey::Scroll;

LegacyInputEvent WindowMessage(std::uint32_t message, std::uint32_t window,
                               std::uintptr_t wparam = 0, std::intptr_t lparam = 0)
{
    LegacyInputEvent result;
    result.message = message;
    result.window_message = LegacyWindowMessage{window, message, wparam, lparam};
    return result;
}

LegacyInputEvent Immediate(std::uint32_t message, std::uintptr_t value)
{
    LegacyInputEvent result;
    result.message = message;
    result.payload = LegacyInputPayload::ImmediateValue;
    result.immediate_value = value;
    return result;
}

std::uint32_t VirtualKey(std::uint32_t key)
{
    if (key >= SDLK_A && key <= SDLK_Z) return static_cast<std::uint32_t>('A') + key - SDLK_A;
    if (key >= SDLK_0 && key <= SDLK_9) return key;
    if (key >= SDLK_F1 && key <= SDLK_F12) return 0x70 + key - SDLK_F1;

    switch (key)
    {
    case SDLK_BACKSPACE: return vkBack;
    case SDLK_TAB: return vkTab;
    case SDLK_RETURN:
    case SDLK_KP_ENTER: return vkReturn;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT: return vkShift;
    case SDLK_LCTRL:
    case SDLK_RCTRL: return vkControl;
    case SDLK_LALT:
    case SDLK_RALT: return vkMenu;
    case SDLK_PAUSE: return vkPause;
    case SDLK_CAPSLOCK: return vkCapital;
    case SDLK_ESCAPE: return vkEscape;
    case SDLK_SPACE: return vkSpace;
    case SDLK_PAGEUP: return vkPrior;
    case SDLK_PAGEDOWN: return vkNext;
    case SDLK_END: return vkEnd;
    case SDLK_HOME: return vkHome;
    case SDLK_LEFT: return vkLeft;
    case SDLK_UP: return vkUp;
    case SDLK_RIGHT: return vkRight;
    case SDLK_DOWN: return vkDown;
    case SDLK_INSERT: return vkInsert;
    case SDLK_DELETE: return vkDelete;
    case SDLK_NUMLOCKCLEAR: return vkNumLock;
    case SDLK_SCROLLLOCK: return vkScroll;
    case SDLK_KP_0: return 0x60;
    case SDLK_KP_1: return 0x61;
    case SDLK_KP_2: return 0x62;
    case SDLK_KP_3: return 0x63;
    case SDLK_KP_4: return 0x64;
    case SDLK_KP_5: return 0x65;
    case SDLK_KP_6: return 0x66;
    case SDLK_KP_7: return 0x67;
    case SDLK_KP_8: return 0x68;
    case SDLK_KP_9: return 0x69;
    case SDLK_KP_MULTIPLY: return 0x6a;
    case SDLK_KP_PLUS: return 0x6b;
    case SDLK_KP_MINUS: return 0x6d;
    case SDLK_KP_DECIMAL: return 0x6e;
    case SDLK_KP_DIVIDE: return 0x6f;
    case SDLK_SEMICOLON: return 0xba;
    case SDLK_EQUALS: return 0xbb;
    case SDLK_COMMA: return 0xbc;
    case SDLK_MINUS: return 0xbd;
    case SDLK_PERIOD: return 0xbe;
    case SDLK_SLASH: return 0xbf;
    case SDLK_GRAVE: return 0xc0;
    case SDLK_LEFTBRACKET: return 0xdb;
    case SDLK_BACKSLASH: return 0xdc;
    case SDLK_RIGHTBRACKET: return 0xdd;
    case SDLK_APOSTROPHE: return 0xde;
    default: return 0;
    }
}

std::uint32_t ScanCode(std::uint32_t scan)
{
    if (scan >= SDL_SCANCODE_1 && scan <= SDL_SCANCODE_9) return 0x02 + scan - SDL_SCANCODE_1;
    if (scan >= SDL_SCANCODE_F1 && scan <= SDL_SCANCODE_F10) return 0x3b + scan - SDL_SCANCODE_F1;
    switch (scan)
    {
    case SDL_SCANCODE_0: return 0x0b;
    case SDL_SCANCODE_A: return 0x1e;
    case SDL_SCANCODE_B: return 0x30;
    case SDL_SCANCODE_C: return 0x2e;
    case SDL_SCANCODE_D: return 0x20;
    case SDL_SCANCODE_E: return 0x12;
    case SDL_SCANCODE_F: return 0x21;
    case SDL_SCANCODE_G: return 0x22;
    case SDL_SCANCODE_H: return 0x23;
    case SDL_SCANCODE_I: return 0x17;
    case SDL_SCANCODE_J: return 0x24;
    case SDL_SCANCODE_K: return 0x25;
    case SDL_SCANCODE_L: return 0x26;
    case SDL_SCANCODE_M: return 0x32;
    case SDL_SCANCODE_N: return 0x31;
    case SDL_SCANCODE_O: return 0x18;
    case SDL_SCANCODE_P: return 0x19;
    case SDL_SCANCODE_Q: return 0x10;
    case SDL_SCANCODE_R: return 0x13;
    case SDL_SCANCODE_S: return 0x1f;
    case SDL_SCANCODE_T: return 0x14;
    case SDL_SCANCODE_U: return 0x16;
    case SDL_SCANCODE_V: return 0x2f;
    case SDL_SCANCODE_W: return 0x11;
    case SDL_SCANCODE_X: return 0x2d;
    case SDL_SCANCODE_Y: return 0x15;
    case SDL_SCANCODE_Z: return 0x2c;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER: return 0x1c;
    case SDL_SCANCODE_ESCAPE: return 0x01;
    case SDL_SCANCODE_BACKSPACE: return 0x0e;
    case SDL_SCANCODE_TAB: return 0x0f;
    case SDL_SCANCODE_SPACE: return 0x39;
    case SDL_SCANCODE_CAPSLOCK: return 0x3a;
    case SDL_SCANCODE_F11: return 0x57;
    case SDL_SCANCODE_F12: return 0x58;
    case SDL_SCANCODE_LSHIFT: return 0x2a;
    case SDL_SCANCODE_RSHIFT: return 0x36;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL: return 0x1d;
    case SDL_SCANCODE_LALT:
    case SDL_SCANCODE_RALT: return 0x38;
    case SDL_SCANCODE_NUMLOCKCLEAR: return 0x45;
    case SDL_SCANCODE_SCROLLLOCK: return 0x46;
    case SDL_SCANCODE_HOME: return 0x47;
    case SDL_SCANCODE_UP: return 0x48;
    case SDL_SCANCODE_PAGEUP: return 0x49;
    case SDL_SCANCODE_LEFT: return 0x4b;
    case SDL_SCANCODE_RIGHT: return 0x4d;
    case SDL_SCANCODE_END: return 0x4f;
    case SDL_SCANCODE_DOWN: return 0x50;
    case SDL_SCANCODE_PAGEDOWN: return 0x51;
    case SDL_SCANCODE_INSERT: return 0x52;
    case SDL_SCANCODE_DELETE: return 0x53;
    default: return 0;
    }
}

bool ExtendedKey(std::uint32_t scan)
{
    switch (scan)
    {
    case SDL_SCANCODE_RCTRL:
    case SDL_SCANCODE_RALT:
    case SDL_SCANCODE_KP_ENTER:
    case SDL_SCANCODE_KP_DIVIDE:
    case SDL_SCANCODE_INSERT:
    case SDL_SCANCODE_DELETE:
    case SDL_SCANCODE_HOME:
    case SDL_SCANCODE_END:
    case SDL_SCANCODE_PAGEUP:
    case SDL_SCANCODE_PAGEDOWN:
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_DOWN:
        return true;
    default:
        return false;
    }
}

std::intptr_t KeyLParam(const KeyEvent & key, bool released)
{
    std::uint32_t result = 1 | (ScanCode(key.physical_key) << 16);
    if (ExtendedKey(key.physical_key)) result |= 1u << 24;
    if (key.modifiers & static_cast<std::uint16_t>(SDL_KMOD_ALT)) result |= 1u << 29;
    const bool toggle_active =
        (key.logical_key == SDLK_CAPSLOCK && (key.modifiers & SDL_KMOD_CAPS)) ||
        (key.logical_key == SDLK_NUMLOCKCLEAR && (key.modifiers & SDL_KMOD_NUM)) ||
        (key.logical_key == SDLK_SCROLLLOCK && (key.modifiers & SDL_KMOD_SCROLL));
    if (toggle_active) result |= LegacyKeyToggleBit;
    if (key.repeat || released) result |= 1u << 30;
    if (released) result |= 1u << 31;
    return static_cast<std::intptr_t>(result);
}

std::uint32_t MouseButtons(std::uint32_t state)
{
    std::uint32_t result = 0;
    if (state & SDL_BUTTON_LMASK) result |= LegacyMouseMask::Left;
    if (state & SDL_BUTTON_RMASK) result |= LegacyMouseMask::Right;
    if (state & SDL_BUTTON_MMASK) result |= LegacyMouseMask::Middle;
    return result;
}

std::uintptr_t MouseWParam(std::uint32_t buttons, std::uint16_t modifiers)
{
    std::uintptr_t result = buttons;
    if (modifiers & static_cast<std::uint16_t>(SDL_KMOD_SHIFT)) result |= LegacyMouseMask::Shift;
    if (modifiers & static_cast<std::uint16_t>(SDL_KMOD_CTRL)) result |= LegacyMouseMask::Control;
    return result;
}

std::intptr_t PackedPair(float first, float second)
{
    const auto encode = [](float value)
    {
        if (!std::isfinite(value)) return std::uint16_t{0};
        const float bounded = std::clamp(value, -32768.0F, 32767.0F);
        return static_cast<std::uint16_t>(static_cast<std::int32_t>(bounded));
    };
    const auto low = encode(first);
    const auto high = encode(second);
    return static_cast<std::intptr_t>(static_cast<std::uint32_t>(low) |
                                      (static_cast<std::uint32_t>(high) << 16));
}

std::intptr_t PackedIntegers(std::int32_t first, std::int32_t second)
{
    const auto low = static_cast<std::uint16_t>(first);
    const auto high = static_cast<std::uint16_t>(second);
    return static_cast<std::intptr_t>(static_cast<std::uint32_t>(low) |
                                      (static_cast<std::uint32_t>(high) << 16));
}

std::uint32_t ButtonMask(std::uint8_t button)
{
    switch (button)
    {
    case SDL_BUTTON_LEFT: return LegacyMouseMask::Left;
    case SDL_BUTTON_RIGHT: return LegacyMouseMask::Right;
    case SDL_BUTTON_MIDDLE: return LegacyMouseMask::Middle;
    default: return 0;
    }
}

std::uint32_t ButtonMessage(std::uint8_t button, bool down, bool double_click)
{
    switch (button)
    {
    case SDL_BUTTON_LEFT:
        return double_click ? LegacyMessage::LeftButtonDoubleClick
                            : (down ? LegacyMessage::LeftButtonDown : LegacyMessage::LeftButtonUp);
    case SDL_BUTTON_RIGHT:
        return double_click ? LegacyMessage::RightButtonDoubleClick
                            : (down ? LegacyMessage::RightButtonDown : LegacyMessage::RightButtonUp);
    case SDL_BUTTON_MIDDLE:
        return double_click ? LegacyMessage::MiddleButtonDoubleClick
                            : (down ? LegacyMessage::MiddleButtonDown : LegacyMessage::MiddleButtonUp);
    default:
        return 0;
    }
}

void AppendText(std::vector<LegacyInputEvent> & output, std::uint32_t window,
                const std::string & text)
{
    const auto * bytes = reinterpret_cast<const unsigned char *>(text.data());
    std::size_t index = 0;
    while (index < text.size())
    {
        std::uint32_t codepoint = 0xfffd;
        std::size_t length = 1;
        const unsigned char first = bytes[index];
        if (first < 0x80)
            codepoint = first;
        else if ((first & 0xe0) == 0xc0 && index + 1 < text.size() &&
                 (bytes[index + 1] & 0xc0) == 0x80)
        {
            codepoint = ((first & 0x1f) << 6) | (bytes[index + 1] & 0x3f);
            if (codepoint >= 0x80) length = 2; else codepoint = 0xfffd;
        }
        else if ((first & 0xf0) == 0xe0 && index + 2 < text.size() &&
                 (bytes[index + 1] & 0xc0) == 0x80 && (bytes[index + 2] & 0xc0) == 0x80)
        {
            codepoint = ((first & 0x0f) << 12) | ((bytes[index + 1] & 0x3f) << 6) |
                        (bytes[index + 2] & 0x3f);
            if (codepoint >= 0x800 && !(codepoint >= 0xd800 && codepoint <= 0xdfff)) length = 3;
            else codepoint = 0xfffd;
        }
        else if ((first & 0xf8) == 0xf0 && index + 3 < text.size() &&
                 (bytes[index + 1] & 0xc0) == 0x80 && (bytes[index + 2] & 0xc0) == 0x80 &&
                 (bytes[index + 3] & 0xc0) == 0x80)
        {
            codepoint = ((first & 0x07) << 18) | ((bytes[index + 1] & 0x3f) << 12) |
                        ((bytes[index + 2] & 0x3f) << 6) | (bytes[index + 3] & 0x3f);
            if (codepoint >= 0x10000 && codepoint <= 0x10ffff) length = 4;
            else codepoint = 0xfffd;
        }
        index += length;

        std::uint16_t units[2]{};
        std::size_t unit_count = 1;
        if (codepoint <= 0xffff)
            units[0] = static_cast<std::uint16_t>(codepoint);
        else
        {
            codepoint -= 0x10000;
            units[0] = static_cast<std::uint16_t>(0xd800 + (codepoint >> 10));
            units[1] = static_cast<std::uint16_t>(0xdc00 + (codepoint & 0x3ff));
            unit_count = 2;
        }
        for (std::size_t unit = 0; unit < unit_count; ++unit)
        {
            output.push_back(Immediate(LegacyMessage::UnicodeCharacter, units[unit]));
            output.push_back(WindowMessage(LegacyMessage::Character, window, units[unit], 1));
        }
    }
}
}

std::vector<LegacyInputEvent> SdlLegacyInputAdapter::Translate(const WindowEvent & event)
{
    std::vector<LegacyInputEvent> result;
    const std::uint32_t window = event.window_id;
    switch (event.type)
    {
    case WindowEventType::QuitRequested:
    case WindowEventType::CloseRequested:
        result.push_back(WindowMessage(LegacyMessage::Close, window));
        break;
    case WindowEventType::FocusGained:
        result.push_back(WindowMessage(LegacyMessage::ActivateApp, window, 1));
        result.push_back(WindowMessage(LegacyMessage::SetFocus, window));
        break;
    case WindowEventType::FocusLost:
        mouse_buttons_ = 0;
        key_modifiers_ = 0;
        result.push_back(WindowMessage(LegacyMessage::KillFocus, window));
        result.push_back(WindowMessage(LegacyMessage::ActivateApp, window));
        break;
    case WindowEventType::Resized:
    {
        const auto & size = std::get<WindowSizeEvent>(event.payload);
        result.push_back(WindowMessage(LegacyMessage::Size, window, 0,
            PackedIntegers(size.width, size.height)));
        break;
    }
    case WindowEventType::PixelSizeChanged:
        break;
    case WindowEventType::KeyDown:
    case WindowEventType::KeyUp:
    {
        const auto & key = std::get<KeyEvent>(event.payload);
        key_modifiers_ = key.modifiers;
        const std::uint32_t virtual_key = VirtualKey(key.logical_key);
        if (!virtual_key) break;
        const bool released = event.type == WindowEventType::KeyUp;
        const bool system_key = (key.modifiers & static_cast<std::uint16_t>(SDL_KMOD_ALT)) ||
                                virtual_key == vkF10;
        const std::uint32_t message = system_key
            ? (released ? LegacyMessage::SystemKeyUp : LegacyMessage::SystemKeyDown)
            : (released ? LegacyMessage::KeyUp : LegacyMessage::KeyDown);
        result.push_back(WindowMessage(message, window, virtual_key, KeyLParam(key, released)));
        // SDL text input omits the control characters used by original Frames.
        if (!released && !system_key && !(key.modifiers & SDL_KMOD_CTRL) &&
            (virtual_key == vkEscape || virtual_key == vkTab ||
             virtual_key == vkBack || virtual_key == vkReturn))
        {
            result.push_back(Immediate(LegacyMessage::UnicodeCharacter, virtual_key));
            result.push_back(WindowMessage(LegacyMessage::Character, window,
                virtual_key, KeyLParam(key, false)));
        }
        break;
    }
    case WindowEventType::TextInput:
        AppendText(result, window, std::get<TextInputEvent>(event.payload).utf8);
        break;
    case WindowEventType::MouseMotion:
    {
        const auto & motion = std::get<MouseMotionEvent>(event.payload);
        mouse_buttons_ = MouseButtons(motion.buttons);
        result.push_back(WindowMessage(LegacyMessage::MouseMove, window,
            MouseWParam(mouse_buttons_, key_modifiers_), PackedPair(motion.x, motion.y)));
        break;
    }
    case WindowEventType::MouseButtonDown:
    case WindowEventType::MouseButtonUp:
    {
        const auto & button = std::get<MouseButtonEvent>(event.payload);
        const std::uint32_t mask = ButtonMask(button.button);
        if (!mask) break;
        const bool down = event.type == WindowEventType::MouseButtonDown;
        if (down) mouse_buttons_ |= mask; else mouse_buttons_ &= ~mask;
        const auto wparam = MouseWParam(mouse_buttons_, key_modifiers_);
        const auto lparam = PackedPair(button.x, button.y);
        result.push_back(WindowMessage(LegacyMessage::MouseMove, window, wparam, lparam));
        result.push_back(WindowMessage(ButtonMessage(button.button, down, false), window, wparam, lparam));
        result.push_back(WindowMessage(LegacyMessage::MouseMove, window, wparam, lparam));
        if (down && button.clicks > 1)
            result.push_back(WindowMessage(ButtonMessage(button.button, true, true), window, wparam, lparam));
        break;
    }
    case WindowEventType::MouseWheel:
    {
        const auto & wheel = std::get<MouseWheelEvent>(event.payload);
        const float scaled = wheel.y * 120.0F;
        const long rounded = std::isfinite(scaled)
            ? std::lround(std::clamp(scaled, -32768.0F, 32767.0F)) : 0;
        const auto delta = static_cast<std::int16_t>(std::clamp(rounded, -32768L, 32767L));
        const std::uintptr_t wparam = MouseWParam(mouse_buttons_, key_modifiers_) |
            (static_cast<std::uintptr_t>(static_cast<std::uint16_t>(delta)) << 16);
        result.push_back(WindowMessage(LegacyMessage::MouseWheel, window, wparam,
            PackedPair(wheel.mouse_x, wheel.mouse_y)));
        break;
    }
    }
    return result;
}

void SdlLegacyInputAdapter::Dispatch(const WindowEvent & event, Sink sink, void * context)
{
    if (!sink) return;
    auto translated = Translate(event);
    for (auto & item : translated)
    {
        void * parameter = item.payload == LegacyInputPayload::WindowMessage
            ? static_cast<void *>(&item.window_message)
            : reinterpret_cast<void *>(item.immediate_value);
        sink(context, item.message, parameter);
    }
}
}
