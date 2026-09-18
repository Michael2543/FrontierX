#include "FrontierX/SdlWindow.h"

#include <SDL3/SDL.h>

#include <utility>

namespace FrontierX
{
namespace
{
bool Translate(const SDL_Event & source, WindowEvent & destination)
{
    destination.payload = std::monostate{};
    destination.timestamp_ns = source.common.timestamp;
    destination.window_id = 0;

    switch (source.type)
    {
    case SDL_EVENT_QUIT:
        destination.type = WindowEventType::QuitRequested;
        return true;
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        destination.type = WindowEventType::CloseRequested;
        destination.window_id = source.window.windowID;
        return true;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        destination.type = WindowEventType::FocusGained;
        destination.window_id = source.window.windowID;
        return true;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        destination.type = WindowEventType::FocusLost;
        destination.window_id = source.window.windowID;
        return true;
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        destination.type = source.type == SDL_EVENT_WINDOW_RESIZED
            ? WindowEventType::Resized : WindowEventType::PixelSizeChanged;
        destination.window_id = source.window.windowID;
        destination.payload = WindowSizeEvent{source.window.data1, source.window.data2};
        return true;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        destination.type = source.type == SDL_EVENT_KEY_DOWN
            ? WindowEventType::KeyDown : WindowEventType::KeyUp;
        destination.window_id = source.key.windowID;
        destination.payload = KeyEvent{static_cast<std::uint32_t>(source.key.scancode),
            static_cast<std::uint32_t>(source.key.key), static_cast<std::uint16_t>(source.key.mod),
            source.key.repeat};
        return true;
    case SDL_EVENT_TEXT_INPUT:
        destination.type = WindowEventType::TextInput;
        destination.window_id = source.text.windowID;
        destination.payload = TextInputEvent{source.text.text ? source.text.text : ""};
        return true;
    case SDL_EVENT_MOUSE_MOTION:
        destination.type = WindowEventType::MouseMotion;
        destination.window_id = source.motion.windowID;
        destination.payload = MouseMotionEvent{static_cast<std::uint32_t>(source.motion.state),
            source.motion.x, source.motion.y, source.motion.xrel, source.motion.yrel};
        return true;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        destination.type = source.type == SDL_EVENT_MOUSE_BUTTON_DOWN
            ? WindowEventType::MouseButtonDown : WindowEventType::MouseButtonUp;
        destination.window_id = source.button.windowID;
        destination.payload = MouseButtonEvent{source.button.button, source.button.clicks,
            source.button.x, source.button.y};
        return true;
    case SDL_EVENT_MOUSE_WHEEL:
    {
        destination.type = WindowEventType::MouseWheel;
        destination.window_id = source.wheel.windowID;
        const float direction = source.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0F : 1.0F;
        const std::int32_t integer_direction = direction < 0.0F ? -1 : 1;
        destination.payload = MouseWheelEvent{source.wheel.x * direction, source.wheel.y * direction,
            source.wheel.integer_x * integer_direction, source.wheel.integer_y * integer_direction,
            source.wheel.mouse_x, source.wheel.mouse_y};
        return true;
    }
    default:
        return false;
    }
}

bool AcceptEvent(SdlWindow & window, const SDL_Event & source, WindowEvent & event)
{
    WindowEvent translated;
    if (!Translate(source, translated)) return false;
    if (translated.window_id != 0 && translated.window_id != window.WindowId()) return false;
    event = std::move(translated);
    return true;
}
}

SdlWindow::~SdlWindow()
{
    Close();
}

bool SdlWindow::Open(const char * title, int width, int height, bool hidden, std::string & error)
{
    Close();
    error.clear();
    if (!title || width <= 0 || height <= 0)
    {
        error = "invalid SDL window parameters";
        return false;
    }
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        error = SDL_GetError();
        return false;
    }
    video_initialized_ = true;
    const SDL_WindowFlags flags = hidden ? SDL_WINDOW_HIDDEN : 0;
    window_ = SDL_CreateWindow(title, width, height, flags);
    if (!window_)
    {
        error = SDL_GetError();
        Close();
        return false;
    }
    return true;
}

