# First native single-player acceptance path

## Original in-window mission teletype checkpoint (2026-10-10)

**Blocker and hypothesis.** The voice checkpoint still replaced Teletype
with explicitly disabled timed handles. Review of original `Teletype.cpp`,
its `MScript::PlayTeletype` caller, `Sol_T::DisplayObjectiveHandler`,
`DrawAgent16.cpp` and `System.cpp::CQCreateLogFont` showed that source line
wrapping, reveal, caret, centering, scrolling, pause and expiry already
existed. GDI font creation/rasterization and the Win32 callback container
were the blockers. The falsifiable milestone was an original objective
teletype visibly drawn during source `CQDoModal`, with the existing normal
selection/movement, voice and moved-state save/reload still passing.

**Integrated owners.** Original `Teletype.cpp` now compiles and subscribes
to the existing native EventSystem. Its source `CQE_ENDFRAME` draw and
`CQE_UPDATE` timing own the live text. The Windows GDI/FULLSCREEN branches
remain in place. A native-only adapter implements existing `IFontDrawAgent`
with optional FreeType; it reads the resource pixel height using the source
`IDEAL2REALY` scaling and an explicitly supplied external face. Monochrome
glyph spans reach the existing App PrimitiveBuilder, batcher and production
native render boundary. Pixel clipping respects the original reveal pane;
no new shader, SDL debug-text overlay or replacement teletype scheduler was
introduced. Glyphs are cached in each font owner and released with its face.
The old `--skip-teletype-visuals` timer-only exclusion remains available.

Source review plus the first native runtime disproved an assumed counted
string contract: source-built RT_STRING stores the character count without
a terminator. Native Teletype now skips that prefix with sequenced pointer
advance and uses its full count; the original Windows expression and branch
remain unchanged. Native line buffers are checked before copying, position
storage includes the final extent, and the reveal's formerly uninitialized
`oldcounter` is initialized. None of these runtime types crosses a disk or
module interface; serialized mission fields and source queue/update order
are unchanged.

**Configuration and dependency.** Default presets stay FreeType-free.
Optional native text is configured with `-DFRONTIERX_BUILD_NATIVE_FONTS=ON`
and requires FreeType >=2.14. The shell requires `--ui-font FILE` for
mission teletype visuals; an absent backend or invalid face fails explicitly.
This checkpoint uses lawful external `FONTS/bauhmib.ttf`, matching the
selected source Bauhaus descriptor's family. Other resource families,
weights, Unicode outside printable ASCII, shaping, multiline font-agent
semantics and Windows glyph/metric parity remain unsupported or unverified.
The single-face option is explicit and is not automatic font substitution.
See [DEPENDENCIES](DEPENDENCIES.md#optional-native-font-rasterizer-freetype-2143)
for the system package, FTL selection, credit and outstanding release gates.

**Automated execution.** GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug
Ninja built `frontierx_native_shell` in the existing optional-FFmpeg trees
`/tmp/frontierx-voice-{gcc,clang}`, then built both complete graphs with
`cmake --build <tree> -j6`. Each preset was configured with
`-DFRONTIERX_BUILD_NATIVE_FONTS=ON` and
`-DFRONTIERX_TEST_FONT_FILE=<external-install>/FONTS/bauhmib.ttf`, retaining
the previously reviewed external shared-LGPL FFmpeg prefix. The exact
focused command `ctest --test-dir <tree> -R '^portability.native_shell$'
--output-on-failure` passed under both compilers.

The existing generated-data shell process check now optionally supplies
`--font-smoke FILE` alongside its existing `--sfx-smoke`. It constructs the
original Teletype owner, checks that counted `AB` is retained completely,
checks source line positions against font width, distinguishes ordinary
pause from ignore-pause, preserves lifetime-zero text, verifies the original
next-update removal of expired handles, flushes and requires live font
ownership to return to its starting count. It restores the native ID counter
and pause flag. This is needed because the old archive and timer-only checks
could not execute source layout or its font ownership; passing permits the
mission to use that owner. No new probe executable was added. Inputs are
generated PCM, schema and counted text plus the explicitly external font.
With no font path supplied, that optional case is not claimed as execution.

A separate Clang Debug ASan/UBSan tree at `/tmp/frontierx-voice-asan` retained
`-fsanitize=address,undefined -fno-omit-frame-pointer` and matching executable
linker flags, enabled fonts with the same external test file, and built
`frontierx_native_shell frontierx_event_system_module`. Its existing shell
CTest passed with `ASAN_OPTIONS=detect_leaks=0` without a reported finding,
including the actual source text lifecycle and font-release check.
The system FreeType binary was not sanitizer-instrumented; LeakSanitizer
and a full mission sanitizer run were not performed. Complete default and
optional CPU/process test runs are recorded only in [BUILD_GRAPH](BUILD_GRAPH.md).
The desktop-dependent GPU resize test was excluded from those reruns because
its previously documented tiling failure is unchanged; it is not a pass.

**Visible external-data runtime.** Separate GCC and Clang Wayland/Vulkan
processes ran the previous voice checkpoint's mission command, removed
`--skip-teletype-visuals`, and added
`--ui-font <external-install>/FONTS/bauhmib.ttf`. Each used 15 seconds,
isolated save/settings roots, lawfully external mission/archives/audio/font,
the repository profile/hotkeys/material root and generated shaders.
Normal mouse selection at `(303,425)` and movement at `(624,480)` retained
objectives 1 and 2, two original unit voices, no active audio stream at
acceptance, modal result 4, moved-state source save/reload and mission close.
The GCC acceptance observed 383 modal frames, 450 source updates and 1144
font draws; The final delayed-input Clang capture run observed 367 frames, 450 updates
and 2153 font draws.
Respective moved positions `(49926.4,66613.2,-1000)` and
`(47970.1,67008.8,-400)` retained owner 1 through reload. Wall-clock input
and render counts are not a cross-build determinism result.
Temporary cropped desktop captures outside the repository were visually
inspected: the source objective text is yellow, progressively revealed with
its caret over the live scene. No retail asset, capture or log was added to
source control. This is bounded external retail-input runtime, not full
retail game parity or a Windows image comparison.

**Next gate.** Connect status/hint/scrolling text and subtitles through their
existing font/presentation owners, then remove their individual exclusions.
Unit alert text, training portraits/speech, mission music, spatial SFX,
other UI/render exclusions, menu/briefing paths and full-game behavior remain
open. Native Windows Debug/Final, module ABI, Windows font/audio comparison,
cross-compiler determinism and full retail parity were not performed.

## Original unit voice mission checkpoint (2026-10-10)

**Blocker and hypothesis.** Normal selection and movement already reached
original `ObjComm`, but `UNITCOMM` was an explicitly disabled owner. Source
review of `UnitComm.cpp::getFilename`, the selection/movement callers and
`StringData.cpp` identified an existing integration path: original UnitComm
queues and filename selection, original StringData lookup, native
`ISoundManager` and production `IStreamer`. The falsifiable gate was two
voice requests from normal mouse input, with completed/interrupted stream
cleanup and the existing moved-state mission save/reload still passing.
No separate voice scheduler or game loop was needed.

**Integration.** The native shell now accepts external `--string-pack FILE`
and creates the original StringData and UnitComm owners before mission load.
StringData copies bounded opaque UTF records into owned native storage,
preserving traversal and bare-name lookup while excluding `Parsed Files`.
The real archive also contains editor source; treating every payload as a
speech-slot array was disproved by that input. UnitComm checks only the
speech schemas and fixed 32-byte strings it actually consumes, before its
unchanged random selection. Disk schemas and public interfaces are unchanged;
runtime pointers are constructed locally, never read from serialized data.
The existing EventSystem drives the original queue/priority updates.

The native mission sound adapter now tracks source object IDs rather than
borrowed object pointers, closes completed streams on source update, clears
speaker highlight on completion/stop, and flushes on mission close. This is
original UnitComm integration with a native sound adapter, not a port of the
entire Windows SoundManager. The selected input used no highlighted speaker;
highlighted alert runtime remains unverified.

`--skip-unit-alerts --skip-subtitles` are required for the enabled voice
path until the source font/subtitle owners work. The visible run also used
`--skip-mission-sfx`: disabled ISFX now keeps logical effect handles without
eager decoder streams, reports preload unavailable, and returns before
unsupported spatial playback, matching the source disabled-Play ordering.
Previously eager disabled SFX held the shared stream capacity and blocked a
unit voice open. The existing shell SFX smoke now exercises disabled spatial
Play and preload/resource behavior. Spatial effects remain unsupported.
`--skip-training-speech` excludes only MSpeech, leaving unit Speech enabled.
The old explicit `--skip-unit-voice` path remains available.

**Build and automated execution.** Linux x86_64 Debug Ninja GCC 16.2.1 and
Clang 22.1.8 built complete default SDL-only trees with
`cmake --build /tmp/frontierx-mission-{gcc,clang} -j8`. Separate optional
FFmpeg trees were configured with each Linux Debug preset, `-B
/tmp/frontierx-voice-{gcc,clang}`, `-DFRONTIERX_BUILD_FFMPEG_AUDIO=ON` and
`-DCMAKE_PREFIX_PATH=/tmp/frontierx-voice-lgpl`. Targets
`frontierx_native_shell`, `frontierx_native_window_manager_probe`,
`frontierx_data_root_probe`, `frontierx_render_pipeline_probe` and
`frontierx_sdl3_gpu_probe` built in both optional trees. The official,
SHA-256-verified FFmpeg 9.0.1 source was rebuilt unmodified outside the
repository with the already reviewed restricted shared LGPL configuration
in [the audio document](NATIVE_AUDIO_STREAMER.md); system GPL FFmpeg was
not used. CMake's license checks passed.

`ctest --test-dir /tmp/frontierx-voice-<compiler> --output-on-failure -R
'portability\.(data_root|native_shell|render_pipeline|native_window_manager)$'`
passed all four focused checks per compiler. Existing data-root generated
UTF cases check nested lookup, ignored editor data, arbitrary opaque bytes,
source-release ownership, duplicate basename rejection and empty meaningful
archives. This extends the existing archive check because original UnitComm
requires an owned StringData view after the input filesystem is released;
passing permits its production mission composition. No new probe was added.
A separate Clang Debug tree used `-fsanitize=address,undefined
-fno-omit-frame-pointer` and matching executable linker flags; building
`frontierx_data_root_probe` and running its CTest with
`ASAN_OPTIONS=detect_leaks=0` passed without a reported finding.
LeakSanitizer and the full mission sanitizer closure were not run.
Full default graph results, including the observed GPU resize failure, are
recorded only in [BUILD_GRAPH](BUILD_GRAPH.md).

**External-data runtime.** Separate visible Wayland/Vulkan optional-FFmpeg
shells used the lawfully external Steam `Sol_Train.dmission`, `gendata.db`,
`gametypes.db`, `stringpack.db` and Speech archive. No retail payload was
copied into the repository. Each ran `--smoke-ms 15000`, isolated save and
settings directories, the existing repository profile/hotkeys/material
root and generated mission shaders. Normal `/dev/uinput` mouse events at
window-local `(303,425)` left and `(624,480)` right triggered selection and
movement through the original game callers. SDL's default host audio path
was used; no dummy audio override was set. Human audibility was not assessed.

Both runs opened and started two unit voices through IStreamer, interrupted
one through original UnitComm and completed the other: `started=2
completed=1 stopped=1 active=0`. GCC selected `SFO3054.wav` then `SFO3058.wav`;
Clang selected `SFO3054.wav` then `SFO3056.wav`. GCC ran 447 modal frames and
Clang 381; both observed 449 source simulation updates, objectives 1 and 2
complete, modal result 4, source save/reload acceptance, mission close and
exit zero. The player Forger moved from `(39936,68608)` to
`(49778.3,66640.9)` under GCC and `(49799.4,66636.7)` under Clang; each
reload preserved its respective owner, position and objective flags.
Wall-clock input and audio RNG outcomes do not establish cross-compiler
determinism.

The executed mission command had the following arguments (external paths
and compiler-specific temporary roots are represented by variables):

```sh
XDG_CONFIG_HOME="$settings" "$build/frontierx_native_shell" \
  --data-root "$install/DATA" --material-root "$repo/Data/mat" \
  --system-profile "$repo/Code/App/Src/Conquest.ini" \
  --skip-movies --skip-notification-cue --skip-sector-map \
  --skip-space-background --skip-unit-labels --skip-planet-ui \
  --skip-animated-portraits --skip-training-speech --skip-unit-alerts \
  --skip-mission-sfx --skip-teletype-visuals --skip-subtitles \
  --base-textures-only --skip-legacy-lighting --skip-mission-music \
  --mission-source Sol_Train.dmission --mission-engine --mission-start \
  --mission-gpu "$build/generated/shaders/sdl3_gpu_probe.vert.spv" \
    "$build/generated/shaders/sdl3_gpu_probe.frag.spv" \
    "$build/generated/shaders/sdl3_gpu_probe_mt.vert.spv" \
    "$build/generated/shaders/sdl3_gpu_probe_t4.vert.spv" \
  --string-pack "$install/stringpack.db" --gen-data "$install/gendata.db" \
  --game-types "$install/gametypes.db" \
  --hotkeys "$repo/Code/App/Src/Hotkeys.utf" \
  --save-root "$save" --smoke-ms 15000
```

The default SDL-only decoder still rejects IAC; this checkpoint requires
the explicit external LGPL configuration. The execution was bounded by
`timeout -k 5s 120s`; both shells exited normally before that timeout.

**Next gate and limits.** Restore in-window mission text/teletype at the
original font and presentation owners. Unit alerts, subtitles, training
speech/portraits, mission music, spatial SFX and the other named UI/render
exclusions remain open. Native Windows Debug/Final, cross-module ABI,
Windows audio comparison, full game/retail parity and determinism comparison
were not performed. No release, commit or publication was made.

## Source modal mission integration (2026-10-10)

**Blocker and hypothesis.** The prior checkpoint already implemented the
production pipeline's full-target color/depth clear, begin/end and swap.
`Modal.cpp::CQDoModal` remained outside the native link. Source review of its
real `Conquest.cpp` game callers and the bounded shell loop showed that a
mission frame clears before `begin_scene`, issues multiple object/UI draws,
ends and presents through `IRenderPipeline`, then advances source object and
GameSystem updates. The falsifiable hypothesis was that this existing
pipeline/SDL GPU contract could run the source loop with a native event
connection and an original mission quit event; a separate game loop or
renderer boundary was unnecessary. The existing CPU pipeline and GPU probes
cover lifecycle order, multiple draws, presentation and resource rebinding.

**Integration.** `Modal.cpp` now compiles in the native shell while preserving
its Windows branches. Native `CQDoModal(nullptr)` subscribes to the active
EventSystem outgoing connection, where the source mission sends dialog
results. The shell retains one startup diagnostic frame, then calls the
source modal loop for all repeated frames. A native shell event callback
posts `CQE_PLAYER_QUIT` after the selected duration or a window-close event;
the original `Mission::Notify` sends `CQE_DLG_RESULT` and modal returns
`MISSION_END_QUIT` (4). A zero duration allows normal continued play until
window close. Source frame-time conversion, event ordering, object rendering,
EndModal flush/present, `OBJLIST->Update`, `GS->Update` and frame timing now
own the repeated path. The native modal leaves the pipeline idle after swap;
the next game frame performs its own clear. Native clear/begin/end/swap
failures abort this modal result instead of claiming a completed frame.
After the accepted save/reload, the shell sends the game-inactive transition
and calls `Mission::Close`; normal window close also follows that shutdown
path. Other original mission end results are recognized and close the mission;
those outcomes were not exercised in this checkpoint. No public interface or
persistent format changed.

**Validation.** Linux x86_64 Debug Ninja GCC 16.2.1 and Clang 22.1.8 built
`frontierx_native_shell` in `/tmp/frontierx-mission-gcc` and
`/tmp/frontierx-mission-clang` with `cmake --build <tree> --target
frontierx_native_shell -j4`. The existing render-pipeline,
render-submission and native-shell CTests passed 3/3 per compiler before the
final builds. `SDL_VIDEODRIVER=x11 ctest --test-dir
/tmp/frontierx-mission-gcc --output-on-failure -j4` passed 51/51 including
GPU. The same Clang CTest command passed 50/51: its GPU test failed a desktop
tiling resize observation at 1854x2085. A separate floating X11 direct
`frontierx_sdl3_gpu_probe ... --require-runtime --visible` under Clang passed
12 Vulkan frames, 639x360 to 800x450 resize, depth recreation and repeated
scene initialization. A GPU skip is not counted as execution.

Separate visible Wayland/Vulkan processes ran the mission with lawful
external `Sol_Train.dmission` and the same named optional exclusions listed
in the playable checkpoint below. Each used `--smoke-ms 15000`, an isolated
save directory and normal `/dev/uinput` mouse events at window-local
`(303,425)` left and `(624,480)` right. The first source diagnostic frame
submitted 28 draws and changed 171 pixels in the bounded readback. GCC ran
407 modal frames and reached 450 source simulation updates; Clang ran 413
modal frames and reached 450 updates. Both selected the player Forger,
completed source training objectives 1 and 2, returned modal result 4,
saved/reloaded owner 1 and the exact respective moved position and flags,
called source mission close and exited zero. GCC moved from `(39936,68608)`
to `(50175.8,66560)`; Clang to `(50174.9,66560.2)`. The event timing is
wall-clock dependent; these values do not establish cross-compiler
determinism. A separate GCC run with no duration was closed through the
normal window close request, returned modal result 4, closed the mission
and exited zero without requiring training acceptance.

The executed focused/runtime command forms were:

```sh
ctest --test-dir /tmp/frontierx-mission-gcc -R '^(portability.render_pipeline|portability.render_submission|portability.native_shell)$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^(portability.render_pipeline|portability.render_submission|portability.native_shell)$' --output-on-failure
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell -j4
SDL_VIDEODRIVER=x11 ctest --test-dir /tmp/frontierx-mission-gcc --output-on-failure -j4
SDL_VIDEODRIVER=x11 ctest --test-dir /tmp/frontierx-mission-clang --output-on-failure -j4
bash /tmp/frontierx-alpha-runtime/run-modal-gcc-long.sh
bash /tmp/frontierx-alpha-runtime/run-modal-clang-long.sh
bash /tmp/frontierx-alpha-runtime/run-modal-gcc-close.sh
```

The three temporary runtime scripts contain the external-data, shader,
exclusion and save-root arguments; compositor cursor positioning and uinput
clicks were issued while each process was active. Logs, saves and retail
inputs remain outside the repository. Native menu hot rectangles and CPU
framebuffer locking are unsupported in this selected mission composition.
Unit voice/text, teletype visuals and the other named media/UI exclusions
remain open. Native Windows Debug/Final, module ABI, sanitizers, full game
progression and a unit-bearing Windows retail comparison were not run.

## Native mission frame lifecycle checkpoint (2026-10-10)

The native mission now clears, begins, ends and presents frames through its
production `IRenderPipeline` owner instead of calling SDL frame methods from
`NativeShell`. `NativeRenderPipeline` binds native-only frame callbacks to
`SdlGpuRenderer`; the legacy public vtable and all disk/save layouts are
unchanged. `RP_CLEAR_COLOR` retains packed ARGB and `RP_CLEAR_DEPTH` retains
its unsigned normalization by `0xffffffff`. The supported operation is a
full-target color-and-depth clear, followed by begin/end and swap. Partial
viewports, subrectangles, separate buffer clears and stencil clears return
`GR_NOT_IMPLEMENTED`. Invalid lifecycle ordering fails, and failed callbacks
do not advance the lifecycle. Unbinding clears borrowed callbacks and state.
Empty frames clear and present correctly. SDL command buffers with an acquired
swapchain image are submitted on draw failure rather than illegally cancelled.

**Blocker and test decision:** source `Modal.cpp::CQDoModal` and
`Mission.cpp` use these existing slots. Their absence prevented the active
mission composition from exercising the source frame contract. The existing
CPU render-pipeline probe now checks clear values, ordering, callback failure,
retry and unbinding. The existing GPU probe checks black/red empty-frame
readback, depth-zero rejection of geometry, restored depth and two captured
draws, presentation, resize and repeated renderer initialization. No parallel
render loop or new probe was added. Passing this gate permits work on linking
`Modal.cpp` and its remaining event/timing/UI services; it does not mean that
`CQDoModal` itself is linked or executed.

**Automated execution and synthetic GPU runtime:** Linux x86_64 Debug Ninja
trees `/tmp/frontierx-mission-gcc` (GCC 16.2.1) and
`/tmp/frontierx-mission-clang` (Clang 22.1.8) were rebuilt with
`cmake --build <tree> -j 12`. The existing render-pipeline and native-shell
CTest checks pass under both compilers. Full `ctest --test-dir <tree>
--output-on-failure` runs are recorded in [BUILD_GRAPH.md](BUILD_GRAPH.md).
The desktop's default Wayland/tiled GPU run failed its resize assertion;
this is not counted as a passing runtime. Separate floating X11/XWayland
Vulkan runs with `SDL_VIDEODRIVER=x11` and `--require-runtime --visible`
passed under both compilers: twelve presentation frames, resize to 800 by
450, two scene initializations and matching final readback checksum
`9787875930373773388` with 3072 changed pixels. Inputs were generated UTF and
repository-owned smoke geometry/textures. The GPU argument order is vertex,
fragment, **t4 extended vertex**, generated UTF path, **mt game vertex**.
The registered `portability.sdl3_gpu` CTest was also rerun with
`SDL_VIDEODRIVER=x11 ctest --test-dir <tree> -R '^portability.sdl3_gpu$'
--output-on-failure` in floating test windows and passed under both compilers.
The test's resize failure now reports observed window and target dimensions.

**Retail-input native runtime:** separate visible Wayland/Vulkan mission
runs used the same lawful external `Sol_Train` inputs, launch flags and
optional presentation exclusions as the prior checkpoint below. Both ran
299 source updates with normal mouse selection and movement, completed
objectives 1 and 2, and preserved the moved owner-1 Forger and objective flags
through original mission save/reload. GCC moved from `(39936,68608)` to
`(45117.5,67571.8)`; Clang to `(45120.1,67571.3)`. The initial frame still
contained 28 source draw submissions and 171 changed readback pixels.
A floating 800-by-600 window received a left click at `(303,425)` and a
right click at `(624,480)` through the external temporary uinput device.
Logs, saves and all external media remain outside the repository.

**Limits:** this completes the bounded native frame-boundary checkpoint,
not the entire modal loop, all clear semantics or retail parity. Source
modal event/timing/UI integration remains the next owner, followed by unit
voice/text and teletype presentation. The optional media/UI exclusions
remain necessary. Windows Debug/Final, cross-module ABI, sanitizers and a
unit-bearing Windows retail comparison were not executed in this step.

## Playable Solarian training checkpoint (2026-10-10)

**Retail-input native runtime:** the first bounded single-player slice now
executes with a lawful external Steam installation, outside the repository.
The selected `Sol_Train.dmission` acceptance is: load the original mission,
show the player-owned `FAB!!S_Forger`, select it with a left mouse click,
issue a right-click move, advance the original offline operation and object
simulation until Solarian training objectives 1 and 2 are complete, then
save and reload the moved Forger and those objective flags. The shell returns
zero only after all these postconditions hold; an absent click or objective
returns a distinct failure. A ten-second bounded run is used for validation.

The active native composition uses the unchanged source system profile and
required SDL `IStreamer`, original `Mission::Load`, `ObjectList`, `ObjComm`,
offline `NetPacket` and `OpAgent`, `MScript`/`SolTrain`, source camera,
Engine/RenderMgr/material/mesh callers and visible SDL/Vulkan presentation.
SDL mouse events enter the normal source selection and operation path.
Source `Mission::Save`/`Load` writes and reads a native UTF save under `/tmp`;
the archive contains script parse/global/program records. Native script
save aborts on a failed required directory or write instead of reporting a
successful save. A captured mission frame contained 28 source draw
submissions and 171 changed pixels in the bounded 64-by-64 GPU readback;
the live scene showed ships and a planet, and the visible Forger selection
completed the script's selection objective. Repeated render frames also
exercise the native line-list pipeline used by source selection graphics.

The validation window was 800 by 600 at desktop origin `(880,433)`.
After the source mission frame appeared, a left click at window-local
`(303,425)` selected the Forger; a right click at `(500,480)` issued the
move. The source simulation advanced 299 GCC and 298 Clang updates in the
final rebuilt runs. GCC 16.2.1 observed initial planar position `(39936,68608)`, moved
position `(45836.1,66641.3)`, objective flags `select=1 move=1`, and an
exact owner-1/position/flags round trip after reload. Clang 22.1.8 observed
the same initial position, `(45808.5,66650.5)` after movement, both flags,
and the exact corresponding reload. The position difference comes from
wall-clock input timing; the acceptance checkpoint is the same source
objective state and preserved moved position, not byte-identical movement
timing. Both separate Wayland/Vulkan processes exited zero. The mouse
events came from a temporary `/dev/uinput` device outside the repository,
not from direct calls into game logic.

For reproduction, build `frontierx_native_shell` with the GCC or Clang
Linux Debug CMake tree and invoke it with `--data-root` pointing to the
lawfully installed `DATA` directory, `--gen-data` and `--game-types`
pointing to the corresponding external databases, `--system-profile`
pointing to `Code/App/Src/Conquest.ini`, `--material-root` to `Data/mat`,
`--hotkeys` to `Code/App/Src/Hotkeys.utf`, `--save-root` to a writable
directory outside the repository, and `--mission-source Sol_Train.dmission
--mission-engine --mission-start --smoke-ms 10000`. Supply the four shaders
from the same build tree to `--mission-gpu` in this order:
`sdl3_gpu_probe.vert.spv`, `sdl3_gpu_probe.frag.spv`,
`sdl3_gpu_probe_mt.vert.spv`, `sdl3_gpu_probe_t4.vert.spv`. The validated
bounded presentation exclusions were `--skip-movies`,
`--skip-notification-cue`, `--skip-sector-map`,
`--skip-space-background`, `--skip-unit-labels`, `--skip-planet-ui`,
`--skip-animated-portraits`, `--skip-subtitles`,
`--base-textures-only`, `--skip-legacy-lighting`,
`--skip-training-speech`, `--skip-unit-voice`,
`--skip-teletype-visuals` and `--skip-mission-music`.

The final validation used preconfigured Debug Ninja trees and these exact
build/test commands (GCC tree first, Clang tree second):

```sh
cmake --build /tmp/frontierx-mission-gcc -j 12
ctest --test-dir /tmp/frontierx-mission-gcc --output-on-failure
cmake --build /tmp/frontierx-mission-clang -j 12
ctest --test-dir /tmp/frontierx-mission-clang --output-on-failure
```

The existing `portability.data_root`, `portability.render_pipeline`,
`portability.render_submission`, `portability.native_shell` and
`portability.state_material` checks executed on generated/repository
inputs under both compilers. The complete CTest result is recorded in
[the build graph](BUILD_GRAPH.md). Separate `frontierx_sdl3_gpu_probe` processes received
the build-tree vertex, fragment, extended vertex and game vertex SPIR-V
files plus a generated UTF fixture and `--require-runtime --visible`;
each used Vulkan and presented twelve frames. The integrated shell used
lawful external mission/data inputs and an external save directory. Its
GCC and Clang processes both exited zero after the normal mouse input,
source objectives and moved-state reload checks. The external mission
data, saves, screenshots and logs remained outside the repository.

`IStreamer` is a required source-profile service for Music, SFX, Speech
and MSpeech and remained initialized. The default SDL-only build cannot
decode this IAC training speech; its playback was explicitly excluded for
this visual/gameplay checkpoint. `IVideoSystem` serves full-screen movies;
the mission's gameplay path does not require a movie, and only explicit
`--skip-movies` omits that service. Unit voice, teletype text visuals,
sector-map GDI, decorative background, animated portrait/subtitles, font
labels, planet resource panel, inherited menu music and some texture/light
effects were separately excluded with named flags because their native
presentation paths are incomplete. Script teletype handles still advance
through source-time events and preserve their save IDs. These exclusions
limit audiovisual fidelity; they do not replace the source mission,
selection, operation, simulation or persistence owners.

This is a first playable slice, not a complete game or verified Windows
retail-parity result. The original CQ2 snapshot still reports some missing
effect archetypes on this retail input; their classification against a
working Windows retail baseline remains open. The documented Windows
mission without units is not a valid gameplay reference. Native Windows
Debug/Final rebuilds, cross-module ABI, sanitizers, complete UI/media,
longer mission outcomes and retail gameplay comparison have not run.
The next concrete blocker for a broader playable mission is the production
`CQDoModal` render contract: native `IRenderPipeline::clear_buffers` and
begin/end scene must execute so the bounded shell loop can give way to the
source modal game loop. The omitted in-game unit voice/text and mission
teletype visuals then require native owners, followed by a Windows
unit-bearing comparison for CQ2 restoration issues. The historical
diagnostics below are superseded by this checkpoint.

## Source perspective and multi-draw mission frame (2026-10-10)

The selected acceptance remains the external `Sol_Train.dmission`: a
player-owned Forger must be visible and selectable, receive a move command
through normal mouse input, reach a repeatable mission state through source
simulation, and retain that state after save/reload. The present shell reaches
the first source update, frame and static-state save/reload. Its post-load list contains one
`FAB!!S_Forger` owned by player 1 in system 2; source render visibility is
`-1`. The current screenshot shows mission geometry and ships, but a pixel
has not yet been attributed specifically to the Forger. The screenshot and
readback are runtime diagnostics, not acceptance of selection or movement.

The source camera must run `SetPosition` before its first render to establish
its near/far distances. The native `IRenderPrimitive::set_perspective` was
still returning `GR_NOT_IMPLEMENTED`, and the caller did not check that
result; therefore all game draws used identity projection and clipped away.
The native owner now builds the source half-angle perspective with the
camera's negative-Z convention, matching the Windows D3D owner's Z flip.
The existing render-submission check protects the matrix and invalid-input
contract. The SDL GPU backend also discarded every submission except the
last one. Explicit `BeginFrame` now captures each draw's buffers, texture,
sampler, pipeline, transform and render state, then presents them in order.
The existing SDL GPU probe now checks a two-draw frame with different
transforms; explicit visible Vulkan runs passed under both compilers.
Frame capture is currently used by the mission shell's first frame only;
the regular game loop is not connected. Frame resource mutation while a
captured texture is referenced fails explicitly.

The same process now calls source `Mission::Save` into the isolated native
save directory, reopens the written UTF archive through source
`Mission::Load`, and verifies that the player Forger still has owner 1 and
position `(39936, 68608, 0)`. The first save attempt did not publish a file
because the source queried the unversioned `IUTFWriter` name. The native
writer now accepts that exact existing caller alias. The next attempt
exposed the source's Debug-only `SaveParseData` call; native builds now
write that required schema or fail the save. `ObjectList::Save` expected
`IFileSystem::CreateInstance` on an existing directory to return a writable
directory view, then its object savers used default `DAFILEDESC` child
creation. The native UTF owner now fulfills both contracts. The existing
data-root check covers alias lookup and nested directory-view child write.
The native mission save also checks `ObjectList::Save` rather than reporting
success on its failure. During reload, native no-GDI `ISystemMap` now tracks
bounded map invalidations. `MeshManager::FindMeshArch` skips an engine
archetype whose name is absent after reload, avoiding a null `strcmp`; this
source-level guard also affects Windows and needs native Windows validation.
The saved state has not yet included a user-commanded move.

On Linux x86_64 Debug Ninja, GCC 16.2.1 and Clang 22.1.8 each built the
integrated shell, SDL GPU probe and render-submission probe. The existing
data-root, render-pipeline, render-submission and native-shell CTests passed
4/4 per compiler on generated/repository data. Separate visible
Wayland/Vulkan `--require-runtime --visible` SDL GPU probes passed, rather
than skipped. Separate visible integrated shell processes used lawfully
external Steam `DATA`, `gendata.db`, `gametypes.db` and `Sol_Train.dmission`.
Both loaded the mission, executed `CQE_GAME_ACTIVE` and one `CQE_UPDATE`,
captured 28 production draw submissions and presented a frame. Each 64x64
readback found 171 changed pixels with checksum `13653293730956160287`.
A save archive derived from the lawful external mission was written under
`/tmp`; both compiler processes reloaded it and preserved the Forger owner
and exact position above.
A live desktop capture showed a planet and several small ships in the
800x600 mission window. Both shells then exited 6 at the explicit missing
input/loop gate. The same bounded optional media and presentation exclusions
listed below were active; required `IStreamer` stayed initialized and
full-screen `IVideoSystem` was excluded solely by `--skip-movies`.

Normal input selection and move dispatch, repeated source updates, and a
defined post-move mission checkpoint remain unexecuted. The next
production blocker is the original `ObjectComm` input owner: the SDL window
already forwards mouse events through `ShellWindowCallback` and the source
`ObjectList` consumes them, but `ObjComm.cpp` is not in the native link.
`IRenderPipeline::clear_buffers` and begin/end scene remain unsupported,
so the diagnostic frame uses the backend's explicit frame clear/present and
is not the complete `CQDoModal` contract. Full presets, sanitizers, native
Windows Debug/Final, module ABI and retail gameplay comparison remain open.
External data, screenshots and logs stayed outside the repository.

The next section is a historical snapshot superseded by this frame result.

## Original planet owner, game-active update and first object-render blocker (2026-10-10)

The original `Planetoid.cpp` factory now compiles and links into the Linux
shell under GCC 16.2.1 and Clang 22.1.8. `IFileSystem::CreateInstance` now
accepts bounded relative archive child paths with Windows separators, such
as the retail planet mesh `Planets\\gas2_LG.cmp`, while rejecting absolute
paths and traversal. The existing data-root check covers a generated nested
path and traversal rejection. This removed the planet placement assertion.
The original `Sol_Train.dmission` completes `Mission::Load`, counts one
`FAB!!S_Forger`, sends `CQE_GAME_ACTIVE`, and executes one `CQE_UPDATE` in
separate visible Wayland/Vulkan processes under both compilers. The required
source-profile `IStreamer` was initialized. Full-screen `IVideoSystem` was
omitted only by `--skip-movies`.

This diagnostic run explicitly omitted the unsupported planet resource
panel (`--skip-planet-ui`), animated talking-head portrait
(`--skip-animated-portraits`), subtitle UI (`--skip-subtitles`) and IAC
training speech (`--skip-training-speech`). The portrait option still routes
the speech file to the real `IStreamer` when speech is enabled; the default
SDL-only build correctly rejects this external IAC WAV, so this run disabled
speech to reach the gameplay owner. The original music fade request now
calls `IMusicManager::SetVolume(0)` through the native sound owner. Inherited
menu music was separately disabled by `--skip-mission-music`; the script's
`solarian_game14.wav` request was still issued. These exclusions concern
presentation and audio. They are not evidence of retail audiovisual parity.

One `CQE_UPDATE` completed, but a production `ObjectList::Render` pass
submitted no GPU scene, and the shell reported a presentation failure rather
than a frame. The Forger belongs to player 1 in active system 2 and is
visible to that player by ownership. Source `ObjectSelection::TestHighlight`
nevertheless sets its frame visibility to zero after projected-bound checks.
GDB measured the original Forger projection near screen `(-0.18, -99.62)`
with radius `17.48` in an 800-by-600 pane, even after the source camera's
`SetLookAtPosition` reported the Forger position. The camera's near distance
was about 160464 world units. The projection/camera initialization and the
CQ2/retail mission camera restoration need investigation before calling this
a Linux renderer defect. The documented Windows mission without units is
not a gameplay reference. The production object pass first also encountered
an unsupported decorative dust point draw; explicit `--skip-space-background`
now suppresses native dust rendering with the rest of that optional visual
layer. Unsupported `IRenderPipeline::clear_buffers` and scene begin/end
still reject calls, so the diagnostic pass uses the backend's implicit
color/depth clear on `Present`; it is not the complete `CQDoModal` contract.

The integrated GCC/Clang Debug Ninja shell and data-root targets built.
Separate lawfully external visible GPU processes each reached the source
game-active update and then failed before presentation; neither produced a
mission GPU frame. The focused `portability.data_root`,
`portability.render_pipeline` and `portability.native_shell` CTests passed
3/3 per compiler on generated/repository inputs. Direct GDB source stacks
identified the earlier planet-mesh path, the `Sol_T` speech and subtitle
calls, and the current Forger projection. No normal input movement,
deterministic mission checkpoint or save/reload ran. Full presets,
sanitizers, native Windows Debug/Final, module ABI and retail gameplay
comparison remain unperformed. The next blocker is the production camera
projection and visibility path for the loaded Forger, followed by native
input and a bounded mission render loop. External media and logs stayed
outside the repository.

## Source mission object chain through platform placement (2026-10-10)

The selected `Sol_Train.dmission` acceptance remains the visible, selectable
Solarion Forger, a normal-input move, a deterministic simulation checkpoint,
and save/reload of the same relevant state. This step did not reach any of
those gameplay gates. The original Jumpgate, Waypoint, Trigger, Gunboat and
build-support Platform factories now compile and link into the native shell
under GCC and Clang. During `CQE_GAME_ACTIVE`, the external training script
progressed from missing wormhole to missing `TriggerPoint`, then `Taos1`,
then `Acropolis` as those production providers were linked. The original
Windows `_localLoadString(0)` returns an empty buffer; the native string
service now matches that documented optional display-name behavior for
invisible mission triggers while still rejecting missing nonzero IDs.

`PLATFORM_INIT::loadPlatformArchetype` created a mesh owner but left its
`archIndex` at the constructor's `-1`, then supplied that invalid index to
blinkers and later instance code. The owner now checks that the mesh and
engine archetype exist and stores the actual engine index. This source fix
also affects the Windows path and is classified as CQ2/retail restoration,
separate from Linux UTF case and include repairs. Native Windows Debug/Final,
module ABI and Windows mission behavior remain unverified. The exact platform
factory class and data layouts were not changed.

Visible Wayland/Vulkan GCC 16.2.1 and Clang 22.1.8 Debug processes with
lawfully external `DATA`, `gendata.db`, `gametypes.db` and `Sol_Train`
now stop during original `Mission::Load` at `TPlatform.h:872`: the `Cocoonr`
platform has no loaded planet on which it can park. Earlier `ObjectList`
records showed absent `Planet Gas`, `Planet Earth` and `Planet Moon`
providers. The next concrete owner is the original `Planetoid.cpp` factory,
followed by another complete mission load and source game-active/update
attempt. The fact that previous combinations reached 100% does not mean
the present combination did. A Forger instance was counted in the preceding
factory combination, but the current platform-enabled load aborts before
the post-load count. Required `IStreamer` remains active; only full-screen
`IVideoSystem` and inherited menu music use explicit exclusions.

Validation for this source chain: both Linux x86_64 Debug Ninja integrated
shell targets built under GCC 16.2.1 and Clang 22.1.8. Direct original
Jumpgate/Waypoint/Trigger/Gunboat/BuildSupPlat compilation also succeeded
under both compilers after native include/template fixes. Separate visible
GPU external-data processes under each compiler aborted at the same required
planet-placement assertion (exit 134). The focused
`portability.data_root`, `portability.render_pipeline` and
`portability.native_shell` checks passed 3/3 per compiler on synthetic and
repository data after the platform link. No simulation tick, mission-owned GPU draw, selection/movement,
checkpoint or save/reload executed. Full presets, sanitizers, native Windows,
ABI and retail gameplay comparison were not run. External data and logs
remained outside the repository.

The sections below are historical snapshots. Their named jumpgate and
Forger blockers have been superseded by the current planet-placement result.

## Original Forger instance and next script blocker (2026-10-10)

The bounded `Sol_Train.dmission` acceptance remains: one player-owned Solarion
Forger must be visible and selectable, accept a move through normal input,
reach a repeatable mission checkpoint under the original simulation, and
preserve that state through save and reload. This step reached the first real
Forger instance but did not reach gameplay.

The original `Fabricator.cpp` registrar now links into `frontierx_native_shell`
with the original `SpaceShip`, `ObjMove`, `ObjControl`, `Damage`, extent, mesh,
blinkers, trail, explosion and debris owners. The native App also binds the
production `ANIM` and `HARDPOINT` services, implements the source hardpoint
enumeration/lookup contract, supplies axis-aligned 2D line drawing and links
the original Banker resource ledger. Unsupported native resource-menu display
and diagonal/smoothed 2D lines fail explicitly. Windows source branches and
serialized structures were not changed by these additions.

The first external run with the factory exposed an expensive native UTF path:
each nested texture lookup fully rescanned the 127 MB `objects` archive. A
validated, immutable disk-archive index now supports the production
`IFileSystem`'s directory and payload lookups. Its archive path resolution
also matches Windows ASCII case-insensitive behavior, rejecting ambiguous
case-folded entries. The standalone UTF metadata reader retains exact-case
behavior. Source `SSforger.cmp` differed in case from the external archive's
`ssforger.cmp`; before the fix `MeshManager::CreateMeshArch` returned null and
the original Forger extent initializer dereferenced a null mesh. The indexed
case-compatible path removed that failure and shortened the same mission run
to about two seconds. This is a Linux filesystem compatibility defect, not
evidence that the documented Windows no-units mission is valid.

Visible Wayland/Vulkan GCC 16.2.1 and Clang 22.1.8 Debug processes using
lawfully external `DATA`, `gendata.db`, `gametypes.db` and
`Sol_Train.dmission` then completed original `Mission::Load` at 100% and
counted **one** `FAB!!S_Forger` in the production `ObjectList`. Required
source-profile `IStreamer` initialized; only full-screen `IVideoSystem` was
omitted with explicit `--skip-movies`, and inherited menu music with separate
`--skip-mission-music`. The native shell's next source `CQE_GAME_ACTIVE`
transition aborts when `MScript::GetPartByName` requests
`Wormhole to Kasse (Solar)`. `ObjectList` has already logged no provider for
`JGATE!!Jumpgate`; linking and executing the original jumpgate owner is the
next concrete integration blocker. Other missing factories remain. The
one-update call placed after that transition has **not** executed, nor have
mission-owned GPU draw, normal selection/move input, deterministic checkpoint
or save/reload. The shell still returns failure at this unfinished boundary.
The existing Windows mission without units remains a separate CQ2/retail
restoration issue, not a gameplay reference.

Validation: both Linux x86_64 Debug Ninja `frontierx_native_shell` and
`frontierx_data_root_probe` targets built under GCC 16.2.1 and Clang 22.1.8.
The focused `portability.data_root`, `portability.render_pipeline` and
`portability.native_shell` checks passed 3/3 per compiler on synthetic and
repository inputs. The data-root check now reads a mixed-case generated UTF
path through the indexed production `IFileSystem`. Separate visible GPU
processes under both compilers reached the external-data Forger count; the
following game-active attempt aborted at the named required jumpgate with
exit 134. Earlier unindexed GCC runs were interrupted or timed out during
Forger texture loading and are not gameplay evidence. Native Windows
Debug/Final, cross-module ABI, full presets, sanitizers, retail gameplay
comparison and save/reload checks were not run. External media and diagnostic
logs remained outside the repository.

```text
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell frontierx_data_root_probe -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell frontierx_data_root_probe -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
```

The following sections are earlier snapshots; their factory-link and
missing-Forger statements are superseded by the evidence above.

## Forger object compile and first production link inventory (2026-10-10)

The original `Fabricator.cpp` now compiles as a Linux C++17 object under
GCC 16.2.1 and Clang 22.1.8. Non-Windows inherited-member qualifications
and exact interface includes close its immediate template compilation path.
The dormant CQ2 `ObjectWarp::setModelScale` helper references an absent
`IEngine::set_instance_property` API and undeclared `MODEL` owner; its Linux
body now aborts explicitly if invoked. Source search found no caller. The
Windows body remains unchanged. No persistent format or module ABI changed.

The `frontierx_app_fabricator_owner` object is in the native CMake graph.
A trial link into `frontierx_native_shell` failed: its first missing owners
include `SpaceShip<FABRICATOR_SAVELOAD, FABRICATOR_INIT>` explicit template
methods, `ObjectMove` methods, `BANKER`, `ANIM`, blinkers, `RayEllipse` and
`SMesh::load`. The real source owners `SpaceShip.cpp` and `ObjMove.cpp` both
stop at the Windows-only `pch.h` (`windows.h`) on a direct native compile.
The factory object was therefore kept outside the shell link. This is a
Linux source/link-closure blocker, separate from the CQ2/retail restoration
question of why the documented Windows mission has no units. The next step
is to bring `SpaceShip.cpp` and `ObjMove.cpp` through their native source
closure and link the required App services before repeating the actual
Forger postcondition.

Validation on Linux x86_64: strict direct GCC/Clang C++17 syntax and object
compilation of `Fabricator.cpp` succeeded. Both Debug Ninja trees built
`frontierx_app_fabricator_owner` and `frontierx_native_shell`; focused
`portability.data_root`, `portability.render_pipeline` and
`portability.native_shell` CTests passed 3/3 per compiler on
synthetic/repository inputs. The trial integrated shell link failed under
GCC at unresolved production symbols; Clang initially failed to instantiate
the old templates without target-local delayed parsing, then built the
separate owner with that option. No external-data mission or GPU runtime was
run in this step. No factory, unit, move, simulation checkpoint or save/load
executed. Full presets, native Windows Debug/Final, ABI and sanitizers were
not run.

```text
cmake --build /tmp/frontierx-mission-gcc --target frontierx_app_fabricator_owner frontierx_native_shell -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_app_fabricator_owner frontierx_native_shell -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
```

The section below records the preceding source-closure snapshot.

## Forger factory source closure remains open (2026-10-10)

The current production `Sol_Train` acceptance and missing-Forger runtime
result remain as recorded below. Source review confirmed that the supported
Windows `Conquest.vcxproj` includes original `Fabricator.cpp`; its static
`_fabricatorship` registers `FabricatorFactory` with the production
`ObjectList` during global startup. The native shell does not link that
owner. This missing native registration is distinct from the separate
CQ2/retail restoration problem represented by the Windows mission with no
units; that Windows observation is not a correct gameplay baseline.

This step prepared the original Forger source's native include order and
case-sensitive names, qualified several inherited template node types and
members only on the non-Windows branch, and kept editor insertion explicitly
unavailable. The direct GCC 16.2.1 `Fabricator.cpp` syntax attempt progressed
from incomplete `SaveNode`/`LoadNode` types to instantiated selection code;
it still fails in dependent-base lookups. Direct Clang 22.1.8 with delayed
template parsing similarly fails in the selection template. These attempts
used `-fpermissive` for GCC and `-fdelayed-template-parsing -fms-extensions`
for Clang to inventory the inherited source closure. Neither is an accepted
production compile, and `Fabricator.cpp` remains unlinked. The next concrete
step is to complete standards-conforming template qualification without
changing node layout, then build/link the factory and repeat the external
Forger postcondition. Further object factories and gameplay still follow.

Both existing Linux x86_64 Debug Ninja `frontierx_native_shell` targets
rebuilt under GCC and Clang after the source preparation. The focused
`portability.data_root`, `portability.render_pipeline` and
`portability.native_shell` CTests passed 3/3 per compiler with
synthetic/repository inputs; `git diff --check` passed. There was no new
external-data mission execution in this source-closure step. The preceding
visible retail-data runs remain valid only through mission loading and the
missing-Forger rejection. No factory instance, mission draw, move command,
simulation checkpoint or save/reload ran. Native Windows Debug/Final,
module ABI, full presets and sanitizers were not run.

```text
g++ -std=c++20 -fpermissive -fmax-errors=6 -w -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -fsyntax-only Code/App/Src/Fabricator.cpp
clang++ -std=c++17 -fdelayed-template-parsing -fms-extensions -ferror-limit=4 -w -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -fsyntax-only Code/App/Src/Fabricator.cpp
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
```

The section below is the preceding mission-load runtime snapshot.

## Production Sol_Train load completion, before playable objects (2026-10-10)

The bounded acceptance remains the selected Solarion Forger's visible
selection and move through normal input, a repeatable simulation checkpoint,
and save/reload of its position and mission state. The native shell now mounts
the lawful external `Speech` and `MSpeech` archives alongside Music and SFX,
requires the source-profile `IStreamer`, and binds native App owners of the
existing `ISoundManager` and `ITeletype` interfaces. The sound owner sends
ordinary speech to `IStreamer`; unsupported actor highlighting, video and
volume mutation fail explicitly. Display teletype remains unsupported, while
its saved last-ID contract is available to `MScript::Load`. No actual speech
or teletype presentation occurred in this run. The source `IVideoSystem` was
omitted only by `--skip-movies`. Optional inherited menu music was disabled
separately by `--skip-mission-music`; its `single_player_menu.wav` streamer
open failed without that explicit option, and the file/codec cause is not yet
established. Normal movement and simulation audio still need validation.

The native `ISystemMap::GetSysMapRect` now derives its bounds from the source
Sector rectangle with the source map padding formula. A visible GCC run then
hit a divide-by-zero in `FogOfWar::UpdateFog`: the native shell had omitted
the source `CQE_ENTERING_INGAMEMODE` transition that initializes the fog
texture dimensions. Sending that event before `IMission::Load` removed the
failure. These are Linux composition repairs; they do not restore missing
CQ2/retail object behavior.

Visible Wayland/Vulkan GCC 16.2.1 and Clang 22.1.8 Debug processes with
lawful external `DATA`, `gendata.db`, `gametypes.db` and
`Sol_Train.dmission` each reported 100% from the original `Mission::Load`.
The shell then inspected the production `ObjectList` and rejected the mission
with exit 6 because it contained no `FAB!!S_Forger` instance. An earlier
count found zero spaceship instances of any type. `ObjectList`
logged absent providers
for `FAB!!S_Forger`, `GBOAT!!S_Taos`, planet, jumpgate, platform, waypoint
and nugget archetypes. In particular no Forger was instantiated, so the
100% loader result cannot satisfy the visible-unit gate. The nugget owner
also reported missing `ionNugget.anm` and `cygnusNugget.anm`; determine
whether those are archive lookup/restoration defects separately from Linux
factory registration. The documented Windows mission without units is not a
valid gameplay reference. The next concrete blocker is compiling and linking
the original Forger/other required object factories, then rejecting a mission
load that lacks the mandatory own unit before attempting input/render/save.
An initial direct GCC/Clang syntax attempt on original `Fabricator.cpp`
reached its inherited template stack and failed at dependent nested types
(`SaveNode`, `LoadNode`, etc.) and further source declarations. It is not in
the native link. Case-corrected includes made this failure observable; no
factory capability is claimed from that source preparation.

Both Debug Ninja `frontierx_native_shell` targets built. Focused
`portability.data_root`, `portability.render_pipeline` and
`portability.native_shell` CTests passed 3/3 under each compiler; these checks
use synthetic/repository inputs, not mission gameplay. The separate external
GPU runs are retail-data runtime evidence for startup and load only, and
exited 6 at the explicit missing-Forger postcondition. No mission-owned GPU draw, selected unit, movement,
simulation checkpoint or save/reload was executed. Full CTest presets,
sanitizers, native Windows Debug/Final, cross-module ABI and Windows retail
gameplay comparison were not run. The external data and runtime logs stayed
outside the repository. This section supersedes the historical 88% snapshot
below.

Focused build/check commands were:

```text
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
git diff --check
```

The visible GPU runs used each tree's `frontierx_native_shell`, matching
generated shaders, the source `Conquest.ini`, repository material and
hotkey inputs, and lawful external `DATA`, `gendata.db`, `gametypes.db` and
`Sol_Train.dmission`. Both selected `--mission-engine --mission-start
--mission-gpu`, isolated `/tmp` save/settings roots and explicit
`--skip-movies --skip-notification-cue --skip-sector-map
--skip-space-background --skip-unit-labels --base-textures-only
--skip-legacy-lighting --skip-mission-music`. Both processes ran with a
visible Wayland window and dummy SDL audio device, and exited 6 at the
Forger check. `--skip-training-speech` was available but not selected;
the source training script did not request playback before this boundary.

## External Sol_Train load reaches the script boundary (2026-10-10)

The chosen acceptance is still the Forger movement and save/reload checkpoint
below. A real texture blocker was resolved: the external Anim2D texture uses
RGB565 with a separate 8-bit alpha plane. The native `TextureLibrary` now
selects RGBA8 for that source, and the production pipeline preserves the
alpha bytes. The existing render-pipeline CTest checks four exact RGBA texels
and native release. The texture's nine source mip levels exceed the current
single-level SDL GPU texture contract. Explicit `--base-textures-only` sets
the production library's `ITL_STATE_TEXTURE_LOD_LOAD` to zero before mission
load; it retains the highest-resolution source image and documents the visual
quality limit. Without this option the nine-level create still fails.

With that option, the original `ObjectList` progressed through its data and
instances. The explicit `--skip-sector-map` path now returns the original
failure sentinel for map-only BMP icon registrations; all map draw calls
remain unsupported. The original `VertexBuffer.cpp` owner is linked, so
`MeshRender` registrations have their production list. Native device-loss
restoration aborts until a GPU restoration contract exists. The App's 3D flag
is enabled after global startup for mesh instance initialization. Explicit
`--skip-legacy-lighting` leaves light objects in simulation while omitting
legacy ambient/dynamic GPU light states unsupported by the current shader;
without it those calls abort. These reductions affect presentation and must
not be confused with retail visual parity.

Visible Wayland/Vulkan GCC 16.2.1 and Clang 22.1.8 Debug processes with
lawful external Steam `DATA`, `gendata.db`, `gametypes.db` and the selected
`Sol_Train.dmission` both reached 88% of `Mission::Load`. Both aborted at
the source `MScript::Load` prerequisites for `Sol_T.dll`. Its source
`SolTrain_Start` program and parser are registered, but `SOUNDMANAGER` and
`TELETYPE` were null at that call. The original `scripting.cpp` owner now
compiles, links and initializes under both compilers; `SCRIPTING` was non-null
in the final repeat runs. Direct source review and a
GCC syntax attempt found that the original `SoundManager.cpp` remains outside
the native closure at Win32 UI/timing/event and source-header boundaries;
`Teletype.cpp` also owns Windows UI and has not been compiled natively.
The earliest next gate is to initialize the sound and teletype owners and run
the real training script. ObjectList also logged missing production factories
for several mission archetypes, including `FAB!!S_Forger`, plus absent
`ionNugget.anm` and `cygnusNugget.anm`; those are independent mandatory
gameplay/data-restoration gates before claiming a unit. Their cause needs
separate Linux registration versus CQ2/retail-data analysis. The Windows
mission without units is not a correct reference result.

Both `frontierx_native_shell` Linux Debug targets built. Existing focused
`portability.data_root`, `portability.render_pipeline` and
`portability.native_shell` CTests passed 3/3 per compiler; the first two use
synthetic/repository inputs and the last tests shell contracts. The separate
external-data GPU runs failed with exit 134 at 88%; their executed load path
is retail-data runtime evidence only up to that boundary. No mission frame,
visible Forger, input movement, simulation checkpoint or save/reload occurred.
Full presets, sanitizers, native Windows Debug/Final, module ABI and retail
gameplay comparison were not run. The runtime scripts and logs remained in
`/tmp`, and no retail input was added to the repository.

```text
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell frontierx_render_pipeline_probe -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell frontierx_render_pipeline_probe -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.render_pipeline$|^portability.native_shell$|^portability.data_root$' --output-on-failure
```

The section immediately below is the preceding 25% status snapshot.

## Production Sol_Train load through ObjectList (2026-10-10)

The selected acceptance remains the bounded Solarion Forger movement and
save/reload checkpoint below. The native shell now links the original
`Sector.cpp` and `SpaceEnv.cpp` registrars, completes
`CreateGlobalComponents`, initializes the source particle, mesh, texture and
material owners in the Windows startup order, and calls the production
`IMission::Load("Sol_Train.dmission", SPMAPDIR)`. The `Mission.cpp` bridge only
forwards to that vtable; it does not implement a second loader. The original
Sector terrain, system and save/load methods remain in the linked owner.
The `ISector` methods implemented only by Sector are now pure virtual without
reordering slots; native Windows Debug/Final and module ABI checks are still
required for that shared header.

`--skip-sector-map`, `--skip-space-background` and `--skip-unit-labels` are
explicit limits of this one-system slice. The first omits the Windows GDI
sector-map view and toolbar; an attempted native GDI draw/input call aborts.
The second uses the production `SpaceEnvironment` owner's existing effect
settings to omit decorative background rendering; missing vertex-buffer
ownership fails if background effects are enabled. The third omits GDI unit
labels; `GetUnitFont` returns failure when no provider is present, and the
debug nugget caller now checks that result. The Forger's game geometry and
normal main-view input remain required. Required source `IStreamer` remains
active with Music and SFX; only full-screen `IVideoSystem` is omitted by
`--skip-movies`, and the notification cue by its separate explicit option.

The production `RenderBatcher` supplies `BATCH` and `CQBATCH` to
`ObjectList::Load`; the source-profile primitive remains the SDL GPU
receiver. The App `PrimitiveBuilder2` binding is removed before its owner
teardown. A core backtrace found an earlier exit crash in the global
DOCUVIEW `SymbolTable` destructor freeing through the native heap after its
static teardown; the process heap now outlives static DOCUVIEW users. A
separate backtrace located a dangling App PrimitiveBuilder binding at exit.
Both lifecycle failures stopped after those ownership changes. The native
`ISystemMap::SetRect` now stores the current and padded system bounds using
the original source formula; other unsupported map methods still fail
explicitly. These are Linux integration repairs, not evidence of CQ2/retail
gameplay parity.

Visible Wayland/Vulkan processes in the GCC 16.2.1 and Clang 22.1.8 Debug
trees used lawful external Steam `DATA`, `gendata.db`, `gametypes.db`, the
repository material directory and source profile, and isolated `/tmp` save
and settings roots. Both initialized required `IStreamer`, parsed the App
and mission schemas, entered `Mission::Load`, executed `Sector::Load` and
`ISystemMap::SetRect`, and reached the production ObjectList load at 25%
progress. Both then exited 134 at a required Anim2D texture assertion:
`astnugget.16.tga0` failed to load, leaving a zero render texture. Before
that failure both logged missing object factories for `MISSION!!WAYPOINT`,
`PLATJUMP!!M_JumpPlat`, and `Planet Gas!!Gas 2`. The archive contents were
not copied into the repository. Whether the texture is absent, encoded in an
unsupported format or affected by CQ2 data restoration remains unverified;
the three factories are a native link/registration gap until their original
owners are checked. The next concrete work is to identify the texture-load
failure in the active `TextureLibrary`/pipeline, then link those object
factories and retry the same mission. No Forger was instantiated or drawn,
normal movement, simulation checkpoint, save/reload or mission GPU frame
was observed. The Windows training mission with no units is not accepted as
the gameplay reference.

Both `frontierx_native_shell` Debug targets built and the focused
`portability.data_root` and `portability.native_shell` CTests passed 2/2 per
compiler. The direct original Sector and SpaceEnvironment syntax checks
passed under both compilers after the native include work. The separate
visible external-data mission runs are failures at 25% and count only as
runtime evidence up to that point. Full CTest presets, native Windows
Debug/Final, module ABI, sanitizers, retail gameplay and cross-platform
determinism checks were not run for this step.

Focused commands executed (the visible shell command used the same flags
for both compiler trees and compiler-specific shader paths):

```text
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell frontierx_data_root_probe -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell frontierx_data_root_probe -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.native_shell$|^portability.data_root$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.native_shell$|^portability.data_root$' --output-on-failure
```

## Sector source closure inventory (2026-10-10)

The current blocker is still the production `Sector` registrar, reached by
`CreateGlobalComponents` before mission reload. Source review found a missing
direct `M_OBJCLASS` definition in `Sector.h` and a missing public declaration
for `CreateTerrainMap`, which `Sector::Load` and `Sector::New` call. Those
declarations now come from the existing mission enum and terrain header;
their numeric IDs and data layouts did not change. The original `Sector.cpp`
now has a native include path with exact on-disk spelling and no unused
`BaseHotRect`/`DSounds` include on Linux. Its Win32 source path remains in
place. This prepares the original owner; it is **not** linked into the native
shell yet.

The falsifiable check was direct syntax compilation of `Sector.cpp` with the
native App and Libs include directories. GCC 16.2.1 progressed into the
`Sector` class and reported 181 errors, dominated by the Windows sector-map
UI (`HPEN`, `HBRUSH`, `MSG`, `TOOLBAR`, menu/Win32 messages) and several
pointer-to-32-bit event conversions. Clang 22.1.8 stopped after its default
20-error limit at the same UI boundary. Both commands failed; neither is a
passing Sector compile. The original `Sector::Load`, `Save`, supply and
system state methods have not executed. The next change must isolate or
implement the sector-map display and resource/input contract without making
required mission methods return fake success. Then link the registrar and
repeat the visible external-data startup.

The affected Debug `frontierx_native_shell` target rebuilt under GCC 16.2.1
and Clang 22.1.8 in `/tmp/frontierx-mission-{gcc,clang}`, and the existing
`portability.native_shell` CTest passed 1/1 for each. Those checks execute the
shell but do not compile or run `Sector.cpp`, a mission, movement or save/load.
No new probe was added. Native Windows Debug/Final, module ABI and retail
gameplay checks were not performed.

```text
g++ -std=c++17 -fsyntax-only -ICode/App/Src -ICode/App/Src/Include -ICode/App/DInclude -ICode/Libs/Include -ICode/Libs/Src/Include Code/App/Src/Sector.cpp
clang++ -std=c++17 -fsyntax-only -ICode/App/Src -ICode/App/Src/Include -ICode/App/DInclude -ICode/Libs/Include -ICode/Libs/Src/Include Code/App/Src/Sector.cpp
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.native_shell$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.native_shell$' --output-on-failure
```

## Production TerrainMap compile boundary for Sector (2026-10-10)

`Sector::Load` and `Sector::New` call the original `CreateTerrainMap`;
`Sector` and the selected Forger's normal movement therefore need this
owner. A temporary GCC syntax check of `Sector.cpp` first confirmed that
the source still depends on the Windows PCH, case-insensitive includes,
GDI drawing, `BaseHotRect`, menu/toolbar/viewer services, and terrain-map
factories. This is a Linux closure problem, not evidence that the retail
training mission has no units. No temporary compile source was added to
the repository.

The original `TerrainMap.cpp` now has a separate native object target linked
into the GPU shell. Its
Linux include path supplies the existing render types, and its local search
step uses fixed-width `S32` coordinates in place of Win32 `POINT`. Dependent
template types have the required `typename`; the public header gains a
forward declaration and a factory declaration. No saved map layout, movement order or Windows path
was changed. The target uses `-ffp-contract=off` under GCC/Clang.

On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each configured and built
`frontierx_app_terrain_map_owner` in the Debug SDL3 GPU trees
`/tmp/frontierx-mission-{gcc,clang}`. Both rebuilt
`frontierx_native_shell`, and the existing `portability.native_shell`
CTest passed once per compiler. Separate visible Wayland/Vulkan runs on the
same lawful external training data entered App startup and again stopped at
the explicit missing-`Sector` abort (exit 134) under both compilers. The
terrain object is linked but neither `CreateTerrainMap` nor pathfinding
executed. The next gate is porting and linking the production
`Sector.cpp` registrar and its required terrain, resource and input owners,
then rerunning the lawful external training mission. Native Windows
Debug/Final, cross-module ABI, retail gameplay, mission-owned GPU rendering and
save/reload checks were not performed for this step.

Focused commands executed (the external-data process used the same options
documented in the preceding startup record, with compiler-specific shader
paths):

```text
cmake --build /tmp/frontierx-mission-gcc --target frontierx_app_terrain_map_owner -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_app_terrain_map_owner -j4
cmake --build /tmp/frontierx-mission-gcc --target frontierx_native_shell -j4
cmake --build /tmp/frontierx-mission-clang --target frontierx_native_shell -j4
ctest --test-dir /tmp/frontierx-mission-gcc -R '^portability.native_shell$' --output-on-failure
ctest --test-dir /tmp/frontierx-mission-clang -R '^portability.native_shell$' --output-on-failure
```

## Production App startup attempt and Sector blocker (2026-10-10)

The selected acceptance remains the `Sol_Train.dmission` Forger movement
checkpoint below. The explicit `--mission-start` mode requires the supplied
profile, visible `--mission-gpu`, external GenData/GameTypes and a writable
save root. It binds the production GameSystem's light, texture, material,
mesh, vertex-buffer and string-table aggregates plus user defaults to the
App globals, then calls the original `CreateGlobalComponents`. This is an
attempt to execute the real registrar; it does not claim a loaded mission.

The diagnostic mission-schema preflight cannot run before this registrar on
the same `IViewConstructor`: `ParseNewMemory` clears its active symbol manager,
and a subsequent `Mission::Startup` `ParseMemory` then reported undefined
fundamental types. The start mode leaves primary schema parsing to
`Mission::Startup`; the existing data-diagnostic mode retains its parser
checks. No persistent schema or module ABI type changed.

GCC 16.2.1 and Clang 22.1.8 Debug `frontierx_native_shell` targets built in
fresh `/tmp/frontierx-mission-{gcc,clang}` Ninja trees with SDL3 GPU enabled
and FFmpeg audio disabled. The existing `portability.native_shell` CTest
passed under both compilers, including rejection of incomplete
`--mission-start` arguments. Separate visible Wayland/Vulkan processes used
the lawful external Steam `DATA`, `gendata.db` and `gametypes.db`, repository
`Data/mat`, the source `Conquest.ini` and hotkeys, isolated `/tmp` save and
settings paths, and explicit `--skip-movies` and
`--skip-notification-cue`. Both initialized required `IStreamer`, opened
`Sol_Train.dmission`, bound the profile renderer to SDL GPU, entered
`CreateGlobalComponents`, and parsed the App `Data.i` primary schema. Both
then aborted with `Native light startup requires the production Sector owner`
(exit 134). A GCC gdb backtrace located the previous null dereference in
`Lights::OnUpdate` at `SECTOR->GetCurrentSystem`; the native light owner now
reports the missing mandatory dependency explicitly. The command sandbox
could not start the GPU, so these were separate visible runs.

The next concrete integration blocker is the original `Sector.cpp` registrar
and its startup dependencies. Source review shows it still includes the
Windows PCH and uses `MENU`, `TOOLBAR`, viewer and terrain-map services. Port
that owner behind its existing App contracts, then retry the same startup
before attempting `IParticleManager::InitParticles`, `IMeshManager::InitManager`
and `Mission::ReloadMission`. No mission object, visible Forger, selection,
movement, simulation checkpoint or mission save/reload executed. Native
Windows Debug/Final rebuilds, cross-module ABI checks and retail gameplay
comparison were not performed.

`IStreamer` is required by the App's Music/SFX startup and remains active.
`IVideoSystem` is omitted only with the explicit `--skip-movies` flag for the
selected checkpoint, which has no full-screen cinematic call; movie mode,
animated messages and audio remain required. The source-reviewed movie policy
is recorded below.

Current combined-link closure, 2026-10-03: a native App 2D owner now
implements the three Camera/DrawAgent calls missing from the selected
mission link through the production PrimitiveBuilder and render vtables.
The existing App material check executed full-screen fill/hash callbacks
under GCC and Clang; both combined App/script diagnostic links now
succeed. Both diagnostic executables still report `mission not started`
with lawful external training data. See
[the focused record below](#native-app-2d-and-combined-mission-link-2026-10-03).
Entries below are historical snapshots.

Current 2D render-boundary step, 2026-10-03: the native primitive now
forwards orthographic projection and a bounded viewport through the
existing projection/viewport callbacks; SDL GPU applies the viewport
and scissor during presentation. GCC and Clang executed the extended
CPU contract check and full Debug graphs, but the command sandbox
skipped GPU tests; separate visible Vulkan runs exercised the viewport
under both compilers. This does not provide `OrthoView` or the original
`DrawAgent` rectangles to the mission link, and no gameplay acceptance
gate passed. See [the focused record below](#native-orthographic-and-viewport-contract-2026-10-03).
Entries below are historical snapshots.

Current production owner closure, 2026-10-03: Blast, App light,
MapGen, MorphMesh, the original fleet pool and native App profile
loading now compile under GCC and Clang. The profile wrapper ran in
both existing synthetic shell checks; lawful external training-data
shells still stopped before mission startup. The combined diagnostic
link now lacks exactly three production 2D/camera symbols. No
gameplay acceptance gate passed. See
[the focused record below](#production-light-profile-and-fleet-closure-2026-10-03).
Entries below are historical snapshots.

Current native SFX and field link gate, 2026-10-03: the native App
`ISFX` now executes source-mapped `BUTTONPRESS1` with the required
`IStreamer` and lawfully external `Sol_Train` effect under GCC and Clang.
The original `MineField` and `AntiMatter` objects compile under both.
The temporary combined mission link still has 22 unresolved references
across 12 symbols. The shell has not called `Mission::Reload`; no visible
unit, movement, simulation checkpoint or mission save/reload ran. See
[the focused record below](#native-isfx-and-field-object-closure-2026-10-03).
Entries below are historical snapshots.

Current App command/particle binding gate, 2026-10-03: production
`CommTrack.cpp` now compiles under GCC and Clang with its Win32
settings dialog excluded from native builds. The ObjectList's Win32
part-debug-menu registration is also excluded. The native shell binds
the active system container's required `IParticleManager` to the App
global after validating the selected mission source; GCC/Clang shell
process tests and lawful external training-data startup ran. The
temporary combined mission link still fails on 32 references across
20 symbols, led by `FIELDMGR` and `EFFECTPLAYER`. The actual mission
and all five gameplay acceptance gates remain open. See
[the focused record below](#production-command-tracker-and-app-particle-binding-2026-10-03).
Entries below are historical snapshots.

Current selected-mission owner boundary, 2026-10-03: the lawful external
`Sol_Train.dmission` has zero `PLAYERAI/Player02..Player08` saved
directories. The existing mission-filesystem check now rejects an
unexpected saved AI owner for this bounded path. Native AI factory
calls abort explicitly if encountered; the full production AI is still
unported. The App `DisableTextures` render state executes in the
existing CPU material check under GCC and Clang, and production
`UnbornMeshList.cpp` compiles under both. A temporary combined
mission link still fails on 46 references across 23 symbols, led by
`COMMTRACK` and `MENU`. No mission startup or gameplay acceptance
condition passed. See [the focused record below](#selected-mission-ai-and-app-render-owner-boundary-2026-10-03).
Entries below are historical snapshots.

Current save-writer and mission-link check, 2026-10-03: the checked-in
native writable UTF archive, `IUTFWriter::Flush`, `SAVEDIR` binding and
explicit `--save-root` option supersede the older save-encoder-only
entries below. A fresh GCC build exposed a failed first flush; the
temporary-path check now accepts a missing path while rejecting an
existing file or symlink. The existing data-root round trip executes
under GCC and Clang. The native shell still only parses the selected
mission. A diagnostic link with the compiled App/script objects and
native `PrimitiveBuilder2` remains unresolved, and no gameplay
acceptance gate has passed. See [the focused record below](#writable-save-flush-and-current-mission-link-check-2026-10-03).
Entries below are historical snapshots.

Latest UTF-save-format gate, 2026-09-27: the native platform now
encodes a bounded deterministic UTF 1.01 tree. GCC/Clang executed the
existing data-root check, reopened generated bytes through the
production reader, and separately read the lawful external training
mission. `SAVEDIR` and `IUTFWriter` are still absent; no actual save,
reload or mission startup ran. See
[the focused record below](#native-utf-save-format-encoder-gate-2026-09-27).

Latest mission-message gate, 2026-09-27: native `SCROLLTEXT` accepts
source App and direct wide messages when the optional beacon cue is
explicitly disabled. Both external-data compiler shells executed its
display branch and still reported "mission not started". The
combined App/script link lacks 28 symbols, led by writable save
storage. See [the focused record below](#native-scrolling-message-and-explicit-cue-policy-gate-2026-09-27).

Latest native status/hint gate, 2026-09-27: production App
`IResourceClient` ownership and text interfaces now have native
`STATUS`/`HINTBOX` owners attached to the shell's EventSystem. GCC and
Clang executed source-string display/reset with the lawful external
training mission; it remains unstarted. A diagnostic combined link
still lacks 29 symbols, led by scrolling text and writable save
storage. See [the focused record below](#native-status-and-hint-resource-client-gate-2026-09-27).

Latest production-music-owner gate, 2026-09-27: the original App
`MusicManager` now builds and links under GCC and Clang. Both lawfully
external training-mission shells executed its native construction,
event subscription, enable and teardown after `IStreamer` startup.
Its fixed-width Linux music load/save branch has compiled but has not
run through `Mission::Reload`; neither `Sol_T` music playback nor the
mission itself started. See
[the focused record below](#production-music-manager-owner-startup-gate-2026-09-27).

Latest mission audio-ownership gate, 2026-09-27: the GPU shell now
retains the required source-profile `IStreamer` and external `Music`
source in the App globals, initializes SDL audio after creating its
window callback, and releases aggregate references before
`GameSystem`. GCC/Clang external training-mission shell runs exited
zero with SDL dummy audio and explicitly skipped full-screen movies;
they still reported "mission not started". See
[the focused record below](#selected-mission-audio-ownership-gate-2026-09-27).

Latest selected-music save-layout gate, 2026-09-27: a Linux-only
fixed-width converter now decodes and encodes the four 44-byte Win32
music songs in the lawful external `Sol_Train.dmission`, retaining
playlist flags, volume and names while clearing saved stream handles.
GCC/Clang executed the existing parser probe against that mission.
`MusicManager` remains outside the native mission link; no gameplay
acceptance gate passed. See
[the focused record below](#selected-music-save-layout-conversion-gate-2026-09-27).

Latest selected-training music gate, 2026-09-27: the selected
`solarian_game14.wav` IAC file now decodes and briefly plays through
the FFmpeg-enabled native `IStreamer` under GCC and Clang with dummy
audio. The external mission's saved music data has a 176-byte Win32
schema; `MusicManager` still does not link or run, so no gameplay
acceptance gate passed. See
[the focused record below](#selected-training-music-decode-and-disk-layout-gate-2026-09-27).

Latest selected-system-map fresh-load gate, 2026-09-27: a native
`ISystemMap` owner executes the selected external mission's absent
system-map save child, matching the original fresh-load branch. GCC
and Clang ran that call through the existing filesystem check. The
shell links the map owner but does not start the mission; no gameplay
acceptance gate passed. See
[the focused record below](#selected-system-map-fresh-load-gate-2026-09-27).

Latest combined mission-script heap link gate, 2026-09-27: the native
`Sol_T` translation unit now uses the host heap when statically linked.
A temporary GCC link of the shell and compiled mission/script objects
has no duplicate definitions, but still lacks 38 distinct production
symbols. No executable mission path or gameplay acceptance gate passed.
See [the focused record below](#combined-mission-script-heap-link-gate-2026-09-27).

Latest selected-script localization and fresh-loader gate, 2026-09-27:
the repository `Sol_T.rc` now produces native counted objective/subtitle
strings, and `MScript` has a bounded native fresh-load branch for the
statically registered selected script. GCC/Clang executed the existing
schema/string check and read-only external mission-schema check. The
shell does not link `MScript` or `Sol_T`, so the mission still does not
start. No gameplay acceptance gate passed. See
[the focused record below](#selected-sol_t-localization-and-fresh-loader-gate-2026-09-27).

Latest production script-owner build gate, 2026-09-27: the original
`MScript.cpp` now compiles under GCC and Clang as a native object.
Windows-only editor dialogs are isolated, and native script schema
validation rejects missing parser/data/localization. Module loading,
string resources and full-screen script video still abort explicitly;
the owner is not linked to the shell and the external training mission
still reports "mission not started". No gameplay gate passed. See
[the focused record below](#production-mscript-owner-build-and-required-resource-gates-2026-09-27).

Latest selected-script resource gate, 2026-09-27: the `Sol_T` native
object target now depends on an embedded copy of its generated parser
schema. GCC/Clang production-parser checks compare those bytes with
the generated file and validate the selected save layout. The shell
still reports "mission not started" on the lawful external training
mission. No gameplay acceptance condition passed. See
[the focused record below](#selected-sol_t-native-schema-resource-gate-2026-09-27).

Latest selected-program compile and schema gate, 2026-09-27: the original
`Sol_T.cpp` and campaign helper compile as native object files under GCC
and Clang. Both compilers generate `DataDef.i` from the original source;
the production `IViewConstructor2` parses it with the selected
`MissionData` and movement-checkpoint offsets. The object files are not
linked to `MScript` or the shell. Both external-data shell runs still
report "mission not started"; none of the gameplay acceptance gates
passed. See [the focused record below](#selected-sol_t-program-and-schema-build-gate-2026-09-27).

Latest selected-script data gate, 2026-09-27: the original `Sol_T`
`DataDef.h` now compiles with native include spelling. The existing
layout check confirms a source-derived 292-byte `MissionData`, fixed
Forger/Taos/Acropolis offsets, and a four-byte saved movement-state
enum under GCC and Clang. A read-only check of the lawfully external
`Sol_Train.dmission` found no `Script/GlobalData`: its first run must
initialize script state through a native `Sol_T` program. The
production script program and mission still do not run. See
[the focused record below](#selected-sol_t-script-data-and-fresh-start-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest training-script reference ABI gate, 2026-09-27: native
`MPartRef` retains its 16-byte Win32 script-save stride instead of
placing two 64-bit debugger-only pointers inside the selected
mission's `MissionData`. The existing layout check executes zero-byte
initialization under GCC and Clang. This does not build `MScript.cpp`
or the `Sol_T` campaign module; no mission started. See
[the focused record below](#training-script-mpartref-save-layout-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest spatial-iteration owner gate, 2026-09-27: the original
`ObjMapIterator.cpp` now builds in the production `ObjMap` object
target under GCC and Clang. `ObjectList` and `MScript` call this owner
to traverse mission objects, but neither the iterator nor its map
executed in the successful shell. Both lawfully external training
mission shell runs still report "mission not started". See
[the focused record below](#production-spatial-iterator-build-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest production camera-manager gate, 2026-09-27: the original
`MovieCameraManager` now compiles into the native shell under GCC and
Clang. The existing offline App startup check actually registers it,
delivers `CQE_MISSION_CLOSE`, and cleans it up. The same check now runs
production `Save`/`Load` through a writable test filesystem: the
44-byte Win32 position record and one queued spot round-trip byte for
byte, while malformed lengths and flags fail. This is synthetic App
runtime, not full mission persistence. A temporary combined mission
link still lacks 50 distinct
production symbols, and both lawfully external `Sol_Train.dmission`
shells still report "mission not started". See
[the focused record below](#production-movie-camera-manager-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest camera-object template gate, 2026-09-27: the native
`TObjFrame`/`TObjTrans`/`TObjRender` header chain used by
`MovieCamera.cpp` now builds in the production App object-list target
under GCC and Clang. A direct camera syntax diagnosis moves past
those template errors and next stops in Win32 `BaseHotRect`/editor
contracts; no camera owner or mission is linked or executed. See
[the focused record below](#camera-object-template-closure-2026-09-27).
No gameplay acceptance condition has passed.

Latest production selection-ring table gate, 2026-09-27: the original
`StacticInit.cpp` now executes its 80-point `circleLines` initializer
in the existing math check under GCC and Clang. `BaseObject` consumes
that table for selection and range rings. A temporary combined App
link resolves `circleLines` but still lacks 51 distinct production
symbols; the successful external-data shell still reports "mission
not started". See
[the focused record below](#production-selection-ring-table-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest simulation event-owner build gate, 2026-09-27: the original
`EventScheduler.cpp` now compiles under GCC and Clang and has a native
`SCHEDULER` slot. `ObjectList::Update` uses its delayed-damage queue;
a temporary combined App link resolves `SCHEDULER` but still fails
on 52 distinct production symbols. The successful external-data
shell still reports "mission not started" and does not execute this
owner. See
[the focused record below](#production-event-scheduler-build-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest spatial-object owner build gate, 2026-09-27: the original
`ObjMap.cpp` now compiles under GCC and Clang, with the native App
`OBJMAP` slot defined. `Sector::Load` requires its `Init` method and
mission objects use its spatial index. A temporary combined App link
resolves the previous `OBJMAP` references but still fails on 53
distinct production symbols; the successful shell does not include or
execute `ObjMap.cpp`. Lawfully external `Sol_Train.dmission` still
reports "mission not started". See
[the focused record below](#production-spatial-object-map-build-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest production fog-owner build gate, 2026-09-27: the original
`FogOfWar.cpp` owner now compiles under GCC and Clang, retaining its
mission `New/Save/Load`, update and render implementations. A
temporary combined App link including it still lacks 54 distinct
production symbols; the successful external `Sol_Train.dmission`
shell still reports "mission not started". See
[the focused record below](#production-fog-of-war-native-build-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest production nugget-owner build gate, 2026-09-27: the original
`Nugget.cpp` manager and factory now compile as Linux x86_64 objects
under GCC and Clang, including their simulation and save/load methods.
The owner remains outside the successful shell: a combined App link
still lacks 53 distinct production symbols after adding its real
dependencies. External `Sol_Train.dmission` shells still report
"mission not started". See
[the focused record below](#production-nugget-manager-native-build-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest native cursor-service gate, 2026-09-27: a platform-backed
`ICursorResource` now creates actual SDL system cursors and participates
in the original App global registrar, resource ownership and update
contracts. Its focused process executed on the visible Wayland host
under GCC and Clang. The combined mission link still lacks 50
distinct production symbols, and external `Sol_Train.dmission` shell
runs still report "mission not started". See
[the focused record below](#native-app-cursor-service-2026-09-27).
No gameplay acceptance condition has passed.

Latest App primitive-builder gate, 2026-09-27: the production
`PrimitiveBuilder2 PB` now has a native owner and its real quad submission
executes through the existing render adapter under GCC and Clang. A
temporary combined mission link is down to 51 distinct unresolved App
symbols; the lawful external `Sol_Train.dmission` shell still reports
"mission not started". This is synthetic CPU rendering of an App draw
caller, not a visible mission unit or gameplay. See
[the focused record below](#native-app-primitive-builder-owner-2026-09-27).
No gameplay acceptance condition has passed.

Latest offline packet-dispatch gate, 2026-09-27: the production
`NetPacket` and `NetBuffer` owners now build and run together under GCC
and Clang. A normal offline `NETF_ECHO` send traverses the private
packet heaps and the real EventSystem to `CQE_NETPACKET`. The selected
mission still stops before startup; this is packet dispatch, not a
movement command. See
[the focused record below](#production-offline-packet-dispatch-2026-09-27).
No gameplay acceptance condition has passed.

Latest local packet-buffer execution gate, 2026-09-27: the production
`NetBuffer.cpp` owner now builds and executes its original offline
self-send queue under GCC and Clang with native DACOM heap and real
user defaults. Native DirectPlay-only calls fail explicitly; the owner
is not linked into the shell, and the selected mission has not started.
See [the focused record below](#production-local-packet-buffer-build-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest operation-owner build gate, 2026-09-27: production `OpAgent.cpp`
now compiles as a native GCC/Clang CMake object target. Its real
operation queue is still outside the shell link closure and has not
advanced the selected mission. See
[the focused record below](#production-operation-owner-native-build-gate-2026-09-27).
No gameplay acceptance condition has passed.

Latest shared App-startup gate, 2026-09-27: the original two-phase
`GlobalComponent` registration and cleanup owner is now compiled and
linked into the native shell under GCC and Clang. A focused check
executes 30 registrations across its node boundary; the real mission
and object registrars remain unlinked. See
[the focused record below](#shared-app-global-startup-and-cleanup-owner-2026-09-27).
No gameplay acceptance condition has passed.

Latest mission-owner build gate, 2026-09-27: production `Mission.cpp`
now compiles as `frontierx_app_mission_owner` with GCC and Clang. This is
only an object build: the shell still stops after external mission schema
loading, and no acceptance condition has passed. See
[the focused record below](#production-mission-owner-native-build-gate-2026-09-27).
Older status entries below describe earlier boundaries.

Latest production object-owner build gate, 2026-09-27: `ObjList.cpp`
now compiles as a native CMake object target under GCC and Clang. A
temporary shell link failed with 80 distinct unresolved production App
symbols, so the owner remains unlinked and has not loaded the mission's
Forger. See [the focused record below](#native-object-owner-build-and-link-closure-2026-09-27).
No gameplay acceptance condition has passed.

Latest GameTypes source gate, 2026-09-27: an explicit lawfully external
`--game-types` source is opened by the native shell and retained for the
production `ObjectList::loadTypesData` branch. Both compiler builds found
the selected Forger's 856-byte type record. The mission still stops at
source/schema loading; `ObjectList` is not linked, so no acceptance
condition has passed. See
[the focused record below](#native-gametypes-source-binding-2026-09-27).

Latest native App event-owner gate, 2026-09-27: the shell now retains
the active system's real `IEventSystem` as `EVENTSYS` throughout its
event loop. The native `ObjectList` source subscribes to that owner's
outgoing `IEventCallback` connection point; Windows still uses
`FULLSCREEN`. Both external-data shell runs executed the identity/lifetime
check, but `ObjectList` is not linked and received no gameplay input.
See [the focused record below](#native-object-list-event-owner-contract-2026-09-27).
No gameplay acceptance condition has passed.

Latest production frame-clock source gate, 2026-09-27: native
`ObjList.cpp` now maps its original performance-counter calls to the
existing monotonic platform clock at one million ticks per second,
without changing the Windows call path or simulation order. Direct
GCC/Clang syntax checks advance through `BeginFrame` and first stop on
real `FULLSCREEN`, nugget, camera and scheduler ownership. The object
owner remains unlinked and its clock call has not run through gameplay;
see [the focused record below](#object-list-native-frame-clock-source-contract-2026-09-27).
No gameplay acceptance condition has passed.

Latest optional object UI boundary, 2026-09-27: the Windows-only
performance report and archetype editor dialog are now excluded from the
native object owner; its clock fields use existing fixed 64-bit types.
Direct GCC/Clang syntax checks now advance to `FULLSCREEN` ownership and
the required `BeginFrame` clock, but the owner still does not build or execute. See
[the focused record below](#object-list-debug-and-clock-declarations-2026-09-27).
No gameplay acceptance condition has passed.

Latest object-owner source boundary, 2026-09-27: direct GCC/Clang syntax
checks of production `ObjList.cpp` now pass its Windows PCH, case-sensitive
include and header-order barriers and reach the implementation body. The
first remaining errors concern Win32 debug allocation, UI/dialogs and
timing. The owner is not in the native build, and no object was created;
see [the focused record below](#object-list-native-source-boundary-2026-09-27).
No gameplay acceptance condition has passed.

Latest QuickSave input boundary, 2026-09-27: the existing native data-root
probe now traverses the chosen external `Sol_Train.dmission` through the
same `IFileSystem` directory and DOS-child operations used by
`ObjectList::Load`. GCC and Clang each enumerated the Forger, Taos and
Acropolis archetype directories and completely read four named object
records. This proves the required input is present and reachable, not that
`ObjectList` created units. The production owner remains outside the native
build; see [the focused record below](#mission-quicksave-object-input-boundary-2026-09-27).
No gameplay acceptance condition has passed.

Latest mission-owner save preflight gate, 2026-09-27: direct GCC/Clang
`Mission.cpp` syntax checks now pass its `IOpAgent` and first App service
declarations and the Windows-only disk-space helper. The Linux
`Mission::Save` branch queries a checked native platform service and
fails when available space cannot be determined; the existing data-root
test covers valid and missing paths. `Mission.cpp` still does not compile
or link as a whole and no save ran. See
[the focused record below](#mission-save-space-platform-contract-2026-09-27).

Latest complete global-owner compile gate, 2026-09-27: the production
`MGlobals.cpp` and `Objwatch.cpp` translation units now build together as
`frontierx_app_mglobals_owner` under GCC and Clang. An attempted native
shell link identified 16 unresolved real App service symbols; the owner
is not linked into the successful shell. Both complete build/CTest
presets and both external mission-schema shell runs still pass. See
[the focused record below](#complete-mglobals-compile-and-link-closure-2026-09-27).

Latest general-data gate, 2026-09-27: the native shell now links the
production `GenData.cpp` owner and loads the lawfully external Steam
`gendata.db` through a read-only native UTF filesystem under GCC and Clang.
It checks that the required `GlobalValues` archetype has the 1640-byte
production `GT_GLOBAL_VALUES` layout. Both external processes exited 0
after this load and the existing mission ParseData check. The App
`MGlobals.cpp` owner remains unlinked, so the mission has not started.
See [the focused record below](#production-general-data-owner-2026-09-27).

Latest global-values source gate, 2026-09-27: `MGlobals.cpp` now excludes
its Win32 development viewer from native translation while keeping a
Linux branch that copies the required `GlobalValues` into the simulation's
existing `globalValues` object. `MGlobals::New` and `Load` fail when that
record is missing or has the wrong size. Direct GCC/Clang syntax compiles
reach gameplay functions and next fail on real App services, including
localized text, player IDs, matrix and object lists. The owner remains
outside the native graph; this is source/compile progress, not gameplay.
See [the focused record below](#mglobals-global-values-source-contract-2026-09-27).

Latest global-owner compile gate, 2026-09-27: direct GCC and Clang
`MGlobals.cpp` syntax compilation now passes its Windows PCH and
case-sensitive include barriers and reaches the implementation body.
It still fails on real App globals, the Win32 `GlobalViewer` window path,
and other owners; `MGlobals.cpp` remains outside the native graph. Both
full Linux Debug build/CTest presets passed, but neither a global-data
load through this owner nor mission gameplay ran. See
[the focused record below](#mglobals-native-owner-compile-boundary-2026-09-27).

Latest mission ParseData gate, 2026-09-27: both native shells now read and
parse the selected external `Sol_Train.dmission` `ParseData/Data.h` through
the active App system's production `IViewConstructor2`, using the same
native loader contract called by `Mission::LoadParseData` in source.
DOCUVIEW now rejects a malformed secondary schema rather than returning a
non-null symbol list; the existing shell test covers that failure. Both
lawful external-data processes exited 0 after parsing 160633 bytes and
opening the 3236-byte Globals record. `Mission.cpp` still fails direct
native syntax compilation on missing App globals, so this is parser/data
execution, not `Mission::ReloadMission` or gameplay. See
[the focused record below](#native-mission-parsedata-owner-contract-2026-09-27).

Latest native App resource gate, 2026-09-27: the repository `data.i` is
embedded at build time and parsed through the profile-owned production
`IViewConstructor2` during bounded external mission-source startup under
GCC and Clang. Four C++ layout assertions accidentally copied into the
DOCUVIEW resource caused parser errors despite its success return; they
were removed from the resource while the actual header assertions remain.
Native `ParseMemory` now rejects parser errors, and the existing check
covers that regression. Both shells loaded the corrected schema and opened
the lawful external `Sol_Train.dmission` (exit 0). `Mission::ParseDefFile`
itself remains unlinked; no mission or gameplay ran. See
[the focused record below](#native-app-parser-resource-binding-2026-09-27).

Latest App schema and command-include gate, 2026-09-27: direct GCC/Clang
syntax compilation of `Mission.cpp` now reaches its function bodies after
the shared command packet and native script ABI changes. It still fails on
missing App globals and the Win32 resource loader in `GetPreprocessData`.
The existing production view-parser check now parses the repository
`data.i` used by `Globals.rc` and finds a 3236-byte `MT_GlobalData` under
both compilers, matching the selected external mission's global-record
size. This is parser execution, not `Mission::ParseDefFile`. See
[the focused record below](#repository-app-schema-and-mission-source-body-2026-09-27).

Latest App declaration and script ABI gate, 2026-09-27: the packet enum used
by normal move commands now has one shared definition with all 67 original
enumerators unchanged. The native `MScript` registration header compiles
under GCC and Clang with pointer-sized 40-/56-byte runtime structures,
while its supported Win32 24-/28-byte assertions remain. `Mission.cpp` is
still outside the native build; direct syntax compilation now exposes its
remaining UI/App-global and legacy include dependencies. Both compiler
shells still opened the lawful external mission and created/destroyed the
Forger Engine instance (exit 0); no mission ran. See
[the focused record below](#app-declarations-and-script-registration-abi-2026-09-27).

Latest mission-owner compile gate, 2026-09-27: the public `IMission` declaration
now includes its actual race/stance and file-transfer status definitions and
compiles as a standalone native header under GCC and Clang. Direct compilation
of the production `Mission.cpp` still stops at the bundled Windows-only
`ZoneLobby.h` RPC include, and `MGlobals.cpp` stops at its Windows PCH.
Neither App owner is linked or executed on Linux. See
[the focused record below](#mission-owner-native-compile-boundary-2026-09-27).

Latest App data-source gate, 2026-09-27: GCC and Clang native shells now bind
the selected external campaign, object and material sources to the App's
`SPMAPDIR`, `OBJECTDIR` and `MATERIALDIR` globals after the unchanged source
profile completes. Both opened `Sol_Train.dmission` through `SPMAPDIR` and
checked its ParseData and Globals members, then exited 0. The mission was
not started. See [the focused record below](#native-app-mission-source-bindings-2026-09-27).

Latest global-data format gate, 2026-09-27: the selected mission's external
3236-byte `MT_GlobalData` record passed the production parser's
`CorrelateSymbol` path and a fixed-width disk/native/disk round trip under
GCC and Clang. The Linux-only format adapter is called from the source
`MGlobals::Load`/`Save` owner, but that App translation unit is still outside
the native build and has not executed. The Windows branch remains unchanged.
See [the focused record below](#native-global-data-format-and-owner-call-2026-09-27).

Latest mission-loader data gate, 2026-09-27: the production native
`IFileSystem` now resolves the root-absolute backslash paths used by
`Mission::LoadParseData` and `MGlobals::Load`. GCC and Clang opened the
lawfully external `Sol_Train.dmission`, its `\ParseData\Data.h`, and
`\MT_GlobalData\Globals`; the production `IViewConstructor2` parsed that
external schema and found `MT_GlobalData`. This is retail-input filesystem
and parser execution, **not** `Mission::Reload` or schema correlation. The
next concrete blocker is the global record's native layout: its parsed
retail size is 3236 bytes, while native `MT_GlobalData` is 3812 bytes because
nine 32-character `M_STRINGW` fields use four-byte Linux `wchar_t` instead
of the two-byte Win32 representation. See
[the focused record below](#mission-loader-paths-and-global-schema-2026-09-27).

Current Forger instance gate, 2026-09-27: GCC and Clang now create and
destroy an Engine/MeshManager instance of the lawful external
`ssforger.cmp` through the unchanged source profile. The diagnostic checks
that a valid Engine root exists during the instance lifetime and no root
remains after destruction; both runs reported 13 face groups and exited 0.
This is a retail-data object instance, not an App-owned mission unit or
visible gameplay. See
[the focused record below](#production-forger-engine-instance-2026-09-27).

Earlier mesh-archetype and animation gates below are historical snapshots.

Earlier Forger mesh gate, 2026-09-27: the native shell now initializes the
source-profile `ITManager`, `IMaterialManager` and `IMeshManager` owners and
asks the production MeshManager to create the external `ssforger.cmp`
archetype. GCC and Clang retail-data runs each returned 13 face groups and
exited 0. This is an
archetype-data and CPU geometry result, not a unit instance, visible draw,
mission, input, simulation or save/load. See
[the focused record below](#production-forger-mesh-archetype-2026-09-27).

Earlier animation-data and startup gates below are historical snapshots.

Earlier animation-data gate, 2026-09-27: GCC and Clang now load one real
Forger object animation (`ssforger.cmp`, script `Sc_build`, duration 1.6 s)
from the external Steam `objects` archive through the production `IAnimation`
and `IChannel2` owners. This is a required object-data path for the selected
mission, but no unit instance, mission, input, simulation or save/load has
run. See [the focused record below](#production-channel-and-forger-animation-data-2026-09-27).

Earlier startup gates below are historical snapshots.

Current startup gate, 2026-09-27: the native shell now completes the original
`Conquest.ini` `GameSystem::startup` under GCC and Clang using the lawful
external Steam data root, repository `Data/mat`, and explicit movie skip.
`IAnimation` and `IHardpoint` are production aggregates, and `Optics` is the
original source-profile render component; the process opens
and closes its SDL dummy window with exit 0. No `Conquest.cpp` App owner
initialization or mission has run, and none of the gameplay acceptance gates
has passed. See [the focused record below](#production-animation-and-hardpoint-startup-2026-09-27).

Earlier owner gates below are historical snapshots.

Current mesh-owner gate, 2026-09-27: the native shell links the production
`MeshManager`, `MeshArch` and `MeshInstance` classes. GCC and Clang now create
`IMeshManager` and then the production Engine `IRenderer` from the unchanged
source profile on lawful external data with explicit `--skip-movies`. Both
stop at required `IAnimation` (exit 3). This is profile/owner startup, not
mesh initialization, a mission, or gameplay. See
[the focused record below](#production-mesh-manager-owner-2026-09-27).

Earlier particle-owner gate, 2026-09-27: the GPU-enabled native shell now
registers the production `IParticleManager` from `ParticleEffect.cpp`. The
unchanged source profile, with the explicit `--skip-movies` policy, creates it
from the lawful external archives under GCC and Clang and then fails at
`IMeshManager` (exit 3). The shell also creates and releases a real point-emitter
program and checks the manager's 29 filters in its existing startup suite.
This is production-owner startup and synthetic program execution, not
`Conquest::InitParticles`, mission execution, particle rendering, or gameplay.
See [the focused validation below](#production-particle-owner-2026-09-27).

Earlier audio gate, 2026-09-27: the optional shared-LGPL FFmpeg adapter now
decodes external `STMG03.wav` through native `IStreamer`. GCC and Clang
completed host PipeWire playback with identical decoded PCM fingerprints.
The default SDL-only build still rejects IAC explicitly. The unmodified
profile's next creation failure remains `IVideoSystem`; full App audio
initialization and mission integration remain open. No media service was
disabled and no mission ran. See
[NATIVE_AUDIO_STREAMER.md](NATIVE_AUDIO_STREAMER.md#optional-iac-decoder-2026-09-27).

Status: 2026-09-27. Source review, synthetic startup/render execution and
read-only inspection of user-supplied external retail data. **No native mission
has started. None of the five gameplay acceptance gates has passed.**

## Mission and bounded acceptance

Use `Campaign/Sol_Train.dmission` (Celareon/Solarian training), whose source
script is [Sol_T.cpp](../Code/App/Src/Scripts/Sol_T/Sol_T.cpp). The campaign
menu selects `Sol_Train.dmission`; `Mission::ReloadMission` selects the same
stem for mission ID zero with the Solarian race. The external Steam install
supplied by the user contains this 178,192-byte entry in its `DATA/maps` UTF
archive and a Windows `Scripts/Sol_T.dll`. The existing native
`ListUtfDirectory` and `ReadUtfEntry` successfully listed and read the mission
entry on Linux. No payload was copied into the repository or printed. This
is archive-reader execution on retail input, not mission execution or proof
that the source script and retail save schema are compatible.

The source program `SolTrain_Start` locates `Forger`, `Taos1`, `Taos2` and
`Acropolis`, initializes player 1 and starts `SolTrain_SelectAndMoveForger`.
That program provides a bounded checkpoint before construction and combat:

1. Launch the mission through production mission loading, with external data
   unchanged and the explicit `--skip-movies` policy below. Require a valid player-1 `Forger`, visible geometry and normal
   selection hit testing. Record initial unit ID, system, position and tick.
2. Select only `Forger` through ordinary input. Require objective 1 completed
   and the script's `MoveForger` state.
3. Issue one normal move command to an unobstructed point in the current
   system. Record the destination and tick once a usable camera/grid is
   available. Do not substitute a direct script `OrderMoveTo` call.
4. Run the production simulation until the unit changes position and the
   script reaches `SelectAll` (saved enum value 2). Require objective 2
   completed, objective 3 present and both Taos units selectable. Stop before
   selecting all three. The script tests `!IsIdle(Forger)`, so a changed
   command queue alone is insufficient: actual movement must also be measured.
5. Save through production persistence at a paused tick. Reload through the
   production loader; require the same mission/program state, objective
   flags, player ownership, stable unit IDs, system, positions, orders,
   resources, simulation tick and RNG state where serialized. Resume for a
   fixed tick interval and compare with an uninterrupted run from the same
   checkpoint. No raw pointer or native handle may enter the save.
6. Repeat the same recorded inputs and checkpoint on native x86_64 GCC and
   Clang builds with visible GPU execution. Preserve images and state
   comparisons outside the repository; a skipped GPU test does not count.

The exact move destination and tick interval remain to be measured after the
real mission can load; inventing these from the synthetic renderer would not
make the procedure reproducible. This acceptance is a short training
checkpoint, not mission victory. The Windows first-Terran-mission scene with
no units is an unresolved restoration defect, not a reference state. The
reported Celareon ships are a selection hint, not measured acceptance evidence.

## Mission loader paths and global schema (2026-09-27)

**Blocker and caller.** `Mission::Reload` calls `LoadParseData`, which opens
`\ParseData\Data.h` from the nested mission filesystem. `MGlobals::Load`
changes to `\MT_GlobalData` and opens `Globals`. The native filesystem
previously interpreted those Win32 root paths as physical absolute paths or
literal backslashes, so the real loader could not reach its schema or global
record. Existing generated UTF tests exercised relative child paths only.
The falsifiable check was to open these exact paths via the production
`OpenDataRootArchive`/`IFileSystem` composition on the selected external
mission and parse the schema through the production view constructor. This
determines whether the next failure is path resolution or record correlation.

The native filesystem now normalizes backslashes and treats one leading slash
as the root of its mounted physical or UTF source. It still canonicalizes
physical paths beneath their source root and rejects empty, `.` and `..`
components; UTF member case remains exact. Existing generated material and
data-root checks cover relative paths, absolute paths and traversal rejection.
The existing view-parser check adds the mission schema path. No disk schema,
Windows filesystem implementation, simulation rule or module ABI changed.

GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug built the four affected
existing probe targets and completed the full build presets. The focused
`portability.data_root`, `portability.view_parser`,
`portability.state_material`, and three game-material-manager cases passed
under each compiler. Both external-data filesystem runs opened
`Sol_Train.dmission`, read 160633 ParseData bytes, found one global-data entry
and read 3236 `Globals` bytes. Both external-data parser runs resolved
`MT_GlobalData` with a 3236-byte schema symbol and exited 0. The data stayed
outside the repository; no payload was printed. Commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_data_root_probe frontierx_view_parser_probe frontierx_state_material_probe frontierx_game_material_manager_probe
cmake --build --preset linux-clang-debug --target frontierx_data_root_probe frontierx_view_parser_probe frontierx_state_material_probe frontierx_game_material_manager_probe
ctest --test-dir out/build/linux-gcc-debug -R '^portability\.(data_root|view_parser|state_material|game_material_manager.*)$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability\.(data_root|view_parser|state_material|game_material_manager.*)$' --output-on-failure
out/build/linux-gcc-debug/frontierx_data_root_probe --mission-filesystem "$LAWFUL_STEAM_DATA_ROOT" Sol_Train.dmission
out/build/linux-clang-debug/frontierx_data_root_probe --mission-filesystem "$LAWFUL_STEAM_DATA_ROOT" Sol_Train.dmission
out/build/linux-gcc-debug/frontierx_view_parser_probe --mission-schema "$LAWFUL_STEAM_DATA_ROOT" Sol_Train.dmission
out/build/linux-clang-debug/frontierx_view_parser_probe --mission-schema "$LAWFUL_STEAM_DATA_ROOT" Sol_Train.dmission
```

**Next blocker.** Source review shows `MGlobals::Load` correlates that symbol
directly into `MT_GlobalData`. A native layout compilation measured 3812 bytes
for the destination and 128 bytes for `M_STRINGW` (32 Linux `wchar_t` values);
the retail record and parsed symbol are 3236 bytes. The nine wide-string
fields explain the 576-byte difference. The current `MGlobals::Load` also
does not check the byte count or correlation result before reporting its read
result, and `MGlobals::Save` writes `sizeof(globalData)` directly. A fixed-width
disk representation and checked conversion at the global-data owner are
needed before native correlation and saves can be considered safe.
`Mission::Reload`, save/reload, normal
input, simulation, visible GPU gameplay, native Windows Debug/Final and
cross-module ABI checks were not executed. This mismatch is a Linux
portability blocker; it does not diagnose the separate CQ2/retail missing-unit
restoration defect.

## Native global-data format and owner call (2026-09-27)

**Blocker and decision.** The previous external mission check found a
3236-byte `MT_GlobalData` schema and record, while native `MT_GlobalData` is
3812 bytes. `MGlobals::Load` passed the record directly to
`Mission::CorrelateSymbol`, and `MGlobals::Save` wrote the native structure
size. This would shift every field after the nine wide strings and produce an
incompatible save. An isolated generated-record check could not demonstrate
the selected retail schema's offsets, so the existing view-parser check was
extended instead. Passing it permits a bounded disk/native adapter at the
actual global-data owner; failing it would require field-by-field layout
repair before App integration.

The production view constructor reports `gameDescription` at offset 0,
`playerNames` at 64, `playerAssignments` at 576 and `gameSettings` at 3160
in the lawful external `Sol_Train.dmission` schema. Both GCC and Clang
compiled the native structure at 3812 bytes, with `playerAssignments` at
1152 and `gameSettings` at 3736. The Linux-only adapter preserves each
16-bit wide code unit in the first 576 disk bytes, copies the remaining 2660
bytes to the matching native suffix, asserts the relevant layout and total
extent, and rejects a native code unit that cannot fit in 16 bits. The source
`MGlobals::Load` now requires the exact record byte count, successful
`Mission::CorrelateSymbol` symbol lookup before decoding and changing global
state. `Save` writes exactly 3236 bytes only after encoding succeeds. The
Win32 code branches and on-disk representation remain unchanged.

The existing `frontierx_view_parser_probe` links this adapter. With the
external mission, it opened `Globals`, called the production parser's
`CorrelateSymbol` on the external record, decoded and re-encoded the result,
compared all 3236 bytes, and confirmed an unrepresentable native wide code
unit fails without changing the output buffer. GCC 16.2.1 and Clang 22.1.8
Linux x86_64 Debug targeted builds and executable runs exited 0 and printed
`Mission ParseData/global record round-tripped; bytes: 3236`. The full build
presets now include that target. Both full CTest presets executed 44 passing
checks and skipped two GPU checks in the command sandbox; their matrix is in
[BUILD_GRAPH.md](BUILD_GRAPH.md). No retail bytes were stored or printed.
Commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_view_parser_probe
cmake --build --preset linux-clang-debug --target frontierx_view_parser_probe
out/build/linux-gcc-debug/frontierx_view_parser_probe --mission-schema "$LAWFUL_STEAM_DATA_ROOT" Sol_Train.dmission
out/build/linux-clang-debug/frontierx_view_parser_probe --mission-schema "$LAWFUL_STEAM_DATA_ROOT" Sol_Train.dmission
cmake --build --preset linux-gcc-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-gcc-debug --output-on-failure
ctest --preset linux-clang-debug --output-on-failure
```

The App `MGlobals.cpp` translation unit is not in the native graph, so its
new guarded calls have only source-review evidence. Its separate save-menu
metadata readers still read raw native structures and need the same adapter
when that UI becomes reachable. The current native `IFileSystem` sources
are read-only; production save output has not executed. The immediate
integration work is the App global-owner and camera/data-directory startup
closure, then actual `Mission::Reload` and writable save ownership. No
visible unit, normal input, simulation checkpoint, save/reload, GPU mission
runtime, native Windows Debug/Final or cross-module ABI check ran. This is a
Linux layout defect, independent of the Windows CQ2/retail missing-unit
restoration issue.

## Native App mission source bindings (2026-09-27)

**Blocker and caller.** The production Windows `menu.cpp` global component
creates `OBJECTDIR`, `SPMAPDIR` and `MATERIALDIR` through its Win32-bound
menu/preferences path. `Conquest.cpp` then passes the first and third to the
production mesh/material managers; `Mission::LoadBriefing` and
`Mission::ReloadMission` use the campaign directory. The native shell had
already mounted the external archives and material source, but had not
assigned these App globals. A falsifiable next gate was to make the shell
open the selected mission through the same `SPMAPDIR` pointer and fail if a
required source or mission member was absent. The existing shell process
check covers that gate; no standalone probe was added.

The Linux App composition now holds references to the real `Objects`,
`Maps/Campaign` and explicit `Mat` filesystems, assigns those three globals,
and clears/releases them after GameSystem teardown. The shell's bounded
`--mission-source Sol_Train.dmission` mode opens the mission through
`SPMAPDIR`, checks `\ParseData\Data.h` and `\MT_GlobalData\Globals`, and
states that the game has not started. The mode requires a `.dmission` leaf
name, rejects traversal and missing mandatory data, and is unavailable in
the no-GPU Engine build. The existing Forger diagnostic uses the bound
`OBJECTDIR`/`MATERIALDIR` when the options are combined. For physical
directory sources, `IFileSystem::CreateInstance` now roots the child view at
the selected directory; otherwise a root-absolute mission path incorrectly
escaped back to the campaign parent. The existing generated shell test
reproduced and protects this contract.

GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug shell targets built. The
existing `portability.native_shell` process test passed under each compiler:
it rejected a generated mission missing `Globals`, then opened the completed
generated directory source and reported `mission not started`. Both compiler
shells separately completed the unchanged `Conquest.ini` source profile on
the lawful external Steam data, with explicit `--skip-movies`, opened
`Sol_Train.dmission` through `SPMAPDIR`, observed 160633 ParseData and 3236
Globals bytes, closed the dummy SDL window and exited 0. In separate bounded
120-second runs, each shell also used the bound object/material sources to
create and destroy the external `ssforger.cmp` Engine/MeshManager instance,
reported 13 face groups and exited 0. Earlier 45-second combined runs
printed the expected diagnostic but timed out (exit 124) and are excluded
from pass evidence. No retail payload was copied into the repository.
Commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell
cmake --build --preset linux-clang-debug --target frontierx_native_shell
ctest --test-dir out/build/linux-gcc-debug -R '^portability.native_shell$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.native_shell$' --output-on-failure
XDG_CONFIG_HOME=/tmp/frontierx-app-sources-linux-gcc-debug SDL_VIDEODRIVER=dummy timeout -k 5s 30s out/build/linux-gcc-debug/frontierx_native_shell --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat --system-profile Code/App/Src/Conquest.ini --skip-movies --mission-source Sol_Train.dmission --smoke-ms 1 --hidden
XDG_CONFIG_HOME=/tmp/frontierx-app-sources-linux-clang-debug SDL_VIDEODRIVER=dummy timeout -k 5s 30s out/build/linux-clang-debug/frontierx_native_shell --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat --system-profile Code/App/Src/Conquest.ini --skip-movies --mission-source Sol_Train.dmission --smoke-ms 1 --hidden
XDG_CONFIG_HOME=/tmp/frontierx-app-combined-linux-gcc-debug SDL_VIDEODRIVER=dummy timeout -k 5s 120s out/build/linux-gcc-debug/frontierx_native_shell --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat --system-profile Code/App/Src/Conquest.ini --skip-movies --mission-source Sol_Train.dmission --mesh-object ssforger.cmp --smoke-ms 1 --hidden
XDG_CONFIG_HOME=/tmp/frontierx-app-combined-linux-clang-debug SDL_VIDEODRIVER=dummy timeout -k 5s 120s out/build/linux-clang-debug/frontierx_native_shell --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat --system-profile Code/App/Src/Conquest.ini --skip-movies --mission-source Sol_Train.dmission --mesh-object ssforger.cmp --smoke-ms 1 --hidden
```

This binds real data sources, but it does not run `CreateGlobalComponents`,
`Mission::Reload`, campaign script registration, a player unit, input,
simulation or save/reload. The game camera and other App owners still need
native integration. Source review also found that `Mission::Reload` ignored
both `LoadParseData` and `MGlobals::Load` failures. The production source now
stops on either failure; `LoadParseData` rejects an empty/invalid size, a
short read and a failed parse, with a 4 MiB native bound above the measured
160633-byte external schema. `Mission.cpp` is not in the Linux build, so
these owner changes have source-review evidence only and were not executed
by the shell or CTest. The existing `IStreamer` remains an active required
aggregate; only full-screen `IVideoSystem` is omitted by explicit movie
skip. Native Windows Debug/Final, cross-module ABI and visible GPU gameplay
were not checked for this change.

## Media contracts and current decision

`IStreamer` is the low-level WAV player, not an asset-streaming filesystem.
Its production implementation uses DirectSound, Win32 worker threads and
window messages. App `Streamer.cpp` initializes it; `Menu_Briefing.cpp`
reinitializes buffer durations. `MusicManager.cpp` consumes playback state;
`SoundManager.cpp` receives `CQE_STREAMER` and uses `INITSUCCESS` to unpause
talking-head animation and restart its stream, and terminal statuses to
release streams. `Sol_T.cpp` waits on `IsStreamPlaying` and ties subtitles
and objectives to media handles. A successful no-op player would alter these
contracts and is not an acceptable implementation. Muting volume alone does
not remove the calls: `PlayAudioMessage` and `updatePendingList` still open
streams. Audio remains a required unresolved startup service until a native
player or an explicit, verified no-audio caller policy is implemented.

`IVideoSystem` provides full-screen movie textures. `Conquest.cpp` tolerates
its absence at interface acquisition and guards `Initialize`, but the source
profile still requests it and `MenuMovie` dereferences it unconditionally.
The exported `MovieScreen` has an existing `bSkipMovies` early return. Calls
to full-screen movies in the reviewed campaign and end-game menus are
commented out. `Sol_T` uses talking-head `IAnimate`/sound paths, not direct
`IVideoSystem` calls. This makes full-screen cinematics a candidate for
explicit exclusion from the bounded movement checkpoint, but does not make
all animation optional. No native video-disable policy or fake video service
was installed in this step. Before omitting video in a playable launcher,
select the existing skip policy explicitly and verify every reachable caller.

Temporary profiles with media entries omitted are diagnostic inventory only;
they are not a supported retail profile or successful audio/video startup.

## Earliest external-data and production-owner blockers

The actual Steam data root contains packed UTF files named `objects`,
`interface`, `textures`, `profiles` and `maps`. `DataRoot.cpp` now validates
these archives and records their actual filename and virtual directory;
the native shell mounts and retains them through `IFileSystem`. Both native
headless executables report every supplied archive present, with `Mat` the
only missing required source. The old directory fixture models the CQ2
source layout. Do not create empty directories or pretend the retail media
is missing to bypass this failure. `menu.cpp` and `Conquest.cpp` currently
require `MATERIALDIR`. The shell now accepts the explicit source-owned
`--material-root Data/mat` directory through native `IFileSystem`, as recorded
below. The full App global assignment remains unconnected. The native `IFileSystem` also
opens the selected entry as a nested UTF archive through the `CreateInstance`
sequence used by the campaign directory and mission callers. Independent
GCC/Clang native executions saw `ParseData`, `MT_SECTOR_SAVELOAD`,
`ObjectList`, `MT_MUSIC_DATA` and `MT_GlobalData` at its root after releasing
the outer archive interfaces. Actual
mission-loader execution and schema correlation remain unresolved; listing
these entries is not mission loading. See the nested UTF validation in
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md#nested-mission-utf-filesystem-contract-2026-09-27).

Using generated directory input to reach the production profile independently
of that earlier data-root failure gave this measured sequence:

| Profile | First failing service | Result |
|---|---|---|
| Unmodified source `Conquest.ini` | `IStreamer` | exit 3, no window |
| Diagnostic omission of `IStreamer` only | `IVideoSystem` | exit 3, no window |
| Diagnostic omission of both media services, before factory fix | `IMaterialManager` | exit 3, no window |
| Same diagnostic after factory fix | `IParticleManager` | exit 3, no window |

The material and texture factories existed but were registered after profile
startup. They now register before `GameSystem::startup`; the existing scene
queries and reuses profile-created owners and explicitly releases their
borrowed scene resources before GPU teardown. See the exact validation in
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md#profile-created-game-material-and-texture-owners-2026-09-27).
`IStreamer` remains the first failure with the unmodified source profile.

Later gates still include `IParticleManager`, `IMeshManager`, Engine animation
and hardpoint/render components, the actual App startup closure,
`Mission::Reload`, object factories, normal command input and update cadence,
and save restoration. `MScript::Load` still calls Windows `LoadLibrary` and
depends on script registration/schema globals; native DACOM ELF loading does
not port that campaign ABI. The installed PE campaign DLL cannot be loaded
as a native Linux module. No native simulation or persistence evidence exists
yet. Missing units may be a CQ2/retail restoration error and must be diagnosed
separately from these known Linux integration gaps.

The user has supplied the external installation path. No additional data
request is currently blocking independent integration work. Keep that private
path in the task context, not in repository configuration or documentation.

## Packed startup-source mounts (2026-09-27)

**Integration blocker.** The native shell and headless startup used a
physical-directory-only check, while the real App `menu.cpp` requests
resource directories through `ISearchPath`/`IFileSystem` and the external
retail installation stores them in UTF archives. `Maps/Campaign` is the
concrete producer of the `SPMAPDIR` used by `Mission::LoadBriefing`.
The falsifiable check was that this virtual directory can be inspected and
opened through that interface while malformed archives, missing virtual
children and missing mandatory material data still fail. Existing data-root
and shell tests were extended; no new standalone probe was added.

The native runtime-only `DataRootEntry` now records `PresentArchive` and its
physical relative path plus a virtual directory. Regular files undergo the
same UTF validator as the existing reader; arbitrary files report
`InvalidArchive`. Known resource names may resolve to a unique differently
cased archive filename, which is retained and actually used by the mount;
ambiguous fallback matches remain errors. Virtual UTF directory names remain
exact-case. Existing physical-directory case diagnostics are unchanged.
`OpenDataRootArchive` reopens and validates the archive and creates the
requested directory view through `IFileSystem`. The shell keeps successful
mounts under their canonical resource names until after game-system teardown.
It does not yet assign these to the unported App global directory owners.
No persistent field, simulation rule, vtable or Windows owner changed.

The generated data-root test replaces `Maps` with a lowercase `maps` UTF,
mounts `Maps/Campaign`, reads a known byte through the real filesystem,
rejects a missing `Campaign` child and a truncated UTF, and retains the prior
missing/case/ambiguity checks. The process test opens both campaign and
multiplayer archive views before dummy SDL startup, then removes `Mat` and
requires exit 2 before a window opens. The shell reports usable data sources
rather than claiming that packed files are physical directories.

GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug full builds and CTest presets
passed; the central matrix remains in [BUILD_GRAPH.md](BUILD_GRAPH.md).
Focused Clang ASan/UBSan data-root and shell checks passed with LeakSanitizer
disabled. Each native headless executable inspected the user-supplied Steam
`DATA` root: Objects, Interface, Textures, Profiles, Campaign, Multiplayer
and all supplied audio archives were `present-archive`; Movies was a
physical directory; Mat was `missing`. Both returned 3 for that remaining
required source. A temporary native caller outside the repository used the
new inspected `Maps/Campaign` entry with `OpenDataRootArchive`, then opened
`Sol_Train.dmission` through `CreateInstance`; both compilers enumerated its
five root entries after releasing the campaign mount. This was filesystem
execution on retail inputs, not mission-loader or gameplay execution.

Executed commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_data_root_probe -j4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.data_root$' --output-on-failure
cmake --build --preset linux-gcc-debug --target frontierx_native_shell frontierx_headless -j4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.(native_shell|headless|data_root)$' --output-on-failure
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_data_root_probe frontierx_native_shell -j4
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.(data_root|native_shell)$' --output-on-failure
c++ -std=c++17 -ICode/Libs/Include /tmp/frontierx-goal-profile/mount.cpp out/build/linux-gcc-debug/libfrontierx_platform.a -o /tmp/frontierx-goal-profile/mount-gcc
clang++ -std=c++17 -ICode/Libs/Include /tmp/frontierx-goal-profile/mount.cpp out/build/linux-clang-debug/libfrontierx_platform.a -o /tmp/frontierx-goal-profile/mount-clang
```

Both temporary callers were run with the same external `DATA` root as their
sole argument. Each headless build ran with `--data-root` set to that root
and `XDG_CONFIG_HOME=/tmp/frontierx-goal-profile/settings`; private paths and
retail payloads remain outside repository documentation and fixtures.
No new visible GPU run, native Windows Debug/Final or ABI validation,
retail render, Mission caller, simulation or persistence execution occurred.
The next change must connect required material source lookup, not merely
mark the still-missing `MATERIALDIR` optional.


## Explicit material directory source, 2026-09-27

The named blocker was the physical CQ2 material source: production
`MaterialManager::Initialize` enumerates `shaders` and `materials`, and
`ITManager` opens `textures`, while the native adapter only read UTF
archives. Existing material checks could cover this by reloading the same
production manager from a generated directory; no new probe was needed.

`OpenNativeDirectoryFileSystem` now supplies read-only directory enumeration,
child factories, byte reads and mapping through the existing interface.
Names are sorted, reads are capped at 64 MiB, and parent traversal and
resolved symlink escapes are rejected. No persistent format or virtual
interface changed. The shell's explicit `--material-root DIRECTORY` requires
openable `shaders`, `materials` and `textures` directory views and retains
the source through startup. It does not copy assets, silently supply empty
data, or make `Mat` optional. This source is not yet assigned to the full
App's `MATERIALDIR` global. The explicit `--game-scene` material argument
independently accepts a directory or UTF archive.

Automated execution: GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 completed
both full Debug builds and CTest presets. The existing three
`portability.game_material_manager*` CPU variants now reset/reload the manager
from generated physical shader/material/TGA files. They also reject a parent
child request and an outside-root symlink. `portability.native_shell`
rejects missing material children and a regular file posing as `textures`,
and accepts all three directory views with generated packed maps.
Focused Clang ASan/UBSan executed these four checks with
`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1`:

```sh
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell frontierx_game_material_manager_probe -j4
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.(native_shell|game_material_manager.*)$' --output-on-failure
```

External-data startup execution: for each `linux-gcc-debug` and
`linux-clang-debug` build, run the following with `RETAIL_DATA` set to the
user's lawful external `DATA` directory:

```sh
SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-goal-profile/settings   out/build/linux-gcc-debug/frontierx_native_shell   --data-root "$RETAIL_DATA" --material-root Data/mat   --system-profile Code/App/Src/Conquest.ini --hidden --smoke-ms 1
```

Both runs reported required sources passed, ten archives mounted, and
`IStreamer returned [FAILED]`, exit 3. The repository material source is
CQ2 source data, not a claimed retail material archive or proof of shader
compatibility. These runs open external retail filesystem views; they do
not execute the retail mission.

Visible synthetic runtime: both compiler shells ran on desktop
Wayland/Vulkan with that external root and source material override, the
previous eleven-service generated profile (empty Engine), and
`--game-scene BUILD/game_material_manager_fixture.utf.directory
BUILD/game_material_manager_fixture.utf.vertexcolor.mesh` followed by the
four generated shader arguments in this order: `sdl3_gpu_probe.vert.spv`, `sdl3_gpu_probe.frag.spv`,
`sdl3_gpu_probe_t4.vert.spv`, `sdl3_gpu_probe_mt.vert.spv` in
`BUILD/generated/shaders`. `BUILD` is the corresponding preset build tree.
Both used `--smoke-ms 200 --capture-ppm` with captures outside the repository.
Each presented one frame, 496 changed pixels, checksum
`1466864583390317354`; the two PPM files have identical SHA-256
`e54ef6c12ff5ca0fd5da724725aae5da4e79f87074f75b6dd9ce26142e3b90d1`.
The scene material is generated and separate from `Data/mat`; mounted retail
assets are not drawn. The diagnostic profile omits unavailable services and
is not a gameplay profile. Sandbox GPU skips are not this runtime evidence.

Next concrete blocker: implement the required `IStreamer` caller contract
or establish an explicit optional-audio policy that correctly advances its
real callers. No successful no-op service or media-disable policy was added.
`IVideoSystem`, remaining production services, native campaign registration,
mission/schema loading, simulation, input gameplay and save/reload remain
open. No Windows rebuild/ABI check or retail rendering was performed.


## Explicit full-screen movie policy, 2026-09-27

The bounded `Sol_Train` acceptance excludes full-screen cinematics explicitly.
Source review finds no `PlayFullScreenVideo` call in that script; it uses
`PlayAnimatedMessage` and `EnableMovieMode`. The latter controls presentation
and mission input; it is not movie decoding and remains required. The reviewed
intro/campaign/end-game full-screen movie calls are commented out. The general
`MScript::PlayFullScreenVideo` caller still funnels into `MovieScreen`.

`MovieScreen.cpp::MovieScreen` already returns early on
`CQFLAGS.bSkipMovies`, before constructing `MenuMovie`, dereferencing VIDEOSYS,
muting music or changing the cursor. The native shell's new `--skip-movies`
sets that same existing runtime flag immediately before GameSystem startup.
The native branch of `SystemContainer::LoadSystemComponents` observes it and
omits exactly `IVideoSystem`, reporting `[DISABLED by --skip-movies]`. It does
not create a substitute video object or return a fabricated successful load.
`Conquest.cpp` already permits absent VIDEOSYS and guards its initialization.
No Windows behavior, shared flag layout, save format or simulation rule changed.
No blanket missing-service exception or automatic fallback was introduced.

The flag defaults off. Audio, IAnimate, particles, mesh management, Engine
components and every other profile entry remain subject to normal creation
and failure checks. Missing video remains a startup failure without the
option. The input profile is neither rewritten nor copied to remove entries.
This policy applies only to full-screen cinematics; it does not establish
video decoding, remove briefing speech or excuse required animations.

The falsifiable check was the existing native-shell suite. A generated
profile containing IStreamer, IVideoSystem and the real window service fails
without the option and opens the dummy window with it. Its logs retain
`IStreamer returned [OK]` and identify video as disabled rather than OK.
Adding an unavailable IParticleManager still fails before opening a window.
A repeated option is rejected with the existing argument-error exit code 1.
No new probe was added. This executes the real profile loader and shared flag,
not the full MovieScreen translation unit, which is still outside the native
closure; the MovieScreen exclusion itself is source-review evidence.

Validation on Linux x86_64: GCC 16.2.1 and Clang 22.1.8 completed both full
Debug builds and CTest presets with the previously configured optional IAC
path. The focused shell process suite also passed under Clang ASan/UBSan with
LeakSanitizer disabled:

```sh
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
ASAN_OPTIONS=detect_leaks=0 cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell -j4
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.native_shell$' --output-on-failure
```

The existing GCC Debug build at `/tmp/frontierx-iac-disabled` (SDL on, GPU
and FFmpeg audio off) also built `frontierx_native_shell` and passed
`portability.native_shell`. This verifies the unconditional App flag include
and policy in the supported window-only configuration.

Both compiler shells also executed this command with `RETAIL_DATA` pointing
to the lawful external DATA root (substitute the matching preset build path):

```sh
SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-goal-profile/settings out/build/linux-gcc-debug/frontierx_native_shell --data-root "$RETAIL_DATA" --material-root Data/mat --system-profile Code/App/Src/Conquest.ini --skip-movies --hidden --smoke-ms 1
```

Both mounted ten archives, created IStreamer and IMaterialManager, explicitly
disabled IVideoSystem, then failed at IParticleManager with exit 3. This is
external-data startup evidence; it does not execute Mission, gameplay, actual
movie callbacks, save/reload or GPU rendering. No new visible GPU run, Windows
Debug/Final rebuild or ABI check was performed for this native-only policy.

The next owner identified by that run was `ParticleEffect.cpp`. Its native
registration and limited execution are recorded below.

## Production particle owner, 2026-09-27

The blocker was the required `IParticleManager` profile entry after the movie
policy. `Conquest.cpp` unconditionally queries that interface and later calls
`InitParticles(camera, position callback, material manager, mesh manager)`.
The falsifiable check was whether the existing profile loader could create
the real `ParticleManager`, then whether its factory could create and release
a point-emitter program. The existing shell process suite covers this startup
path, so no new standalone probe was added.

The GPU-enabled native shell now links the production `ParticleEffect.cpp`,
`Particles.cpp` and `ParticleParamFunctions.cpp` owners and registers their
aggregate factory before `GameSystem::startup`. A launcher validation queries
the container-created interface, requires its 29 filters, and creates and
releases its `PE_POINT_EMMITER` program. The no-GPU shell does not register
this owner; its required-service failure remains explicit.

Native source changes include exact-case headers and a Linux-compatible form
of named structures inside particle unions. Compile-time checks retain the
encoded float/transform entry and header sizes and final-field extents on
disk; LP64 runtime pointer structures remain separate. The original
`IParticleInstance::SetContext` declaration lacked a definition while
`ParticleSystem` already implements it. Making the interface method pure
virtual resolves the linked vtable without moving its slot. This is a CQ2
source-restoration defect exposed by native linking, distinct from the Linux
include/layout port. Native Windows module rebuild and ABI checks remain
mandatory before claiming that shared-interface change safe.

Validation on Linux x86_64: GCC 16.2.1 and Clang 22.1.8 completed their full
Debug preset builds and CTest suites, including `portability.native_shell`.
The focused Clang ASan/UBSan shell process suite passed with LeakSanitizer
disabled. The existing SDL/no-GPU, no-IAC GCC shell build and process suite
also passed after linking the particle objects only in the GPU configuration.
These process suites use generated profiles and repository-owned fixtures.

Both compiler shells additionally executed the unchanged source profile with
the lawful external `DATA` root, source-owned `Data/mat`, and explicit
`--skip-movies`. Each mounted ten archives, reported `IParticleManager returned
[OK]`, then `IMeshManager returned [FAILED]` and exited 3. This is a real
external-data startup run but no mission, simulation, normal unit input,
save/reload, or retail rendering. The CTest GPU probes skipped in the command
sandbox and provide no gameplay or GPU-runtime evidence for this change.

`IMeshManager` is the next concrete required owner. Its production manager is
in `Code/Libs/Src/MeshManager/MeshManager.cpp`; the real App caller later
passes Engine, animation, renderer, pipeline, camera, hardpoints, materials
and object directory to `InitManager`. Its current `ShutdownAggregate` assumes
the Engine and animation references were initialized, so merely registering
the factory without an ownership-safe pre-init shutdown path would make the
present startup failure unsafe. A direct GCC syntax check also stops at the
Windows precompiled header; the production `MeshArch` contains a D3D9 index
buffer and the linked mesh classes invoke animation and buffer methods. That
ownership/render contract must be carried across the existing boundaries,
not bypassed with a shell-only success. The lifecycle and linked mesh classes
are the next integration check. The broader Engine/Mission closure, native
Windows Debug/Final rebuild and cross-module ABI check remain unperformed.

## Production MeshManager owner, 2026-09-27

The preceding external-data run failed on required `IMeshManager`. The real
caller in `Conquest.cpp` later calls `InitManager` with Engine, animation,
renderer, pipeline, camera, hardpoint, material and object-directory owners.
The nearest falsifiable check was that the unchanged profile could create the
production aggregate and release it safely before `InitManager`; the existing
native-shell process suite exercises that early lifetime. No new standalone
probe or successful substitute manager was added.

The native GPU shell now links the three `MeshManager.vcxproj` production
translation units: `MeshManager.cpp`, `MeshArch.cpp`, and `MeshInstance.cpp`.
Linux exact-case includes and callback declarations allow those owners to
compile. The existing native index-buffer object supplies their D3D9-shaped
vtable slot and read-lock flag. The native manager initializes borrowed
pointers to null and conditionally releases its Engine/animation references
on aggregate shutdown, which also covers a profile that fails before App
initialization. The two legacy `MeshInstance` classes from MeshManager and
PolyMesh have identical global symbols in separate Windows DLLs; a Linux-only
namespace keeps their concrete implementations separate in the monolithic
shell. Public `IMeshManager` slots and Windows class names are unchanged.

`MeshInstance::Render` previously cast a 32-bit pipeline vertex-buffer handle
to a D3D9 pointer. Its native branch now passes the established typed
`IDirect3DVertexBuffer9` wrapper to production `Material::DrawVB`, preserving
the 32-bit handle as a handle rather than a truncated address. Windows keeps
its original call. The draw branch itself has compiled but has not executed
with a production MeshManager instance or retail mesh; its rendering behavior
is therefore still unverified.

The shell registers the production RenderMgr/PolyMesh factories before
`GameSystem::startup`, because the source profile requests `IRenderer` during
that call. The later generated scene reuses those registrations. The existing
shell test now verifies that a generated profile creates `IMeshManager` and
tears down normally with its Engine and animation fields still uninitialized.
It does not claim successful `InitManager` or animation.

Linux x86_64 validation: full Debug preset builds completed with GCC 16.2.1
and Clang 22.1.8. Each CTest preset ran 44 passing checks and skipped the
two command-sandbox GPU checks. `portability.native_shell` passed with the
GPU owners under both compilers, with focused Clang ASan/UBSan and
LeakSanitizer disabled, and with the GCC SDL/no-GPU, no-IAC configuration.
These automated checks use generated profiles and repository-owned fixtures.
Both compiler shells also ran the unchanged `Conquest.ini` under SDL's dummy
video driver against the lawful external `DATA` root and source-owned
`Data/mat`: each mounted ten archives, logged `IMeshManager returned [OK]`,
then Engine aggregation `[OK]` for `IRenderer` and `[FAILED]` for the next
profile entry, `IAnimation`, and exited 3. No visible GPU gameplay or mission
execution occurred. The profile's order and `IAnimation` factory absence
identify that next required owner; the native log currently omits Engine
component names. No native Windows Debug/Final rebuild, cross-module ABI
validation or retail mesh comparison was performed for this change.

The next integration blocker is the production `Anim` owner (`Anim.cpp`,
`AnimComponent.cpp`, `channel.cpp`, `mstack.cpp`) and its `IChannel2` creation
and Engine/mission animation contracts. It cannot be replaced by a successful
empty animation service: the selected training mission uses talking-head
animation and MeshManager instances call script methods.

## Production animation and hardpoint startup, 2026-09-27

The preceding external-data startup failed on the third `[Engine]` entry,
`IAnimation`, after `IRenderer`. The existing `Conquest.cpp` caller requires
both `IAnimation` and `IHardpoint` before its global-owner initialization;
`IMeshManager::InitManager` subsequently receives those interfaces. The
falsifiable check was whether the unchanged profile could create and
initialize these exact owners and complete `GameSystem::startup`. A generated
Engine profile in the existing native-shell process suite checks their
aggregate startup and teardown without creating a parallel implementation.
The Engine loader previously removed an aggregate after a failed
`Initialize()` but still returned success. Its native branch now fails the
whole profile load and releases the temporary aggregate reference; the
supported Windows behavior is unchanged. This makes the startup exit code
meaningful for required Engine owners.

The GPU shell now links all four translation units of the original Anim
project (`Anim.cpp`, `AnimComponent.cpp`, `channel.cpp`, `mstack.cpp`), the
original HardPoint owner, and the three source files of the original Optics
render component. Native factories register their original Engine and render
aggregate classes. Exact-case includes, Win32-file compatibility definitions
and a native diagnostic-output branch allow compilation without altering the
Windows DLL path. The animation script-set and channel-instance index types
are pointer-sized only on native Linux because the original owner stores
live object addresses in them; Win32 keeps its 32-bit interface declarations.
These are runtime tokens, not disk IDs. The animation target comparator also
retains the full event-handler pointer on native Linux. Pointer-based target
ordering is inherited behavior and has **not** been shown deterministic
across runs; no simulation claim follows from this startup result. Neither
`IChannel2` script creation nor a hardpoint archetype load has executed.
The native comparator converts fixed-width object/joint IDs before shifting,
avoiding signed-shift undefined behavior without changing the Win32 branch.

Native `RenderMgr` now rejects a named render component that cannot be
created instead of tracing and silently omitting it. Empty profile lines
are ignored. The existing shell process test creates `Optics` through a
generated `[RenderManager]` profile and checks that an unknown component
causes exit 3 before the window opens. The source profile now creates both
`PolyMesh` and `Optics`. Particle-effect rendering and fog state remain
unverified; compilation and aggregate creation do not establish them.
After this render-component change, both GCC 16.2.1 and Clang 22.1.8 full
Debug presets rebuilt and each CTest preset passed 44 executing checks with
two command-sandbox GPU skips. Both compilers repeated the unchanged source
profile on ten external archives with exit 0. The added `D3DRS_FOGENABLE`
symbol has the D3D9 numeric value 28; no native backend support is claimed.
The focused Clang ASan/UBSan shell check passed after Optics integration with
LeakSanitizer disabled.

The native UTF filesystem now returns Win32's `ERROR_NO_MORE_FILES` after
`FindNextFile` exhausts a directory and clears it after a successful step.
The real `Anim.cpp` caller asserts this terminal condition. The existing
StateMaterial UTF fixture checks two exhausted searches, because the
previous generic error value would invalidate animation enumeration. This
does not claim that an external mission's animation data has loaded.

On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each built the full Debug
preset and passed the existing `portability.native_shell` and
`portability.state_material` checks. The former now exercises generated
`[Engine]` entries for `IAnimation` and `IHardpoint`; the latter uses a
generated UTF archive. The focused Clang ASan/UBSan shell check passed with
`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1`; LeakSanitizer was
disabled. The GCC SDL/no-GPU shell process check passed; that configuration
does not load Engine aggregates, so only the GPU-enabled check establishes
their lifecycle. Each compiler also executed this bounded external
data invocation with exit 0:

```sh
SDL_VIDEODRIVER=dummy out/build/linux-gcc-debug/frontierx_native_shell \
  --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat \
  --system-profile Code/App/Src/Conquest.ini --skip-movies --hidden --smoke-ms 1
```

The Clang invocation used `out/build/linux-clang-debug/frontierx_native_shell`
with the same arguments. Both mounted ten external archives, completed
`GameSystem::startup`, opened the dummy SDL window and closed cleanly.
`--skip-movies` excludes only full-screen `IVideoSystem` as documented above;
`IStreamer` and animation were not disabled. The SDL dummy driver is not
visible GPU execution.

The earliest remaining integration blocker is the App sequence after
`GAME.startup` in `Conquest.cpp`: binding its global interfaces,
`CreateGlobalComponents`, production `InitParticles`/`InitManager`, texture
and material initialization, then `init_mission`/`Mission::ReloadMission`.
Source review found `CreateGlobalComponents` traverses a static startup list
populated by App translation units; roughly 200 files currently declare a
`GlobalComponent`. That closure must be identified from the selected mission's
real callers, rather than replaced with a synthetic mission bootstrap.
The native shell does not yet call that sequence. Its selected
`Sol_Train.dmission` therefore has no spawned unit, input command,
simulation tick or saved state. Other object scripts, animation playback and
unexecuted particle-effect render states may become blockers when the real
mission reaches them. These are Linux closure questions; any absent retail unit in a Windows
mission would be a separate CQ2/retail-restoration defect. Native Windows
Debug/Final rebuilds, cross-module ABI validation, visible GPU mission run,
save/reload and retail gameplay comparison were not performed for this gate.

## Production Channel and Forger animation data, 2026-09-27

The preceding source-profile run created `IAnimation` but could not load a
channel: `Anim::LoadChannels` asks DACOM for `IID_IChannel2`, whose production
factory was not registered in the native process. `MeshManager` consumes the
same animation service during object-archetype setup. The falsifiable gate
was that a profile-created `IAnimation` owner should load a named script
from an object in the existing `Objects` data source and report its duration;
a missing channel factory or malformed frames must fail rather than yield a
successful script set. The existing shell process suite now exercises that
path, so no standalone product-like probe was added.

The shell links the original `Code/Libs/Src/Channel/channel.cpp` and
registers its `ChannelArchetype` factory. A narrow shell option,
`--animation-object OBJECT_NAME SCRIPT_NAME`, resolves one child from the
already-mounted `Objects` archive or physical object directory and calls the
real `IAnimation::create_script_set_arch`, `IAnimation2::script_count` and
`IAnimation::get_duration` interfaces. It does not create an Engine object
instance or start playback. The object and script names are not echoed in
runtime diagnostics. The native channel loader rejects zero frames, unknown
or mixed event data types, truncated frames and event offsets outside the
payload; its supported Windows data layout and factory remain unchanged.
`Anim::LoadChannels` now preserves channel-open, factory and decoder failures
instead of returning a successful set with a missing channel.

The existing `portability.native_shell` test uses a generated physical
Forger-shaped object with one float channel and one old-format script. GCC
and Clang each observed one loaded script with duration 1 second, then
rejected a truncated `Frames` entry with exit 6. The GCC SDL/no-GPU build
explicitly rejects this diagnostic because it does not start the Engine.
The focused Clang ASan/UBSan shell check passed with LeakSanitizer disabled.
Both full Linux Debug build presets completed; each CTest preset passed 44
executing checks and skipped the two command-sandbox GPU tests.

Separately, the lawful external Steam `objects` archive lists
`ssforger.cmp/Animation/Script/Sc_build`. With the unchanged source
`Conquest.ini`, ten external archives, repository `Data/mat` and explicit
`--skip-movies`, both compiler shells ran:

```sh
SDL_VIDEODRIVER=dummy out/build/linux-gcc-debug/frontierx_native_shell \
  --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat \
  --system-profile Code/App/Src/Conquest.ini --skip-movies \
  --animation-object ssforger.cmp Sc_build --hidden --smoke-ms 1
```

The Clang command substituted `out/build/linux-clang-debug/frontierx_native_shell`.
Both exited 0 and reported one script of 1.6 seconds before clean SDL dummy
window shutdown. This is production animation-data execution on external
retail input, not a unit render, animation playback, mission, simulation or
save/load check. No external data or payload was copied into the repository.
The selected mission's Forger unit identity and object assignment still need
confirmation through the real App archetype loader, and other object scripts
remain untested. Native Windows Debug/Final builds, module ABI checks and
visible GPU gameplay were not performed for this gate.

## Production Forger mesh archetype, 2026-09-27

**Named integration blocker.** After `GameSystem::startup`, the original
`Conquest.cpp` owner sequence calls `ITManager::Initialize`,
`IMaterialManager::Initialize` and `IMeshManager::InitManager`; its unit
callers then use `CreateMeshArch`. The shell previously stopped before those
contracts. A diagnostic through those same owners is needed to distinguish
profile startup from real object geometry loading. The existing generated
scene test calls a different composition path and cannot check an external
compound archetype. This diagnostic has no parallel mesh implementation.

`--mesh-object OBJECT_NAME` now resolves the mounted `Objects` root, queries
the source-profile Engine/Renderer/Animation/Hardpoint and system
pipeline/material/texture/mesh services, initializes the production managers
with repository `Data/mat`, and calls `IMeshManager::CreateMeshArch`. It
requires an Engine archetype and releases the returned mesh reference.
The App camera is absent for this archetype-only check; mesh instance
creation, rendering, selection and movement are outside its contract.
`MeshManager::CreateMeshArch` now returns null if the Engine rejects an
archetype, releasing a script set it may have acquired. Animation-less static
archetypes remain allowed by the existing owner contract. No serialized
field or public interface layout changed.

On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 built the full
`linux-gcc-debug` and `linux-clang-debug` presets. Both focused
`portability.native_shell` checks passed; the extended generated input test
rejects a mesh request when required production services are absent and,
with all source-profile owners present, rejects an animation-only object
whose Engine archetype cannot be created (exit 6).
Both full CTest presets passed 44 executed checks each. The two GPU checks
skipped in the command sandbox and are **not** runtime evidence for this
change. The external command for each compiler was:

```sh
SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-mesh-settings \
  out/build/linux-gcc-debug/frontierx_native_shell \
  --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat \
  --system-profile Code/App/Src/Conquest.ini --skip-movies \
  --mesh-object ssforger.cmp --hidden --smoke-ms 1
```

The Clang run substituted `out/build/linux-clang-debug/frontierx_native_shell`.
Both exited 0, reported ten external archive mounts and
`Native mesh archetype loaded; face groups: 13`, then closed their SDL dummy
windows cleanly. A first 45-second GCC attempt returned timeout code 124
and is excluded from the pass claim; a subsequent bounded 120-second run
exited 0. The external input is the user's lawfully installed Steam data;
none was copied into the repository. This is production CPU archetype
execution on retail input, not visible retail gameplay. Full-screen movie
video alone was explicitly omitted via the existing `--skip-movies` policy;
`IStreamer`, animation and other required services remained active.

The next concrete blocker is `CreateGlobalComponents` and its real App
camera/object-directory ownership before `Mission::ReloadMission` can
create a Forger unit in `Sol_Train.dmission`. The CQ2/released-source and
retail mission object schemas may differ; any such failure needs to be
classified against the Windows restoration baseline before attributing it
to Linux. No mission, visible GPU unit, input, simulation, save/reload,
native Windows Debug/Final or cross-module ABI validation ran for this gate.

## Production Forger Engine instance, 2026-09-27

**Blocker and real caller.** The App ship and platform owners call
`IMeshManager::CreateMesh`, which forwards to `MeshInstance::Initialize` and
`IEngine::create_instance2`. The former previously ignored an invalid
instance index and returned a non-null mesh. That could make subsequent
transform, animation or rendering calls dereference a missing Engine
instance. `MeshInstance::Initialize` now reports failure internally;
`MeshManager::CreateMesh` deletes the failed instance and returns null, and
its filename overload releases the newly acquired archetype on that path.
This changes no public interface, serialized layout or simulation order.

The existing `--mesh-object` diagnostic now continues from
`CreateMeshArch` to `CreateMesh`, checks that an Engine root appears and
disappears around `IMeshManager::DestroyMesh`, then releases the archetype.
For a compound object, the Engine root's archetype identifies its root
**part**, not the containing compound: an initial equality check rejected
the valid Forger instance (GCC exit 6). Source inspection of
`ENGINE::create_compound_instance` resolved that diagnostic mistake; the
final check requires a valid root-part archetype, without equating it to
the compound archetype. It also requires 13 nonzero Forger face groups in
the observed external run. No synthetic replacement Engine or mesh was
introduced.

GCC 16.2.1 and Clang 22.1.8 built the full Linux Debug presets and ran
their CTest presets: 44 checks executed and passed in each, while the two
GPU checks skipped in the command sandbox. The focused
`portability.native_shell` check was extended earlier for invalid object
requests and passed under both compilers. With the external Steam data
root, repository `Data/mat`, source `Conquest.ini`, SDL dummy video and
explicit `--skip-movies`, both compiler commands below exited 0:

```sh
SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-mesh-settings \
  out/build/linux-gcc-debug/frontierx_native_shell \
  --data-root "$LAWFUL_STEAM_DATA_ROOT" --material-root Data/mat \
  --system-profile Code/App/Src/Conquest.ini --skip-movies \
  --mesh-object ssforger.cmp --hidden --smoke-ms 1
```

The Clang command used `out/build/linux-clang-debug/frontierx_native_shell`.
Both printed `Native mesh instance created and destroyed; face groups: 13`
and closed the SDL dummy window cleanly. This is production
Engine/MeshManager instance execution on retail input; it is **not**
visible GPU output, mission loading, App gameplay simulation or an input
command. The lawful external payload remained outside the repository.
`IStreamer` remained in the source profile. The existing `--skip-movies`
policy disabled only optional full-screen `IVideoSystem` playback for the
bounded checkpoint; animation and other media contracts were not faked.

The next integration blocker is the App owner sequence. Source review
shows `CreateGlobalComponents` traverses at least 194 statically registered App
components, including `MenuResource::Startup` with Win32 menu/dialog calls
and `_camera::Initialize` with `MENU`/`FULLSCREEN` callbacks. The native
shell has neither their App globals nor a mission; the new Forger Engine
instance cannot be treated as the `Sol_Train.dmission` player's Forger.
The next step must connect the real App data-directory and camera owners
behind native platform services, then enter `Mission::ReloadMission` and
classify any CQ2/retail restoration mismatch against the Windows baseline.
Save/reload additionally requires the production UTF writer and Mission
persistence owners. Native Windows Debug/Final builds, cross-module ABI
checks and visible GPU execution were not performed for this change.

## Mission-owner native compile boundary (2026-09-27)

**Blocker and caller.** `Mission::ReloadMission` is the production entry for
the selected `Sol_Train.dmission`; `Mission::Reload` then calls
`LoadParseData` and `MGlobals::Load`. Existing filesystem and view-parser
checks execute those lower-level data contracts but cannot compile or invoke
the App mission owner. A direct translation-unit compile is therefore the
cheapest check that can disprove native owner readiness. `IMission` is also
a module-shared vtable; these changes add declarations/includes and correct
the existing `__stdcall` spelling without changing method order or stored
fields. Native Windows module and ABI checks are still required.

**Change.** Corrected on-disk include case in the mission's direct header
chain and replaced its Windows-only precompiled-header include with the
existing native App declarations on Linux. The `IMission` header now obtains
`M_RACE`, `UNIT_STANCE` and `FTSTATUS` from their defining headers and
forward-declares `IBaseObject`; the Win32 `FAR` qualifier on the two packet
pointer typedefs was removed, leaving the same pointer type. The source
retains the original PCH and DirectPlay includes on Windows. This does not
disable or pretend to implement networking, lobby, UI or mission services.

**Validation.** On Linux x86_64, `g++` 16.2.1 and `clang++` 22.1.8 each
compiled a translation unit including `FrontierX/LegacyFileTypes.h` and
`Mission.h` with `-std=c++17 -fsyntax-only` (exit 0). A direct GCC syntax
compile of `Code/App/Src/Mission.cpp` progressed through its direct includes
and stopped at `ZoneLobby.h:195`, missing `rpc.h` (exit 1); the bundled Zone
lobby interface is Windows/DirectPlay-specific and is used at several
multiplayer and mission-end call sites. Direct Clang syntax compilation
also fails before that include: `Sector.h` assumes PCH-provided `memset` and
`Transform`, while `NetPacket.h` assumes `PACKET_TYPE` from `Globals.h`.
These are additional source-closure errors, not passed checks. A direct
GCC syntax compile of
`MGlobals.cpp` stopped at `pch.h:29`, missing `windows.h` (exit 1). Thus neither
App implementation was built. Both GCC and Clang Debug native-shell and
view-parser targets were already up to date (`ninja: no work to do`). The
existing view-parser executable under each compiler correlated and
round-tripped the 3236-byte global record from the lawfully external
`Sol_Train.dmission` (exit 0); this remains parser/format execution, not
`MGlobals::Load` or `Mission::Reload`. `git diff --check` passed. Native
Windows Debug/Final, cross-module ABI, visible GPU gameplay, input,
simulation and save/reload were not performed.

**Next blocker.** Separate the single-player App loader from the bundled
DirectPlay/RPC lobby dependency at its owning call sites while preserving
explicit failure for unsupported network requests. Then compile the actual
`Mission.cpp` and `MGlobals.cpp` translation units, connect the required
camera/object-list/script owners, and execute `Mission::ReloadMission`.

## App declarations and script registration ABI (2026-09-27)

**Blocker and caller.** `Mission.cpp` includes `NetPacket.h` for commands and
`MScript.h` for program registration. Its native compile had no source of
`PACKET_TYPE` because that enum lived inside Windows `Globals.h`. The script
header asserted Win32 sizes for two in-process tables containing pointers;
on Linux x86_64 `CQSCRIPTENTRY` is 40 bytes rather than 24, and
`CQSCRIPTDATADESC` is 56 rather than 28. Those tables are runtime module
contracts, not serialized copies of a retail save record. The existing
data-root and parser checks cannot compile the App script interface; direct
header and translation-unit syntax checks resolve this integration decision.

**Change.** Moved the unchanged 67-value `PACKET_TYPE` enum into one header
shared by Windows `Globals.h` and native App declarations, with assertions
for representative command values. Shared the existing eight-player constant
through `AppObjectContracts.h`. Made the `Startup.h` registration declaration
available before its constructor and supplied native declarations for the
legacy menu/cursor handles and App resource globals. `Sector.h` now declares
the `Transform` pointer type it uses and includes the required C string
declarations. The native `Mission.cpp` branch no longer includes the unused
`BaseHotRect.h` and
includes `ZoneLobby.h` only for the Windows branch. Zone scoring calls in
the source are not yet ported or executed; network support remains absent.
`MScript.h` now keeps the Win32 size checks and adds native size/last-pointer
offset checks, with no DLL decoration for native static linkage. No saved or
packet field was widened.

**Validation.** A token-by-token comparison with the original `Globals.h`
found all 67 `PACKET_TYPE` enumerators identical and in the same order.
GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each compiled a translation
unit including the native legacy types, `CQTrace.h` and `MScript.h` using
`-std=c++17 -fsyntax-only` (exit 0); the native size and offset assertions
executed at compile time. Both full `linux-gcc-debug` and
`linux-clang-debug` builds completed, and each CTest preset executed 44
passing checks with two GPU checks skipped by the sandbox. In separate
120-second bounded processes, each compiler's native shell completed the
unchanged system profile on the user's lawful external Steam data with
`IStreamer` active and only `IVideoSystem` explicitly omitted by
`--skip-movies`; both opened `Sol_Train.dmission` through `SPMAPDIR`, observed
160633 ParseData and 3236 Globals bytes, created/destroyed the Forger
Engine/MeshManager instance with 13 face groups, and exited 0. This is
retail-input startup and object-instance execution, not visible gameplay.
Direct GCC/Clang syntax compilation of `Mission.cpp` still fails on other
App/Windows assumptions, including UI globals, Win32 declarations and an
incorrect-case `heapobj.h` include at this historical check. `MGlobals.cpp` still stops at the
Windows PCH. `git diff --check` passed. Native Windows Debug/Final, ABI
checks, visible GPU mission, normal input, simulation and save/reload were
not performed.

**Next blocker.** Give the actual `Mission.cpp` and `MGlobals.cpp` owners a
native App-global/platform boundary and compile them, preserving explicit
failure for unsupported Zone/DirectPlay paths. The subsequent integration
must initialize camera, sector, object list and native script registration
before `Mission::ReloadMission` can be a meaningful gameplay run.

## Repository App schema and mission source body (2026-09-27)

**Blocker and caller.** `Mission::ParseDefFile` consumes the `IDR_PARSER1`
resource from `Globals.rc`, which points to repository `Code/App/Src/data.i`.
Its Win32 `GetPreprocessData` uses `FindResource`/`LoadResource`; the native
App has no equivalent binding yet. `Mission::LoadParseData` separately reads
the selected external mission's embedded `ParseData/Data.h`. The two inputs
cannot be assumed equivalent merely because both parse as C-like schemas.
The existing generated view-parser check could not decide whether the App
resource itself parses on Linux; extending that check determines whether
the native binding can use the repository source or first needs a parser or
CQ2/retail restoration repair.

**Change.** Extended the existing view-parser executable with a bounded
`--app-schema` mode and registered a deterministic CTest against the
repository `data.i`. The mode reads at most 4 MiB, passes the source text
through production `IViewConstructor2::ParseNewMemory`, finds
`MT_GlobalData` and destroys its symbols. This remains a diagnostic for the
real `Mission::ParseDefFile` input; it is not a parallel schema parser.
The direct mission include chain now supplies the two packet enum types
from their defining headers, uses dependent-base lookup for `USR_PACKET`
members under GCC/Clang, and excludes three unused UI/agent/lighting
headers from the native branch while preserving the Windows include path.
The native declarations add pointer-sized `PVOID` and fixed-width
`DWORDLONG` aliases. No packet values or serialized fields changed.

**Validation.** GCC 16.2.1 and Clang 22.1.8 each built the entire Linux
x86_64 Debug graph and ran 45 passing CTests; two GPU checks skipped in the
command sandbox. The new `portability.app_schema` check exited 0 under both
compilers and reported a 3236-byte `MT_GlobalData` from the 165216-byte
repository `data.i`. The lawfully external selected mission's embedded
ParseData was previously measured at 160633 bytes and its global record
at 3236 bytes; the different schema-text lengths still require a
CQ2/retail compatibility assessment before equating all fields. Calls with
`--app-schema` and a missing path failed with exit 1 under both compilers;
the diagnostic does not report success without the resource. Direct
GCC/Clang `-fsyntax-only` compilation of `Mission.cpp` now passes its direct
include chain and reaches the source body, then fails on undeclared App
globals (`THEMATRIX`, `NETPACKET`, `DEFAULTS`), Win32 resource APIs in
`GetPreprocessData`, and other App platform assumptions. This is failed
source compilation, not a built or executed `Mission` owner.
`MGlobals.cpp` still stops at its Windows PCH. Native Windows Debug/Final,
cross-module ABI, visible GPU mission, input, simulation and save/reload
were not performed.

**Next blocker.** Bind the repository App schema through a native resource
owner at `GetPreprocessData` with explicit read/parse failure, and provide
the actual App globals/platform services required by `Mission.cpp` and
`MGlobals.cpp`. Only then can the production mission loader be linked and
entered; any mismatch between `data.i` and the retail mission should be
classified separately from Linux portability.

## Native App parser resource binding (2026-09-27)

**Blocker and caller.** `Mission::GetPreprocessData` supplies the
`Globals.rc` `data.i` bytes to `Mission::ParseDefFile`. The Win32 resource
API cannot run on Linux, while a returned parser success must mean the
required App schema really parsed. The existing `portability.app_schema`
check and shell mission-source path can exercise this boundary before the
full `Mission` owner is linkable.

**Change.** CMake generates an embedded, byte-for-byte native resource from
the repository `data.i`; `GetPreprocessData` and `ParseDefFile` call its
native adapter only on Linux. The shell binds the active App system
container to `GS`, queries its actual `IViewConstructor2`, and loads this
schema before opening the external mission source. Four `static_assert`
statements in the checked-in `data.i` were a pre-existing source/resource
compatibility error, rather than a Linux platform defect: DOCUVIEW
reported each as an undefined symbol, while `ParseMemory` returned `GR_OK`.
The assertions still exist in `DPlatform.h` for C++ layout checking; their
resource copies contained no schema fields and were removed. Native
`ViewConstructor::ParseMemory` now rejects parser errors and discards a
partial symbol list. The existing parser check verifies source/embedded
byte equality and rejects a malformed `static_assert` input before loading
the clean resource. The Windows parser and resource-loading branches retain
their previous behavior; native Windows rebuild remains required.

**Validation.** GCC 16.2.1 and Clang 22.1.8 each built
`frontierx_view_parser_probe` and `frontierx_native_shell` in Linux Debug.
Both `portability.view_parser` and `portability.app_schema` passed under
each compiler. Both full Debug builds and repeated CTest presets passed
45 executing checks and skipped two sandbox GPU checks each. The first
full test attempt found an outdated synthetic-shell expectation; its
minimal profile now correctly fails without a parser, and the updated
test supplies `IViewConstructor` to cover subsequent missing-global and
source-open behavior. Separate shell processes with SDL dummy video, the source
`Conquest.ini`, repository `Data/mat`, explicit `--skip-movies`, and lawful
external Steam `DATA` mounted ten archives, created required `IStreamer`,
parsed the embedded App schema without parser diagnostics, opened
`Sol_Train.dmission` through `SPMAPDIR`, observed 160633 ParseData bytes and
3236 Globals bytes, and exited 0. `IVideoSystem` was explicitly disabled
only for full-screen movies. This is production parser and retail-input
source execution, not execution of `Mission::ParseDefFile` or a mission.
Full build/CTest results are in [BUILD_GRAPH.md](BUILD_GRAPH.md); no visible
GPU mission, player unit, input, simulation, save/reload, native Windows
Debug/Final or cross-module ABI check was performed.

**Next blocker.** Compile and link the actual `Mission.cpp` and
`MGlobals.cpp` owners with their required App globals and services. A
direct `Mission.cpp` syntax compile still first fails at undeclared
`THEMATRIX`, `NETPACKET` and `DEFAULTS`; its later save helper also calls
Win32 disk-space APIs. Resolve those contracts at their owning boundary,
then enter `Mission::ReloadMission` on this schema and external source. Diagnose
any CQ2/retail field mismatch separately from Linux platform failures.

## Native mission ParseData owner contract (2026-09-27)

**Blocker and caller.** `Mission::LoadParseData` requires a secondary
DOCUVIEW schema from `\\ParseData\\Data.h` before `MGlobals::Load` and
`Mission::ReloadMission` can interpret a save. The original
`ParseNewMemory` could return a non-null list even after parser errors;
the mission caller treated that list as success. The previous shell path
checked only file presence and length, so it did not exercise this required
parse contract through the active App system.

**Change.** Native `ViewConstructor::ParseNewMemory` resets the per-parse
error counter, rejects parser errors or a missing parsed symbol list, and
destroys a partial list. A Linux-only App adapter performs the bounded
`OpenChild`/complete `ReadFile`/close/`ParseNewMemory` sequence and requires
`MT_GlobalData`; on success the caller owns and destroys the returned list.
The native branch of `Mission::LoadParseData` calls this adapter, while its
Windows branch is unchanged. The shell invokes the same adapter through
the profile-owned parser and `SPMAPDIR` mission source. This keeps the
file-system and parser contracts in production owners; it does not replace
the rest of mission loading.

**Validation.** GCC 16.2.1 and Clang 22.1.8 built the linked parser and
shell targets; focused `portability.view_parser`, `portability.app_schema`
and `portability.native_shell` checks passed under both. The existing
parser check now rejects a malformed secondary schema, and the existing
shell check rejects malformed mission ParseData through the profile-owned
parser before accepting a corrected generated schema. Both complete Linux
Debug build and CTest presets passed; the full counts and sandbox GPU skips
are in [BUILD_GRAPH.md](BUILD_GRAPH.md). Separate bounded GCC and Clang
shell processes on lawful external Steam data loaded the unchanged source
profile, required `IStreamer`, and the embedded App schema; they parsed
160633 bytes of selected mission ParseData without diagnostics, opened its
3236-byte Globals entry and exited 0. `IVideoSystem` was explicitly
disabled by `--skip-movies` only for full-screen movies. The external
source was read, not copied into the repository. Direct GCC and Clang
`Mission.cpp` syntax compilation still fails first at undeclared
`THEMATRIX`, `NETPACKET`, `DEFAULTS` and `FULLSCREEN`; therefore its newly
connected caller is source-review evidence, not a linked or executed
mission owner. Native Windows Debug/Final, cross-module ABI, visible GPU
mission, input, simulation and save/reload remain unperformed.

**Next blocker.** Provide real App global/service owners and the remaining
platform contracts needed to compile and link `Mission.cpp` and
`MGlobals.cpp`. Then enter `Mission::ReloadMission` with the selected
external mission and diagnose any CQ2/retail mismatch independently of
Linux compilation issues.

## MGlobals native owner compile boundary (2026-09-27)

**Blocker and caller.** `Mission::ReloadMission` calls `MGlobals::Load`
after loading the secondary ParseData schema. The native fixed-width
global-record adapter exists at that source call, but `MGlobals.cpp`
previously stopped before its body at the Windows PCH. A direct syntax
compile identifies the next real owner dependency without pretending that
the global-data load has executed.

**Change.** The native translation unit now includes the existing
`NativeAppGlobals.h` and diagnostic macro header instead of the Windows
PCH; the Windows include branch is unchanged. The native `MGlobals.h`
declaration includes its actual mission enum definitions. Corrected
case-sensitive `resource.h`, `ObjList.h`, `Filesys.h`, `HeapObj.h`,
`ENGINE.H` and shared `Document.h` includes, added a pointer-only
`IMeshArchetype` forward declaration, and left an unused `OpAgent.h`
include on the Windows branch. `Objwatch.h` now omits Windows DLL
decoration on Linux while retaining it on Windows. No serialized layout,
enum value, virtual interface or simulation order was changed.

**Validation.** Direct GCC 16.2.1 and Clang 22.1.8 C++17
`-fsyntax-only` commands with the App, DInclude, Libs and DOCUVIEW include
directories still exit 1, but now reach `MGlobals.cpp` body lines. GCC's
first diagnostics are the `ENGINE.H` `vis_state` forward enum, missing
`WINDOWPLACEMENT` base for `WindowPosStruct`, and `BASE_MAX_GAS` /
`BASE_MAX_METAL` / `BASE_MAX_CREW` constants; Clang reports the same
initial class/constant boundary. Later errors include Win32
`GetWindowPlacement`, `DEFAULTS`, `EVENTSYS` and App event constants.
The direct command used with each compiler was:

```sh
g++ -std=c++17 -fsyntax-only -D__stdcall= \
  -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude \
  -I Code/Libs/Include -I Code/Libs/Src/Include \
  -I Code/Libs/Src/Viewer/DOCUVIEW Code/App/Src/MGlobals.cpp
# Repeat with clang++ in place of g++.
```

The failure is a measured compile result, not a linked native owner. Both
full Linux GCC and Clang Debug builds and CTest presets passed after these
header changes; [BUILD_GRAPH.md](BUILD_GRAPH.md) records the full counts.
No external data was needed for this compile check. Native Windows
Debug/Final, module ABI, `MGlobals::Load` runtime, mission startup,
visible unit, normal input, simulation and save/reload were unperformed.

**Next blocker.** Establish the required App startup owners through the
`System.cpp` global-component lifecycle, then resolve `MGlobals`'s
development `GlobalViewer` window dependency without discarding its
required `GT_GLOBAL_VALUES` loading behavior. Avoid dummy global pointers:
the production mission caller dereferences them during load and update.

## Production general-data owner (2026-09-27)

**Blocker and decision.** The Windows App creates the production `GENDATA`
service before its mission lifecycle; `GlobalViewer::createViewer3` consumes
its `GlobalValues` archetype for the development viewer. The native shell
previously had no App general-data owner. This establishes the real service
without claiming that the development viewer is needed for gameplay. The first external run of
the linked `GenData.cpp` owner found 390 entries but zero aggregate bytes:
native UTF `FindFirstFile` omitted `nFileSizeLow`, which production
`GenData::get_total_bytes` uses for allocation. This was a Linux filesystem
contract defect, separate from any CQ2/retail gameplay restoration error.

**Change.** Native UTF directory entries now carry their validated stored
byte length into `WIN32_FIND_DATA`; unmatched searches set
`ERROR_FILE_NOT_FOUND`, allowing the native GenData traversal to distinguish
empty directories from malformed searches. The Linux-only `GenData` factory
loads the external read-only UTF source through the production owner,
bounds the allocation, checks every file's reported length and complete
read, and requires the `GlobalValues` archetype to match
`sizeof(GT_GLOBAL_VALUES)` (1640). The shell binds its resulting
`IGeneralData` pointer with retained interface ownership. The existing
generated-UTF material test now checks a six-byte file's enumeration size
and the unmatched-search error. The existing shell process check rejects
an invalid `--gen-data` UTF archive with exit 6 instead of reporting a
loaded service; no new standalone probe was added. These
runtime types are not serialized records or cross-module ABI structures.
The Win32 general-data lifecycle and archive bytes are unchanged.

**Validation.** `cmake --build --preset linux-gcc-debug --target
frontierx_native_shell frontierx_state_material_probe` and the analogous
Clang target build succeeded on Linux x86_64 (GCC 16.2.1, Clang 22.1.8).
Both shell processes used the supplied external Steam installation's
`gendata.db`, data archives and `Sol_Train.dmission` with `--gen-data`,
`--mission-source` and `--skip-movies`; each exited 0. Each loaded the
1640-byte `GlobalValues` through production `GenData`, parsed the
mission's 160633-byte secondary schema and opened its 3236-byte global
record. The GameSystem profile created required `IStreamer` and disabled
only optional full-screen `IVideoSystem` at the explicit movie-skip flag.
This is retail-input general-data and schema runtime, not a started mission
or a verified retail gameplay state. The focused
`portability.state_material` and `portability.native_shell` checks passed
under both compilers, including a repeated focused run after the invalid
archive assertion. Full `cmake --build --preset linux-gcc-debug` and
`linux-clang-debug` builds succeeded; both full CTest presets passed 45
executing tests, with two GPU tests skipped in the command sandbox. Those
skips are not GPU execution evidence. The external shell runs are the
runtime evidence for this general-data change, with SDL's dummy video
driver and no visible GPU gameplay.

**Next blocker.** Compile and link the production `MGlobals.cpp` owner with
actual App services, isolate its development-only Win32 `GlobalViewer`
windows while retaining the real general-data owner, and enter its
`Load` path from the real mission owner. Normal input, a visible Forger,
simulation, save/reload and visible GPU gameplay remain unexecuted. Native
Windows Debug/Final and cross-module ABI checks have not run.

## MGlobals global-values source contract (2026-09-27)

**Blocker and change.** Source review found that the simulation reads the
file-scope `globalValues` object in upgrade, movement and damage formulas.
On Windows, `updateGlobalViewer` obtains that object indirectly by opening
the development viewer or falling back to `GENDATA::GetArchetypeData`.
Excluding the viewer without loading those values would silently change
gameplay. The Linux-only `updateGlobalViewer` now retrieves and copies the
1640-byte `GlobalValues` from the production `IGeneralData` owner and
reports failure on missing/wrong-sized data; `MGlobals::New` and `Load`
propagate that failure. The Windows branch keeps the existing viewer and
initialization behavior. Native translation omits Win32 window placement,
menu, registry and document callbacks only; no simulation formula or
serialized field was changed. The native source uses the same three base
resource constants defined by the Windows App globals (250 crew, 220
metal, 180 gas) and includes the defining camera enum before `ENGINE.H`.

**Validation.** Direct GCC 16.2.1 and Clang 22.1.8 C++17 `-fsyntax-only`
checks of `Code/App/Src/MGlobals.cpp` with the App, DInclude, Libs and
DOCUVIEW include roots still exit 1, but now pass the Win32 viewer and
required-values source path into gameplay methods. The first errors are
unresolved `_localLoadStringW`, `SCROLLTEXT`, `PLAYERID`, `HOSTID`,
`THEMATRIX`, `__max`, `ARCHLIST` and CRT string helpers. `MGlobals.cpp`
is not in the native CMake graph, and `New`/`Load` did not execute. Both
full GCC/Clang Debug builds and CTests reported above cover the linked
general-data owner, not this source-only `MGlobals` change. Native Windows
Debug/Final, cross-module ABI and all five gameplay acceptance checks
remain unperformed.

**Next blocker.** Supply the actual App owner declarations and services
needed by those gameplay methods, then link `MGlobals` with its real
callers. A successful syntax check alone will not establish mission
execution or save/reload behavior.

## Complete MGlobals compile and link closure (2026-09-27)

**Named blocker and caller.** The real `Mission::Reload` calls
`MGlobals::Load` after its secondary schema is parsed. Native
`MGlobals.cpp` previously could not be compiled as a whole; success at
the source `Load` branch alone did not prove that the owner could join
the App graph. The existing native shell cannot cover this because it
parses mission data without linking `Mission.cpp` or `MGlobals.cpp`.

**Change.** The native App declaration header now declares the original
external App services and localization/build-version signatures, without
defining placeholder objects. `MGlobals.cpp` gets the four existing event
IDs/timing constants it uses, and native MSVC min/max behavior. The
production `OpAgent.h` include has correct case and a native opaque
integer enum declaration; the Windows declaration stays unchanged.
Three alliance messages use bounded native wide formatting, and the
part-name hexadecimal prefix uses a bounded uppercase 32-bit conversion
on Linux. Four `COMPTR<IFileSystem>` null checks now use the existing
boolean operator, preserving their Windows condition. The original
object-watcher volatility constants moved unchanged into the shared App
object contract; `Objwatch.cpp` now compiles with the native App include
branch and is in the same CMake object target. Both explicit Linux build
presets include that object target so future full builds recheck it. No persistent format,
interface vtable, simulation formula or update order was intentionally
changed. The part-name and wide-string differences still require retail
comparison before gameplay parity can be claimed.

**Validation.** `cmake --preset linux-gcc-debug` and
`linux-clang-debug`, followed by `cmake --build --preset` for each full
Linux x86_64 Debug graph, succeeded with GCC 16.2.1 and Clang 22.1.8.
Both `frontierx_app_mglobals_owner` object builds succeeded; direct
`-fsyntax-only` checks of the production `MGlobals.cpp` and
`Objwatch.cpp` also exited 0 under both compilers. A temporary shell
link with this full owner and function-section garbage collection failed:
16 real App symbols were unresolved, including `MISSION`, `EVENTSYS`,
`ARCHLIST`, `THEMATRIX`, `SAVEDIR`, `SCROLLTEXT`, player/host IDs,
localization/build-version calls and `UninitObjectPointer`. The linked
experiment preceded addition of `Objwatch.cpp`, which supplies the watcher
function; the remaining full link closure has not yet been remeasured.
The experiment was removed; no placeholder definitions or successful-result
shims were added. The final full GCC and Clang CTest presets each passed
45 executing checks and skipped two sandbox GPU tests. Separate final
shell processes on the lawfully external Steam data and selected
`Sol_Train.dmission` each exited 0 after production GenData loading
(1640-byte `GlobalValues`) and mission schema parsing (160633-byte
ParseData, 3236-byte Globals). These executions do not enter the new
`MGlobals` target. Native Windows Debug/Final and cross-module ABI checks
were not performed.

**Next blocker.** Link the required App service owners with correct
lifetime, beginning with `Mission.cpp` and its real `IMission`
correlation contract, then execute `MGlobals::Load` through
`Mission::Reload`. Direct syntax compilation of `Mission.cpp` currently
first fails on the incomplete `IOpAgent` type and missing `NETPACKET` /
`DEFAULTS` declarations; its Windows disk-space helper is another
measured native compile barrier. A visible unit, normal movement input,
simulation checkpoint, save/reload and visible GPU gameplay remain open.

## Mission save-space platform contract (2026-09-27)

**Blocker and real caller.** Full `Mission.cpp` syntax compilation first
stopped in its `SetAIResign`/constructor path on missing App service
declarations and then in `Mission::getDiscFreeSpace`, called by the real
`Mission::Save`, on Win32 Kernel32 dynamic loading and disk-space APIs.
An unchecked replacement could let the save path report success despite
an unavailable space check. The existing data-root check could exercise
the platform filesystem without adding a parallel mission probe.

**Change.** The native App declaration header now declares the original
`IUserDefaults`, `IFileTransfer` and `INetPacket` service pointers and
the source `OpAgent.h` interface is included on Linux. The Win32
`BaseHotRect` development/UI class remains out of the native compile
path; only its Win32 event unsubscription is guarded. The
`CQE_LOAD_TOOLBAR` value is the original `0x102A` on the native branch.
The new focused platform function `AvailableDiskBytes` uses standard
filesystem space reporting, returns failure without modifying output
for missing/error paths, and is called by the Linux-only
`Mission::getDiscFreeSpace` branch. Linux `Mission::Save` returns false
if the free-space query fails or yields less than one MiB; the low-space
diagnostic is printed without requiring a Win32 message box. A native
save filename comparison uses exact case for the case-sensitive Linux
filesystem; the Windows call remains `stricmp`. No save file layout,
simulation state or Windows helper behavior changed.

**Validation.** `cmake --preset linux-gcc-debug` and
`linux-clang-debug` plus both full `cmake --build --preset` commands
succeeded on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8. The existing
`frontierx_data_root_probe` self-test, registered as
`portability.data_root`, passed under each compiler after checking a
temporary existing directory and a missing child: the latter returned
failure and left its output untouched. Both full CTest presets passed
45 executing tests and skipped two sandbox GPU tests. Separate final
shell processes using the lawful external Steam archives, `gendata.db`
and selected `Sol_Train.dmission` exited 0 after GenData loading and
mission schema parsing; the mission was not started. Direct GCC and
Clang `-fsyntax-only` commands for `Code/App/Src/Mission.cpp` still exit
1, but their first diagnostics moved to the real `RecursiveDelete`,
camera, nugget, music and fog owners in the save body. The production
`Mission.cpp` is not in the native graph, so neither its new disk-space
call nor a save/load operation executed through that caller. Native
Windows Debug/Final, module ABI, visible GPU gameplay, normal input and
simulation checks remain unperformed.

**Next blocker.** Establish the real App owner/service closure for
`Mission::Reload` and `MGlobals::Load`. The source still depends on
`System.cpp`'s `RecursiveDelete`, camera, nugget, music and fog owners,
and later Win32 menu/message paths. The current native physical and UTF
filesystem owners are read-only; a writable save path and `IUTFWriter`
remain necessary before save/reload can satisfy the acceptance criterion.
As on the current Windows call, a null disk-space path checks the process
working directory; the eventual save target must be checked when its
native writable filesystem owner is selected.

## Mission QuickSave object input boundary (2026-09-27)

**Blocker and caller.** Source review of the real `ObjectList::Load` shows
that `Sol_Train.dmission` uses `\\ObjectList\\QuickSave`, then enumerates
archetype directories with `FindFirstFile("*.*")`, creates nested DOS
filesystem views, loads each archetype, creates an object and calls its
`IQuickSaveLoad::QuickLoad`. The lawfully external mission contains a Forger,
two Taos ships and an Acropolis in that tree. The reported Windows mission
without units is therefore an unresolved CQ2/retail-restoration result, not
a valid gameplay reference or proof that the Linux filesystem is missing
these records. The existing data-root probe already opened the mission and
its parser/global files, so extending it was the narrowest check of this
specific input-side blocker.

**Change and evidence.** The probe's `--mission-filesystem` mode now checks
QuickSave enumeration, DOS child views and complete reads of
`FAB!!S_Forger/MT_QSHIPLOAD/Forger` (12 bytes),
`GBOAT!!S_Taos/MT_QSHIPLOAD/Taos1` and `Taos2` (12 bytes each), and
`PLATBUILDSUP!!S_Acropolis/MT_PLATFORM_QLOAD/Acropolis` (8 bytes). It
rejects absent entries, incorrect sizes and incomplete reads. A Linux-only
`Mission::ReloadMission` source branch bounds its selected mission filename
and extension, rejects an empty race-derived name or missing `SPMAPDIR`,
and retains the Windows `wsprintf` path. It is not linked or executed.

**Validation.** Linux x86_64 GCC 16.2.1 and Clang 22.1.8 focused
`frontierx_data_root_probe` builds succeeded. Each external-input
`--mission-filesystem DATA Sol_Train.dmission` process exited 0 and reported
four complete QuickSave records, 160633 ParseData bytes and 3236 Globals
bytes. The existing `portability.data_root` CTest passed under both
compilers with temporary synthetic inputs. Both full Debug builds
succeeded; each full CTest preset passed 45 executing checks and skipped
two GPU checks in the command sandbox. Separate GCC and Clang native shell
processes on the lawful Steam installation exited 0, loaded the 1640-byte
`GlobalValues`, parsed the mission schema and explicitly reported that the
mission was not started. Direct C++17 syntax checks of `Mission.cpp` still
exit 1, beginning at undeclared `RecursiveDelete`, `CAMERAMANAGER` and
`NUGGETMANAGER`; they cannot validate the unlinked reloader branch. Native
GCC and Clang direct syntax checks of `ObjList.cpp` also exit 1 at its
Windows-only `pch.h` inclusion of `windows.h`, before the body compiles.
Windows Debug/Final, module ABI, `ObjectList::Load`, visible mission units,
normal input, simulation, save/reload and GPU gameplay were not executed.

**Next blocker.** Link the real App mission and global service closure,
then open the Windows PCH/`CommCtrl.h` compile boundary of production
`ObjectList` and link its archetype/object factories before
claiming unit creation. Source review also finds `ObjectList::GetArchName`,
`GetArchetypeData`, `GetArchetypeDataID` and `LoadArchetype(U32)` forming
runtime archetype offsets through `U32(pointer)` in `ObjList.cpp`; this
truncates native LP64 pointers. Its `ARCHDATATYPE` embeds a pointer, so a
replacement ID mapping needs a save/module-ABI audit before changing the
32-bit identifier contract. The native filesystem is currently read-only;
production persistence also needs a writable UTF owner.

## Object-list native source boundary (2026-09-27)

**Blocker and caller.** `Mission::Reload` ultimately calls the production
`ObjectList::Load` to turn the selected training mission's QuickSave entries
into live units. Before this change, direct Linux syntax compilation stopped
at the owner's unconditional Windows PCH (`windows.h`). The prior external
mission filesystem check had already verified that the Forger and companion
object records are reachable; it cannot exercise archetype construction or
`IQuickSaveLoad::QuickLoad`.

**Change.** `ObjList.cpp` now retains its original Windows PCH/global and
Win32-only header path while using the existing native App declarations on
Linux. Its source includes use the actual on-disk case and order the
required component, camera, nugget, debug and renderer declarations before
their callers. The native App declaration list gains only the original
`SFXMANAGER` and debug iterator pointer contracts; neither is given a
placeholder definition. `ObjectImpl<Base>::QueryInterface` now qualifies
the dependent inbound-map lookup with `Base::`, preserving its map and
offset behavior while making the template valid for GCC/Clang.

**Validation.** Direct C++17 `-fsyntax-only` checks of `ObjList.cpp` on
Linux x86_64 with GCC 16.2.1 and Clang 22.1.8 still exit 1, but now parse
past all direct includes into `REPORTFONT` and `ObjectList` methods. GCC's
first errors include the Win32 `__declspec(dllimport)` import and
`VirtualAlloc`/`VirtualFree` constants; Clang also rejects that import, then
reports Win32 UI/timing types and services. Both full Linux Debug build
presets succeeded with the changed shared header/template; each full CTest
preset passed 45 executing checks and skipped two GPU checks in the command
sandbox. These builds do not compile `ObjList.cpp`; no external mission
runtime or `ObjectList::Load` execution occurred in this change. Native
Windows Debug/Final, module ABI, visible unit, input, simulation and
save/reload were not run.

**Next blocker.** Separate the optional Win32 debug font, dialog and
timing facilities from the required object loading and simulation paths,
then compile the full owner and link its real archetype/object factories.
Keep the `U32(pointer)` archetype-ID representation under separate
save/ABI review before executing native QuickLoad or persistence.

## Object-list debug and clock declarations (2026-09-27)

**Blocker and caller.** After opening `ObjList.cpp`'s native include chain,
the earliest body errors were its imported Windows batch heap,
`VirtualAlloc`-backed performance-report font, and MSVC `__int64` clock
declarations. The report is a developer diagnostic; neither it nor the
archetype editor dialog participates in the chosen training-mission
movement checkpoint. `ObjectList::Load`, `BeginFrame`, `Update` and normal
input remain required production work.

**Change.** The `GetBatchHeap` import and `REPORTFONT` type now remain on
the original Windows branch. The performance-warning declaration, body
and both call sites are Windows-only; native gameplay does not claim a
working performance report. The object clock fields and locals now use
existing `U64`/`S64` typedefs, which preserve 64-bit widths on Windows
and Linux without changing serialized data. The archetype editor dialog
members, Win32 callback/helper bodies, and presentation calls are guarded
on Windows; the mission input handler is still present. The checksum
message to the Windows main window remains Windows-only, and a native
checksum consumer is still required for full parity. No frame-time algorithm,
simulation order or save layout was intentionally changed.

**Validation.** Direct GCC 16.2.1 and Clang 22.1.8 C++17
`-fsyntax-only` checks of `ObjList.cpp` on Linux x86_64 still exit 1, but
their first diagnostics now occur at `FULLSCREEN` event ownership in the
destructor and the Win32 performance counter in `BeginFrame`, rather than
the optional report, MSVC clock fields or editor dialog. The dialog's
member declarations, implementation and presentation calls remain on
Windows only; `ObjectList::Notify` remains in the native translation.
Both full Linux Debug build presets succeeded with no rebuild work;
`ObjList.cpp` is not a target and these builds do not validate its body.
No mission startup, object creation, normal movement, simulation,
save/reload, native Windows Debug/Final or module-ABI check was executed
for this change. The prior two GPU CTest skips are not runtime evidence.

**Next blocker.** Bind the real input/event owner in place of
`FULLSCREEN` and supply a native monotonic clock through the existing
platform-time owner, then compile the required object-loading methods.
`ObjectList::Notify` must retain normal selection and move input; do not
report success while required archetype factories or QuickLoad remain
missing.

## Object-list native frame-clock source contract (2026-09-27)

**Blocker and real caller.** Production `ObjectList::BeginFrame`,
`EndFrame`, `Update`, and its startup initializer use Windows
`QueryPerformanceCounter`/`QueryPerformanceFrequency` and `Sleep(0)`.
They control elapsed render time and the existing update cadence; a
stub clock would make a linked object list appear to run while preventing
real simulation progress. The existing platform-time owner already
provides a monotonic microsecond clock and yield operation.

**Change.** The Linux-only source adapter gives the object's original
counter calls microsecond ticks and a matching fixed 1,000,000-Hz
frequency, with `Sleep(0)` routed to the existing platform yield. The
Windows calls are unchanged. `NativeAppGlobals` now owns the original
`DEF_ELAPSED_TIME`, `DEF_REALTIME_FRAMERATE`, `RENDER_PERIOD`,
`ELAPSED_TIME` and `REALTIME_FRAMERATE` contracts required by
`BeginFrame`; the two mutable native globals start at the original
defaults. The adapter checks its output pointers and copies exact
eight-byte ticks without aliasing an unsigned clock value through a
signed pointer. No persistent field width, tick update ordering or
floating-point expression was changed.

**Validation.** Linux x86_64 GCC 16.2.1 and Clang 22.1.8 full Debug
builds succeeded. Each full CTest preset passed 45 executing checks
and skipped two GPU checks in the command sandbox. The existing
`portability.platform_time` test also passed explicitly under both
compilers, exercising the monotonic platform service and yield; it does
not invoke `ObjList.cpp`. Direct C++17 `-fsyntax-only` checks of the
production object source still exit 1, but now pass `BeginFrame`'s
counter/frequency declarations and fail first on `FULLSCREEN` event
ownership and the real `NUGGETMANAGER`, `CAMERAMANAGER` and `SCHEDULER`
services in `Update`. Separate GCC and Clang native shells using the
lawful external Steam installation each exited 0 after loading the
1640-byte GenData `GlobalValues` and parsing the selected mission's
160633-byte ParseData and 3236-byte Globals records. Both explicitly
reported that the mission was not started. The source adapter was not
linked into either shell, so neither a production frame nor gameplay
executed. Native Windows Debug/Final, module ABI, visible unit, normal
input, simulation, save/reload and GPU gameplay remain unperformed.

**Next blocker.** Source review locates `FULLSCREEN` in the production
`Menu_Toolbar.cpp` `FullScreen` UI owner; `ObjectList` subscribes to
its outgoing `IEventCallback` connection point. The native SDL
`IWindowManager` delivers window messages but does not itself supply
that UI connection point. Connect the real input/event owner and
compile/link the required nugget, camera and scheduler services. The selected mission's Forger
still requires production archetype factories, QuickLoad and the
LP64-safe 32-bit archetype-ID mapping before it can move or persist.

## Native object-list event-owner contract (2026-09-27)

**Blocker and caller.** The real `ObjectList` destructor and
`_olist::Initialize` subscribe/unsubscribe to the outgoing
`IEventCallback` connection point of the Windows `FULLSCREEN`
`BaseHotRect`, created by `Menu_Toolbar.cpp`. The native SDL window owner
already translates input to Win32-shaped messages; the shell callback
sends these records through the active production `IEventSystem`.
That event system exposes its own outgoing `IEventCallback` point.
Before this change, App `EVENTSYS` was only declared, not bound, and
the temporary App system binding ended before the shell's event loop.

**Change.** `NativeAppSystemBinding` now queries and retains the active
system container's real `IEventSystem` transactionally and releases it
on teardown. The shell keeps that binding across the event loop and
checks it matches the event instance it initializes. Linux-only
`ObjectList` source code subscribes its existing callback to this
connection point, fails explicitly if required subscription cannot be
created, and unsubscribes on destruction. The Windows `FULLSCREEN`
chain and priority call remain unchanged. Native App declarations now
name the original nugget, movie-camera and event-scheduler service
interfaces without defining placeholder instances or altering their
vtable contracts. This source connection alone does not reproduce the
Windows `FullScreen` UI's hit-test filtering; its effect on normal
selection must be verified when the object owner actually runs.

**Validation.** GCC 16.2.1 and Clang 22.1.8 full Linux x86_64 Debug
builds succeeded, and final focused `frontierx_native_shell` rebuilds
and `portability.native_shell` CTests passed after the binding error-path
cleanup. Each full CTest preset passed 45 executing checks and skipped
two GPU checks in the command sandbox before that final error-path edit;
the focused shell check was repeated afterward. Separate final GCC and
Clang shell processes on the lawful external Steam installation exited
0 after binding the App system/event owner, loading 1640-byte GenData
`GlobalValues` and parsing the selected mission's 160633-byte ParseData
and 3236-byte Globals records. They explicitly reported that the
mission was not started. Direct C++17 GCC/Clang syntax checks of
`ObjList.cpp` still exit 1, but reach `Update` and first identify the
real particle/fog and render-state services (`PARTMAN`, `FOGOFWAR`,
`DisableTextures`, `PB`) rather than `FULLSCREEN` or the nugget/camera/
scheduler declarations. `ObjectList` did not compile as a target,
subscribe at runtime, create a unit or receive normal gameplay input.
Native Windows Debug/Final, module ABI, visible gameplay, simulation,
save/reload and GPU mission execution remain unperformed.

**Next blocker.** Connect the actual particle/fog and game render-state
owners in the App runtime, then compile the full object-list source and
link its archetype factories. After that, verify the shell's legacy
window-message payload against the `MSG` fields read by
`ObjectList::Notify` and test selection/move through ordinary SDL input;
do not infer that behavior from the binding alone.

## Native GameTypes source binding (2026-09-27)

**Blocker and owner.** `ObjectList::loadTypesData` loads archetype
records from `GameTypes.db` before its object factories can create the
Forger in `Sol_Train.dmission`. The native component manager did not
register a file factory for that database. Source review found the
lawfully external Steam installation's `gametypes.db`; the native UTF
filesystem can open it, but the App object owner is still outside the
linked native shell. The chosen mission and its select/move, `SelectAll`,
save/reload acceptance sequence remain as specified above.

**Change.** `--game-types FILE` explicitly opens the read-only external
UTF archive. The shell checks `BT_FABRICATOR_DATA/FAB!!S_Forger` through
the legacy `IFileSystem` interface and rejects a missing or empty record.
The retained App binding exposes that archive to the native branch of
`ObjectList::loadTypesData`; a missing binding returns failure. The
Windows `DACOM->CreateInstance` search and database format are unchanged.
The GenData owner/binding lifetime was also extended through the shell
event loop so a later object owner will not observe a dangling
`GENDATA`. The native selection source uses the full window when no
toolbar owner exists; actual input and toolbar hit testing await runtime
validation. Native source declarations for the original particle, fog,
render and UI services are declarations only; no successful placeholders
were installed. Native source now translates the original U32 Win32 byte
offsets (`4 + 44 * index`) to LP64 `ARCHDATATYPE` table positions and
rejects out-of-range or misaligned IDs rather than narrowing pointers.
At this gate, `ObjList.cpp` remained outside CMake; direct C++17 syntax checks reached
`ObjectList::Notify` before failing on the legacy input/event contract.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 built the
native shell and complete Debug presets. Each full CTest preset ran 45
passing checks and skipped two command-sandbox GPU tests; the existing
`portability.native_shell` process test passed on both compilers and
rejects an invalid `--game-types` archive with exit 6. Separate shell
processes used the lawful external Steam `DATA`, `gendata.db` and
`gametypes.db`, the checked-in `Conquest.ini` profile and
`--mission-source Sol_Train.dmission --skip-movies --smoke-ms 1 --hidden`.
Each exited 0 after opening the 856-byte Forger type record, loading
1640-byte GenData `GlobalValues`, parsing 160633 bytes of mission schema
and opening the 3236-byte Globals record. The shells reported "mission
not started"; dummy SDL video did not render a unit. `IStreamer`
initialized as a required profile aggregate. Only optional full-screen
`IVideoSystem` was expressly disabled by `--skip-movies`. Native Windows
Debug/Final and cross-module ABI checks, `ObjectList::loadTypesData`
execution, visible GPU gameplay, normal move input, simulation,
deterministic mission checkpoint and save/reload were not performed.
GCC `-m32` and Clang `-target i686-pc-linux-gnu` compile-time checks of
the equivalent Win32-shaped record found a 44-byte stride and a four-byte
array offset; these checks do not establish the supported MSVC ABI.

**Next blocker.** Match the shell's `LegacyWindowMessage` to
`ObjectList::Notify` without truncating pointer-sized message payloads;
then finish native object-owner compilation and link the real object and
archetype factories. Measure the supported Win32/MSVC archetype-ID ABI
before treating native save/network IDs as parity evidence.

## Native object-owner build and link closure (2026-09-27)

**Blocker and owner.** The external GameTypes archive could not be
consumed by mission objects while `ObjList.cpp` failed native
translation. The real callers are `ObjectList::loadTypesData`,
`ObjectList::Load` and its `IEventCallback::Notify` input path; a shell
archive-open check alone cannot create or move the Forger. The local
hypothesis was that the remaining source errors were native message
layout, pointer-width and App declaration barriers, after which a
direct CMake object build would expose the actual link closure.

**Change.** The native `ObjectList::Notify` reads the shell's
`LegacyWindowMessage` fields for mouse events and rejects missing or
inconsistent envelopes. Its integer-ID events check that an encoded
pointer value fits U32 before use. The native App binding retains the
active `IWindowManager` as `WM` alongside `IEventSystem` and the shell
checks its identity. The native object-list source uses bounded save
path formatting, excludes the Windows-only editor viewer action with
an explicit unsupported result if invoked, and has no toolbar slop
region. The Windows event, file and UI branches remain unchanged.
`frontierx_app_object_list_owner` is now an object target in CMake. It
is intentionally not linked into the successful shell; required
services are not replaced with success stubs. The Win32-shaped U32
archetype-ID adapter remains source/compile evidence only.

**Validation.** `cmake --preset linux-gcc-debug` and
`cmake --preset linux-clang-debug`, followed by builds of
`frontierx_app_object_list_owner` and the complete presets, succeeded
on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8. Both full CTest
presets passed 45 executing checks and skipped two GPU checks in the
command sandbox. The existing shell test includes invalid GameTypes
rejection. Separate GCC and Clang shell processes with lawfully external
Steam `DATA`, `gendata.db`, `gametypes.db`, the checked-in source profile
and `Sol_Train.dmission` exited 0. Both opened the 856-byte Forger type,
loaded 1640-byte `GlobalValues`, parsed 160633 bytes of mission schema,
and opened the 3236-byte Globals record; each reported "mission not
started". `IStreamer` initialized as required; `IVideoSystem` remained
explicitly disabled for optional full-screen movies. A temporary GCC
shell link including the new object failed with 251 unresolved-reference
occurrences across 80 distinct symbols, led by App startup/cleanup,
`MGlobals`, `MISSION`, nugget/camera/scheduler, render and object
support. The temporary link source was removed and the complete builds
and external shell runs were repeated successfully. The object target
compiled only: `ObjectList` did not execute or subscribe, and no mission
unit, normal move input, simulation checkpoint or save/reload ran.
Native Windows Debug/Final, MSVC module/ABI, visible GPU mission and
retail gameplay checks were unperformed. The Windows mission-without-
units report remains a separate CQ2/retail-restoration issue, not a
gameplay reference for Linux.

**Next blocker.** Compose the production App startup and global owners
needed to instantiate `ObjectList`, beginning with
`AddToGlobalStartupList`, `MGlobals` and the mission-owned services
visible in the failed link. Do not make the source-only object target
count as mission execution. After that, run `loadTypesData` on the
external archive, create the Forger through the actual factory, and
exercise selection/move through the ordinary event callback before
claiming any gameplay condition.

## Production mission-owner native build gate (2026-09-27)

**Blocker and production caller.** `Conquest.cpp` invokes
`CreateGlobalComponents()` before opening a mission. The `_mission`
registrar in `Mission.cpp` creates the real `IMission` owner, connects its
event and file-transfer callbacks, and its `Load`, `Update`, `Save` and
`Reload` methods call the production simulation owners. The previous
source-only check failed before this registrar on Win32 UI, event, clock
and DLL-entry declarations. The falsifiable local step was a GCC/Clang
CMake object build, without substituting any required services.

**Change.** The native source uses the existing monotonic platform clock
for the mission counter and the shell's pointer-sized legacy input
message. Native event IDs that the mission interprets as U32 are checked
before narrowing. The source retains the real music, parser, map,
lighting, progress and file-transfer dependencies. Native window-title
decoration and Windows-only menu/dialog operations are excluded; an
unsaved or active mission application-close request is refused without
native confirmation UI; `Mission::close` itself refuses unsaved data, and a
deferred ending slide refuses to close until its renderer exists.
The Windows module initializer, window callback and UI paths remain
under `_WIN32`. A new CMake object target compiles this original
mission owner; it is not linked into the native shell.

**Validation.** On Linux x86_64, `cmake --preset linux-gcc-debug` and
`cmake --preset linux-clang-debug`, followed by
`cmake --build --preset linux-gcc-debug --target frontierx_app_mission_owner`
and the equivalent Clang target, succeeded. Both complete builds and
CTest presets succeeded: 45 executing tests passed, and two GPU tests
skipped in the command sandbox. Separate GCC and Clang native shell
processes used lawfully external Steam `DATA`, `gendata.db` and
`gametypes.db`, the checked-in profile, `--skip-movies`, and
`--mission-source Sol_Train.dmission`; both exited 0 after opening the
856-byte Forger GameTypes record, 1640-byte GenData `GlobalValues`,
160633-byte mission ParseData and 3236-byte Globals record. Both printed
"mission not started". `IStreamer` was initialized as a required
aggregate; only optional full-screen `IVideoSystem` was expressly
disabled by `--skip-movies`. Dummy SDL video produced no GPU gameplay.
Native Windows Debug/Final, module ABI, mission execution, visible unit,
normal movement input, simulation checkpoint, save/reload and retail
gameplay were not performed. Existing pointer-cast warnings remain in
legacy smart-pointer code and native mission event arguments.

**Next blocker.** Link and initialize the real App global startup owner,
then satisfy the measured object-list and mission service closure
without success stubs. Only after `IMission::Load` creates the Forger
from the external archive can input, simulation and save/reload be
tested. The Windows mission-without-units observation remains a separate
CQ2/retail-restoration defect rather than a Linux gameplay reference.

## Shared App global startup and cleanup owner (2026-09-27)

**Blocker and production caller.** `Conquest.cpp` calls
`CreateGlobalComponents()` before mission loading. Static registrars in
`Mission.cpp` and `ObjList.cpp` use `AddToGlobalStartupList`; their owners
need the same two-phase order and `CleanupGlobals` release contract as
Windows. That contract was embedded in Win32-heavy `System.cpp` and
absent from the native shell. The falsifiable step was to compile and
link the original ordering logic once for both platforms, then execute
more than its 26-entry node capacity. Existing shell tests cannot cover
the registrar order because the mission and object registrars are not
linked. The result determines whether the real App owners can use their
existing static registration rather than a second native startup scheme.

**Change.** Moved the original registration, reverse `Startup`, reverse
`Initialize`, reverse release and pointer-reset code into
`GlobalStartup.cpp`. `System.cpp` no longer defines those functions;
the Windows `Trim.vcxproj` includes the new source, and the native shell
links its CMake object target. A focused CTest check calls the production
functions with 30 components, crossing the original node boundary.
The native shell does not yet call `CreateGlobalComponents` because its
required App registrars and services remain outside the link closure.
No required service was replaced with a success stub.

**Validation.** On Linux x86_64 with GCC 16.2.1 and Clang 22.1.8,
`cmake --preset linux-gcc-debug` / `linux-clang-debug`, targeted builds
of `frontierx_app_global_startup_owner`, `frontierx_native_shell` and
`frontierx_app_global_startup_probe`, and the focused
`portability.app_global_startup` CTest passed. Each execution observed
30 callbacks in reverse registration order in each startup phase,
followed by 30 reverse releases and null service slots. Both complete
build and CTest presets passed 46 executing checks with two GPU skips
in the command sandbox. `Trim.vcxproj` and its filters parsed as XML;
native Windows Debug/Final, DLL export and ABI validation were not run.
Separate GCC and Clang native shell processes using lawfully external
Steam `DATA`, `gendata.db`, `gametypes.db` and `Sol_Train.dmission`
exited 0 after opening the 856-byte Forger type, 1640-byte GenData
record, 160633-byte ParseData and 3236-byte Globals record. Both still
reported "mission not started". `IStreamer` remained required and
initialized; only optional full-screen `IVideoSystem` was explicitly
disabled by `--skip-movies`. Dummy SDL video did not execute GPU
gameplay. A separate temporary GCC shell link with the compiled
`Mission`, `ObjList`, `MGlobals` and `Objwatch` owners failed on 504
unresolved references across 89 production symbols, with no multiple
definitions. Its diagnostic and binary path stayed outside the
repository. This link attempt is evidence of the next closure, not an
executed mission. Visible Forger, normal movement input, simulation
checkpoint, save/reload and retail gameplay remain unperformed.

**Next blocker.** Resolve and initialize the required App simulation,
input and mission services exposed by the link audit, starting with
`THEMATRIX`, localization, `FILETRANSFER` and object support as their
real callers demand. Once the registrars can start, run the real
`ObjectList::loadTypesData` on the external archive and reject
incomplete archetype reads before invoking `IMission::Load`. The
Windows mission-without-units report is a separate CQ2/retail
restoration defect, not a valid Linux gameplay reference.

## Production local packet-buffer build gate (2026-09-27)

**Blocker and real caller.** `OpAgent::flushSendBuffer` sends commands via
`NetPacket::Send`; that owner uses `NETBUFFER->Send`, and its receive path
calls `NETBUFFER->Receive(0, 0, ...)`. The original `NetBuffer` has a
`PLAYERID == idTo` holding queue, so local single-player packet delivery
does not require DirectPlay. The hypothesis was that this queue could
compile with the native App globals while unavailable remote operations
remained explicit failures. A two-compiler object build was the narrowest
check; it cannot establish packet delivery without the `NetPacket` and
App service link closure.

**Change.** Added `frontierx_app_net_buffer_owner` for the production
source. The native branch uses the existing monotonic millisecond clock,
defines the offline `PLAYERID`/`HOSTID` and `NETBUFFER` global slots, and
keeps the original self-send, holding-queue, heap and unfiltered receive
code. DirectPlay send, filtered receive, player synchronization, system
message handling and player destruction abort with an explicit native
diagnostic. Windows keeps its DirectPlay branches. Runtime-only player
latency fields use fixed 32/64-bit widths on LP64, matching Win32; no
packet or persistent format was widened. The source is compile-only and
does not claim a delivered operation.

**Validation.** On Linux x86_64, `cmake --preset linux-gcc-debug` and
`linux-clang-debug`, `cmake --build --preset <preset> --target
frontierx_app_net_buffer_owner`, both full builds and both CTest presets
passed with GCC 16.2.1 and Clang 22.1.8. Each preset executed 46 checks;
two GPU checks skipped in the command sandbox. Clang reported one
existing signed one-bit `CQFLAGS.bDPDelivery` assignment warning in this
source. Separate native shell processes with the lawfully external Steam
`DATA`, root-level `gendata.db`/`gametypes.db`, source `Conquest.ini`,
`--skip-movies` and `Sol_Train.dmission` exited 0. Each opened the
856-byte Forger type, 1640-byte GlobalValues, 160633-byte mission
ParseData and 3236-byte Globals record, then reported "mission not
started". A first run pointed the explicit DB flags into `DATA` rather
than the installation root and correctly failed with exit 6; the
corrected run passed. Required `IStreamer` initialized; only optional
full-screen `IVideoSystem` was disabled by the explicit movie flag.
`NetBuffer` did not run in those shells, and the command sandbox did
not execute GPU mission rendering. Native Windows Debug/Final, module
ABI, retail gameplay, movement, simulation and save/reload checks remain
unperformed.

**Next blocker.** Compile the production `NetPacket` owner against the
native local queue, then close its required App service dependencies and
execute a real self-send/receive through the packet owner before
connecting `OpAgent` to mission startup. Keep DirectPlay multiplayer
outside this bounded single-player path.

**Focused regression-check rationale.** The existing native shell does not
link `NetBuffer`, and the native heap probe does not call its packet
queue. A bounded check using the production `NetBuffer` registrar,
native DACOM heap and real user-defaults owner should verify that two
offline packets sent to `PLAYERID == 0` are received in order with their
payloads and can be freed, then that the queue is empty. Passing it
allows the local queue to be used by a future linked `NetPacket`; failure
would make packet delivery the first integration blocker. It is not a
mission-input or movement check.

**Executed follow-up.** Added the focused check above using the
production `NetBuffer` App registrar, `CreateGlobalComponents`, native
DACOM heap and registered native `IUserDefaults`. It sends two
`PT_USRMOVE`-typed header/payload records to offline `PLAYERID == 0`,
receives them in order with the original queue, checks payload and
sender, frees both with `FreePacket`, confirms the queue is empty and
releases the registered owner. The test records are deliberately only
packet-envelope fixtures; they are not full movement commands. GCC
16.2.1 and Clang 22.1.8 configured, built and executed
`frontierx_net_buffer_probe` and `portability.net_buffer` on Linux
x86_64; both direct processes printed "production offline NetBuffer
queue delivered two packets in order" and exited 0. Both complete
Debug builds succeeded; each CTest preset executed 47 passing checks
with two GPU skips in the command sandbox. Separate lawfully external
Steam-data shell runs under each compiler exited 0 after Forger,
GenData and mission-schema loading and still reported "mission not
started". Required `IStreamer` initialized; only optional full-screen
`IVideoSystem` was disabled with `--skip-movies`. The packet probe used
no retail data; the shell used external retail data read-only. Native
Windows Debug/Final, module ABI, linked `NetPacket`/`OpAgent` command
delivery, visible GPU mission, simulation and save/reload remain
unperformed.

**Corrected caller detail.** Source review of `NetPacket::Send` shows
that `OpAgent::flushSendBuffer`'s `NETF_ALLREMOTE` broadcast does not
echo in the offline `PLAYERID == 0` branch. Normal input uses the
`NETF_ECHO` path to inject a local packet, then the original
`NetBuffer::Receive` and `NetPacket::receivePackets` dispatch it to
`CQE_NETPACKET`. The next executable gate must include that caller;
the buffer check alone cannot establish a move command.

**Native `NetPacket` source boundary.** Exact-case includes and a
self-contained `<cstring>` dependency were corrected. Direct GCC
syntax compilation of the unlinked production `NetPacket.cpp` still
fails on Windows-only DirectPlay names, polling-thread/window UI,
pointer-width event parameters and host-migration calls. It is not
a native object target and has not executed. The next step is to
retain its real offline `NETF_ECHO` send/receive and event dispatch
while isolating only the unsupported multiplayer and UI branches.

## Production operation-owner native build gate (2026-09-27)

**Blocker and real caller.** `Mission::New`, `Mission::Update`,
`Mission::Reload` and `Mission::Save` call the production `IOpAgent`
instance, `THEMATRIX`; it is needed for the chosen training mission's
orders and deterministic update, even though its implementation also
contains network logic. The previous combined link lacked this owner.
The local hypothesis was that native source compilation would first
need exact-case includes, the App event service, a monotonic millisecond
clock and separation of Windows-only UI/Zone lobby and optional client
playback. A GCC/Clang object build was the cheapest falsifier; no
existing runtime test can exercise the owner while its required App
services remain unresolved.

**Change.** `OpAgent.cpp` now compiles its original operation and packet
paths on Linux. The native global declaration has the same fixed
`MAX_SYSTEMS`, hyper-system mask and event values as Windows, and
`THEMATRIX`/optional debug `NETOUTPUT` have native pointer-sized global
slots. The native registrar uses the real `IEventSystem` connection
point and requires its callback to attach. The existing platform
monotonic millisecond clock replaces `GetTickCount` for bandwidth and
backoff bookkeeping. Native debug hotkey IDs are validated before U32
conversion; the unsupported optional client-playback mode fails
`OpAgent::New` rather than silently starting. Zone lobby scoring and
Windows menu messages remain Windows-only. Native construction of the
existing `SYNCSTATS` packet uses properly aligned storage and placement
construction without changing its fields or wire values. Windows
source branches retain the supported paths. The target is compile-only
and is not linked into the successful shell.
Native compile-time checks pin the six-byte packed host subpacket,
its two-byte `groupID` offset, four-byte command enum and 16-byte
expanded runtime subpacket header; they are not MSVC ABI evidence.

**Validation.** On Linux x86_64, `cmake --preset linux-gcc-debug` and
`cmake --preset linux-clang-debug`, targeted
`frontierx_app_op_agent_owner` builds, and both complete build/CTest
presets succeeded with GCC 16.2.1 and Clang 22.1.8. Each full CTest
preset executed 46 passing checks and skipped two GPU checks in the
command sandbox; none executes `OpAgent`. Separate GCC and Clang shell
processes used lawfully external Steam `DATA`, `gendata.db`,
`gametypes.db` and `Sol_Train.dmission`, plus the checked-in profile and
explicit `--skip-movies`. Both exited 0 after reading the 856-byte
Forger type, 1640-byte GenData `GlobalValues`, 160633-byte mission
ParseData and 3236-byte Globals record, then reported "mission not
started". `IStreamer` initialized as required; only optional
full-screen `IVideoSystem` was disabled. No GPU mission ran under dummy
SDL video. A temporary GCC link of `Mission`, `ObjList`, `MGlobals`,
`Objwatch` and `OpAgent` objects against the native shell failed on 561
unresolved references across 91 production symbols, led by `PLAYERID`,
`HOSTID`, cursor, nugget, fog, file-transfer and packet services; it
reported no multiple definitions. The temporary diagnostic stayed
outside the repository. Native Windows Debug/Final, DLL ABI, gameplay,
visible unit, normal movement input, simulation checkpoint and
save/reload checks were unperformed. Existing legacy smart-pointer
pointer-cast warnings remain, so this object build is not an LP64
runtime-safety claim.

**Next blocker.** Bind real player/host, packet and mission services in
the App composition, then link the `OpAgent` registrar with
`Mission`/`ObjList` and run the production startup sequence. The
`ObjectList::loadTypesData` path must then reject incomplete external
archetype reads instead of reporting success. The Windows
mission-without-units report remains a separate CQ2/retail-restoration
defect, not a Linux gameplay reference.

## Production offline packet dispatch (2026-09-27)

**Blocker and real caller.** Normal input reaches `NetPacket::Send` with
`NETF_ECHO`; the offline `PLAYERID == 0` branch calls the original
`send`, which stages a guaranteed packet in `NetBuffer`. A subsequent
`CQE_UPDATE` calls `NetPacket::receivePackets` and dispatches the game
packet to `CQE_NETPACKET`, where `OpAgent` can consume it. Existing
shell tests do not link either packet owner. The existing buffer check
covered only its queue, so it was extended to distinguish a real
`NetPacket` event from a local buffer copy. Passing this gate allows
packet services to enter the App composition; failure would keep
normal movement input blocked before `OpAgent`.

**Change.** `NetPacket.cpp` now compiles as
`frontierx_app_net_packet_owner`. Native source keeps its original
offline send, acknowledgment, receive and EventSystem update paths.
Required callback subscription uses the active native `IEventSystem`
and fails if it cannot attach. DirectPlay names, polling thread,
system messages, host migration and remote-session sends fail
explicitly on Linux; optional multiplayer diagnostics and turtle
overlay are inactive in the bounded offline run. The Windows branches
remain. Native event parameters are checked before narrowing to U32;
disk and packet fields were not widened. The native `IHeap` owner now
honors `DAHEAPFLAG_PRIVATE` by creating a separate reference-counted
heap for each packet owner, preserving independent allocation and
release diagnostics. The original shared heap remains process-owned.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
configured and built `frontierx_app_net_packet_owner`, the extended
`frontierx_net_buffer_probe`, `frontierx_native_heap_probe`, and the
complete Debug preset. The focused packet process sent two local
buffer envelopes, then sent one `NETF_ECHO` envelope through the
production `NetPacket` owner. Its real EventSystem update dispatched
exactly one `CQE_NETPACKET` with the expected payload. The private
heap check verified that a packet-owner allocation is absent from the
shared heap and that releasing the private heap balances its storage.
Both direct packet runs exited 0 without the earlier false leak
warnings. Each full CTest preset executed 47 passing checks and
skipped two GPU checks in the command sandbox. Separate GCC and Clang
native shell processes with lawfully external Steam `DATA`, root-level
`gendata.db`/`gametypes.db`, the checked-in profile and
`Sol_Train.dmission` exited 0 after schema loading; both still
reported "mission not started". Required `IStreamer` initialized;
only optional full-screen `IVideoSystem` was disabled by explicit
`--skip-movies`. The packet check used generated envelope data, not a
complete move command or retail mission. Native Windows Debug/Final,
module ABI, mission-owned input, visible GPU unit, simulation and
save/reload checks remain unperformed. Clang still reports the
original `NETPLAYER` memset and incomplete packet-switch warnings.

**Combined link audit.** A temporary GCC link of the current native
shell plus the compiled `Mission`, `ObjectList`, `MGlobals`, `Objwatch`,
`OpAgent`, `NetBuffer` and `NetPacket` objects failed with 409 unresolved
references across 87 production symbols and no multiple definitions.
The remaining group is led by `CURSOR`, `NUGGETMANAGER`, `FOGOFWAR`,
the App `MISSION` slot, `SCROLLTEXT`, `CAMERAMANAGER`, `FILETRANSFER`,
`MUSICMANAGER` and status/UI services. The diagnostic executable and
log stayed under `/tmp`; this was a link audit, not a mission run.

**Next blocker.** Link the packet registrars and the compiled
`OpAgent`, `Mission`, `ObjectList` and global owners into the native
composition with their real required App services. Execute the
production mission loader and confirm that a selected Forger move
event reaches `OpAgent`; the current `NETF_ECHO` envelope check does
not prove gameplay movement.

## Native offline mission registrar contract (2026-09-27)

**Blocker and caller.** The production `_mission::Initialize` registrar
unconditionally dereferenced `FILETRANSFER`, although its offline
`PLAYERID == HOSTID == 0` path cannot open a remote mission channel.
`Mission::StartNetDownload` and `Mission::updateNetFile` are the actual
remote callers. The same registrar requires a functioning `IViewConstructor`
and parsed App schema before it may create `MISSION`; a debug-only bomb
would become a no-op in a release build. The existing shell process check
cannot execute this registrar because the combined App link is incomplete.

**Change.** Native App globals now provide the original `PARSER`, `MISSION`
and `ARCHLIST` slots for their production registrars, plus the exact nine
Win32 player colors. `ARCHLIST` remains the interface obtained from the
production ObjectList, not a separate placeholder owner. Native offline
mission startup skips only the absent remote `IFileTransferCallback`;
network IDs or an actual download without `FILETRANSFER` abort with a
diagnostic. A missing App parser, failed schema parse, or absent system/game
event callback also aborts before continuing mission startup. Windows retains its original file-transfer subscription
and color definitions. No App service returns fabricated success.

**Validation.** Linux x86_64 GCC 16.2.1 and Clang 22.1.8 each built the
`frontierx_native_app_globals` and `frontierx_app_mission_owner` targets and
the complete Debug preset. Each full CTest preset ran 47 passing CPU/process
checks and skipped two GPU checks in the command sandbox. A temporary GCC
shell link with the production `Mission`, `ObjList`, `MGlobals`, `Objwatch`,
`OpAgent`, `NetBuffer` and `NetPacket` objects still failed, now at 335
unresolved references across 81 distinct production symbols, with no
multiple definitions; the diagnostic remains under `/tmp`. Separate GCC
and Clang native shell processes read the lawfully external Steam `DATA`,
`gendata.db`, `gametypes.db` and `Sol_Train.dmission`. Both initialized the
required `IStreamer`, explicitly disabled optional full-screen
`IVideoSystem` using `--skip-movies`, parsed the 160633-byte mission
`ParseData`, then reported "mission not started". These processes did not
execute `_mission::Initialize`, a mission tick, GPU gameplay, input movement
or save/reload. Native Windows Debug/Final and module ABI checks were not
run. The Windows mission-without-units report remains a separate
CQ2/retail-restoration defect rather than a Linux gameplay reference.

**Next blocker.** Close the real App service link graph starting with the
productive cursor, nugget, fog, camera and UI owners; then execute the
original two-phase registrar, `ObjectList::loadTypesData` and
`Mission::Reload` against the external training data. Do not treat the
new global slots as initialized services until their owners run.

## Native mission save archive cleanup (2026-09-27)

**Blocker and real callers.** `Mission::Save` and its two quicksave paths
clear an existing UTF archive through `RecursiveDelete` before writing the
new state. The only definition was in the Windows `System.cpp`; the
combined native App link therefore lacked this required function. The
native mounted retail UTF filesystem is read-only, so treating an empty
search result as successful cleanup could let a future save path claim
success without replacing its records. The existing `data_root_probe`
already mounts a generated UTF archive, so it can guard this boundary
without a new probe. A passing check permits the native save owner to
link and rejects retail data as a write target; it does not establish
writable save support.

**Change.** The native App globals library now implements the original
`RecursiveDelete(IFileSystem *)` entry through the production filesystem
vtable. It requires `GENERIC_WRITE`, recursively removes files and
directories, restores the parent directory, closes its search handle,
and reports any failed operation. Native `Mission::Save` and both
quicksave paths now stop if changing to the root or clearing the archive
fails. Windows retains its original cleanup and save behavior. No disk
or module-ABI field changed.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
configured and built `frontierx_data_root_probe` and compiled the real
`frontierx_app_mission_owner`. The extended `portability.data_root`
check executed under both compilers: `RecursiveDelete` returned false
for its generated read-only UTF archive, after which the one-byte
`Marker` entry still read as `M`. Both complete Debug builds and CTest
presets passed; the two GPU checks were skipped in the command sandbox
and are not runtime evidence. Both lawfully external Steam-data shell
runs loaded the required `IStreamer`, explicitly disabled only the
optional full-screen `IVideoSystem` with `--skip-movies`, parsed the
training mission input and reported "mission not started". A temporary
combined GCC App link now fails at 329 unresolved references across 80
distinct production symbols, with no multiple definitions; its log
remains under `/tmp`. The writable cleanup branch, actual save/reload,
mission simulation, normal input, visible unit and native Windows
Debug/Final and ABI checks remain unexecuted.

**Next blocker.** Native saves require a writable UTF `IFileSystem` and
`IUTFWriter` owner; current `NativeUtfFileSystem` rejects writes and
deletions. Mission execution first requires closing the productive App
service graph: cursor, nugget, fog, camera and UI owners still lead the
combined link failures. Direct source compilation additionally reaches
MSVC-only object-template syntax in `Nugget.cpp`; the script owner
`MScript.cpp` still loads a Windows `Sol_T.dll` through `LoadLibrary`.
These are separate Linux porting gates before the selected script
checkpoint can execute.

## Production base-object owner in the native link graph (2026-09-27)

**Blocker and real caller.** The production `ObjectList` stores and calls
`IBaseObject` for mission instances. The combined App link still lacked
its virtual method definitions and vtable because `BaseObject.cpp` was
outside the native graph. Compiling that real owner is the cheapest
check that can disprove this part of the link blocker; the existing
synthetic mesh and shell checks do not instantiate mission objects.
Its selection rectangle also uses the original actual-to-ideal screen
coordinate contract, which needs dimensions from the active native
window rather than an assumed resolution. No persistent or cross-module
type layout was changed.

**Change.** `BaseObject.cpp` now uses the native App declarations,
case-correct `View2d.h`, production `PrimitiveBuilder2` and render
interfaces, and local Win32-compatible color-channel extraction on
Linux. Windows retains its PCH/global and `__max`/`__min` path. The
existing `frontierx_app_object_list_owner` target now builds both
`ObjList.cpp` and `BaseObject.cpp`. The native App declaration set
contains the original 80-segment circle and coordinate formulas, with
zero-initialized dimension slots. After configuring its SDL-backed
window, the shell reads `IWindowManager::GetClientArea`, rejects an
invalid size, and binds the measured physical dimensions and 800-by-600
ideal dimensions for the selected slice.

**Validation.** Linux x86_64 GCC 16.2.1 and Clang 22.1.8 each
configured and built `frontierx_app_object_list_owner`, the native
shell and complete Debug preset. The existing native-shell process
check passed under both compilers after the window-size binding.
Both full CTest presets passed their executing checks; two GPU checks
were skipped in the command sandbox and provide no runtime evidence.
A temporary combined GCC shell link with `Mission`, `ObjList`,
`BaseObject`, `MGlobals`, `Objwatch`, `OpAgent`, `NetBuffer` and
`NetPacket` failed at 337 unresolved references across 55 distinct
production symbols, with no multiple definitions; the diagnostic
stayed under `/tmp`. Separate GCC and Clang shells read lawfully
external Steam archives and `Sol_Train.dmission`, initialized required
`IStreamer`, disabled optional full-screen `IVideoSystem` only with
explicit `--skip-movies`, parsed the mission input, and still reported
"mission not started". `BaseObject` compiled but was not executed in
those shells. Native Windows Debug/Final, module ABI, visible mission
unit, input movement, simulation and save/reload checks were not run.

**Next blocker.** The real App service closure still begins with
`CURSOR`, `PB`, `NUGGETMANAGER`, `FOGOFWAR`, `SCROLLTEXT`, camera, music
and status owners. These require their productive definitions and
startup order before the original registrar can load a Forger;
placing null slots or synthetic services in the shell would not
establish gameplay.

## Production grid-distance owner for mission objects (2026-09-27)

**Blocker and real caller.** `ObjectList` calls
`GRIDVECTOR::operator-(GRIDVECTOR const&)` when examining nearby
mission objects. Its original table owner, `GridVector.cpp`, was not
in the native link graph. The existing math check did not execute
this lookup, so extending it answers whether the production owner
builds and gives matching deterministic values under both Linux
compilers. Passing this gate permits the owner to enter the combined
App link; failure would block object proximity before normal movement.

**Change.** `GridVector.cpp` now has a native include path and uses
its own table member during static initialization. The original
lookup arithmetic, 4096-element table, quarter-grid fields and
Win32 source path remain. A focused object target compiles with
floating-point contraction disabled and is linked into the existing
`math_core_probe`. The check exercises the original 3-4 case,
the 64-quarter-grid boundary and all 4096 table positions. Source
review confirmed the original square-root expression divides the
squared distance by 16 *inside* the root: the 3-4 case returns
`1.25`, not `0.3125`.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
configured and built `frontierx_app_grid_vector_owner`, directly
executed `frontierx_math_core_probe`, and passed its CTest entry.
Both produced the same 64-bit distance-table fingerprint,
`10760374359831800577`, now an asserted regression value. Both
complete Debug builds and CTest presets passed their executing
checks; two GPU checks per preset skipped in the command sandbox.
A temporary GCC shell link including the new owner still failed at
336 unresolved references across 54 distinct production symbols,
with no multiple definitions; the diagnostic stayed under `/tmp`.
Separate GCC and Clang shells read the lawfully external Steam
training-mission data, initialized required `IStreamer`, disabled
only optional full-screen `IVideoSystem` via explicit
`--skip-movies`, and reported "mission not started". The grid
check is synthetic CPU execution of a real production owner, not
mission simulation. Windows Debug/Final, module ABI, cross-build
numeric parity, gameplay input, visible unit and save/reload remain
unperformed.

**Next blocker.** The productive App link now starts with `CURSOR`,
the original `PrimitiveBuilder2 PB` defined by `System.cpp`,
`NUGGETMANAGER`, `FOGOFWAR` and UI/camera owners. The App-owned
`PrimitiveBuilder2` methods must be exposed from their real source
without importing the rest of the Win32 `System.cpp` process code.

## Native App primitive-builder owner (2026-09-27)

**Blocker and real caller.** `BaseObject` and other App game draw callers
use the global `PrimitiveBuilder2 PB`, whose Windows definitions are
embedded in the Win32 process owner `System.cpp`. The combined mission
link lacked that global and its methods. The existing render-submission
check exercised only the library `PrimitiveBuilder`, so extending it with
the App builder tests whether this real caller can submit at the established
`IRenderPrimitive` boundary. Passing permits the App owner into the
mission link; failure would leave selection and object geometry blocked.
`PrimitiveBuilder2` is runtime-only; no persistent or module ABI layout
was changed.

**Change.** A native-only object target now defines the original global
`PB` and its destructor, capacity check and `Begin` overloads. Its vertex
buffer uses pointer-sized 32-byte alignment instead of the Windows
`U32` address cast, grows in original block order, and reports allocation
failure. Native `End` and `Draw` stop on a missing receiver or rejected
submission; the Windows methods remain unchanged. The shared inline
constructor initializes native receiver and cursor state so local App
builders have defined ownership. No renderer is silently supplied to
the mission, and the new owner is not yet linked into the successful
shell.

**Validation.** Linux x86_64 GCC 16.2.1 and Clang 22.1.8 configured,
built and executed `frontierx_render_submission_probe` with the App
owner. Its repository-generated CPU quad reached the existing legacy
render adapter with six vertices, checked color `0x44332211`, 32-byte
alignment and balanced receiver references. Both complete Debug builds
and CTest presets passed 47 executing checks; two GPU tests per preset
skipped in the command sandbox and are not runtime evidence. A temporary
combined GCC shell link including `Mission`, `ObjectList`, `BaseObject`,
`MGlobals`, `Objwatch`, `OpAgent`, `NetBuffer`, `NetPacket`, `GridVector`
and this App builder failed at 295 unresolved references across 51
distinct production symbols, without multiple definitions; the
diagnostic stayed under `/tmp`. Separate GCC and Clang shell processes
read lawfully external Steam archives and `Sol_Train.dmission`, started
the required `IStreamer`, disabled optional full-screen
`IVideoSystem` only via explicit `--skip-movies`, parsed the mission
schema, and still reported "mission not started". Native Windows
Debug/Final, module ABI, visible mission unit, input movement,
simulation and save/reload were not executed.

**Next blocker.** The combined link now first lacks the original
`CURSOR` service (36 references) alongside `NUGGETMANAGER`, `FOGOFWAR`,
`SCROLLTEXT` and camera/music/status services. `Cursor.cpp` creates the
real `ICursorResource` during global registration; its dependencies and
startup order must be resolved before mission object loading can run.

## Native App cursor service (2026-09-27)

**Blocker and real caller.** `Mission::Load`, `ObjectList` and
`TResClient` use the global `ICursorResource` for busy transitions,
selection cursor ownership and notification. The original `Cursor.cpp`
is coupled to Win32 cursor resources and DirectInput, while the native
SDL `IWindowManager` already translates normal mouse events to the
production EventSystem. The named blocker was 36 unresolved `CURSOR`
references in the combined App link. The existing startup probe could
check registrar order but cannot run system-cursor creation with SDL's
dummy video driver; a separate host-dependent cursor check tests this
specific integration contract. No persistent or cross-module type
layout changed; the Linux declaration drops an ignored top-level
`const` on a bool return while Windows retains its source declaration.

**Change.** The native platform owner implements the original
`ICursorResource` and `IResource` interfaces through the App's
`Resource` arbitration and global registrar. It creates real SDL
default, move, crosshair, prohibited and wait cursors, nests busy
requests, toggles visibility, and delivers pending resource-owner
notifications on `CQE_UPDATE`. Unknown retail cursor IDs return
`FALSE` without changing the current cursor. The implementation
subscribes to the active EventSystem callback when one is bound at
registrar initialization; standalone verification invokes that same
callback directly. Native `DrawMouse` relies on SDL hardware cursor
drawing, and the Windows-only cursor handle returns null. Cursor art
still differs from retail and additional cursor IDs are unsupported;
the selected training interaction needs default and move. The shell
links the registrar but does not yet call `CreateGlobalComponents`,
so this is not mission startup.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
configured and built `frontierx_app_cursor_probe`, the native shell
and the complete Debug preset. The cursor probe ran separately on the
visible Wayland desktop under both compilers and checked default and
move shapes, two-level busy nesting, rejection of an unknown ID,
outgoing `IResourceClient` ownership notification on `CQE_UPDATE`,
visibility and cleanup. The command-sandbox CTest entry skipped
because no usable system-cursor video driver was available; that skip
is not runtime evidence. Both full CTest presets passed 47 executing
checks and skipped three host-dependent checks (cursor and two GPU).
Both lawfully external Steam-data shells initialized required
`IStreamer`, explicitly disabled only optional full-screen
`IVideoSystem` via `--skip-movies`, parsed `Sol_Train.dmission` and
reported "mission not started". A temporary GCC combined shell link
with the cursor registrar and prior App mission/object owners failed
at 259 unresolved references across 50 distinct symbols, without
multiple definitions; its diagnostic stayed under `/tmp`. Native
Windows Debug/Final and module ABI checks, integrated cursor input
against a loaded mission, visible Forger, movement, simulation and
save/reload were not executed.

**Next blocker.** The combined App link now first lacks the production
`NUGGETMANAGER` (28 references), then `FOGOFWAR`, `SCROLLTEXT`, camera,
music, status and localization owners. Their required startup order
and actual mission use must be resolved before `CreateGlobalComponents`
can run in the shell.

## Production nugget manager native build gate (2026-09-27)

**Blocker and real callers.** `ObjectList` updates, renders and tests
nuggets; `Mission` calls their original `Save` and `Load` methods.
The combined App link had 28 unresolved references to the required
`NUGGETMANAGER`. The cheapest falsifiable check was to compile the
actual `Nugget.cpp` manager and object factory, then add that object
to a temporary combined link. No existing executable check can run
this owner while its App dependencies remain unresolved; no new probe
was introduced. This changes the integration decision from supplying
a missing manager to closing the concrete dependencies of its original
implementation. Nugget save/data fields and public interfaces were
not changed.

**Change.** `Nugget.cpp` now uses native App globals and exact-case
Linux includes while preserving the Windows PCH and include path.
Unused Windows object-template headers are omitted from this source
on Linux; the actual `AnimNugget`, `NuggetManager` and `NuggetFactory`
implementations remain compiled. The native App declaration set now
exposes the original event IDs, coordinate and draw helpers, animation
slot and watcher functions consumed by this owner. The native
`TObject.h` interface-offset calculation uses pointer-sized address
arithmetic before narrowing the small offset back to its original
`U32` map field; Windows retains its original expression. The native
registrar clears `NUGGETMANAGER` through global cleanup. Editor-only
object insertion aborts explicitly on Linux; editor teardown is
omitted because native insertion is unavailable. The Linux screen
rectangle converts bounded window dimensions explicitly to `LONG`.
No simulation order, RNG expression, disk layout or Windows source
branch was changed.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each
configured and built `frontierx_app_nugget_owner` and the complete
Debug preset. Symbol inspection of the GCC object found the
production `NuggetManager::CreateNugget`, `Update`, `Render`, `Save`,
`Load` and `NuggetFactory::CreateInstance` definitions. Both full
CTest presets passed 47 executing checks and skipped three
command-sandbox cursor/GPU checks; these checks did not execute
`Nugget.cpp`. A temporary GCC shell link with the prior mission,
object, packet, cursor, grid and App builder owners plus `Nugget.cpp`
failed at 247 unresolved references across 53 distinct production
symbols, without multiple definitions; the diagnostic remained under
`/tmp`. Both successful shell processes read the lawfully external
Steam `Sol_Train.dmission` and reported "mission not started". Their
required `IStreamer` initialized; only optional full-screen
`IVideoSystem` was explicitly disabled by `--skip-movies`. Native
Windows Debug/Final, module ABI, nugget runtime, visible Forger,
movement, simulation and save/reload were not executed.

**Next blocker.** `FOGOFWAR` has 22 references in the combined link,
including `Mission::New/Save/Load` and `ObjectList::Update/Render`.
The production `FogOfWar.cpp` owner must be compiled and its actual
dependencies measured before the App registrars can run.

## Production fog-of-war native build gate (2026-09-27)

**Blocker and real callers.** The combined mission link lacked
`FOGOFWAR`, which `Mission::New/Save/Load` and `ObjectList::Update`
and `Render` call. The original `FogOfWar.cpp` is the required owner;
the chosen training mission cannot replace its visibility and save
state with a placeholder. Compiling this translation unit and adding
it to the temporary combined link was the cheapest falsifiable check.
Existing tests cannot execute it before the remaining App service
closure is linked, so no new probe was added. No persistent format or
module interface was changed.

**Change.** `FogOfWar.cpp` now uses exact-case native includes and
the existing App builder/renderer declarations while preserving the
Windows include path. Its pointer-sized runtime dependencies and
platform event IDs are declared in the native App header. Two
MSVC-accepted extra-qualified destructors have native standard C++
spellings; native min/max calls retain the original operands and
evaluation order. The Windows GDI menu/viewer is a nonfatal debug
tool: native `CreateViewer` still initializes the original fog
settings but returns `FALSE` explicitly because that viewer does not
exist on Linux. Native close events release fog textures just as
`WM_CLOSE` does on Windows. The production fog simulation, texture
generation, `New`, `Save` and `Load` bodies remain compiled.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each
configured and built `frontierx_app_fog_owner` and the complete Debug
preset. Both full CTest presets passed 47 executing checks and skipped
three command-sandbox cursor/GPU entries; none executed the fog owner.
GCC symbol inspection
found production `FogOfWar::New`, `Save`, `Load`, `Update`, `Render`
and `Notify`. A temporary combined GCC shell link with prior mission,
object, nugget, cursor and packet owners plus fog failed at 235
unresolved references across 54 distinct production symbols, with no
multiple definitions; the diagnostic remained under `/tmp`.
The fog owner was not executed by the successful shell. Both
lawfully external Steam-data shells still parsed `Sol_Train.dmission`
without starting it; required `IStreamer` initialized and optional
full-screen `IVideoSystem` was disabled only by explicit
`--skip-movies`. Native Windows Debug/Final and module ABI checks,
fog runtime on retail input, visible Forger, movement, simulation
and save/reload remain unperformed.
The pre-existing, currently uncalled `BlackFog::setFog` missing-return
warning is a CQ2 source defect, not evidence of a Linux runtime failure;
it was left unchanged.

**Next blocker.** The combined App link now first lacks `SCROLLTEXT`
(20 references), `CAMERAMANAGER` (18), `SYSMAP` (18) and other
production UI/mission services. The next owner should be chosen by
its position in `CreateGlobalComponents` and first mission load,
not by supplying success-returning substitutes.

## Production spatial-object map build gate (2026-09-27)

**Blocker and real callers.** The combined mission link lacked
`OBJMAP`. `Sector::Load` calls its `Init`, while `ObjectList` and
mission objects use the map to register and locate objects. The
falsifiable check was to compile the original `ObjMap.cpp` and add it
to the temporary combined link. No existing test executes this owner
before the remaining App closure is linked, so no new probe was added.
The `IObjMap` interface and saved formats were not changed.

**Change.** The original owner uses native exact-case includes and
the existing App globals on Linux. Its two diagnostic pointer checks
use pointer-sized casts; the low-word sentinel mask remains the same.
The Windows source path and spatial-index algorithm are retained.
The native globals translation unit now defines the actual `OBJMAP`
slot for the original registrar. A direct `ScrollingText.cpp` build
attempt showed that the next UI owner depends on `Frame`, GDI fonts,
`IVideoSurface` and legacy UI containers; the unbuildable attempt
was removed without introducing a substitute service.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
configured and built `frontierx_app_object_map_owner` and the complete
Debug preset. Each full CTest preset executed 47 passing checks and
skipped three command-sandbox cursor/GPU checks. None of these checks
executed `ObjMap.cpp`. A temporary GCC combined App link including
the map, mission, object list, operation, packet, nugget and fog owners
failed at 223 unresolved references across 53 distinct symbols with
no multiple definitions. `OBJMAP` was absent from that unresolved
list; `SCROLLTEXT` (20), `CAMERAMANAGER` (18) and `SYSMAP` (18) lead
it. Both successful native shells mounted the user's lawfully
external Steam data, initialized required `IStreamer`, explicitly
disabled optional full-screen `IVideoSystem` with `--skip-movies`,
parsed `Sol_Train.dmission` and reported "mission not started".
Neither shell executed the compiled map owner. Native Windows
Debug/Final, module ABI, retail gameplay, visible unit, normal-input
movement, simulation checkpoint and save/reload were not performed.

**Next blocker.** The active mission link needs the actual App UI and
mission service owners, beginning with `SCROLLTEXT` and
`CAMERAMANAGER`, before `CreateGlobalComponents` and mission load can
run. `SysMap.cpp` is a 3,512-line UI/rendering owner; its save/load
contract also remains necessary. No successful return may conceal
these missing services.

## Production event scheduler build gate (2026-09-27)

**Blocker and real callers.** The combined mission link lacked
`SCHEDULER`. `ObjectList::Update` calls its `UpdateQueue` on the
simulation cadence, and mission close calls `CleanQueue`; delayed
damage is delivered through `OBJLIST->FindObject`. The falsifiable
check was to compile the original `EventScheduler.cpp` and include
it in the temporary mission link. Existing tests do not execute a
combined App mission/object closure, so no disconnected probe was
added. No persistent format or module ABI type was changed.

**Change.** The owner uses exact-case native includes and the existing
App global declarations under Linux, with a real native `SCHEDULER`
slot for its original registrar. Its native tick minimum compares the
same `SINGLE` values in the same order as the Windows macro; the
Windows source branch is retained. The queue ordering, update
counter and event delivery algorithm were not changed.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each
configured and built `frontierx_app_event_scheduler_owner` and the
complete Debug preset. Each full CTest preset executed 47 passing
checks and skipped three command-sandbox cursor/GPU checks. These
checks did not execute the scheduler. A temporary combined GCC App
link including the scheduler, map, mission, object list, operation,
packet, nugget and fog owners failed at 213 unresolved references
across 52 distinct symbols with no multiple definitions. The prior
`SCHEDULER` references were resolved. `SCROLLTEXT` (20), `SYSMAP`
(18), `MUSICMANAGER` (16), `STATUS` (16) and `CAMERAMANAGER` (15)
now lead the unresolved list. Both successful native shells mounted
the user's lawfully external Steam data, initialized required
`IStreamer`, explicitly disabled optional full-screen
`IVideoSystem` with `--skip-movies`, parsed `Sol_Train.dmission`
and reported "mission not started". The scheduler did not execute
in either shell. The included `Objwatch.h` still emits existing
32-to-64-bit pointer-cast warnings; their runtime path has not been
validated by this compile. Native Windows Debug/Final, module ABI,
visible unit, normal-input movement, simulation checkpoint and
save/reload were not performed.

**Next blocker.** The active App link must close the production
mission UI/camera/map service chain. `SCROLLTEXT` provides save/load
notifications but its existing implementation is coupled to the
Windows `Frame`/GDI/`IVideoSurface` stack. `SYSMAP` and
`CAMERAMANAGER` save/load state is required by `Mission::Reload`.
No substitute may claim successful mission execution while those
required calls are absent.

## Production selection-ring table gate (2026-09-27)

**Blocker and real caller.** The combined mission link lacked
`circleLines`, which `BaseObject` reads while drawing selection and
range rings. The original `StacticInit.cpp` is its initializer. The
falsifiable check was to compile that file and execute its static
initialization through the existing math-core check, then add it to
the temporary App link. The existing check already exercises the
related production `GridVector` geometry, so no new probe was added.
No persistent format or module interface was changed.

**Change.** The original initializer now uses native App declarations
and the same `3dmath.h` `PI` constant under Linux, while retaining
the Windows PCH branch. The existing math-core check validates all
80 unit-radius, zero-height points and the expected quadrant values.
The object target disables FMA contraction, as the nearby production
grid-distance owner does. The source algorithm and initialization
order remain unchanged.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each
configured and built `frontierx_app_circle_lines_owner`, executed
`portability.math_core`, and built and ran the complete Debug preset.
Each full CTest preset executed 47 passing checks and skipped three
command-sandbox cursor/GPU checks. The focused math check executed
the production static initializer and verified its geometry; it did
not render a mission unit. A temporary combined GCC App link with
the prior mission, object, packet, operation, map, scheduler, nugget
and fog owners failed at 207 unresolved references across 51
distinct symbols with no multiple definitions. Its prior
`circleLines` references were resolved. Both successful native shells
mounted the user's lawfully external Steam data, initialized required
`IStreamer`, explicitly disabled optional full-screen `IVideoSystem`
with `--skip-movies`, parsed `Sol_Train.dmission` and reported
"mission not started". The successful shell did not include
`BaseObject` or draw a ring. Native Windows Debug/Final, module ABI,
visible Forger, normal-input movement, simulation checkpoint and
save/reload were not performed.

**Camera blocker.** Direct GCC compilation of the original
`MovieCamera.cpp` was attempted because its manager serves
`ObjectList::Update` and `Mission::Save/Reload`. After resolving
include-case differences, compilation first exposed unported
`TObjTrans`/`TObjRender` template syntax and `BaseHotRect` Win32 UI
contracts; the file also contains x86 inline assembly. The
unbuildable target and source edits were removed. This is a Linux
port closure, not evidence that the documented Windows mission with
no units is an acceptable gameplay reference.

**Next blocker.** The combined App link still needs the production
message, map and camera service chain. The camera object's template
and UI dependencies must be ported or cleanly separated at the
existing owner boundary while preserving the manager's save/load and
physics behavior. No fake-success implementation can satisfy the
selected mission.

## Camera object template closure (2026-09-27)

**Blocker and real caller.** `MovieCamera.cpp` defines the original
`CAMERAMANAGER` used by `ObjectList::Update` and
`Mission::Save/Reload`. Its camera objects compose `ObjectFrame`,
`ObjectTransform` and `ObjectRender`. A direct native compile stopped
in that template chain before reaching the manager. The falsifiable
local check was to compile those exact headers together through the
existing `frontierx_app_object_list_owner`, then repeat direct camera
syntax diagnosis. This compile-only source is justified by that named
integration blocker; existing math/render probes do not include
these App templates. No persistent structure or public module
interface was changed.

**Change.** Linux branches of `TObjTrans` and `TObjRender` use
standard dependent-type declarations and inherited-member access.
`TObjTrans` includes the original `ICamera.h` enum before `ENGINE.H`.
The two overlapping one-byte sync-buffer shifts in `TObjFrame` use
`std::memmove` natively, matching the source comment's intended
upward shift; the Windows `rmemcpy` calls remain. Their cross-platform
byte-for-byte parity has not been measured. A native `MESHMAN` slot
is declared and defined for the transform owner but is not yet bound
to its real aggregate. The compile-only translation unit is part of
the existing object-list target, not a new synthetic implementation.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each
configured and built `frontierx_app_object_list_owner` with the
combined header translation unit, then built the complete Debug
preset. Each full CTest preset executed 47 passing checks and
skipped three command-sandbox cursor/GPU checks. The template
methods and camera owner did not execute in those checks. A temporary
GCC syntax copy of `MovieCamera.cpp` outside the repository, with
only PCH and include-case substitutions, advanced past the previous
`TObjTrans`/`TObjRender` failures and still failed at the Win32
`BaseHotRect` event contract, editor dialogs, and other owner-specific
calls. That copy was diagnostic only and was not linked. Both
successful native shells mounted the user's lawfully external Steam
data, initialized required `IStreamer`, explicitly disabled optional
full-screen `IVideoSystem` with `--skip-movies`, parsed
`Sol_Train.dmission` and reported "mission not started". The camera
manager, visible Forger, input movement, simulation checkpoint and
save/reload did not run. Native Windows Debug/Final and module ABI
checks were not performed.

**Next blocker.** The production camera owner must separate or port
its Win32 editor/hot-rectangle UI at the existing object boundary,
then retain real camera physics and save/load in the native mission
closure. The native `MESHMAN` slot also needs the actual mesh-manager
aggregate before camera objects can own meshes. The combined App
link still lacks the camera, map, text and other mission services;
no successful mission startup follows from this header compile.

## Production movie-camera manager gate, 2026-09-27

**Blocker and scope.** `ObjectList::Update` calls
`CAMERAMANAGER->PhysUpdate`, and `Mission::Save`/`Mission::Reload` use
the same manager. The original `MovieCamera.cpp` had no Linux object
because its movie-camera object, editor hot rectangle and Win32 dialogs
shared a translation unit with the manager. Source review of the
selected `Sol_T.cpp` mission script found no movie-camera commands;
the lawfully external `Sol_Train.dmission` QuickSave tree has four
object records (Forger, two Taos and Acropolis), none a movie-camera
object. This bounds the native cinematic-object exclusion to this
acceptance mission. `ChangeCamera(IBaseObject*)` now aborts with a
diagnostic if that unsupported branch is called; it cannot report a
successful cinematic camera transition. The original manager's queue,
physics, event, and save/load code remains compiled. Windows keeps the
full cinematic/editor source branch.

The native manager subscribes to the real `EVENTSYS` outgoing callback
used by the App startup check and unsubscribes during teardown. Its
`MovieManager` save-position record uses a fixed 44-byte Win32-shaped
disk type instead of serializing the 64-bit runtime `CamSpot` pointer.
Native reads and writes require exact field lengths; load rejects
truncation, invalid `lastOn`, and excess bytes, and clears a partial
queue on failure. The Windows serialization branch remains unchanged.
The existing offline App probe had no writable `IFileSystem` and
could not execute these methods. A small in-memory DOS child now
extends that probe specifically for the native 44-byte format and
partial-load regression. It does not implement a product filesystem.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 built
`frontierx_app_movie_camera_manager_owner`,
`frontierx_app_objwatch_owner`, `frontierx_net_buffer_probe`, and
`frontierx_native_shell` in the complete `linux-gcc-debug` and
`linux-clang-debug` presets. `portability.net_buffer` executed the
production global registrar, manager event subscription, mission-close
callback, and cleanup under both compilers using synthetic offline
App state. It loaded and saved a 111-byte in-memory `MovieManager`
record with one queued spot and identical bytes, rejected a truncated
spot, a trailing byte and an invalid last-spot flag, and saved a
74-byte empty queue after the truncated load. Each complete CTest
preset executed 47 passing checks and
skipped the three command-sandbox cursor/GPU entries. These skips are
not runtime evidence. Separate native shell processes under both
compilers used the external Steam installation: ten data archives
mounted, required `IStreamer` initialized, `IVideoSystem` was
explicitly disabled with `--skip-movies`, the Forger GameTypes record
was 856 bytes, and the mission source parsed 160633 schema bytes and
3236 Globals bytes before reporting "mission not started". The movie
manager is linked into that shell but its registrar is not called
there. A temporary combined GCC App link including the other compiled
mission owners still failed on 50 distinct production symbols, led by
`SCROLLTEXT`, `SYSMAP`, `MUSICMANAGER`, `STATUS`, and `HINTBOX`; it was
not executed. No GPU mission run, native Windows Debug/Final or module
ABI check was performed.

Commands executed for this gate (with `FRONTIERX_RETAIL_ROOT` set to
the lawfully installed game directory outside the repository):

```sh
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
out/build/linux-gcc-debug/frontierx_data_root_probe --mission-filesystem "$FRONTIERX_RETAIL_ROOT/DATA" Sol_Train.dmission
```

The separate shell runs used each preset's `frontierx_native_shell`
with `--data-root "$FRONTIERX_RETAIL_ROOT/DATA"`,
`--gen-data "$FRONTIERX_RETAIL_ROOT/gendata.db"`,
`--game-types "$FRONTIERX_RETAIL_ROOT/gametypes.db"`,
`--system-profile Code/App/Src/Conquest.ini`, `--skip-movies`,
`--mission-source Sol_Train.dmission`, `--smoke-ms 1`, and `--hidden`
with `SDL_VIDEODRIVER=dummy`. The read-only data-root check opened four
QuickSave object records; the shell did not instantiate those objects.

**Next blocker.** Close the missing App service and UI owners in the
combined production mission link, then call the actual global and
mission registrars from the shell. The native UTF filesystem still
lacks the writable container contract for a full `Mission::Save` and
reload. The camera-manager format check is now executed; the full
mission save cycle remains necessary. The external data are present;
the remaining blockers are
Linux integration and CQ2/retail restoration, not missing media.

## Production spatial iterator build gate, 2026-09-27

**Blocker and hypothesis.** The temporary combined mission link
reported missing `ObjMapIterator` constructor, `Next`, and boolean
conversion symbols. `ObjectList` uses this iterator for nearby object
queries; `MScript` uses it for scripted spatial searches. The real
owner is `ObjMapIterator.cpp`, adjacent to the already compiled
`ObjMap.cpp`. If its native include and debug-global closure is
complete, that source should build in the existing map owner target
without changing map iteration order, mission IDs or serialized data.

The original iterator is now part of `frontierx_app_object_map_owner`.
Only its precompiled-header/global include and case-sensitive
`Objwatch.h` spelling are conditional on Linux. A native
`DEBUG_ITERATOR` pointer occupies the original debug slot; it is a
runtime diagnostic pointer, not a persisted map field or ABI change.
The Windows source branch remains unchanged. No new probe was added:
the map and game callers still lack a linked Linux mission closure,
so compilation is the falsifiable check at this gate.

**Validation.** `cmake --build --preset linux-gcc-debug --target
frontierx_app_object_map_owner` and the equivalent
`linux-clang-debug` target built under GCC 16.2.1 and Clang 22.1.8
on Linux x86_64. The GCC object defines the production constructor,
`UpdateArea`, `SetFirst`, `Next`, `GetApparentPlayerID`, and debug
boolean conversion. Both complete Debug presets built and their CTest
presets executed 47 passing checks each, with three host-dependent
cursor/GPU skips. Separate native shell processes under both compilers
mounted the lawfully external Steam data, initialized required
`IStreamer`, explicitly disabled only full-screen `IVideoSystem` with
`--skip-movies`, parsed `Sol_Train.dmission`, and reported "mission not
started". Those shells did not link or execute `ObjMapIterator`,
`ObjectList` or `MScript`; the CTests did not exercise iterator
behavior. Native Windows Debug/Final, module ABI, retail gameplay,
GPU mission rendering and mission save/reload remain unperformed.

**Next blocker.** Build and link the production `MScript` and selected
campaign-program path, including their module registration and fixed
save ABI, along with the remaining App service owners needed before
`Mission::Reload` can run. The Windows `Sol_T.dll` in the external
installation cannot be loaded as a native Linux campaign module.
The existing combined-link report also lists text, system-map, music
and HUD globals; adding null slots would not provide those contracts.

## Training-script MPartRef save-layout gate, 2026-09-27

**Blocker and real caller.** `Mission::Reload` calls `MScript::Load`
after loading objects and services. The selected `Sol_T` source stores
`MPartRef` values for Forger, Taos and Acropolis in `MissionData`,
which the original `MScript::saveGlobalData` writes through its script
save contract. The retail `MPartRef` is 16 bytes: cache index and
mission ID occupy eight bytes, and two debugger-only pointer slots
occupy eight. Directly compiling those pointer fields on LP64 would
make each value 24 bytes and change every later script-data offset.
The production reference methods resolve the object through the
mission-ID cache; source review found no runtime reads of the two
debugger slots. This is a Linux width defect, separate from the
still-unmeasured CQ2/retail script-data compatibility.

On Linux the two debugger-only slots now hold four-byte placeholders
that discard pointer assignments and initialize to zero. The Windows
fields remain native Win32 pointers. No pointer is narrowed or saved
through a `U32` cast. The App reference layout stays 16 bytes with
four-byte alignment, preserving the selected script's field strides.
This does not establish the full `MissionData` schema or campaign
module ABI; those still require their own native compile and retail
correlation checks. The existing `portability.layout` check was
extended because the failure would change whether the selected
mission's script save data can be read and written at its original
offsets. A separate probe was unnecessary.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
built `frontierx_layout_probe`, executed `ctest --preset
linux-gcc-debug -R '^portability.layout$' --output-on-failure` or the
equivalent Clang command, and observed a passing check. The check
asserts 16-byte size and four-byte alignment, then verifies that all
16 bytes of a newly constructed `MPartRef` are zero. Both complete
Debug presets built and their full CTest presets executed 47 passing
checks each with three command-sandbox cursor/GPU skips. A temporary
direct GCC syntax diagnosis of `MScript.cpp` outside the repository
still stopped in the Win32 `Frame.h`/`Resource.h` include chain after
precompiled-header and include-case substitutions. No `MScript` or
`Sol_T` owner was built or executed, no external mission was started
in this gate, and native Windows Debug/Final, module ABI, retail
script-state and mission save/reload checks were not performed.

**Next blocker.** Port the real `MScript` compilation and module
registration path, keeping the selected `Sol_T` program and its
global/program save layouts compatible with the external mission.
The source still calls Win32 `LoadLibrary` for the campaign DLL and
contains menu/`Frame` dependencies; the external Windows DLL cannot
serve as a native Linux module. Do not bypass that failure with a
successful script-load result.

## Selected Sol_T script data and fresh-start gate, 2026-09-27

**Blocker and caller.** `Mission::Reload` loads objects before invoking
`MScript::Load`; the training mission's source `Sol_T` registers
`MissionData` and a `SolTrain_SelectAndMoveForger_Save` program state.
The previous `MPartRef` width fix was necessary but did not establish
the enclosing saved layout. The original `DataDef.h` uses Windows
include spelling and was not in a native build. Its Linux-only
include names now match the repository, leaving the Windows include
branch unchanged. The existing `portability.layout` check includes
that original header and checks source-derived offsets: 292-byte
`MissionData`, Forger at 168, Taos1 at 184, Taos2 at 200,
Acropolis at 216, final teletype-handle field at 288, and a four-byte
saved `SelectAndMoveForger` enum. These are C++ source-layout checks,
not a measured retail script-schema comparison.

The existing read-only data-root check now asks the actual external
mission filesystem whether `\Script\GlobalData` exists. It does not
copy or emit its contents. Both compiler builds observed that this
entry is absent while the four previously identified QuickSave object
records are present. Thus this first mission load cannot restore an
already-saved `Sol_T` global state; a native campaign module must
register the script and initialize its state. This read changes the
integration decision for the selected first-start path. It does not
remove the later save/reload requirement.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each
built `frontierx_layout_probe` and executed `portability.layout`
successfully. Each built `frontierx_data_root_probe` and ran
`out/build/<preset>/frontierx_data_root_probe --mission-filesystem
"$FRONTIERX_RETAIL_ROOT/DATA" Sol_Train.dmission` against the user's
lawfully installed data: four QuickSave object records opened,
160633 `ParseData` bytes and 3236 `Globals` bytes were reported, and
no `Script/GlobalData` child was found. Both complete Debug presets
built and their CTest presets executed 47 passing checks with three
command-sandbox cursor/GPU skips each. The source layout check uses
the game's header; the external-data check only reads UTF metadata.
Neither executes `MScript`, `Sol_T`, a mission tick, a visible GPU
unit, or a mission save. Native Windows Debug/Final, module ABI,
retail script-schema correlation and full save/reload are unperformed.

**Next blocker.** Port the production `MScript` compilation, replace
its Win32 campaign-DLL load with a native module built from `Sol_T`
source, and register that program against a compatible parser schema.
Direct GCC diagnosis after temporarily removing Windows editor/menu
code still finds missing game-service globals, native include closure
and `LoadLibrary`; it is not a runnable owner. The real first-start
path must fail explicitly until the native module initializes.

## Selected Sol_T program and schema build gate (2026-09-27)

**Blocker and caller.** `Mission::Reload` calls `MScript::Load`; the Windows
script loader dynamically loads the selected `Sol_T` module, whose static
`CQSCRIPTPROGRAM` entries register program factories and whose DLL entry
supplies a preprocessed `DataDef.i` schema to `MScript::SetScriptData`.
The first start of the external training mission has no saved
`Script/GlobalData`, so these factories and the source-defined data layout
are required. The existing native layout check covered C++ types but could
not prove that the production parser accepts the generated script schema.

**Change and falsifiable check.** A native-only include path in
`ScriptDef.h` avoids `windows.h` and keeps script entry pointer arithmetic
pointer-sized. `DLLScriptMain.h` leaves Win32 `DllMain` intact and exposes
only the heap slot on Linux; it deliberately does not claim module
initialization. The original `Sol_T.cpp` and `Helper/helper.cpp` now form
`frontierx_sol_t_script` object target. A compiler-specific preprocessing
command generates `Sol_T_DataDef.i` in the build directory. The existing
`frontierx_view_parser_probe --script-schema` feeds those bytes to the
production `IViewConstructor2::ParseNewMemory` and checks 292-byte
`MissionData`, Forger/Taos1/Taos2/Acropolis offsets 168/184/200/216, and
the four-byte saved `SolTrain_SelectAndMoveForger` state at offset zero.
The check fails if preprocessing produces a schema the game parser rejects
or if its relevant layout differs from the C++ layout.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each configured
`linux-*-debug`, built `frontierx_sol_t_script`,
`frontierx_sol_t_schema` and `frontierx_view_parser_probe`, and executed
`portability.sol_t_schema` plus `portability.layout`; all passed. Complete
build presets passed; each complete CTest preset reported 48 executed checks
and three command-sandbox cursor/GPU skips. Separate GCC and Clang native
shell processes used the lawfully external Steam installation outside the
repository, the existing `Conquest.ini`, `--mission-source
Sol_Train.dmission`, and explicit `--skip-movies`. Both exited zero after
mounting ten archives, initializing required `IStreamer`, parsing 160633
mission-schema bytes and 3236 Globals bytes, then reporting "mission not
started". An initial invocation without the required `--material-root
Data/mat` failed with exit 2; the corrected invocation above passed. No
mission program, simulation, input, visible GPU unit or save/reload ran.
The native Windows Debug/Final build, script DLL ABI and retail gameplay
comparison were not performed.

**Next blocker.** Link the production `MScript` owner and selected program
into a native module lifecycle that supplies the generated schema, heap and
localization resources and verifies registration failure explicitly. The
current Win32 `LoadLibrary`/`DllMain` path cannot load the retail DLL under
native Linux. The combined App mission link and required UI/services remain
incomplete; its previously measured missing-symbol count is not changed by
compiling the script object alone. The native UTF save container contract
must then support `Mission::Save` and reload. The source-derived training
mission remains an acceptance target, not a playable runtime result.

## Selected Sol_T native schema resource gate (2026-09-27)

**Named blocker.** The previous gate proved that the source-generated
`Sol_T_DataDef.i` parses, but the checked object target did not carry those
bytes into a future native campaign module. On Windows `Sol_T.rc` embeds
`DataDef.i` as a `PARSER` resource, and `DLLScriptMain.h` supplies it to
`MScript::SetScriptData`. A file-only parser check could pass even if the
actual native module lacked the resource.

**Change.** The build-local generated schema now feeds
`frontierx_sol_t_schema_resource`, a native static resource owner linked as
a dependency of the original `frontierx_sol_t_script` object target. The
existing embedding tool accepts a selected symbol name while retaining the
App `data.i` path. The existing production view-parser check reads the
embedded `Sol_T` bytes, checks they match the generated file byte for byte,
then parses those same bytes and verifies 292-byte `MissionData`, the four
unit-reference offsets and the four-byte movement-state record. This tests
the bytes the future native loader can give to `MScript`, while keeping the
Windows DLL resource path unchanged. Generated schema and C++ include files
remain in the build directories, outside source control.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each configured
and built `frontierx_sol_t_script`, `frontierx_sol_t_schema_resource` and
`frontierx_view_parser_probe`. Each executed `portability.sol_t_schema`
and `portability.app_schema`; both passed and the App resource regression
remained intact. Both complete Debug builds and CTest presets passed; the
three cursor/GPU checks skipped in the command sandbox and provide no GPU
runtime evidence. Separate native shell processes under both compilers used
the lawful external Steam installation, mounted its data, initialized
required `IStreamer`, explicitly disabled optional full-screen
`IVideoSystem` with `--skip-movies`, parsed `Sol_Train.dmission`, reported
"mission not started", and exited zero. Neither shell uses the new script
resource yet. No script factory, game simulation, input, visible mission
unit, or mission save/reload executed. Native Windows Debug/Final, script
ABI and retail gameplay comparison were not performed.

**Next blocker.** The production `MScript::Load` still uses Win32
`LoadLibrary` to obtain the campaign module. Its startup condition also
requires a non-null local-resource module; `_localLoadString` and the
training script's objectives/subtitles use that localization path. The
native owner must bind the selected module, generated schema, heap and
localization without claiming success when a required resource is missing.
`MScript.cpp` also has Win32 editor-dialog and menu dependencies that
currently prevent it from compiling as a Linux object; source-only syntax
diagnosis of a temporary copy found these before any native script execution.

## Production MScript owner build and required resource gates (2026-09-27)

**Production path and hypothesis.** `Mission::Reload` calls
`MScript::Load` after map/global/object setup. The previous
`Sol_T` script and schema targets could not connect to that caller because
`MScript.cpp` did not compile as a Linux object. The cheapest disproof was
a direct GCC/Clang object build of the original translation unit, with
Windows editor dialogs kept outside native gameplay. A temporary direct
syntax diagnostic initially found 180 errors, led by Win32 headers and
editor dialogs; this was diagnostic evidence, not a successful build.

**Change.** The Linux include path now uses existing native App globals,
portable event and input declarations, and exact on-disk filename case.
Only the Win32 script-debug menu, dialogs and focus-frame branch are
guarded; native `EnableMouse` still sends the production focus event and
updates scrolling, script UI state and cursor. Native script hotkey event
tokens are checked before narrowing to `U32`. The original part-cache
assignment now returns its reference, removing undefined behavior in a
script-state owner. Source-level fixed-size script-data layouts and the
Windows DLL path were not widened or replaced.

The native `MScript::Load` and `LoadScript` entry points abort with a
specific error instead of claiming a mission load while the module
lifecycle is missing. `GetCountedString` aborts until native script-local
resources are supplied; the original Windows path uses counted 16-bit
string resources and cannot be reinterpreted as Linux four-byte
`wchar_t`. Full-screen script video aborts if called, while the shell's
explicit `--skip-movies` policy remains restricted to optional
full-screen `IVideoSystem`. Native `SetScriptData` now rejects a missing
parser, schema, data object or local-resource handle and aborts if the
production parser fails to define the requested data type. These are
unexecuted failure gates, not replacements for the required contracts.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
compiled `frontierx_app_script_owner` from the production `MScript.cpp`
with the regular CMake Debug preset. Both complete Debug builds and CTest
presets passed; the three host-dependent cursor/GPU entries skipped and
do not prove GPU runtime. Separate native shell processes under both
compilers used the lawful external Steam data outside the repository,
initialized required `IStreamer`, explicitly disabled optional
full-screen `IVideoSystem` with `--skip-movies`, parsed
`Sol_Train.dmission`, reported "mission not started", and exited zero.
The shell does not link or call this script object, so its native abort
paths, program registration, localization, simulation and save/load
have not run. The compiler still reports legacy pointer/integer-cast
warnings in `Objwatch.h`; their impact remains unverified. Native
Windows Debug/Final, cross-module ABI, visible GPU gameplay and retail
behavior comparisons were not performed. The source-level cache-return
fix should also be checked in native Windows rebuilds.

**Next blocker.** Provide a native lifecycle for the selected `Sol_T`
program: bind its generated schema and stable heap, resolve its script
name without using the external Windows DLL, provide the required
script-local counted strings, then execute `MScript::Load` from the
actual `Mission::Reload` caller. The native App service and UI owners
needed by that combined link remain incomplete. In particular, the
selected training script uses objective/subtitle string IDs, so null
local resources cannot be treated as optional. Only after this loader
path works can the first real mission tick and movement checkpoint be
validated.

## Selected Sol_T localization and fresh-loader gate (2026-09-27)

**Production blocker and check.** `Mission::Reload` calls
`MScript::Load`; the selected `Sol_T` objective and subtitle callers
require the script DLL's counted `STRINGTABLE` records. A Win32 resource
pointer cannot be cast to Linux's four-byte `wchar_t`. The previous
native `MScript` object therefore aborted at localization and module
loading. The cheapest falsifiable check was to compile the original
`Sol_T.rc`, decode its selected string IDs and compare the external
mission's script name and update count with the bounded fresh-load
branch. This extends the existing parser check because the shell does
not yet link the App script owner.

**Change.** CMake invokes system `llvm-rc` on repository `Sol_T.rc` with
build-generated `DataDef.i` and a two-constant language compatibility
header. A build-local extractor validates complete RT_STRING groups,
UTF-16 counts, duplicate IDs and resource bounds, then generates stable
counted native strings. `GetCountedString` requires the matching native
resource identity and aborts on missing IDs. Native `MScript::Load`
accepts only the source-selected `Sol_T.dll`, registered program,
parser, schema and required SoundManager/Teletype owners. Native
`Sol_T` initialization supplies its generated schema and resource
identity through production `SetScriptData`; saved-script reload and
full-screen script video remain explicit unsupported failures. The
Win32 DLL and resource branches remain in place. Fixed mission/save
records and simulation ordering were not changed.

**Observed validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8
each configured and built the complete Debug graph, including
`frontierx_sol_t_string_resource`, `frontierx_sol_t_script` and
`frontierx_app_script_owner`. Both executed the existing
`portability.app_schema` and `portability.sol_t_schema` checks: the
source resource returned the counted 14-character "Select Forger."
objective, 53-character movement objective and 219-character subtitle,
and rejected an unknown ID. The existing read-only
`frontierx_view_parser_probe --mission-schema` ran on the lawfully
external Steam `DATA/Maps/Campaign/Sol_Train.dmission` under both
compilers. Its 3236-byte Globals record round-tripped after production
symbol correlation and yielded `Sol_T.dll` and update count zero.
Both complete CTest presets passed; three host-dependent cursor/GPU
checks skipped in the command sandbox per preset. Both native shell
processes initialized required `IStreamer`, disabled optional
full-screen `IVideoSystem` only with explicit `--skip-movies`, parsed
the external mission source and exited zero while reporting "mission
not started". Those shell runs did not execute this new script branch
or a visible GPU gameplay path. Windows Debug/Final, script/module ABI,
retail string equivalence, simulation and full save/reload were not
checked.

**Next blocker.** Link the actual `Mission::Reload` caller, `MScript`
and `Sol_T` registration with required App service owners into one
native process, then execute the fresh script load on the external
mission. The current shell only parses the mission and does not link
either script object. Resolve remaining App symbols through their
production owners before claiming a started mission; subsequently
verify Forger creation, normal input and movement, the named movement
checkpoint and full mission save/reload.

## Combined mission-script heap link gate (2026-09-27)

**Production blocker and hypothesis.** The native shell currently omits
the compiled `Mission`, `MScript` and `Sol_T` objects. Linking their
real caller chain is the cheapest way to identify the next startup
owner. A temporary link based on the successful shell's Ninja link
command, with 19 additional compiled App/script objects, initially
failed because `DLLScriptMain.h` defined `HEAP` in native `Sol_T` while
the already-linked `NativeHeap.cpp` defined the same host global. A
statically linked campaign program must share the process heap;
separate heap globals would also break cross-owner allocation and
deletion. This is runtime-only ownership, not a saved field or module
ABI layout change.

**Change and evidence.** The native branch of `DLLScriptMain.h` now
declares the host `HEAP` instead of defining it; its Windows DLL
declaration and `DllMain` branch remain unchanged. GCC 16.2.1 and
Clang 22.1.8 each rebuilt the production `frontierx_sol_t_script`
object and completed their full Linux Debug builds and CTest presets.
The existing schema checks still executed; three command-sandbox
cursor/GPU checks skipped per preset and provide no GPU runtime
evidence. The repeated temporary GCC link used the shell's actual
Ninja link command and added those 19 objects before the existing
static libraries, followed by `frontierx_sol_t_string_resource`,
`frontierx_sol_t_schema_resource` and
`frontierx_native_global_data_format`. Its diagnostic remained in
`/tmp`: it reported zero multiple definitions, but 194 unresolved
references across 38 distinct production symbols. This failed link
was not run. The most frequent missing symbols were `SYSMAP` (24),
`SCROLLTEXT` (20), `MUSICMANAGER` (20), `STATUS` (16) and `HINTBOX`
(14). Their actual production implementations and startup dependencies
must be integrated; successful object builds alone cannot prove a
mission load. Neither external data nor GPU gameplay was executed in
this specific diagnostic. Native Windows Debug/Final, cross-module ABI,
retail gameplay, movement, simulation and full save/reload checks
remain unperformed.

`nm -C` on both compiler outputs showed an undefined `HEAP` reference
in the `Sol_T.cpp` object and the single definition in
`libfrontierx_native_heap.a`, confirming the intended native ownership
at the symbol boundary.

**Next blocker.** Close the map and required UI/service owners in the
combined link, beginning with their real mission-load callers and
global registration order. `ScrollingText.cpp` is coupled to the Win32
FullScreen/UI layer, while `Mission.cpp` still references it for
quick-save/load notifications. No null or success-return substitute
for those required paths is acceptable. Then put the linked mission
owner into the shell and execute `Mission::Reload` on the external
training mission.
Source review identifies `SYSMAP` as the earliest high-count mission
dependency: `Mission::Reload` calls `ISystemMap::Load` before objects and
scripts, while `ObjectList` obtains its map timing through the same
interface. A direct GCC syntax attempt on the unmodified `SysMap.cpp`
stops at its Win32 `pch.h` include; its full owner also contains GDI
terrain-map drawing and menu registration. The next implementation
must preserve the source map save/load and timing behavior while
isolating that Windows presentation layer.

## Selected system-map fresh-load gate (2026-09-27)

**Production blocker and decision.** `Mission::Reload` calls
`SYSMAP->Load` before object and script loading; `ObjectList::Render`
later reads `GetMapTiming`. The production `SysMap.cpp` is coupled to
Win32 menus, GDI terrain thumbnails and `IViewer`, so its unmodified
translation unit stops at `windows.h` in a direct native syntax
attempt. The existing `ISystemMap` boundary permits a bounded native
implementation for the selected training path. The falsifiable input
question was whether the lawfully external mission contains the
`SysMapSaveload/SysMap` child that the source `SysMap::Load` tries to
open. The existing mission-filesystem check was extended rather than
adding a disconnected probe.

**Change.** A native `ISystemMap` App registrar and factory use the
unchanged interface. `Load` follows the source's absent-directory and
absent-child fresh-state branches. A present saved map rejects the
unsupported animation state explicitly; other unimplemented map
presentation, scrolling and animation methods also abort with their
names. `GetMapTiming` returns zero because this owner never renders a
map frame, matching the measurement contribution of an undrawn map.
`Save` writes nothing while there are no mission animations, as in the
source owner. The selected `Sol_T` program does not call the map
animation methods before the movement checkpoint. Native
`SysMap.h` includes the exact-case `GridVector.h` while the Windows
include branch remains unchanged. No persistent record or cross-module
interface layout was altered. This adapter is limited to the named
fresh mission; it is not a replacement for the source map UI or a
general saved-map loader.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 each
configured and built the complete Debug graph, including the native
map owner, data-root check and shell. The existing
`frontierx_data_root_probe --mission-filesystem` executed on the
lawful external Steam `Sol_Train.dmission` under both compilers:
`SysMapSaveload/SysMap` was absent, the native `ISystemMap::Load`
fresh branch executed, map timing was zero, and four QuickSave object
records opened. The existing correlated Globals check also confirmed
an empty `baseTerrainMap`, so this mission takes `Mission::Reload`'s
direct mission-file `SYSMAP->Load` branch rather than first loading a
separate terrain mission. Both full CTest presets passed with three
command-sandbox cursor/GPU skips each; the skips are not runtime
evidence. Both external-data shell processes mounted ten archives,
initialized required `IStreamer`, explicitly disabled optional
full-screen `IVideoSystem` with `--skip-movies`, parsed the mission
schema and Globals, and exited zero while reporting "mission not
started". Initial shell invocations without a writable isolated
`XDG_CONFIG_HOME` exited 5 at native preference save; the corrected
isolated `/tmp` preference runs exited zero. They do not call App
global startup or the map registrar. A temporary GCC combined link
using the shell and 19 compiled App/script objects had no multiple
definitions and no missing `SYSMAP`; it still failed on 169 unresolved
references across 37 distinct production symbols. No visible GPU
mission, ordinary input, simulation, save/reload, native Windows
Debug/Final or cross-module ABI check ran.

**Next blocker.** The combined link next lacks `SCROLLTEXT` (20
references), `MUSICMANAGER` (19), `STATUS` (16), `HINTBOX` (14) and
App localization. `ScrollingText.cpp` and other UI owners rely on
Windows FullScreen/Frame services; resolve their required training
interaction and quick-save/load calls through production owners or
focused interface adapters with explicit unsupported behavior. Then
call App global startup and the actual `Mission::Reload` path in the
shell. Saved map animations and map presentation need their own
 productive implementation before they can be claimed.

## Selected training music decode and disk-layout gate (2026-09-27)

**Production blocker and real caller.** `SolTrain_Start` calls
`MScript::PlayMusic("solarian_game14.wav")`. The source
`MusicManager` then opens the song through required `IStreamer`, while
`Mission::Reload` calls `MusicManager::Load` before `MScript::Load`.
The existing external-audio check initially failed on this selected
music input. A temporary out-of-repository file and `ffprobe` measured
a 10028080-byte IAC WAV, 22050 Hz stereo and about 909.6 seconds; the
FFmpeg CLI decoded it to a null sink without an error. That result
separates the native 64-MiB decoded-buffer limit from damaged retail
media or an observed CQ2 music defect. The temporary file was removed.

**Change and checks.** The native FFmpeg adapter retains a 64-MiB
compressed-input limit and accepts up to 256 MiB of decoded F32 PCM;
the SDL `IStreamer::Open` output check uses the same bounded limit.
The existing window-manager probe now uses a bounded 150-ms play/stop
path for long external audio and retains full completion for short
speech. Under GCC 16.2.1 and Clang 22.1.8 on Linux x86_64, both
FFmpeg-enabled probes decoded the external music into exactly
160448512 bytes with identical FNV1a64
`16776800164497827250`, opened it through production `IStreamer`,
observed `PLAYING`, then stopped and closed it with SDL dummy audio.
Both compilers also completed the selected `STMG03.wav` speech
regression (1487872 decoded bytes, identical FNV1a64
`10538922693386034611`). The two complete Debug builds and CTest
presets passed with three command-sandbox cursor/GPU skips each.
Host PulseAudio attempts did not complete in the sandbox, emitted an
operation-permission error and were interrupted with exit 130; no
audible hardware result is claimed.

The existing external mission-filesystem check opened a 176-byte
`MT_MUSIC_DATA/Music` child in `Sol_Train.dmission` under both
compilers. Its production parser schema describes four 44-byte
`SONG` entries with handle at offset 4, volume at 8 and filename at
12. The native `SONG` carries an eight-byte stream pointer and cannot
be read directly from that Win32 record; `MusicManager::Load` and
save need a fixed-width conversion that never serializes native
handles. The currently successful external-data shells initialized
`IStreamer`, explicitly skipped optional full-screen `IVideoSystem`,
parsed the mission and reported "mission not started"; they did not
initialize App audio or run `MusicManager` or `Sol_T`. No visible
Forger, ordinary input, simulation, full save/reload, native Windows
Debug/Final or cross-module ABI check ran.

**Next blocker at this gate.** Bring the production music owner into the native
mission closure with a checked 176-byte disk/runtime conversion and
the existing `IStreamer` for the selected music call. Keep active
stream handles process-local; reject malformed or unsupported music
save data. The combined App link also still lacks scrolling-text,
status, hint and other UI/service owners before `Mission::Reload` can
execute.

## Selected music save-layout conversion gate (2026-09-27)

`Mission::Reload` calls the production `MusicManager::Load` before
`MScript::Load`. The selected lawfully external `Sol_Train.dmission`
contains `MT_MUSIC_DATA/Music` with four 44-byte Win32 `SONG` entries.
The source manager reads that record directly into native `SONG` after
symbol correlation. On Linux x86_64, native `SONG` is 56 bytes because
its `HSTREAM` pointer is eight bytes. A direct read would corrupt the
playlist and treat a saved process handle as live state. The decision
gate is whether a fixed-width conversion can preserve the selected
mission's playlist without restoring those invalid handles. The
existing external mission-schema probe already owns the real
`IViewConstructor2` and mission filesystem path, so no new probe was
needed.

`NativeMusicDataFormat` defines the 176-byte disk buffer and asserts
the native size and field offsets. Its decoder reads the two playlist
bits and IEEE-754 volume from explicit little-endian words, retains
the 32-byte filename, rejects non-finite volume and unterminated
names, and clears every runtime stream handle. Its encoder writes the
same saved fields with zero handles. The production manager has not
yet been adapted to call it; this is a checked disk/runtime contract,
not a music-manager or mission runtime result. The supported Windows
source and save format are unchanged.

On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 built
`frontierx_view_parser_probe` in the `linux-*-debug` presets and ran:

```text
frontierx_view_parser_probe --mission-schema <lawful-install>/DATA Sol_Train.dmission
```

Both runs exited zero. The probe read the external 176-byte child,
passed it through production `IViewConstructor2` symbol correlation,
decoded four songs, checked all runtime handles were null, confirmed
playlist flags plus volume/name bytes after re-encoding, and rejected
a deliberately unterminated filename. It did not copy retail bytes
into the repository. Both complete Debug builds and CTest presets
passed with 48 executed checks and three command-sandbox skips each;
the skipped cursor/GPU entries are not runtime evidence. A fresh GCC
SDL/no-FFmpeg build of `frontierx_native_window_manager_probe` and its
focused CTest passed, preserving the default decoder-disabled graph.
No native Windows Debug/Final or cross-module ABI check ran.

The next concrete blocker is the original `MusicManager.cpp`: it still
depends on Win32 debug viewer/menu and full-screen event owners, so it
cannot yet be linked into the native App mission. Its Linux `Load` and
`Save` must use the fixed-width converter, initialize its playlist
through required `IStreamer`, and propagate malformed-file or stream
failure rather than report success. Only then can the combined mission
link advance to the next unresolved App owner. This work has not
started the mission or met any gameplay acceptance condition.
## Selected mission audio-ownership gate (2026-09-27)

The selected `Sol_T` program calls `MScript::PlayMusic`, which delegates
to `IMusicManager`; the source manager opens its track through the App
`STREAMER` and `MUSICDIR` globals. The source-profile `IStreamer`
aggregate already existed in the native SystemContainer, but the shell
had neither bound it to those App globals nor called `IStreamer::Init`.
`Mission::Reload` would therefore enter its music call with missing
required audio state. `IVideoSystem` is a separate full-screen movie
owner and remains disabled only with the explicit `--skip-movies`
policy; the selected music path is not disabled.

The native shell now queries the exact `IStreamer` aggregate from the
active `GameSystem` container and resolves `Music` through its existing
lawful external data-root mount. `NativeAppAudioBinding` retains both
references and clears both globals at teardown. It is constructed
after `GameSystem` so its aggregate reference is released first. Once
the native window callback is registered, the shell passes its live
window token, `CQE_STREAMER` message and source buffering hints to
`IStreamer::Init`; failure returns a nonzero shell exit. The existing
shell process test covers a supplied source-profile streamer and a
missing-streamer rejection. The test uses synthetic empty data
directories and SDL dummy audio, so it does not prove music playback.

The initial synthetic process test reproduced a SIGSEGV after
`IStreamer::Init`. `coredumpctl list` was unavailable to this command
sandbox and `gdb` could not attach (`ptrace: Operation not permitted`).
Bounded diagnostic output placed the failure after the App audio
binding's destructor. Source lifetime review showed that the binding
had been declared before `GameSystem`, so its aggregate reference was
released after container teardown. Moving the declaration after
`GameSystem` removed the crash; the same process test then exited zero.
The machine had ample free memory, so no OOM evidence was observed.
The temporary diagnostic output was removed.

GCC 16.2.1 and Clang 22.1.8 built the complete `linux-*-debug`
presets and ran each CTest preset: 48 executed passes, with the cursor
and two GPU checks skipped in the command sandbox. Both compiler
shells also ran against the lawful external Steam `DATA` root and
`Sol_Train.dmission` with `SDL_VIDEODRIVER=dummy`,
`SDL_AUDIODRIVER=dummy`, the source `Conquest.ini`, and explicit
`--skip-movies`. Each printed `Native App mission IStreamer initialized
with Music source`, exited zero, and still reported `mission not
started`. This verifies native source binding and SDL dummy-device
initialization, not audible hardware output, actual App music playback,
simulation or GPU gameplay. Native Windows Debug/Final and module ABI
checks were not run. A separate GCC SDL/no-GPU, no-FFmpeg shell build
compiled but did not link because of existing `TRANSFORM::get_roll`,
`get_pitch`, `get_yaw` and `ICQImage::Assert` references in the
no-GPU App closure; it was not executed.

The next required owner is the production `MusicManager`: its selected
mission load must use the checked 176-byte disk conversion, then its
playlist and `Sol_T` music request must use the now initialized
`IStreamer`. The native shell still does not run `Mission::Reload` or
`Sol_T`, and no gameplay acceptance condition has passed.
## Production MusicManager owner startup gate (2026-09-27)

`Mission::Reload` calls `MUSICMANAGER->Load` before `MScript::Load`,
and the selected `Sol_T` program later calls `MScript::PlayMusic` for
`solarian_game14.wav`. The source `MusicManager.cpp` was absent from
the Linux link: it included the Windows PCH, menu and debug document
viewer, and its saved `MT_MUSIC_DATA` read assumed Win32 pointer
width. Merely having an initialized `IStreamer` could not satisfy
either App call. The next decision gate was whether the original owner
could compile and link while preserving its playlist and stream
behavior.

The original source now has a native branch for its Windows-only
viewer/menu and message types. The manager still implements
`IMusicManager` and `IEventCallback`; the native factory creates that
same source class after `STREAMER`, `MUSICDIR` and `EVENTSYS` are
bound, subscribes it to `CQE_STREAMER` window messages, and enables
it. The shell retains it through `NativeAppMusicBinding`, whose
lifetime ends before audio and system aggregate teardown. The
supported Windows registrar and UI branch remain in place.

The native `Load` branch requires the exact 176-byte music child,
calls the real mission symbol correlator, decodes it into native
`SONG` entries, clears stale handles and preserves the source
load-time transition. The native `Save` branch writes exactly 176
bytes when any song is playing and otherwise leaves an empty Music
child as the source owner does. `PlayMusic` bounds filenames to the
32-byte saved field, and `OnUpdate` rejects a failed required stream
open, restart or volume operation instead of silently keeping a
playing flag. Missing or malformed mandatory save data aborts with a
diagnostic because the unchanged `IMusicManager::Load`/`Save` methods
return `void`. The native owner invokes `SOUNDMANAGER`'s volume fade
when that owner exists; without it, the music starts or stops
immediately. This is a temporary audio-transition difference, not a
media disable or simulation change.

GCC 16.2.1 and Clang 22.1.8 built
`frontierx_app_music_manager_owner`, linked it into
`frontierx_native_shell`, and completed the full `linux-*-debug`
build and CTest presets: 48 executed passes, with the cursor and two
GPU checks skipped in the command sandbox. The existing shell process
test now asserts successful manager startup and an explicit failure
when the required source-profile `IStreamer` is absent. Both native
shells ran with the lawful external Steam `Sol_Train.dmission`,
source `Conquest.ini`, explicit `--skip-movies` and SDL dummy
video/audio; each printed `Native App MusicManager initialized`,
exited zero and still reported `mission not started`. This is
external-data startup/teardown runtime, not music playback or mission
runtime. Native `Load`, `Save`, `PlayMusic`, audible hardware output,
GPU gameplay, native Windows Debug/Final and cross-module ABI checks
remain unperformed.

The next concrete integration blocker is `Mission::Reload` itself:
the native shell only opens and parses the mission source, so there
is no active `IMission` to supply `CorrelateSymbol` for
`MusicManager::Load`. The remaining App owner/link closure must be
brought into that real load call before the selected `Sol_T` script
can exercise music, object creation or simulation. No gameplay
acceptance condition has passed.

## Source App string-resource link gate (2026-09-27)

The combined `Mission.cpp`/`MScript.cpp` link lacked wide and narrow
`_localLoadString` references. The real Windows owner is
`PrintHeap.cpp`, which reads the source `Globals.rc` resource table;
the production `IStringTable` system aggregate is a different owner.
The nearest callers are mission quick-save/status text and script
alerts. The falsifiable check was whether source-built App strings
would satisfy both APIs in the existing external-mission shell and
reduce the combined link's missing-symbol set. The existing shell
process test covers the path, so no new probe was added.

The native build now extracts only `STRINGTABLE` blocks from the
original `Globals.rc`, compiles them with `llvm-rc` at source codepage
1252, and converts the counted RT_STRING records to an embedded,
sorted lookup. Both App lookup variants use that table; a missing ID
aborts with a diagnostic rather than returning an empty success value.
The native narrow lookup supports direct Latin-1 code points and
rejects a code point needing unimplemented CP1252 conversion. Windows
`PrintHeap.cpp` and its resource handling remain unchanged. No
persistent or cross-module type was altered.

`cmake --build --preset linux-gcc-debug` and
`cmake --build --preset linux-clang-debug` completed with GCC 16.2.1
and Clang 22.1.8. Both `ctest --preset linux-*-debug --output-on-failure`
runs passed 48 executed checks; `portability.app_cursor` and both GPU
checks skipped in the command sandbox. The existing native-shell test
checked the source `IDS_DISABLED` wide/narrow lookup on synthetic data.
Both compiler shells also ran with lawful external Steam `DATA`,
repository `Data/mat`, source `Conquest.ini`, `Sol_Train.dmission`,
explicit `--skip-movies`, isolated XDG settings, SDL dummy video/audio,
and `--hidden --smoke-ms 1`. Each printed `Native App source string
resources initialized`, exited zero and still reported `mission not
started`. This was source/localization runtime, not GPU gameplay.

A temporary GCC diagnostic link of the shell plus 19 compiled App and
script objects now fails on 126 references to 34 distinct symbols,
down from 150/36 before this change. The leading missing owners are
`STATUS`, `SCROLLTEXT`, `HINTBOX`, `SAVEDIR`, and `UNBORNMANAGER`.
`SAVEDIR` cannot be replaced with the current read-only
`NativeUtfFileSystem`: `Mission::SaveAs` requires a writable UTF
factory and output stream. The next concrete step is to connect the
first UI and save owners with their real caller contracts, then rerun
the combined link and execute `Mission::Reload`. None of the five
gameplay acceptance conditions passed. Native Windows Debug/Final,
cross-module ABI, actual save/reload, hardware GPU and retail gameplay
checks were not run.

## Native packet-selection search gate (2026-09-27)

The temporary combined mission link exposed five `dwordsearch`
references from `CommPacket.h::VerifySet`. `Search.asm` supplies this
Win32 x86 fastcall function; packet duplicate checks and `ObjSet`
callers require the address of the first matching 32-bit ID. The
falsifiable Linux contract is that duplicates return the earliest
matching address and absent or empty ranges return null. The existing
production offline-packet check can cover this contract, so no new
probe was added. The native scalar function preserves scan order and
does not change packet fields, serialized formats or simulation order.

GCC 16.2.1 and Clang 22.1.8 built `frontierx_native_search`,
`frontierx_net_buffer_probe` and `frontierx_native_shell`; focused
`portability.net_buffer` and `portability.native_shell` CTests passed
under both. Both full Linux Debug builds and CTest presets passed 48
executed checks, with the cursor and two GPU checks skipped in this
command sandbox. The packet check used synthetic IDs and the original
offline `NetBuffer`/`NetPacket` owners; external Steam-data shells
still opened `Sol_Train.dmission`, initialized App strings, streamer
and music manager, and reported `mission not started`. No move command
was sent to a mission unit. A temporary GCC App/script link now has
121 unresolved references across 33 symbols, down from 126/34. It
produced no executable and was not run. The next concrete blocker is
the remaining required UI/save and simulation-owner closure before
`Mission::Reload` can execute. Windows x86 assembler comparison,
native Windows Debug/Final, cross-module ABI, gameplay, save/reload
and hardware GPU checks were not run.

## Native load progress and source build-version gate (2026-09-27)

`Mission::Reload` calls `CreateInProgressAnim` immediately after
acquiring the App cursor, then uses `IPANIM::UpdateString`,
`SetProgress` and `GetProgress` throughout the source load order.
The Windows `InProgressAnim.cpp` owns a Win32 surface/background
thread. A native implementation at the existing `IPANIM` boundary
now owns a reference-counted, text-based progress state, validates
that each source App string ID exists, rejects invalid percent values
and emits percentage/resource-ID updates. `Start` and `Stop` govern
emission; creation and destruction preserve the source
`CQFLAGS.bInProgressAnimActive` lifetime flag. It deliberately has
no loading animation or background
render/update thread; the selected shell's terminal shows progress
while actual game rendering remains a separate required gate. The
Windows implementation and `IPANIM` vtable are unchanged. This
addresses a real loader call, not gameplay or a synthetic scene.

The source `System.cpp::GetBuildVersion` reads the `Conquest.rc`
VERSIONINFO `FileVersion` and packs its four decimal fields into a
32-bit multiplayer number, masking the last field to one byte. The
native owner now generates the same string and packed number from
the source RC at build time. Current source `02.00.05.2501` yields
`0x020005c5`; it does not claim retail version equality. The existing
view-parser check asserts the source conversion and both compiler
processes read the lawful external `Sol_Train.dmission` global-data
`gameSettings.version` as zero. Consequently the selected fresh
mission's source version-compatibility conditional skips comparison;
native saves still need the generated value. No persistent record or
cross-module interface layout changed.

GCC 16.2.1 and Clang 22.1.8 built the complete `linux-*-debug`
presets and passed both full CTest presets: 48 executed checks, with
the cursor and two GPU checks skipped in the command sandbox. The
existing shell process test now checks text progress at 5 percent
with a synthetic mission-source directory. Both external-data shell
runs used the lawful Steam `DATA`, repository `Data/mat`, source
`Conquest.ini`, `Sol_Train.dmission`, explicit `--skip-movies`, SDL
dummy video/audio and isolated XDG settings. Both printed the native
progress-owner initialization, emitted 0 and 5 percent for resource
2101, exited zero and still reported `mission not started`.
`frontierx_view_parser_probe --mission-schema` on that external
mission printed saved build version zero under both compilers.

A temporary GCC link of the shell plus 19 compiled App/script objects
remains unsuccessful with 115 unresolved references across 31 symbols,
down from 121/33 before these owners. `STATUS`, `SCROLLTEXT`,
`HINTBOX`, `SAVEDIR` and `UNBORNMANAGER` lead the missing closure.
The earliest subsequent runtime work is to connect the actual
mission UI/progress-adjacent owners without masking failure, then run
`Mission::Reload` against the external mission. A writable UTF save
factory is still required before quick-save/reload can pass. No
visible unit, movement, simulation checkpoint, save/reload or native
GPU game runtime occurred. Native Windows Debug/Final and module ABI
checks were not run.

## Native status and hint resource-client gate (2026-09-27)

The production `TResClient` users in mission and object paths acquire
`STATUS` and `HINTBOX` by `GetOwnership`, subscribe to each
`IResourceClient` connection point, and release ownership when their
priority ends. The original App owners call `Resource::updateResource`
on `CQE_UPDATE`, which notifies waiting clients. Those two global
owners were absent from the native combined link. The existing shell
process test could exercise their startup and source-string behavior,
so no separate probe was added. These resources do not cross a
persistent-format boundary; the existing `IResource`/status/hint
interfaces and Windows implementations were not replaced.

The native owners reuse the production `Resource` and
`ConnectionPointContainer` templates, publish `IResourceClient`
outbound points, subscribe to the active EventSystem, and clear their
global pointers after releasing the owner. `SetText` resolves the
original App string table; wide string setters convert Unicode to
UTF-8 and print to stderr. `SetDefaultState` clears the text ID. The
terminal output is a temporary presentation: `GetHeight` returns zero
because no SDL overlay is drawn, and the first playable mission still
needs an in-window status/hint presentation if those messages convey
necessary player information. Ownership priority and deferred
notification follow the reused production template; direct
client-preemption callbacks were not yet exercised by the shell test.

GCC 16.2.1 and Clang 22.1.8 built
`frontierx_native_app_text_resources` and their complete
`linux-*-debug` presets. Both full CTest presets passed 48 executing
checks; `portability.app_cursor` and the two GPU tests skipped in the
command sandbox and are not runtime evidence. The existing
`portability.native_shell` process check observed `Disabled` through
both synthetic status/hint callers, both outbound connection points,
and reset IDs. Both compiler shells also ran with lawful external
Steam `DATA`, repository `Data/mat`, source `Conquest.ini`, selected
`Sol_Train.dmission`, explicit `--skip-movies`, SDL dummy video/audio,
and isolated XDG settings. Both exited zero, printed native text
initialization and `Disabled` twice, and still reported `mission not
started`. No game GPU path ran.

The temporary GCC combined shell plus 19 App/script objects link
still fails on 85 unresolved references across 29 symbols, reduced
from 115/31. `SCROLLTEXT` (14 references) is the next UI owner and
calls the required sound manager for its notification cue; a native
text implementation must either play it or explicitly disable only
that optional cue with a reason. `SAVEDIR` (10 references) requires a
writable UTF factory for real save/reload; the current read-only mount
must not return false success. No visible unit, input movement,
simulation checkpoint or save/reload occurred. Native Windows
Debug/Final, cross-module ABI, hardware GPU and retail gameplay
validation remain unperformed.

## Native scrolling message and explicit cue policy gate (2026-09-27)

`Mission::Save`, `Mission::QuickLoad`, mission events, `MGlobals` and
`OpAgent` use the App `SCROLLTEXT` global to show source resource IDs or
wide messages. The original `ScrollingText::SetText` and
`SetTextString` always call `SFXMANAGER->Play` for the `BEACON` cue
after optionally opening it. This was the leading missing symbol in
the temporary App/script link. The selected single-player path does
not require multiplayer-attributed `SetTextStringEx`, and the existing
shell process test can exercise source-string display without a new
probe. This interface is a runtime UI contract, not a persistent or
cross-module layout change.

The native App text binding now publishes `SCROLLTEXT` before mission
loading. It resolves source App resource IDs, converts the wide text
to UTF-8, and displays messages on stderr. The original beacon sound
is **optional for the selected single-player interaction**: only the
explicit `--skip-notification-cue` option permits a non-empty message
without a sound owner. Without the option, the first actual message
aborts with a diagnostic instead of returning success or dereferencing
a missing `SFXMANAGER`. The source's `SetText(0)` returns
`GR_INVALID_PARMS`; native `SetTextStringEx` aborts as unsupported.
The native adapter does not yet retain/fade four lines, draw an SDL
window overlay, support multiplayer chat menus, or play the cue.
Those missing presentation functions cannot be counted as a playable
in-window UI. Windows `ScrollingText.cpp` and the interface layout are
unchanged.

GCC 16.2.1 and Clang 22.1.8 built both complete `linux-*-debug`
presets. The existing `portability.native_shell` test exercised the
explicit cue-skip branch with synthetic mission-source data, observed
the source `Disabled` message and invalid-zero-ID rejection, and
passed under both compilers. Both full CTest presets passed 48
executing checks; the cursor and two GPU checks skipped in the
command sandbox. Both compiler shells also ran against lawful
external Steam `DATA`, repository `Data/mat`, source `Conquest.ini`
and `Sol_Train.dmission`, with SDL dummy video/audio and explicit
`--skip-movies --skip-notification-cue`. They displayed `Disabled`
from the source resource and the direct wide string `Native message`,
exited zero, initialized required `IStreamer` and still reported
`mission not started`. Neither run rendered game geometry or advanced
simulation.

The temporary GCC combined shell plus 19 App/script object link
still fails on 70 unresolved references across 28 symbols, reduced
from 85/29; `SAVEDIR` is now the leading owner with nine references.
Its writable UTF factory is required for the requested save/reload
acceptance and cannot be satisfied by the existing read-only data
mount. `UNBORNMANAGER` and other production App owners remain in the
same link closure. No native mission load, unit display, movement,
simulation checkpoint or save/reload occurred. Native Windows
Debug/Final, cross-module ABI, hardware GPU and retail gameplay
validation were not run.

## Native UTF save-format encoder gate (2026-09-27)

`Mission::SaveAs` creates a UTF container through its parent
`IComponentFactory` with `CREATE_ALWAYS` and read/write access.
`Mission::Save` then deletes old entries, asks multiple productive
owners to create directories and write records, queries `IUTFWriter`
for `Flush`, and reopens the archive for reading. The existing native
UTF mount is deliberately read-only; it cannot serve `SAVEDIR` or
claim that a save succeeded. The earliest save integration blocker is
therefore a writable parent/archive pair that emits compatible
UTF 1.01 bytes. Before binding one, its encoder must create metadata
the production reader accepts. The existing `portability.data_root`
check already owns generated UTF and filesystem mounting, so that
check was extended instead of adding a parallel probe. Passing this
check permits the writable `IFileSystem` integration to proceed;
failure would require correcting the disk encoder before exposing
`SAVEDIR`.

The new encoder accepts exact-case, slash-separated runtime tree
items and emits a 56-byte fixed-width header, 44-byte directory
records, a names table and uncompressed payloads. It sorts children
before laying out sibling/child links, so input order does not change
bytes. It bounds the archive to 256 MiB and each entry to 64 MiB,
rejects duplicate/conflicting or traversing paths, and leaves output
unchanged on failure. These are runtime types and explicit byte writes;
no native pointers or LP64 `long` fields enter the serialized format.
The original Windows `UTFShare` writer and `IUTFWriter` interface were
not changed; exact Windows-writer byte parity was not tested.

GCC 16.2.1 and Clang 22.1.8 configured and built
`linux-*-debug`, including `frontierx_platform` and
`frontierx_data_root_probe`. Both full CTest presets passed; the
cursor and two GPU checks skipped in the
command sandbox. The existing `portability.data_root --self-test`
encoded one nested `MT_GlobalData/Globals` payload and an empty
directory, validated four directory entries and one reachable file,
read the payload through the metadata reader, wrote the bytes to a
temporary file, and reopened/read them through the native production
`IFileSystem` UTF owner. It also observed identical bytes after
reordering input and unchanged output after duplicate/traversal
rejection. Both compiler builds separately ran
`frontierx_data_root_probe --mission-filesystem` against lawful
external Steam `DATA` and `Sol_Train.dmission`: each opened four
QuickSave object records, a 176-byte music record, 160633 ParseData
bytes and 3236 GlobalData bytes. No external file was written.
A focused Clang ASan/UBSan executable compiled the encoder and
production metadata reader with nested saved-object paths, decoded
its payload and exited zero without a reported finding; LeakSanitizer
was disabled on this host.

This encoder is not yet connected to a writable `IFileSystem`,
`IUTFWriter::Flush`, or a user save directory. A temporary GCC
combined App/script link remains unsuccessful with 70 unresolved
references across 28 symbols; `SAVEDIR` remains first with nine
references. The next concrete implementation is a writable directory
factory and UTF archive owner that use this encoder, reject incomplete
writes, flush atomically, reopen through the production reader and
then bind `SAVEDIR` outside the retail-data root. No mission load,
unit display, movement, simulation checkpoint or save/reload occurred.
Native Windows Debug/Final, cross-module ABI, hardware GPU and retail
gameplay validation remain unperformed.

## Writable save flush and current mission link check (2026-10-03)

**Blocker and hypothesis.** The current code already implements the
`Mission::SaveAs` writable parent/archive contract in
`NativeWritableUtfArchive.cpp` and binds `SAVEDIR` through the native
shell's explicit `--save-root` outside the retail data root. The
existing `portability.data_root` check is the closest executable
caller: it creates a UTF child through the save parent, writes a
record, calls the production `IUTFWriter::Flush`, reopens it through
the production reader and compares the record. A fresh build failed
that check at the first flush, disproving the older claim that the
remaining obstacle was implementing the writer. The check of the
temporary output path treated an absent path as a filesystem error on
this host.

**Change and execution.** The writer now calls `symlink_status`,
accepts only `no_such_file_or_directory` as an absent temporary path,
and still rejects an existing file, symlink or other status error.
The writer remains in the native platform layer; UTF disk widths and
Windows writer code are unchanged. Fresh `linux-gcc-debug` and
`linux-clang-debug` configurations with optional FFmpeg audio disabled
built `frontierx_data_root_probe` and `frontierx_native_shell` on
Linux x86_64 with GCC 16.2.1 and Clang 22.1.8. Before the change,
`ctest --test-dir /tmp/frontierx-gcc-goal -R '^portability.data_root$'
--output-on-failure` aborted with `Native UTF save failed: UTF save
temporary path already exists or cannot be checked`. After the
change, that exact CTest check passed under both compilers. Its inputs
were generated save-shaped UTF data; this is automated execution of
the save archive contract, not `Mission::Save` or retail gameplay.
The pre-existing GCC preset cache pointed at a missing external FFmpeg
library, so these fresh configurations did not test IAC playback.

**Next production blocker.** A temporary GCC link based on the shell's
actual Ninja command, with 19 additional compiled App/script objects
and the existing native `PrimitiveBuilder2` owner, still failed with
61 unresolved references across 27 distinct symbols. The largest
sets are `UNBORNMANAGER` (6), `DisableTextures` (6), `COMMTRACK` (5),
`MENU` (5), and `PARTMAN`, `FIELDMGR` and `EFFECTPLAYER` (4 each).
`Mission.cpp` itself also requires both overloads of
`ISPlayerAI::CreateInstance`; direct compilation of its production
`SPlayerAI.cpp` with the mission target's GCC flags stops at the
Windows-only `pch.h`/`windows.h` include. A temporary source-only
preflight replacing that include and correcting case-sensitive header
names exposed further dependencies on `FULLSCREEN` event callbacks,
banking and mission types, plus pointer-to-`U32` casts. No exploratory
AI source edits were retained. The next step is to make that
production AI owner compile without narrowing runtime pointers and
close its real UI/event dependencies,
then continue the combined link and call `Mission::Reload`. The
temporary link produced no executable and was not run. No external
mission, unit, normal input, simulation checkpoint or mission
save/reload executed in this check. Native Windows Debug/Final,
cross-module ABI, retail-runtime and visible mission GPU checks remain
unperformed.

## Selected mission AI and App render owner boundary (2026-10-03)

**Production callers and integration decision.** `Mission::loadPlayerAI`
checks `PLAYERAI/Player02..Player08` in a mission-defined game and
creates an `ISPlayerAI` only if a saved player directory contains an AI
entry. The selected `Sol_T.cpp` source does not request
`EnableEnemyAI` for the bounded Forger movement checkpoint. The
full `SPlayerAI.cpp` owner still depends on Windows precompiled headers,
`FULLSCREEN` event callbacks and runtime pointer casts. The existing
mission-filesystem check was extended to count those selected external
directories; an unexpected saved AI directory now fails that check
and would require porting the real AI owner before mission startup.
This is a source-reviewed branch plus read-only external-data execution,
not proof that `Mission::Reload` ran.

**Selected branch and render state.** A native-only
`ISPlayerAI::CreateInstance` boundary aborts with a diagnostic on
either overload. It cannot silently claim to have created AI and is
not a substitute for the production AI in other missions. The native
GCC object was also called by a temporary standalone harness with a
named AI type; it printed the unsupported-owner diagnostic and exited
with SIGABRT (status 134), with core-file creation disabled. The native
App render-state owner now implements `DisableTextures` with the same
stage-zero diffuse selection and stage-one disable sequence as
`System.cpp`; each backend rejection aborts. The existing game-material
manager check exercises the state through the native primitive and
observes a cleared texture and diffuse-only color/alpha arguments.
The original `UnbornMeshList.cpp` now compiles as a native object,
retaining its production registrar, construction preview and queued
mesh logic. Its native child enumeration follows the same bounded
depth-first order as the source `Blinkers.cpp` helper; runtime execution
of that owner remains open. The Windows branches and persistent
records were not changed.

**Validation and next blocker.** On Linux x86_64, GCC 16.2.1 and
Clang 22.1.8 built `frontierx_native_player_ai_boundary`,
`frontierx_app_unborn_mesh_owner`, `frontierx_data_root_probe` and
`frontierx_game_material_manager_probe` with optional FFmpeg audio
disabled. Both `portability.data_root` and
`portability.game_material_manager` CTests passed. Both compiler
mission-filesystem runs opened the lawful external Steam training
mission and reported zero saved player-AI directories, four QuickSave
objects, 160633 ParseData bytes and 3236 Globals bytes. A temporary
GCC link based on the shell's Ninja command with 21 additional
App/script objects still failed on 46 references across 23 symbols;
`COMMTRACK` (5), `MENU` (5), `PARTMAN` (4), `FIELDMGR` (4) and
`EFFECTPLAYER` (4) lead the list. The link was not executable and no
mission simulation, normal input, visible unit, save/reload or GPU
mission frame ran. The next production owner is `CommTrack.cpp`, whose
normal command tracker also needs a native non-dialog event path.
Native Windows Debug/Final, cross-module ABI and retail-runtime checks
remain unperformed.

## Production command tracker and App particle binding (2026-10-03)

**Production owner and caller.** The combined `Mission`/`ObjectList`
link lacked `COMMTRACK`; the original `CommTrack.cpp` owns command
history and receives `CQE_UPDATE` from the ObjectList event stream.
Its Win32 dialog is a debugging settings interface, separate from
normal unit command delivery. The source `Conquest.cpp` queries
`IParticleManager` from `GAME.GS` before initializing App particles,
and `ObjectList::Update` invokes `PARTMAN->Update`. The native
GameSystem already creates the production particle aggregate from
the source profile, but the App global had no binding. These are
runtime owners and event values, not saved or module ABI fields.

**Change.** The native `CommTrack.cpp` build uses the portable App
declarations, retains its original registrar, command history and
default-setting behavior, and bounds the pointer-sized encoded
`CQE_UPDATE` value before converting it to `U32` milliseconds. Its
Win32 settings dialog remains on Windows. The native-only ObjectList
omits calls that populate the Win32 part-debug menu; its object
registration, selection and simulation paths remain in place. A
reference-counted `NativeAppParticleBinding` now queries
`IID_IParticleManager` from the live system container and releases
the App global before GameSystem teardown. A missing required particle
aggregate fails the mission-source shell explicitly. This binding
does not call App `InitParticles`, which still needs the real camera,
material and mesh-manager initialization order.

**Validation and next blocker.** GCC 16.2.1 and Clang 22.1.8 built
`frontierx_app_comm_track_owner`, `frontierx_app_object_list_owner`
and `frontierx_native_shell` on Linux x86_64 with optional FFmpeg
audio disabled. The existing `portability.native_shell` process check
first exposed the synthetic mission profile's absent particle entry;
that profile now includes the production `IParticleManager` owner.
The check subsequently passed under both compilers. Both
`portability.app_global_startup` checks passed after building their
targets. Both compiler shells also exited zero against the lawful
external Steam `DATA`, `gendata.db`, `gametypes.db`, the source
`Conquest.ini`, repository `Data/mat`, and `Sol_Train.dmission`, using
SDL dummy video/audio, an isolated save/settings directory, explicit
`--skip-movies` and `--skip-notification-cue`. Each opened 160633
ParseData bytes and 3236 Globals bytes, bound the particle aggregate,
initialized the required `IStreamer` and `MusicManager`, and still
reported `mission not started`. This is external-data startup and
App owner binding, not a mission tick, visible unit or GPU frame.
A temporary GCC link based on the shell's Ninja command with 22
additional compiled App/script objects failed on 32 references across
20 symbols. `FIELDMGR` and `EFFECTPLAYER` each account for four;
their source registrars in `cloud.cpp` and `EffectPlayer.cpp` are the
next production global-owner closure. A temporary direct GCC source
preflight of `cloud.cpp` with native App declarations and corrected
top-level include case reached the nested `anim2d.h` include of
`heapobj.h`; the production field manager was not compiled or linked.
Windows Debug/Final, module
ABI, retail gameplay, movement and full mission save/reload checks
remain unperformed.

## Native mission name conversion and field-owner preflight (2026-10-03)

**Production owner and caller.** The combined App/mission link lacked
`_localAnsiToWide` and `_localWideToAnsi`. The supported Windows
definitions in `DataView.cpp` convert through the active Windows ANSI
code page; `MGlobals.cpp` uses the first function for player names and
the mission/App UI path also calls both. The selected `Sol_T` checkpoint
uses ASCII archive and mission names. These routines do not change a
saved structure or module ABI. `cloud.cpp` owns the still-unresolved
`FIELDMGR` registrar; `Conquest.cpp` invokes global registration before
App particle, mesh and texture initialization.

**Change and limit.** The native App globals now provide bounded ASCII
conversion in both directions. A zero return and empty output reject
non-ASCII input or insufficient byte capacity; the conversion does not
claim to emulate an unspecified Windows `CP_ACP` for other languages.
The Windows definitions are unchanged. The existing data-root check
exercises a `Sol_T` round trip and both rejection cases. A temporary
out-of-tree GCC preflight of `cloud.cpp`, with corrected source include
case and a case-alias include overlay, reached extensive Win32 event,
field-template and object dependency errors. No field-manager source
change was made; a global pointer alone cannot satisfy that owner.

**Validation and next blocker.** Linux x86_64 GCC 16.2.1 and Clang
22.1.8 built `frontierx_data_root_probe`,
`frontierx_app_global_startup_probe` and `frontierx_native_shell` in
`/tmp/frontierx-gcc-goal` and `/tmp/frontierx-clang-goal`, with optional
FFmpeg audio disabled. `portability.data_root` and
`portability.app_global_startup` passed under both compilers. The
conversion check uses synthetic names and byte limits; the earlier
external Steam training-data shell runs did not execute the new
conversion. The latest combined mission link predates this change and
had 32 unresolved references across 20 symbols, including both
conversion functions, `FIELDMGR` and `EFFECTPLAYER`. The next blocker is
the real field/effect global-owner closure and then executing
`Mission::Reload`. No mission tick, normal movement input, visible
Forger, full mission save/reload or GPU mission frame ran. Native
Windows Debug/Final, cross-module ABI and retail runtime checks remain
unperformed.

## Production effect-player compile boundary (2026-10-03)

**Production owner and caller.** The combined mission/ObjectList link
needs the original `EffectPlayer.cpp` registrar and `EFFECTPLAYER`
global: `ObjectList::Render` and `ObjectList::engine_update` use it.
The source `Conquest.cpp` passes the registered `GAMEPOSCALLBACK` to
App particle initialization. These are runtime interfaces; no saved
record or cross-module type was changed.

**Change.** The production effect player now has a native include
boundary with exact filename case and camera declarations before the
legacy engine headers. Its two angular-step minimum calls retain the
Windows expressions and use an equivalent native scalar comparison.
The native App globals define `EFFECTPLAYER` and `GAMEPOSCALLBACK`.
The registrar and effect implementation are compiled as a dedicated
object target. They are not linked into the shell while the full
mission owner closure remains unresolved.

**Validation and next blocker.** GCC 16.2.1 and Clang 22.1.8 built
`frontierx_app_effect_player_owner` and `frontierx_native_shell` on
Linux x86_64 in the fresh Debug build trees with optional FFmpeg
audio disabled. The existing `portability.data_root`,
`portability.app_global_startup` and `portability.native_shell` checks
all executed and passed under both compilers, using synthetic inputs.
The effect player itself was compile-only; neither its registrar nor
its update method ran in those checks. The last combined mission link
was measured before this target joined it; no revised unresolved-count
claim is made. The production `FIELDMGR` owner and remaining globals,
then the actual App `CreateGlobalComponents` and `Mission::Reload`
sequence, are the next integration gates. No visible mission unit,
normal movement, simulation checkpoint or full mission save/reload
executed. Native Windows Debug/Final, module ABI, retail runtime and
mission GPU checks remain unperformed.

Both complete Debug builds and CTest runs also passed after this
change; the full-preset counts and three command-sandbox skips are
recorded in `BUILD_GRAPH.md`. These skips are not GPU runtime evidence.
Both updated compiler shells separately ran on the lawfully external
Steam `DATA`, `gendata.db` and `gametypes.db`, the source `Conquest.ini`
and `Sol_Train.dmission`, using SDL dummy devices, isolated save and
settings directories, explicit `--skip-movies` and
`--skip-notification-cue`, and `--smoke-ms 1 --hidden`. Each exited zero,
mounted ten archives, read 160633 mission ParseData bytes and 3236
Globals bytes, bound the production particle owner, initialized the
required `IStreamer` and printed `mission not started`. These are
external-data startup executions, not effect-player or mission runtime.

## Field and animation owner compile boundary (2026-10-03)

**Blocker, owner and caller.** The combined native App/script link needs
the original `FieldManager` registrar in `cloud.cpp`. `Conquest.cpp`
creates global App components before `Mission::Reload`; ObjectList and
mission field loading then use that owner. The diagnostic link also
needs its minefield, antimatter, blast, audio and rendering dependencies.
The selected `Sol_T` checkpoint still requires the source Forger, normal
movement input, real update ticks and full mission save/reload. Its
acceptance criteria above remain unchanged.

**Changes.** The production field manager, App 2D animation, dust and
deterministic random-generator sources now compile as separate native
objects. Native-only include, dependent-template and scalar conversion
corrections also allow the actual field/object template instantiations;
Windows expressions and data layouts remain unchanged. Editor-only
field methods abort with a diagnostic on Linux. The App 2D animation
owner aborts on a missing required texture and on its unsupported
smooth-render stage-one blend. Native-only pure-virtual declarations
match the existing concrete implementations of three ObjectList debug
methods, two field methods and `IFogOfWar::New`, removing unresolved
interface vtables without supplying false success. The original App
`RandomGen::rand` ran in the existing math-core check with seed 42 and
the fixed six-result sequence 667, 288, 870, 165, 162, 556. These
source and interface changes still require native Windows module/ABI
validation; no disk schema was changed.

**Validation and next blocker.** Linux x86_64 GCC 16.2.1 and Clang
22.1.8 built the complete fresh Debug graphs in
`/tmp/frontierx-gcc-goal` and `/tmp/frontierx-clang-goal` with optional
FFmpeg audio disabled. The existing `portability.math_core` and
`portability.native_shell` checks executed and passed under both
compilers; the other new App owners were compile-only. Each updated
shell separately loaded the lawfully external Steam `DATA`,
`gendata.db`, `gametypes.db`, source `Conquest.ini` and
`Sol_Train.dmission` with SDL dummy devices, isolated save/settings
roots, explicit movie/notification-cue skips and a one-millisecond
hidden smoke. Both initialized the required `IStreamer`, mounted ten
archives, read 160633 mission ParseData bytes and 3236 Globals bytes,
and reported `mission not started`. A temporary GCC combined diagnostic
link including the new owner objects failed on 41 undefined-reference
lines across 16 distinct symbols, led by 20 `SFXMANAGER` references;
the next production closure is sound effects and the remaining
minefield/antimatter/blast, light, UI and drawing owners. The DirectSound
based `SFX.cpp` requires an actual native contract, not a dummy owner.
The field, animation and dust owners have not executed in a mission;
point-list dust presentation also remains unverified. No mission tick,
visible Forger, input movement, mission save/reload or hardware GPU
mission frame ran. Native Windows Debug/Final, cross-module ABI and
retail gameplay checks remain unperformed. `IStreamer` stays required;
only full-screen `IVideoSystem` and the notification cue were explicitly
omitted for the bounded pre-combat smoke.

## External SFX source binding (2026-10-03)

**Blocker, owner and caller.** The source `Menu` owner binds a dedicated
`SFXDIR` to the sound-effect manager. `Sfx::loadData` maps `SFX::ID`
values to WAV names through `sfxdata.xmf`/the generated resource, and
field/object callers open or preload those IDs. The native shell had
bound the external Music source and required `IStreamer`, but not the
external SFX source. A real native `ISFX` owner will need both the ID
mapping and readable effect files before its calls can execute. No
persistent format or module type changed in this step.

**Change and falsifiable check.** The native App audio binding now
retains the source `SFX` filesystem alongside Music and `IStreamer`
for the same lifetime, and the mission-source shell rejects a missing
SFX source. The existing shell process check covers that rejection.
The existing external mission-filesystem check now opens the source
`SFX::BUTTONPRESS1` file (`interface1.wav`) through the UTF-backed
`IFileSystem`, accepting its unique case-insensitive archive name and
requiring a readable `RIFF`/IAC WAVE header. This checks the needed production
data path, not audio decoding or playback.

**Validation and next blocker.** Linux x86_64 GCC 16.2.1 and Clang
22.1.8 each built the complete fresh Debug graph with optional FFmpeg
audio disabled. `portability.native_shell` passed under both compilers
with a synthetic directory fixture, including rejection of absent
`SFX`. Both compiler runs of `frontierx_data_root_probe
--mission-filesystem` passed against the lawfully external Steam
`Sol_Train.dmission` and SFX archive, opening the source-mapped WAV
header with source WAVE tag `0x0402`. Both updated shells then loaded the same external mission
source, initialized required `IStreamer` with Music and SFX sources,
and still printed `mission not started`; SDL dummy devices, isolated
save/settings directories and explicit `--skip-movies` and
`--skip-notification-cue` were used. The default window probes rejected
this IAC effect because their optional FFmpeg adapter was disabled.
The next integration blocker is a
real native `ISFX` implementation with source ID mapping, decoding,
handle ownership, playback and mission-facing spatial/loop behavior;
the DirectSound-only `SFX.cpp` cannot be linked as-is. The combined
mission link last failed on 41 undefined-reference lines across 16
symbols, led by `SFXMANAGER`; this count was not rerun after the source
binding. No `ISFX` playback, mission tick, visible unit, input movement,
mission save/reload or hardware GPU mission frame ran. Native Windows
Debug/Final, module ABI and retail gameplay checks remain unperformed.

**External IAC playback follow-up.** The pinned official FFmpeg 9.0.1
source was fetched again outside the repository, matched the recorded
SHA-256 in `DEPENDENCIES.md`, and built unmodified into replaceable
shared LGPL libraries using the documented restricted configuration.
Both new GCC and Clang Debug CMake configurations passed the library
license check and built `frontierx_native_window_manager_probe` and
`frontierx_native_shell`. Both compilers passed the focused
`portability.native_window_manager` and `portability.native_shell`
checks. With SDL dummy audio, the existing external-input window probe
decoded the lawful `interface1.wav` to 30720 bytes of 22050-Hz mono
float PCM with identical FNV1a64 `16092735175899199743`; GCC and Clang
completed playback after 404 and 408 ms respectively. Their external
mission shell runs still reported `mission not started`. This is real
IAC decode and dummy-device `IStreamer` playback of one source effect,
not `ISFX` manager execution or audible hardware evidence. The
FFmpeg-enabled configurations built only these affected targets and
ran only these focused CTests; no full FFmpeg-enabled CTest matrix,
Windows/ABI check or mission GPU run was performed.

## Native ISFX and field object closure (2026-10-03)

**Blocker, production owners and callers.** `SFX::BUTTONPRESS1` and
other App effects use the original `ISFX` interface, but the source
`SFX.cpp` owns DirectSound and cannot link in the native graph.
`FieldManager::CreateArchetype` also calls the original `Minefield`
and `AntiMatter` factories. The prior combined link had no
`SFXMANAGER`; it could not progress to `Mission::Reload`. The selected
acceptance input remains the lawfully external Steam `Sol_Train.dmission`:
start it, show and select the player Forger, issue one normal movement
command, reach a scripted training state, then save and reload that
state. This entry advances owner closure only. No persistent field,
mission schema or cross-module interface was widened.

**Change and check.** The native-only `ISFX` owner compiles the original
`sfxdata.xmf` ID/name/volume table into the build directory, requires
all 146 nonzero IDs, and opens the already-bound external SFX filesystem
through the required `IStreamer`. It owns preloaded streams and
reference-counted sound handles, applies volume, restart, no-restart
and loop flags, and closes them on teardown. The original Windows
resource generation and DirectSound owner are unchanged. Spatial
playback currently fails explicitly: camera/fog attenuation and source
priority have not been ported. The existing native shell process check
generates a tiny synthetic PCM `interface1.wav` and exercises invalid
ID rejection, preload, open, handle sharing, volume, play, stop, loop
and release through `SFX::BUTTONPRESS1`. It represents the production
`ISFX` caller contract; it is not a gameplay test. The original
`MineField.cpp` and `AntiMatter.cpp` now build as native object targets
with case-correct includes, native part-name conversion and a
Windows-only editor-input branch. The native D3D9 compatibility header
defines the two stage values referenced by anti-matter; its draw path
has not been exercised. No native editor feature is claimed.

**Executed validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8
each built the complete Debug graph in `/tmp/frontierx-{gcc,clang}-goal`
and passed the full CTest graph: 48 executing checks out of 51, with
`portability.app_cursor`, `portability.game_material_gpu` and
`portability.sdl3_gpu` skipped by the command sandbox. The focused
`portability.native_shell` check passed under both compilers in the
separate FFmpeg-enabled Debug configurations. Each FFmpeg-enabled
shell then loaded the lawful external `Sol_Train.dmission`, source
`Conquest.ini`, SFX archive and `BUTTONPRESS1` IAC effect with SDL
dummy audio, isolated settings/save roots, explicit `--skip-movies`,
`--skip-notification-cue` and `--sfx-smoke`. Both printed
`Native ISFX BUTTONPRESS1 preload/open/play/stop checked`, then
`mission not started`. This is external-data native `ISFX` runtime,
not audible hardware playback or a mission tick.
The explicit rerun used `cmake --build /tmp/frontierx-{gcc,clang}-goal`
and `ctest --test-dir /tmp/frontierx-{gcc,clang}-goal --output-on-failure
-j4` for the default Debug matrices. For each FFmpeg-enabled compiler
shell, an existing empty external save directory was supplied with
`--data-root "$FRONTIERX_RETAIL_ROOT/DATA" --material-root Data/mat
--system-profile Code/App/Src/Conquest.ini --gen-data
"$FRONTIERX_RETAIL_ROOT/gendata.db" --game-types
"$FRONTIERX_RETAIL_ROOT/gametypes.db" --save-root "$SAVE_ROOT"
--mission-source Sol_Train.dmission --skip-movies
--skip-notification-cue --sfx-smoke --smoke-ms 1 --hidden`, with
`SDL_VIDEODRIVER=dummy` and `SDL_AUDIODRIVER=dummy`. Two initial manual
invocations used the installation root instead of its `DATA` child,
then an absent save directory; they exited 2 and 6 respectively.
Corrected invocations exited zero under both compilers.

**Next concrete blocker.** A temporary diagnostic GCC link of the
shell with compiled App and `Sol_T` objects, including both new field
objects and existing source resources, fails on 22 references to 12
distinct symbols. The unresolved owners include `CQLight`,
`CreateBlast`, `MAPGEN`, `DBHOTKEY`, `DEBUGFONT`, `OrthoView`,
`DA::RectangleHash/Fill`, `Bcr` from `MorphMesh`, and fleet-list
cleanup from `FlagShip`. First connect the concrete production light
and blast owners required by `FieldManager`; then continue link closure
and invoke `Mission::Reload` with the selected external data. The
diagnostic executable did not link or execute. No native Windows
Debug/Final, module ABI, retail gameplay, hardware GPU mission frame,
visible unit, input movement, simulation checkpoint or full mission
save/reload check was performed.

## Production light, profile and fleet closure (2026-10-03)

**Blocker and falsifiable path.** The selected `Sol_Train.dmission`
needs the original App mission and ObjectList owners linked before
`Mission::Reload` can be called. The previous combined link lacked
`CreateBlast`, `CQLight`, `MAPGEN`, the anti-matter Catmull-Rom basis,
fleet cleanup and a profile loader. `FieldManager::CreateFieldBlast`
and Nebula construction are real Blast/light callers; anti-matter uses
`Bcr`; `ObjectList::Close` calls both fleet-pool cleanup functions.
The cheapest check was native object compilation followed by the same
combined App/script link, without claiming that a compiled object had
executed. No disk, wire or module structure was widened.

**Changes.** Original `blast.cpp`, `light.cpp`, `MapGen.cpp` and
`MorphMesh.cpp` now build as GCC/Clang object targets. Native-only
include spelling, missing template declarations and checked numeric
conversions preserve the Windows source path. Win32 light editor
windows are excluded from the native owner; requesting unsupported
D3D9 dynamic or ambient App light state aborts explicitly. The existing
native App data-source owner now retains the external Profiles UTF as
`PROFILEDIR`. Its native `CreateProfileParser` reads a bounded file
through `IFileSystem`, creates the production `IProfileParser2` and
returns an initialized, reference-counted parser. The original
`FlagShip.cpp` fleet-pool definitions were moved verbatim into
`FleetGroupPool.inc`; Windows still compiles them from `FlagShip.cpp`,
while a native object compiles the same definitions for ObjectList
cleanup. Native debug font calls abort when their optional owner is
absent, and the debug-only highlight hotkey defaults to unpressed if
its table has not been created. These debug branches do not supply
normal game hotkeys. `MAPGEN` is the original owner, not an empty
multiplayer stub.

**Executed validation.** Linux x86_64 GCC 16.2.1 and Clang 22.1.8
built the complete Debug graphs using `cmake --build
/tmp/frontierx-{gcc,clang}-goal -j4`. Both full `ctest --test-dir
/tmp/frontierx-{gcc,clang}-goal --output-on-failure -j4` runs passed
48 executing checks out of 51; the cursor and two GPU tests skipped
in the command sandbox. The existing `portability.native_shell` check
was extended with a generated `Profiles/native-smoke.ini`. Under both
compilers it opened the file through App `PROFILEDIR`, read the
`NativeSmoke/Value = ready` key through production `IProfileParser2`,
and released the parser. The separate FFmpeg-enabled GCC/Clang shell
targets and focused shell CTests passed. Both shells also loaded the
lawfully external Steam `Sol_Train.dmission` with source `Conquest.ini`,
external GenData/GameTypes, required `IStreamer`, SDL dummy audio,
external SFX, isolated settings/save roots and explicit `--skip-movies`
and `--skip-notification-cue`; `--sfx-smoke` completed. They printed
`mission not started`. No external profile was parsed by the new
wrapper, and no original Blast/light/MapGen/MorphMesh/fleet path ran
in those shell processes.

**Current link and renderer boundary.** The temporary GCC diagnostic
link includes the compiled App/script owners and source resources,
plus original fleet cleanup. It now fails on only three distinct
symbols: `DA::RectangleFill`, `DA::RectangleHash` and `OrthoView`.
The original `DrawAgent.cpp` owns the rectangles, and `Camera.cpp`
owns `OrthoView`; both complete sources were preflighted and remain
outside the native build. DrawAgent needs legacy VFX, image-reader and
surface contracts. Camera needs Win32/editor input and the normal
hotkey/window route. The current native primitive returns
`GR_NOT_IMPLEMENTED` for ortho and viewport, and App light rendering
is likewise unsupported. The next integration step is a genuine
2D/camera rendering and input path through those owners and the
existing native renderer, followed by the first `Mission::Reload`
execution. The diagnostic link did not produce an executable. No
mission tick, visible Forger, normal move order, `SelectAll` checkpoint,
mission save/reload, native Windows Debug/Final, cross-module ABI,
retail gameplay or hardware GPU mission frame ran.

## Native orthographic and viewport contract (2026-10-03)

**Blocker and hypothesis.** The combined production App/script link
still lacks `OrthoView` and `DA::RectangleFill/Hash`. Their real callers
use pane-relative orthographic output. The native `IRenderPrimitive`
previously returned `GR_NOT_IMPLEMENTED` for both ortho and viewport;
therefore linking those App owners alone could not make their draw
contract work. The cheapest falsifiable check was to extend the
existing render-submission probe with the source-style 640-by-480
orthographic call, viewport round trip and invalid-input rejection.
The added callback descriptor is native-only and neither serialized
nor part of the Windows game/module ABI.

**Change and validation.** The native primitive constructs a checked
orthographic `Matrix4`, rejects nonfinite output, and forwards it through the already used
projection callback. A native-only viewport callback stores accepted
coordinates; SDL GPU applies both viewport and scissor to each
presentation/readback pass, rejecting a pane outside the target
instead of reporting a successful draw. The shell and synthetic game
scene bind their respective GPU contexts to this callback. Linux
x86_64 GCC 16.2.1 and Clang 22.1.8 built their complete Debug graphs
with `cmake --build /tmp/frontierx-{gcc,clang}-goal -j4` and executed
`ctest --test-dir /tmp/frontierx-{gcc,clang}-goal --output-on-failure -j4`:
each ran 48 passing checks out of 51. The existing
`portability.render_submission` check observed the expected X/Y scale
and offsets, the accepted viewport and unchanged state after invalid
extents. The cursor and two GPU checks skipped in the command sandbox.
Separately, `ctest --test-dir /tmp/frontierx-{gcc,clang}-goal -R
portability.sdl3_gpu --output-on-failure -V` ran on the visible
Wayland/Vulkan host under each compiler. The extended existing probe
read back a 16-by-16 clipped viewport with fewer changed pixels than
the full 64-by-64 target, then restored the original checksum
`7951318278861886233` after resetting the viewport; both runs also
presented 12 frames. For the reduced 64-pixel diagnostic target, a
full-window viewport at origin maps to the whole offscreen target;
production presentation retains the actual window viewport. Inputs
were repository-generated fixtures; no retail or external data ran in
the modified path. A fresh temporary combined GCC App/script link
still reports exactly the three symbols above. No mission executable,
`Mission::Reload`, visible Forger, input order, simulation checkpoint,
mission save/reload, native Windows Debug/Final, cross-module ABI or
hardware GPU mission frame ran. The next concrete blocker is to bring
the original `Camera`/`DrawAgent` 2D owners and their real callers into
the native closure, then execute `Mission::Reload` with the selected
external data.

## Native App 2D and combined mission link (2026-10-03)

**Named blocker and owner.** The selected production `Nugget::DrawHighlighted`
caller references `DA::RectangleHash/Fill`, and `ObjectList::Render` calls
`OrthoView`. The complete original `DrawAgent.cpp` currently needs VFX
framebuffer/image/surface contracts; direct native preflight reported 146
compile errors after case-only includes were supplied. The complete
`Camera.cpp` needs Win32 editor/menu/input code; excluding its
`BaseHotRect` include in an out-of-tree preflight still left 41 errors.
The already integrated `IRenderPrimitive` ortho/viewport backend made a
native App 2D path falsifiable. The smallest check that could disprove it
was to execute the source-style rectangle calls with the existing App
PrimitiveBuilder and render callback, then retry the combined link.
No serialized type or Windows module interface was changed.

**Change.** `NativeApp2DDraw.cpp` supplies the three missing native App
symbols. It applies the same half-pixel modelview, full-screen/pane
orthographic bounds and viewport, cull disable, toolbar guard, fill/hash
colors, hash alpha blend and six-vertex quad submission used by the
original non-frame-lock source path. It checks return values from the
render boundary and aborts on unavailable framebuffer locking instead
of claiming support. The existing native App PrimitiveBuilder object is
linked into the shell. On Linux only, `cloud.cpp` explicitly instantiates
`DefaultArchetype<BT_MINEFIELD_DATA>` after the method definitions;
Clang had otherwise left its constructor unresolved in the combined
link. The supported Windows DrawAgent/Camera functions and template
placement remain intact.

**Executed checks.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 built
`frontierx_native_app_2d_owner`, `frontierx_native_shell`,
`frontierx_game_material_manager_probe` and the complete Debug graphs
in `/tmp/frontierx-{gcc,clang}-goal`. The existing
`portability.game_material_manager` check executed both native App
rectangle calls: each submitted six vertices, applied the expected
64-by-64 viewport and X/Y orthographic coefficients, and left stage-zero
texture unbound. No new standalone probe was added. Each full CTest
matrix passed 48 executing checks out of 51; cursor and two GPU tests
skipped in the command sandbox. Separate visible Wayland/Vulkan
`portability.game_material_gpu` runs under both compilers passed and
reported the existing synthetic CQ2 App draw checksum
`17301943790166719418`; those GPU draws were not the new rectangles
or a mission unit. The temporary out-of-tree
`/tmp/frontierx-mission-link-diagnostic.py` linked the selected App
objects, original `Sol_T` script and native resources without unresolved
symbols under both compilers. The resulting diagnostic executables
then ran the existing shell path with the lawful external training
installation, supplied `Conquest.ini`, GenData, GameTypes, an isolated
save directory and preferences file, SDL dummy video/audio, explicit
`--skip-movies` and `--skip-notification-cue`, and
`--mission-source Sol_Train.dmission`. Both exited zero, initialized
required `IStreamer`, parsed 160633 bytes of mission schema and the
3236-byte Globals record, and printed `mission not started`. Initial
GCC diagnostic invocations exited 5 due to an unwritable default
preferences location and then a missing explicit preferences file;
the corrected isolated preferences file completed cleanly.

**Next blocker.** The successful combined link is a diagnostic binary,
not an integrated game executable. It still does not call the App
`CreateGlobalComponents` sequence or `Mission::ReloadMission`. The
source camera registrar in `Camera.cpp` is absent from the native
closure; its ordinary event/hotkey path and other App service globals
must be bound before mission startup. Then enter the production mission
load, classify any missing unit against the Windows CQ2/retail
restoration baseline, and continue to input, simulation and save/reload.
No native Windows Debug/Final, module ABI, visible mission GPU frame,
mission tick, player Forger, normal movement order, script checkpoint or
mission persistence check ran.

## Production camera and hotkey boundary (2026-10-03)

**Blocker and hypothesis.** The selected mission's App global startup
creates `MAINCAM` through the original `Camera.cpp` registrar. The earlier
combined link omitted that owner. A direct native compile exposed Windows
menu/debug-viewer dependencies, `FULLSCREEN` event subscription, mismatched
include case and pointer-width event casts. The cheapest falsifiable check
was a GCC/Clang camera object build followed by a combined App/`Sol_T` link
including that object. The camera state is a runtime owner; this change
does not alter its saved `CAMERA_DATA` record or a Windows module vtable.

**Change.** `Camera.cpp` uses exact-case includes and native App globals on
Linux. It retains camera transforms, frustum, zoom and source event logic;
the Windows-only menu, GDI data viewer and Win32 message cases stay on
Windows. The native callback subscribes to the existing EventSystem,
checks that U32-encoded hotkey/update payloads fit 32 bits, rejects active
updates without the required normal hotkey service, and treats absent
debug hotkeys as unpressed. The editor-only viewer remains optional. The
existing native 2D owner supplies `OrthoView`; the original Windows
definition is unchanged. `IBaseCamera::SphereInFrustrum` is pure on Linux
because its interface declaration has no base implementation; the
production `Camera` override is present, and the Windows declaration is
unchanged. `BASECAM.H` includes the exact on-disk header names. A new
object target compiles the original camera source. The GPU shell now links
the selected mission, operation, object, packet and `Sol_T` owners along
with the camera without yet invoking their registrars.

The native shell's explicitly supplied source hotkey table now emits
`CQE_HOTKEY`, matching the App's Windows `HKManager` event, and binds its
live service to the native App `HOTKEY` global. It clears that pointer
before releasing the service. This exposes the real hotkey contract for
camera and later gameplay callbacks; it does not yet drive them.

**Validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 built
`frontierx_app_camera_owner`, `frontierx_native_shell` and their complete
Debug graphs in `/tmp/frontierx-{gcc,clang}-goal`. Each full CTest run
passed 48 executing checks out of 51; the cursor and two GPU checks
skipped in the command sandbox. The focused hotkey, native-shell and
App-startup checks passed under both compilers. The temporary
`/tmp/frontierx-mission-camera-link.py` combined all selected App objects,
the original `Sol_T` script and the new camera object with zero unresolved
references under both compilers. Both resulting executables then ran the
existing shell path with lawfully external Steam training data, supplied
`Conquest.ini`, source `Hotkeys.utf`, explicit `--skip-movies` and
`--skip-notification-cue`, isolated save/preferences paths, and SDL dummy
video/audio. Each exited zero, initialized required `IStreamer`, reported
160633 mission ParseData bytes and 3236 Globals bytes, then reported
`mission not started`. An initial invocation pointed `--data-root` at the
installation parent and exited 2 for missing required sources; the
corrected `DATA` root passed. The rebuilt real GPU shell, now linked with
the camera and selected mission/script owner closure, repeated that
external-data startup and clean exit
under both compilers. No retail payload was copied into the repo.

**Remaining gate.** This is compilation and shell execution, not camera
execution or gameplay. The shell still does not call
`CreateGlobalComponents` or `Mission::ReloadMission`. The normal shell
path has an active GameSystem Engine but no mission GPU pipeline,
primitive, renderer, texture/material bindings or original camera lifetime;
the temporary GPU composition in `NativeGameScene.cpp` serves only its
synthetic fixture. These App globals must be composed before entering
the source registrar, with normal hotkeys required for mission startup.
The original source `FULLSCREEN` callback priority
has no matching priority API in the native EventSystem. No camera frame,
visible Forger, movement input, simulation checkpoint or mission
save/reload was observed. Native Windows Debug/Final, module ABI, GPU
mission and retail-runtime checks remain unperformed. The Windows mission
that starts without units is still not an acceptable gameplay oracle.

## Native mission Engine composition (2026-10-03)

**Blocker and hypothesis.** The GPU shell owned a started `GameSystem` and
`IEngine`, but its mission-source path did not call
`IEngine::load_engine_components` or expose an `IRenderer` and source-profile
render interfaces to App code.
The original `Conquest.ini` declares `IRenderer`, `IAnimation` and
`IHardpoint` in `[Engine]`. A bounded run with that supplied profile
could disprove whether the production Engine aggregates load before
mission startup. This change concerns runtime interface ownership; it
does not alter serialized records or Windows module interfaces.

**Change.** The native GameSystem bridge now loads the active Engine's
profile components and queries `IRenderer`. The shell exposes this as
explicit `--mission-engine`, requiring both `--mission-source` and
`--system-profile`. It temporarily supplies the already parsed source
profile to DACOM for the Engine load, then clears the parser binding.
On success it binds `ENGINE`, `REND`, `PIPE` and `BATCH` for the shell
lifetime and clears them before the GameSystem and interface references
are released. `PIPE` and `BATCH` are queried from the source-profile
SystemContainer and are explicitly reported as not GPU-initialized.
Missing Engine components, renderer or render interfaces fail. The ordinary
`--mission-source` data diagnostic does not claim to compose an Engine.

**Validation.** GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 rebuilt
`frontierx_native_shell` and the complete Debug graphs in
`/tmp/frontierx-{gcc,clang}-goal`. Each full CTest run passed 48
executing checks out of 51; the cursor and two GPU checks skipped in
the command sandbox. The existing native-shell check now runs the
new option against a synthetic `[Engine]` section without `IRenderer`
and observes exit 6 with the explicit missing-Engine/renderer error.
Both real compiler shells ran with lawfully external Steam training
data, supplied `Conquest.ini`, source `Hotkeys.utf`, isolated
save/preferences paths, SDL dummy video/audio, and explicit
`--skip-movies` and `--skip-notification-cue`. Each reported
`Native App mission Engine and render interfaces bound; GPU not initialized`, the
required `IStreamer` initialized with Music/SFX, 160633 mission
ParseData bytes, 3236 Globals bytes, and `mission not started`; both
exited zero. The first focused CTest runs failed because they used a
reduced synthetic profile with no renderer. Making Engine composition
an explicit option preserved that data-diagnostic contract, and the
final focused and full checks passed. Retail files remained outside
the repository.

Commands executed (external installation locations are represented
by shell variables here):

```text
cmake --build /tmp/frontierx-gcc-goal --target frontierx_native_shell -j4
ctest --test-dir /tmp/frontierx-gcc-goal -R '^portability.native_shell$' --output-on-failure
cmake --build /tmp/frontierx-clang-goal --target frontierx_native_shell -j4
ctest --test-dir /tmp/frontierx-clang-goal -R '^portability.native_shell$' --output-on-failure
cmake --build /tmp/frontierx-gcc-goal -j4
ctest --test-dir /tmp/frontierx-gcc-goal --output-on-failure -j4
cmake --build /tmp/frontierx-clang-goal -j4
ctest --test-dir /tmp/frontierx-clang-goal --output-on-failure -j4
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-goal-diagnostic-config timeout -k 5s 90s /tmp/frontierx-gcc-goal/frontierx_native_shell --data-root "$LAWFUL_DATA_ROOT" --material-root Data/mat --system-profile Code/App/Src/Conquest.ini --skip-movies --skip-notification-cue --mission-source Sol_Train.dmission --mission-engine --gen-data "$LAWFUL_INSTALL_ROOT/gendata.db" --game-types "$LAWFUL_INSTALL_ROOT/gametypes.db" --hotkeys Code/App/Src/Hotkeys.utf --save-root /tmp/frontierx-goal-diagnostic-save --preferences /tmp/frontierx-goal-diagnostic-preferences.bin --smoke-ms 1 --hidden
```

The Clang external-data invocation substituted
`/tmp/frontierx-clang-goal/frontierx_native_shell` for the GCC binary
with the same options and external inputs. Logs remained under `/tmp`.

**Next gate.** The Engine, renderer and profile render interfaces now
exist in the real mission shell, but `PIPE` and `BATCH` have no GPU
backend, and texture/material managers and the source camera are not
assembled into a persistent mission context.
`NativeGameScene.cpp` assembles these only for its synthetic fixture.
Connect the existing SDL renderer and source-profile aggregates to
the mission shell, then call `CreateGlobalComponents` and the real
mission load. No mission camera callback, visible Forger, normal move
input, simulation checkpoint, or save/reload executed. Native Windows
Debug/Final, module ABI, retail gameplay and GPU mission runtime checks
remain unperformed.
