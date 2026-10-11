# Windows build declarations and next portable slice

Native source Teletype graph, 2026-10-10: original Teletype and the optional
native FreeType adapter join the existing mission-media object owner; the
existing shell process test accepts an explicitly external font fixture.
Linux x86_64 Debug Ninja GCC 16.2.1 and Clang 22.1.8 complete default trees
`/tmp/frontierx-mission-{gcc,clang}` and optional FFmpeg+font trees
`/tmp/frontierx-voice-{gcc,clang}` built with `cmake --build <tree> -j6`.
Each `ctest --test-dir <tree> -E '^portability.sdl3_gpu$'
--output-on-failure -j4` passed all 50 executing CPU/process checks.
The GPU resize test was deliberately excluded from these reruns because its
previous desktop-tiling failure is unchanged; there is no new CTest GPU pass.
Separate visible Wayland/Vulkan missions under both compilers passed source
teletype, normal selection/movement, unit voices and source save/reload.
A focused Clang ASan/UBSan shell CTest passed, including source text lifecycle
and font-release validation with leak checking disabled. The system FreeType
binary and full mission were not sanitizer-instrumented/executed respectively.
Windows Debug/Final, module ABI, Windows font/image comparison and full retail
parity remain unperformed. See [the focused text record](NATIVE_SINGLEPLAYER_SLICE.md#original-in-window-mission-teletype-checkpoint-2026-10-10).
Older records below are historical.

Native unit voice graph, 2026-10-10: original UnitComm joins the existing
native mission-media owner; original StringData now has a native object
owner linked into the shell and existing data-root probe. Complete SDL-only
Linux x86_64 Debug GCC 16.2.1 and Clang 22.1.8 builds succeeded with
`cmake --build /tmp/frontierx-mission-<compiler> -j8`.
`SDL_VIDEODRIVER=x11 ctest --test-dir <tree> --output-on-failure -j4`
passed 50/51 per compiler. Both GPU tests failed to observe the requested
resize under desktop tiling; this is a recorded failure, not a skip or pass.
All other checks executed and passed. The unchanged renderer was separately
exercised in visible Wayland/Vulkan external-data mission runs under both
compilers, with successful unit voices, normal movement and save/reload.
Separate optional external shared-LGPL FFmpeg trees built shell, data-root,
window-manager, pipeline and GPU targets and passed all four focused
`data_root|native_shell|render_pipeline|native_window_manager` CTests per
compiler. A separate Clang ASan/UBSan data-root check passed with leak
checking disabled. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-unit-voice-mission-checkpoint-2026-10-10)
for commands, provenance and unperformed Windows/ABI/full mission checks.
Older records below are historical.

Native source-modal integration, 2026-10-10: the original `Modal.cpp` is now
an object in the Linux x86_64 Debug `frontierx_native_shell` graph. Focused
`portability.render_pipeline`, `portability.render_submission` and
`portability.native_shell` CTests passed 3/3 per compiler before the final
shell builds. `cmake --build /tmp/frontierx-mission-{gcc,clang} --target
frontierx_native_shell -j4` succeeded after integration. With
`SDL_VIDEODRIVER=x11`, the full GCC CTest run passed 51/51 including GPU;
the Clang run passed 50/51 because the tiled GPU probe's resize was not
observed (1854x2085 to 1854x2085). A separate direct visible floating
Clang/Vulkan GPU probe with `--require-runtime --visible` passed 12 frames,
resize 639x360 to 800x450, depth recreation and two scene runs. This direct
pass does not relabel the failed Clang CTest as passing. Separate 15-second
visible Wayland/Vulkan retail-input mission processes under both compilers
passed selection, movement, objectives, moved-state save/reload and source
mission close. Windows Debug/Final, ABI, sanitizer and Windows retail-runtime
checks were not run. See [the focused modal record](NATIVE_SINGLEPLAYER_SLICE.md#source-modal-mission-integration-2026-10-10).
Older records below are historical.

Native frame lifecycle validation, 2026-10-10: complete existing Linux x86_64
Debug Ninja trees `/tmp/frontierx-mission-gcc` (GCC 16.2.1) and
`/tmp/frontierx-mission-clang` (Clang 22.1.8) rebuilt with
`cmake --build <tree> -j 12`. Each unmodified-environment full
`ctest --test-dir <tree> --output-on-failure` run passed 50/51 checks;
`portability.sdl3_gpu` failed because the Wayland desktop did not honor its
resize request. Separate floating X11/XWayland Vulkan GPU runs passed all
probe assertions under both compilers, including the new frame lifecycle,
clear values, depth, resize and repeated initialization. This is an explicit
host-dependent runtime result, not a clean default full-suite result or a
skip counted as execution. The registered `portability.sdl3_gpu` CTest then
passed 1/1 per compiler when rerun with `SDL_VIDEODRIVER=x11` in a floating
test window. The separate visible Wayland external mission
runs preserved the playable checkpoint through the production frame boundary.
See [the focused evidence](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-frame-lifecycle-checkpoint-2026-10-10).
Earlier records below are historical.

Current playable native build, 2026-10-10: Linux x86_64 Debug Ninja
`frontierx_native_shell` rebuilt and linked under GCC 16.2.1 and Clang
22.1.8. Four focused data-root/render-pipeline/render-submission/native-shell
CTests passed per compiler on generated/repository inputs; the complete
native CTest trees then passed 51/51 per compiler, including visible GPU
tests, after updating a stale exact-case assertion for the already integrated
Windows-style UTF lookup. Separate visible
Wayland/Vulkan integrated shell processes on lawful external `Sol_Train`
data took normal mouse selection and move input, completed objectives 1 and
2 after 299/298 source updates, and saved/reloaded the moved player Forger
with exact owner, position and flags; each exited zero. The explicit
`frontierx_sdl3_gpu_probe --require-runtime --visible` also executed under
both compilers, rather than skipping. Full presets, sanitizers, native
Windows Debug/Final, ABI and retail parity remain open. See
[the focused acceptance record](NATIVE_SINGLEPLAYER_SLICE.md#playable-solarian-training-checkpoint-2026-10-10).
Entries below are historical snapshots.

Current native mission frame build, 2026-10-10: Linux x86_64 Debug Ninja
`frontierx_native_shell`, `frontierx_sdl3_gpu_probe` and
`frontierx_render_submission_probe` built under GCC 16.2.1 and Clang
22.1.8. Four focused data-root/render-pipeline/render-submission/native-shell
CTests passed per compiler. Separate explicit visible Vulkan GPU probes
passed under both compilers, and separate integrated shell executions on
lawfully external `Sol_Train` data presented 28 production draws after one
source update. Each 64x64 readback changed 171 pixels. Both compiler
processes also executed source mission save/reload and preserved the static
player Forger owner and position in an external native UTF save. This is
native runtime evidence for a first mission frame and static-state round
trip, not a playable mission. Full
presets, sanitizers, native Windows Debug/Final and ABI checks remain open.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-perspective-and-multi-draw-mission-frame-2026-10-10).
Entries below are historical snapshots.

Current native mission build, 2026-10-10: Linux x86_64 Debug Ninja
`frontierx_native_shell` and `frontierx_data_root_probe` built under GCC
16.2.1 and Clang 22.1.8 with original `Planetoid.cpp`. Each compiler passed
the three focused data-root/render-pipeline/shell CTests on generated and
repository inputs. Separate visible Wayland/Vulkan runs on lawful external
`Sol_Train` data loaded the mission and executed one source update, then
failed before a GPU scene because the Forger was excluded by the source
projected-bound visibility check. Full presets, sanitizers, native Windows
Debug/Final and ABI checks remain open. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-planet-owner-game-active-update-and-first-object-render-blocker-2026-10-10).
Entries below are historical snapshots.

Current mission link, 2026-10-10: GCC 16.2.1 and Clang 22.1.8 Linux
x86_64 Debug Ninja integrated `frontierx_native_shell` targets build with
the original Forger, Jumpgate, Waypoint, Trigger, Gunboat and build-support
Platform objects. Separate visible external `Sol_Train` runs stop at the
required planet-placement assertion in the source mission loader (exit 134),
before simulation or draw. Three focused data-root/render-pipeline/shell
CTests passed per compiler after the platform link. Full presets, native Windows
Debug/Final, ABI and sanitizers remain open. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-mission-object-chain-through-platform-placement-2026-10-10).
Entries below are historical snapshots.

Current integrated build, 2026-10-10: Linux x86_64 GCC 16.2.1 and Clang
22.1.8 Debug Ninja both link the original Forger, spaceship/movement and
Banker object owners into `frontierx_native_shell`. Both build the updated
native UTF index and `frontierx_data_root_probe`; three focused
data-root/render-pipeline/shell CTests pass per compiler. Separate visible
Wayland/Vulkan processes on lawful external `Sol_Train` data load one Forger,
then abort at the missing jumpgate part during `CQE_GAME_ACTIVE` (exit 134).
No simulation tick or playable mission ran. Full presets, Windows
Debug/Final, module ABI and sanitizers were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-forger-instance-and-next-script-blocker-2026-10-10).
Entries below are historical snapshots.

Current Forger owner build, 2026-10-10: Linux x86_64 GCC 16.2.1 and Clang
22.1.8 Debug Ninja each built `frontierx_app_fabricator_owner` and the
existing `frontierx_native_shell`. Three focused data-root/render-pipeline/
shell CTests passed per compiler. A trial integrated factory link failed on
production ship/movement/service symbols and is not included in the shell.
No external-data or GPU runtime occurred in this step. Full presets,
Windows Debug/Final, ABI and sanitizers were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#forger-object-compile-and-first-production-link-inventory-2026-10-10).
Entries below are historical snapshots.

Current Forger source gate, 2026-10-10: the original `Fabricator.cpp` is in
the supported Windows `Conquest.vcxproj` but remains outside the Linux link.
Direct GCC 16.2.1 and Clang 22.1.8 syntax attempts after non-Windows
template preparation still fail; they are compile failures, not factory
runtime evidence. Existing Linux x86_64 Debug Ninja `frontierx_native_shell`
targets built under both compilers, and focused data-root/render-pipeline/
shell CTests passed 3/3 per compiler. Full presets, native Windows builds,
ABI and sanitizers were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#forger-factory-source-closure-remains-open-2026-10-10).
Entries below are historical snapshots.

Current mission-link result, 2026-10-10: GCC 16.2.1 and Clang 22.1.8
Linux x86_64 Debug Ninja `frontierx_native_shell` targets built with native
mission media and system-map owners. Focused `portability.data_root`,
`portability.render_pipeline` and `portability.native_shell` CTests passed
3/3 under each compiler. Separate visible Wayland/Vulkan processes on lawful
external training data each reached 100% original mission loading, then the
shell exited 6 at its missing-Forger postcondition; missing object factories
remain. Direct original `Fabricator.cpp` GCC/Clang syntax attempts failed
and it is unlinked. Full presets, Windows Debug/Final, ABI and sanitizers
were not run. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-sol_train-load-completion-before-playable-objects-2026-10-10).
Entries below are historical snapshots.

Current mission-link result, 2026-10-10: Linux x86_64 GCC 16.2.1 and
Clang 22.1.8 Debug Ninja targets both built `frontierx_native_shell` with
original App vertex-buffer and scripting objects. Focused
`portability.data_root`, `portability.render_pipeline` and
`portability.native_shell` CTests passed 3/3 per compiler. Separate visible
Wayland/Vulkan runs on lawful external training data reached 88% mission load
and aborted at missing SoundManager/Teletype initialization; they are runtime
evidence only to that boundary. Full presets, Windows Debug/Final, ABI and
sanitizers were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#external-sol_train-load-reaches-the-script-boundary-2026-10-10).
Entries below are historical snapshots.

Current mission-link build, 2026-10-10: Linux x86_64 GCC 16.2.1 and
Clang 22.1.8 Debug Ninja trees each built `frontierx_native_shell` with
the original Sector and SpaceEnvironment objects plus the mission-load
bridge. Focused `portability.data_root` and `portability.native_shell`
CTest checks passed 2/2 per compiler. Separate visible external-data
Wayland/Vulkan runs entered Mission::Load but aborted at 25% ObjectList
loading on the Anim2D texture assertion. Full presets, Windows Debug/Final
and module ABI checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-sol_train-load-through-objectlist-2026-10-10).
Entries below are historical snapshots.

Current Sector dependency build, 2026-10-10: Linux x86_64 GCC 16.2.1
and Clang 22.1.8 Debug Ninja trees each built the new
`frontierx_app_terrain_map_owner` object and linked it into
`frontierx_native_shell`. The focused `portability.native_shell` CTest
passed once per compiler. Separate visible external-data mission-start runs
still stopped at the missing `Sector` owner; no terrain-map method executed.
Windows Debug/Final and ABI
checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-terrainmap-compile-boundary-for-sector-2026-10-10).
Entries below are historical snapshots.

