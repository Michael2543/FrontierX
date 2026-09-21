# Native Startup Shell

Status: integrated startup/window lifecycle plus explicit repository render-smoke path implemented,
2026-09-20.

## Scope

`frontierx_native_shell` is the first native Linux executable that owns a complete, bounded startup
and window/event lifecycle rather than testing each service in isolation. It is intentionally named
and labelled as a shell: it does not construct the game, load a campaign or advance simulation. An
explicit option can create the reusable SDL GPU owner and render one repository-owned scene; that
path is an integration check, not game rendering.

The shell composes existing production and portable boundaries without adding SDL or Linux calls to
game logic:

- the shared `StartupDataRoot` owner validates optional native configuration, selects command-line,
  environment, configuration, or `Data` fallback, and checks the exact-case required directory set;
- the shared `StartupPreferences` owner selects an explicit or XDG-based settings path, loads the
  bounded container and applies player/global/default precedence transactionally;
- the real `IUserDefaults` interface is created through a native DACOM factory, exposes mapped live
  defaults, and persists the fixed-width schema before dependency-ordered teardown;
- `NativeComponentManager` registers static production `EventSystem` and native window-manager
  fallbacks, then applies an optional explicit `[Libraries]` list whose equal-priority factories
  can override those fallbacks;
- the public process-global `DACOM_Acquire` entry point owns that manager and registers both
  production profile-parser factories before shell-specific services;
- the shell initializes the native shared heap before static or configured component creation and
  exports host symbols needed by adapted ELF components;
- the real aggregate and `IEventSystem` interfaces are created and initialized;
- an optional explicit source hotkey table is opened through the native read-only UTF
  `IFileSystem`; the production hotkey service is connected from raw EventSystem input back to its
  queued symbol output;
- the real `IWindowManager` aggregate owns SDL video/window, text-input and callback-pump lifetime;
- an optional `SdlGpuRenderer` owns device, window claim, pipeline, GPU resources, presentation,
  bounded readback and teardown while a registered native `IRenderPrimitive` receives the shared
  scene from production `PrimitiveBuilder`;
- the shell callback forwards compatibility records synchronously to `IEventSystem` without a
  parallel direct SDL dispatch path; and
- the real EventSystem system owner advances once after each window batch; and
- `PlatformTime` supplies a one-millisecond idle wait and bounded smoke-test deadline.

Interface owners release `IWindowManager` before its aggregate, which closes SDL before the event
interfaces are released, and the component manager shuts its factory registrations down last.
Normal close and global quit observations leave the loop without calling `exit` from the event
handler.

## Command line

```text
frontierx_native_shell [--config FILE] [--data-root DIRECTORY] [--preferences FILE]
                       [--hotkeys FILE]
                       [--render-smoke VERTEX_SPV FRAGMENT_SPV]
                       [--smoke-ms MILLISECONDS [--hidden]]
```

Data-root selection is command line, `FRONTIERX_DATA_ROOT`, explicit native configuration, then
`Data` relative to the working directory. Configuration uses `[FrontierX]` and `DataRoot=path`;
relative values resolve against the configuration file's directory. An explicitly supplied invalid
configuration fails even when a higher-priority source supplies a path. The same implementation is
used by `frontierx_headless`. An optional `[Libraries]` section is passed to the native component
manager after the static fallback factories are registered. Missing sections are allowed; present entries use
the exact config-relative native module policy documented in
[DACOM_MANAGER_PORTABILITY.md](DACOM_MANAGER_PORTABILITY.md). Component failures are reported
without printing the configured path. The shell never prints the resolved path. `--smoke-ms` accepts 1 through 60000 and exits
after that duration if no close event arrives. `--hidden` is accepted only with that bounded mode;
the ordinary invocation opens a visible 800 by 600 window and runs until close or quit.

`--render-smoke` is available when `FRONTIERX_BUILD_SDL3_GPU` is enabled. It requires explicit
vertex and fragment SPIR-V paths, initializes `SdlGpuRenderer` against the window owned by
`IWindowManager`, obtains native `IRenderPrimitive` from the process component manager and submits
the shared triangle-and-quad fixture through `PrimitiveBuilder`. The shell presents until
close/deadline, performs a bounded 64-by-64 structural readback, reports counts and a diagnostic
checksum, and tears the renderer down before the window. It never loads a retail asset. Missing or
invalid shaders and GPU/presentation/readback failures return exit code 7. The shell does not request
legacy centering because Wayland compositors own top-level placement.

`--hotkeys FILE` is an explicit bring-up input, not an installed-resource search policy. The file
must be a validated UTF 1.01 container with the production hotkey children. Successful startup
reports only that the pipeline is connected and never prints the path. Failure returns exit code 6
with a path-redacted diagnostic. Omitting the option preserves shell startup without a hotkey
instance.