bool SdlWindow::StartTextInput(std::string & error)
{
    error.clear();
    if (!window_)
    {
        error = "SDL window is not open";
        return false;
    }
    if (!SDL_StartTextInput(static_cast<SDL_Window *>(window_)))
    {
        error = SDL_GetError();
        return false;
    }
    return true;
}

bool SdlWindow::Configure(int width, int height, bool fullscreen, bool resizable,
                          bool always_on_top, bool show, bool center, std::string & error)
{
    error.clear();
    auto * window = static_cast<SDL_Window *>(window_);
    if (!window || width <= 0 || height <= 0)
    {
        error = "invalid SDL window configuration";
        return false;
    }
    if (!SDL_SetWindowResizable(window, resizable) ||
        !SDL_SetWindowAlwaysOnTop(window, always_on_top) ||
        !SDL_SetWindowFullscreen(window, fullscreen))
    {
        error = SDL_GetError();
        return false;
    }
    if (!fullscreen && !SDL_SetWindowSize(window, width, height))
    {
        error = SDL_GetError();
        return false;
    }
    if (center && !SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED))
    {
        error = SDL_GetError();
        return false;
    }
    if (show && !SDL_ShowWindow(window))
    {
        error = SDL_GetError();
        return false;
    }
    return true;
}

bool SdlWindow::PollEvent(WindowEvent & event)
{
    SDL_Event source{};
    while (SDL_PollEvent(&source))
    {
        if (AcceptEvent(*this, source, event)) return true;
    }
    return false;
}

bool SdlWindow::WaitEvent(WindowEvent & event, std::string & error)
{
    error.clear();
    if (!window_)
    {
        error = "SDL window is not open";
        return false;
    }
    SDL_Event source{};
    while (SDL_WaitEvent(&source))
        if (AcceptEvent(*this, source, event)) return true;
    error = SDL_GetError();
    return false;
}

bool SdlWindow::WindowArea(int & x, int & y, int & width, int & height) const
{
    auto * window = static_cast<SDL_Window *>(window_);
    if (!window || !SDL_GetWindowPosition(window, &x, &y) ||
        !SDL_GetWindowSize(window, &width, &height)) return false;
    int top = 0;
    int left = 0;
    int bottom = 0;
    int right = 0;
    if (SDL_GetWindowBordersSize(window, &top, &left, &bottom, &right))
    {
        x -= left;
        y -= top;
        width += left + right;
        height += top + bottom;
    }
    return true;
}

bool SdlWindow::ClientArea(int & x, int & y, int & width, int & height) const
{
    auto * window = static_cast<SDL_Window *>(window_);
    return window && SDL_GetWindowPosition(window, &x, &y) &&
        SDL_GetWindowSize(window, &width, &height);
}

void SdlWindow::SetCursorPosition(int x, int y)
{
    if (window_) SDL_WarpMouseInWindow(static_cast<SDL_Window *>(window_),
                                      static_cast<float>(x), static_cast<float>(y));
}

void SdlWindow::CursorPosition(int & x, int & y) const
{
    float float_x = 0.0F;
    float float_y = 0.0F;
    SDL_GetMouseState(&float_x, &float_y);
    x = static_cast<int>(float_x);
    y = static_cast<int>(float_y);
}

std::uint32_t SdlWindow::WindowId() const
{
    return window_ ? SDL_GetWindowID(static_cast<SDL_Window *>(window_)) : 0;
}

std::string SdlWindow::VideoDriverName() const
{
    const char * driver = video_initialized_ ? SDL_GetCurrentVideoDriver() : nullptr;
    return driver ? driver : "";
}

bool SdlWindow::IsOpen() const
{
    return window_ != nullptr;
}

void SdlWindow::Close()
{
    if (window_)
    {
        SDL_DestroyWindow(static_cast<SDL_Window *>(window_));
        window_ = nullptr;
    }
    if (video_initialized_)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        video_initialized_ = false;
    }
}

std::string CurrentSdlVideoDriverName()
{
    const char * driver = SDL_GetCurrentVideoDriver();
    return driver ? driver : "";
}
}