Current mission Engine binding Debug matrix, 2026-10-03: complete Linux
x86_64 GCC 16.2.1 and Clang 22.1.8 Debug graphs built after adding the
GameSystem Engine-component bridge and explicit shell mode. Each full
CTest run passed 48 executing checks out of 51; the cursor and two GPU
checks skipped in the command sandbox. Both real shells executed
`--mission-engine` with a supplied source profile and lawfully external
training data, bound `IRenderer` and the uninitialized source-profile
render interfaces, initialized required `IStreamer` and
closed cleanly before game startup. No mission or GPU frame ran. Windows
Debug/Final and module ABI checks remain unperformed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-engine-composition-2026-10-03).
Entries below are historical snapshots.

Current camera-owner Debug matrix, 2026-10-03: complete Linux x86_64
GCC 16.2.1 and Clang 22.1.8 graphs built with the original App camera
object and selected mission, simulation, packet and `Sol_T` owners linked
into the GPU shell. Each full CTest run passed 48 executing checks out of
51; the cursor and two GPU tests skipped in the command sandbox. The temporary
combined App/`Sol_T` link including the camera object succeeded under
both compilers, and both external-data processes executed the existing
shell and hotkey startup paths. The camera registrar and mission did not
execute. Windows Debug/Final, module ABI and mission GPU checks remain
unperformed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-camera-and-hotkey-boundary-2026-10-03).
Entries below are historical snapshots.

Current combined App link Debug matrix, 2026-10-03: complete Linux
x86_64 GCC 16.2.1 and Clang 22.1.8 Debug graphs built. Each full
CTest run passed 48 executing checks out of 51; one cursor and two
GPU tests skipped in the command sandbox. Separate visible
Wayland/Vulkan `portability.game_material_gpu` runs passed under both
compilers. Temporary combined App/script links succeeded under both
compilers and ran the existing external-data shell path, but no App
global startup or mission ran. Native Windows Debug/Final and module
ABI checks remain unperformed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-2d-and-combined-mission-link-2026-10-03).
Entries below are historical snapshots.

Current native 2D contract Debug matrix, 2026-10-03: Linux x86_64 GCC
16.2.1 and Clang 22.1.8 each built the complete
`/tmp/frontierx-{gcc,clang}-goal` Debug graph after the native
ortho/viewport callback change. Each full CTest run passed 48 executing
checks out of 51; one cursor and two GPU tests skipped in the command
sandbox. The focused GPU probe then ran visibly on Wayland/Vulkan
under both compilers and verified viewport clipping and restoration.
The combined mission link remains unresolved on three
Camera/DrawAgent symbols. Native Windows Debug/Final, module ABI,
mission GPU and mission runtime were not checked. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-orthographic-and-viewport-contract-2026-10-03).
Entries below are historical snapshots.

Current mission-owner Debug matrix, 2026-10-03: Linux x86_64 GCC
16.2.1 and Clang 22.1.8 each built the complete
`/tmp/frontierx-{gcc,clang}-goal` Debug graph with the original Blast,
App light, MapGen and MorphMesh objects, native profile parser binding
and original fleet-pool code. Each full CTest run passed 48 executing
checks out of 51; the cursor and two GPU tests skipped in the command
sandbox. Separate FFmpeg-enabled GCC/Clang shell targets and their
focused CTests passed, and both lawful external-data shell processes
exercised `ISFX` but still reported `mission not started`. The
combined App/script link still fails on three 2D/camera symbols.
Native Windows Debug/Final, module ABI, hardware GPU and mission
runtime were not checked. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-light-profile-and-fleet-closure-2026-10-03).
Entries below are historical snapshots.

Current native SFX/field Debug matrix, 2026-10-03: Linux x86_64 GCC
16.2.1 and Clang 22.1.8 each built the complete `/tmp/frontierx-*-goal`
Debug graph, including native `ISFX`, `MineField` and `AntiMatter` objects.
Each full CTest run passed 48 executing checks out of 51; the cursor
and two GPU checks skipped in the command sandbox and provide no GPU
runtime evidence. The separate FFmpeg-enabled GCC/Clang shell targets
and focused shell CTests passed, and each lawful external training-data
shell exercised `BUTTONPRESS1` through `ISFX` on SDL dummy audio.
No full FFmpeg-enabled CTest matrix, native Windows Debug/Final, module
ABI, hardware GPU or mission runtime check ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-isfx-and-field-object-closure-2026-10-03).
Entries below are historical snapshots.

Current SFX source Debug matrix, 2026-10-03: Linux x86_64 GCC 16.2.1
and Clang 22.1.8 each built the complete fresh Debug graph after the
native App SFX filesystem binding. Each full CTest run passed 48
executing checks out of 51; the cursor and two GPU checks skipped in
the command sandbox. Both compiler shells loaded lawfully external
training data and retained Music/SFX sources with required
`IStreamer`, but reported `mission not started`. Both compiler
mission-filesystem probes opened the external source-mapped IAC WAV
header. Separate FFmpeg-enabled GCC/Clang Debug builds of the native
window probe and shell passed their focused two-CTest sets; each
external-input probe decoded and completed the same effect on SDL
dummy audio with matching PCM fingerprints. No full FFmpeg-enabled
CTest matrix or integrated `ISFX` playback ran. Native Windows
Debug/Final, module ABI, retail gameplay
and hardware GPU checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#external-sfx-source-binding-2026-10-03).
Entries below are historical snapshots.

Current native Debug matrix, 2026-10-03: Linux x86_64 GCC 16.2.1
and Clang 22.1.8 each built the complete fresh Debug graph after adding
the production field, App animation, dust and random-generator object
targets. Each full CTest run passed 48 executing checks out of 51;
`portability.app_cursor`, `portability.game_material_gpu` and
`portability.sdl3_gpu` skipped in the command sandbox and provide no
display/GPU runtime evidence. Both compiler shells initialized required
`IStreamer` and parsed the lawfully external training mission, then
reported `mission not started`. Native Windows Debug/Final, module ABI,
retail gameplay and hardware GPU checks were not run. See
[the focused mission record](NATIVE_SINGLEPLAYER_SLICE.md#field-and-animation-owner-compile-boundary-2026-10-03).
Entries below are historical snapshots.

Current native Debug matrix, 2026-10-03: Linux x86_64 GCC 16.2.1
and Clang 22.1.8 each built the complete fresh `/tmp/frontierx-*-goal`
graph after adding the production effect-player compile target and
native App name conversion. Each full CTest run passed 48 executing
checks; `portability.app_cursor`, `portability.game_material_gpu` and
`portability.sdl3_gpu` skipped in the command sandbox. The effect
player was compiled but not part of an executed mission. Native
Windows Debug/Final, module ABI, retail runtime and hardware GPU
checks were not run. See [the focused mission record](NATIVE_SINGLEPLAYER_SLICE.md#production-effect-player-compile-boundary-2026-10-03).
Entries below are historical snapshots.

Latest UTF-save-format matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 configured and built the complete Linux Debug presets with
`NativeUtfWriter.cpp`. Both full CTest presets passed 48 executing
checks and skipped the sandbox cursor/two GPU checks. The existing
`portability.data_root` check encoded and reopened a generated
save-shaped UTF archive through the platform `IFileSystem` reader;
both compiler probes separately read the lawful external
`Sol_Train.dmission` without modifying it. A temporary combined
App/script link still fails on 70 references across 28 symbols.
Native Windows Debug/Final, module ABI and hardware GPU checks were
not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#native-utf-save-format-encoder-gate-2026-09-27).

Latest mission-message matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built the complete Linux Debug presets with native scrolling
text. Each full CTest preset passed 48 executing checks and skipped
the sandbox cursor/two GPU checks. The existing shell CTest passed, and
both compiler shells ran against lawful external `Sol_Train.dmission`
with explicit movie/notification-cue skips and SDL dummy devices.
They displayed the source `Disabled` message, exited zero and still
reported "mission not started". A temporary GCC App/script link
failed on 70 references across 28 symbols. Native Windows Debug/Final,
module ABI and hardware GPU validation were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#native-scrolling-message-and-explicit-cue-policy-gate-2026-09-27).

Latest App text-resource matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built `frontierx_native_app_text_resources` and the complete
`linux-*-debug` presets. Each full CTest preset passed 48 executing
checks and skipped the sandbox cursor/two GPU checks. The existing
shell test exercised source-string display and reset; both compiler
shells executed the same startup with lawful external
`Sol_Train.dmission`, explicit movie skip and SDL dummy devices, then
reported "mission not started". A temporary GCC combined App/script
link failed on 85 references across 29 symbols; Windows Debug/Final,
module ABI and hardware GPU checks were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#native-status-and-hint-resource-client-gate-2026-09-27).

Latest load-progress/version matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built the native `IPANIM` and generated source-version owners,
native shell and view-parser check. Both full Linux Debug builds and
CTest presets passed (48 executed checks, three command-sandbox
cursor/GPU skips each). Both lawful external training-mission shells
executed progress updates and exited zero while reporting "mission not
started"; both view-parser processes read saved build version zero.
A temporary GCC combined App/script link failed on 115 references
across 31 symbols; native Windows Debug/Final and module ABI checks
were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#native-load-progress-and-source-build-version-gate-2026-09-27).

Latest packet-search matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
built the native `dwordsearch` owner, the production packet check and
native shell. Both full Linux Debug builds and CTest presets passed
(48 executed checks, three command-sandbox cursor/GPU skips each).
The existing offline packet check verified first-match pointer,
duplicate, absent and zero-length cases. A temporary GCC App/script
link still fails on 121 references across 33 missing symbols; it did
not run. Native Windows Debug/Final and ABI checks were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#native-packet-selection-search-gate-2026-09-27).

Latest App string-resource matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built the complete Linux Debug presets, including the generated
`Globals.rc` English resource and native App lookup owner. Each CTest
preset passed 48 executed checks with three command-sandbox cursor/GPU
skips. Both external Steam-data training-mission shells executed wide
and narrow lookup, exited zero and still reported "mission not started".
A temporary GCC combined App/script link remained unsuccessful with
126 references across 34 absent symbols. Windows Debug/Final and ABI
checks were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#source-app-string-resource-link-gate-2026-09-27).

Latest production music-owner matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built the original `MusicManager.cpp` as
`frontierx_app_music_manager_owner`, linked it into the native shell,
and completed both Linux Debug builds and CTest presets (48 executed
passes, three command-sandbox cursor/GPU skips each). Both compiler
shells with lawful external `Sol_Train.dmission`, source profile,
explicit movie skip and SDL dummy audio printed `Native App
MusicManager initialized`, exited zero, and still reported "mission
not started". Native music `Load`, `Save` and `PlayMusic` did not run;
native Windows Debug/Final, module ABI, hardware GPU/audio and
integrated gameplay remain unperformed. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-music-manager-owner-startup-gate-2026-09-27).

Latest mission audio-binding matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 completed both Linux Debug builds and CTest presets (48
executed passes, three command-sandbox cursor/GPU skips each). Both
native shells loaded the lawful external `Sol_Train.dmission`, bound
the Music source, initialized required `IStreamer` on SDL dummy audio,
and exited zero while reporting "mission not started". The existing
shell test rejected a missing `IStreamer`. A separate GCC SDL/no-GPU,
no-FFmpeg shell build compiled but failed to link at existing
`TRANSFORM::get_roll/get_pitch/get_yaw` and `ICQImage::Assert`
references; it was not executed. Native Windows Debug/Final, module
ABI, hardware GPU/audio and integrated mission checks remain open.
See [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-mission-audio-ownership-gate-2026-09-27).

Latest saved-music layout matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built the complete Linux Debug graph and executed the existing
view-parser probe against the lawful external `Sol_Train.dmission`.
Both converted four saved music songs to native records, discarded
saved handles, and preserved flags, volume and names in a disk-format
round trip. Both CTest presets executed 48 passes with three
command-sandbox cursor/GPU skips. A clean GCC SDL/no-FFmpeg build of
`frontierx_native_window_manager_probe` and its CTest passed. Native
Windows Debug/Final, module ABI, hardware GPU/audio and integrated
mission checks remain unperformed. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-music-save-layout-conversion-gate-2026-09-27).

Latest selected-music matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
built the complete Linux Debug graph with the FFmpeg-enabled native
streamer and each CTest preset executed 48 passes with three
command-sandbox cursor/GPU skips. Both existing window-manager probes
decoded the lawful external `solarian_game14.wav` to 160448512 F32
bytes and briefly played/stopped it through SDL dummy audio; both
short `STMG03.wav` probes completed playback. The external mission
filesystem check read a 176-byte music save, and the parser check
validated its 44-byte `SONG` schema. Both external-data shells
initialized `IStreamer` and parsed the mission source but reported
"mission not started". Host PulseAudio attempts did not complete in
the command sandbox and were interrupted; native Windows Debug/Final
and cross-module ABI checks did not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-training-music-decode-and-disk-layout-gate-2026-09-27).

