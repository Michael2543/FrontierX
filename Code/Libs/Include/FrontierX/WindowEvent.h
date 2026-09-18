#ifndef FRONTIERX_WINDOW_EVENT_H
#define FRONTIERX_WINDOW_EVENT_H

#include <cstdint>
#include <string>
#include <variant>

namespace FrontierX
{
enum class WindowEventType
{
    QuitRequested,
    CloseRequested,
    FocusGained,
    FocusLost,
    Resized,
    PixelSizeChanged,
    KeyDown,
    KeyUp,
    TextInput,
    MouseMotion,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel
};

struct WindowSizeEvent
{
    std::int32_t width = 0;
    std::int32_t height = 0;
};

struct KeyEvent
{
    std::uint32_t physical_key = 0;
    std::uint32_t logical_key = 0;
    std::uint16_t modifiers = 0;
    bool repeat = false;
};

struct TextInputEvent
{
    std::string utf8;
};

struct MouseMotionEvent
{
    std::uint32_t buttons = 0;
    float x = 0.0F;
    float y = 0.0F;
    float delta_x = 0.0F;
    float delta_y = 0.0F;
};

struct MouseButtonEvent
{
    std::uint8_t button = 0;
    std::uint8_t clicks = 0;
    float x = 0.0F;
    float y = 0.0F;
};

struct MouseWheelEvent
{
    float x = 0.0F;
    float y = 0.0F;
    std::int32_t integer_x = 0;
    std::int32_t integer_y = 0;
    float mouse_x = 0.0F;
    float mouse_y = 0.0F;
};

using WindowEventPayload = std::variant<std::monostate, WindowSizeEvent, KeyEvent, TextInputEvent,
                                        MouseMotionEvent, MouseButtonEvent, MouseWheelEvent>;

struct WindowEvent
{
    WindowEventType type = WindowEventType::QuitRequested;
    std::uint64_t timestamp_ns = 0;
    std::uint32_t window_id = 0;
    WindowEventPayload payload;
};
}

#endif