Preference selection is an explicit `--preferences FILE`, then an absolute `XDG_CONFIG_HOME`, then
an absolute `HOME` plus `.config/frontierx/preferences.dat`. A missing default file is valid
first-run state; a missing or malformed explicit file fails. Diagnostics report only the selection
source, load status, entry count and player/global/default settings source, never the selected path
or player name. The shell keeps this owner and its live `IUserDefaults` component alive for the
complete lifecycle. Clean shutdown creates a missing parent directory and persists the fixed-width
schema; it never writes the compiler-owned `USER_DEFAULTS` bytes.

Exit codes are:

| Code | Meaning |
|---:|---|
| 0 | Help or a clean close/smoke lifecycle |
| 1 | Invalid command line |
| 2 | Invalid configuration or inaccessible/incompatible required data-directory structure |
| 3 | Native component configuration, event-system creation or initialization failure |
| 4 | Native window-manager/SDL runtime failure or unexpected exception |
| 5 | Unusable preference path, container or native user-settings schema |
| 6 | Invalid hotkey UTF or hotkey/EventSystem pipeline initialization failure |
| 7 | Render-smoke shader, GPU, submission, presentation or readback failure |

General diagnostic UTF inspection remains exclusive to `frontierx_headless`. The shell uses only
the shared bounded reader behind the explicit hotkey adapter and does not claim to inspect or
certify a retail installation. Configuration, data-root selection and preferences use shared
owners rather than duplicated command-line policy.

## Automated evidence

`Tools/portability/test_native_shell.py` creates only empty synthetic data directories and uses the
checked-in source `Hotkeys.utf` for the explicit hotkey case. With
`SDL_VIDEODRIVER=dummy`, it verifies:

- command-line data-root selection, hidden window creation, bounded pumping and clean shutdown;
- environment/configuration data-root selection, precedence, relative paths, invalid configuration,
  inaccessible roots and incompatible required directory reporting;
- successful empty `[Libraries]` application plus a redacted missing-module failure;
- configured loading and clean teardown of the production EventSystem ELF module while the static
  fallback remains registered, with only the two module C entry points globally visible;
- default first-run and explicit preference selection, an explicit valid container, and redacted
  missing/malformed preference failures, plus a persisted `FXSET001` shutdown schema;
- native `IWindowManager` creation, startup and bounded queue service in every successful process;
- production hotkey/UTF creation, EventSystem connection, per-loop update, redacted missing-table
  failure and dependency-ordered disconnection;
- help output, option dependencies, numeric bounds, missing values and unknown options;
- render-smoke argument validation and path-redacted missing-shader failure before GPU startup; and
- redacted output that does not disclose the temporary absolute data path.

GCC 16.2.1 and Clang 22.1.8 compile the executable with strict warnings and pass the process test.
The focused `frontierx_native_window_manager_probe` separately verifies the public DACOM interface,
system update interface, repeat and duplicate startup rules, size/area and cursor state,
wait/background event service, focus transitions, reverse close veto, accepted close, quit callback
and aggregate teardown with the SDL dummy driver.
A separate Clang AddressSanitizer/UndefinedBehaviorSanitizer build passes the process suite with
leak detection disabled because process tracing prevents LeakSanitizer in this environment. That
run includes simultaneous static and dynamic EventSystem owners plus the source hotkey UTF path. It
found the original duplicate RTTI boundary; the real hotkey table later exposed an overlapping-copy
defect, and both now pass with the explicit module symbol contract and overlap-safe compaction.

Reproduce the registered test from the repository root:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_native_shell
ctest --test-dir out/build/linux-gcc-debug -R portability.native_shell --output-on-failure

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_native_shell
ctest --test-dir out/build/linux-clang-debug -R portability.native_shell --output-on-failure
```

On 2026-09-20, separate non-sandboxed visible executions used the GCC and Clang preset binaries,
the same generated shaders, a synthetic empty directory tree and `--smoke-ms 250`. Both reported
`video driver: wayland`, `backend: vulkan`, 17 presented frames, nine vertices, 2,496 changed pixels
and checksum `3246152458680859569`. A focused Clang AddressSanitizer/UndefinedBehaviorSanitizer build
executed the same integrated shell path with the same pixel result; leak detection was disabled in
the host's graphics-driver/`ptrace` environment. This is synthetic Linux runtime evidence, not a
Windows or retail run.

## Remaining gates

This is compile and synthetic process evidence, not a native game runtime. It does not prove retail
data compatibility merely because empty required directories exist. The next integration work must:

- select the tested native `IUserDefaults` service from the future game-startup owner and add the
  file-picker, document/viewer, installation-metadata and EULA owners needed by UI callers;
- extract retail-metadata inspection only when a real startup consumer requires it, keeping schema
  validation distinct from constructing the game;
- adapt and connect additional real engine services through configured native modules in dependency
  order;
- establish game application update/render cadence and focus/background behavior without changing
  simulation order or timing;
- connect representative legacy render/material/blend state through existing renderer ownership;
- exercise a lawful retail installation without recording its files or private path; and
- pass native Windows Debug/Final builds plus affected ABI and behavior comparisons.

No retail media, campaign module, game renderer, audio, video, network service, save data or
simulation path is opened by the shell. The optional renderer consumes only the checked-in smoke
fixture and build-generated shaders.
