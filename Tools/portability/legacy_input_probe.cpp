#include "FrontierX/LegacyInput.h"
#include "FrontierX/LegacyEventDispatch.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/SdlEventPump.h"
#include "FrontierX/SdlWindow.h"
#include "HeapObj.h"
#include "IConnection.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

FrontierX::WindowEvent Event(FrontierX::WindowEventType type)
{
    FrontierX::WindowEvent event;
    event.type = type;
    event.window_id = 7;
    return event;
}

std::int16_t HighSigned(std::uintptr_t value)
{
    return static_cast<std::int16_t>((value >> 16) & 0xffff);
}

struct SinkCapture
{
    std::uint32_t message = 0;
    FrontierX::LegacyWindowMessage window_message;
    std::size_t calls = 0;
};

struct EventSystemCapture final : IEventCallback
{
    struct Record
    {
        U32 message = 0;
        FrontierX::LegacyWindowMessage windowMessage;
        std::uintptr_t immediateValue = 0;
    };

    GENRESULT QueryInterface(const C8 * interfaceName, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interfaceName && std::strcmp(interfaceName, "IEventCallback") == 0)
        {
            *instance = static_cast<IEventCallback *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }

    GENRESULT Notify(U32 notifiedMessage, void * parameter) override
    {
        Record record;
        record.message = notifiedMessage;
        message = notifiedMessage;
        if (notifiedMessage == FrontierX::LegacyMessage::UnicodeCharacter)
        {
            immediateValue = reinterpret_cast<std::uintptr_t>(parameter);
            record.immediateValue = immediateValue;
        }
        else if (parameter)
        {
            windowMessage = *static_cast<FrontierX::LegacyWindowMessage *>(parameter);
            record.windowMessage = windowMessage;
        }
        records.push_back(record);
        ++calls;
        return GR_OK;
    }

    U32 references = 0;
    U32 message = 0;
    FrontierX::LegacyWindowMessage windowMessage;
    std::uintptr_t immediateValue = 0;
    std::size_t calls = 0;
    std::vector<Record> records;
};

void Capture(void * opaque, std::uint32_t message, void * parameter) noexcept
{
    auto & capture = *static_cast<SinkCapture *>(opaque);
    capture.message = message;
    if (parameter) capture.window_message = *static_cast<FrontierX::LegacyWindowMessage *>(parameter);
    ++capture.calls;
}
}

