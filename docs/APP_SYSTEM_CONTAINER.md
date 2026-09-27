# App system-container ownership on Linux

Status: 2026-09-27, production App container compiled and used by the native
shell for its EventSystem and SDL-backed `IWindowManager` through the real
`LoadSystemComponents` profile traversal. This is synthetic
startup/runtime evidence. It is not a native `Conquest.cpp` entry point or
retail game startup.

## Production owner and integration boundary

The supported Windows `Conquest.cpp` calls `RegisterContainerFactory()` from
`Code/App/Src/SysContainer.cpp`, then `GameSystem::startup` creates an
`ISystemContainer`. `GameSystem::startup` calls `LoadSystemComponents`, which
reads the `[System]` profile and adds the App `CQPipeline` wrapper. The
similarly named `Code/Libs/Src/System/SysContainer.cpp` is a separate owner;
compiling it would not exercise the game's actual factory. The App source is
therefore the native target, with its public `ISystemContainer`,
`IAggregateComponent`, `ISystemComponent` and connection-point interfaces
unchanged. This class is runtime-only and does not cross a serialized or
campaign module ABI layout boundary; its public interfaces still require
native Windows module verification.

The prior native shell manually created and initialized EventSystem and
WindowManager aggregates. Its successful scene draw could not prove that
the App startup container could aggregate those services, forward their
interfaces, update them in order and release them at shutdown. The falsifiable
local check is that the existing shell process suite and visible game-scene
run still start, pump events and shut down after the factory and ownership
move; failure would keep the real App container outside native startup.

`frontierx_app_system_container_owner` now compiles the actual App source in
GCC and Clang builds. The native shell registers its factory through the
shared DACOM manager, then supplies synthetic `[System]` entries for the
production `IEventSystem` and `IWindowManager` to the existing
`IProfileParser2` owner. `LoadSystemComponents` creates and initializes
these aggregates. The shell queries their interfaces through the container and
uses the container's `ISystemComponent::Update` during its event loop. The
App DACOM global stays valid until after container teardown. The native
manager now recognizes the loader's unversioned `IProfileParser` query
alongside the versioned IID; without that alias, the loader found no parser
and the shell could not query the event service. The native pipeline is
already a direct shell composition, so the Windows-only
`CreateCQPipeline` insertion in `LoadSystemComponents` is excluded from the
Linux object. The shell executes the App loader with synthetic profile entries;
it does not read a retail `[System]` profile. The GPU-enabled shell now calls
`GameSystem::startup` around this container; its separate evidence is in
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).

The container's previously implicit heap-zero assumptions are explicit:
its list, tail, aggregate and loaded fields, and each element's system and
aggregate pointers start null. Removing a failed final aggregate updates the
tail. `AddComponent` now copies its `const AGGDESC` input before supplying
the container's outer/inner pointers; callers' descriptors are never written
through a const pointer. A profile line without `=` now has a null description
instead of an indeterminate pointer. Exact-case includes let both Linux compilers read the
owner. The supported Windows branch retains `pch.h`, App globals and the
`CQPipeline` creation call. Its Debug/Final rebuild remains unperformed.

The native scene now calls `RemoveNativeSystemComponent` for the temporary
service aggregate before its borrowed pipeline and primitive expire. This
native-only helper identifies the exact
inner component added by `AddComponent`, unlinks it, updates the tail and
calls `ShutdownAggregate` when present. Profile-loaded event and window
components cannot be removed through this helper. The public
`ISystemContainer` vtable and Windows implementation are unchanged. The
production `ITManager`, `IMaterialManager` and `TextureLibrary` are now
separately added to and owned by the App container. The pipeline and primitive
were scene-local GPU owners before profile primitive aggregation was added.

## Validation

