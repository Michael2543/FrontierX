# Native Startup Shell

Status: 2026-09-27 integrated startup/window lifecycle, explicit production-batcher
render smoke, and repeated production Engine/RenderMgr/PolyMesh scenes from
supplied legacy or CQ2 UTF meshes. The CQ2 branch also executes App
`MeshRender::Render` and the source-pinned `VertexColor.fx` material.

The optional `--material-root DIRECTORY` supplies the required CQ2 `Mat`
source independently of external retail archives. It must contain readable
`shaders`, `materials` and `textures` directory views; for source data use
`--material-root Data/mat`. It neither disables media nor starts a mission.
The `--game-scene` material argument also accepts a physical directory as
well as UTF. See [the material-source validation](NATIVE_SINGLEPLAYER_SLICE.md#explicit-material-directory-source-2026-09-27).

`--skip-movies` explicitly uses the App's existing `bSkipMovies` guard and
omits only `IVideoSystem` from native profile creation. Default startup still
requires the service. Audio and Talking-Head animation remain required. This
is the documented policy for the training checkpoint; see
[the movie-policy evidence](NATIVE_SINGLEPLAYER_SLICE.md#explicit-full-screen-movie-policy-2026-09-27).

## Scope

`frontierx_native_shell` is the first native Linux executable that owns a complete, bounded startup
and window/event lifecycle rather than testing each service in isolation. It is intentionally named
and labelled as a shell: it does not load a campaign or advance simulation. An
explicit option creates the reusable SDL GPU owner and renders one repository-owned
fixture. A separate explicit option composes production game mesh owners and renders
one supplied UTF mesh, including the App material caller for the supported CQ2
effect. The scene remains visible while the shell pumps events; it does not
advance game simulation or start a campaign.

The shell composes existing production and portable boundaries without adding SDL or Linux calls to
game logic:

- the shared `StartupDataRoot` owner validates optional native configuration, selects command-line,
  environment, configuration, or `Data` fallback, and checks the exact-case required directory set;
- the shared `StartupPreferences` owner selects an explicit or XDG-based settings path, loads the
  bounded container and applies player/global/default precedence transactionally;
- the real `IUserDefaults` interface is created through a native DACOM factory and
  executes `LoadDefaults` before `GameSystem::startup`, matching the supported
  application's owner ordering; it exposes mapped live defaults and persists
  the fixed-width schema before dependency-ordered teardown;
- `NativeComponentManager` registers static production `EventSystem` and native window-manager
  fallbacks, then applies an optional explicit `[Libraries]` list whose equal-priority factories
  can override those fallbacks;
- the public process-global `DACOM_Acquire` entry point owns that manager and registers both
  production profile-parser factories before shell-specific services;
- the shell initializes the native shared heap before static or configured component creation and
  exports host symbols needed by adapted ELF components;
- the production App `ISystemContainer` loads EventSystem and native WindowManager
  aggregates from a synthetic `[System]` profile, initializes them and forwards
  their interfaces; see
  [APP_SYSTEM_CONTAINER.md](APP_SYSTEM_CONTAINER.md);
- the GPU-enabled shell starts and shuts down the production `GameSystem` with
  those services and an empty synthetic `[Engine]` section, then loads the
  fixture renderer into that same Engine; the App container owns the game
  texture/material aggregates and can own profile-created primitive and pipeline aggregates that bind after
  GPU initialization; see
  [GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md);
- an optional explicit source hotkey table is opened through the native read-only UTF
  `IFileSystem`; the production hotkey service is connected from raw EventSystem input back to its
  queued symbol output;
- the real `IWindowManager` aggregate owns SDL video/window, text-input and callback-pump lifetime;
- an optional `SdlGpuRenderer` owns device, window claim, pipeline, GPU resources, presentation,
  bounded readback and teardown while production `PrimitiveBuilder` submits the shared scene through
  the App-side `RenderBatcher`, real native `IRenderPipeline` vtable and registered
  `IRenderPrimitive`;
- the shell callback forwards compatibility records synchronously to `IEventSystem` without a
  parallel direct SDL dispatch path; and
- the App container updates its system components once after each window batch; and
- `PlatformTime` supplies a one-millisecond idle wait and bounded smoke-test deadline.

Interface owners release borrowed window and event interfaces before the App
container, which shuts down its aggregates before the component manager's
factory registrations.
Normal close and global quit observations leave the loop without calling `exit` from the event
handler.

## Command line

```text
frontierx_native_shell [--config FILE] [--system-profile FILE]
                       [--data-root DIRECTORY] [--preferences FILE]
                       [--hotkeys FILE]
                       [--render-smoke VERTEX_SPV FRAGMENT_SPV]
                       [--game-scene MATERIAL_UTF MESH_UTF VERTEX_SPV FRAGMENT_SPV
                                     EXTENDED_SPV GAME_SPV]
                       [--capture-ppm FILE]
                       [--animation-object OBJECT_NAME SCRIPT_NAME]
                       [--skip-movies]
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

`--system-profile FILE` replaces the shell's built-in synthetic `[System]`
and empty `[Engine]` text before the production App container and Engine
loaders run. A supplied profile is read as a bounded file of at most one
megabyte. Missing, empty, oversized or unreadable input fails with exit code
3 and a path-redacted diagnostic. On Linux, the production loaders now
return failure for a missing `[System]` section or an uncreatable system or
Engine component; no opened window is reported. A supplied retail profile
does not load its `[Libraries]` DLL list; native modules remain governed by
`--config` and registered native factories. It will still need its referenced
native components and lawful media, so this
option alone is not evidence of game startup.
With the explicit movie skip and lawful external data, the checked-in
`Conquest.ini` now completes `GameSystem::startup`; the App mission caller
remains outside this shell.

`--animation-object OBJECT_NAME SCRIPT_NAME` is a bounded diagnostic for
the production object-animation path. After a GPU-enabled Engine profile
starts, it opens one child of the mounted `Objects` source, asks the real
`IAnimation`/`IChannel2` owners to load its script set, and requires the named
script to have a finite nonnegative duration. The option does not create an
Engine instance or play the animation. Exit 6 indicates an unavailable
object, failed channel/script load or missing script. On the lawful external
Steam data root, `ssforger.cmp Sc_build` loaded with duration 1.6 seconds
under GCC and Clang; see [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-channel-and-forger-animation-data-2026-09-27).

`--render-smoke` is available when `FRONTIERX_BUILD_SDL3_GPU` is enabled. It requires explicit
vertex and fragment SPIR-V paths, initializes `SdlGpuRenderer` against the window owned by
`IWindowManager`, obtains native `IRenderPrimitive` from the process component manager, wraps it in
the implemented `IRenderPipeline` adapter, registers and initializes the production App-side
`RenderBatcher` aggregate, and submits the shared triangle-and-quad fixture first through the
production primitive constructor of `PrimitiveBuilder`, then as seven unique RPVertex values and
nine U16 indices through the same batcher/pipeline boundary. It takes a bounded direct readback
before the indexed upload and compares it with the indexed readback after presentation. The indexed
submission and the following alpha-depth queued-pool submission have exactly matching 64-by-64
readback checksums and changed-pixel counts. The pooled submission is the image presented by the
shell. The shell presents until
close/deadline, performs a bounded 64-by-64 structural readback, reports counts and a diagnostic
checksum, and tears the renderer down before the window. It never loads a retail asset. Missing or
invalid shaders and GPU/presentation/readback failures return exit code 7. The shell does not request
legacy centering because Wayland compositors own top-level placement.

`--game-scene` is a separate, visible scene path. It reads the supplied
material and mesh UTF files without copying them, loads four compiled SPIR-V
shaders, registers the production math, game material/texture, texture-library,
Engine, RenderMgr and PolyMesh factories, and composes those owners through a
real Engine profile. The game's `IMaterialManager` and `ITManager` initialize
against the supplied material UTF and native pipeline; `TextureLibrary` loads
the mesh's embedded RGB565 frame in the legacy branch. The production Engine
creates a mesh archetype and instance. A legacy mesh updates the Engine and
calls `render_instance` with a bounded camera; the native primitive forwards
its indexed `MTVERTEX` draw into SDL/Vulkan. A CQ2 material mesh creates the
App `MeshRender` and borrowed `MeshInfoTree`, resolves the production
`IMaterial`, and calls `MeshRender::Render` with the same native pipeline and
primitive. The source-pinned `VertexColor.fx` pass applies its TGA texture
and vertex constants before the 68-byte App vertex draw. The shell captures
and presents each CQ2 frame inside the submission callback, before App state
cleanup. Both branches require a live texture and nonempty visible geometry.
They keep the Engine instance, archetype, material, pipeline and GPU alive
while the shell services window messages and EventSystem updates. The legacy
branch repeats `render_instance` without advancing the Engine clock; the CQ2
branch repeats `MeshRender::Render`. Each bounded 64-by-64 readback must match
the first frame's checksum, and a failed redraw exits with code 7. On close or
`--smoke-ms` expiry, the owners release before the window. `--capture-ppm`
writes the last successful bounded image. `--hidden` remains unavailable for
this scene. The duration begins before material/GPU initialization, so a
short smoke deadline may yield only one presented frame. This path accepts
only the native-supported formats; it does not load campaign state, advance
simulation, or demonstrate retail behavior.

The first combined ELF run exposed a real C++ symbol collision: the unrelated
CQ2 material-manager `Material` and legacy mesh `Material` classes had the
same global destructor symbol. The legacy `XMesh` cleanup called the CQ2
destructor and aborted in `free()`. Only the Linux CMake object target now
renames the CQ2 implementation type to `NativeGameMaterial`; its `IMaterial`
vtable contract and the Windows source path are unchanged. The final ELF has
both distinct destructor symbols. The original core was unavailable from
`coredumpctl`; a reproduced GDB stack located the wrong destructor at
`XMesh::release_full_mesh` during `ENGINE::release_archetype`.

The shell currently supplies the batcher's required App globals and fatal diagnostics locally.
Those are composition scaffolding for the repository fixture, not native game services. The
repository texture is bound by a runtime `U32` handle through the batcher and native renderer;
production texture/material-library loading and other pool modes remain open.

`--hotkeys FILE` is an explicit bring-up input, not an installed-resource search policy. The file
must be a validated UTF 1.01 container with the production hotkey children. Successful startup
reports only that the pipeline is connected and never prints the path. Failure returns exit code 6
with a path-redacted diagnostic. Omitting the option preserves shell startup without a hotkey
instance.

Preference selection is an explicit `--preferences FILE`, then an absolute `XDG_CONFIG_HOME`, then
an absolute `HOME` plus `.config/frontierx/preferences.dat`. A missing default file is valid
first-run state; a missing or malformed explicit file fails. Diagnostics report only the selection
source, load status, entry count and player/global/default settings source, never the selected path
or player name. The shell creates its live `IUserDefaults` component and
executes `LoadDefaults` before `GameSystem::startup`, then keeps it alive for
the complete lifecycle. Clean shutdown creates a missing parent directory and persists the fixed-width
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
| 6 | Invalid hotkey UTF or hotkey/EventSystem pipeline initialization failure; animation object/script load failure |
| 7 | Render-smoke or game-scene shader, UTF, GPU, submission, presentation or readback failure |

General diagnostic UTF metadata inspection remains exclusive to `frontierx_headless`. The shell
uses the production `IFileSystem` archive mounts for the explicit animation-object caller and
does not claim to inspect or certify a retail installation. Configuration, data-root selection
and preferences use shared
owners rather than duplicated command-line policy.

## One-frame production game mesh evidence (2026-09-27)

The integration blocker was that the visible production PolyMesh draw existed
only in a linked test executable, while the shell initialized two shaders for
its repository smoke fixture and never composed game mesh services. The real
caller is `IEngine::render_instance` through production `RenderMgr` and
`PolyMesh`; the previous test could establish its rendering contract but not
shell startup, resource ownership or teardown. The falsifiable check is that
the shell loads the generated legacy mesh UTF and material UTF through its
window and component manager, presents an indexed textured draw, then exits
without a destructor mismatch or sanitizer finding. If it fails, the game
composition remains confined to the test executable.

On this Linux x86_64 Wayland/Vulkan host, GCC 16.2.1, Clang 22.1.8 and a
focused Clang ASan/UBSan build each presented 496 changed pixels with RGB
checksum `11826028615312451232`. GCC and Clang shell P6 captures were
byte-identical to one another and to the prior production PolyMesh probe:
SHA-256 `bed238d526e7f3ae36cd5c55a1a6fdea20bcf1a955553e7e4925be00acba1bfc`,
12,301 bytes including the P6 header. Inputs were repository-generated UTF
material/mesh files, the repository RGB565 texture fixture and compiled
repository GLSL; captures stayed outside the repository. The Clang fixture
had initially been overwritten by a CPU-only check without the embedded
texture; rerunning its visible GPU fixture generator restored the intended
input before the successful shell comparison. A missing `ILightManager` notice
is emitted by production PolyMesh, but this bounded legacy material draw
does not use that service.

Commands executed from the repository root (the generated fixture files are
created by each compiler's `portability.game_material_gpu` check):

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell frontierx_game_material_manager_probe -j2
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' --output-on-failure
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/legacy_game_mesh_fixture.utf out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-ppm /tmp/frontierx-shell-game-scene.ppm --smoke-ms 1500
cmake --build --preset linux-clang-debug --target frontierx_native_shell frontierx_game_material_manager_probe -j2
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_gpu$' --output-on-failure
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-clang-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-clang-debug/game_material_gpu_fixture.utf out/build/linux-clang-debug/legacy_game_mesh_fixture.utf out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-ppm /tmp/frontierx-shell-clang-game-scene.ppm --smoke-ms 1500
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' --output-on-failure
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings /tmp/frontierx-texture-handle-asan/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf /tmp/frontierx-texture-handle-asan/legacy_game_mesh_fixture.utf /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 5000
ctest --test-dir out/build/linux-gcc-debug -R '^portability.native_shell$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.native_shell$' --output-on-failure
```

At this earlier gate, the native-shell process check passed under GCC and Clang, including invalid
`--game-scene` option forms. The focused ASan/UBSan scene passed with
LeakSanitizer disabled for this host. Neither a native Windows Debug/Final
rebuild nor module ABI verification ran. No retail mesh, campaign, gameplay
simulation or SDL event-driven game loop was executed. A same-input Windows
D3D9 game-scene capture and Windows/Linux comparison remain Milestone 5 gates.

## CQ2 App material in the native shell (2026-09-27)

The next blocker was that the shell's production Engine/RenderMgr/PolyMesh
composition could present a legacy mesh, while the source-pinned CQ2
`VertexColor.fx` material and real `MeshRender::Render` caller executed only
in the linked integration executable. The shell now inspects the production
mesh's `imaterial_list` after instance creation. For a CQ2 material mesh it
sets the native App globals to the same Engine, renderer, pipeline and
primitive, creates the production `MeshRender` and borrowed `MeshInfoTree`,
and calls the real App renderer. The native primitive's 68-byte vertex
callback takes the bounded image and presents before `MeshRender` resets
texture and stage state. The shell requires a submitted App draw, successful
presentation, live `ITManager` texture handle, a changed image when that
texture is unbound, applied `c30`/`c31` vertex constants, and balanced App
vertex-buffer registration. A mesh whose material names unsupported
`Synthetic.fx` must fail the scene rather than report a stale GPU frame.
This extends an existing production caller; it does not implement a parallel
mesh renderer.

Visible GCC 16.2.1 and Clang 22.1.8 Wayland/Vulkan runs with the generated
`VertexColor.txt`, repository `VertexColor.fx` source, generated TGA and CQ2
UTF mesh each presented 496 changed pixels with RGB checksum
`1466864583390317354`. Their 64-by-64 P6 files were byte-identical,
SHA-256 `e54ef6c12ff5ca0fd5da724725aae5da4e79f87074f75b6dd9ce26142e3b90d1`.
The shell's image differs from the linked probe's CQ2 App capture because
the startup and draw sequences differ; no cross-sequence parity claim is
made. A visible focused Clang ASan/UBSan shell run produced the same checksum
without a reported finding, with LeakSanitizer disabled. The old legacy
PolyMesh shell branch retained its prior checksum `11826028615312451232`.
The unsupported `Synthetic.fx` mesh exited with code 7 and no success line.
Generated inputs and captures stayed outside the repository.

Commands executed from the repository root after generating each fixture
through the existing visible `portability.game_material_gpu` check:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j2
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/game_material_gpu_fixture.utf.vertexcolor.mesh out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-ppm /tmp/frontierx-shell-cq2-scene.ppm --smoke-ms 1500
cmake --build --preset linux-clang-debug --target frontierx_native_shell -j2
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-clang-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-clang-debug/game_material_gpu_fixture.utf out/build/linux-clang-debug/game_material_gpu_fixture.utf.vertexcolor.mesh out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-ppm /tmp/frontierx-shell-clang-cq2-scene.ppm --smoke-ms 1500
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings /tmp/frontierx-texture-handle-asan/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf.vertexcolor.mesh /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 5000
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/game_material_gpu_fixture.utf.mesh out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 1500
```

The full GCC/Clang Linux debug builds and CTest presets passed after the
shell link change; both GPU checks skipped before video initialization in
the command sandbox. The visible commands above supply runtime evidence;
the complete preset counts are in [BUILD_GRAPH.md](BUILD_GRAPH.md).
Native Windows Debug/Final rebuilds and
module ABI checks, a same-input Windows D3D9 effect capture, retail-runtime
behavior, other D3DX9 effects, multi-material scenes, simulation and a
playable game loop remain unperformed. Milestone 5 is still open.

## Repeated scene lifetime (2026-09-27)

The one-frame shell composition released the production Engine, material,
pipeline and GPU immediately after presentation. It could not establish that
the App or legacy mesh owner remains valid while window events continue or
that a second production draw has the same material state. The shell now
passes its existing `IWindowManager::ServeMessageQueue` and EventSystem
`Update` cadence into the scene owner. While those owners remain alive, each
legacy frame calls `IEngine::render_instance` and each CQ2 frame calls the
actual `MeshRender::Render`. Both paths capture and present every frame; the
scene rejects a changed 64-by-64 checksum. The CQ2 callback additionally
checks the production texture's visual influence on each frame. The finite
`--smoke-ms` deadline allows a repeatable visible run; with no deadline the
window remains open until close. All mesh/material objects tear down after
the loop and before the SDL window.

On this Linux x86_64 Wayland/Vulkan host, GCC 16.2.1 presented 73 legacy
frames over a 1500 ms deadline, each retaining the original 496 changed
pixels and checksum `11826028615312451232`. GCC presented 69 CQ2 App frames
at the same deadline, each retaining 496 changed pixels and checksum
`1466864583390317354`; Clang 22.1.8 presented 74 CQ2 frames with the same
checksum. A visible Clang ASan/UBSan build presented 104 CQ2 frames over
5000 ms without a reported sanitizer finding; a separate instrumented legacy
run presented 98 frames with its original checksum. LeakSanitizer was disabled for
this host. The shorter 1500 ms sanitizer run finished startup at the deadline
and presented only the initial frame, so it is not counted as repeated-frame
evidence. The GCC/Clang complete Debug builds passed; each full CTest preset
executed 40 checks and skipped two GPU checks in the command sandbox. The
separate visible game-material GPU test regenerated the textured legacy input
and passed. All scene inputs were generated from repository fixtures and
source; no retail data was used.

Representative commands from the repository root after the existing fixture
generator has run:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' --output-on-failure
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/legacy_game_mesh_fixture.utf out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 1500
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/game_material_gpu_fixture.utf.vertexcolor.mesh out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 1500
cmake --build --preset linux-clang-debug --target frontierx_native_shell
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-clang-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene out/build/linux-clang-debug/game_material_gpu_fixture.utf out/build/linux-clang-debug/game_material_gpu_fixture.utf.vertexcolor.mesh out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 1500
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings /tmp/frontierx-texture-handle-asan/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf.vertexcolor.mesh /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 5000
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings /tmp/frontierx-texture-handle-asan/frontierx_native_shell --data-root /tmp/frontierx-capture-data --game-scene /tmp/frontierx-texture-handle-asan/game_material_gpu_fixture.utf /tmp/frontierx-texture-handle-asan/legacy_game_mesh_fixture.utf /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 5000
ctest --preset linux-gcc-debug --output-on-failure
ctest --preset linux-clang-debug --output-on-failure
```

This is repeated synthetic rendering and event pumping, not a game update
loop: no campaign state, player input response, simulation clock advance,
retail mesh or hardware/Windows comparison is established. Native Windows
Debug/Final and module ABI checks, same-input D3D9 comparison, broader D3DX9
effects and a playable native game scene remain Milestone 5 gates.

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
- live `IUserDefaults::LoadDefaults` before native `GameSystem::startup` in the
  bounded shell process, with the ordering asserted from its process report;
- generated supplied system-profile success, missing `[System]`, unknown
  system/Engine components, missing file and oversized input, with no private
  test path in process output;
- native `IWindowManager` creation, startup and bounded queue service in every successful process;
- production hotkey/UTF creation, EventSystem connection, per-loop update, redacted missing-table
  failure and dependency-ordered disconnection;
- help output, option dependencies, numeric bounds, missing values and unknown options;
- render-smoke and game-scene argument validation, plus path-redacted missing-shader failure before GPU startup; and
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

On 2026-09-21, separate non-sandboxed visible executions used the GCC and Clang preset binaries,
the same generated shaders, a synthetic empty directory tree and `--smoke-ms 250`. Both reported
`video driver: wayland`, `backend: vulkan`, 17 presented frames, nine vertices, 2,496 changed pixels
and checksum `7951318278861886233` after configuring depth plus source-alpha blending through
`IRenderPipeline` and the native `IRenderPrimitive` state callback. Those earlier shells identified
their source as `production PrimitiveBuilder via IRenderPipeline`. Focused
AddressSanitizer/UndefinedBehaviorSanitizer CPU checks cover pipeline inheritance, production-builder
dispatch, state validation and rollback.

On 2026-09-23, visible GCC, Clang and focused Clang ASan/UBSan shell runs included the production
App-side `RenderBatcher` direct branch and reported the same nine vertices, 2,496 changed pixels
and checksum `7951318278861886233`. The latter run used
`ASAN_OPTIONS=detect_leaks=0` because LeakSanitizer cannot run reliably under host ptrace. Exact
commands, fixture provenance and the absent queued/texture/material evidence are in
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md). These are synthetic Linux runtime
results, not Windows or retail runs.

A later 2026-09-23 visible run of each of those three builds submitted the same fixture in indexed
form through the production batcher after the direct draw. The shell required matching structural
readbacks and exact local checksum equality between both forms in each process, and verified seven uploaded
vertices and nine U16 indices before presentation. Each run reported Wayland/Vulkan, 17 presented
frames, seven vertices, 2,496 changed pixels and the same local checksum
`7951318278861886233`. This proves indexed GPU presentation through the batcher for synthetic
fixture data. It does not establish queued pool batching or texture-stage/material mapping; the
SDL renderer still receives the fixture texture directly. Exact commands and the separate
ASan/UBSan and sandbox-skip limits are recorded in
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

The subsequent 2026-09-23 shell run bound the original and an additional repository-owned RGBA8
texture by runtime `U32` handles through the production batcher's texture-stage setter. The extra
texture changed the bounded readback; rebinding the original recovered checksum
`7951318278861886233` before the indexed draw. Visible GCC, Clang and focused Clang ASan/UBSan
Wayland/Vulkan shell runs passed, each with 17 presentations and clean shutdown. The fixture
bytes still originate in the shell; no `TextureManager` or material owner was executed. Exact
commands and handle-lifetime checks are in
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

The 2026-09-25 shell change enables `RPR_BATCH` with the native primitive's supported
texture-stage snapshot, sets one material ID and identity modelview, submits the same
U16-indexed fixture into the production pool, and flushes it before readback. The immediate and
pooled images agree exactly in visible GCC, Clang and focused Clang ASan/UBSan Wayland/Vulkan runs:
2,496 changed pixels, checksum `7951318278861886233`, 17 presented frames. A separate linked CPU
check verifies that the draw is deferred until flush across two batcher lifecycles. Commands and
limits are in [RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

## Profile-owned lighting and texture service (2026-09-27)

The GPU shell registers the production `LightManager` and math factories.
For a supplied seven-service `[System]` profile, its game-scene path reuses
the App container's profile-created `IRenderPrimitive`,
`IRenderPipeline` and `TextureLibrary` instead of creating a second
texture library. With the repository-generated older-format mesh, the
GameSystem Engine calls production RenderMgr/PolyMesh, which queries
`ILightManager` from the container and submits smooth-shaded vertices.
The linked CPU integration check observes different vertex colors at two
ambient settings; the visible Linux result has 496 changed pixels and
checksum `3422507570980372643` under GCC and Clang. Without the
LightManager service, the same generated textured fixture and shader
produce checksum `11826028615312451232`. The full profile also presents
the CQ2 App fixture with checksum `1466864583390317354` under both
compilers. A focused Clang ASan/UBSan full-profile legacy run produced the
same lighted checksum without a reported finding, with LeakSanitizer
disabled. These are synthetic Linux runtime results, not retail execution
or Windows lighting parity.

The GPU shell also registers the production `VertexBufferManager` with
the App container's required aggregate factory. A generated eight-service
profile adds it after LightManager. The scene initializes it against the
bound profile pipeline and cleans it up before pipeline unbinding. Visible
GCC, Clang and focused Clang ASan/UBSan older-format scenes retained 496
changed pixels and checksum `3422507570980372643`; the full App
buffer-acquisition path remains unexecuted in this scene. With only
`IStreamer` omitted, the checked-in source profile at that gate passed eight
service entries and stops at `IStringTable`.

The later 2026-09-27 StringTable step links the production owner and
registers its aggregate factory in the GPU shell. A generated nine-service
profile completes synthetic `GameSystem::startup` and opens the dummy SDL
window in GCC and Clang process tests. A diagnostic copy of the source
profile with only `IStreamer` omitted now passes nine entries and stops at
`IVideoSystem`. The unmodified profile still stops at `IStreamer`.

The legacy mesh fixture at `out/build/<preset>/legacy_game_mesh_fixture.utf`
is rewritten by both the CPU and GPU material checks. Run the visible
`portability.game_material_gpu` check immediately before a manual
legacy shell run to obtain its embedded RGB565 texture. A full CTest
run afterward replaces it with an untextured version; the shell now
reports that specific missing texture bind. The detailed ownership,
commands and remaining gates are in
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).

## Remaining gates

This is compile and synthetic process evidence, not a native game runtime. It does not prove retail
data compatibility merely because empty required directories exist. The next integration work must:

- connect the tested native `IUserDefaults` service to the future
  `Conquest.cpp` game-startup owner and add the file-picker, document/viewer,
  installation-metadata and EULA owners needed by UI callers;
- extract retail-metadata inspection only when a real startup consumer requires it, keeping schema
  validation distinct from constructing the game;
- adapt and connect additional real engine services through configured native modules in dependency
  order;
- establish game application update/render cadence and focus/background behavior without changing
  simulation order or timing;
- replace the shell-local batcher globals and diagnostics with production native owners;
- extend production `RenderBatcher` pool/flush behavior beyond one alpha-depth indexed draw, and connect
  material loading/state beyond the representative depth/blend, stage-zero combine and texture-handle binding now routed
  through existing renderer ownership;
- exercise a lawful retail installation without recording its files or private path; and
- pass native Windows Debug/Final builds plus affected ABI and behavior comparisons.

The shell opens no retail media, campaign module, audio, video, network
service or save data. Its game-scene mode executes production Engine,
RenderMgr, PolyMesh and App mesh callers on generated UTF inputs; it does
not yet run the `Conquest.cpp` simulation or retail game scene.

### Packed data-source ownership (2026-09-27)

The existing `--data-root` path now accepts validated UTF archives for known
resource sources as well as exact-case physical directories. The shell
retains archive-backed `IFileSystem` views under canonical resource names
through GameSystem teardown. It does not yet assign them to the App globals.
The external retail root still fails because the current CQ2 App requires a
missing `Mat` source; no missing required source is silently skipped.
Generated packed-map startup, GCC/Clang full checks and focused sanitizer
results are in [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#packed-startup-source-mounts-2026-09-27).