int main()
{
    try
    {
        Require(HEAP_Acquire() != nullptr, "native shared heap acquisition failed");
        int contextValue = 0;
        const auto legacyParameter = FrontierX::LegacyLParamFromPointer(&contextValue);
        Require(FrontierX::PointerFromLegacyLParam<int>(legacyParameter) == &contextValue,
            "legacy lParam did not preserve pointer identity");
        Require(sizeof(FrontierX::LegacyWindowToken) == sizeof(void *) &&
            sizeof(FrontierX::LegacyWParam) == sizeof(void *) &&
            sizeof(FrontierX::LegacyLParam) == sizeof(void *),
            "legacy message parameters are not pointer-width");

        FrontierX::SdlLegacyInputAdapter adapter;

        auto translated = adapter.Translate(Event(FrontierX::WindowEventType::FocusGained));
        Require(translated.size() == 2 &&
            translated[0].message == FrontierX::LegacyMessage::ActivateApp &&
            translated[0].window_message.wparam == 1 &&
            translated[1].message == FrontierX::LegacyMessage::SetFocus,
            "focus-gained message sequence");

        auto key = Event(FrontierX::WindowEventType::KeyDown);
        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_A, SDLK_A, SDL_KMOD_NONE, false};
        translated = adapter.Translate(key);
        Require(translated.size() == 1 && translated[0].message == FrontierX::LegacyMessage::KeyDown &&
            translated[0].window_message.wparam == 'A', "letter virtual-key translation");
        Require((translated[0].window_message.lparam & 0x00ff0000) == 0x001e0000 &&
            (translated[0].window_message.lparam & 0xc0000000) == 0,
            "letter scan/repeat/transition bits");

        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_A, SDLK_A, SDL_KMOD_NONE, true};
        translated = adapter.Translate(key);
        Require((translated[0].window_message.lparam & 0x40000000) != 0,
            "repeat did not set previous-state bit");
        key.type = FrontierX::WindowEventType::KeyUp;
        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_A, SDLK_A, SDL_KMOD_NONE, false};
        translated = adapter.Translate(key);
        Require(translated[0].message == FrontierX::LegacyMessage::KeyUp &&
            (translated[0].window_message.lparam & 0xc0000000) == 0xc0000000,
            "key-up state/transition bits");

        key.type = FrontierX::WindowEventType::KeyDown;
        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_RCTRL, SDLK_RCTRL, SDL_KMOD_RCTRL, false};
        translated = adapter.Translate(key);
        Require(translated[0].window_message.wparam == 0x11 &&
            (translated[0].window_message.lparam & 0x01000000) != 0,
            "right-control compatibility bits");

        // Original Frame::onEscPressed and focus navigation consume WM_CHAR.
        for (const auto control : {SDLK_ESCAPE, SDLK_TAB, SDLK_BACKSPACE, SDLK_RETURN})
        {
            key.payload = FrontierX::KeyEvent{SDL_SCANCODE_ESCAPE, control, SDL_KMOD_NONE, false};
            translated = adapter.Translate(key);
            Require(translated.size() == 3 &&
                translated[0].message == FrontierX::LegacyMessage::KeyDown &&
                translated[1].message == FrontierX::LegacyMessage::UnicodeCharacter &&
                translated[2].message == FrontierX::LegacyMessage::Character &&
                translated[2].window_message.wparam == translated[0].window_message.wparam,
                "Frame control key lacks ordered character delivery");
            key.type = FrontierX::WindowEventType::KeyUp;
            Require(adapter.Translate(key).size() == 1,
                "control release generated a duplicate character");
            key.type = FrontierX::WindowEventType::KeyDown;
        }
        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_TAB, SDLK_TAB, SDL_KMOD_SHIFT, true};
        translated = adapter.Translate(key);
        Require(translated.size() == 3 &&
            (translated[2].window_message.lparam & 0x40000000) != 0,
            "shift-tab/repeat character contract");
        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_ESCAPE, SDLK_ESCAPE, SDL_KMOD_CTRL, false};
        Require(adapter.Translate(key).size() == 1,
            "Ctrl shortcut generated an unrequested Frame character");

        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_A, SDLK_A, SDL_KMOD_LALT, false};
        translated = adapter.Translate(key);
        Require(translated[0].message == FrontierX::LegacyMessage::SystemKeyDown &&
            (translated[0].window_message.lparam & 0x20000000) != 0,
            "Alt system-key compatibility bits");

        key.payload = FrontierX::KeyEvent{SDL_SCANCODE_A, SDLK_A,
            static_cast<std::uint16_t>(SDL_KMOD_SHIFT | SDL_KMOD_CTRL), false};
        adapter.Translate(key);
        auto motion = Event(FrontierX::WindowEventType::MouseMotion);
        motion.payload = FrontierX::MouseMotionEvent{SDL_BUTTON_LMASK, -2.0F, 300.0F, 0.0F, 0.0F};
        translated = adapter.Translate(motion);
        Require(translated.size() == 1 && translated[0].message == FrontierX::LegacyMessage::MouseMove &&
            translated[0].window_message.wparam ==
                (FrontierX::LegacyMouseMask::Left | FrontierX::LegacyMouseMask::Shift |
                 FrontierX::LegacyMouseMask::Control),
            "mouse button/modifier mask");
        Require((translated[0].window_message.lparam & 0xffff) == 0xfffe &&
            ((translated[0].window_message.lparam >> 16) & 0xffff) == 300,
            "signed mouse-coordinate packing");

        motion.payload = FrontierX::MouseMotionEvent{0,
            std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(),
            0.0F, 0.0F};
        translated = adapter.Translate(motion);
        Require(translated[0].window_message.lparam == 0,
            "non-finite mouse coordinates were converted to integers");

        auto button = Event(FrontierX::WindowEventType::MouseButtonDown);
        button.payload = FrontierX::MouseButtonEvent{SDL_BUTTON_RIGHT, 1, 10.0F, 20.0F};
        translated = adapter.Translate(button);
        Require(translated.size() == 3 &&
            translated[0].message == FrontierX::LegacyMessage::MouseMove &&
            translated[1].message == FrontierX::LegacyMessage::RightButtonDown &&
            translated[2].message == FrontierX::LegacyMessage::MouseMove &&
            (translated[1].window_message.wparam & FrontierX::LegacyMouseMask::Right),
            "button move/down/move sequence");

        adapter.Translate(Event(FrontierX::WindowEventType::FocusLost));
        button.payload = FrontierX::MouseButtonEvent{SDL_BUTTON_LEFT, 2, 11.0F, 21.0F};
        translated = adapter.Translate(button);
        Require(translated.size() == 4 &&
            translated[1].message == FrontierX::LegacyMessage::LeftButtonDown &&
            translated[3].message == FrontierX::LegacyMessage::LeftButtonDoubleClick,
            "double-click compatibility sequence");

        auto wheel = Event(FrontierX::WindowEventType::MouseWheel);
        wheel.payload = FrontierX::MouseWheelEvent{0.0F, -1.0F, 0, -1, -5.0F, 9.0F};
        translated = adapter.Translate(wheel);
        Require(translated.size() == 1 && translated[0].message == FrontierX::LegacyMessage::MouseWheel &&
            HighSigned(translated[0].window_message.wparam) == -120,
            "mouse-wheel delta packing");

        auto resize = Event(FrontierX::WindowEventType::Resized);
        resize.payload = FrontierX::WindowSizeEvent{640, 480};
        translated = adapter.Translate(resize);
        Require(translated.size() == 1 && translated[0].message == FrontierX::LegacyMessage::Size &&
            (translated[0].window_message.lparam & 0xffff) == 640 &&
            ((translated[0].window_message.lparam >> 16) & 0xffff) == 480,
            "logical resize packing");

        auto text = Event(FrontierX::WindowEventType::TextInput);
        text.payload = FrontierX::TextInputEvent{"A\xC3\x85\xF0\x9F\x98\x80"};
        translated = adapter.Translate(text);
        Require(translated.size() == 8, "UTF-8 to UTF-16 event count");
        const std::uint16_t expected_units[] = {0x0041, 0x00c5, 0xd83d, 0xde00};
        for (std::size_t index = 0; index < 4; ++index)
        {
            const auto & unicode = translated[index * 2];
            const auto & character = translated[index * 2 + 1];
            Require(unicode.message == FrontierX::LegacyMessage::UnicodeCharacter &&
                unicode.payload == FrontierX::LegacyInputPayload::ImmediateValue &&
                unicode.immediate_value == expected_units[index], "Unicode event payload");
            Require(character.message == FrontierX::LegacyMessage::Character &&
                character.window_message.wparam == expected_units[index], "character message payload");
        }

        auto unknown = Event(FrontierX::WindowEventType::KeyDown);
        unknown.payload = FrontierX::KeyEvent{0, 0, 0, false};
        Require(adapter.Translate(unknown).empty(), "unknown key produced a compatibility message");

        SinkCapture capture;
        adapter.Dispatch(resize, Capture, &capture);
        Require(capture.calls == 1 && capture.message == FrontierX::LegacyMessage::Size &&
            capture.window_message.message == FrontierX::LegacyMessage::Size &&
            capture.window_message.window == 7,
            "synchronous dispatch did not expose a valid owned window message");
        adapter.Dispatch(resize, nullptr);

        FrontierX::NativeComponentManager manager;
        RegisterTheEventSystem(&manager);
        AGGDESC descriptor("IEventSystem", "EventSystem");
        void * aggregateInstance = nullptr;
        Require(manager.CreateInstance(&descriptor, &aggregateInstance) == GR_OK &&
            aggregateInstance, "production event-system creation failed");
        auto * aggregate = static_cast<IAggregateComponent *>(aggregateInstance);
        IEventSystem * eventSystem = nullptr;
        Require(aggregate->QueryInterface(IID_IEventSystem,
            reinterpret_cast<void **>(&eventSystem)) == GR_OK && eventSystem,
            "production IEventSystem query failed");
        IDAConnectionPoint * eventPoint = nullptr;
        Require(eventSystem->QueryOutgoingInterface("IEventCallback", &eventPoint) == GR_OK &&
            eventPoint, "production event callback point query failed");
        EventSystemCapture eventCapture;
        U32 eventToken = 0;
        Require(eventPoint->Advise(&eventCapture, &eventToken) == GR_OK,
            "production event callback subscription failed");

        adapter.Dispatch(resize, FrontierX::DispatchLegacyInputToEventSystem, eventSystem);
        Require(eventCapture.calls == 1 && eventCapture.message == FrontierX::LegacyMessage::Size &&
            eventCapture.windowMessage.message == FrontierX::LegacyMessage::Size &&
            eventCapture.windowMessage.window == 7 &&
            (eventCapture.windowMessage.lparam & 0xffff) == 640 &&
            ((eventCapture.windowMessage.lparam >> 16) & 0xffff) == 480,
            "SDL compatibility record did not traverse the production event system synchronously");
        adapter.Dispatch(text, FrontierX::DispatchLegacyInputToEventSystem, eventSystem);
        Require(eventCapture.calls == 9 &&
            eventCapture.immediateValue == 0xde00 &&
            eventCapture.message == FrontierX::LegacyMessage::Character &&
            eventCapture.windowMessage.wparam == 0xde00,
            "immediate Unicode and window-message payloads did not retain dispatch semantics");

        FrontierX::SdlWindow window;
        std::string windowError;
        if (!window.Open("FrontierX event-pump probe", 320, 200, true, windowError))
            throw std::runtime_error(windowError);
        SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
        const SDL_WindowID windowId = window.WindowId();
        const SDL_WindowID foreignId = windowId == 1 ? 2 : 1;
        SDL_Event source{};
        source.window.type = SDL_EVENT_WINDOW_RESIZED;
        source.window.windowID = foreignId;
        source.window.data1 = 1;
        source.window.data2 = 1;
        Require(SDL_PushEvent(&source), SDL_GetError());

        source = {};
        source.window.type = SDL_EVENT_WINDOW_RESIZED;
        source.window.windowID = windowId;
        source.window.data1 = 800;
        source.window.data2 = 600;
        Require(SDL_PushEvent(&source), SDL_GetError());

        const char pumpText[] = "\xC3\x85";
        source = {};
        source.text.type = SDL_EVENT_TEXT_INPUT;
        source.text.windowID = windowId;
        source.text.text = pumpText;
        Require(SDL_PushEvent(&source), SDL_GetError());

        source = {};
        source.quit.type = SDL_EVENT_QUIT;
        Require(SDL_PushEvent(&source), SDL_GetError());

        eventCapture.calls = 0;
        eventCapture.records.clear();
        FrontierX::SdlEventPump pump(window, *eventSystem);
        const auto pumpResult = pump.Pump();
        Require(pumpResult.event_count == 3 && !pumpResult.close_requested &&
            pumpResult.quit_requested, "SDL event pump did not report the drained batch");
        Require(eventCapture.calls == 4 && eventCapture.records.size() == 4,
            "SDL event pump did not synchronously deliver the expected legacy records");
        Require(eventCapture.records[0].message == FrontierX::LegacyMessage::Size &&
            eventCapture.records[0].windowMessage.window == windowId &&
            (eventCapture.records[0].windowMessage.lparam & 0xffff) == 800 &&
            ((eventCapture.records[0].windowMessage.lparam >> 16) & 0xffff) == 600,
            "SDL event pump did not preserve resize order or filter foreign-window input");
        Require(eventCapture.records[1].message == FrontierX::LegacyMessage::UnicodeCharacter &&
            eventCapture.records[1].immediateValue == 0x00c5 &&
            eventCapture.records[2].message == FrontierX::LegacyMessage::Character &&
            eventCapture.records[2].windowMessage.wparam == 0x00c5 &&
            eventCapture.records[3].message == FrontierX::LegacyMessage::Close,
            "SDL event pump did not preserve text and quit dispatch order");
        window.Close();

        Require(eventPoint->Unadvise(eventToken) == GR_OK,
            "production event callback unsubscribe failed");
        eventPoint->Release();
        eventSystem->Release();
        aggregate->Release();
        manager.ShutDown();

        std::cout << "SDL legacy input and production event dispatch passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "legacy input probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
