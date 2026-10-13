# FrontierX Project Charter

Latest M3 evidence (2026-10-13): **NOT ACCEPTED**. Completed protected CI
[run38426508626](https://github.com/Michael2543/frontierX/actions/runs/38426508626)
at `b24115686c0f733f3d22a7ee852f638dd26cd205` fails overall. Its six Linux
CPU scopes and both explicit GPU jobs pass; native observer export, mission,
sanitizer and comparison gates fail. Follow-up
[run38439947717](https://github.com/Michael2543/frontierX/actions/runs/38439947717)
at `b6ca3dd2051703764cbf0144516bce22b6442f16` is still running. Both GPU jobs
pass; native Debug/Final rebuilds and ABI execute successfully, but the job
fails afterward in the launch-helper regression. Windows mission is skipped.
GCC measured mission sanitizers expose an unaligned Planetoid sync store after
the preceding OpAgent correction. Working-tree follow-up is supplementary,
not executed CI proof. The user confirms the separate Steam executable is
unmodified retail; its runtime has not yet been compared. Retail behavior is
the preferred corroborating reference, rather than the restored conversation
failure. Full state/RNG, repeatability and Save/Reload continuation remain open.
See [the current evidence](M3_SIMULATION_COMPARISON.md#retail-reference-and-next-sync-boundary-2026-10-13).

Earlier updates below are dated historical snapshots.

Latest M3 working update (2026-10-13):b241156 CI run38426508626 actually passes
both explicit X11 GPU jobs; its introduced native observer export and mission
sanitizer gates fail. Working corrections retain sound through real completion,
fix the Linux variable-length sync append and expand declared global/world
state at complete physical boundaries. Native comparison fixtures and private
SSH cross-platform plus production RandomGen vector jobs are prepared; their
corrected complete CI execution is pending. Same-build live speech/script and
Save/Reload continuation still differ; M3 remains **NOT ACCEPTED**. The current
[evidence/normalization ledger](M3_SIMULATION_COMPARISON.md#post-physical-state-and-sound-lifetime-follow-up-working-tree-2026-10-13)
supersedes older pending-run summaries below.

Current M3 update (2026-10-13): **NOT ACCEPTED**. Completed protected Actions
[run38386971218](https://github.com/Michael2543/frontierX/actions/runs/38386971218)
atd089a15 passes six Linux CPU build scopes and native Windows Debug/Final/ABI;
GPU/mission/sanitizer/compatibility gates fail. Run38384161068 was cancelled.
Current Linux working-tree follow-up corrects typed Field factory access and
RandomGen's original Win32 default domain, excludes uncontrolled host input
through a narrow fixture, uses the original player move event and expands
semantic script/operation observations. Explicit X11 GPU preparation is moved
after first presentation. Supplementary results do not replace CI; full
physics/RNG coverage, repeatability, equivalent Save/Reload continuation and
retail/visible/audible corroboration remain open. Windows product repair remains
out of scope. See [the current owner/evidence ledger](M3_SIMULATION_COMPARISON.md#completed-d089a15-ci-and-next-owner-corrections-2026-10-13).
Earlier pending-run statements below are historical snapshots.

Current Sol_Train milestone, M2, 2026-10-12: **Linux gameplay complete in
the agreed scope**. Original selector/briefing, all23 training goals, required
HUD/commands/map, pause, early/later F7/F8, actual victory/Continue and repeated
entry/load/close execute under GCC/Clang on lawful external data. GCC full
fresh gameplay and Clang's own early-save continuation/user completion are
separate from latest bounded lifecycle repeats. Current GCC/Clang/ASan end/
restart/reload closes check original hints idle and live mission streams0.
ASan reports no invalid-memory/UBSan error in the observed path; whole LSan
still fails on3156B/7 host NVIDIA/PipeWire/DBus allocations. Explicit media
exclusions, unavailable optional nontraining providers and ordinary Wayland
probe resize remain documented limitations, not success placeholders.

The active composition uses original App/Engine/event/resource/gameplay owners,
the existing native render/font/audio adapters and original command processing.
No replacement training, altered objectives, simulation/RNG order or save
format is introduced. Default font/decoder-free builds compile/check; full
visible M2 uses the optional external font/shared decoder configuration.
M3 comparison/determinism is now in progress, with supplementary native
Windows Debug/Final full rebuilds and same-build campaign ABI execution.
Actions run38368454837 at62662a1 passes Linux GCC/Clang default/optional/
sanitizer jobs and native Windows Debug/Final plus same-build ABI. Host/GPU/
mission and CRT compatibility comparisons fail. The user's restored Windows
skips conversations in all missions; further Windows repairs are out of scope,
and that path is not the Linux behavior specification. Current Linux work adds
original frame/checkpoint test access and corrects native UTF sibling order;
complete semantic/RNG/script/order capture and equivalent Save/Reload continuation
remain open, as do retail/visible/audible checks. See
[M3 comparison evidence](M3_SIMULATION_COMPARISON.md).
Current Actions run38384161068 tests118efae. A Linux-only Camera startup
return-contract fix and opt-in workflow `gpu_host_setup=hyprland-x11` prepare
corrected startup/GPU checks; their runtime/CI proof remains pending. The GPU
setup changes only owned test windows, with no persistent desktop rules.
The next acceptance milestone remains M3 comparison/determinism. Windows/module ABI and actual
retail comparison are explicitly assigned there by the user; no current
Windows or parity pass is inferred. Details: [final M2 evidence](NATIVE_SINGLEPLAYER_SLICE.md#m2-acceptance-in-the-agreed-linux-scope-2026-10-12)
and [current next step](PORT_READINESS.md).

Current original gameplay Hintbox checkpoint, M1, 2026-10-11: normal SDL
mouse hover now reaches original FieldManager/HintResource in Sol_Train.
Separate visible GCC/Clang runs show Cygnus and medium asteroid hints from
original resources, target switching/removal, new hover after source reload,
and clear text/timers/ownership on actual mission close. Original edge-scroll
camera access is enabled without toolbar/hot-rectangle UI. Selection,
movement, training objectives, source text/voices and moved-state save/reload
remain demonstrated. Existing focused lifecycle and Clang ASan/UBSan checks
passed; sanitizer scope excludes full mission execution and leak checking.
M1's selected Linux gameplay acceptance is demonstrated; native Windows
Debug/Final and module-ABI execution are an external open gate, not a success.
Next: execute that gate on the supported host, then M2's original controls
for the next Sol_Train multiple-selection/control-group/command objective.
No full UI, cross-compiler determinism or retail parity is claimed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-gameplay-hintbox-checkpoint-m1-2026-10-11).
Older entries below are historical.

Current UnitComm alert checkpoint, 2026-10-11: original alert history,
colors and expiry now reach the optional native font/render adapter. Explicit
`--mission-alert-smoke` invokes original `MScript::AlertMessage` on the
selected external-data Forger; visible GCC/Clang runs drew its cyan warning
and red Teletype, retained normal movement/objectives and passed source
save/reload. This controlled source call is not a naturally triggered combat
alert. Focused lifecycle and Clang ASan/UBSan checks passed with leak checking
disabled. Map pings remain explicitly excluded by `--skip-sector-map`.
Gameplay Hintbox hover remains unverified (zero draws); next is its real
caller/resource ownership and the needed original UI closure. Windows/ABI,
full mission sanitizers and retail parity remain open. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-unitcomm-alert-rendering-checkpoint-2026-10-11).
Older entries below are historical.

Current status and unit-text checkpoint, 2026-10-11: original StatusBar and
ScrollingText now draw through the optional FreeType adapter during the
selected native mission. UnitComm selection/movement text and voices run
alongside original training speech subtitles. Separate visible GCC/Clang
Wayland/Vulkan runs passed normal input, objectives and moved-state source
save/reload; a GCC capture shows status, unit text and speech together.
Original Hintbox passed synthetic timing/font lifecycle checks, but no
visible gameplay hint executed. Focused Clang ASan/UBSan passed with leak
checking disabled. Next: UnitComm alerts and real gameplay hint callers.
Windows/ABI, font/audio comparison, full mission sanitizers and retail parity
remain open. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-status-and-unit-scrolling-text-checkpoint-2026-10-11).
Older entries below are historical.

Current mission subtitle checkpoint, 2026-10-11: original `Subtitle.cpp`
now owns selected training speech subtitles through EventSystem and the
optional FreeType font/render adapter. Separate visible GCC/Clang
Wayland/Vulkan runs passed multiline subtitle draws, normal selection/movement
and source moved-state save/reload. Training speech is enabled; portraits
and UnitComm voice/text remain explicitly excluded in this combination.
Focused source lifecycle and Clang ASan/UBSan checks passed with leak checking
disabled. Next: original status/hint/scrolling text, including UnitComm text.
Windows/ABI, font/audio comparison, full mission sanitizers and retail parity
remain open. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-mission-speech-subtitles-checkpoint-2026-10-11).
Older entries below are historical.

Current in-window teletype checkpoint, 2026-10-10: original `Teletype.cpp`
now owns selected training objective text, reveal/caret and expiry through
EventSystem and the existing native App render boundary. An optional
FreeType `IFontDrawAgent` uses an explicitly external face and source pixel
height; default builds remain FreeType-free. Separate visible GCC/Clang
Wayland/Vulkan mission runs passed text, normal selection/movement, unit
voices and source save/reload. The original owner lifecycle check also
passed under Clang ASan/UBSan with leak checking disabled. Next: status,
hint/scrolling text and subtitles through their production owners.
Windows font/ABI, full mission sanitizers and retail parity remain open.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-in-window-mission-teletype-checkpoint-2026-10-10).
Older entries below are historical.

Current unit voice checkpoint, 2026-10-10: original UnitComm and StringData
now connect normal selection/movement in the selected external `Sol_Train`
mission to native ISoundManager and production IStreamer. Separate visible
Wayland/Vulkan GCC/Clang optional-FFmpeg Debug runs started two unit voices,
completed one, stopped one and retained no active stream; both preserved
movement, objectives and source mission save/reload and exited zero.
This requires the reviewed external shared-LGPL IAC decoder and explicit
unit-alert/subtitle and mission-SFX exclusions. Next: restore in-window
text/teletype through the original owners. Windows/ABI, full mission
sanitizers, audible comparison and retail parity remain unverified. See
[the focused voice record](NATIVE_SINGLEPLAYER_SLICE.md#original-unit-voice-mission-checkpoint-2026-10-10).
Older entries below are historical.

Current native modal mission checkpoint, 2026-10-10: the selected external
`Sol_Train` mission now enters source `CQDoModal` after startup. Its source
event, frame-time, render, simulation and GameSystem update order runs through
the existing `IRenderPipeline`/SDL GPU backend. A source `CQE_PLAYER_QUIT`
event returns modal result 4; the shell then saves/reloads the moved Forger
and objectives and closes the mission. Separate 15-second visible
Wayland/Vulkan GCC and Clang runs passed with normal mouse selection and
movement. A no-duration run also exited cleanly after a normal window close.
Native menu hot rectangles, CPU framebuffer locking and the explicitly
excluded media/UI paths remain open; Windows, ABI and retail parity are
unverified. See [the focused modal record](NATIVE_SINGLEPLAYER_SLICE.md#source-modal-mission-integration-2026-10-10).
Older status entries below are historical.

Current native frame-boundary checkpoint, 2026-10-10: the selected mission
now uses production `IRenderPipeline` clear/begin/end/swap backed by SDL GPU.
GCC and Clang CPU checks and separate visible X11/Vulkan synthetic GPU
runs passed; visible Wayland mission runs completed selection, movement,
training objectives and moved-state save/reload. Full-target color/depth
clear is supported; partial/stencil operations remain explicit failures.
The default desktop GPU CTest resize failed under Wayland tiling; separate
floating X11 runs passed. Next: link the source `CQDoModal` event/timing/UI
closure, then restore excluded in-game media. Windows and retail parity
remain unverified. See [the focused frame record](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-frame-lifecycle-checkpoint-2026-10-10).
Earlier status entries below are historical.

Current native playable-slice status (2026-10-10): GCC and Clang Linux
x86_64 Debug builds load lawful external `Sol_Train` data, present the
player-owned Forger through the production Engine/SDL/Vulkan path, accept
normal left-click selection and right-click movement through original
offline operation owners, advance source simulation to training objectives
1 and 2, and round-trip the moved unit and objectives through source
mission save/reload. Both visible integrated runs exited zero; full native
CTest trees passed. Required `IStreamer` remains active. Full-screen
`IVideoSystem` and other incomplete optional media/UI paths were excluded
only by explicit flags. This bounded checkpoint is not complete gameplay
or Windows retail parity; Windows Debug/Final, ABI and unit-bearing Windows
mission comparison remain open. See
[the focused acceptance record](NATIVE_SINGLEPLAYER_SLICE.md#playable-solarian-training-checkpoint-2026-10-10).
The status snapshots below are historical.

Native mission-map status (2026-09-27): the selected external
`Sol_Train.dmission` has no saved system-map child. A bounded native
`ISystemMap` owner now executes that fresh-load branch and is linked
into the shell, but the App mission still cannot link or run; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-system-map-fresh-load-gate-2026-09-27).

Native selected-script status (2026-09-27): the original `Sol_T.rc`
now builds into checked native objective/subtitle resources with
`llvm-rc`; the source training program and production `MScript` owner
compile under GCC/Clang. The lawful external mission names `Sol_T.dll`
and has a zero update count, matching the bounded fresh-load contract.
The native shell still does not link or execute the script or mission;
see [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-localization-and-fresh-loader-gate-2026-09-27).
The Linux Debug presets require Python 3 and `llvm-rc` for this
source-resource target in addition to SDL3 and `glslc`.

Native operation-owner status (2026-09-27): production `OpAgent.cpp`
now builds under GCC and Clang with its required event callback and
native millisecond clock contracts, but the App mission link still
lacks player/host and packet services. No mission operation or unit
movement has run; see [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-operation-owner-native-build-gate-2026-09-27).

Native App startup contract status (2026-09-27): the original
`GlobalComponent` registration and cleanup functions now compile once
for Windows `Trim` and native Linux, and link into the GCC/Clang shell.
A focused check executed their order across 30 registrars. The mission
and object owners still do not link into that shell, so no mission has
started; see [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#shared-app-global-startup-and-cleanup-owner-2026-09-27).

Native animation-data status (2026-09-27): the production `IAnimation` and
`IChannel2` owners loaded the external Forger `Sc_build` script through the
mounted Steam `objects` archive under GCC and Clang (one script, 1.6 seconds).
This does not create an object instance or run the mission; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-channel-and-forger-animation-data-2026-09-27).

Native startup status (2026-09-27): the unchanged source `Conquest.ini`
completes production `GameSystem::startup` with source-profile Optics under
GCC and Clang on lawfully
external Steam archives with explicit full-screen movie skip. This is a
bounded SDL dummy-window startup, not App global-owner initialization or
mission gameplay. The next production gate is the post-startup App sequence
in `Conquest.cpp`; see [the focused startup record](NATIVE_SINGLEPLAYER_SLICE.md#production-animation-and-hardpoint-startup-2026-09-27).

Native audio status (2026-09-27): the existing window-manager check now also
executes `IStreamer` PCM playback and notifications. Retail training speech
executes with the optional external shared-LGPL FFmpeg IAC decoder; see [the audio contract](NATIVE_AUDIO_STREAMER.md).
This is not mission execution. For IAC, configure either Linux preset with
`-DFRONTIERX_BUILD_FFMPEG_AUDIO=ON -DCMAKE_PREFIX_PATH=/external/lgpl-prefix`
after building the reviewed dependency as documented there. The default stays
SDL-only and rejects IAC; GPL-enabled FFmpeg libraries are rejected at configure.
Existing build/test commands are unchanged.
The explicit `--skip-movies` option excludes full-screen cinematics for the
training checkpoint through the existing App flag; it preserves audio and
animation requirements. The source profile now completes `GameSystem::startup`.

Status: incremental native bring-up, 2026-09-27. The supported VS2022 Win32 Debug and Final
no-deploy builds were rebuilt on physical Windows 11, including the 18 campaign modules.
Fresh modules passed the original campaign import check. On the physical Windows host, the
restored Final D3D9 path reached the main menu, first Terran campaign briefing and a visible,
scrollable game scene with external installed data and repository-built TextureLibrary. The first
Terran mission currently shows the background and HUD but no running mission or units; the user
separately reported two visible ships in the Celareon campaign. The repository-owned rendering
fixture now also executes and captures through the supported Windows D3D9 path. Its 64-by-64 P6
image passes the documented one-pixel-tolerant comparison with the visible Linux SDL/Vulkan
capture; this is synthetic cross-backend runtime evidence, not retail gameplay parity. See
[WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md).

## Mission

FrontierX will make *Conquest: Frontier Wars* run as a native 64-bit Linux application while
preserving the shipped game's behavior. The intended result is an x86_64 ELF build that does not
require Wine and consumes media from a lawfully obtained retail installation.

This is a port of the original C and C++ engine, not a gameplay rewrite or a new engine inspired by
Conquest. Modernization exists to remove platform dependencies and keep the game maintainable; it
must not silently redesign simulation, content formats, missions, or balance.

## Source Baseline

- Upstream restoration: `https://github.com/Revan67/ConquestFWR`
- Reference branch: `master`
- Bootstrap reference revision: `8051dd2fd54e141000694a4c24e027a086e6a52f`
- Upstream purpose: restore retail behavior on modern Windows before broad modernization

The 2013 source release is a partially modified CQ2 development snapshot, not the exact retail
source. Retail-compatible layouts, object IDs, campaign ABI repairs, and verified runtime fixes in
the restoration repository are part of the baseline and must not be discarded during the port.

## Definition of Success

FrontierX is successful when:

1. The game configures and builds reproducibly on a supported Linux x86_64 toolchain.
2. The produced executables and libraries are native ELF binaries without a Wine runtime.
3. A legitimate retail installation can supply all required media without copying that media into
   this repository or its release artifacts.
4. Campaign loading, gameplay, saves, maps, and other supported retail data remain compatible.
5. Simulation behavior remains deterministic across supported builds.
6. The retail acceptance matrix has Linux evidence comparable to the Windows baseline.

Multiplayer, additional architectures, and other operating systems are later milestones. They must
not broaden the first Linux bring-up.

## Ordered Strategy

1. **Protect the baseline.** Record source provenance, preserve the working Win32 build, retain ABI
   and layout checks, and establish small executable tests where possible.
2. **Build portably.** Introduce CMake and Ninja incrementally alongside the supported Visual Studio
   build. A generated project file is not evidence that a target runs.
3. **Create a platform boundary.** Move windowing, input, timing, configuration, filesystem, dynamic
   loading, threading, and diagnostics behind portable interfaces. Do not spread Linux, X11,
   Wayland, or Win32 calls through game logic.
4. **Bring up one rendering path.** Preserve the current D3D9-facing contracts and connect one real
   production draw caller to the proven native submission surface before selecting a backend.
   Compare a representative scene with Windows and replace unsupported D3DX9 facilities explicitly.
5. **Port content services.** Address audio and video after startup, data loading, input, and basic
   rendering are measurable. Network transport comes after deterministic single-player behavior.
6. **Package last.** Release packaging starts only after license, dependency provenance, retail-data
   separation, and runtime acceptance checks are automated.

Each phase must leave a testable result. Avoid broad mechanical rewrites that mix build-system,
platform, renderer, ABI, and gameplay changes in one step.

## Architecture Direction

| Area | Direction | Current status |
|---|---|---|
| Primary target | Linux x86_64, native ELF | Accepted goal |
| Windows | VS2022 Win32 restoration build | Regression baseline |
| Build | CMake presets with Ninja; preserve existing MSBuild path | Isolated probes, platform services, a configured production EventSystem ELF module, production event/profile owners and native lifecycle shell build; game graph planned |
| Hotkey input | Existing DAHOTKEY event processor behind legacy-message and `IFileSystem` contracts | Production factory, read-only UTF adapter, checked-in source tables and optional shell/EventSystem composition work natively; packaging, SDL joystick, recorder and Windows parity remain open |
| Window/input/time | SDL3 plus standard C++ behind a FrontierX platform boundary | A native SDL-backed DACOM `IWindowManager` owns the shell window, callback pump, focus/cursor state and close-veto lifecycle; application cadence, Windows parity, renderer and game integration remain |
| Graphics | Keep D3D9-facing code for bring-up; evaluate maintained native Vulkan paths | The native shell can explicitly send the repository scene from production `PrimitiveBuilder` through the actual App-side `RenderBatcher`, native `IRenderPipeline` vtable and registered `IRenderPrimitive` to Vulkan presentation/readback. The adapter owns checked RPVertex/RPVERTEX2 CPU buffers through create/lock/unlock/destroy, routes direct/indexed draws into the registered owner and supports six transactional depth/blend states; unsupported pipeline slots fail explicitly. The production `BMPOOLJR` arena owner, vertex-buffer manager CPU lifecycle and neighboring vertex-descriptor copier execute in focused native targets. The batcher direct and U16-indexed branches plus opaque and alpha-depth queued-pool CPU paths have linked synthetic checks; the alpha-depth pool has visible GCC/Clang and focused sanitizer GPU evidence using repository geometry. A separate GPU probe draws indexed RPVERTEX2 geometry, observes the second UV pair through readback and verifies two GPU scene lifecycles plus preservation after an invalid upload. Six stage-zero color/alpha combine fields traverse the batcher and primitive to the fragment shader, with bounded image-change and rollback evidence. Production `StateMaterial` submits a cached indexed draw in visible GCC/Clang and focused sanitizer Vulkan probes; `MaterialLibrary` creates it in a linked Linux CPU check. A generated RGB565 UTF texture now loads through production `TextureLibrary`, is resolved by a `SimpleMaterial` loaded from production `MaterialLibrary`, and reaches visible Vulkan output via `StateMaterial`; `SimpleMaterial::apply`, other texture formats, other queued pool modes, production global/diagnostic services, backend selection and a running game scene remain open |

| D3DX9 | Inventory and replace effects, texture helpers, and math dependencies | Required |
| Audio | Portable backend, selected after dependency and behavior evaluation | Undecided |
| Video | FFmpeg-based replacement for legacy movie playback | Planned, later phase |
| Network | Portable sockets with deterministic protocol behavior | Deferred |

The SDL owner now registers two repository RGBA8 textures under monotonic runtime `U32` handles;
the production `RenderBatcher` binds both through the existing texture-stage slot, with bounded
image-change, exact restoration and stale-handle checks. The production texture
library's one-level RGB565 path and `StateMaterial`-driven synthetic rendering
now execute natively; see
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).
The production `MaterialLibrary` now discovers two material classes from a
repository-owned UTF fixture through the native `IFileSystem` directory path,
creates and indexes `StateMaterial` instances, and drives one indexed CPU draw.
The production `SimpleMaterial` reads diffuse color, opacity and a named
texture from the same synthetic UTF fixture and submits one cached indexed
draw in a focused CPU check; native `set_material` remains open.
The supported game uses separate `PolyMesh` and App `IMaterialManager`/`DrawVB`
mesh paths. At this initial object-compile gate the App draw remained
unported; the
production `PolyMesh/Mesh.cpp`, `polymesh.cpp`, `Engine/engine.cpp` and
`RenderMgr/RenderMgr.cpp` owners now compile as isolated GCC/Clang objects. The linked native
CPU check loads `IRenderer` and `PolyMesh` through the production Engine
profile, creates a generated CQ2 mesh archetype and an engine instance, and
retrieves that instance's mesh through `IRenderer`. A synthetic CPU call now
enters production `render_instance` with the scalar `I3DMathEngine` provider,
then reads native `RP_TEXTURE` and forwards model/view through a registered
primitive receiver. Its indexed `MTVERTEX` draw reaches a rejecting CPU
callback with six U16 indices; no game image is produced. The SDL GPU
renderer now compiles a 36-byte game-vertex pipeline with specular color and
UV-set selection, and the production component stops its remaining passes
after a rejected draw. A separate synthetic game-format GPU probe has
visible GCC, Clang and focused sanitizer Vulkan readback. The production
generated `PolyMesh` instance now presents a production-loaded RGB565 texture
on SDL/Vulkan under GCC and Clang, with 496 changed pixels in a bounded
readback. A white fallback produces a different checksum on the same draw. This
remains a synthetic mesh fixture rather than an App scene or retail game
frame; see [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).
The App `StandardVertex` format now has a checked 68-byte native CPU
vertex-buffer and primitive-callback path. A native U16 index-buffer object
lets a direct pipeline call send a generated indexed slice through that path
with exact byte comparison. Production App
`MeshRender::Init`, `RestoreVertexBuffers`, `SetupMeshInfo` and
`SetupDiffuseBlend` now execute in a generated CPU fixture. The App
`MeshRender::Render` caller selects a generated CQ2 `IMaterial` and rejects
its unsupported `Synthetic.fx` effect. A separate generated mesh and a
source-pinned native `VertexColor.fx` pass now drive that same App caller
to visible Vulkan through production `Material::DrawVB`.
Stage-zero SDL sampler switching has visible Vulkan image-change and
rollback evidence. The older-format App draw now reaches a separate
visible 68-byte SDL/Vulkan pipeline, with 496 changed readback pixels
while its texture-factor and clockwise culling states are active.
The native `set_vs_constants` adapter now carries the `VertexColor.fx`
`c30` UV and `c31` color registers into independently changed and restored
visible App readbacks using synthetic input. The supported CQ2
`VertexColor.fx` material now invokes this constant path. Other D3DX
effect techniques and fixed-function states remain open.
`XMesh::read` executes on a generated UTF mesh with one legacy
material in the linked CPU check; the old-format material resolves a
production texture frame. A separate native check now compiles and executes
the actual game `MaterialManager` and `Material` owners on a generated UTF
script and saved float parameter, then connects them to `XMesh::read` and
the production `TextureLibrary` through a generated CQ2 mesh. The manager's
script texture now also loads through the production game `ITManager` and
TGA reader, reaches a checked 4-by-4 native pipeline CPU upload and is
released through the texture manager. The unsupported synthetic material
still rejects its draw. The source-pinned `VertexColor.fx` script and its
saved TGA now produce a visible direct production material draw and a
separate CQ2 App mesh draw through the production Engine profile. The
native shell now accepts supplied material and mesh UTF files for a
repeated production Engine/RenderMgr/PolyMesh scene through SDL/Vulkan. It
keeps the game owners alive while pumping window and EventSystem messages,
redraws until close or a bounded deadline, and requires a stable 64-by-64
image on each frame. Its
legacy branch's GCC/Clang bounded captures match the linked PolyMesh
fixture. The CQ2 branch now also calls production App `MeshRender::Render`
with the source-pinned `VertexColor.fx` effect and TGA; both compilers
produced the same bounded image in that shell mode. A playable native
game scene and wider effect formats remain open. Their
ownership and integration gates are in
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).
The active App `SysContainer.cpp` also compiles on Linux and now owns the
native shell's EventSystem and WindowManager aggregates through its real
`ISystemContainer` interfaces. Its `LoadSystemComponents` now executes with
synthetic EventSystem/WindowManager profile entries by default. The shell can
also pass a bounded external profile into that production loader; missing
sections and uncreatable native components fail explicitly. A lawfully
supplied retail profile has not been executed. The Windows `CQPipeline`
wrapper remains a separate gate; see
[APP_SYSTEM_CONTAINER.md](APP_SYSTEM_CONTAINER.md).
The GPU-enabled shell now also executes the production `GameSystem::startup`
caller with an empty synthetic `[Engine]` section and releases it through
`GameSystem::shut_down`. Its Engine now draws the fixture after a later
synthetic renderer-component load. The App container now owns the production
`ITManager`, `IMaterialManager` and `TextureLibrary` aggregates. An external
profile can create a dormant `IRenderPrimitive` aggregate there; the scene
binds it after GPU startup and unbinds it after Engine teardown. A supplied profile can also create an `IRenderPipeline` aggregate that
binds after GPU startup. The production `IViewConstructor` and
`TextureLibrary`, `LightManager`, `VertexBufferManager` and `IStringTable`
factories now also aggregate from a supplied profile. A generated nine-service profile
starts successfully; its vertex-buffer manager initializes after the
scene pipeline binds and cleans up before it unbinds.
The production StringTable's `ISaverLoader` also reads a generated UTF
archive through the DOS-root child interface requested by `MScript` in a
linked CPU check. The App `MScript.cpp` caller and the linked check now share
that small load function; the `MScript` translation unit remains unexecuted.
The checked-in `Conquest.ini` still stops first at the unported audio
`IStreamer`. The production light owner also shades generated vertices in
a focused CPU check. A generated legacy mesh now reaches its strided
lighting method through the production PolyMesh system-service lookup, and
the supplied eight-service profile produces visible Vulkan output through
that caller. The CQ2 App fixture uses a separate draw branch;
see
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).
The production `DOCUVIEW` scanner, parser and symbol-table core now compiles
and executes a synthetic Linux layout check. A parsed `long` and pointer
remain four bytes, and the generated record has the expected Win32 offsets.
The production `IDataParser` also enumerates those fields through its public
callback interface.
`IViewConstructor2` now compares and correlates generated record schemas in
a linked Linux check. Its production `MScript` caller has not run natively;
see [GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).
The native UTF `IFileSystem` factory now returns read-only, exact-case child
directory and implicit file views for the production texture and game
material loader call patterns.
Its nested-view contract executes with a repository-owned synthetic archive;
the production `TextureLibrary` owner now also compiles and initializes in the
linked Linux material check. It decodes a generated 2-by-2 RGB565 UTF image,
uploads RGBA8 through the native pipeline, exposes the frame to `SimpleMaterial`
and rolls back an injected upload failure. The existing SDL/Vulkan probe now
loads `SimpleMaterial` through production `MaterialLibrary`, binds its resolved
texture through `StateMaterial`, observes a bounded
image change, restores the original image and repeats the scene under GCC,
Clang and focused ASan/UBSan. Other formats, `SimpleMaterial` GPU application
and native game scene composition remain open.
See [TEXTURE_LIBRARY_PORTABILITY.md](TEXTURE_LIBRARY_PORTABILITY.md).
The production `StateMaterial` runs through DACOM initialization, native
state/texture callbacks and a cached indexed draw in a synthetic CPU target
and visible Vulkan probe; its ownership checks and remaining material-library
and game-composition gates are in
[STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md).
The native `IRenderPipeline` also creates and uploads one RGBA8 texture level for the probe and
shell. A 2026-09-24 synthetic regression rejects duplicate live backend handles while retaining
the first texture's metadata. Its one-level CPU lock/unlock path now preserves a writable
shadow and retries a failed backend upload. Visible GCC, Clang and focused ASan/UBSan
Wayland/Vulkan probe runs observed the resulting alternate texture. Production material
file loading and further formats remain open.

### Verified Linux Layout Probe

Compile and automated-check evidence from Linux x86_64 on 2026-09-12:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug
```

GCC 16.2.1 and Clang 22.1.8 each produced native 64-bit x86-64 ELF probes and passed the four
registered CTests. The layout probe verifies the LP64 process model, fixed-width legacy integer
aliases, native pointer width in `HYBRID`, retail mission enum values, and the size, alignment,
field offsets, final-field extent, and x86_64 bitfield encoding of the production
`MISSION_DATA_BIN` and `ResourceCost` definitions. The build also compiles the existing TinyXML
sources as the first portable static library and links a behavior test that covers parsing,
entities, errors, and LP64 attribute conversion. A separate standard-C++ platform library now
provides monotonic microseconds and 32-bit milliseconds, clamped elapsed-time calculation,
millisecond sleep, and thread yield; its linked test checks deterministic clamping and that the
clock advances across a timed wait. The modal UI frame clock and progress-animation clock and sleep
are the first production consumers. The remaining QPC calls in `Modal.cpp` feed legacy profiler
counters and are not part of this migration. These targets do not compile game or engine modules,
load retail data, or establish runtime parity. The owning `Trim` module, Windows rebuild, and ABI
validation remain pending on native Windows. TinyXML release provenance remains a distribution
blocker recorded in [DEPENDENCIES.md](DEPENDENCIES.md).

The read-only `frontierx_data_root_probe` checks the seven required and five optional directory
paths used by `MenuResource::updateLoadPath` and distinguishes exact matches, case mismatches,
missing paths, ambiguous case-folded matches, and non-directories. Its self-test proves that
optional media warnings do not make an otherwise valid root unusable. A source-tree run found four
required and five optional case mismatches plus three missing required paths under `Data`; no retail
installation was inspected, and no file contents were read or copied.
The headless Linux selection order is an explicit command-line path, `FRONTIERX_DATA_ROOT`, then
the existing `Data` relative-path default. Synthetic process checks verified environment selection
and command-line precedence. The Windows registry-backed `DataPath` policy remains unchanged.

The platform library also owns a bounded native preference store for the two value classes used by
`IUserDefaults`: strings and opaque binary values. Its new `FXSET001` format uses explicit
little-endian lengths and types rather than dumping native structs. Keys are serialized in stable
byte order, malformed loads are transactional, and saves replace the target through a temporary
file in the same directory. `LegacyPreferenceValues` now reproduces the existing typed
binary/string, deletion, short-buffer and four-entry MRU value semantics without registry or dialog
calls. A native DACOM component implements the real `IUserDefaults` vtable, maps its live
`USER_DEFAULTS` view to the fixed-width schema, performs transactional immediate and shutdown
saves, and runs in the native shell. Native game-startup selection, file dialogs, document/viewer
editing, installation metadata, EULA presentation and Windows comparison remain open.
The shell now executes the live component's `LoadDefaults` before the
production `GameSystem::startup` caller, matching the App's relative owner
order; the actual `Conquest.cpp` entry point remains Windows-only.
`StartupPreferences` now executes the isolated player/global lifecycle for both headless startup
and the native shell through `NativeUserSettingsOwner`:
it selects player/global/default settings, scopes player keys by hex-encoded name bytes, removes
only an invalid player namespace, and stores global/player schemas transactionally. The
separate `NativeUserSettings` conversion persists 25 required fixed-width version-1 fields and 31
explicit flag assignments rather than native bitfields. Both startup paths resolve an explicit
preference path, then absolute `XDG_CONFIG_HOME`, then
`$HOME/.config/frontierx/preferences.dat`; it ignores relative base-directory variables and
validate both the container and user-default schema without exposing the resolved path. The
supported Windows registry implementation is unchanged.

The archived `misyltoad/dxvk-native` project is useful historical evidence that native D3D9 and SDL
integration can work, but it is not an approved or maintained dependency. Do not vendor it or make
it the architecture's source of truth without a current maintenance, compatibility, and license
review.

## GeneralsX as a Reference

GeneralsX is a process and architecture reference, not a source or status baseline for FrontierX.
FrontierX adopts its strongest transferable principles: preserve the original engine and retail
behavior, isolate platform code, keep one portable source line, migrate incrementally, protect
determinism, and validate narrow vertical slices before broad rewrites.

FrontierX does not inherit Generals-specific paths, build commands, backport rules, math wrappers,
or completed-platform claims. Its mandatory DXVK, OpenAL/MiniAudio parity, macOS, and ARM64 choices
also do not become FrontierX decisions without Conquest-specific technical, maintenance, license,
and runtime evidence.

## Compatibility Invariants

- Retail file, save, map, schema, and network layouts use fixed-width integer types and explicit
  packing where required. Never widen a persistent field merely because the runtime is 64-bit.
- Runtime pointers and handles must not be stored in `U32`, `DWORD`, or other 32-bit surrogates.
  Separate serialized identifiers from native runtime references.
- Changes to structures shared by `Conquest`, `Mission`, `Trim`, `Globals`, `ZBatcher`, or campaign
  modules require layout and ABI validation across every affected target.
- Platform, graphics, audio, and logging changes must not alter simulation order, random-number
  consumption, floating-point policy, or serialized bytes without an explicit compatibility plan.
- Case-sensitive Linux paths are authoritative for portable code. Include and asset lookups must
  use the exact on-disk spelling.

## Evidence and Scope

The retail game and its data define expected behavior when the CQ2 snapshot disagrees. Use the
existing `HANDOFF.md`, `RETAIL_TEST_MATRIX.md`, and tools under `Tools/` as the initial evidence
base. State whether a conclusion comes from compilation, an automated check, or a runtime test; do
not describe planned Linux behavior as implemented.

Do not opportunistically fix unrelated retail-restoration defects while introducing a platform
boundary. Report them or isolate them in a separate change so regressions remain attributable.

## Legal Boundary

`Conquest Source License.txt` is the controlling license. `LEGAL.md` defines this repository's
compliance process and `MODIFICATIONS.md` is the required prominent change record. No dependency,
copied implementation, binary, retail asset, or release artifact may be added merely because it is
technically convenient.

In particular, `binkw32.dll` and `Conquest_Frontier_Wars_Manual.pdf` have unresolved redistribution
provenance and must remain outside FrontierX history and release packages unless documented
permission is obtained. Raw retail binaries, disassembly projects, decompiler output, dumps, and
extracted media stay outside the repository.
## Port readiness follow-up

See [Linux port readiness](PORT_READINESS.md) for the first proposed layout-probe milestone,
initial blocker inventory, ABI tool parameters and pending validation. The layout probe and isolated
TinyXML library are implemented; there is no native game build. The ABI scripts are parameterized,
reject non-Windows hosts, and still require native Windows execution with `dumpbin`.

## Build declaration inventory

Run `python3 Tools/portability/build_inventory.py` from the repository root to inspect the
supported Windows entry points without invoking Windows tools. See [build graph evidence](BUILD_GRAPH.md)
for scope, test commands, measured dependencies and production profile-parser evidence. This
read-only inventory does not evaluate MSBuild or replace native Windows ABI validation.

## Profile buffer core (2026-09-13)

The Linux presets build `frontierx_profile_buffer_probe`; its registered checks pass with both GCC
and Clang. `FrontierX/ProfileBuffer.h` owns section marking, line/section navigation,
key lookup/copying, and section/key callback traversal shared with the production DACOM parser.
Runtime section offsets now use the pointer-width encoded-handle contract instead of `U32`
decoding. This remains an internal borrowed-buffer contract with sentinel padding, not a complete
portable DACOM implementation or game target. Native Windows Debug/Final rebuilds, callback
comparison and ABI checks remain required; see
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

### Owning native profile reader (2026-09-13)

The existing GCC and Clang presets build and execute `frontierx_profile_document_probe`.
`FrontierX/ProfileDocument.h` provides owning text/file loading, ordered section names
and key lookup through the shared profile-buffer core. Native startup configuration currently
accepts ASCII without NUL bytes, up to 1 MiB, with locale-independent ASCII case folding.
This policy is limited to the new reader; Windows DACOM comparison behavior is unchanged.
No native game startup or retail-configuration compatibility is claimed by the synthetic tests.

The production DACOM profile-parser source now builds as a native static service with both real
factories and interfaces. Standard-library file I/O is isolated to the non-Windows branch, buffer
initialization no longer transports sizes through invalid enum values, and the aggregate passes
GCC/Clang plus sanitizer execution through the native component manager. The owning document is
now a focused target whose raw-line view drives bounded transactional native `[Libraries]`
configuration. A native process-global `DACOM_Acquire` now owns this manager and registers both
production profile factories once. It also registers the native `IHeap`; the EventSystem proves
one cross-ELF owner while further owners, writable INI behavior and Windows parity remain open; see
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

### Native heap and first cross-ELF owner (2026-09-19)

`frontierx_native_heap` implements the unchanged `IHeap` interface with synchronized tracked native
allocations. The global manager registers it before other factories; `InitializeDAHeap`,
`HEAP_Acquire`, `SetDefaultHeapMsg` and `HEAP` execute through that owner. GCC, Clang and focused
sanitizers cover allocation, zero fill, reallocation, metadata, enumeration, foreign-pointer
rejection and repeated initialization. The production EventSystem ELF uses this same host heap for
its aggregate and queue nodes, restores exact accounting before unload, and hides every C++/RTTI
definition behind a two-entry-point version script. Configured capacity is diagnostic, private
COMHeap pass-through slots are unsupported, and further modules require owner-specific proof. See
[NATIVE_HEAP.md](NATIVE_HEAP.md).

The static EventSystem fallback also compiles with DA-heap allocation enabled. Its probe observes
tracked aggregate/queue blocks and exact baseline restoration, so configured and fallback owners
no longer select different allocation families.

## Native headless startup check (2026-09-13)

The GCC and Clang presets build `frontierx_headless`; its registered process check executes ten
scenarios. Python 3 is required when BUILD_TESTING is enabled.
The following help command was executed:

```text
out/build/linux-gcc-debug/frontierx_headless --help
```

The executable accepts `--data-root DIRECTORY` and an optional `--config FILE`. The latter is a
new native configuration, not retail Conquest.ini: use `[FrontierX]` and `DataRoot=path` with a
final LF newline. Relative config paths resolve against the config file's directory. Selection
order is CLI, FRONTIERX_DATA_ROOT, explicit config, then Data relative to the working directory.
An explicitly supplied invalid configuration always fails, even when another path overrides it.
ASCII/control-character restrictions of the native reader apply; paths are not silently trimmed.

Exit codes: 0 means directory checks passed (or help), 1 invalid arguments, 2 invalid configuration,
3 unusable required data directories, 4 unexpected startup-check failure. Optional directories
produce status lines without failing the process. Reports are deterministic and do not print the
user's absolute installation path. No media contents, metadata, campaign or game startup are
validated. Synthetic tests cover selection precedence, relative configuration paths, malformed
options/configuration, missing roots, case mismatches and optional-media behavior.

### UTF metadata header check (2026-09-13)

`frontierx_headless` additionally accepts `--metadata FILE`, an explicit path resolved from the
working directory. After directory checks, it reads only the 56-byte UTF 1.01 header and validates
signature, version, 44-byte directory stride, aligned directory size, metadata ranges, overlap and
free-entry offset. Fields are decoded as little-endian fixed-width words; native structs are never
mapped onto disk bytes. Invalid metadata returns exit code 5. Omitting the option leaves this check
unperformed. Success does not validate directory entries, names, payloads or GenData archetypes.

The headless process check passes under both compiler presets, including synthetic UTF header
mutations and missing/truncated metadata. No retail files were read.
The format evidence is the source declarations in DOSFile/BaseUTF.h, header validation in
UTFDMan.cpp and the GenData.cpp loadTypesData caller. The UTF inspector is not yet the DOSFile
backend replacement. Its summary is diagnostic only and does not certify a compatible installation.

### UTF directory validation (2026-09-13)

The metadata check now reads directory and used-name tables (combined limit 64 MiB), walks the
reachable tree iteratively, rejects repeated/cyclic/misaligned links, validates NUL-terminated
name references and bounds file allocation/used extents against the container size. The free-entry
chain is checked separately. Source review of UTFDMan.cpp corrected the earlier assumption:
the header's first free-entry offset is absolute; its subsequent dwNext links are directory-relative.
Synthetic tests include a valid free chain and a cyclic one. Unreachable slots and free payload-space
lists are not certified; payload contents/decompression and archetype schemas remain unchecked.

The GCC and Clang headless process checks pass with synthetic UTF trees, invalid links, names and
payload ranges. No retail installation was inspected. This
extends the read-only inspector; it does not yet replace the Windows DOSFile implementation.

The headless checker now accepts `--metadata FILE --entry VIRTUAL_PATH` to read a bounded UTF
entry into memory and report its byte count and diagnostic checksum (2026-09-13). The focused
process scenarios pass under both presets and include a nested `dir/file` entry, exact case-sensitive
lookup and rejection of malformed virtual separators. No schema or retail compatibility is
implied; see the UTF entry-reading
section of the port-readiness plan for limits and outstanding work.

### Portable scalar math core

An isolated `frontierx_math` target now implements the scalar operation surface behind
`I3DMathEngine` using ABI-neutral values and fixed-width inverse-square-root bit handling. Its
GCC/Clang targets disable FMA contraction, and the complete sixteen-test matrices cover the new
probe. The historical K62/3DNow sources own the concentrated assembly findings and are separate
from scalar `x86math.cpp`. The supported Windows wrapper currently reaches `MathLib` but does not
build or verify the provider DLL. The native DACOM adapter, Windows numerical oracle and
floating-point-environment policy remain open; see [MATH_BACKEND.md](MATH_BACKEND.md).

### Portable mesh ray geometry

The production MeshManager hit test now converts named engine positions into a header-only
portable ray-geometry boundary instead of aliasing them as D3DX vectors. GCC and Clang probes cover
the documented box and triangle contract; the full matrices contain sixteen tests. This removes
two D3DX helper functions from one real engine path, but Windows boundary-tolerance comparison and
the remaining renderer replacements are open. See [D3DX9_PORT.md](D3DX9_PORT.md).

Native training entry (2026-10-12): --mission-campaign with --mission-start
and Sol_Train.dmission uses the original selector/briefing. M2 Linux training
and winning return are accepted in the documented optional runtime scope;
other campaigns, host leak/resize limits and M3 comparisons remain outside
that acceptance. Commands and limits: [native singleplayer evidence](NATIVE_SINGLEPLAYER_SLICE.md).