Latest selected-system-map matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 configured and built the complete Linux Debug graph with the
native `ISystemMap` owner. Both executed the existing
`frontierx_data_root_probe --mission-filesystem` on the lawful
external `Sol_Train.dmission`, entering the fresh map-load branch.
Each CTest preset executed 48 passes and skipped three host-dependent
cursor/GPU checks. Both external-data shells initialized required
`IStreamer` and parsed the mission source but reported "mission not
started". A temporary GCC combined link resolved `SYSMAP` and still
failed on 169 references across 37 distinct symbols with no multiple
definitions. Native Windows Debug/Final and cross-module ABI checks
did not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-system-map-fresh-load-gate-2026-09-27).

Latest combined mission-script link diagnostic, 2026-09-27: GCC 16.2.1
and Clang 22.1.8 built `frontierx_sol_t_script` after its native heap
ownership correction; both complete Linux Debug builds passed. Each
CTest preset executed 48 passes and skipped three host-dependent
cursor/GPU checks. A temporary GCC link derived from the successful
shell's Ninja command plus 19 compiled App/script objects, source
string/schema resources and the native global-data format failed with
194 unresolved references across 38 distinct symbols and no multiple
definitions. It produced no executable and was not runtime evidence.
Native Windows Debug/Final and cross-module ABI checks did not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#combined-mission-script-heap-link-gate-2026-09-27).

Latest selected-script resource matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 each configured and built the complete Linux Debug graph
with system `llvm-rc` 22.1.8 and executed `portability.app_schema` and
`portability.sol_t_schema`, including native counted strings from source
`Sol_T.rc`. Each complete CTest preset had 48 executed passes and three
command-sandbox cursor/GPU skips. Both read-only external mission-schema
checks passed the `Sol_T.dll` and zero-update fresh-load preconditions;
both external-data shells exited zero but reported "mission not started".
Native Windows Debug/Final and cross-module ABI checks did not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-localization-and-fresh-loader-gate-2026-09-27).

Latest production script-owner matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 each built `frontierx_app_script_owner` from the original
`MScript.cpp` in the complete Linux Debug graph. Both complete builds
passed; each CTest preset executed 48 checks and skipped three
host-dependent cursor/GPU checks. Separate lawfully external-data
shell runs exited zero after parsing `Sol_Train.dmission`, but
reported "mission not started" and did not link the new owner.
Native Windows Debug/Final and cross-module ABI checks were not run.
See [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-mscript-owner-build-and-required-resource-gates-2026-09-27).

Latest selected-script resource matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 generated and embedded the source `Sol_T` parser schema,
built its native object/resource targets, and executed the existing
`portability.sol_t_schema` and `portability.app_schema` checks. Both
complete Linux Debug builds passed; each CTest preset executed 48
checks and skipped three host-dependent cursor/GPU checks. Separate
lawful external-data shell runs exited zero but reported "mission not
started". Native Windows Debug/Final and script module ABI checks
remain unperformed. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-native-schema-resource-gate-2026-09-27).

Latest selected-script matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 each built the original `Sol_T.cpp` and helper as native object
target `frontierx_sol_t_script`, generated `Sol_T_DataDef.i`, and ran
the production parser check `portability.sol_t_schema`. Both complete
Linux Debug builds passed; each CTest preset executed 48 checks and
skipped three host-dependent cursor/GPU entries. Lawfully external
training-mission shell runs under both compilers exited zero but
reported "mission not started". Windows Debug/Final and script ABI
checks were not performed. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-program-and-schema-build-gate-2026-09-27).

Latest selected-script data matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 built and executed `frontierx_layout_probe` using the
original `Sol_T/DataDef.h`. It observed a source-derived 292-byte
`MissionData`, fixed unit-reference offsets and a four-byte saved
movement state. Both compilers also built and executed the existing
read-only `frontierx_data_root_probe --mission-filesystem` on the
lawfully external training mission: four QuickSave objects opened and
`Script/GlobalData` was absent. Both complete Linux Debug presets
built; each CTest preset executed 47 passing checks with three
host-dependent cursor/GPU skips. No native `MScript`/`Sol_T` gameplay,
Windows Debug/Final or module ABI check ran. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-script-data-and-fresh-start-gate-2026-09-27).

Latest training-script reference matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 built `frontierx_layout_probe` and executed
`portability.layout`, proving native `MPartRef` is 16 bytes, aligned
to four bytes, and zero-initializes all save bytes. Both complete
Linux x86_64 Debug presets built; each CTest preset executed 47
passing checks and skipped three host-dependent cursor/GPU checks.
`MScript.cpp`, the `Sol_T` module, native Windows Debug/Final and
module ABI checks were not executed. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#training-script-mpartref-save-layout-gate-2026-09-27).

Latest spatial-iterator matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built `frontierx_app_object_map_owner` with the original
`ObjMapIterator.cpp`, then both complete Linux x86_64 Debug presets.
Each CTest preset executed 47 passing checks and skipped three
host-dependent cursor/GPU checks. Both external-data shell processes
parsed `Sol_Train.dmission` and reported "mission not started"; the
iterator is not in that executable's active mission path. Native
Windows Debug/Final and module ABI checks were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-spatial-iterator-build-gate-2026-09-27).

Latest movie-camera manager matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 built the new production manager/Objwatch objects and
complete Linux x86_64 Debug presets. The focused
`portability.net_buffer` check executed manager registration, event
callback, camera `Save`/`Load` byte round trip with malformed-input
rejection, and cleanup in a synthetic offline App process. Each full
CTest preset executed 47 passing checks and skipped three host-dependent
cursor/GPU checks. Both external-data native shells ran and parsed
`Sol_Train.dmission` but stopped before mission startup. A temporary
combined App link still lacked 50 production symbols. Native Windows
Debug/Final, module ABI and full mission save/reload were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-movie-camera-manager-gate-2026-09-27).

Latest camera-template matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 built the shared `TObjFrame`/`TObjTrans`/`TObjRender` header
closure in `frontierx_app_object_list_owner` and completed both Linux
x86_64 Debug presets. Each CTest preset executed 47 passing checks
and skipped three command-sandbox cursor/GPU entries. A direct GCC
`MovieCamera.cpp` syntax diagnosis still fails on Win32 UI/editor
contracts; the camera owner did not execute. Native Windows
Debug/Final and module ABI checks were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#camera-object-template-closure-2026-09-27).

Latest selection-ring table matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 built `frontierx_app_circle_lines_owner` and executed
its original static initializer through `portability.math_core`.
Both complete Linux x86_64 Debug presets built; each CTest preset
executed 47 passing checks and skipped three command-sandbox
cursor/GPU entries. A temporary combined GCC App link resolved
`circleLines` but still lacked 51 distinct production symbols.
Native Windows Debug/Final and module ABI checks were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-selection-ring-table-gate-2026-09-27).

Latest simulation event-owner matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 built `frontierx_app_event_scheduler_owner` and both
complete Linux x86_64 Debug presets. Each CTest preset executed 47
passing checks and skipped three command-sandbox cursor/GPU entries.
The owner did not execute in the successful shell. A temporary
combined GCC App link resolved `SCHEDULER` but still lacked 52
distinct production symbols. Native Windows Debug/Final and module
ABI checks were not run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-event-scheduler-build-gate-2026-09-27).

Latest spatial-object map matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 built `frontierx_app_object_map_owner` and complete
Linux x86_64 Debug presets. Each full CTest preset executed 47
passing checks and skipped three command-sandbox cursor/GPU entries.
The owner did not execute in the successful shell. A temporary
combined GCC App link resolved `OBJMAP` but still lacked 53 distinct
production symbols. Both lawfully external Steam-data shells reported
"mission not started". Windows Debug/Final and ABI checks were not
run. See [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-spatial-object-map-build-gate-2026-09-27).

Latest fog-owner matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
built the production `frontierx_app_fog_owner` and complete Linux
x86_64 Debug presets. Each full CTest preset passed 47 executing
checks and skipped three command-sandbox cursor/GPU entries. Its
mission `New/Save/Load`, update and render symbols were confirmed in
the GCC object, but the owner did not execute in the successful
shell. A temporary combined GCC App link including fog still lacks
54 distinct production symbols without multiple definitions. Both
lawfully external Steam-data shells report "mission not started".
Native Windows Debug/Final and module ABI checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-fog-of-war-native-build-gate-2026-09-27).
Entries below are historical.

Latest nugget-owner matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
built the production `frontierx_app_nugget_owner` and complete Linux
x86_64 Debug presets. Both full CTest presets passed 47 executing
checks and skipped three command-sandbox cursor/GPU entries; the
nugget owner did not execute. Both lawfully external Steam-data shells
parsed `Sol_Train.dmission` but reported "mission not started". A
temporary GCC combined App link including the owner lacked 53
distinct production symbols and had no multiple definitions. Native
Windows Debug/Final and module ABI checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-nugget-manager-native-build-gate-2026-09-27).
Entries below are historical.

Latest native cursor-service matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 built `frontierx_app_cursor_probe` and the complete
Linux x86_64 Debug presets. The focused cursor processes executed on
the visible Wayland host under both compilers, checking SDL shape,
busy, resource update and cleanup. Each full CTest preset passed 47
executing checks and skipped three host-dependent entries in the
command sandbox (cursor and two GPU); these skips are not runtime
evidence. Both external-data shells parsed `Sol_Train.dmission` but
reported "mission not started". A temporary GCC combined App link
still lacks 50 distinct production symbols without multiple
definitions. Native Windows Debug/Final and module ABI checks were
not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-cursor-service-2026-09-27).
Entries below are historical.

Latest App primitive-builder matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 configured and built the native builder owner and
`frontierx_render_submission_probe`; the existing CPU test executed
the real App quad draw with six vertices and checked 32-byte alignment.
Both full Linux x86_64 Debug builds passed; each CTest preset executed
47 passing checks and skipped two command-sandbox GPU checks. Both
lawfully external Steam-data shell processes parsed `Sol_Train.dmission`
but reported "mission not started". A temporary GCC combined App link
still lacks 51 distinct production symbols. Native Windows Debug/Final
and module ABI checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-primitive-builder-owner-2026-09-27).
Entries below are historical.

Latest production grid-distance matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 configured and built the production
`frontierx_app_grid_vector_owner` and linked it into
`frontierx_math_core_probe`. Both direct processes and CTest ran;
the 4096-position fingerprint matched at `10760374359831800577`.
Both complete Linux x86_64 Debug builds passed; each full CTest
preset executed 47 passing checks and skipped two command-sandbox
GPU checks. Both external-data shells reported "mission not started".
A temporary combined GCC App link still lacks 54 distinct production
symbols. Native Windows Debug/Final and module ABI checks were not
run. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-grid-distance-owner-for-mission-objects-2026-09-27).
Entries below are historical.

Latest base-object matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
configured and built `frontierx_app_object_list_owner` with both
`ObjList.cpp` and `BaseObject.cpp`, plus the complete native shell and
Linux x86_64 Debug presets. Each full CTest preset executed 47
passing checks and skipped two command-sandbox GPU checks. Lawful
external-data shells under both compilers still reported "mission not
started". A temporary GCC combined App link failed at 55 distinct
unresolved production symbols without multiple definitions. Native
Windows Debug/Final and module ABI checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-base-object-owner-in-the-native-link-graph-2026-09-27).
Entries below are historical.

Latest native save-cleanup matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 configured and built `frontierx_data_root_probe`, compiled
`frontierx_app_mission_owner`, and built both complete Linux x86_64
Debug presets. Each full CTest preset executed 47 passing checks and
skipped two command-sandbox GPU checks. The generated UTF data-root
check kept its marker intact after native save cleanup rejected its
read-only archive. Both external-data shells reported "mission not
started". A temporary GCC combined App link still lacks 80 distinct
production symbols; native Windows Debug/Final and ABI checks were not
run. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-save-archive-cleanup-2026-09-27).
Entries below are historical.

Latest offline packet-dispatch matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 configured and built `frontierx_app_net_packet_owner`,
the linked packet/heap checks, and both full Linux x86_64 Debug
presets. The focused process delivered one `NETF_ECHO` envelope to
`CQE_NETPACKET` through production packet owners and the real
EventSystem; private heaps balanced without the former false leak
warning. Each full CTest preset executed 47 passing checks and
skipped two command-sandbox GPU checks. Both lawful external-data
shells exited 0 after mission schema loading and reported "mission
not started". Native Windows Debug/Final and ABI checks were not run.
A temporary combined GCC App link failed on 87 unique required symbols
without multiple definitions; the packet owners are not yet in the
successful shell link.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-offline-packet-dispatch-2026-09-27).
Entries below are historical.

