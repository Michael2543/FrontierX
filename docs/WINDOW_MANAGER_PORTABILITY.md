# Window-manager portability boundary

Status date: 2026-09-18

## Scope and evidence

The public `IWindowManager` contract in `Code/Libs/Include/WindowManager.h` now has a
standard-C++ Linux declaration path. `FrontierX::RegisterNativeWindowManager` registers a native
DACOM factory whose instances own the SDL window and message-pump lifetime. The native shell now
creates this component through `NativeComponentManager`; it no longer owns a parallel `SdlWindow`
or `SdlEventPump`.

The existing implementations in `Code/App/Src/WindowManager.cpp` and
`Code/Libs/Src/System/WindowManager.cpp` remain the supported Win32 implementations. The native
owner is a separate platform implementation and does not add SDL calls to game logic.

The GCC and Clang `frontierx_dacom_interface_probe` targets include the real public header and
verify that:

- `IWindowManager` still derives from `IDAComponent`;
- a native window token, `WPARAM`, and `LPARAM` are pointer-width on Linux x86_64;
- `LONG` and `UINT` remain explicit 32-bit compatibility scalars; and
- the callback declarations use the existing portable DACOM calling-convention macros.

`frontierx_native_window_manager_probe` additionally creates the real factory through the native
manager and exercises the component under `SDL_VIDEODRIVER=dummy`. The focused target executes
successfully in the GCC and Clang presets. The source contract is also checked by
`test_platform_inventory.py`. These checks do not establish MSVC ABI parity; native Windows Debug
and Final rebuilds remain required.

A focused Clang ASan/UBSan run initially found that the shared `DAComponentAggregate` template
deleted an allocated aggregate through its smaller base-component type. The inner owner now retains
the actual aggregate wrapper type, so final `Release` runs the complete destructor and matching
sized delete. The corrected window-manager probe and the five native-shell process scenarios pass
under both sanitizers with leak detection disabled for the process test environment.

## Type policy

On Windows, `Windows.h` continues to own `HINSTANCE`, `HWND`, `LONG`, `LRESULT`, `HANDLE`, `UINT`,
`WPARAM`, and `LPARAM`. The header does not replace or redefine those platform types.

On non-Windows builds, the compatibility declaration uses:

| Public name | Native declaration | Reason |
|---|---|---|
| `HINSTANCE` | `void *` | Process-local startup token; never serialized |
| `HWND` | `FrontierX::LegacyWindowToken` | Pointer-width process-local window identity |
| `LONG` | `std::int32_t` | Preserve the legacy scalar width on LP64 |
| `HRESULT` | `std::int32_t` | Preserve the legacy result width |
| `LRESULT` | `std::intptr_t` | Preserve pointer-width callback results |
| `HANDLE` | `void *` | Opaque process-local handle |
| `UINT` | `std::uint32_t` | Preserve the legacy message-ID width |
| `WPARAM` | `FrontierX::LegacyWParam` | Pointer-width callback parameter |
| `LPARAM` | `FrontierX::LegacyLParam` | Signed pointer-width callback parameter |

These aliases are runtime compatibility values, not disk, save, map, campaign, or network fields.
They must not be written into persistent data. In particular, the change does not globally widen
`U32`, `S32`, object IDs, offsets, or connection handles.

`WMEXITCB` and `WNDPROC` now use `DACOM_CDECL` and `COMAPI`. Those macros expand to the existing
MSVC calling conventions on Windows and to empty standard-C++ decoration on the current native
toolchains. This follows the same policy as the portable DACOM, profile, and connection callback
headers.

## Relationship to the SDL slice

`FrontierX::SdlWindow` owns SDL video/window lifetime and returns neutral `WindowEvent` values.
`NativeWindowManager` owns one `SdlWindow`, retains one `SdlLegacyInputAdapter` across pump batches,
and exposes only the existing DACOM interfaces. The separate `SdlEventPump` remains as an isolated
adapter/integration probe; the native shell no longer uses it.

The native component currently implements:

- `Startup` with mandatory name/exit-handler validation, hidden initial creation, text-input
  activation, repeat-call success and deterministic `WMF_PREVENT_MULTIPLE` rejection;
- resize, resizable, fullscreen, always-on-top, show, and center requests through SDL;
- a pointer-width process-local window token derived from the SDL window ID;
- `ServeMessageQueue` polling plus the `WMF_WAIT` and `WMF_BACKGROUND` branches;
- ordered callback delivery through the installed `WNDPROC`, rather than a second direct send to
  `IEventSystem`;
- focus state, cursor cache/warp, client and decorated window-area queries;
- the outgoing `ISystemEventCallback` connection point and reverse-order close veto; and
- quit delivery through the mandatory exit callback and aggregate-owned teardown.

`WMF_DLGMSG` is accepted but currently has no additional behavior because no native dialog-message
filter exists. The probe treats the SDL window ID as an opaque process-local token; it is not a
Win32 `HWND`, an SDL pointer, or a persistent value.

The shell installs a narrow synchronous `WNDPROC` adapter. It sends each translated window record
through the production `IEventSystem`; for `WM_CHAR` it first emits the corresponding immediate
Unicode value, matching the source-reviewed Windows callback ordering. Temporary records remain
valid only for the synchronous `Send` call and are never posted. This selects one dispatch route and
avoids the duplicate delivery that would result from also running `SdlEventPump`.

## Acceptance gates

The window-manager slice is not complete until the remaining gates are demonstrated:

- dummy-driver tests extend beyond the current startup, size, cursor, focus, wait/background,
  close-veto, quit, multiple-instance and teardown coverage to fullscreen transitions, show state,
  decorated-area differences and failure injection;
- a visible Wayland or X11 smoke test verifies responsive resize and close behavior;
- application-loop placement is compared against `System.cpp` and does not change update order;
- `WMF_DLGMSG` is either implemented for a concrete native dialog consumer or its unsupported
  behavior is made explicit at the consumer boundary;
- native Windows Debug and Final builds confirm that the conditional declaration did not alter the
  supported interface ABI; and
- focus, repeat, text input, mouse capture, double-click, close-veto, area, and cursor behavior are
  compared with the supported Windows runtime.

Renderer integration remains separate. A valid native window token and lifecycle do not establish
a swapchain, D3D9 translation path, or a rendered game frame.
