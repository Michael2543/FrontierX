#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeWindowManager.h"
#include "FrontierX/NativeStreamer.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "Streamer.h"
#if defined(FRONTIERX_HAVE_IAC_DECODER)
#include "NativeIacWave.h"
#endif
#include <filesystem>
#include <fstream>
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
IStreamer * closeOnInit = nullptr;

void DACOM_CDECL ExitHandler(int code)
{
    if (code == 0) ++exit_calls;
}

LRESULT COMAPI WindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    messages.push_back({window, message, wparam, lparam});
    if (message == 0x500 && wparam == IStreamer::INITSUCCESS && closeOnInit)
        closeOnInit->CloseHandle(reinterpret_cast<HSTREAM>(lparam));
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

// IStreamer blocks the source profile; SoundManager depends on deferred init and
// completion via this window callback. Extend this owner/pump check, not a mock.
void CheckStreamer(FrontierX::NativeComponentManager & manager, IWindowManager * window)
{
    Require(FrontierX::RegisterNativeStreamer(&manager), "streamer registration failed");
    AGGDESC descriptor("IStreamer", "Streamer");
    void * raw = nullptr;
    Require(manager.CreateInstance(&descriptor, &raw) == GR_OK, "streamer creation failed");
    auto * aggregate = static_cast<IAggregateComponent *>(raw);
    IStreamer * streamer = nullptr;
    Require(aggregate->QueryInterface(IID_IStreamer, reinterpret_cast<void **>(&streamer)) == GR_OK,
            "streamer query failed");
    Require(aggregate->Initialize() == GR_OK, "streamer aggregate init failed");
    STREAMERDESC init;
    init.hMainWindow = window->GetWindowHandle();
    init.uMsg = 0x500;
    Require(!streamer->Open("missing", nullptr, 0), "uninitialized stream accepted");
    STREAMERDESC invalid = init;
    invalid.hMainWindow = static_cast<HWND>(-1);
    Require(!streamer->Init(&invalid), "invalid notification window accepted");
    if (!streamer->Init(&init)) throw std::runtime_error(SDL_GetError());
    Require(!streamer->Init(&init), "duplicate initialization accepted");
    std::cout << "SDL audio driver: " << SDL_GetCurrentAudioDriver() << '\n';
    const auto root = std::filesystem::current_path() / "native_streamer_fixture";
    std::filesystem::create_directories(root);
    std::vector<unsigned char> wav(44 + 1600, 0);
    std::memcpy(wav.data(), "RIFF", 4);
    std::memcpy(wav.data() + 8, "WAVEfmt ", 8);
    auto put = [&](std::size_t offset, unsigned value, unsigned width) {
        for (unsigned i = 0; i < width; ++i) wav[offset + i] = static_cast<unsigned char>(value >> (8 * i));
    };
    put(4, static_cast<unsigned>(wav.size() - 8), 4);
    put(16, 16, 4); put(20, 1, 2); put(22, 1, 2); put(24, 8000, 4);
    put(28, 16000, 4); put(32, 2, 2); put(34, 16, 2);
    std::memcpy(wav.data() + 36, "data", 4); put(40, 1600, 4);
    for (std::size_t i = 44; i < wav.size(); i += 2) put(i, (i / 16) % 2 ? 1000 : 64536, 2);
    std::ofstream file(root / "tone.wav", std::ios::binary);
    file.write(reinterpret_cast<const char *>(wav.data()), static_cast<std::streamsize>(wav.size()));
    file.close();
    std::ofstream(root / "invalid.wav") << "not a wave";
    IFileSystem * source = nullptr;
    std::string error;
    Require(FrontierX::OpenNativeDirectoryFileSystem(root, &source, error), error.c_str());
    Require(!streamer->Open("missing.wav", source, 0) &&
            !streamer->Open("invalid.wav", source, 0), "bad audio accepted");
    const auto before = messages.size();
    HSTREAM stream = streamer->Open("TONE.WAV", source, 0);
    Require(stream && streamer->GetStatus(stream) == IStreamer::STOPPED && messages.size() == before,
            "Open must defer initialization notification");
    window->ServeMessageQueue(WMF_BACKGROUND);
    Require(messages.back().message == 0x500 && messages.back().wparam == IStreamer::INITSUCCESS &&
            messages.back().lparam == reinterpret_cast<LPARAM>(stream), "init message mismatch");
    S32 volume = 0;
    Require(streamer->SetVolume(stream, -2000) && streamer->GetVolume(stream, &volume) && volume == -2000 &&
            !streamer->SetVolume(stream, 1), "volume contract failed");
    Require(streamer->Restart(stream) && streamer->Stop(stream), "pause failed");
    SDL_Delay(150);
    window->ServeMessageQueue(WMF_BACKGROUND);
    Require(streamer->GetStatus(stream) == IStreamer::STOPPED, "paused stream completed");
    Require(streamer->Restart(stream), "resume failed");
    const Uint64 deadline = SDL_GetTicks() + 3000;
    while (streamer->GetStatus(stream) != IStreamer::COMPLETED && SDL_GetTicks() < deadline)
    { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
    Require(streamer->GetStatus(stream) == IStreamer::COMPLETED &&
            messages.back().wparam == IStreamer::COMPLETED && !streamer->Restart(stream),
            "playback did not complete or replay was accepted");
    std::vector<WPARAM> states;
    for (const auto & record : messages)
        if (record.message == 0x500 && record.lparam == reinterpret_cast<LPARAM>(stream))
            states.push_back(record.wparam);
    Require(states == std::vector<WPARAM>{IStreamer::INITSUCCESS, IStreamer::EOFREACHED, IStreamer::COMPLETED},
            "stream notification order or multiplicity changed");
    Require(streamer->CloseHandle(stream) && !streamer->CloseHandle(stream) &&
            streamer->GetStatus(stream) == IStreamer::INVALID, "stale handle accepted");
    HSTREAM loop = streamer->Open("tone.wav", source, STRMFL_LOOPING | STRMFL_PLAY);
    Require(loop && loop != stream, "loop handle was reused");
    const Uint64 loopUntil = SDL_GetTicks() + 350;
    while (SDL_GetTicks() < loopUntil) { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
    Require(streamer->GetStatus(loop) == IStreamer::PLAYING && streamer->CloseHandle(loop),
            "loop prematurely completed");
    HSTREAM cancelled = streamer->Open("tone.wav", source, STRMFL_PLAY);
    Require(cancelled != nullptr, "cancel test open failed");
    closeOnInit = streamer;
    window->ServeMessageQueue(WMF_BACKGROUND);
    closeOnInit = nullptr;
    Require(streamer->GetStatus(cancelled) == IStreamer::INVALID,
            "notification callback close did not invalidate stream");
    wav[20] = 2; wav[21] = 4;
    wav.resize(40); // Truncated IAC must fail even with an available decoder.
    std::ofstream unsupported(root / "iac.wav", std::ios::binary);
    unsupported.write(reinterpret_cast<const char *>(wav.data()), static_cast<std::streamsize>(wav.size()));
    unsupported.close();
    Require(!streamer->Open("iac.wav", source, 0), "truncated IAC falsely succeeded");
#if defined(FRONTIERX_HAVE_IAC_DECODER)
    SDL_AudioSpec rejectedSpec{SDL_AUDIO_U8, 2, 1234};
    std::vector<Uint8> rejectedSamples{1, 2, 3};
    Require(!FrontierX::DecodeNativeIacWave(wav, rejectedSpec, rejectedSamples) &&
            rejectedSamples == std::vector<Uint8>{1, 2, 3} && rejectedSpec.freq == 1234,
            "failed IAC decode mutated caller output");
#endif
    if (const char * archive = std::getenv("FRONTIERX_STREAMER_ARCHIVE"))
    {
        IFileSystem * external = nullptr;
        Require(FrontierX::OpenNativeUtfFileSystem(archive, &external, error), error.c_str());
        const char * entry = std::getenv("FRONTIERX_STREAMER_ENTRY");
        Require(entry && *entry, "external audio requires an explicit entry");
        Uint64 durationMs = 0;
#if defined(FRONTIERX_HAVE_IAC_DECODER)
        WIN32_FIND_DATA found{};
        HANDLE search = external->FindFirstFile(entry, &found);
        Require(search != INVALID_HANDLE_VALUE && external->FindClose(search), "external entry lookup failed");
        DAFILEDESC waveEntry(found.cFileName);
        HANDLE input = external->OpenChild(&waveEntry);
        Require(input != INVALID_HANDLE_VALUE, "external entry open failed");
        const DWORD size = external->GetFileSize(input);
        Require(size <= 64U * 1024U * 1024U, "external wave too large");
        std::vector<Uint8> compressed(size);
        DWORD read = 0;
        Require(external->ReadFile(input, compressed.data(), size, &read, nullptr) && read == size &&
                external->CloseHandle(input), "external wave read failed");
        SDL_AudioSpec decodedSpec{};
        std::vector<Uint8> decodedSamples;
        Require(FrontierX::DecodeNativeIacWave(compressed, decodedSpec, decodedSamples), "IAC decode failed");
        durationMs = 1000ULL * decodedSamples.size() /
            (sizeof(float) * static_cast<Uint64>(decodedSpec.channels) *
             static_cast<Uint64>(decodedSpec.freq));
        std::uint64_t hash = 14695981039346656037ULL;
        for (Uint8 byte : decodedSamples) { hash ^= byte; hash *= 1099511628211ULL; }
        std::cout << "IAC PCM: frequency " << decodedSpec.freq << "; channels " << decodedSpec.channels
                  << "; bytes " << decodedSamples.size() << "; FNV1a64 " << hash << '\n';
#endif
        const Uint64 speechStart = SDL_GetTicks();
        HSTREAM speech = streamer->Open(entry, external, STRMFL_PLAY);
        external->Release();
        Require(speech != nullptr, "external WAV failed to decode/open");
        if (durationMs > 45000)
        {
            const Uint64 limit = SDL_GetTicks() + 150;
            while (SDL_GetTicks() < limit)
            { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
            Require(streamer->GetStatus(speech) == IStreamer::PLAYING &&
                    streamer->Stop(speech) && streamer->CloseHandle(speech),
                    "external long WAV did not play and stop");
            std::cout << "External long WAV played and stopped through native IStreamer; elapsed-ms "
                      << SDL_GetTicks() - speechStart << '\n';
        }
        else
        {
            const Uint64 limit = SDL_GetTicks() + 45000;
            while (streamer->GetStatus(speech) != IStreamer::COMPLETED && SDL_GetTicks() < limit)
            { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
            Require(streamer->GetStatus(speech) == IStreamer::COMPLETED && streamer->CloseHandle(speech),
                    "external WAV did not complete");
            std::cout << "External WAV playback completed through native IStreamer; elapsed-ms "
                      << SDL_GetTicks() - speechStart << '\n';
        }
    }
    source->Release();
    streamer->Release();
    aggregate->Release();
    messages.clear();
    std::cout << "Native IStreamer: decoded WAV, deferred init, pause/resume, completion, loop and stale handles passed\n";
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

void CheckControlledInput()
{
    FrontierX::NativeComponentManager manager;
    FrontierX::RegisterNativeWindowManager(&manager);
    setenv("FRONTIERX_M3_CONTROLLED_INPUT", "1", 1);
    setenv("FRONTIERX_M3_TRACE", "generated-test-access", 1);
    AGGDESC descriptor("IWindowManager", "WindowManager");
    void *raw = nullptr;
    Require(manager.CreateInstance(&descriptor, &raw) == GR_OK && raw,
            "controlled input component creation failed");
    unsetenv("FRONTIERX_M3_CONTROLLED_INPUT");
    unsetenv("FRONTIERX_M3_TRACE");
    auto *aggregate = static_cast<IAggregateComponent *>(raw);
    IWindowManager *window = nullptr;
    Require(aggregate->QueryInterface(IID_IWindowManager,
            reinterpret_cast<void **>(&window)) == GR_OK && window,
            "controlled input window query failed");
    Require(window->Startup(nullptr, "controlled input", ExitHandler,
            WMF_ALLOW_WINDOW_RESIZE), "controlled input window startup failed");
    Require(window->SetWindowPos(400, 300, WMF_ALLOW_WINDOW_RESIZE),
            "controlled input window dimensions failed");
    window->SetCallback(WindowCallback);
    window->ServeMessageQueue(WMF_BACKGROUND);
    messages.clear();
    const auto id = static_cast<Uint32>(window->GetWindowHandle());
    SDL_Event event{};
    event.window.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    event.window.windowID = id;
    Push(event);
    event = {};
    event.motion.type = SDL_EVENT_MOUSE_MOTION;
    event.motion.windowID = id;
    event.motion.x = 1; event.motion.y = 1;
    Push(event);
    event = {};
    event.key.type = SDL_EVENT_KEY_DOWN;
    event.key.windowID = id;
    event.key.key = SDLK_ESCAPE;
    Push(event);
    window->ServeMessageQueue(WMF_BACKGROUND);
    S32 x = 0, y = 0;
    window->GetCursorPos(x, y);
    Require(window->AppIsActive() && x == 200 && y == 150 && messages.empty(),
            "host focus/key/motion contaminated the explicit input fixture");
    // Completion callbacks must still reach the real source window pump.
    CheckStreamer(manager, window);
    messages.clear();
    event = {};
    event.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
    event.window.windowID = id;
    Push(event);
    window->ServeMessageQueue(0);
    Require(messages.size() == 1 && messages.front().message == FrontierX::LegacyMessage::Close,
            "controlled input swallowed an external close instead of failing the run");
    window->Release();
    aggregate->Release();
    Require(manager.ShutDown() == GR_OK, "controlled input component shutdown failed");
}
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

        // A normal background-window close must return to the modal owner,
        // without waiting for or consuming a later focus event.
        event.window.type = SDL_EVENT_WINDOW_FOCUS_LOST;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(window->AppIsActive() == FALSE, "close countercheck did not lose focus");
        const std::size_t inactive_before_close = messages.size();
        close_callback.veto = false;
        event.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        Push(event);
        event.window.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        Push(event);
        window->ServeMessageQueue(0);
        Require(window->AppIsActive() == FALSE,
            "inactive accepted close waited for a later focus event");
        window->ServeMessageQueue(0);
        Require(window->AppIsActive() == FALSE,
            "a second pump waited for focus after accepted close");
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(window->AppIsActive() == TRUE, "queued focus event was lost");
        Require(close_callback.calls == 2 &&
            messages[inactive_before_close].message == FrontierX::LegacyMessage::Close,
            "accepted close did not reach the window callback");

        event = {};
        event.quit.type = SDL_EVENT_QUIT;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(exit_calls == 1, "quit event did not invoke the configured exit callback");

        CheckStreamer(manager, window);

        Require(close_point->Unadvise(close_token) == GR_OK,
            "close callback unsubscribe failed");
        close_point->Release();
        system->Release();
        window->Release();
        aggregate->Release();
        Require(manager.ShutDown() == GR_OK && manager.RegisteredCount() == 0,
            "native window manager shutdown failed");
        CheckControlledInput();

        std::cout << "native SDL IWindowManager component passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "native window-manager probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
