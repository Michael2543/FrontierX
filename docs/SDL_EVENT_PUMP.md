# Native SDL Event Pump

Status: implemented as an isolated integration slice, 2026-09-18.

## Purpose and owner boundary

`FrontierX::SdlEventPump` is the first concrete native owner that drains the existing `SdlWindow`
event source, retains the stateful `SdlLegacyInputAdapter`, and delivers every generated compatibility
record through the production `IEventSystem`. It exists in the separate
`frontierx_sdl3_event_pump` target so the SDL-only platform library remains independent of DACOM.

This boundary has two source-reviewed consumers: the legacy `IWindowManager::ServeMessageQueue`
queue-service role and the `System.cpp::wndProc` path that sends focus, keyboard, character and mouse
messages to `IEventSystem`. The public `IWindowManager` declaration and a separate SDL-backed DACOM
owner now have tested native paths. This isolated pump remains useful for direct adapter/event-system
tests, but the native shell now uses `IWindowManager` and does not run both dispatch routes. The
remaining parity requirements are tracked in
[WINDOW_MANAGER_PORTABILITY.md](WINDOW_MANAGER_PORTABILITY.md).

## Dispatch contract

One `Pump` call repeatedly invokes `SdlWindow::PollEvent` until the SDL queue contains no further
translated event for the owned window. `SdlWindow` discards unsupported events and events carrying a
different nonzero window ID. Global quit events retain window token zero.

For each accepted neutral event, the pump:

1. records close and global-quit observations in the returned batch result;
2. translates through its persistent `SdlLegacyInputAdapter`, retaining modifier and mouse-button
   state across pump calls; and
3. calls `IEventSystem::Send` synchronously for each compatibility record in translation order.

The pump deliberately does not use `Post`. A translated window-message parameter points into a
temporary owning record held by the adapter only for the duration of `Send`; queueing that pointer
would create a use-after-lifetime defect. Event subscribers must copy any values they need after the
callback. Immediate Unicode values retain the existing integer-in-pointer callback convention and
are never dereferenced or serialized.

The returned `event_count` counts accepted neutral events, not generated compatibility records.
`close_requested` reports an owned-window close request; `quit_requested` reports SDL's global quit
request. The pump reports these observations but does not destroy the window or terminate a process,
leaving shutdown ordering with the future application owner.

## Automated evidence

The existing `frontierx_legacy_input_probe` now opens a hidden SDL window with the dummy video
driver, creates the production DAHOTKEY `EventSystem` through `NativeComponentManager`, and attaches
a real DACOM `IEventCallback`. It injects:

- a resize for a foreign window, which must be filtered;
- an owned-window resize;
- UTF-8 text containing U+00C5; and
- a global quit event.

The probe verifies that one pump call accepts three neutral events and synchronously produces, in
order, resize, immediate Unicode, legacy character and close records. It copies callback parameters
inside `Notify`, thereby checking the intended temporary lifetime. GCC and Clang compile and pass the
focused test with strict warnings under `SDL_VIDEODRIVER=dummy`. A separate Clang build of the pump,
SDL platform, production event owner, native manager and probe passes AddressSanitizer plus
UndefinedBehaviorSanitizer with leak detection disabled because this environment traces processes.

Reproduce the focused checks from the repository root:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_legacy_input_probe
ctest --test-dir out/build/linux-gcc-debug -R portability.legacy_input --output-on-failure

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_legacy_input_probe
ctest --test-dir out/build/linux-clang-debug -R portability.legacy_input --output-on-failure
```

## Evidence limits and remaining gates

This is synthetic process-level evidence. It does not establish a native game loop, frame cadence,
renderer surface, retail input parity or gameplay behavior. The next application integration must:

- define which owner opens and closes `SdlWindow` and when text input is active;
- place each pump relative to simulation and rendering without changing update order or timing;
- translate shutdown observations into the existing exit callback/lifecycle contract;
- connect the implemented `IWindowManager` cadence to the real application loop without exposing
  SDL or Linux calls to game logic;
- compare focus, key repeat, text composition, mouse capture and double-click behavior against the
  supported Windows runtime; and
- pass native Windows Debug/Final rebuilds and affected ABI checks.

No retail files, renderer, audio, video, networking or simulation code are exercised by this probe.