Latest local packet execution matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 configured and built the production
`frontierx_app_net_buffer_owner`, `frontierx_net_buffer_probe` and
both complete Linux x86_64 Debug presets. The focused process and
CTest each delivered two ordered offline packets through the original
queue. Each full CTest preset executed 47 passing checks and skipped
two command-sandbox GPU checks. Both lawful external-data shells
exited 0 after mission schema loading and explicitly reported
"mission not started". `NetPacket` is not yet a native object target;
native Windows Debug/Final and ABI checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-local-packet-buffer-build-gate-2026-09-27).
Entries below are historical.

Latest packet-buffer matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
configured and built `frontierx_app_net_buffer_owner` and both complete
Linux x86_64 Debug presets. Each CTest preset executed 46 passing
checks and skipped two command-sandbox GPU checks. Both lawful
external-data shell runs exited 0 after source loading and explicitly
reported "mission not started"; the new owner was not linked or
executed. Native Windows Debug/Final and ABI checks were not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-local-packet-buffer-build-gate-2026-09-27).
Entries below are historical.

Latest operation-owner build matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 configured and built the production
`frontierx_app_op_agent_owner` object target and both complete Linux
x86_64 Debug presets. Each full CTest preset passed 46 executing
checks with two sandbox GPU skips. Separate lawful external-data shell
runs exited 0 but did not start a mission. A temporary GCC shell link
with `Mission`, `ObjList`, `MGlobals`, `Objwatch` and `OpAgent` found 91
unresolved production App symbols and no multiple definitions. Native
Windows Debug/Final and module ABI checks were not executed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-operation-owner-native-build-gate-2026-09-27).
Entries below are historical.

Latest shared App-startup build matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 configured and built `frontierx_app_global_startup_owner`,
the linked `frontierx_native_shell` and the focused startup probe on
Linux x86_64. Both complete Debug builds succeeded; each CTest preset
passed 46 executing checks and skipped two GPU checks in the command
sandbox. Both lawful external-data shell runs exited 0 but expressly
did not start a mission. A temporary GCC link adding the production
mission/object/global owners found 89 unresolved App symbols. Native
Windows Debug/Final and DLL ABI checks were not executed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#shared-app-global-startup-and-cleanup-owner-2026-09-27).
Entries below are historical.

Latest production object-owner build matrix, 2026-09-27: GCC 16.2.1
and Clang 22.1.8 configured and built the new
`frontierx_app_object_list_owner` CMake object target and both complete
Linux x86_64 Debug presets. Both full CTest presets passed 45 executing
checks with two sandbox GPU skips. A temporary GCC shell link of that
object failed with 80 unique unresolved production App symbols, including
global startup, `MGlobals`, mission and object support. The temporary
link was removed; the successful shell still loads only external
GameTypes/GenData/mission schema and does not run `ObjectList`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-object-owner-build-and-link-closure-2026-09-27).
Entries below are historical.

Latest App event-owner matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
full Linux x86_64 Debug builds succeeded after binding the production
`IEventSystem` in the mission-source shell path. Each full CTest preset
passed 45 executing checks and skipped two GPU checks; the final
`frontierx_native_shell` targets and focused shell CTests were rebuilt/
rerun after a binding error-path cleanup. Both final external-data
shells exited 0 after executing the event-owner identity check and
mission-schema load, explicitly without starting the mission.
Direct `ObjList.cpp` source checks still exit 1 at further App services;
the owner is not in the native graph. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-object-list-event-owner-contract-2026-09-27).

Latest object frame-clock source matrix, 2026-09-27: GCC 16.2.1 and
Clang 22.1.8 full Linux x86_64 Debug builds completed after the shared
native App timing-global changes. Each full CTest preset passed 45
executing checks and skipped two GPU checks in the command sandbox;
the existing `portability.platform_time` test passed explicitly under
both compilers. Direct `ObjList.cpp` syntax checks still exit 1 on
App event/nugget/camera/scheduler services; it remains outside the
native graph. Both external-data shells exited 0 after parsing
`Sol_Train.dmission` but did not start it. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-native-frame-clock-source-contract-2026-09-27).

Latest object-debug source matrix, 2026-09-27: direct GCC 16.2.1 and
Clang 22.1.8 C++17 syntax checks of `ObjList.cpp` on Linux x86_64 now
first fail at `FULLSCREEN` ownership and `BeginFrame` timing after the optional
performance-report, editor dialog and `__int64` source barriers were removed. Both
full Debug build presets succeeded with no rebuild work; the object
owner is not in either graph, and no CTest or external-data runtime was
executed for this source-only change. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-debug-and-clock-declarations-2026-09-27).

Latest object-owner source matrix, 2026-09-27: direct C++17 syntax
compilation of production `ObjList.cpp` under GCC 16.2.1 and Clang 22.1.8
on Linux x86_64 now reaches its body and exits 1 on Win32 debug/UI/timing
dependencies. It is not a native target. Both full Debug builds succeeded
after shared header/template changes; each CTest preset passed 45
executing checks and skipped two GPU checks in the command sandbox.
No retail mission runtime ran in this change. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-native-source-boundary-2026-09-27).

Latest mission-input matrix, 2026-09-27: the existing
`frontierx_data_root_probe` built under GCC 16.2.1 and Clang 22.1.8
on Linux x86_64 and each process opened four selected lawfully external
`Sol_Train.dmission` QuickSave object records through nested DOS views
(exit 0). Both full Debug builds succeeded and each CTest preset passed
45 executing checks; two GPU checks skipped in the command sandbox.
Separate external native shells loaded GenData and parsed the mission
schema with exit 0, explicitly without starting the mission. Direct
`Mission.cpp` syntax compilation still fails on App services. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-quicksave-object-input-boundary-2026-09-27).

Latest mission platform matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
full Linux x86_64 Debug builds completed after adding the native
`AvailableDiskBytes` platform source. Each full CTest preset passed 45
executing checks and skipped two sandbox GPU checks. The existing
`portability.data_root` check executed the new free-space success and
failure cases with temporary synthetic filesystem input. Separate
lawfully external-data shell processes loaded GenData and the selected
mission schema with exit 0, but `Mission.cpp` remains source-only and
did not execute. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-save-space-platform-contract-2026-09-27).

Latest mission-global owner matrix, 2026-09-27: both full Linux x86_64
Debug builds now include `frontierx_app_mglobals_owner` with production
`MGlobals.cpp` and `Objwatch.cpp`; GCC 16.2.1 and Clang 22.1.8 completed
those builds. Each full CTest preset passed 45 executing checks and
skipped two sandbox GPU checks. The App owner is an object target, not
linked into the native shell; a direct shell link experiment measured
unresolved App service symbols. Both final external-data shell processes
continued to load GenData and the selected mission schema with exit 0,
but did not start the mission. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#complete-mglobals-compile-and-link-closure-2026-09-27).

Latest general-data matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
full Linux x86_64 Debug builds completed after linking production
`GenData.cpp` and correcting the native UTF enumeration contract. Each
full CTest preset passed 45 executing checks and skipped two sandbox GPU
checks. Separate external-data shell processes under both compilers loaded
the real `gendata.db` `GlobalValues` (1640 bytes) and parsed the selected
mission schema with exit 0. Neither process started a mission or produced
visible gameplay. Direct `MGlobals.cpp` syntax compiles still fail on the
App service/window/Engine declaration closure. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-general-data-owner-2026-09-27).

Latest App-global compile gate, 2026-09-27: after opening the native
`MGlobals.cpp` include chain, GCC 16.2.1 and Clang 22.1.8 full Linux
x86_64 Debug builds completed. Each CTest preset passed 45 executing
checks and skipped two GPU checks in the command sandbox. Separate direct
C++17 syntax compiles of `MGlobals.cpp` failed in its body on missing App
globals, Win32 `GlobalViewer` window state and an Engine forward enum;
that owner is not part of the successful native graph. No retail-data
mission startup or visible GPU gameplay was executed in this change.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mglobals-native-owner-compile-boundary-2026-09-27).

Latest mission schema matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8
completed full Linux x86_64 Debug builds after connecting the production
DOCUVIEW secondary parse contract to the native App adapter and shell.
Each full CTest preset passed 45 executing checks; the two GPU checks
skipped in the command sandbox and are not runtime evidence. Separate
GCC and Clang shell processes parsed the lawful external selected
mission's 160633-byte ParseData and opened its 3236-byte Globals record
with exit 0. `Mission.cpp` still fails direct native syntax compilation
on missing App globals; no mission gameplay or visible GPU runtime ran.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-parsedata-owner-contract-2026-09-27).

Latest native App resource matrix, 2026-09-27: GCC 16.2.1 and Clang
22.1.8 completed full Linux x86_64 Debug builds after embedding the
corrected repository `data.i` and tightening native DOCUVIEW error
reporting. Each full CTest preset passed 45 executing checks and skipped
two GPU checks in the command sandbox. The first full test attempt exposed
an outdated minimal-profile shell assertion: mission-source startup now
correctly fails when required `IViewConstructor` is absent. The existing
shell test now checks that failure, then adds the real parser aggregate and
checks subsequent missing-global and successful source-open paths; both
focused and repeated full CTests pass. Separate lawful external-data shell
processes under both compilers loaded the App schema and opened
`Sol_Train.dmission` with exit 0. This is not visible GPU gameplay or
`Mission::ReloadMission`; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-parser-resource-binding-2026-09-27).

Latest matrix repeat, 2026-09-27: after adding the repository App-schema
mode to the existing view-parser check, GCC 16.2.1 and Clang 22.1.8 full
Linux x86_64 Debug builds completed. Each CTest preset executed 45 passing
checks and skipped two GPU checks in the command sandbox. The new
`portability.app_schema` check parsed `Code/App/Src/data.i` and found a
3236-byte `MT_GlobalData` under both compilers. `Mission.cpp` and
`MGlobals.cpp` remain outside the native build; no mission or visible GPU
gameplay ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#repository-app-schema-and-mission-source-body-2026-09-27).

Latest matrix repeat, 2026-09-27: after sharing the App packet enum and
adding native script-registration layout checks, GCC 16.2.1 and Clang
22.1.8 full Linux x86_64 Debug builds completed. Each CTest preset ran 44
passing checks; the two GPU checks were skipped in the command sandbox.
Separate bounded external-data shells under both compilers opened
`Sol_Train.dmission` and created/destroyed the Forger Engine instance with
13 face groups, exit 0. `Mission.cpp` and `MGlobals.cpp` are not part of
this graph; no gameplay or visible GPU path ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#app-declarations-and-script-registration-abi-2026-09-27).

Latest matrix repeat, 2026-09-27: after the native App data-source binding
and physical child-directory root repair, GCC 16.2.1 and Clang 22.1.8 full
Linux x86_64 Debug build presets completed. Each CTest preset executed 44
passing checks and skipped the two command-sandbox GPU checks. Separate
external-data shells under both compilers opened `Sol_Train.dmission` through
`SPMAPDIR`, created/destroyed the external Forger instance using bound
`OBJECTDIR`/`MATERIALDIR`, reported 13 face groups and exited 0. No mission
gameplay or visible GPU runtime occurred; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-mission-source-bindings-2026-09-27).

Latest matrix repeat, 2026-09-27: both GCC 16.2.1 and Clang 22.1.8 Linux
x86_64 Debug build presets now include the existing `frontierx_view_parser_probe`
target and its native global-data format library. Full builds completed;
each CTest preset executed 44 passing checks and skipped the two GPU checks
in the command sandbox. Separate compiler processes correlated and
round-tripped the 3236-byte lawful external `Sol_Train.dmission` global
record. The App `MGlobals.cpp` owner, save output, mission and GPU gameplay
did not run. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-global-data-format-and-owner-call-2026-09-27).

Latest matrix repeat, 2026-09-27: after the native filesystem's mission
root-path repair and extension of existing data-root, view-parser and material
checks, GCC 16.2.1 and Clang 22.1.8 full Linux x86_64 Debug builds completed.
Each CTest preset discovered 46 checks, executed 44 successfully and skipped
the two GPU checks in the command sandbox. Separate GCC and Clang processes
opened lawful external `Sol_Train.dmission` loader files and parsed its
schema through production interfaces; no mission or visible GPU gameplay
ran. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-loader-paths-and-global-schema-2026-09-27).

