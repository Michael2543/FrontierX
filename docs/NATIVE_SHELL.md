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
- `NativeComponentManager` hosts the production DAHOTKEY `EventSystem` static factory;
- the real aggregate and `IEventSystem` interfaces are created and initialized;
- `SdlWindow` owns SDL video/window and text-input lifetime;
- `SdlEventPump` drains neutral events and synchronously sends compatibility records; and
- `PlatformTime` supplies a one-millisecond idle wait and bounded smoke-test deadline.

Interface owners release `IEventSystem` before its aggregate, the SDL window closes before those
interfaces are released, and the component manager shuts its factory registrations down last.
Normal close and global quit observations leave the loop without calling `exit` from the event
handler.

## Command line

```text
frontierx_native_shell [--config FILE] [--data-root DIRECTORY]
                       [--smoke-ms MILLISECONDS [--hidden]]
```

Data-root selection is command line, `FRONTIERX_DATA_ROOT`, explicit native configuration, then
`Data` relative to the working directory. Configuration uses `[FrontierX]` and `DataRoot=path`;
relative values resolve against the configuration file's directory. An explicitly supplied invalid
configuration fails even when a higher-priority source supplies a path. The same implementation is
used by `frontierx_headless`. The shell never prints the resolved path. `--smoke-ms` accepts 1 through 60000 and exits
after that duration if no close event arrives. `--hidden` is accepted only with that bounded mode;
the ordinary invocation opens a visible 800 by 600 window and runs until close or quit.

Exit codes are:

| Code | Meaning |
|---:|---|
| 0 | Help or a clean close/smoke lifecycle |
| 1 | Invalid command line |
| 2 | Invalid configuration or inaccessible/incompatible required data-directory structure |
| 3 | Native component/event-system creation or initialization failure |
| 4 | SDL window/runtime failure or unexpected exception |

This interface remains deliberately smaller than `frontierx_headless`: preferences and UTF
inspection stay there until each policy has a reusable owner rather than duplicated command-line
logic. Configuration and data-root selection are now shared.

## Automated evidence

`Tools/portability/test_native_shell.py` creates only empty synthetic directories. With
`SDL_VIDEODRIVER=dummy`, it verifies:

- command-line data-root selection, hidden window creation, bounded pumping and clean shutdown;
- environment/configuration data-root selection, precedence, relative paths, invalid configuration,
  inaccessible roots and incompatible required directory reporting;
- help output, option dependencies, numeric bounds, missing values and unknown options; and
- redacted output that does not disclose the temporary absolute data path.

GCC 16.2.1 and Clang 22.1.8 compile the executable with strict warnings and pass the process test.
A separate Clang AddressSanitizer/UndefinedBehaviorSanitizer build passes all three scenarios with
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

- move shared preferences and metadata startup policy out of the headless executable so the shell
  can use the same validated owners without duplicating behavior;
- connect additional real engine services through the component manager in dependency order;
- establish application update/render cadence and focus/background behavior without changing
  simulation order or timing;
- introduce a renderer surface only after the native D3D9/Vulkan direction is prototyped and
  approved;
- exercise a lawful retail installation without recording its files or private path; and
- pass native Windows Debug/Final builds plus affected ABI and behavior comparisons.

No retail media, campaign module, renderer, audio, video, network service, save data or simulation
path is opened by the shell.
