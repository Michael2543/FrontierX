#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeWindowManager.h"
#include "FrontierX/LegacyInput.h"
#include "IConnection.h"
#include "SYSTEM.H"
#include "WindowManager.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
struct MessageRecord
{
    HWND window;
    UINT message;
    WPARAM wparam;
    LPARAM lparam;
};

std::vector<MessageRecord> messages;
int exit_calls = 0;

void DACOM_CDECL ExitHandler(int code)
{
    if (code == 0) ++exit_calls;
}

LRESULT COMAPI WindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    messages.push_back({window, message, wparam, lparam});
    return 0;
}

void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

void Push(SDL_Event & event)
{
    Require(SDL_PushEvent(&event), SDL_GetError());
}

struct CloseCallback final : ISystemEventCallback
{
    GENRESULT QueryInterface(const C8 * interface_name, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interface_name && std::strcmp(interface_name, "ISystemEventCallback") == 0)
        {
            *instance = this;
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }
    BOOL32 OnAppClose() override
    {
        ++calls;
        return veto ? TRUE : FALSE;
    }

    U32 references = 0;
    U32 calls = 0;
    bool veto = true;
};
}

int main()
{
    try
    {
        FrontierX::NativeComponentManager manager;
        FrontierX::RegisterNativeWindowManager(&manager);
        Require(manager.RegisteredCount() == 1,
            "native window-manager factory registration failed");

        AGGDESC descriptor("IWindowManager", "WindowManager");
        void * raw_aggregate = nullptr;
        Require(manager.CreateInstance(&descriptor, &raw_aggregate) == GR_OK && raw_aggregate,
            "native window-manager aggregate creation failed");
        auto * aggregate = static_cast<IAggregateComponent *>(raw_aggregate);

        IWindowManager * window = nullptr;
        ISystemComponent * system = nullptr;
        Require(aggregate->QueryInterface(IID_IWindowManager,
            reinterpret_cast<void **>(&window)) == GR_OK && window,
            "IWindowManager query failed");
        Require(aggregate->QueryInterface(IID_ISystemComponent,
            reinterpret_cast<void **>(&system)) == GR_OK && system,
            "native window ISystemComponent query failed");
        Require(aggregate->Initialize() == GR_OK, "native window aggregate initialization failed");
        Require(window->Startup(nullptr, "FrontierX native window probe", ExitHandler,
            WMF_PREVENT_MULTIPLE, nullptr) == 1, "native window startup failed");
        Require(window->GetWindowHandle() != 0, "native window token is zero");
        Require(window->SetWindowPos(400, 300,
            WMF_ALLOW_WINDOW_RESIZE | WMF_CENTER) == TRUE,
            "native window configuration failed");

        WM_WINAREA client{};
        window->GetClientArea(client);
        Require(client.w == 400 && client.h == 300,
            "native client-area dimensions changed");
        window->SetCursorPos(17, 23);
        S32 cursor_x = 0;
        S32 cursor_y = 0;
        window->GetCursorPos(cursor_x, cursor_y);
        Require(cursor_x == 17 && cursor_y == 23,
            "native cursor cache did not round trip");

        void * second_raw_aggregate = nullptr;
        Require(manager.CreateInstance(&descriptor, &second_raw_aggregate) == GR_OK &&
            second_raw_aggregate, "second native window aggregate creation failed");
        auto * second_aggregate = static_cast<IAggregateComponent *>(second_raw_aggregate);
        IWindowManager * second_window = nullptr;
        Require(second_aggregate->QueryInterface(IID_IWindowManager,
            reinterpret_cast<void **>(&second_window)) == GR_OK && second_window,
            "second IWindowManager query failed");
        Require(second_window->Startup(nullptr, "duplicate", ExitHandler,
            WMF_PREVENT_MULTIPLE, nullptr) == -1,
            "prevent-multiple startup contract changed");
        second_window->Release();
        second_aggregate->Release();

        IDAConnectionPoint * close_point = nullptr;
        Require(window->QueryOutgoingInterface("ISystemEventCallback", &close_point) == GR_OK &&
            close_point, "close callback connection-point query failed");
        CloseCallback close_callback;
        U32 close_token = 0;
        Require(close_point->Advise(&close_callback, &close_token) == GR_OK && close_token == 1,
            "close callback subscription failed");

        window->SetCallback(WindowCallback);
        SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
        const auto window_id = static_cast<SDL_WindowID>(window->GetWindowHandle());
        SDL_Event event{};
        event.window.type = SDL_EVENT_WINDOW_FOCUS_LOST;
        event.window.windowID = window_id;
        Push(event);
        event = {};
        event.motion.type = SDL_EVENT_MOUSE_MOTION;
        event.motion.windowID = window_id;
        event.motion.x = 31.0F;
        event.motion.y = 41.0F;
        Push(event);
        system->Update();
        Require(window->AppIsActive() == FALSE,
            "focus-lost event did not update native window state");
        window->GetCursorPos(cursor_x, cursor_y);
        Require(cursor_x == 31 && cursor_y == 41,
            "mouse motion did not update native cursor state");
        Require(messages.size() == 3 &&
            messages[0].message == FrontierX::LegacyMessage::KillFocus &&
            messages[1].message == FrontierX::LegacyMessage::ActivateApp &&
            messages[2].message == FrontierX::LegacyMessage::MouseMove,
            "native window callback dispatch order changed");

        event = {};
        event.window.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        event.window.windowID = window_id;
        Push(event);
        window->ServeMessageQueue(WMF_WAIT | WMF_BACKGROUND);
        Require(window->AppIsActive() == TRUE,
            "focus-gained event did not update native window state");

        const std::size_t before_close = messages.size();
        event = {};
        event.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        event.window.windowID = window_id;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(close_callback.calls == 1 && messages.size() == before_close,
            "close veto did not suppress the window callback");

        close_callback.veto = false;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(close_callback.calls == 2 && messages.size() == before_close + 1 &&
            messages.back().message == FrontierX::LegacyMessage::Close,
            "accepted close did not reach the window callback");

        event = {};
        event.quit.type = SDL_EVENT_QUIT;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(exit_calls == 1, "quit event did not invoke the configured exit callback");

        Require(close_point->Unadvise(close_token) == GR_OK,
            "close callback unsubscribe failed");
        close_point->Release();
        system->Release();
        window->Release();
        aggregate->Release();
        Require(manager.ShutDown() == GR_OK && manager.RegisteredCount() == 0,
            "native window manager shutdown failed");

        std::cout << "native SDL IWindowManager component passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "native window-manager probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