Latest animation-data matrix, 2026-09-27: GCC 16.2.1 and Clang 22.1.8 full
Linux Debug build presets completed after linking the original Channel
owner and its narrow shell validation caller. Each full CTest preset passed
44 executing checks with two command-sandbox GPU skips. The focused Clang
ASan/UBSan shell check passed with LeakSanitizer disabled, as did the GCC
SDL/no-GPU shell check (which cannot execute animation). Both compiler
shells separately loaded a Forger animation script from ten lawfully external
Steam archives with exit 0; no mission or visible GPU gameplay ran. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-channel-and-forger-animation-data-2026-09-27).

Latest render-component gate, 2026-09-27: the native GPU shell now links the
three production Optics source files. Native `RenderMgr` rejects a missing
named render component and ignores blank profile lines. GCC and Clang shells
each executed the unchanged source profile with the original `PolyMesh` and
`Optics` entries on external data (exit 0). The existing generated-profile
shell check passed for Optics creation and missing-component rejection.
Both full Debug builds and CTest presets passed with 44 executing checks and
two command-sandbox GPU skips each. Particle rendering and GPU mission runtime
remain unexecuted. Focused Clang ASan/UBSan shell execution passed with
LeakSanitizer disabled; see
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-animation-and-hardpoint-startup-2026-09-27).

Latest matrix repeat, 2026-09-27: after linking the original Anim and
HardPoint Engine owners, GCC 16.2.1 and Clang 22.1.8 completed the full Linux
Debug build presets. Each full CTest preset passed 44 executed checks and
skipped the two command-sandbox GPU checks (`game_material_gpu`, `sdl3_gpu`).
The existing shell check now includes an Engine animation/hardpoint aggregate
startup and teardown case; focused Clang ASan/UBSan passed with LeakSanitizer
disabled. The GCC SDL/no-GPU shell process check passed without Engine
aggregate execution. Separately, both compiler shells executed the
unchanged source profile with ten lawful external Steam archives and explicit
movie skip under SDL's dummy video driver, exiting 0 after
`GameSystem::startup`. No visible GPU mission execution occurred; see
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-animation-and-hardpoint-startup-2026-09-27).

Latest matrix repeat, 2026-09-27: after linking the three production
MeshManager classes and moving RenderMgr/PolyMesh registration before startup,
GCC 16.2.1 and Clang 22.1.8 completed full Debug preset builds. Each CTest
preset executed 44 passing checks and skipped two GPU checks in the command
sandbox. Focused Clang ASan/UBSan and GCC SDL/no-GPU native-shell checks
passed. The unchanged external-data profile now stops at required Engine
`IAnimation`; no game or mission ran. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-mesh-manager-owner-2026-09-27).

Latest matrix repeat, 2026-09-27: after linking the production particle owner
into the GPU-enabled shell, both GCC 16.2.1 and Clang 22.1.8 full Debug preset
builds completed. Each CTest preset ran 44 passing checks and skipped the two
sandbox GPU checks; the existing native-shell process check exercised actual
point-emitter program creation. The GCC SDL/no-GPU shell build and process
check also passed. The new external-data startup reaches required
`IMeshManager`, but no native mission or GPU gameplay executed. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-particle-owner-2026-09-27).

Original inventory evidence: source review and automated inventory, 2026-09-13. It did not run
MSBuild or the retail game. A separate native-Windows rebuild is recorded below.

This build-declaration view is complemented by the 2026-09-16 lexical
[platform touchpoint inventory](PLATFORM_INVENTORY.md), which groups Windows, compiler,
pointer-width, graphics, audio, and video candidates by source owner. Neither inventory evaluates
MSBuild imports or establishes a native game target.

Latest native matrix status, 2026-09-27: the checked-in GCC 16.2.1 and
Clang 22.1.8 full debug builds completed after adding the production game
material-manager, game-texture-manager, `PolyMesh`, `Engine`, `RenderMgr` and
scalar `I3DMathEngine` objects and their separate
script-and-CQ2-mesh executable. Each
CTest preset now discovers 46 checks, executes 44 successfully and skips
`portability.game_material_gpu` and `portability.sdl3_gpu` before SDL video
initialization in the command sandbox. The same full result passed after
nested UTF mission-file support and its extended StringTable owner check,
after packed startup-source detection and native mount ownership, and after
physical material-directory support and explicit shell source selection,
native IStreamer integration in the existing window owner/check, and the
optional shared-LGPL FFmpeg IAC adapter and explicit full-screen movie
exclusion through the existing App flag;
the focused Clang ASan/UBSan owner run also passed. No new GPU execution
was performed for that filesystem-only change. This full result was repeated after
connecting the GPU shell to `GameSystem::startup`, after linking the
production `IViewConstructor` owner, and after profile registration of the
production `TextureLibrary`, and after linking the production `LightManager`
owner and its factory into the GPU shell, then after adding the bounded
StringTable UTF-16LE file-format check, then after linking the production
StringTable owner into the GPU shell and after the StringTable failed-load
rollback change, then after pre-startup registration of the production game
material/texture factories and profile-owner reuse/cleanup. The latter full
runs again executed 44 checks successfully and skipped the two sandbox GPU
checks. Separate visible GCC/Clang and Clang ASan/UBSan profile-created
material/texture shell runs passed; see
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md#profile-created-game-material-and-texture-owners-2026-09-27).
The generated owner check executes
DACOM aggregation, XML save/load, and a UTF archive load under both compilers.
The generated nine-service system
profile and focused production light aggregate check execute under both
compilers; the latter also passes Clang ASan/UBSan with LeakSanitizer disabled.
The no-GPU shell process suite and
focused Clang ASan/UBSan constructor check passed, with LeakSanitizer
disabled on this host. Both GPU checks separately executed
under GCC and Clang on the desktop Wayland/Vulkan session. The native shell
now links the production game material, texture, Engine, RenderMgr, PolyMesh
and math owners, and its explicit repeated `--game-scene` mode draws a
supplied UTF mesh through those owners. The production App
`frontierx_app_system_container_owner` object now also links into the shell
and loads its EventSystem and WindowManager aggregates from synthetic
`[System]` entries through `LoadSystemComponents`; see
[APP_SYSTEM_CONTAINER.md](APP_SYSTEM_CONTAINER.md). The GPU-enabled shell now
also compiles the launcher adapter for the real `GameSystem::startup` caller,
and the production Engine it creates now draws the repository CQ2 fixture
after a later synthetic component load. The fixture's four temporary render
and material services reach that Engine through the App container's real
`AddComponent` and interface forwarding path. The SDL3/no-GPU shell retains direct
App container composition; see
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md). GCC, Clang and focused Clang
ASan/UBSan visible runs produced 496 changed pixels and checksum
`11826028615312451232`; GCC/Clang shell P6 captures matched each other
and the linked PolyMesh probe byte for byte. A native-only compile-time
rename separates the CQ2 and legacy mesh `Material` destructor symbols in
that combined ELF. The shell now also links the App mesh owner: its CQ2
`VertexColor.fx` material branch applied `c30`/`c31`, bound a production
TGA and presented a second 496-pixel image with checksum
`1466864583390317354` under visible GCC, Clang and focused sanitizer runs.
The GCC/Clang P6 files were byte-identical; an unsupported `Synthetic.fx`
mesh was rejected. See [NATIVE_SHELL.md](NATIVE_SHELL.md). The new game
material check extends the linked production Engine/RenderMgr/PolyMesh
executable and observed 496 changed readback pixels and a presented frame;
see [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).
The same executable now also calls production App `MeshRender::Render` for
generated older-format and CQ2 material instances. Its `IMaterial::DrawVB`
branch sends six U16 indices through the 68-byte native CPU receiver after
the App caller's D3D vertex-count argument was corrected from `maxVert` to
`maxVert + 1`. Both full debug builds and CTest presets passed after this
extension; the visible game-mesh GPU check was rerun under GCC, Clang and
focused Clang ASan/UBSan. The older-format App branch now also presents
through the 68-byte SDL GPU pipeline under GCC and Clang. The first
App readback was a false positive from split `PIPE`/`BATCH` ownership;
the corrected check reads back during the App draw with texture factor
and clockwise culling active. The existing GPU executable can now
write 64-by-64 P6 captures of the production PolyMesh and App synthetic
draws; GCC and Clang files were byte-identical for each draw (see
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md)). The CQ2 `VertexColor.fx`
App material subsequently reached visible GPU presentation in that linked
check and the native shell; Windows comparison remains open.
The earlier 41-entry
matrix was rerun after the native `RP_TEXTURE` pipeline state
and explicit `PolyMesh` model/view rejection; both focused GCC and Clang
render-pipeline and game-material checks passed. The same 40-execution
matrix was rerun after the production `MTVERTEX` indexed CPU callback and
its 36-byte adapter were added. It was rerun again after local `PolyMesh`
draw-failure propagation, stage-zero UV/specular state handling and a third
SDL graphics pipeline for that vertex format. The GPU test still skipped;
the new game-format readback assertions compiled but did not execute.
The later App `StandardVertex` boundary added native allocation and checked
CPU forwarding for the exact 68-byte `XYZ | NORMAL | DIFFUSE | TEX5` layout,
plus an optional fourth primitive callback. On 2026-09-27 the native U16
index-buffer object and renderer overload allowed the production
`Material::DrawVB` method to reach that callback with a nonzero vertex-base
slice and exact byte comparison. GCC and Clang full builds and CTest presets
retained the same 40 executions and one GPU skip; focused Clang ASan/UBSan
executed the synthetic material method in all three generated UTF variants.
The App `MeshRender.cpp` caller and material effect execution remain open;
see [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).
On 2026-09-27, after exact-case include and native declaration repairs along
that App caller's dependency path, GCC and Clang full debug preset builds
again executed 40 passing checks and skipped the GPU check. A separate
temporary `MeshRender.cpp` syntax preflight passed the native `IObject`,
`GridVector` and `DBaseData` declarations and reached further PCH/D3DX
blockers in `ObjList.h` and `MeshRender.cpp`; it is not a CMake object or
game draw.
After moving the unchanged `ObjList` contracts into a shared App header
and adding a native App-global declaration subset, GCC and Clang directly
syntax-checked production `MeshRender.cpp`. A bounded D3DX math helper then
passed `portability.math_core` under both compilers. The remaining
material-pointer batching cast now uses a reserved native `U32` state-ID
range; native texture handles are capped below it and the existing pipeline
probe checks rejection/cleanup at the boundary. Native D3DX effect branches
stop before their effect-dependent draws. GCC and Clang now compile
`MeshRender.cpp` in the `frontierx_app_mesh_render_owner` CMake object target.
Both full debug builds and CTest presets passed after this addition: each
executed 40 checks and skipped `portability.sdl3_gpu` before device creation.
The App object is not linked or executed, and its face-storage changes have
no runtime evidence yet.
The same App object target now also compiles production `MeshInfo.cpp` and
`SuperTrans.cpp`. A native `get_angle` owner linked into the existing math
check passes analytic quadrants and the legacy axis/origin convention under
GCC and Clang; no Windows numeric oracle was run. A manual link against the
existing game-material fixture still fails at 23 distinct App globals,
diagnostic and blend-helper symbols, with no duplicate definitions. Both
full debug builds and CTest presets passed after this expansion, with 40
executions and the same GPU skip per preset. See
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md) for the owner and link audit.
The 2026-09-27 App composition step supplied native owners for process
globals, diagnostics and the active diffuse-state helper while leaving the
supported Windows definitions in place. The production `MeshRender.cpp`,
`MeshInfo.cpp` and `SuperTrans.cpp` objects now link into the generated
game-material executable without unresolved or duplicate App symbols. That
executable creates and destroys a real App `MeshInfoTree` around a generated
Engine/RenderMgr/PolyMesh instance and checks that the borrowed instance
survives. The same fixture now also executes production `MeshRender::Init`,
`RestoreVertexBuffers` and `SetupMeshInfo` with the existing native pipeline
and a narrow test registration manager, checking nonempty geometry and
balanced registration/release. GCC and Clang full debug builds each completed; their CTest presets
again executed 40 checks successfully and skipped `portability.sdl3_gpu`
before device initialization. All three game-material variants passed a
focused Clang ASan/UBSan run. At that point no App mesh draw or native game
scene executed, and the helper still stopped on unsupported render states.
The later stage-zero sampler and fixed vertex-color/unlit state work let
the production App `SetupDiffuseBlend` function execute in the generated
game-material CPU fixture, including clamp and wrap. The full GCC and Clang
debug builds again completed; each command-sandbox CTest preset ran 40
checks and skipped the GPU check at video initialization. The focused
render-submission check and three material variants passed Clang ASan/UBSan.
With desktop-session access, the SDL/Vulkan CTest passed under GCC, Clang
and focused Clang ASan/UBSan: 12 presented frames, two scene lifecycles,
2,496 changed pixels and final checksum `7951318278861886233` in each run.
Its new linear-versus-point sampler image assertion and existing transform
and material checks executed. This is synthetic hardware evidence, not an
App game scene. See [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md) and
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).
Two game-manager checks use
CRLF and CR versions of a generated UTF
script and resolve a second generated mesh through the production `XMesh`,
`TextureLibrary`, game `ITManager` and material manager. The script's
`Tile.tga` now reaches a 4-by-4 native pipeline CPU upload with all 64
expected RGBA bytes and a matched `ITManager::Flush` release; a third
generated material has no
`END` line and verifies that the production reader terminates at EOF. The
native TGA file path now rejects a truncated mapped pixel payload before
decode in that same probe. GCC and Clang full debug builds and CTest presets
were rerun after the Engine/RenderMgr instance change with the same 40 executions and one sandbox
GPU skip. The production `PolyMesh`, `Engine` and `RenderMgr` objects link into
that probe. The component factory initializes with production engine and game
services and creates a generated CQ2 mesh archetype; a fixed-width property
conversion rejects pointer-shaped data on native LP64.
The same executable also verifies rejection and cleanup of an unclaimed
production Engine archetype after correcting an invalidated-iterator read;
the production Engine now also loads `RenderMgr` and `PolyMesh` from a generated
profile, creates an instance, and exposes its mesh through `IRenderer`. Both
full presets were rerun after this integration with the matrix result above.
With the scalar provider registered, the same synthetic CPU check enters
production `render_instance`, reads native `RP_TEXTURE`, and forwards
model/view to a test primitive receiver. The production `PolyMesh` indexed
`MTVERTEX` call now reaches a rejecting native CPU callback with six U16
indices and matching first/second UV pairs. The SDL GPU transform path and
all four shaders compile but their runtime image check skipped. A test camera
records the inverse-transform query. The full presets were rerun after
this render-entry change with the matrix result above.
Both fixed build-preset target lists now include the game-material executable;
the GCC and Clang full builds rebuilt it before the latest 41-entry CTest
run, so these results do not rely on a stale focused binary.
This does not draw or present a game component. The
existing material check also verifies implicit UTF file views and their
bounded read-only mapping contract. Focused Clang ASan/UBSan game-manager
runs passed for all three inputs; the mapped-file material check passed with
focused sanitizers too.
These are synthetic CPU results, not GPU, Windows or game-scene execution;
see [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).