Commands executed on the Linux x86_64 host:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_app_system_container_owner -j 4
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j 4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.native_shell$' --output-on-failure
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_app_system_container_owner -j 4
cmake --build --preset linux-clang-debug --target frontierx_native_shell -j 4
ctest --test-dir out/build/linux-clang-debug -R '^portability.native_shell$' --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
cmake -S . -B /tmp/frontierx-shell-no-gpu -G Ninja -DFRONTIERX_BUILD_SDL3=ON -DFRONTIERX_BUILD_SDL3_GPU=OFF
cmake --build /tmp/frontierx-shell-no-gpu --target frontierx_native_shell -j 4
ctest --test-dir /tmp/frontierx-shell-no-gpu -R '^portability.native_shell$' --output-on-failure
cmake -S . -B /tmp/frontierx-texture-handle-asan
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell -j 4
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.native_shell$' --output-on-failure
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/game_material_gpu_fixture.utf.vertexcolor.mesh out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 1500
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings /tmp/frontierx-texture-handle-asan/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf.vertexcolor.mesh /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 5000
```

GCC 16.2.1 and Clang 22.1.8 built the App object and shell, and their full
Debug presets passed. Each full preset executed 40 checks and skipped two GPU
checks before video initialization in the command sandbox. The focused shell
process suite passed with both compilers, including dynamic EventSystem,
hotkey, preference, window and teardown cases. A fresh GCC SDL3 build with
GPU disabled also built and passed that suite. Its shader error assertion
now checks the build-specific unavailable-GPU result; GPU builds continue
to require the exact missing-shader failure. The non-GPU shell also opened
and closed a bounded SDL dummy window. Focused Clang ASan/UBSan shell process
execution passed with LeakSanitizer disabled on this host.

The visible Wayland/Vulkan GCC CQ2 shell runs presented 72 frames after the
container move and 70 frames after the descriptor-copy change, with 496 changed pixels and checksum
`1466864583390317354`. Focused Clang ASan/UBSan visible runs presented 98
frames and, after the descriptor-copy change, 97 frames with that checksum
and no reported sanitizer finding.
After the synthetic profile loader replaced direct shell insertion, the visible
GCC run presented 73 frames with the same 496 changed pixels and checksum.
Its production `LoadSystemComponents` notices reported successful creation of
both `IEventSystem` and `IWindowManager`. GCC and Clang full Debug builds and
CTest presets passed again (40 executing checks, two sandbox GPU skips), as did
the fresh no-GPU and focused ASan/UBSan shell process suites.
The visible focused Clang ASan/UBSan CQ2 run through the profile loader
presented 98 frames with the same checksum and no reported finding;
LeakSanitizer was disabled.
Inputs were repository-generated synthetic UTF material/mesh/texture data,
compiled repository shaders and empty temporary data directories. The
command-sandbox GPU skips are not counted as GPU execution.

The retail App `[System]` profile, `CQPipeline` wrapper,
native `Conquest.cpp` startup, game-level `IUserDefaults` selection,
application cadence, Windows Debug/Final and module ABI checks, lawfully
external retail data and same-input Windows D3D9 game-scene comparison remain
open. The observed native factory warning about the legacy DACOM factory
base's nonvirtual destructor and the `TempStr` literal signature are still
present; this step did not change those shared contracts.

With targeted removal, GCC 16.2.1 and Clang 22.1.8 rebuilt the App owner
and shell, passed the focused shell suite and full Debug CTest presets
(40 executing checks and two command-sandbox GPU skips per preset). The
visible GCC Wayland/Vulkan scene presented 72 frames, 496 changed pixels
and checksum `1466864583390317354`. Focused Clang ASan/UBSan presented 98
frames with the same image result and no reported sanitizer finding;
LeakSanitizer was disabled. The ASan/UBSan shell process suite also passed.
These synthetic runs exercise normal removal and teardown; they do not
exercise an injected failed removal or Windows module ABI.

The next step used native `AddNativeSystemComponent` to return a retained
inner identity for the production `TextureLibrary` while the App container
holds its own reference. The library initialized by querying the pipeline
through the container and was removed after Engine teardown, before scene GPU
resources. GCC and Clang full Debug builds and CTest presets passed again
(40 executed checks, two command-sandbox GPU skips each). A visible GCC
Wayland/Vulkan run presented 74 frames, 496 changed pixels and checksum
`1466864583390317354`; a focused Clang ASan/UBSan run presented 100 frames
with the same image result and no reported sanitizer finding, with
LeakSanitizer disabled. The focused sanitizer shell suite passed. These
synthetic repository fixtures do not establish retail or Windows behavior.

The production game `IMaterialManager` and `ITManager` were then added by
their registered factories and removed in reverse order after the Engine.
The App container holds all three game texture/material owners; their
game-specific initialization still receives a generated archive and the
scene's pipeline explicitly. A visible GCC Wayland/Vulkan run presented 71
frames, 496 changed pixels and checksum `1466864583390317354`.
The focused Clang ASan/UBSan run presented 100 frames with the same image
result and no reported sanitizer finding, with LeakSanitizer disabled.
The final container identity check additionally verifies that querying
`IID_ITManager` returns the same production manager used by the scene.
Visible GCC and focused Clang ASan/UBSan runs presented 72 and 99 frames,
respectively, with unchanged pixels and checksum; no sanitizer finding was
reported with LeakSanitizer disabled.

The shell now accepts a bounded external `--system-profile` file. This
exposed an App owner failure contract: `LoadSystemComponents` previously
returned `GR_OK` on Linux when the parser or `[System]` section was absent,
or when a component factory or aggregate initializer failed. The native
branch now propagates those failures to `CreateGameSystem`; the container is
released before reporting startup failure. Windows behavior is unchanged.
Native component-creation notices now report status without echoing user-supplied
profile names or descriptions. The adjacent production Engine loader also
rejects failed native `[Engine]` creation; see
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).

The existing shell process suite generated a valid external profile and
separate missing-section, unknown-component, missing-file and oversized-file
cases. The valid profile opened and closed an SDL dummy window; bad system
inputs returned code 3 before window startup, and temporary private paths
were absent from output. GCC 16.2.1 and Clang 22.1.8 built their complete
Linux Debug presets and passed their focused shell and full CTest presets
(40 executing checks, two command-sandbox GPU skips each). The no-GPU GCC
and focused Clang ASan/UBSan shell process suites passed. A visible GCC
Wayland/Vulkan synthetic scene presented 72 frames, 496 changed pixels and
checksum `1466864583390317354`. No retail profile, Windows Debug/Final or
module ABI check was executed for this change.
The same generated external profile also reached a visible 72-frame scene
with that checksum; no private profile path appeared in its process output.
Both full Linux Debug CTest presets passed again (40 executing checks and
two command-sandbox GPU skips per preset), as did the focused sanitizer shell
process suite.

The App container can now also own a production-profile `IRenderPrimitive`
aggregate. The shell registers the existing native primitive's aggregate
factory before `GameSystem::startup`; the aggregate remains dormant until the
scene creates its SDL GPU and binds callbacks. It unbinds after Engine
teardown, before GPU-context destruction. The generated three-service profile
passes the existing shell process suite under GCC, Clang and focused Clang
ASan/UBSan; visible GCC Wayland/Vulkan output retains 496 changed pixels and
checksum `1466864583390317354`. The checked-in `Conquest.ini` then advances
to its third system entry, `IRenderPipeline`, where startup still fails.
Inputs were generated or checked-in source configuration, not retail media.
Windows Debug/Final and ABI checks remain unperformed. The complete owner
and command record is in [GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).

The same profile path now also creates an unbound `IRenderPipeline` aggregate
in the App container. The scene binds its existing primitive and GPU
callbacks after window/GPU initialization and clears them after Engine
teardown. The generated four-service profile passes the GPU shell process
suite under GCC and Clang and the focused Clang ASan/UBSan shell suite.
Visible GCC Wayland/Vulkan and focused Clang ASan/UBSan scenes retained 496
changed pixels and checksum `1466864583390317354`; the sanitizer scene
presented one frame with LeakSanitizer disabled and no reported finding.
The checked-in `Conquest.ini` now reaches its fifth `[System]` entry,
`IViewConstructor`, then returns startup code 3. No retail media, Windows
Debug/Final or ABI validation was used; see
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).
