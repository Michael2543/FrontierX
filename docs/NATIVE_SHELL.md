# Native Startup Shell

Status: first integrated lifecycle implemented, 2026-09-18.

## Scope

`frontierx_native_shell` is the first native Linux executable that owns a complete, bounded startup
and window/event lifecycle rather than testing each service in isolation. It is intentionally named
and labelled as a shell: it does not construct the game, load a campaign, create a renderer, advance
simulation, or render a frame.

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
- the real aggregate and `IEventSystem` interfaces are created and initialized;
- the real `IWindowManager` aggregate owns SDL video/window, text-input and callback-pump lifetime;
- the shell callback forwards compatibility records synchronously to `IEventSystem` without a
  parallel direct SDL dispatch path; and
- `PlatformTime` supplies a one-millisecond idle wait and bounded smoke-test deadline.

Interface owners release `IWindowManager` before its aggregate, which closes SDL before the event
interfaces are released, and the component manager shuts its factory registrations down last.
Normal close and global quit observations leave the loop without calling `exit` from the event
handler.

## Command line

```text
frontierx_native_shell [--config FILE] [--data-root DIRECTORY] [--preferences FILE]
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

UTF inspection remains deliberately exclusive to `frontierx_headless`: the native shell does not
claim to inspect retail metadata. Configuration, data-root selection and preferences now use shared
owners rather than duplicated command-line policy.

## Automated evidence

`Tools/portability/test_native_shell.py` creates only empty synthetic directories. With
`SDL_VIDEODRIVER=dummy`, it verifies:

- command-line data-root selection, hidden window creation, bounded pumping and clean shutdown;
- environment/configuration data-root selection, precedence, relative paths, invalid configuration,
  inaccessible roots and incompatible required directory reporting;
- successful empty `[Libraries]` application plus a redacted missing-module failure;
- configured loading and clean teardown of the production EventSystem ELF module while the static
  fallback remains registered;
- default first-run and explicit preference selection, an explicit valid container, and redacted
  missing/malformed preference failures, plus a persisted `FXSET001` shutdown schema;
- native `IWindowManager` creation, startup and bounded queue service in every successful process;
- help output, option dependencies, numeric bounds, missing values and unknown options; and
- redacted output that does not disclose the temporary absolute data path.

GCC 16.2.1 and Clang 22.1.8 compile the executable with strict warnings and pass the process test.
The focused `frontierx_native_window_manager_probe` separately verifies the public DACOM interface,
system update interface, repeat and duplicate startup rules, size/area and cursor state,
wait/background event service, focus transitions, reverse close veto, accepted close, quit callback
and aggregate teardown with the SDL dummy driver.
A separate Clang AddressSanitizer/UndefinedBehaviorSanitizer build passes the process suite with
leak detection disabled because process tracing prevents LeakSanitizer in this environment.

Reproduce the registered test from the repository root:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_native_shell
ctest --test-dir out/build/linux-gcc-debug -R portability.native_shell --output-on-failure

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_native_shell
ctest --test-dir out/build/linux-clang-debug -R portability.native_shell --output-on-failure
```

## Remaining gates

This is compile and synthetic process evidence, not a native game runtime. It does not prove retail
data compatibility merely because empty required directories exist. The next integration work must:

- select the tested native `IUserDefaults` service from the future game-startup owner and add the
  file-picker, document/viewer, installation-metadata and EULA owners needed by UI callers;
- extract retail-metadata inspection only when a real startup consumer requires it, keeping schema
  validation distinct from constructing the game;
- adapt and connect additional real engine services through configured native modules in dependency
  order;
- establish application update/render cadence and focus/background behavior without changing
  simulation order or timing;
- introduce a renderer surface only after the native D3D9/Vulkan direction is prototyped and
  approved;
- exercise a lawful retail installation without recording its files or private path; and
- pass native Windows Debug/Final builds plus affected ABI and behavior comparisons.

No retail media, campaign module, renderer, audio, video, network service, save data or simulation
path is opened by the shell.
