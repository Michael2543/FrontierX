#include "FrontierX/SdlWindow.h"
#include "FrontierX/LegacyInput.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

void Push(SDL_Event & event)
{
    Require(SDL_PushEvent(&event), SDL_GetError());
}

void Ignore(void *, std::uint32_t, void *) noexcept
{
}
}

int main(int argc, char ** argv)
{
    try
    {
        const bool visible_smoke = argc == 2 && std::string(argv[1]) == "--visible-smoke";
        Require(argc == 1 || visible_smoke, "usage: frontierx_sdl3_window_probe [--visible-smoke]");
        FrontierX::SdlWindow window;
        std::string error;
        if (!window.Open("FrontierX SDL3 probe", 320, 200, !visible_smoke, error))
            throw std::runtime_error(error);
        Require(window.WindowId() != 0, "SDL window has no ID");
        if (!window.StartTextInput(error)) throw std::runtime_error(error);

        if (visible_smoke)
        {
            const char * driver = SDL_GetCurrentVideoDriver();
            Require(driver && *driver, "SDL did not report a video driver");
            const std::string driver_name(driver);
            FrontierX::SdlLegacyInputAdapter adapter;
            FrontierX::WindowEvent event;
            const Uint64 deadline = SDL_GetTicks() + 250;
            bool close_requested = false;
            while (!close_requested && SDL_GetTicks() < deadline)
            {
                while (window.PollEvent(event))
                {
                    adapter.Dispatch(event, Ignore);
                    close_requested = event.type == FrontierX::WindowEventType::CloseRequested ||
                                      event.type == FrontierX::WindowEventType::QuitRequested;
                }
                SDL_Delay(1);
            }
            window.Close();
            std::cout << "SDL3 visible-window smoke passed; video driver: " << driver_name << '\n';
            return EXIT_SUCCESS;
        }

        SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
        const SDL_WindowID id = window.WindowId();
        const SDL_WindowID foreign_id = id == 1 ? 2 : 1;
        SDL_Event source{};

        source.window.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        source.window.timestamp = 10;
        source.window.windowID = foreign_id;
        Push(source);

        source = {};
        source.window.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        source.window.timestamp = 11;
        source.window.windowID = id;
        Push(source);

        source = {};
        source.key.type = SDL_EVENT_KEY_DOWN;
        source.key.timestamp = 12;
        source.key.windowID = id;
        source.key.scancode = SDL_SCANCODE_A;
        source.key.key = SDLK_A;
        source.key.mod = SDL_KMOD_LSHIFT;
        source.key.repeat = true;
        Push(source);

        source.key.type = SDL_EVENT_KEY_UP;
        source.key.timestamp = 13;
        source.key.repeat = false;
        Push(source);

        const char text[] = "\xC3\x85";
        source = {};
        source.text.type = SDL_EVENT_TEXT_INPUT;
        source.text.timestamp = 14;
        source.text.windowID = id;
        source.text.text = text;
        Push(source);

        source = {};
        source.motion.type = SDL_EVENT_MOUSE_MOTION;
        source.motion.timestamp = 15;
        source.motion.windowID = id;
        source.motion.state = SDL_BUTTON_LMASK;
        source.motion.x = 12.5F;
        source.motion.y = 20.25F;
        source.motion.xrel = 1.5F;
        source.motion.yrel = -2.0F;
        Push(source);

        source = {};
        source.button.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
        source.button.timestamp = 16;
        source.button.windowID = id;
        source.button.button = SDL_BUTTON_RIGHT;
        source.button.clicks = 2;
        source.button.x = 31.0F;
        source.button.y = 41.0F;
        Push(source);

        source.button.type = SDL_EVENT_MOUSE_BUTTON_UP;
        source.button.timestamp = 17;
        source.button.clicks = 1;
        Push(source);

        source = {};
        source.wheel.type = SDL_EVENT_MOUSE_WHEEL;
        source.wheel.timestamp = 18;
        source.wheel.windowID = id;
        source.wheel.x = 1.0F;
        source.wheel.y = 2.0F;
        source.wheel.integer_x = 1;
        source.wheel.integer_y = 2;
        source.wheel.direction = SDL_MOUSEWHEEL_FLIPPED;
        source.wheel.mouse_x = 50.0F;
        source.wheel.mouse_y = 60.0F;
        Push(source);

        source = {};
        source.window.type = SDL_EVENT_WINDOW_RESIZED;
        source.window.timestamp = 19;
        source.window.windowID = id;
        source.window.data1 = 640;
        source.window.data2 = 480;
        Push(source);

        source = {};
        source.window.type = SDL_EVENT_WINDOW_FOCUS_LOST;
        source.window.timestamp = 20;
        source.window.windowID = id;
        Push(source);

        source = {};
        source.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        source.window.timestamp = 21;
        source.window.windowID = id;
        Push(source);

        source = {};
        source.quit.type = SDL_EVENT_QUIT;
        source.quit.timestamp = 22;
        Push(source);

        FrontierX::WindowEvent event;
        FrontierX::SdlLegacyInputAdapter adapter;
        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::FocusGained &&
            event.timestamp_ns == 11 && event.window_id == id, "focus-gained translation");
        auto legacy = adapter.Translate(event);
        Require(legacy.size() == 2 && legacy[0].message == FrontierX::LegacyMessage::ActivateApp &&
            legacy[1].message == FrontierX::LegacyMessage::SetFocus,
            "focus compatibility integration");

        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::KeyDown,
            "key-down translation");
        const auto & key = std::get<FrontierX::KeyEvent>(event.payload);
        Require(key.physical_key == SDL_SCANCODE_A && key.logical_key == SDLK_A &&
            key.modifiers == SDL_KMOD_LSHIFT && key.repeat, "key payload");
        legacy = adapter.Translate(event);
        Require(legacy.size() == 1 && legacy[0].message == FrontierX::LegacyMessage::KeyDown &&
            legacy[0].window_message.wparam == 'A' &&
            (legacy[0].window_message.lparam & 0x40000000) != 0,
            "key compatibility integration");
        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::KeyUp &&
            !std::get<FrontierX::KeyEvent>(event.payload).repeat, "key-up translation");

        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::TextInput &&
            std::get<FrontierX::TextInputEvent>(event.payload).utf8 == text, "owned UTF-8 text input");

        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::MouseMotion,
            "mouse-motion translation");
        const auto & motion = std::get<FrontierX::MouseMotionEvent>(event.payload);
        Require(motion.buttons == SDL_BUTTON_LMASK && motion.x == 12.5F && motion.y == 20.25F &&
            motion.delta_x == 1.5F && motion.delta_y == -2.0F, "mouse-motion payload");

        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::MouseButtonDown,
            "mouse-button translation");
        const auto & button = std::get<FrontierX::MouseButtonEvent>(event.payload);
        Require(button.button == SDL_BUTTON_RIGHT && button.clicks == 2 &&
            button.x == 31.0F && button.y == 41.0F, "mouse-button payload");
        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::MouseButtonUp &&
            std::get<FrontierX::MouseButtonEvent>(event.payload).clicks == 1,
            "mouse-button-up translation");

        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::MouseWheel,
            "mouse-wheel translation");
        const auto & wheel = std::get<FrontierX::MouseWheelEvent>(event.payload);
        Require(wheel.x == -1.0F && wheel.y == -2.0F && wheel.integer_x == -1 &&
            wheel.integer_y == -2 && wheel.mouse_x == 50.0F && wheel.mouse_y == 60.0F,
            "normalized mouse-wheel payload");

        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::Resized,
            "resize translation");
        const auto & size = std::get<FrontierX::WindowSizeEvent>(event.payload);
        Require(size.width == 640 && size.height == 480, "resize payload");
        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::FocusLost,
            "focus-lost translation");
        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::CloseRequested,
            "close translation");
        Require(window.PollEvent(event) && event.type == FrontierX::WindowEventType::QuitRequested &&
            event.window_id == 0, "quit translation");
        Require(!window.PollEvent(event), "unexpected translated event");

        window.Close();
        std::cout << "SDL3 window and input translation passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "SDL3 probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