Earlier native matrix status, 2026-09-26: the checked-in GCC 16.2.1 and Clang 22.1.8 debug build presets
completed. Their CTest presets each discovered 38 tests, executed 37 successfully and skipped only
`portability.sdl3_gpu` because this sandbox exposed no graphical/GPU runtime. The focused render
submission and pipeline tests executed in both matrices; the skipped GPU test is not runtime
evidence. The 2026-09-26 rerun followed the production `MaterialLibrary` link into the
`frontierx_state_material_probe`; both full builds had no remaining work, and both CTest
matrices again reported 37 executed passes and the same GPU skip. No Windows, retail or
native game execution was performed in this matrix. After the read-only UTF
directory and material discovery integration, both full presets were rebuilt
and their CTest suites rerun with the same 37 executed passes and one GPU skip.
The changed `frontierx_state_material_probe` now loads two material classes
from a generated UTF fixture. Exact commands and limits are in
[STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md).
An audit on 2026-09-26 found that the two full build presets omitted five
executables registered in CTest: vertex-buffer manager, state material, math
accessor, render-batcher runtime and `BMPOOLJR`. Thus earlier full CTest runs
could use stale binaries for those checks. Both presets now build every
directly named CTest executable; a source-to-preset comparison reports no
missing target. Fresh GCC and Clang configure/build/CTest runs rebuilt the
changed material target, executed 37 passing checks and skipped only the GPU
test. This supersedes the stale-binary risk in earlier preset results.
After the native UTF child-factory addition, both full presets were rebuilt
and again executed 37 passing CTests with the same GPU skip. The focused
Clang ASan/UBSan material executable also exercised nested read-only UTF
views without a reported finding; see the focused material document.
The production `TextureLibrary.cpp` owner was then added as a Linux object
target and linked into the material probe. A clean GCC configure, 37-step
focused build and direct execution passed. GCC and Clang full presets each
executed 37 passing CTests with the GPU check skipped; focused Clang
ASan/UBSan also passed. This verifies owner compilation and a synthetic
runtime-ID lifecycle, not UTF image loading; exact evidence is in
[TEXTURE_LIBRARY_PORTABILITY.md](TEXTURE_LIBRARY_PORTABILITY.md).
The subsequent RGB565 UTF integration uses that same production owner and
material target. GCC and Clang full debug presets again built and executed
37 passing CTests with the GPU test skipped; the focused Clang ASan/UBSan
material executable passed. The CPU check verifies four converted pixels,
the material texture frame and failed-upload rollback. It is not native GPU
or game-scene evidence; see the focused texture document for commands.
The existing SDL GPU probe now also links the production `TextureLibrary`
owner and a shared generated UTF fixture writer. GCC and Clang full presets
again executed 37 passing CTests while the sandbox GPU CTest skipped before
SDL video initialization. Separate direct visible Wayland/Vulkan runs of GCC,
Clang and focused ASan/UBSan builds executed the production RGB565 load,
reported identical UTF image checksum `9085705336011966884` and repeated
scene teardown/presentation. These are synthetic GPU results, not a native
game scene or current Windows comparison; focused commands and boundaries are
in [TEXTURE_LIBRARY_PORTABILITY.md](TEXTURE_LIBRARY_PORTABILITY.md).
The GPU target now also compiles production `MaterialLibrary.cpp` and
`SimpleMaterial.cpp`. Its visible GCC, Clang and focused ASan/UBSan runs loaded
the UTF material through the process DACOM factory, resolved the texture
reference with `SimpleMaterial::verify`/`update` and preserved the same
`9085705336011966884` GPU checksum and repeated teardown evidence. Both
full Linux debug presets again executed 37 passing CTests with the sandbox
GPU entry skipped. This does not execute `SimpleMaterial::apply` or a game
scene; the focused material and texture documents record the remaining gate.
On 2026-09-26 the focused target additionally linked production
`SimpleMaterial`, read its diffuse and opacity fields from the generated UTF
fixture, and rejected an unknown material type without replacing those values.
A clean GCC configure and 36-step focused build passed. The subsequent full
GCC and Clang debug presets each executed 37 passing CTests and skipped the
same GPU test; the focused Clang ASan/UBSan material executable passed. These
are synthetic CPU results, with no Windows or retail execution for this change.
The active game `PolyMesh/Mesh.cpp` owner now has the isolated
`frontierx_polymesh_mesh_owner` object target. GCC 16.2.1 and Clang 22.1.8
focused target builds and both full debug preset builds succeeded on
2026-09-26; each preset CTest run executed 37 passing non-GPU checks and
skipped the sandbox GPU check. This initial mesh-owner result compiled one
production game-mesh source; the later component and game-material links are
recorded in the latest matrix status above. No game scene executed.
The source boundary and unperformed Windows/ABI checks are in
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).
That object now also links with its production `sortQ` source, the native
math accessor and the existing texture owner in the material CPU check.
A generated mesh UTF with one legacy material loads through production
`XMesh::read` under GCC and
Clang; the focused Clang ASan/UBSan executable also passed. Both full debug
presets again executed 37 passing non-GPU checks and skipped the sandbox GPU
check. The old-format material also resolves a production texture frame;
this is synthetic CPU geometry and material loading, not a
native game component or renderer result; exact input and commands are in
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).
The same check now runs a second generated mesh archive whose legacy material
names an absent texture. It observes no frame or alpha flag and releases the
production texture owner's temporary placeholder after mesh teardown. GCC,
Clang and focused Clang ASan/UBSan passed; both full preset CTest matrices
again had 37 non-GPU passes and one sandbox GPU skip.
Separate
required visible GCC and Clang probe and shell runs exercised the current shared scene through
Wayland/Vulkan. The 2026-09-23 visible shell runs additionally traversed the production App-side
`RenderBatcher` direct and U16-indexed branches under GCC, Clang and focused Clang ASan/UBSan; their separate GPU
evidence is recorded in the focused batcher, renderer and shell documents. The separate indexed
RPVERTEX2 GPU probe was also executed visibly under GCC, Clang and focused
Clang ASan/UBSan, with distinct readbacks for its two second-UV inputs. The current visible probe
also preserved the last scene after a rejected oversize upload and reproduced its readback across
two renderer initialization cycles. The latest visible probe and shell additionally bind two
repository RGBA8 textures by runtime `U32` handle through the existing renderer boundary and the
production batcher, observe different images and recover the original readback. The preset CTest
GPU entry still skips in the command sandbox.
After the Windows fixture comparison, both native Linux preset builds were repeated successfully.
Their first CTest rerun exposed two stale lexical expectations in
`portability.platform_inventory`: the Windows repair correctly places `<windows.h>` behind
`_WIN32` in `EventSys.cpp`, and guards `numeric_limits<DWORD>::max` against the Win32 `max`
macro in `HKEvent.cpp`. The inventory now asserts those exact guarded forms instead of the old
ungated absence and unparenthesized spelling. The focused inventory test then passed, followed
by complete GCC and Clang CTest reruns: each reported 38 discovered, 37 executed passing and
the same one GPU test skipped in the command sandbox. The prior separate visible Vulkan P6
exports, not these skipped CTests, supply GPU runtime evidence.
After the queued-pool change, `cmake --build --preset linux-gcc-debug -j2` and the corresponding
Clang build reported no remaining work; `ctest --preset linux-gcc-debug --output-on-failure` and
the Clang counterpart each executed 37 passing checks and skipped only `portability.sdl3_gpu` in
the command sandbox. Separate visible Wayland/Vulkan shell runs under both compilers and focused
Clang ASan/UBSan exercised one production alpha-depth queued draw per process, with immediate and pooled
readback equality. Exact commands, checksum and limitations are in
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).
The separate native-Windows 2026-09-25 regression rebuilt the supported VS2022 Win32 Debug and
Final wrappers, including both engine DLLs and all 18 campaign modules, in a Windows 11 VM.
This is compilation/link evidence only; commands, outputs and remaining ABI, retail and image
comparison gates are in [WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md).
On the later physical Windows 11 host, the wrapper's explicit engine roots were extended to
`Engine`, `Anim`, `HardPoint`, `RenderMgr`, `TextureLibrary`, `PolyMesh` and `Optics` after each
target had passed a focused build. Fresh NoDeploy Win32 Debug and Final wrapper rebuilds then
completed with all nine explicit
engine DLLs, the five core outputs and 18 campaign DLLs in each configuration. Same-build
campaign import checks found zero missing `Mission` or `Trim` symbols. After the final D3D9
depth-buffer, clear-color and fixture changes, the full NoDeploy Win32 Debug and Final wrappers
were repeated on the physical host; both rebuilt the same nine engine DLLs, five application
outputs and 18 campaigns and passed immediate same-build import checks (153 `Mission`, two `Trim`,
zero unsatisfied in each configuration). An isolated Final runtime reached a visible, scrollable
first Terran scene after staging repository TextureLibrary, but that mission does not progress.
The same isolated Final runtime also presented and captured the repository-owned D3D9 fixture;
its 64-by-64 P6 passed the documented Linux Vulkan tolerance comparison. The wrapper still does
not rebuild every DLL listed in `Conquest.ini`; see the focused Windows validation and renderer
documents for exact observations and limits.
The new `frontierx_state_material_probe` compiles and executes production
`StateMaterial` through its DACOM factory and a cached indexed native draw in
both presets. Its focused CPU and sanitizer evidence is in
[STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md). Separate
visible GCC, Clang and focused sanitizer Vulkan runs checked its material-driven
indexed draw and the native texture-copy slot with equal alternate/copy readback
checksums; see [STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md) and
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

## Reproduce

From the repository root, using Python 3 (standard library only):

```text
python3 Tools/portability/build_inventory.py
PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s Tools/portability -p 'test_build_inventory.py' -v
```

Both commands passed on Linux. The first writes JSON to stdout; redirect diagnostic output outside
source control if needed. Five synthetic tests cover solution edges, conditional declarations,
reference cycles, missing references, unresolved project paths and references outside the root.

The scanner starts with Conquest.sln, ConquestCampaign.sln and the literal engine project list in
Build-Conquest.ps1. It follows ProjectReference declarations recursively. It preserves conditions,
source items, include/library directories, additional link inputs, imports, custom commands and
outputs. Cycles do not cause infinite traversal; each project appears once.

This is a union across configurations, not an evaluated MSBuild graph. Excluded source items remain
counted. Imported props/targets, macro expansion, command execution and implicit library edges
are not evaluated. Directory.Build.props and environment-specific SDK imports still need explicit
review when translating a target. A successful scan does not prove that a source path exists with
exact case, that a project builds, or that a linked DLL was rebuilt.

## Measured closure

The current supported entry points reach **36 projects**, **34 distinct ProjectReference edges** and
**8 solution dependency edges**. The closure contains five application projects, eighteen campaign
projects and thirteen library projects. These counts were regenerated from the literal wrapper roots
after the 2026-09-25 extension; the five synthetic inventory checks passed. StringTable is not in
this explicit closure; its existing TinyXML
ownership does not prove that the supported wrapper rebuilds it.

| Library in explicit closure | Declared source items | Boundary found by source review |
|---|---:|---|
| COMHeap | 1 | Allocation boundary; preserve existing CRT/DACOM ownership |
| DACOM | 5 | Component loading, heaps and profile parser; separate services before porting |
| MathLib | 1 | MathLib.cpp acquires I3DMathEngine through DACOM; the provider is a separate runtime component |
| RPUL | 3 | Win32 device enumeration, render interfaces and text utilities |
| MeshManager | 4 | Link declarations include DACOM, MathLib, COMHeap and D3DX9 |
| D3DRenderPipe | 5 | Renderer and its dependent engine libraries; unsuitable as first isolated service |
| Engine | 1 | Production archetype dispatch and component aggregation; Windows runtime enters this owner |
| Anim | 4 | Animation support module loaded by the restored runtime |
| HardPoint | 1 | Hardpoint support module loaded by the restored runtime |
| RenderMgr | 2 | Existing `IRenderer` implementation and render-component selection |
| TextureLibrary | 1 | Production material texture-library load and upload path; restored Windows scene now executes it |
| PolyMesh | 3 | Production mesh archetype and draw component; current scene-entry diagnostic boundary |
| Optics | 3 | Production particle-system render component |

Globals declares only one ClCompile source item but also custom generation steps for Data.cpp and
sfxdata.xmf. A source-count ranking would therefore hide both generation and runtime dependencies.

## Selected next candidate: profile parsing core

The next engine-owned slice to investigate is the in-memory parsing core of
Code/Libs/Src/DACOM/ProfileParser.cpp. It has concrete startup consumers in DACOM.CPP and
Code/App/Src/Conquest.cpp and an existing IProfileParser boundary. Its section-marking and
navigation core now has a shared header and Linux probe (2026-09-13); key lookup, output copying
and an owning portable reader are also shared. Section/key callback traversal is now shared by the
native reader and production DACOM adapter. Runtime section offsets use pointer-width encoded
handles, and the public profile interface header compiles on the portable path. Windows regression
and the Win32 I/O/heap owner closure remain unported.

Before extracting it:

1. Characterize section/key matching, line endings, comments, duplicate entries, missing keys and
   output-buffer truncation from the existing implementation with synthetic input tests. Section
   and key enumeration, early callback stop, final unterminated values, and the production callback
   adapter now have Linux synthetic/source-regression coverage.
2. Separate byte parsing from CreateFile/ReadFile, DACOM interface registration and heap ownership.
3. Section handles are classified as runtime byte offsets in a pointer-width carrier and use the
   shared encoded-handle helper. They are not native object pointers or persistent fields.
4. Link the actual extracted implementation into a Linux test consumer. Preserve the Windows
   adapter and require native Windows regression/ABI validation before accepting that integration.

No replacement parser, generic configuration framework, math-policy change or new dependency was
introduced. Focused profile buffer, document, and public callback paths pass AddressSanitizer and
UndefinedBehaviorSanitizer. Detailed contracts and remaining gates are in
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

## IFF build-edge evidence

The 2026-09-17 pointer-width review found that `DOSFile.vcxproj` lists `IFF.cpp` but marks it
`ExcludedFromBuild` in every one of its six current Win32 configurations. Therefore neither the
documented Windows DOSFile build nor the Linux CMake graph currently compiles that implementation.
Its pointer-width source migration has isolated contract tests, but production acceptance requires
first proving a retail consumer and then adding an explicit compile/behavior target. This excluded
edge must not be described as validated by an ordinary DOSFile rebuild. See
[IFF_PORTABILITY.md](IFF_PORTABILITY.md).

## Math provider follow-up

The 2026-09-16 follow-up established that the 402 concentrated inline-assembly candidates belong
only to the optional legacy K62/3DNow provider. The standard `x86math.vcxproj` compiles scalar
`x86math.cpp` alone. It is not reached by the current `Build-Conquest.ps1` roots even though the
reachable `MathLib` accessor requires a registered provider at runtime. An isolated portable scalar
core and CTest now cover that operation surface without changing the Windows ABI. See
[native math-backend readiness](MATH_BACKEND.md) for the evidence, limitations and adapter gates.

## Parallel platform slice: SDL3 events

Source review on 2026-09-16 identified two concrete consumers for the native window boundary:
`IWindowManager::ServeMessageQueue` owns window state and queue service, while
`System.cpp::wndProc` forwards focus, keyboard, text and mouse messages to `IEventSystem`. The
isolated `frontierx_sdl3_platform` target now owns SDL video/window lifetime and produces SDL-free,
owning event values for those categories. It deliberately does not expose Windows messages or link
the DACOM/event-system graph, so the adapter can be characterized without changing the supported
Win32 path. The legacy-input probe links the otherwise separate targets and synchronously dispatches
compatibility records through the production `EventSystem` to a real DACOM subscriber; the
`frontierx_sdl3_event_pump` integration target now owns complete queue draining and persistent
adapter state, while keeping SDL out of the event-system target. Its injected batch test covers
foreign-window filtering plus ordered resize, Unicode and quit delivery. See
[SDL_EVENT_PUMP.md](SDL_EVENT_PUMP.md) and
[PORT_READINESS.md](PORT_READINESS.md) for evidence.

## Parallel renderer-candidate slice: SDL GPU

`frontierx_render_submission` is an SDL-free interface target containing the first validated
position/BGRA8/UV submission shape, topology and optional U16-index view. Its standalone probe tests
layout, bounds and primitive counts; a Direct3D-free adapter maps the stable primitive values and
exact `D3DFVF_RPVERTEX` byte shape into caller-owned typed storage. The SDL GPU probe is its first
backend consumer. The production `PrimitiveBuilder` now expands the fixture to a direct triangle
list, crosses the real `IRenderPipeline` vtable and registered
`frontierx_native_render_primitive` adapter, and supplies the exact data uploaded, presented and
read back by SDL GPU in the same process. The same owner validates the
equivalent U16-index submission, while the CPU probe additionally covers `RPVertex` layout, quad
expansion, callback failure propagation and explicit triangle-fan rejection. The GPU lifecycle is
owned by `frontierx_sdl3_gpu_renderer`; the native shell can select it explicitly and submit the
shared `frontierx_render_smoke_scene` through the same boundaries. Six depth/blend states and
checked CPU vertex-buffer draws are connected; the actual `RenderBatcher`, RPVERTEX2 GPU and
texture-stage/material paths remain unconnected. See
[RENDER_SUBMISSION_PORTABILITY.md](RENDER_SUBMISSION_PORTABILITY.md).

The optional `frontierx_sdl3_gpu_probe` target is gated by `FRONTIERX_BUILD_SDL3_GPU`, requires the
existing SDL3 option, and has no dependency on the game, D3D9 renderer or retail data. CMake invokes
the system `glslc` to compile two repository-owned GLSL test shaders into the
out-of-tree generated directory; the backend has no direct Vulkan-loader dependency. It creates
SDL's Vulkan device and window swapchain, builds a graphics pipeline, uploads seven vertices, nine
U16 indices and a 2-by-2 texture, samples with depth test/write and source-alpha blending enabled,
submits/presents twelve frames and recreates the depth target after an observed swapchain resize. It
also renders to a bounded 64-by-64 target, waits on a GPU
fence, downloads the pixels and validates clear/background/foreground structure. GCC and Clang
visible probe and integrated-shell runtime checks passed on 2026-09-21 with 2,496 changed pixels and
diagnostic checksum `7951318278861886233` for the shared scene on the local Vulkan driver.

This edge proves the local device/upload/draw/present lifecycle, representative depth/blend mapping
and its explicit shell composition, not a selected game backend or complete state/material
integration. Its registered test uses a transient non-focusable window and skip return code 77 when
a host has no graphical/GPU runtime; runtime-capable shader, pipeline, upload, or submission failures
remain failures. See
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

The supported App-side `RenderBatcher.cpp` compiles as
`frontierx_render_batcher_object` under both GCC and Clang. A focused linked
process executes its direct and U16-indexed draw branches against the native CPU submission
owner; the shell visibly presents direct, indexed and one alpha-depth queued-pool fixture through that
same batcher and observes bounded GPU readback. Additional pool modes, production
global/diagnostic ownership, production texture/material loading and Windows comparison remain
open. See
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

The public `WindowManager.h` declaration now compiles as part of the existing
`frontierx_dacom_interface_probe` under GCC and Clang. The non-Windows branch uses the shared
pointer-width legacy-message types and explicit 32-bit compatibility scalars.
`frontierx_native_window_manager` now supplies the separate SDL-backed DACOM owner, and
`frontierx_native_window_manager_probe` verifies its lifecycle through the real native manager. See
[WINDOW_MANAGER_PORTABILITY.md](WINDOW_MANAGER_PORTABILITY.md).

`frontierx_native_shell` is the first executable consumer of that integration target. It composes
the existing `frontierx_platform`, `frontierx_dacom_core`, production event owner and native window-manager library,
uses the headless checker's shared configuration/data-root owner, validates the required directory
shape, loads the shared transactional preference owner, creates the real `IUserDefaults`, event and
window services, persists the fixed-width settings schema, pumps through `IWindowManager`, and exits
on close, quit or a bounded smoke deadline. This is an application
lifecycle edge, not a game target or renderer edge. See [NATIVE_SHELL.md](NATIVE_SHELL.md).

`frontierx_native_user_defaults` includes the production application interface header but not the
Win32 implementation. Its probe exercises the unchanged vtable, live-default mapping, immediate
value/MRU transactions, schema restart and explicit unsupported UI behavior. The compiler-owned
legacy struct remains runtime-only; game-startup selection and Windows ABI validation remain open.

The adjacent `EventSys.cpp` source was then reviewed as the next owner. Its portable queue contract
is implemented in `frontierx_platform` and tested independently: callback-time posts remain in the
current FIFO dispatch batch, while `Peek` exposes the last completed batch. The production source
now also builds as `frontierx_event_system`; its real aggregate factory, DACOM maps, connection
point, direct send and queue execute through `frontierx_event_system_probe`. That probe uses the
real `ICOManager` interface provided by `frontierx_dacom_core`, including priority/fallback,
enumeration, references and shutdown. The EventSystem ELF has a measured shared-heap closure;
broader owner-specific heap evidence, the full
Hotkey module and native Windows validation remain open. See
[EVENT_SYSTEM_PORTABILITY.md](EVENT_SYSTEM_PORTABILITY.md) and
[DACOM_MANAGER_PORTABILITY.md](DACOM_MANAGER_PORTABILITY.md).

The measured four-field `MSG` consumer surface now has an isolated SDL compatibility adapter.
It retains pointer-width window/parameter storage, reconstructs only the numeric messages and bit
fields actually read by controls and hotkey code, and offers synchronous dispatch so temporary
message payloads cannot escape. It is linked only into the SDL platform target and probes; the
Windows Hotkey project and application graph remain unchanged.

## DACOM pointer-width follow-up

The 2026-09-17 audit migrated the public component-map, connection-point, resource, and dispatch
templates away from `U32` address arithmetic. A neutral helper target characterizes
multiple-inheritance adjustment, member-owner recovery, and the deterministic tokens used to keep
the established `IDAConnectionPoint` `U32` handle ABI without returning a node address. This
removes all five previously measured public-header pointer truncations.

The Linux graph now contains a native `ICOManager` core, production event/profile service owners,
and an explicit ELF component-module loader. The dynamic path and configuration-relative
`[Libraries]` bootstrap are wired into the native shell and proven with both a controllable fixture
and the production EventSystem ELF module. `frontierx_native_dacom_bootstrap` now supplies the real
process-global `DACOM_Acquire` symbol, registers both production profile factories once and owns the
manager used by the shell. Its registered process-local `IHeap` supplies the public bootstrap and
tracked allocation contract. The EventSystem ELF now uses that exact owner across `dlopen`; broader
module ownership and legacy COMHeap behavior remain open. The explicit DACOM
project still owns the supported Windows DLL loading and profile integration. The separate 32-bit MSVC automation dispatch is referenced only by four
legacy Viewer tool headers, not by component creation. The public `DACOM.H`, `TComponent.h`, and connection-point template layer now has a real GCC/Clang
executable probe, including an instantiated `DAComponent` map and connection container. It executes
point lookup, owner recovery, subscription enumeration, removal, and deterministic token reuse.
The next graph step is another game- or renderer-owned component with the same ELF heap/symbol proof,
native Windows rebuild and ABI/layout evidence. See
[DACOM_RUNTIME_PORTABILITY.md](DACOM_RUNTIME_PORTABILITY.md).

The graph also compiles production `ProfileParser.cpp` as `frontierx_profile_parser`. Its two real
factories and aggregate execute through the native manager, including typed buffer and native
transactional file initialization. The owning `ProfileDocument` is now a focused reusable target;
the manager uses its raw-line traversal for bounded file/string `[Libraries]` configuration rather
than aggregating the service parser. The global bootstrap registers both production factories;
writable INI behavior and cross-module proof for further owners remain open. See
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

`frontierx_native_heap` implements the unchanged `IHeap` surface for process-local native startup,
and `frontierx_native_dacom_bootstrap` registers it before the profile factories. Its probe covers
public initialization, allocation/reallocation, metadata, enumeration, foreign-pointer rejection
and repeated initialization. The production EventSystem ELF additionally proves aggregate and
queue-node ownership through the host heap, exact final accounting and unload. Descriptor capacity
is diagnostic, private COMHeap pass-through slots are unsupported, and further owners need their
own proof. See [NATIVE_HEAP.md](NATIVE_HEAP.md).

The graph now also compiles the production `DOCUVIEW` scanner, parser,
symbol-table, file-stream and `DataParser` sources as `frontierx_view_parser_core`. The
linked `frontierx_view_parser_probe` uses the native heap and parses a
generated Win32-shaped record through those production sources. It verifies
four-byte `long` and pointer fields, a 12-byte record, four-byte alignment
and offsets 0/4/8. It then enumerates those fields through the real
`IDataParser`/`IParserCallback` vtables; a nested 16-byte record also
creates a child parser for its embedded record and enumerates its inner
fields before a trailing field at offset 12. GCC and Clang focused CTests passed, as did a focused
Clang ASan/UBSan run with LeakSanitizer disabled. The parser owner initially
failed ASan because `Symbol` used heap allocation with standard delete; its
Linux release now returns memory to the same heap. Native scanner tokens now
use `calloc` because the production parser frees them with `free`; the check
observes exact final native-heap capacity after nested parser teardown.
The same generated parser input now checks the shared viewer symbol rules
for recursive equality, unequal records and a zero-length trailing array;
the Windows viewer forwards its corresponding methods to those rules but
has not been rebuilt in this Linux run. The graph now links the production
`ViewCnst.cpp` with its native scalar/bitfield/enum value-copy owner as
`frontierx_view_constructor`. The existing parser probe creates the real
factory and `IViewConstructor2`, compares independently parsed same and
changed schemas, correlates reordered fields and nested record arrays,
exercises bitfield and enum values, creates `IDataParser` through the
constructor, rejects the unavailable native GUI factory, and observes exact
heap balance after unregister. The native shell registers the constructor;
its generated five-service profile succeeds, while the checked-in
`Conquest.ini` reports five successful components and stops at `IStreamer`.
The shell also registers the already compiled production `TextureLibrary`
factory in its GPU configuration. A generated six-service profile including
that owner passes `GameSystem::startup` in the dummy SDL process suite under
GCC and Clang. An explicitly diagnostic copy of `Conquest.ini` with only
`IStreamer` omitted reports six successful creations and next stops at
`LightManager`; the original profile and audio service remain unchanged.
The subsequent production `LightManager` object, native factory and linked
CPU shading check now execute in GCC and Clang Debug. A generated seven-service
profile also passes native shell startup. The checked-in profile still stops
first at `IStreamer`; see [GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).
The linked game-material check now executes the same LightManager through
production PolyMesh and observes different submitted vertex colors under
two ambient settings. Its first sanitizer run found and led to removal of
a one-past-end strided normal-index read. The GPU shell now reuses a
profile-owned TextureLibrary instead of adding a duplicate, so the generated
seven-service profile presents both the older-format PolyMesh and CQ2 App
scenes; GCC, Clang and focused sanitizer evidence is recorded in the
startup document.
The production `VertexBufferManager` now has a shared object target used by
its existing lifecycle check and the GPU shell. Its native factory matches
the Windows aggregate shape, so an eight-service generated profile loads
it through the App system container. The scene initializes it after
pipeline binding and cleans up before unbinding. GCC, Clang and focused
sanitizer visible PolyMesh runs retain 496 changed pixels and checksum
`3422507570980372643`. With only IStreamer omitted, the checked-in
source profile now advances through eight successful services and stops at
`IStringTable`; see [GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).
These are synthetic constructor and startup results, not `MScript`
execution, an audio implementation or retail data. See
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).

Both native EventSystem targets compile with `DA_HEAP_ENABLED`. The static library has an explicit
transitive dependency on `frontierx_native_heap`; its production probe and SDL integration acquire
that owner before factory creation and verify final block-count balance. The ELF imports the host's
exported `HEAP` instead of embedding a second allocator.

`frontierx_hotkey_event` now compiles the production `HKEvent.cpp` service with the same native
heap. `frontierx_hotkey_event_probe` registers its real factory, supplies fixed-width data through
the public `IFileSystem` vtable and executes keyboard, mouse, joystick, callback, malformed-input
and exact allocation-balance paths. `NativeUtfFileSystem` provides the bounded read-only adapter
used to parse both checked-in source hotkey tables. The probe also connects raw EventSystem input
to the hotkey service and its translated output back through the production queue. The native shell
offers this composition through explicit `--hotkeys FILE` and advances `ISystemComponent::Update`
after each window batch. This does not add `HkeyRec.cpp`, installed-resource policy, a complete
DOSFile/UTF owner or a game target. See
[HOTKEY_EVENT_PORTABILITY.md](HOTKEY_EVENT_PORTABILITY.md).

`frontierx_dynamic_library` supplies a move-only Linux ELF handle with local eager symbol
resolution. `NativeComponentManager` now layers explicit C-named registration and liveness entry
points over it, attributes registrations, reference-counts repeated exact-path adds and removes
factories before close. Its synthetic module probe covers failure, lookup, live-object refusal,
reload, configuration rollback and shutdown. The production EventSystem uses this path with
instance-counted unload gating and runs from the native shell; campaigns and further components do
not. Heap ownership and broader ABI contracts remain owner-specific. See
[DYNAMIC_LIBRARY_PORTABILITY.md](DYNAMIC_LIBRARY_PORTABILITY.md).

## Stack-local debug guard follow-up

Six guards shared by mission and object-wrapper owners previously depended on x86 `esp` and
32-bit `this` arithmetic. They now use the header-only `FrontierX/StackAddress.h` boundary, which
has concrete consumers in `MGroupRef.h`, `MPart.h`, and four `Objwatch.h` specializations. The
isolated target verifies the pointer-width, stack-direction-independent distance contract without
pulling an application owner into CMake.

This removes six pointer-truncation and inline-assembly candidates but does not make the mission or
Trim closure portable. Those owners still require their surrounding DACOM/Win32 dependencies and
native Windows ABI/runtime regression. See
[STACK_ADDRESS_PORTABILITY.md](STACK_ADDRESS_PORTABILITY.md).

## Runtime callback boundary follow-up

The remaining measured pointer truncations crossed four existing boundaries rather than one new
generic subsystem. ParticleEffect and Conquest now share the pointer-width `ParticleContext` for
the existing callback context. Trim and DAHOTKEY share pointer-width legacy-message aliases with
the SDL adapter. The Windows-owned RichEdit and DirectShow calls now use their documented
`DWORD_PTR` and `LONG_PTR` fields. The helper probes and source regressions remove the measured
object/address-cast category without adding any of these Windows modules to CMake.

The required next graph evidence is therefore native Windows cross-module validation followed by a
real Linux owner closure, not another isolated alias. The affected virtual interfaces, callbacks,
and synchronous payload lifetimes are detailed in
[RUNTIME_CONTEXT_PORTABILITY.md](RUNTIME_CONTEXT_PORTABILITY.md).

## Native render-pipeline vertex-buffer follow-up

The existing render-submission graph now includes `frontierx_native_render_pipeline` and the
deterministic `frontierx_render_pipeline_probe` CTest. The public `rendpipeline.h` declaration is
includable on Linux through fixed-width non-Windows declaration types, and the native owner derives
from the real `IRenderPipeline` interface. A default adapter returns `GR_NOT_IMPLEMENTED` for every
unported slot; implemented direct/state methods forward to the registered `IRenderPrimitive`. The
CPU target also models the
`RenderBatcher` lock/slice/draw pattern: its context-aware lookup obtains a caller-owned
vertex-buffer view, and the same two draw entry points used by production inspect that view's FVF,
validate the requested direct or U16-indexed slice, preserve slice-relative indices and forward
portable RPVertex-shaped vertices through the already tested `IRenderPrimitive` boundary. The
portable contract and adapter also handle direct and U16-indexed 32-byte
`D3DFVF_RPVERTEX2` slices with validated list and line-strip topologies.

`frontierx_native_render_primitive` now links that adapter and owns it when its native-only
descriptor supplies a context-aware buffer lookup. Its existing `IRenderPrimitive`
vertex-buffer methods therefore execute the same checked route instead of returning
`GR_NOT_IMPLEMENTED`. The adapter now also owns CPU buffers through production-shaped
create/lock/unlock/destroy methods. Nonzero monotonic `U32` tokens keep runtime addresses out of the
legacy handle field; lock state gates draw and destruction. The registered-owner regression creates,
fills and destroys one of these resources and routes its indexed draw through separate typed
RPVertex/RPVERTEX2 callbacks. This proves native resource ownership, owner construction, vtable
routing and shutdown. The regression additionally invokes the repository smoke scene through the
production `PrimitiveBuilder(IRenderPipeline *)` constructor and real pipeline vtable. It does not
execute the production game batcher.

On 2026-09-21, `cmake --preset`, `cmake --build --preset ... -j2` and `ctest --preset`
completed for both `linux-gcc-debug` (GCC 16.2.1) and `linux-clang-debug` (Clang 22.1.8)
on Linux x86_64. Each full preset executed 31 passing tests and skipped `portability.sdl3_gpu`;
the skip is not GPU runtime evidence. Both build presets now explicitly include the pipeline
probe. A fresh Clang ASan/UBSan build also passed the two focused renderer CPU checks.
Exact commands and remaining Windows/retail gates are in
[RENDER_SUBMISSION_PORTABILITY.md](RENDER_SUBMISSION_PORTABILITY.md).

The same registered owner now also links an SDL-free `RenderStateSnapshot` for six existing depth
and alpha-blend values. `set_render_state` validates a complete candidate and commits it only after
the backend callback succeeds; the focused submission regression covers set/get, unsupported and
invalid values, callback propagation and rollback. The SDL GPU callback maps the snapshot into a
replacement graphics pipeline, including all eight D3D9 depth comparisons and eleven directly
mappable blend factors. On 2026-09-21 both full presets again executed 31 passing tests and skipped
only the non-required GPU CTest. After the real pipeline vtable was connected, separate required
visible GCC and Clang GPU-probe and native-shell runs used Vulkan and again agreed on 2,496 changed
pixels with checksum `7951318278861886233`. Probe diagnostics identify
`primitivebuilder-native-irenderpipeline`, and both shells report the production builder via
`IRenderPipeline`. Those four runs are synthetic Linux hardware evidence; Windows, retail,
RPVERTEX2 GPU and actual `RenderBatcher` evidence remain open.

On 2026-09-23 the existing `frontierx_render_batcher_header_probe` gained a second
translation unit for the production `MyVertex.h`/`CQTrace.h`/`CQImage.h` include
order and the 32-byte `Vertex2` stride. The original `FDump.h`-first unit remains
separate because combining these orders triggers `CQTrace.h`'s duplicate error-kind
guard. GCC and Clang configured, built and executed this target and their full
presets. Neither translation unit compiles the App-side `RenderBatcher.cpp` object;
the exact remaining errors and validation limits are in
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

The graph now also builds the real App-side `BMPoolJr.cpp` in
`frontierx_bmpooljr_probe`, registered as `portability.bmpooljr`. Its synthetic
arena ownership, growth, bounds and teardown checks execute under both
compiler presets, with a separate Clang ASan/UBSan run. It remains an isolated
production owner; the full ZBatcher translation unit and game graph are not
linked. Focused commands and limitations are in
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

The existing `frontierx_render_batcher_header_probe` also compiles the real
`CopyVertexBufferDesc.cpp` owner and executes its equal-format AOS and indexed
PN/color/UV copies on
repository-owned synthetic vertices. GCC and Clang built and ran that path,
and a focused Clang ASan/UBSan execution passed. The broader vertex-buffer
manager and App-side ZBatcher objects remain outside the native graph; the
focused document records the exact commands and open branches.
