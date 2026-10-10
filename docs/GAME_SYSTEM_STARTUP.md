# Native GameSystem startup boundary

Current playable mission startup, 2026-10-10: the active native
GameSystem/App composition loaded lawful external `Sol_Train`, kept the
source-profile `IStreamer` bound for Music/SFX/Speech/MSpeech and omitted
full-screen `IVideoSystem` only with explicit `--skip-movies`. Original
mission/global/script, object/input/offline operation and simulation owners
ran through a normal mouse-selected Forger move. GCC and Clang separate
visible Vulkan processes completed 299/298 source updates respectively, training
objectives 1 and 2, and a source mission save/reload preserving the moved
player unit and both flags. Other optional media/UI exclusions and exact
runtime evidence are in [the focused acceptance record](NATIVE_SINGLEPLAYER_SLICE.md#playable-solarian-training-checkpoint-2026-10-10).
The source `CQDoModal` render/scene contract remains the next broader-loop
gate; unit voice and teletype visuals are incomplete startup-facing
presentation contracts. Native Windows/ABI and retail parity are open.
Entries below are historical snapshots.

Current first-frame startup, 2026-10-10: the original external `Sol_Train`
mission loads a player-owned Forger and runs `CQE_GAME_ACTIVE` plus one
`CQE_UPDATE` under GCC and Clang visible Linux GPU processes. Source camera
initialization and native `IRenderPrimitive::set_perspective` now allow 28
production draws to reach a visible SDL/Vulkan frame. The source-profile
`IStreamer` is required and initialized. `--skip-movies` explicitly omits
only full-screen `IVideoSystem`; the diagnostic run separately excludes
unsupported IAC training speech and optional presentation layers. Source
`Mission::Save` and `Mission::Load` now preserve the player Forger's static
owner and position in an isolated native save under both compilers. No
normal input loop, move or post-move save/reload has run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-perspective-and-multi-draw-mission-frame-2026-10-10).
Entries below are historical snapshots.

Current external mission startup, 2026-10-10: original `Planetoid.cpp` and
relative Windows-style archive child paths resolved the earlier planet
placement failure. GCC and Clang visible native processes each completed
`Mission::Load`, counted one player-owned Forger, sent `CQE_GAME_ACTIVE` and
completed one `CQE_UPDATE`. Required source-profile `IStreamer` initialized;
default SDL-only decoding rejects IAC training speech, which the diagnostic
run explicitly disabled along with portrait/subtitle presentation. Explicit
`--skip-movies` alone omits full-screen `IVideoSystem`. The current render
pass made no GPU submission because the source Forger projected outside the
pane; normal input and save/reload remain open. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-planet-owner-game-active-update-and-first-object-render-blocker-2026-10-10).
Entries below are historical snapshots.

Current mission startup result, 2026-10-10: GCC and Clang visible
Wayland/Vulkan external-data runs now link the original Jumpgate, Waypoint,
Trigger, Gunboat and build-support Platform registrations along with Forger.
The present `Mission::Load` aborts when a `Cocoonr` platform has no planet
instance to park around; `Planetoid.cpp` is not linked. Required
source-profile `IStreamer` remains active, while only explicit `--skip-movies`
omits full-screen `IVideoSystem`. No source update loop or gameplay has run.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-mission-object-chain-through-platform-placement-2026-10-10).
Entries below are historical snapshots.

Current production startup result, 2026-10-10: the native source-profile
`IStreamer` remains required and initialized, while explicit `--skip-movies`
omits only full-screen `IVideoSystem`. GCC/Clang visible external-data
`Sol_Train` runs now link the original Forger factory/ship closure, complete
`Mission::Load`, and count one Forger in the production object list. A
validated native UTF archive index and case-insensitive `IFileSystem` lookup
made the Forger mesh available. The next source `CQE_GAME_ACTIVE` transition
aborts in `MScript::GetPartByName` for `Wormhole to Kasse (Solar)` because
the original jumpgate provider is absent. No update/render/input loop or
save/reload executed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-forger-instance-and-next-script-blocker-2026-10-10).
Entries below are historical snapshots.

Current object-owner gate, 2026-10-10: the original Forger factory source
builds as a native GCC/Clang object, but a production shell trial link failed
on original ship/movement templates and App services. Its registrar has not
run, and the preceding full mission load still has no Forger. Required
`IStreamer` remains in the source profile; full-screen `IVideoSystem` is
excluded only by explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#forger-object-compile-and-first-production-link-inventory-2026-10-10).
Entries below are historical snapshots.

Current factory boundary, 2026-10-10: original `Fabricator.cpp` owns the
static startup registrar needed by the production ObjectList, but is not in
the native shell link. Its native source closure now reaches instantiated
selection templates under direct GCC/Clang checks and still fails to compile.
The existing GameSystem/mission-load runtime result below remains the last
external-data execution; no new factory or gameplay ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#forger-factory-source-closure-remains-open-2026-10-10).
Entries below are historical snapshots.

Current mission startup, 2026-10-10: the active GameSystem, App global
startup and original `Mission::Load` complete on lawful external `Sol_Train`
under GCC and Clang visible Vulkan. Required `IStreamer` is bound to Music,
SFX, Speech and MSpeech. Native App mission-facing sound/teletype owners
provide the script's saved-ID contract; unsupported playback/display requests
fail explicitly. `IVideoSystem` is omitted by `--skip-movies`, and inherited
menu music by separate `--skip-mission-music`. The source in-game event is
sent before load to initialize fog dimensions. Missing original object
factories leave the production list without its Forger, so the shell exits 6 before the
game loop. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-sol_train-load-completion-before-playable-objects-2026-10-10).
Entries below are historical snapshots.

Current production script boundary, 2026-10-10: the native shell's active
GameSystem and original `Mission::Load` reach 88% on external `Sol_Train`
under GCC and Clang visible Vulkan. Original App `VertexBuffer.cpp` and
`scripting.cpp` now join global startup. The selected `Sol_T` source program
is registered, but original SoundManager and Teletype aggregates remain
uninitialized, so `MScript::Load` aborts before script execution. Required
`IStreamer` ran; `IVideoSystem` was explicitly omitted by `--skip-movies`.
No unit, mission frame or save/load ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#external-sol_train-load-reaches-the-script-boundary-2026-10-10).
Entries below are historical snapshots.

Current production mission load, 2026-10-10: the native shell links the
original Sector and SpaceEnvironment registrars, completes App global
startup, initializes source particle/mesh/texture/material services and
calls `IMission::Load` on external `Sol_Train.dmission`. GCC/Clang visible
runs reached 25% ObjectList loading and stopped at an Anim2D texture
assertion after logging three missing object factories. Explicit options
omit the GDI sector map, background effects, unit labels and full-screen
movies; required `IStreamer` executed. No playable mission or save/load
result exists. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-sol_train-load-through-objectlist-2026-10-10).
Entries below are historical snapshots.

Current `Sector` source closure, 2026-10-10: direct GCC/Clang syntax
checks now reach the original `Sector` class after native include and
declaration corrections, but fail in the Win32 sector-map UI and event
contract. The native shell still lacks the `Sector` registrar, so
`CreateGlobalComponents` cannot complete. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#sector-source-closure-inventory-2026-10-10).
Entries below are historical snapshots.

Current Sector prerequisite, 2026-10-10: the original terrain-map owner
now compiles and links into the GPU shell under GCC and Clang, but its
factory has not executed. The production `Sector` registrar and its Linux
UI/resource dependencies still block `CreateGlobalComponents`, before
`Mission::ReloadMission`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-terrainmap-compile-boundary-for-sector-2026-10-10).
Entries below are historical snapshots.

Current production App startup, 2026-10-10: the explicit visible
`--mission-start` path now calls `CreateGlobalComponents` with the active
GameSystem's required App aggregates bound. GCC and Clang lawful external
training-data runs initialized required `IStreamer`, parsed the App primary
schema, and aborted at the missing production `Sector` registrar during App
light initialization. Only full-screen `IVideoSystem` was explicitly skipped.
The mission reloader, simulation and save/load did not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-app-startup-attempt-and-sector-blocker-2026-10-10).
Entries below are historical snapshots.

Current mission Engine composition, 2026-10-03: the shell's explicit
`--mission-engine` path passes the supplied source profile to the original
`IEngine::load_engine_components`, queries its production `IRenderer`,
and binds them plus source-profile `IRenderPipeline` and
`IRenderPrimitive` as native App globals for the shell lifetime. The
render interfaces are not yet GPU-backed.
GCC and Clang ran that path on lawfully external training data. The
synthetic profile without a renderer fails explicitly when this mode is
requested. App `CreateGlobalComponents` and mission reload still do not
run; GPU binding for the profile render interfaces and remaining globals are the next startup
boundary. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-engine-composition-2026-10-03).
Entries below are historical snapshots.

Current App camera boundary, 2026-10-03: the GPU shell now links the
production App mission, simulation, input and camera objects and original
`Sol_T` script under GCC and Clang. Linux excludes the Windows-only debug
data viewer and menu,
keeps the camera behavior and subscribes its event callback to the active
EventSystem. The native shell binds the source hotkey service to `HOTKEY`
and uses the production `CQE_HOTKEY` message. Neither the camera registrar
nor the App global startup sequence has executed in the shell. The selected
external mission remains unopened by `Mission::ReloadMission`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-camera-and-hotkey-boundary-2026-10-03).
Entries below are historical snapshots.

Current App startup closure, 2026-10-03: the native shell now links its
2D App draw owner and PrimitiveBuilder, while temporary GCC/Clang
combined links include the selected production mission/script objects
without unresolved symbols. Both combined executables completed the
existing lawful external-data shell path but did not call
`CreateGlobalComponents` or `Mission::ReloadMission`. The real App
camera and further service globals remain the next startup blockers.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-2d-and-combined-mission-link-2026-10-03).
Entries below are historical snapshots.

Current native renderer handoff, 2026-10-03: the shell's native
primitive descriptor now binds checked ortho and viewport operations
to SDL GPU; GCC/Clang CPU contract and full Debug checks passed, and
separate visible Vulkan runs verified viewport clipping. The
production Camera/DrawAgent 2D owners are still absent from the shell,
and `Mission::Reload` has not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-orthographic-and-viewport-contract-2026-10-03).
Entries below are historical snapshots.

Current native profile and App-owner boundary, 2026-10-03: the shell
retains the mounted Profiles archive as App `PROFILEDIR`. GCC and Clang
executed `CreateProfileParser` through the production `IProfileParser2`
on a synthetic file in the existing shell check; both external
`Sol_Train` shell runs still reported `mission not started`. Original
Blast, light, MapGen, MorphMesh and fleet-pool code now compiles as
native objects, but those objects have not run through
`CreateGlobalComponents` or `Mission::Reload`. The App light owner
explicitly aborts if its unsupported D3D9 dynamic/ambient light state
is requested. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-light-profile-and-fleet-closure-2026-10-03).
Entries below are historical snapshots.

Current native App SFX owner, 2026-10-03: the shell now constructs and
retains a native `ISFX` after the required source-profile `IStreamer`
starts. GCC/Clang shell runs exercised the source-mapped `BUTTONPRESS1`
effect from lawful external training data through FFmpeg and SDL dummy
audio. Positional playback remains an explicit unsupported call, and
`Mission::Reload` has not run. Full-screen `IVideoSystem` and the
notification cue were omitted only with explicit options. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-isfx-and-field-object-closure-2026-10-03).
Entries below are historical snapshots.

Current mission SFX source binding, 2026-10-03: the native shell now
retains the external SFX filesystem with Music and the required
`IStreamer`; a missing SFX source fails mission-source startup. GCC and
Clang shell processes loaded lawful external training data but still
reported `mission not started`. The full-screen `IVideoSystem` and the
notification cue were omitted only with explicit options. `ISFX`
playback remains the next production contract. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#external-sfx-source-binding-2026-10-03).
Entries below are historical snapshots.

Current App field-owner boundary, 2026-10-03: original field, App 2D
animation and dust owners now compile under GCC and Clang, but are not
yet linked into an executing mission. The combined App/script link
still lacks the production SFX and other dependencies; the external
training-data shell continues to report `mission not started`.
`IStreamer` initializes as a required profile service. Full-screen
`IVideoSystem` is omitted only with explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#field-and-animation-owner-compile-boundary-2026-10-03).
Entries below are historical snapshots.

Current App system binding, 2026-10-03: the native shell now queries
the source-profile `IParticleManager` from its active GameSystem and
retains it in the App global for the mission-source path. Both GCC and
Clang process checks and lawful external training-data shell runs
exercised this binding. `Conquest.cpp`'s later `InitParticles` call and
`CreateGlobalComponents` remain unexecuted, so no mission tick ran.
Required `IStreamer` still initializes; full-screen `IVideoSystem` is
omitted only with explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-command-tracker-and-app-particle-binding-2026-10-03).
Entries below are historical snapshots.

Current mission-owner closure, 2026-10-03: the selected external
training mission has no saved player-AI directories, and the native
factory aborts if AI creation is requested. The production
`UnbornMeshList` compiles but has not executed through App startup.
`Mission::Reload` still cannot link because 23 App/service symbols
remain; `COMMTRACK` is the next production owner. Required
`IStreamer` and explicit `--skip-movies` remain the media policy. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-mission-ai-and-app-render-owner-boundary-2026-10-03).
Entries below are historical snapshots.

Current save/startup boundary, 2026-10-03: the native shell can bind a
writable `SAVEDIR` only with explicit `--save-root` outside the retail
data root; the platform UTF writer supplies `IUTFWriter::Flush`. The
existing generated create/flush/reopen check now passes with GCC and
Clang after a first-flush temporary-path fix. This does not execute
`Mission::Save` or `Mission::Reload`; the combined App/script link
still has 27 missing symbols with the existing native primitive
builder included. `IStreamer` remains a required initialized source
profile service. The full-screen `IVideoSystem` omission still
requires explicit `--skip-movies`; the selected pre-combat movement
checkpoint does not require a movie. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#writable-save-flush-and-current-mission-link-check-2026-10-03).
Entries below are historical snapshots.

Latest save-format gate, 2026-09-27: the native platform layer can
encode deterministic UTF 1.01 bytes from a bounded directory tree;
the existing data-root check reopens those bytes through the
production read-only `IFileSystem` interface under GCC and Clang.
`Mission::SaveAs` still lacks the writable `SAVEDIR` parent and
`IUTFWriter`/flush owner, so neither mission save nor reload ran.
The combined App/script link remains at 28 missing symbols. Required
`IStreamer` and explicit `--skip-movies` policy are unchanged. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-utf-save-format-encoder-gate-2026-09-27).

Latest mission-message gate, 2026-09-27: the shell creates native
`SCROLLTEXT` alongside `STATUS`/`HINTBOX`. An explicit
`--skip-notification-cue` suppresses the optional original beacon
sound; a message without it or a real sound owner aborts with a
diagnostic. Text is still terminal-only, and unsupported
player-attributed chat aborts. Both compiler shells ran with lawful
external `Sol_Train.dmission`, but `Mission::Reload` did not run;
28 combined-link symbols remain. Required `IStreamer` still starts,
and full-screen `IVideoSystem` is disabled only by `--skip-movies`.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-scrolling-message-and-explicit-cue-policy-gate-2026-09-27).

Latest App text-resource gate, 2026-09-27: the shell binds native
`STATUS` and `HINTBOX` to the active EventSystem and source App string
table. Both retain the production `IResourceClient` connection and
ownership contract, but currently display text in the terminal rather
than the SDL window. GCC/Clang external mission shells executed their
startup and still reported "mission not started"; 29 combined-link
symbols remain. `IStreamer` is required and initialized; full-screen
`IVideoSystem` is disabled only by explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-status-and-hint-resource-client-gate-2026-09-27).

Latest mission-load progress/version gate, 2026-09-27: the native
shell now creates a functional text `IPANIM` owner that validates
source progress strings and tracks percent, while native build version
comes from source `Conquest.rc`. GCC/Clang external mission shells
executed owner startup only; `Mission::Reload` remains unlinked with
31 missing symbols. `IVideoSystem` is still disabled only by explicit
`--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-load-progress-and-source-build-version-gate-2026-09-27).

Latest packet-selection link gate, 2026-09-27: the native
`dwordsearch` equivalent now satisfies the production packet/object
set callers in the combined diagnostic link. GCC/Clang offline packet
checks ran; `Mission::Reload` still cannot link because 33 production
symbols remain absent. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-packet-selection-search-gate-2026-09-27).

Latest App localization gate, 2026-09-27: the native shell links the
source `Globals.rc` English strings and executes both App string lookup
variants under GCC and Clang with the lawful external training mission.
The original `IStringTable` aggregate is separate from these App RC
resources. The mission remains unstarted; the combined App link still
lacks 34 production symbols. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-app-string-resource-link-gate-2026-09-27).

Latest production-music-owner gate, 2026-09-27: the native shell now
creates the original App `MusicManager` after `IStreamer` device
initialization and binds it to the event system. The owner and its
fixed-width Linux music save/load branch build under GCC and Clang;
both external training-mission shells executed owner startup and
teardown but still reported "mission not started". The optional
full-screen `IVideoSystem` skip remains explicit. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-music-manager-owner-startup-gate-2026-09-27).

Latest mission audio-ownership gate, 2026-09-27: the native shell now
retains the source-profile `IStreamer` and lawfully external `Music`
source for the App mission path, then initializes SDL audio with the
live window callback. Missing required streamer or music source fails
the shell explicitly. GCC/Clang executed the external selected-mission
shell using SDL dummy audio; `MusicManager` and the mission still do
not run. Optional full-screen `IVideoSystem` remains disabled only by
explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-mission-audio-ownership-gate-2026-09-27).

Latest music save-layout gate, 2026-09-27: the selected mission's
176-byte record now has a checked native disk/runtime conversion,
with four stale Win32 stream handles cleared. The existing parser
check executed it on the lawful external mission under GCC and Clang.
The production `MusicManager` still is not linked or executing in the
shell; its `Load`/`Save` caller must consume this conversion and the
required native `IStreamer`. Optional full-screen `IVideoSystem`
remains explicitly disabled only with `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-music-save-layout-conversion-gate-2026-09-27).

Latest selected-music gate, 2026-09-27: the FFmpeg-enabled native
`IStreamer` now decodes and briefly plays the selected 15-minute
Solarian IAC track under GCC and Clang with SDL dummy audio; the
previous 64-MiB PCM limit rejected its 160448512 decoded bytes.
Short `STMG03.wav` speech still completes. The mission carries a
176-byte Win32 music record, so `MusicManager::Load` needs a fixed-width
conversion before native handles can be restored. The shell initializes
required `IStreamer` but does not yet run the App music manager or
mission; optional full-screen `IVideoSystem` is disabled only by
explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-training-music-decode-and-disk-layout-gate-2026-09-27).

Latest selected-system-map gate, 2026-09-27: a native implementation
of the existing `ISystemMap` interface now owns the selected training
mission's empty fresh-load state and supplies zero undrawn-map timing.
The existing mission-filesystem check executed `Load` on the lawful
external mission under both compilers. The shell links the registrar
but still does not call App global startup or `Mission::Reload`;
37 other distinct symbols block the combined mission link. Required
`IStreamer` initialized in both external shell runs, and optional
full-screen `IVideoSystem` was disabled only by explicit
`--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-system-map-fresh-load-gate-2026-09-27).

Latest combined mission-link gate, 2026-09-27: statically linked native
`Sol_T` now uses the production host `HEAP` owner. A temporary GCC
shell/App/script link no longer has a heap multiple definition, but
still lacks 38 distinct production symbols, led by map, scrolling-text,
music, status and hint owners. The executable was not produced, so
`Mission::Reload` and `MScript::Load` did not run. The previously
validated required `IStreamer` and explicit optional full-screen
`IVideoSystem` policy are unchanged. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#combined-mission-script-heap-link-gate-2026-09-27).

Latest selected-script localization gate, 2026-09-27: source `Sol_T.rc`
now supplies native counted strings through a build-local `llvm-rc`
resource. The production `MScript` fresh-load branch validates the
selected script, schema, parser, strings and required managers, rejecting
unsupported saved-script reload. Both compilers checked representative
objective/subtitle strings and the lawful external mission's `Sol_T.dll`
and zero update count. The shell still does not link or run `MScript`;
required `IStreamer` initializes, while optional full-screen
`IVideoSystem` is disabled only by explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-localization-and-fresh-loader-gate-2026-09-27).

Latest script-owner build gate, 2026-09-27: the production `MScript.cpp`
object now compiles under GCC/Clang but is not linked to the native
shell. Its native mission-loader and script-localization entry points
abort explicitly while their required contracts remain unimplemented;
no mission starts. External shell runs still initialize required
`IStreamer`, while `--skip-movies` explicitly disables optional
full-screen `IVideoSystem`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-mscript-owner-build-and-required-resource-gates-2026-09-27).

Latest selected-script resource gate, 2026-09-27: the native `Sol_T`
object target now carries a compiler-generated parser resource with a
stable process lifetime. The production parser consumes its embedded
bytes in the existing check, but `MScript::Load` and mission startup
remain outside the shell. Both external-data shell runs initialized
required `IStreamer`; explicit `--skip-movies` disabled only optional
full-screen `IVideoSystem`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-native-schema-resource-gate-2026-09-27).

Latest selected-program build gate, 2026-09-27: the source `Sol_T`
program and helper build as native objects, and generated script schema
bytes pass the production parser under GCC/Clang. This does not register
the programs with `MScript` in the shell or start the external mission.
Required `IStreamer` still initializes in both external-data shell
runs; optional full-screen `IVideoSystem` is disabled only by explicit
`--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-program-and-schema-build-gate-2026-09-27).

Latest selected-script data gate, 2026-09-27: the original `Sol_T`
data header builds in the native layout check with a 292-byte
`MissionData` and fixed training-unit offsets. A read-only check of
the lawfully external mission found no `Script/GlobalData`, so the
first start requires fresh native campaign-program registration after
object loading. Neither `MScript` nor that program runs in the shell.
Required `IStreamer` and the explicit `--skip-movies` policy for
optional full-screen `IVideoSystem` remain unchanged. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-script-data-and-fresh-start-gate-2026-09-27).

Latest training-script ABI gate, 2026-09-27: source review of
`Mission::Reload` and `Sol_T` found `MPartRef` inside saved script
global data. The native type now retains its 16-byte Win32 layout;
the GCC/Clang layout checks pass. The mission still cannot run because
the production `MScript` and native campaign registration are not
integrated. The external shell's required `IStreamer` and explicit
optional `IVideoSystem` policy are unchanged. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#training-script-mpartref-save-layout-gate-2026-09-27).

Latest spatial-iterator build gate, 2026-09-27: `ObjMapIterator`,
which the production `ObjectList` and `MScript` callers use for mission
object traversal, now compiles with `ObjMap` under GCC and Clang. It
does not run in the native shell. Both external-data shell runs still
initialize required `IStreamer`, explicitly disable optional
full-screen `IVideoSystem` with `--skip-movies`, parse
`Sol_Train.dmission`, and report "mission not started". See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-spatial-iterator-build-gate-2026-09-27).

Latest camera-manager integration gate, 2026-09-27: production
`MovieCameraManager` is in the GCC/Clang shell link. The existing
offline App check executes its global registration, real EventSystem
mission-close callback, 44-byte camera save/load round trip and
teardown, but the shell does not yet call
that registrar or start the mission. Both lawfully external training
mission shell runs mounted ten archives and initialized required
`IStreamer`; `--skip-movies` explicitly disabled only the optional
full-screen `IVideoSystem`. Both parsed the mission source and reported
"mission not started". See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-movie-camera-manager-gate-2026-09-27).

Latest camera-template gate, 2026-09-27: the shared App object
templates required by `MovieCamera.cpp` now compile under GCC and
Clang, but the camera owner remains blocked by Win32 UI/editor code
and is not part of the successful shell. Both external training
mission shell runs initialized required `IStreamer`, explicitly
disabled optional full-screen `IVideoSystem` with `--skip-movies`,
and stopped before mission startup. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#camera-object-template-closure-2026-09-27).

Latest App selection-ring table gate, 2026-09-27: the original
`StacticInit.cpp` executes under GCC and Clang in the existing math
check, but `BaseObject` still has not drawn a mission selection ring.
The combined App link lacks 51 distinct symbols. Both lawfully
external training-mission shell runs initialized required `IStreamer`,
explicitly disabled optional full-screen `IVideoSystem` with
`--skip-movies`, and stopped before mission startup. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-selection-ring-table-gate-2026-09-27).

Latest App simulation event-owner gate, 2026-09-27: the original
`EventScheduler.cpp` and native `SCHEDULER` slot build under GCC and
Clang, but remain outside the successful shell. The combined App
link still lacks 52 distinct symbols. Both external
`Sol_Train.dmission` shell runs initialized required `IStreamer`,
explicitly disabled optional full-screen `IVideoSystem` with
`--skip-movies`, and stopped before mission startup. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-event-scheduler-build-gate-2026-09-27).

Latest App spatial-object map gate, 2026-09-27: the original
`ObjMap.cpp` now builds under GCC and Clang but remains outside the
successful shell. A combined App link resolves `OBJMAP` and still
lacks 53 distinct symbols. Both external training-mission shell runs
initialized required `IStreamer`, explicitly disabled only optional
full-screen `IVideoSystem` with `--skip-movies`, and stopped after
mission schema parsing. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-spatial-object-map-build-gate-2026-09-27).

Latest fog-owner build gate, 2026-09-27: the original
`FogOfWar.cpp` owner now builds under GCC and Clang but is not yet
linked into the successful native shell. A combined App link still
lacks 54 distinct production symbols, and external
`Sol_Train.dmission` runs stop before mission startup. `IStreamer`
remains required and initializes; only optional full-screen
`IVideoSystem` is explicitly disabled by `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-fog-of-war-native-build-gate-2026-09-27).

Latest App nugget-owner build gate, 2026-09-27: the production
`NuggetManager` and its object factory compile under GCC and Clang,
but remain outside the linked shell while 53 distinct combined App
symbols are unresolved. Both external `Sol_Train.dmission` processes
still initialize required `IStreamer`, explicitly disable only
optional full-screen `IVideoSystem` with `--skip-movies`, and stop
before mission startup. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-nugget-manager-native-build-gate-2026-09-27).

Latest App cursor-service gate, 2026-09-27: the native shell now links
the platform-backed `ICursorResource` registrar, while its focused
process executes real SDL system cursors under GCC and Clang on the
visible Wayland host. The shell has not called the combined App global
startup or started `Sol_Train.dmission`. External shell runs still
initialize required `IStreamer` and explicitly disable only optional
full-screen `IVideoSystem` with `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-cursor-service-2026-09-27).

Latest object-owner integration gate, 2026-09-27: the native shell now
reads its configured SDL window client area through the production
`IWindowManager` and binds the App's actual/ideal screen coordinates.
The real `BaseObject.cpp` compiles beside `ObjList.cpp` under GCC and
Clang, but neither owner is linked into the successful shell. External
`Sol_Train.dmission` processes still stop before mission startup.
Required `IStreamer` initialized; optional full-screen `IVideoSystem`
was disabled only by `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-base-object-owner-in-the-native-link-graph-2026-09-27).

Latest offline mission-registrar gate, 2026-09-27: the native `MISSION`,
`ARCHLIST` and `PARSER` slots are available for their production owners;
`Mission.cpp` now rejects missing required schema or event setup and remote
file transfer, while permitting an offline mission without the network
callback. The registrar remains outside the linked shell. GCC/Clang
external runs initialized required `IStreamer`, explicitly skipped only
optional full-screen `IVideoSystem` with `--skip-movies`, and stopped
after parsing `Sol_Train.dmission`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-offline-mission-registrar-contract-2026-09-27).

Latest offline packet-dispatch gate, 2026-09-27: a focused GCC/Clang
process now runs production `NetPacket` and `NetBuffer` through the
active EventSystem callback and separate private heaps. The shell
still does not link their App registrars or start `Sol_Train.dmission`.
External runs initialized required `IStreamer` and disabled only
optional full-screen `IVideoSystem` with explicit `--skip-movies`.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-offline-packet-dispatch-2026-09-27).

Latest local packet execution gate, 2026-09-27: a focused GCC/Clang
process now runs the production `NetBuffer` registrar and offline
queue with native DACOM and real user defaults. The shell still does
not link `NetBuffer` or `NetPacket` and does not start
`Sol_Train.dmission`. External shell runs initialized required
`IStreamer`; explicit `--skip-movies` disabled only optional
full-screen `IVideoSystem`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-local-packet-buffer-build-gate-2026-09-27).

Latest packet-buffer gate, 2026-09-27: the production `NetBuffer` local
queue now builds under GCC and Clang, but neither it nor `NetPacket`
is linked into the shell. External `Sol_Train.dmission` startup still
stops after schema loading. The required `IStreamer` initialized in
both external runs; only optional full-screen `IVideoSystem` was
disabled by explicit `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-local-packet-buffer-build-gate-2026-09-27).

Latest operation-owner build gate, 2026-09-27: production `OpAgent.cpp`
compiles as a GCC/Clang object target and its registrar now targets the
real native EventSystem callback point. The combined App mission link
still lacks player/host, packet and other required services; the shell
does not call this owner. External training data stops after schema
loading. `IStreamer` is required and initializes; only optional
full-screen `IVideoSystem` is explicitly disabled by `--skip-movies`.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-operation-owner-native-build-gate-2026-09-27).

Latest shared App-startup gate, 2026-09-27: the same
`AddToGlobalStartupList`/`CreateGlobalComponents`/`CleanupGlobals`
contract used by Windows now compiles and links into the native shell.
Its reverse two-phase order and release were executed in a focused
GCC/Clang check. The shell still has no linked mission/object
registrars, and the external training mission stops after schema
loading. `IStreamer` remains required; only optional full-screen
`IVideoSystem` is explicitly disabled by `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#shared-app-global-startup-and-cleanup-owner-2026-09-27).

Latest mission-owner build gate, 2026-09-27: `Mission.cpp` now builds as
an original App object target with GCC and Clang, but no App global
startup or mission service closure is linked into the shell. External
Steam-source shell runs still end after schema loading with no mission
started. The required `IStreamer` aggregate initializes; optional
full-screen `IVideoSystem` is disabled only by `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-mission-owner-native-build-gate-2026-09-27).

Latest object-owner build gate, 2026-09-27: the native App binding now
retains the active `IWindowManager` alongside `IEventSystem`; the shell
checks their production aggregate identities. `ObjList.cpp` compiles as
a GCC/Clang object target, but its temporary shell link exposed a large
required App closure, so no mission input ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-object-owner-build-and-link-closure-2026-09-27).

Latest external GameTypes gate, 2026-09-27: the native shell retains a
validated read-only `gametypes.db` source through its event loop and
exposes it to the native `ObjectList::loadTypesData` branch. GCC and Clang
opened the selected Forger type record, but `ObjectList` is not linked;
the mission remains unstarted. The production profile still requires
`IStreamer`; only optional full-screen `IVideoSystem` is disabled by the
explicit `--skip-movies` switch. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-gametypes-source-binding-2026-09-27).

Latest native App event-owner gate, 2026-09-27: the mission-source
shell path now keeps `EVENTSYS` bound to the active production system
container through the window event loop and checks its identity against
the initialized aggregate. The native `ObjectList` source subscribes
to that event callback point, but the owner is not yet linked and no
mission input ran. `IStreamer` remains required, and only full-screen
`IVideoSystem` remains explicitly disabled by `--skip-movies`. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-object-list-event-owner-contract-2026-09-27).

Latest object frame-clock source gate, 2026-09-27: the production
`ObjList.cpp` source now routes native frame-counter calls to the
existing monotonic platform clock, while the Windows path remains.
Direct GCC/Clang checks next expose required event, nugget, camera and
scheduler owners; `ObjectList` is still unlinked. Separate external
shells loaded the selected mission schema but did not start it. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-native-frame-clock-source-contract-2026-09-27).

Latest object-debug source gate, 2026-09-27: the optional Windows
performance report and editor dialog are excluded from native
`ObjList.cpp`, and direct GCC/Clang checks now reach event ownership
and frame timing. The
object owner still does not compile, link or start the training mission.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-debug-and-clock-declarations-2026-09-27).

Latest object-owner source gate, 2026-09-27: production `ObjList.cpp`
now reaches its implementation body under direct GCC/Clang syntax checks,
but Win32 debug/UI/timing paths still block compilation and it remains
outside the native shell. No mission startup or unit creation ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-native-source-boundary-2026-09-27).

Latest mission-object input gate, 2026-09-27: the selected external
training mission exposes Forger, Taos and Acropolis QuickSave records to
the native `IFileSystem` traversal required by `ObjectList::Load` under
GCC and Clang. `IStreamer` still runs as a required audio aggregate;
only optional full-screen `IVideoSystem` is explicitly disabled by
`--skip-movies`. External shell runs parsed the mission schema but did
not invoke `Mission::Reload` or create objects. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-quicksave-object-input-boundary-2026-09-27).

Latest mission-owner compile gate, 2026-09-27: the real `Mission.cpp`
source now passes its initial App service declarations and Win32
disk-space helper under GCC/Clang, using a checked native platform
service for the Linux save preflight. The owner still fails syntax
compilation at later App services and is unlinked. The successful
external-data shell stops at schema parsing, not mission startup. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-save-space-platform-contract-2026-09-27).
Entries below are earlier gates.

Latest production global-owner compile gate, 2026-09-27: `MGlobals.cpp`
and its `Objwatch.cpp` pointer service build as a GCC/Clang native object
target. A full-owner shell link remains blocked by real App services,
chiefly the mission, event, archetype and localization owners. The
successful shell still opens external mission data only through the
schema stage. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#complete-mglobals-compile-and-link-closure-2026-09-27).
Entries below are earlier gates.

Latest App-global source boundary, 2026-09-27: native `MGlobals.cpp`
now draws its simulation `globalValues` directly from the production
`GENDATA` owner after excluding the optional Win32 development viewer.
`New` and `Load` fail if that required record is missing or wrong-sized.
Direct GCC/Clang syntax checks still fail on later App service dependencies;
the owner has not linked or run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mglobals-global-values-source-contract-2026-09-27).
Entries below are earlier gates.

Latest App general-data gate, 2026-09-27: both compiler shells now bind a
real `IGeneralData` from the production `GenData.cpp` owner using the
lawfully external `gendata.db`. The native UTF enumeration now supplies
the file sizes required by that owner; `GlobalValues` is 1640 bytes in
the loaded database. `MGlobals.cpp` still does not compile or execute, so
mission global-record loading and gameplay remain open. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-general-data-owner-2026-09-27).
Entries below are earlier gates.

Latest App-global compile gate, 2026-09-27: the real `MGlobals.cpp`
translation unit now reaches its body under both native compilers, then
fails on the App global owner and Win32 `GlobalViewer` dependencies.
Source review confirms `Conquest.cpp` calls `CreateGlobalComponents` from
`System.cpp` before the mission's global-data load; the native shell does
not yet reproduce that App lifecycle. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mglobals-native-owner-compile-boundary-2026-09-27).
Entries below are earlier gates.

Latest mission ParseData gate, 2026-09-27: after the unchanged source
profile creates the App view constructor and required `IStreamer`, both
compiler shells use that parser to load the selected external mission's
secondary schema through `SPMAPDIR`. `IVideoSystem` remains explicitly
disabled for full-screen movies. `Mission::LoadParseData` calls the same
Linux adapter in source, but the App mission owner is still unlinked; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-parsedata-owner-contract-2026-09-27).
Entries below are earlier gates.

Latest native App resource gate, 2026-09-27: after the unchanged source
profile creates `IStreamer` and the view constructor, both compiler shells
bind the active App system container and parse the embedded `data.i` through
that production aggregate before opening the external mission source.
Only full-screen `IVideoSystem` is explicitly disabled by `--skip-movies`;
audio and animation remain required. The parser now fails on malformed
native input; `Mission::ParseDefFile` and App gameplay still do not run.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-parser-resource-binding-2026-09-27).
Entries below are earlier gates.

Latest App resource gate, 2026-09-27: the production view parser parses
the repository `data.i` named by `Globals.rc` under GCC and Clang, with
3236-byte `MT_GlobalData`. `Mission::ParseDefFile` still depends on the
Win32 resource loader and has not run natively; direct source compilation
now reaches that body and exposes the missing App globals. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#repository-app-schema-and-mission-source-body-2026-09-27).
Entries below are earlier gates.

Latest App/script declaration gate, 2026-09-27: the shared command-packet
enum and native script registration layout now compile under GCC and Clang.
Both external-data shells still complete GameSystem startup with required
`IStreamer` active and only optional full-screen `IVideoSystem` disabled by
`--skip-movies`; `Mission::ReloadMission` has not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#app-declarations-and-script-registration-abi-2026-09-27).
Entries below are earlier gates.

Latest App mission-owner compile gate, 2026-09-27: the public `IMission`
header compiles under GCC and Clang, while direct `Mission.cpp` compilation
stops at its Windows-only Zone lobby RPC dependency and `MGlobals.cpp` stops
at the Windows PCH. GameSystem profile startup remains executable, but App
mission startup has not run. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-owner-native-compile-boundary-2026-09-27).
Entries below are earlier gates.

Latest App data-source gate, 2026-09-27: the native shell binds the real
`Objects`, `Maps/Campaign` and material filesystems to the App directory
globals after unchanged `GameSystem::startup`. GCC and Clang external-data
processes opened the selected mission through `SPMAPDIR` and created and
destroyed the Forger Engine/MeshManager instance through the bound sources,
then exited 0. This still does not execute `CreateGlobalComponents` or
`Mission::Reload`; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-mission-source-bindings-2026-09-27).
Entries below are earlier gates.

Latest global-record gate, 2026-09-27: the selected external mission's
`MT_GlobalData` record now passes production view-constructor correlation
and a checked 3236-byte disk/native/disk adapter under GCC and Clang.
Linux-only calls were added at the source `MGlobals::Load`/`Save` owner;
that App translation unit has not yet compiled in the native graph or run.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-global-data-format-and-owner-call-2026-09-27).
Entries below are earlier gates.

Latest mission-data gate, 2026-09-27: the native UTF filesystem now accepts
the root-absolute backslash paths used by the real `Mission::LoadParseData`
and `MGlobals::Load` callers. GCC and Clang opened those paths from the
lawful external `Sol_Train.dmission`, and the production view constructor
parsed its `MT_GlobalData` declaration. This is retail-input data/parser
execution only; `Mission::Reload`, global-record correlation and the App
startup closure have not run. The 3236-byte parsed global record differs
from the 3812-byte native `MT_GlobalData` layout. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-loader-paths-and-global-schema-2026-09-27).
Entries below are earlier gates.

Latest Engine-instance gate, 2026-09-27: the existing shell diagnostic now
continues from the external Forger archetype to production
`IMeshManager::CreateMesh`/`IEngine::create_instance2` and destroys the
result. GCC and Clang each observed a live Engine root during that instance
and none afterward, with 13 face groups and exit 0. No App-owned game unit,
camera, mission or rendering was exercised. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-forger-engine-instance-2026-09-27).
Entries below are earlier gates.

Latest post-startup mesh gate, 2026-09-27: after the unchanged source-profile
`GameSystem::startup`, the native shell can initialize the original game
texture, material and mesh managers and request the Forger archetype from
external Steam data. GCC and Clang each returned 13 face groups and exited 0. This is a
bounded diagnostic of the same manager contracts that `Conquest.cpp` calls;
`CreateGlobalComponents`, the App camera, mission and simulation remain
unexecuted. Exact validation is in
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-forger-mesh-archetype-2026-09-27).
Entries below are earlier gates.

Latest animation-data gate, 2026-09-27: the native shell registers the real
`IChannel2` decoder and can ask the profile-created `IAnimation` owner to
load an object script. GCC and Clang both loaded the external Forger
`Sc_build` script (one script, 1.6 seconds) through the mounted `objects`
archive. No object instance or mission was created. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-channel-and-forger-animation-data-2026-09-27).
Entries below are earlier gates.

Latest production startup gate, 2026-09-27: the native shell links the original
Anim and HardPoint Engine components. GCC and Clang completed the unchanged
`Conquest.ini` `GameSystem::startup` on ten external Steam archives with
explicit movie skip and a bounded SDL dummy window (exit 0). The real
`IAnimation` and `IHardpoint` aggregates and the source-profile `Optics`
render component are created and torn down, but no animation script,
hardpoint archetype, particle effect, App global owner or mission was loaded.
Native `RenderMgr` now rejects a missing named render component. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-animation-and-hardpoint-startup-2026-09-27).
Entries below are previous gates.

Latest production-owner gate, 2026-09-27: GCC and Clang create the real
`IMeshManager` and Engine `IRenderer` from the unchanged source profile on
lawful external data with explicit movie skip; both next fail at required
`IAnimation` (exit 3). The linked MeshManager classes and a generated
profile's pre-init teardown execute in the existing shell check, but no
mission, simulation or save/load has run. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-mesh-manager-owner-2026-09-27).
Entries below record earlier gates.

Previous particle-owner gate, 2026-09-27: GCC and Clang now create the real
`IParticleManager` from the unchanged source profile on lawful external data
with explicit `--skip-movies`, then fail at required `IMeshManager` (exit 3).
The existing native-shell suite creates and releases a real point-emitter
program. This is startup/owner execution, not mission or gameplay runtime.
See [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-particle-owner-2026-09-27).
Earlier gate records below are historical snapshots.

Earlier optional-movie gate, 2026-09-27: `--skip-movies` explicitly sets the
production `CQFLAGS.bSkipMovies` guard and omits only native profile creation
of `IVideoSystem`. The selected training movement checkpoint uses audio and
Talking-Head animation, not full-screen movies; those services remain required.
With this option, GCC/Clang startup on external data now fails at
`IParticleManager`. Without it, missing video remains fatal. This is startup
execution and source-reviewed MovieScreen exclusion, not native mission or
MovieScreen execution. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#explicit-full-screen-movie-policy-2026-09-27).

Earlier audio gate, 2026-09-27: the optional shared-LGPL FFmpeg adapter now
decodes external `STMG03.wav` through native `IStreamer`. GCC and Clang
completed host PipeWire playback with identical decoded PCM fingerprints.
The default SDL-only build still rejects IAC explicitly. The unmodified
profile's next creation failure remains `IVideoSystem`; full App audio
initialization and mission integration remain open. No media service was
disabled and no mission ran. See
[NATIVE_AUDIO_STREAMER.md](NATIVE_AUDIO_STREAMER.md#optional-iac-decoder-2026-09-27).

Earlier material-source gate, 2026-09-27: `--material-root Data/mat` opens
the repository-owned CQ2 material directory through read-only `IFileSystem`.
With the external Steam `DATA` root, GCC and Clang pass data validation,
retain ten archive mounts and fail at `IStreamer` in the unmodified profile
(exit 3). No media service was disabled. Physical shader/material/TGA reads
also execute in the existing generated material test and visible CQ2 scene.
This does not assign the full App globals or load a mission. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#explicit-material-directory-source-2026-09-27).
Earlier entries below record the preceding gates.

Latest startup-data gate, 2026-09-27: the source check now validates packed
UTF files and the shell owns their `IFileSystem` mounts through startup.
The external Steam data root passes every required source except the
CQ2 `Mat` source. Material search/overlay composition remains unresolved;
the unmodified system profile still fails at `IStreamer` independently.
See [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#packed-startup-source-mounts-2026-09-27)
for exact mount validation and its limits.

Latest data gate, 2026-09-27: the native UTF filesystem now opens nested UTF
missions through the production `IFileSystem::CreateInstance` contract.
GCC/Clang executions opened external `Campaign/Sol_Train.dmission` and
enumerated its five root entries. The generated StringTable load exercises
the same nesting with independent child lifetime. Startup mounting and
the actual Mission caller remain open; see the final validation section.

Latest gate, 2026-09-27: `IMaterialManager` and `ITManager` now register
before profile startup, and the native scene reuses the profile-created
owners. The unmodified source profile remains blocked at `IStreamer`.
The user-supplied packed retail root fails an earlier directory-only data
check; the selected mission and media contract audit are recorded in
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md).

Status: 2026-09-27. The GPU-enabled Linux shell executes the production
`GameSystem::startup` inline caller with a synthetic profile, then draws the
repository CQ2 fixture through that same Engine. This is synthetic runtime
evidence. It does not start
`Conquest.cpp`, load a retail profile, or run the game simulation.

## Owner, caller and falsifiable blocker

The supported Windows entry point calls `GAME.startup` in
[`Conquest.cpp`](../Code/App/Src/Conquest.cpp). Its implementation in
[`gamesys.H`](../Code/Libs/Include/gamesys.H) creates the App
`ISystemContainer`, invokes `LoadSystemComponents`, starts its
`IWindowManager`, creates `IEngine`, and calls `load_engine_components` with
the system container. The native shell previously called the container
factory and its profile loader itself. That proved the container path but
left the actual `GameSystem::startup` caller unexecuted.

The local hypothesis was that the existing native App container, SDL window
manager and production Engine factory could complete this caller when given
synthetic `[System]` entries and an empty `[Engine]` section. The cheapest
disproof was the existing bounded shell process suite: it would fail to open
the SDL dummy window or reach normal teardown. The visible CQ2 shell fixture
then checked that the additional Engine lifetime did not disturb presentation
or the bounded image. No new parallel startup implementation or standalone
probe was added.

## Native composition and ownership

[`NativeGameSystem.cpp`](../Code/Launcher/NativeGameSystem.cpp) is a narrow
launcher adapter around the real inline `GameSystem::startup` and
`GameSystem::shut_down`. It exists so the shell can own the legacy
`GameSystem` without bringing `ENGINE.H` warnings into the strict shell
translation unit. Its GameSystem outlives borrowed event/window interfaces,
and its explicit shutdown releases the Engine before the App system
container. The GPU-enabled shell registers the production Engine factory
once before startup. The fixture scene borrows that Engine and loads its
`IRenderer` component after constructing the temporary GPU and material
services. No second Engine or Engine factory is created for the scene.

The shell's temporary profile contains `IEventSystem`, `IWindowManager`, and
an empty `[Engine]` section. The App container loads the two system services;
the native `IUserDefaults` component is created and its real `LoadDefaults`
method succeeds before this `GameSystem::startup` call. This matches the
relative owner order in `Conquest.cpp` while retaining native file-backed
settings. The component remains shell-owned and is not yet selected by the
actual game entry point.
`GameSystem::startup` starts the window and creates an Engine whose component
list is initially empty. On Linux, `ENGINE::load_engine_components` leaves that
empty load retryable, so the same Engine can consume the later synthetic
`[Engine] IRenderer` and `[RenderManager] PolyMesh` profile. This retry behavior
is scoped to Linux; the supported Windows one-time behavior is unchanged.
The scene holds an extra Engine reference while drawing. Its scope guard
releases the GameSystem reference before scene services are destroyed, then
releases the scene reference; later `GameSystem::shut_down` still shuts down
the App container. A failed scene load follows the same release order.
The no-GPU shell still uses the App container directly because the native
Engine and game-scene owners are currently linked only into the GPU graph.

`RenderMgr` and `PolyMesh` require the pipeline, primitive, texture library
and game material manager through their `system` argument. The scene
registers a short-lived factory for a fixture-specific aggregation component
and calls the production App `ISystemContainer::AddComponent` after the
pipeline and primitive exist. That component forwards those two resources.
The production `ITManager`, `IMaterialManager` and `TextureLibrary` are then
created through native `AddNativeSystemComponent` and held by the App
container. Their game-specific initialization still receives the generated
material archive, pipeline and texture manager explicitly; `TextureLibrary`
initializes with the container as its system outer and queries the pipeline
through it.
The scene checks all four forwarded pointers before passing the App container
to `IEngine::load_engine_components`; it also checks the container's
`ITManager` identity directly. The fixture factory is immediately
unregistered. Its component borrows two resources and clears those pointers
after the Engine releases its components, before the scene resources are
destroyed. Its inner `QueryInterface` checks for a live pointer before
`AddRef`, so a later request for a detached service returns unsupported.
The App container now removes the forwarding component after the Engine
releases its renderer components and before scene resources are destroyed.
The native-only removal uses the inner component's identity, accepts only
components added through `AddComponent`, unlinks the matching element and
updates the tail; the profile-loaded window and event aggregates remain.
This proves the real RenderMgr/PolyMesh system-service lookup path and App
ownership of the production game texture and material aggregates. Pipeline
and primitive creation remain scene-local.

The shared inline header now spells `SYSTEM.H` and `ENGINE.H` with their exact
filesystem case. It initializes factory result pointers, checks profile,
window and Engine-load results, and releases partial objects on failure.
It preserves the Windows `WNDCLASSEX` parameter spelling and aliases the
native parameter to the window manager's `tagWNDCLASSEXA` type. Direct
`COMPTR::ptr` comparisons avoid the old integer-to-pointer comparison on
LP64. These edits change failure handling in a header used by the supported
Windows build; native Windows Debug/Final rebuilds and affected module ABI
checks remain required. No persistent layout or simulation type changed.

## Validation

Executed on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j 4
ctest --test-dir out/build/linux-gcc-debug -R '^(portability.native_shell|portability.platform_inventory)$' --output-on-failure
cmake --build --preset linux-gcc-debug -j 4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j 4
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-shell-no-gpu --target frontierx_native_shell -j 4
ctest --test-dir /tmp/frontierx-shell-no-gpu -R '^portability.native_shell$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell -j 4
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.native_shell$' --output-on-failure
```

The GCC and Clang full Debug builds and presets passed. The focused shell
process suite passed under both compilers, in the fresh SDL3/no-GPU build,
and under Clang ASan/UBSan with LeakSanitizer disabled on this host. Its
GPU-enabled bounded run now asserts the explicit successful synthetic
`GameSystem` startup report. The inventory check traces the shell to the
adapter's call to the real `GameSystem::startup` and explicit shutdown.
The same process suite supplies an invalid SDL video driver: the GPU build
rejects it through `CreateGameSystem` with the game-system startup error,
while the no-GPU build rejects it at its direct window startup. Both paths
return before reporting an opened window.
Temporary process-test data directories contain no retail media.

The visible Wayland/Vulkan GCC scene reported successful loading of both
system services, synthetic `GameSystem` startup, 73 presented frames, 496
changed pixels and checksum `1466864583390317354`. The corresponding
focused Clang ASan/UBSan scene presented 96 frames with the same changed
pixels and checksum and no reported sanitizer finding. Its inputs were
repository-generated synthetic UTF material/mesh/texture fixtures and
compiled repository shaders. Command-sandbox GPU skips are not counted as
runtime execution.

After switching the fixture to the GameSystem-owned Engine, the visible GCC
scene presented 74 frames with the same 496 changed pixels and checksum. The
focused Clang ASan/UBSan scene presented 93 frames with that checksum and no
reported finding. GCC and Clang full Debug builds and CTest presets passed
again; the focused ASan/UBSan shell process suite passed with LeakSanitizer
disabled. These observations cover the successful fixture path and its normal
teardown, not retail startup or every injected failure path.

After routing the four service lookups through the App container, the visible
GCC scene presented 73 frames with the unchanged image. The focused Clang
ASan/UBSan scene presented 95 frames with the same checksum and no reported
finding. GCC and Clang full Debug builds and CTest presets and focused shell
process checks passed again. The resource-pointer checks executed before the
production Engine loaded its renderer component.
After replacing the forwarding component's nullable DACOM map with an explicit
null-safe inner `QueryInterface`, the visible GCC and focused Clang ASan/UBSan
scenes presented 72 and 99 frames with the same 496 changed pixels and
checksum and no reported sanitizer finding. A post-detach interface request
has source-review coverage; it was not separately executed in this run.

After adding targeted native `AddComponent` removal, GCC and Clang rebuilt
the App container and shell, passed their focused shell process checks, and
passed their full Debug CTest presets (40 checks executed, two GPU skips in
the command sandbox). A visible GCC Wayland/Vulkan run presented 72 frames,
496 changed pixels and checksum `1466864583390317354`. A focused Clang
ASan/UBSan visible run presented 98 frames with the same pixels and checksum;
the shell process check also passed under these sanitizers. No sanitizer
finding was reported, with LeakSanitizer disabled. Both runs used generated
repository UTF material, mesh and texture fixtures and compiled repository
shaders. Normal scene teardown exercised removal after the Engine released
its components. Injected removal failure and repeated scene creation in one
process were not executed.

After moving the production `TextureLibrary` into the App container, the
visible GCC Wayland/Vulkan scene presented 74 frames, 496 changed pixels and
checksum `1466864583390317354`. The focused Clang ASan/UBSan scene
presented 100 frames with the same image result and no reported sanitizer
finding, with LeakSanitizer disabled. GCC and Clang full Debug CTest presets
passed again (40 executed checks, two command-sandbox GPU skips each), as
did the focused ASan/UBSan shell process suite. The library was initialized
through the container's production `IRenderPipeline` query and was removed
before the pipeline and GPU resources were destroyed. Inputs were generated
repository UTF fixtures and compiled shaders; no retail media was used.

The production `IMaterialManager` and `ITManager` were then moved into the
same App container. Each is added by its registered production factory and
removed in reverse order after Engine teardown. The visible GCC Wayland/Vulkan
scene presented 71 frames, 496 changed pixels and checksum
`1466864583390317354`. The focused Clang ASan/UBSan scene presented 100
frames with the same image result and no reported sanitizer finding, with
LeakSanitizer disabled. These are synthetic runtime observations; a later
source cleanup removed the adapter's unused texture/material forwarding
branches and both compilers rebuilt the shell. The normal scene release path
was exercised; injected initialization/removal failures were not separately
run under GPU instrumentation.

After the explicit `ITManager` identity check and unused adapter-branch
cleanup, the final visible GCC scene presented 72 frames and the focused
Clang ASan/UBSan scene presented 99 frames, both with 496 changed pixels and
checksum `1466864583390317354`. No sanitizer finding was reported, with
LeakSanitizer disabled. GCC, Clang and sanitizer shell targets rebuilt.
Both full Linux Debug CTest presets passed again with 40 executing checks
and two command-sandbox GPU skips each; the focused sanitizer shell process
suite passed.

The next integration gate is to connect these container-owned resources to
the production `Conquest.cpp` startup cadence and retail profile, replacing
the temporary fixture adapter where the platform boundary allows it. The
GPU pipeline and primitive were scene-local owners at this stage.

The native user-defaults order was then aligned with `Conquest.cpp`:
GCC 16.2.1 and Clang 22.1.8 rebuilt `frontierx_native_shell` and their
full Linux Debug presets passed (40 executing checks and two command-sandbox
GPU skips each). The focused shell process suite passed with both compilers,
in a no-GPU GCC build, and under focused Clang ASan/UBSan with LeakSanitizer
disabled. Its bounded synthetic first-run checks assert that the live
`IUserDefaults::LoadDefaults` report precedes the production `GameSystem`
startup report, while the fixed-width settings file is still persisted at
shutdown. A visible GCC Wayland/Vulkan scene with repository-generated UTF
fixtures and compiled shaders presented 74 frames, 496 changed pixels and
checksum `1466864583390317354` after the change. No visible sanitizer scene,
Windows Debug/Final or ABI check, retail profile or retail runtime was
executed for this ordering change.

The App `CQPipeline`, retail `[System]` and
`[Engine]` profiles, production game-level `IUserDefaults`, lawfully supplied
retail media, Windows Debug/Final rebuilding, module ABI checks, retail
runtime and same-input Windows D3D9 image comparison remain unperformed.

## External profile and explicit native load failures (2026-09-27)

The native shell now accepts `--system-profile FILE` and passes its bytes to
the production `IProfileParser2` used by `GameSystem::startup`. The file is
read with a one-megabyte limit; missing, empty, oversized and unreadable
files fail with a path-redacted error. With no option, the existing synthetic
profile remains the startup input. The option is a way to exercise the real
loader with lawfully supplied configuration; it does not make every retail
component available or start a game scene from a retail profile.

Source review found that the active App `LoadSystemComponents` returned
`GR_OK` on Linux when no parser, no `[System]` section, a failed component
factory, or a failed aggregate initializer prevented a valid system load.
The native branch now returns a failure and leaves the container to be
released by `CreateGameSystem`. The Engine loader likewise rejects a failed
`[Engine]` component creation on Linux, while retaining its empty-first-load
retry for the synthetic scene. Native component-creation notices omit profile
names and descriptions; the exercised missing-factory paths do not echo a
supplied private path. The supported
Windows loader behavior and public interface layouts are unchanged; this is
not Windows runtime or ABI evidence.

The existing shell process suite generated an external profile with
`IEventSystem`, `IWindowManager` and an empty `[Engine]`; it opened and closed
the SDL dummy window. Separate generated inputs without `[System]`, with an
unknown system component, and with an unknown Engine component failed before
the opened-window report. A missing file and a file larger than one megabyte
also failed without printing the supplied path. GCC 16.2.1 and Clang 22.1.8
built the full Linux Debug presets and passed the focused shell suite and full
CTest presets (40 executing checks and two command-sandbox GPU skips each).
The no-GPU GCC shell and focused Clang ASan/UBSan shell suite passed. The
visible GCC Wayland/Vulkan synthetic CQ2 scene presented 72 frames, 496
changed pixels and checksum `1466864583390317354` after this change.
Passing the same generated `[System]`/empty `[Engine]` text through
`--system-profile /tmp/frontierx-profile-smoke.ini` also reached the visible
scene: 72 frames, 496 changed pixels and the same checksum. The shell
reported a supplied profile and did not print its path.
Inputs were generated in temporary directories or by repository fixtures;
no retail media was read or copied. A retail profile, real game startup,
native Windows Debug/Final and ABI checks, retail runtime and a same-input
Windows D3D9 game-scene comparison remain open.

Before profile aggregation was added, the checked-in source
[`Conquest.ini`](../Code/App/Src/Conquest.ini) was
passed to the shell without retail media, with and without an explicit render
mode. Both synthetic startup attempts returned code 3 after one system
component reported `[OK]` and the next reported `[FAILED]`, before a window
opened. Source inspection identifies that second `[System]` entry as
`IRenderPrimitive`. The registered native factory requires a
`NativeRenderPrimitiveDesc` with a live submission callback, while the App
profile loader supplies `AGGDESC`; registering the factory alone cannot
satisfy this contract. The native GPU/window also does not exist at this
point in `GameSystem::startup`. The source file's `[Libraries]` DLL list is
not passed to the native manager by `--system-profile`; module registration
is separately controlled by `--config`. The next production gate was therefore a
profile-loadable renderer owner with an explicit bind/lifetime step after
window startup, followed by the `IRenderPipeline` aggregate. This observation
is source review plus synthetic process execution against a checked-in
configuration, not a running game or a retail runtime result.

Focused commands for this profile boundary on Linux x86_64 included:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j 4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.native_shell$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_native_shell -j 4
ctest --test-dir out/build/linux-clang-debug -R '^portability.native_shell$' --output-on-failure
cmake --build /tmp/frontierx-shell-no-gpu --target frontierx_native_shell -j 4
ctest --test-dir /tmp/frontierx-shell-no-gpu -R '^portability.native_shell$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell -j 4
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.native_shell$' --output-on-failure
env SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --system-profile Code/App/Src/Conquest.ini --smoke-ms 1 --hidden
```

The supplied-profile visible command used the same generated UTF mesh,
material and shader paths as the earlier synthetic game-scene run, plus
`--system-profile /tmp/frontierx-profile-smoke.ini`. That generated profile
contains only the two currently available startup services and an empty
`[Engine]` section.

## Profile-created render primitive and deferred GPU binding (2026-09-27)

The next local blocker was the production App loader's `AGGDESC` for the
second `[System]` entry in `Conquest.ini`. The existing native primitive
factory accepted only a callback-bearing direct descriptor, and the SDL GPU
did not yet exist during `GameSystem::startup`. The falsifiable hypothesis
was that the same production primitive could first join the App system
container as a dormant aggregate, then acquire scene callbacks after GPU
initialization. The existing shell process suite and visible game-scene run
were the cheapest checks: profile creation had to pass before window startup,
and the bound object had to draw the unchanged fixture through the production
Engine. This extends the existing primitive owner and shell check.

`RegisterNativeRenderPrimitive` now registers direct and `AGGDESC` aggregate
factories under `IRenderPrimitive`. The aggregate validates its descriptor
and outer/inner interfaces without borrowing GPU callbacks. After SDL GPU
initialization, the scene queries `IID_IRenderPrimitive` from the App
container and binds the existing aggregate. If the profile omits it, the
scene still creates a direct primitive. A scene scope guard unbinds the
profile primitive after Engine teardown, clearing borrowed callbacks/context,
CPU vertex-pipeline ownership, cached states and transforms, while the App
container retains the aggregate. Callback binding constructs the vertex
pipeline before publishing borrowed pointers; allocation failure cannot
leave a stale scene context. The pipeline remains a temporary scene resource
forwarded by the fixture adapter. No persistent layout or public renderer
interface changed; native Windows module rebuilds and ABI checks remain open.

The generated external profile in the shell process suite now contains
`IEventSystem`, `IRenderPrimitive` and `IWindowManager`, with an empty
`[Engine]` section. The GPU build opens and closes the SDL dummy window;
the no-GPU build explicitly rejects the unavailable primitive factory. The
existing render-submission check expects both registered factories. GCC
16.2.1 and Clang 22.1.8 built the full Linux Debug presets and each passed
40 executed CTests; two GPU tests skipped in the command sandbox. The
no-GPU GCC shell process suite and focused Clang ASan/UBSan shell process
suite passed with LeakSanitizer disabled. The visible GCC Wayland/Vulkan
scene using that generated profile and repository-generated UTF fixtures and
shaders presented 69 frames, 496 changed pixels and checksum
`1466864583390317354`. A visible focused Clang ASan/UBSan run presented
one frame with the same 496 pixels and checksum and no reported finding;
LeakSanitizer was disabled. These are synthetic runtime observations.

Focused commands for this aggregate boundary on Linux x86_64 included:

```text
cmake --build --preset linux-gcc-debug -j 4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j 4
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-shell-no-gpu --target frontierx_native_shell -j 4
ctest --test-dir /tmp/frontierx-shell-no-gpu -R '^portability.native_shell$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell -j 4
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.native_shell$' --output-on-failure
env SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --system-profile Code/App/Src/Conquest.ini --smoke-ms 1 --hidden
```

The visible runs added `--system-profile /tmp/frontierx-profile-primitive.ini`
to `--game-scene` with the generated `game_material_gpu_fixture.utf`, its
`.vertexcolor.mesh`, and four compiled `sdl3_gpu_probe` shader inputs. The
generated external profile contained the three service names above; all
runtime data directories were temporary. No retail media was used.

The checked-in `Conquest.ini` now reports two successful system component
creations before the third fails and GameSystem startup returns code 3,
without opening a window. Source order identifies the third entry as
`IRenderPipeline`; that aggregate is the next concrete profile-loading
blocker. This is synthetic process execution with checked-in configuration,
not retail data or a game startup. It does not establish a native renderer
backend choice or Windows parity.

## Profile-created render pipeline and deferred binding (2026-09-27)

The next `Conquest.ini` blocker was `IRenderPipeline = RendSft`. The App
profile loader creates it before `IWindowManager` starts, but the existing
native pipeline constructor previously required a live primitive and GPU
texture callbacks. The local hypothesis was that the existing pipeline
owner could enter the App container unbound, then use the same checked
submission methods after the scene supplies those dependencies. The cheapest
disproof was an external four-service profile in the existing shell process
suite followed by the same visible generated UTF fixture: either creation,
service identity, drawing, or teardown would fail. No second pipeline
implementation or standalone probe was added.

The existing native render-pipeline probe now checks the binding lifecycle
that the visible shell run cannot isolate: an unbound draw fails, the first
bind forwards one production-interface draw, a duplicate bind fails, unbind
rejects subsequent draws, and a later bind/unbind succeeds. This permanently
protects the callback lifetime contract introduced by profile aggregation.
The focused `portability.render_pipeline` CTest passed under both GCC and
Clang. It uses repository synthetic vertices and a capturing primitive.

The `NativeRenderPipeline` owner now has an `AGGDESC` aggregate factory under
`IRenderPipeline`. Its profile component validates the descriptor and joins
the App container without a GPU pointer. The scene queries that interface
after SDL initialization and binds the existing primitive, texture callbacks
and vertex-constant callback. Its service identity check requires the
container query to return this same pipeline before the Engine loads
`RenderMgr` and `PolyMesh`. Profiles without the entry continue to use the
scene-local native pipeline. After Engine and App material/texture teardown,
a scope guard clears any remaining pipeline resources and borrowed pointers
before the GPU context expires. The profile aggregate remains container-owned.
The GPU callback failure is returned by the explicit unbind method; no
persistent layout, game simulation ordering, or Windows public interface
changed.

GCC 16.2.1 and Clang 22.1.8 rebuilt their full Linux Debug presets and
passed 40 executing CTests each; two command-sandbox GPU tests skipped per
preset. The generated external profile with `IEventSystem`,
`IRenderPrimitive`, `IRenderPipeline = RendSft`, `IWindowManager` and an
empty `[Engine]` section passed the GPU shell process suite. The no-GPU GCC
shell process suite rejected the unsupported render components, and the
focused Clang ASan/UBSan shell suite passed with LeakSanitizer disabled.
The visible GCC Wayland/Vulkan scene using that generated profile and
repository-generated UTF fixtures and compiled shaders presented 72 frames,
496 changed pixels and checksum `1466864583390317354`. The visible focused
Clang ASan/UBSan scene presented one frame with the same pixels and checksum
and no reported sanitizer finding; LeakSanitizer was disabled. The visible
command used the same `--game-scene` inputs as the primitive section above,
with `--system-profile /tmp/frontierx-profile-pipeline.ini`.

At this pipeline-only step the checked-in source `Conquest.ini` reported four successful `[System]`
component creations before the fifth fails and GameSystem startup returns
code 3 without opening a window. Source order identifies `IViewConstructor`
as the next blocker. These are synthetic process and runtime observations,
not a retail profile, game startup, Windows runtime or backend parity result.
Native Windows Debug/Final rebuilds, affected module ABI checks, a lawfully
supplied retail profile and retail runtime remain unperformed.

Source review identifies the production `IViewConstructor` owner in
[`ViewCnst.cpp`](../Code/Libs/Src/Viewer/DOCUVIEW/ViewCnst.cpp). It also
implements `IViewConstructor2` and works with the parser and symbol-table
sources in `DOCUVIEW`; it was not yet part of the Linux CMake graph. The
follow-on integration check needed to compile and register that production owner with
its actual parser dependencies, then verify its `AGGDESC` initialization and
at least one real caller before advancing further through `Conquest.ini`.
No placeholder component is treated as progress through this gate.

## View-constructor parser ABI gate (2026-09-27)

Source review of the production `IViewConstructor` and `MScript::SetScriptData`
caller found that the parser's `SymbolManager::set_reserved` previously used
host `sizeof(long)` when recording the size of a parsed `long`. On LP64 Linux
that would be eight bytes, changing script/save record offsets from the Win32
layout before `IViewConstructor2::ParseNewMemory` could be used safely. The
falsifiable local hypothesis is that the production scanner, parser and
symbol-table sources can compile and parse a small record with a four-byte
`long` and the expected following-field offset. The cheapest check is a
linked, generated-input parser check; the existing generic layout check does
not execute the production parser or its symbol table. A passing result
allows work to proceed to `ViewConstructor` aggregation; a failure requires
fixing the owning parser layout code before profile registration. This check
protects an ABI contract that cannot yet run through the full production
viewer/mission closure. It is not a replacement parser or game capability.

The existing `DOCUVIEW` scanner, parser, symbol-table and file-stream owners
now compile as `frontierx_view_parser_core` on Linux. Their Windows includes
are scoped to Windows and source include names use exact filesystem case.
`SymbolManager::set_reserved` records a parsed `long` using the fixed
four-byte Win32 width instead of LP64 host `sizeof(long)`; its `__int64`
width remains eight bytes. The parser already assigns four-byte size and
alignment to pointer declarations. This changes runtime parser metadata,
not the in-memory `Symbol` type, persisted bytes or a public module vtable.

The new focused check links those actual production sources and parses a
generated `struct NativeLayout` containing `long`, `void *` and `int`.
Under both GCC 16.2.1 and Clang 22.1.8 on Linux x86_64, it observed a
four-byte reserved `long`, a 12-byte record with four-byte alignment, and
field offsets 0, 4 and 8, with each field occupying four bytes. The input
is generated in code and contains no retail data. The check also exercises
the production symbol-table release path. A focused Clang ASan/UBSan run
first found that `Symbol::operator new` allocated through `HEAP` while
`Symbol::Release` used standard `delete`; a Linux-only class-specific delete
now returns the memory to `HEAP`. The same focused sanitizer check then
passed with no reported ASan or UBSan finding and LeakSanitizer disabled.
An attempt with LeakSanitizer enabled failed because LeakSanitizer cannot
operate under this host's ptrace environment; that run is not leak evidence.
A later explicit `IHeap` accounting check found that the native token
allocator used `IHeap` while the production parser releases tokens with
`free()`. The native token path now uses `calloc` to match those frees;
the supported Windows and Xbox selections remain as before. The first
accounting attempt also left the predefined symbol chain live because the
check released only its head. The check now follows the production
`ViewConstructor::Release` chain traversal, releases every predefined
symbol, and observes the native heap back at its initial available capacity
after nested parser and symbol teardown. GCC, Clang and focused Clang
ASan/UBSan checks pass this exact accounting invariant, with LeakSanitizer
still disabled for the sanitizer run.

Executed commands:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_view_parser_probe -j 4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.view_parser$' --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_view_parser_probe -j 4
ctest --test-dir out/build/linux-clang-debug -R '^portability.view_parser$' --output-on-failure
cmake -S . -B /tmp/frontierx-texture-handle-asan
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_view_parser_probe -j 4
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.view_parser$' --output-on-failure
```

Both complete Linux Debug builds and CTest presets also passed after the
addition; their rolling counts belong in [BUILD_GRAPH.md](BUILD_GRAPH.md).
At this parser-only point the production `ViewConstructor` aggregate, its `ParseNewMemory` method,
`DataViewer` correlation/viewer implementation, App `MScript` caller,
native Windows Debug/Final rebuilds, affected ABI checks and retail runtime
remained unexecuted. The checked-in `Conquest.ini` still stopped at
`IViewConstructor`; the parser check does not clear that startup gate.
The legacy parser target builds with `-Wall -Wextra -Wpedantic` without
`-Werror` while it is being ported. Current compiler diagnostics include
mutable string-literal conversions, a signed-`char` table index, an
unhandled-enum switch and a suspicious `=+` in the numeric scanner. No
pointer-truncation warning was observed in this target, but the other
diagnostics need owner-level review before the parser closure is treated as
retail-compatible.

## Production IDataParser enumeration (2026-09-27)

The production `ViewConstructor::CreateInstance` also creates `IDataParser`
objects for parsed symbols. Source review found that this object depends on
the same `DOCUVIEW` symbol table but only calls the viewer's small type
resolution helper; the viewer's Win32 controls are not needed for field
enumeration. The falsifiable next check was to create the real
`DataParser` from the already parsed generated record and enumerate fields
through its public `IDataParser` and `IParserCallback` vtables. If the sizes
or order differed from the symbol table, that caller boundary could not
support a native `ViewConstructor` factory. The existing parser check was
extended because it already owns this input and heap lifetime.

`DataParser.cpp` now compiles into `frontierx_view_parser_core`. On Linux it
resolves `TYPESYM`/`VARSYM` chains with the same rule as
`DataViewer::GetType`; the Windows path still calls the original viewer
method. The public callback declarations use `COMAPI`, which remains
`__stdcall` under MSVC, instead of an unguarded Windows spelling. A
Linux-only class delete returns `DataParser` allocations to the same
`IHeap` used by its class new. The factory helper clears its output for a
null symbol, avoiding a null dereference at the future aggregate boundary.

The generated record's production `IDataParser::Enumerate` callback visited
`first`, `pointer`, and `second` in order, reported four-byte sizes and
offsets 0/4/8, and supplied no nested parser for these scalar/pointer
fields. A second generated `NestedLayout` record occupied 16 bytes and its
`item` field supplied a child `IDataParser`; that child enumerated the three
inner fields before the trailing `int` at offset 12. This exercised nested
parser creation, callback use and release. GCC 16.2.1 and Clang 22.1.8 focused `portability.view_parser`
CTest runs passed; the same focused Clang ASan/UBSan run passed with
LeakSanitizer disabled and no reported finding. This is synthetic runtime
through the real DataParser vtable. At this DataParser-only point,
`ViewConstructor::CreateInstance`,
`IViewConstructor2::ParseNewMemory`, `MScript`, the Windows viewer/correlation
path, Windows Debug/Final and module ABI checks, and retail runtime remain
unperformed. The checked-in profile still stopped at `IViewConstructor`.

## IViewConstructor integration decision (source review, 2026-09-27)

`ViewConstructor::CreateInstance` owns both the profile aggregate and the
`IDataParser` factory. Its `ParseNewMemory`, `GetSymbol`, and
`DestroySymbols` methods use the parser/symbol sources now executing in the
focused check. `MScript::loadParseData`, `loadProgramData`, and
`loadGlobalData` call these methods through `IViewConstructor2`; the latter
two use `IsEqual` to select direct copy or `CorrelateSymbol` to migrate
record fields. Registering an aggregate that only parses would therefore
misrepresent the game's save-load path.

The remaining production source couples `ViewConstructor` to the Win32
`IViewer` factory and to `DataViewer::CopySymValue`. The pure
`GetType`, `IsEqual`, and `HasVariableSize` rules are now in the shared
`ViewSymbolOperations.h` source used by the Windows viewer; native
`DataParser` uses the same type-resolution rule. The existing parser check
now exercises recursive equality for a parsed record, inequality for a
different record, fixed-size decisions for both records, and a variable-size
decision for a parsed zero-length trailing array. At this point it did not
execute `ViewConstructor` or its correlation algorithm. The next
falsifiable gate was to compile and aggregate the production constructor with
all `IViewConstructor2` operations available, then execute a generated
same-schema and changed-schema record through its public interface. The
check must show a direct-copy equality decision, field correlation for a
changed schema, complete symbol and heap teardown, and rejection of the
unavailable native GUI factory. Only after that gate could the checked-in
profile advance past `IViewConstructor` to its next component. This was a
source-review integration decision, not implemented aggregate or game
runtime evidence.

The focused `frontierx_view_parser_probe` built and executed under GCC
16.2.1 and Clang 22.1.8 Linux Debug. The same target passed a Clang
ASan/UBSan execution with LeakSanitizer disabled. All schema text was
repository-generated; the Windows viewer translation unit was not rebuilt
on its supported host, so its forwarded methods have source-equivalence
review but no current Windows execution evidence. At that point, the native constructor,
`MScript`, retail data and game scene remained unexecuted in this check.

## Production ViewConstructor through the profile (2026-09-27)

The owner is `ViewCnst.cpp`; the real game consumers are the App system
container, `Mission.cpp`'s `IViewConstructor` query and `MScript.cpp`'s
`IViewConstructor2` save-load paths. The integration blocker was that
`ViewCnst.cpp` included Win32 viewer controls, while its value-copy helper
also lived inside the Win32 `DataViewer`. The falsifiable check was whether
that production constructor could compile, create an aggregate from an
`AGGDESC`, expose both public interfaces, compare independently parsed
schemas and correlate changed records without linking the GUI viewer. The
existing parser probe already held the generated schema and heap lifetime;
the shell process suite already exercised App `SysContainer` profile loading.
Passing this check permits the profile to advance to its next production
owner, `IStreamer`; it does not prove `MScript` or game startup.

`frontierx_view_constructor` now links the production `ViewCnst.cpp` with
the native `ViewSymbolCopy.cpp`. On Linux the constructor excludes the Win32
`IViewer` creation path and returns `GR_INTERFACE_UNSUPPORTED` for it; its
`IViewConstructor` and `IViewConstructor2` methods remain the production
methods. The native value-copy owner follows the viewer's scalar raw-copy,
signed/unsigned bitfield and enum-name migration rules with explicit byte
widths. The Windows constructor still calls `DataViewer::CopySymValue` and
retains its GUI factory. Shared type-resolution, equality and variable-size
rules remain in `ViewSymbolOperations.h`. The constructor's native class
delete returns its allocation to `IHeap`. The public enumeration callback
typedefs use `COMAPI`, preserving Win32 `__stdcall`; its internal reference
count remains 32-bit. No serialized type or public vtable slot changed.

The expanded `portability.view_parser` check registered the production
factory with DACOM, created `IViewConstructor`, queried
`IViewConstructor2`, and used `ParseNewMemory`, `GetSymbol`, `IsEqual`,
`CorrelateSymbol` and `DestroySymbols`. Two separately parsed identical
eight-byte schemas compared equal; reordered scalar fields compared unequal
and their two `U32` values moved to the expected new offsets. A second pair
of generated 24-byte records reordered two fields and an array of two
nested records; all six resulting `U32` values matched the expected
by-name mapping. Additional public-interface calls checked unsigned
cross-word and signed bitfield moves and enum remapping by constant name.
The constructor's `IDataParser` factory enumerated the reordered fields;
its native GUI factory rejected `IViewer`. After destroying every symbol
list, unregistering the factory and releasing its interfaces, available
native-heap capacity equaled total heap capacity. These are synthetic
runtime results from repository-generated input.

The existing `portability.native_shell` process suite now supplies a
five-service `[System]` profile and observes successful `GameSystem::startup`
and the dummy SDL window under both GCC and Clang. The checked-in source
`Conquest.ini` was also supplied directly to the GCC shell with the dummy
SDL driver: its first five `[System]` components returned `[OK]`, the sixth
returned `[FAILED]`, and startup exited with code 3 before opening a window.
The source profile order identifies the sixth component as `IStreamer`.
This is startup/profile evidence, not a native game scene or retail-profile
result. An existing GCC no-GPU build also rebuilt the shell and passed its
process suite, including expected rejection of renderer-dependent profiles.

Executed commands on Linux x86_64:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_view_parser_probe frontierx_native_shell -j 4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.(view_parser|native_shell)$' --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_view_parser_probe frontierx_native_shell -j 4
ctest --test-dir out/build/linux-clang-debug -R '^portability.(view_parser|native_shell)$' --output-on-failure
cmake -S . -B /tmp/frontierx-texture-handle-asan
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_view_parser_probe -j 4
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.view_parser$' --output-on-failure
env SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --system-profile Code/App/Src/Conquest.ini --smoke-ms 1 --hidden
cmake -S . -B /tmp/frontierx-shell-no-gpu
cmake --build /tmp/frontierx-shell-no-gpu --target frontierx_native_shell -j 4
ctest --test-dir /tmp/frontierx-shell-no-gpu -R '^portability.native_shell$' --output-on-failure
```

GCC 16.2.1 and Clang 22.1.8 full Linux Debug builds and CTest presets also
passed after this change; the repository-wide results are in
[BUILD_GRAPH.md](BUILD_GRAPH.md). Focused Clang ASan/UBSan passed with
LeakSanitizer disabled and no reported finding. The constructor still emits
legacy warnings for a nonvirtual destructor and incomplete `SYMKIND` switch;
those need owner-level review. Windows Debug/Final rebuilds, module ABI
checks, the actual App `MScript` call, lawful retail inputs, a visible game
scene, and same-input Windows comparison have not been performed for this
integration. The next startup owner is `IStreamer`.

Source review of that next entry finds that the production owner in
`Code/Libs/Src/Streamer/Streamer.cpp` is a DirectSound player with Win32
thread, event and message delivery. `Conquest.cpp` queries `IStreamer`, and
`SoundManager.cpp`/`MusicManager.cpp` consume its playback statuses. It is
an audio port rather than a parser or renderer dependency. The ordered
port plan defers audio until startup, data loading and a game scene are
measurable. A native profile that omits `IStreamer` can inventory the next
non-audio owner, but cannot be reported as a working retail profile or an
implemented stream service. No native `IStreamer` was built or executed in
this change.

## TextureLibrary aggregation after an omitted audio entry (2026-09-27)

The next non-audio `[System]` owner is the production Shading
`TextureLibrary.cpp`, which already compiles in the linked game-material
closure and exposes `RegisterNativeTextureLibrary`. Its `Initialize` obtains
the aggregate's `IRenderPipeline` and reads texture settings through the
profile parser. The falsifiable check was to register that existing factory
before `GameSystem::startup` and load it from a generated profile after the
five previously working services. A failed pipeline query or factory
ownership would stop startup. The existing shell process suite owns this
profile check; adding a stand-alone texture probe would not test App system
aggregation.

The GPU-enabled shell now registers the production factory. A generated
profile with `IEventSystem`, `IRenderPrimitive`, `IRenderPipeline`,
`IWindowManager`, `IViewConstructor` and `TextureLibrary` completes
`GameSystem::startup` and opens the dummy SDL window under GCC and Clang;
the no-GPU build rejects this renderer-dependent profile as expected. A
temporary copy of the checked-in source `Conquest.ini` with only its
`IStreamer` line removed reports six successful `[System]` creations and
then fails at the next entry, `LightManager`, with exit code 3 before a
window opens. The original checked-in profile still stops at `IStreamer`.
The modified copy is a diagnostic profile under `/tmp`, not a retail
profile or a proposed shipped configuration. No new texture was uploaded
in this profile test, and it is not visible rendering evidence.

Executed Linux x86_64 commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j 4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.native_shell$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_native_shell -j 4
ctest --test-dir out/build/linux-clang-debug -R '^portability.native_shell$' --output-on-failure
ctest --test-dir /tmp/frontierx-shell-no-gpu -R '^portability.native_shell$' --output-on-failure
env SDL_VIDEODRIVER=dummy XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-capture-data --system-profile /tmp/frontierx-profile-no-streamer.ini --smoke-ms 1 --hidden
```

The one-line profile omission was made with a local Python command and
then supplied to the GCC shell with the same dummy SDL, data root and
preferences environment as the checked-in profile run above. No Windows
Debug/Final or ABI validation, `MScript`, retail runtime, or same-input
game-scene comparison was performed for this registration. The next
non-audio profile owner to inspect is `LightManager`.

Source review locates the supported solution's `LightManager` at
`Code/Libs/Src/LightManager/LightManager.cpp`; a separate older-looking
copy exists under `Shading/LightManager` and was not selected. The supported
owner registers an `AGGDESC` factory from `DllMain` and contains an x86
`__declspec(naked)` inverse-square-root assembly path in its per-light
calculations. Native compilation requires an owner-level numerical and
determinism decision, not merely a registration shim. This is source review
only; no LightManager Linux object, synthetic lighting execution, Windows
numeric comparison or game lighting result was produced here.

## LightManager execution gate (2026-09-27)

The supported `LightManager.cpp` is queried by `Conquest.cpp`; production
`PolyMesh::render` and App `MeshRender` call its vertex-light methods. The
immediate integration blocker was that its x86 naked inverse-square-root
function and Windows build spellings prevented Linux object compilation.
The falsifiable behavior is for the real `ILightManager` factory to
register one generated point/spot light, shade a vertex above ambient
through both `LightRGB` and App `LightRGB_U8`, and restore ambient
after deactivation. Neither the parser check nor the existing game-material
fixture exercises the light owner. The source's `get_best_lights` method
is not exposed by the published `ILightManager` vtable, so this check does
not exercise selection. A passing check permits the production factory
in a generated diagnostic profile; it does not establish retail lighting
or Windows parity.

The production source compiles as `frontierx_light_manager_owner` under
GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug. Windows retains the x86
assembly branch; Linux uses the
existing non-assembly seed formula with a fixed 32-bit bit pattern and no
FMA contraction. The unused inverse-square-root lookup-table object is
excluded from Linux because its `unsigned long` union assumes Win32 width.
Exact-case includes, standard min/max calls, the portable DACOM sort
callback declaration, removal of obsolete `register` storage and a
Windows-only linker entry stub complete this object-level compilation.
The public `ILightManager` vtable and light data layout were not changed.

The linked `frontierx_light_manager_probe` uses the production DACOM
factory, aggregate, scalar `I3DMathEngine` and light implementation. With
one generated spot light at `(0, 0, 5)` and a vertex at the origin, it
observes ambient `(10, 20, 30)` before activation, RGB above ambient
after activation in both vertex methods, registered light identity and
removal, ambient again after deactivation,
and complete native heap release after unregistering the light and factory.
The light path executes the native inverse-square-root calculation. An
initial sanitizer run exposed a missing math-provider registration in the
probe; registering the existing production provider resolved the null call.
This is automated synthetic CPU execution, not a game-scene or retail result.

The GPU-enabled shell now registers the production math and LightManager
factories. Its process suite loads a generated seven-service `[System]`
profile through the App `ISystemContainer` and reports successful
`GameSystem::startup`; the no-GPU shell still rejects unavailable render
services. This process suite checks synthetic startup rather than scene
lighting; the subsequent PolyMesh gate below covers that caller. The checked-in `Conquest.ini` still stops first
at `IStreamer`, which has not been ported.

Validation on Linux x86_64, 2026-09-27:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_light_manager_probe
ctest --test-dir out/build/linux-gcc-debug -R '^portability.light_manager$' --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_light_manager_probe
ctest --test-dir out/build/linux-clang-debug -R '^portability.light_manager$' --output-on-failure
```

Both focused checks passed in the complete GCC and Clang Debug preset runs,
along with `portability.native_shell`. A separate Clang ASan/UBSan
execution of `frontierx_light_manager_probe` passed with LeakSanitizer
disabled; the focused Clang sanitizer shell process suite also passed.
An existing GCC SDL3/no-GPU shell process suite passed. The input light
and profile were generated; no retail media was
used. Windows Debug/Final rebuilding and module ABI checks, exact lighting
comparison with Windows x86 and retail profile/runtime remain unperformed
by this focused gate. The Linux scalar inverse-square-root
behavior must be compared with the Windows assembly before determinism is
claimed.

### PolyMesh lighting integration gate (2026-09-27)

The standalone light check cannot show that the production `PolyMesh::render`
caller receives `ILightManager` from its system services or writes the result
into the submitted game mesh. The existing linked game-material check already
creates a smooth-shaded legacy mesh and reaches that caller. The falsifiable
check was to supply the real light aggregate and compare submitted vertex
diffuse values under two ambient settings while keeping the same mesh,
material and draw path. No new probe was needed.

The linked `frontierx_game_material_manager_probe` now gives
`PolyMesh::render` the production `ILightManager` through its actual system
service query. Its smooth-shaded generated legacy mesh submits different
36-byte vertex diffuse values at ambient `(255,255,255)` and
`(32,48,64)`; the mesh, material and renderer caller are identical. The
visible GPU mode retains white ambient so its established image oracle
remains comparable. This is synthetic CPU integration evidence for the
real production caller, not a retail lighting comparison.

The first Clang ASan/UBSan run found a one-past-end read in
`LightManager::light_vertices_strided`: its loop increment loaded the next
normal index even after the last vertex. The owner now computes the masked
normal index at the start of each iteration. The focused light check also
covers a one-vertex flipped-index call, and the linked production PolyMesh
check passes under GCC, Clang and Clang ASan/UBSan with LeakSanitizer
disabled. No public light interface or serialized type changed.

A seven-service supplied profile initially failed during scene composition
because it already owned `TextureLibrary`, while `NativeGameScene` added
another instance and required that second pointer in its container
identity check. The scene now reuses the profile-owned library when present
and creates a local aggregate only for profiles that omit it. The existing
profile pipeline and primitive are still bound at GPU startup. Generated
seven-service profiles now present both the older-format PolyMesh fixture
and the CQ2 App fixture through the GameSystem Engine. The latter does
not use LightManager.

Visible Wayland/Vulkan Linux x86_64 execution used repository-generated UTF
material and mesh archives and the checked-in shaders. The seven services
were EventSystem, render primitive, render pipeline, window manager, view
constructor, TextureLibrary and LightManager, with `IStreamer` omitted.
The legacy PolyMesh scene presented one frame with 496 changed pixels and
checksum `3422507570980372643` under both GCC 16.2.1 and Clang 22.1.8.
With the same textured mesh and material but no LightManager service, the
GCC scene produced 496 changed pixels and checksum
`11826028615312451232`. The distinct bounded readbacks show that the
production light service affects visible output. The full-profile CQ2 App
scene presented one frame with checksum `1466864583390317354` under both
compilers. A focused visible Clang ASan/UBSan seven-service legacy scene
presented the same `3422507570980372643` checksum without a reported
sanitizer finding; LeakSanitizer was disabled.

The legacy mesh fixture is generated by
`portability.game_material_gpu` in its textured mode. The CPU
`portability.game_material_manager` check writes an untextured fixture
to the same build-directory name. Re-run the visible GPU check immediately
before the manual shell command; an intervening full CTest run replaces
that input. A Clang shell run with the untextured fixture correctly
reported that PolyMesh did not bind a texture. The shell now reports
separate geometry, texture, readback and presentation failures rather
than one generic draw failure.

The full GCC and Clang Debug build/CTest presets still pass 42 executing
checks; two command-sandbox GPU checks skip and are not counted as runtime
evidence. The visible `portability.game_material_gpu` check passed
separately under GCC, Clang and focused Clang ASan/UBSan. Native Windows
Debug/Final and module ABI checks, Windows x86 lighting/image parity,
retail media/profile/runtime, `Conquest.cpp` game cadence and lighting
for the CQ2 App branch remain unperformed. The checked-in
`Conquest.ini` still stops first at its unported `IStreamer` service.

### VertexBufferManager profile gate (2026-09-27)

The next non-audio `[System]` owner in `Conquest.ini` is the production
`VertexBufferManager`. `System.cpp` calls `IVertexBufferManager::initialize`
after renderer setup, and App `RenderBatcher.cpp` acquires/releases its
buffers. The existing linked manager probe proves buffer lifecycle against
the native pipeline but creates a non-aggregate native factory. Source review
shows the supported Windows DLL registers the aggregate factory that the App
`ISystemContainer::LoadSystemComponents` caller requires; the native factory
does not populate its inner component for that caller. The falsifiable check
was to use the production aggregate factory in the existing manager probe,
then load the same owner through a generated system profile and initialize
it only after the scene pipeline is bound.

The native registration now uses the same
`DAComponentFactory2<DAComponentAggregate<VertexBufferManager>, AGGDESC>`
shape as the supported Windows DLL. The existing
`frontierx_vertex_buffer_manager_probe` supplies an outer component,
requires the factory to populate its inner component, queries the real
`IVertexBufferManager` through that inner component, and still exercises
preallocation, acquisition, release and cleanup against
`NativeRenderPipeline`. No public interface, persistent layout or Windows
registration branch changed.

The GPU-enabled native shell links and registers the production owner.
Its process suite loads a generated eight-service profile with
`VertexBufferManager` after LightManager. For a supplied scene, the
launcher queries the profile-owned manager, calls its production
`initialize(nullptr)` only after binding the profile pipeline and calls
`cleanup()` before unbinding that pipeline. Minimal profiles still omit
the manager. These operations follow the relevant `System.cpp` ordering;
the shell does not yet run the full App render-batcher buffer cadence.

The generated eight-service older-format PolyMesh scene presented one
Wayland/Vulkan frame with 496 changed pixels and the unchanged lit checksum
`3422507570980372643` under GCC 16.2.1 and Clang 22.1.8 Linux x86_64
Debug. A focused Clang ASan/UBSan visible scene produced the same checksum
without a reported finding, with LeakSanitizer disabled. The focused
`portability.vertex_buffer_manager` and `portability.native_shell`
checks passed with both compilers and the sanitizer build. All inputs were
generated profiles, repository fixtures and shaders; no retail media was
read. A diagnostic copy of the checked-in `Conquest.ini` with only
`IStreamer` removed now reports eight successful system component
creations and stops at the next entry, `IStringTable`. This is synthetic
startup evidence; the original profile still stops first at
`IStreamer`.

The first full GCC and Clang CTest rerun after the factory change exposed a
`portability.state_material` segmentation fault: that existing material
fixture and the SDL GPU fixture had treated the factory's returned outer
pointer as a standalone component. Both fixtures now provide an
`AGGDESC::inner` slot and query/release the manager through that inner
component. The focused StateMaterial checks then passed under GCC, Clang
and Clang ASan/UBSan, and the visible SDL GPU checks passed under GCC and
Clang. Both complete Linux Debug builds and CTest presets subsequently
passed 42 executing checks; the two command-sandbox GPU checks skipped.
The GCC SDL3/no-GPU shell and manager checks also passed.

At this source-review stage, the next non-audio profile owner,
`StringTable.cpp`, included Win32 headers, historical XML/FileSystem
persistence and `wchar_t` storage. Its disk representation and Linux/Win32
width difference required an owner-level audit before native registration.
That audit and registration are recorded below. Native Windows
Debug/Final and cross-module ABI checks, retail profiles and runtime,
full App `RenderBatcher`/VertexBufferManager draws and deterministic
gameplay remain unperformed.

### Next profile boundary: production StringTable source audit (2026-09-27)

**Source review, not a native compile or runtime result.** The checked-in
`Conquest.ini` places `IStringTable` immediately after
`VertexBufferManager`. `Conquest.cpp` obtains it from `GAME.GS` with
`IID_IStringTable` and asserts that the pointer exists. The real
`MScript.cpp::loadStringTable` caller later obtains its `ISaverLoader`
interface and loads strings from a DOS child `IFileSystem`. Registering a
dummy table would advance the profile count without satisfying that caller.

The existing Windows DLL registers
`DAComponentFactory2<DAComponentAggregate<StringTable>, AGGDESC>`.
`StringTable.cpp` includes `<windows.h>`, calls Win32 locale and character
conversion functions, and persists through the legacy XML and
`IFileSystem` interfaces. Its file save loop writes one `wchar_t` at a time
using `sizeof(wchar_t)` and then writes a wide NUL. Its file load path
allocates a buffer of the file's byte length with `alloca`, reads bytes
directly into `wchar_t` storage, and constructs `std::wstring` through
`NewString`. Since Win32 uses two-byte `wchar_t` and Linux uses four-byte
`wchar_t`, compiling this path unchanged would alter saved bytes and
misinterpret existing UTF-16 strings. The load path also has no explicit
check for an even byte count, complete NUL terminator or bounded stack
allocation. XML persistence has a separate `TiXmlElement` wide-string
conversion path.

The public `IStringTable` returns `wchar_t*` strings to consumers and the
`IStringEnum::StringInfo` callback carries `unsigned long idString`. The
latter is four bytes in the supported Win32 ABI and eight bytes in Linux
LP64. No public layout or serialized string width was changed here. The
next owner change must keep UTF-16LE as the file representation, define
explicit conversion at the Linux `wchar_t` boundary, validate malformed
input, and exercise the production `ISaverLoader` caller with a generated
UTF archive through the existing file-system contract. It must preserve
the Windows DLL registration and receive a native Windows Debug/Final
and ABI regression check before claiming cross-platform parity. Neither
the StringTable owner nor that caller executed in this gate.

**XML conversion gate, automated execution (2026-09-27).** The production
`StringTable::Save(TiXmlNode&)` and `Load(TiXmlNode&)` call the existing
TinyXML wide-attribute methods. `TiXmlElement::SetAttribute(wchar_t*)` used
a fixed 16 KiB buffer and wrote four hex digits for each native `wchar_t`;
this could overrun for long strings and truncate non-BMP Linux characters.
`QueryUnicodeValue` decoded each four-digit group directly into native
`wchar_t`, leaving a Linux surrogate pair as two separate characters and
accepting malformed hexadecimal input. The methods now serialize UTF-16
code units in the original four-digit lowercase hex form, including a
trailing `0000`, and decode valid pairs to a Linux wide scalar. The Windows
two-byte `wchar_t` path retains its code-unit representation. Malformed hex
and unpaired Linux surrogate input return `TIXML_WRONG_TYPE`; the fixed
output buffer is gone. The existing `portability.tinyxml` check now covers
the exact `03a9d83dde420000` representation, round-trip decoding, a
5,000-character string and invalid input. GCC 16.2.1 and Clang 22.1.8
Linux Debug built and executed that check; focused Clang ASan/UBSan passed
with LeakSanitizer disabled after the host's ptrace restriction prevented
LSan startup. Both complete Linux Debug builds and CTest presets passed
42 executing checks each, with two GPU checks skipped in the command
sandbox. The test input is repository-generated; the StringTable owner and
its `ISaverLoader` caller are still uncompiled and unexecuted on Linux.
Native Windows Debug/Final, StringTable module ABI, and retail XML/file
round trips remain unperformed.

### StringTable UTF-16LE file boundary (2026-09-27)

**Blocker and real caller.** `MScript.cpp::loadStringTable` passes its DOS
child `IFileSystem` to the production `ISaverLoader::Load` method. The old
`StringTable.cpp` read archive bytes into an `alloca` buffer of native
`wchar_t` and used `sizeof(wchar_t)` when saving. This cannot interpret the
same file on Win32 and Linux. The existing TinyXML check covers a different
XML attribute representation and cannot disprove this file-format defect.

The production `StringTable` save and load methods now call
`FrontierX/StringTableFileText.h` to write and read NUL-terminated UTF-16LE
code units. Saving retains the original one-code-unit `IFileSystem::WriteFile`
cadence while requiring each two-byte write to complete. Loading uses an
owned byte vector, checks an even byte count, the final NUL, complete reads,
no embedded NUL, and valid surrogate pairs on Linux. It closes file and
enumeration handles and restores the parent directory on failed reads.
The public enumerator's ID is now `U32`, which is four bytes on Linux and is
the same `unsigned long` width used by the Win32 build. Exact-case includes
were corrected, and the XML numeric attribute calls explicitly select
TinyXML's unsigned-long overload; the Windows DLL factory remains present.

**Falsifiable check and result.** The new
`portability.string_table_file_text` check compiles the public header,
asserts the callback ID's width and pointer-relative offset, and compares
`A`, Greek omega and a supplementary character against the exact ten
UTF-16LE bytes `41 00 a9 03 3d d8 42 de 00 00`. It round trips that text,
accepts an empty NUL-only value, rejects an odd byte count, missing or
embedded NUL, a lone high surrogate and an invalid Linux input scalar,
and verifies that rejected input leaves the previous output intact. This
probe is needed to decide whether the owner can safely use the codec for
both file methods; it does not substitute for executing those methods.
`cmake --build --preset linux-gcc-debug --target
frontierx_string_table_file_text_probe` and the corresponding Clang command
built with GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 Debug. The focused
CTest passed under both compilers and Clang ASan/UBSan with LeakSanitizer
disabled. Both complete Linux Debug builds and CTest presets then passed:
45 discovered, 43 executed, two GPU checks skipped in the command sandbox.
The bytes and strings were generated by the repository probe; no retail
files were used.

**Remaining integration.** A direct Clang C++17 syntax attempt on
`Code/Libs/Src/StringTable/StringTable.cpp` with the repository include
directories still fails on Win32 language conversion, LCID constants and
`DllMain` types. The production StringTable object and its `ISaverLoader`
method therefore have no native compile or runtime evidence. The language
directory mapping, native aggregate registration, generated UTF archive
execution through `MScript`'s file-system contract, and native Windows
Debug/Final and module ABI regression remain required. The checked-in
profile still reaches only eight successful non-audio services in its
diagnostic `IStreamer`-omitted copy.

### Production StringTable aggregate and native archive load (2026-09-27)

**Blocker and caller.** The supported `Conquest.cpp` startup profile next
requests `IStringTable`; `MScript.cpp::loadStringTable` later queries its
`ISaverLoader` and passes a DOS child `IFileSystem`. The preceding file-text
check proved UTF-16LE bytes but did not compile or execute the production
owner. A direct C++17 syntax check exposed the Win32 LCID symbols, locale
conversion calls and DLL entry point. Passing the existing synthetic archive
through the real `StringTable::Load` is the local decision gate: it proves the
owner can be registered and can consume the native read-only UTF contract.

The Linux-only `LegacyLanguageIds.h` supplies the symbolic language IDs
actually used by the existing owner. Its numeric values were checked against
Microsoft's Windows SDK `winnt.h`; the historical
`SUBLANG_LITHUANIAN_CLASSIC` entry is absent from the current SDK and does
not occur in the executable `getSubLangID` branch. Windows retains its SDK
constants, locale conversion, system-default LCID and `DllMain` factory.
Linux uses the owner's existing symbolic names as ASCII directory tokens,
parses them into the same 16-bit LANGID layout, defaults a fresh dictionary
to en-US `0x0409`, and registers the same DACOM aggregate factory through
`RegisterTheStringTable`. This default is a temporary native policy; a
platform-selected user language has not been integrated. No persistent
layout or simulation type was changed in this step. The public enumerator
ID's fixed four-byte width was established in the preceding gate.

The focused `frontierx_string_table_owner_probe` uses the registered factory,
queries `IStringTable` and `ISaverLoader`, checks tag/ID access and
enumeration, selects both en-US and French dictionaries, saves and reloads
both through the production TinyXML methods, and
loads a generated UTF 1.01 archive through `OpenNativeUtfFileSystem`. That
archive contains `IStringTable/LANGUAGE.LANG_ENGLISH.SUBLANG_ENGLISH_US/
Greeting` with NUL-terminated UTF-16LE `Hello Ω`. It is repository-generated
synthetic input, with no retail media. The GPU shell also links this owner;
its existing process suite now supplies a nine-service `[System]` profile
including `IStringTable`, calls the production App loader and
`GameSystem::startup`, and opens the dummy SDL window. This does not execute
`MScript.cpp` or prove retail string-table compatibility.

**Executed validation.** On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 each
built `frontierx_string_table_owner_probe` and `frontierx_native_shell` in
their `linux-*-debug` presets. The focused CTests
`portability.string_table_owner` and `portability.native_shell` passed under
both compilers. Both full build and CTest presets passed with 46 discovered,
44 executed and two command-sandbox GPU skips. A focused Clang ASan/UBSan
build and both focused checks passed with `ASAN_OPTIONS=detect_leaks=0` and
`UBSAN_OPTIONS=halt_on_error=1`; the no-GPU GCC shell process suite also
passed. The observable owner result was a recovered `Hello Ω`, stable
four-byte enumeration ID and successful two-language XML reload. The shell result was
successful synthetic nine-service startup before the dummy window opened.
No visible GPU scene was run for this registration.

**Remaining gate.** A temporary diagnostic copy of checked-in `Conquest.ini`
with only `IStreamer` omitted exited 3 after nine `[OK]` component reports
and one `[FAILED]`; source order identifies that tenth entry as
`IVideoSystem`. This is a synthetic process observation without retail data.
The unaltered profile remains blocked by `IStreamer`. Native `MScript.cpp` file
loading, external and retail string tables, non-en-US startup selection,
native Windows Debug and
Final rebuilds, affected module ABI checks and retail runtime remain open.

### MScript DOS-root file-system contract (2026-09-27)

`MScript.cpp::loadStringTable` does not call the table loader directly on its
incoming archive. It first requests a read-only `DAFILEDESC` child named `\`
with `lpImplementation = "DOS"`, sets that child's current directory to `\`,
then passes it to `ISaverLoader::Load`. Source review found that the native
UTF `IFileSystem` rejected both exact root spellings, so the previous owner
probe did not cover this real caller boundary. The hypothesis was that a
root view over the same validated UTF archive can satisfy that contract
without enabling writes or changing ordinary child-path rejection.

The native UTF factory now creates a separate root view only for the exact
DOS descriptor with `GENERIC_READ` and `OPEN_EXISTING`; `SetCurrentDirectory`
also accepts `\` as the root spelling. The existing
`portability.string_table_owner` check follows the production caller's
descriptor and directory sequence, loads its generated UTF-16LE `Greeting`
through that child, and rejects a read/write DOS-root request. The fixture is
synthetic repository-generated data. This executes the real `IFileSystem`
and `StringTable::Load` implementations, not the `MScript.cpp` translation
unit or a campaign module.

GCC 16.2.1 and Clang 22.1.8 on Linux x86_64 rebuilt the focused owner target
and passed `portability.string_table_owner`. Both complete `linux-*-debug`
builds and CTest presets also passed; the two GPU checks skipped in the
command sandbox and are not runtime evidence. A focused Clang ASan/UBSan
owner build and CTest passed with
LeakSanitizer disabled on this host. Native Windows Debug/Final, module ABI,
retail archive behavior and `MScript::Load` execution remain unperformed.

### Atomic StringTable archive load (2026-09-27)

**Regression and integration decision.** `MScript.cpp::loadStringTable` passes
an archive to production `StringTable::Load`. That owner inserted each decoded
file immediately. If a later entry was malformed, `Load` returned false but
left the earlier entry installed. The existing file-text test checks one
buffer and cannot establish table-level failure behavior. A new case in the
linked owner check writes a generated UTF archive with valid `Added` followed
by `Broken`, whose UTF-16LE bytes lack a terminal NUL. Before the fix, the
check failed because `Added` remained visible after `Load` failed. This
result established that archive loading needed rollback before relying on
failure status in startup or save restoration.

The production owner now decodes entries into a pending list after entering
the archive's `IStringTable` directory. It inserts them only after every
file, enumeration close and parent-directory restoration succeeds. A failed
read therefore never mutates the dictionaries or active language; an
unrelated string also keeps its existing address after a successful load.
Successful loads retain the existing insertion order and ID behavior. This changes neither
the on-disk UTF bytes nor public interface layout, but it changes failed
load behavior in the supported Windows owner and therefore needs native
Windows regression validation.

The existing `portability.string_table_owner` test now enters the malformed
archive through the production `MScript` DOS-root child contract. It verifies
`Load` rejects it, neither `Added` nor `Broken` is visible afterward, the
previous English `Greeting` remains at the same string address, and the
active French dictionary still returns `Bonjour` from its original address.
The pre-existing English `Local` string also retains its address when the
valid archive adds `Greeting`.
GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug rebuilt
and executed that target successfully. Both full Linux Debug builds and
CTest presets passed again. A focused Clang ASan/UBSan build and the same
owner CTest passed with LeakSanitizer disabled on this host. The input was
generated by the repository test; no retail media was used. Native Windows
Debug/Final rebuilds, module ABI checks, retail archive behavior,
`MScript::Load` execution and error recovery in the full game remain open.

### Shared MScript StringTable load call (2026-09-27)

The preceding native check repeated the descriptor and interface sequence
from `MScript.cpp::loadStringTable`; duplication could drift before the App
translation unit becomes buildable on Linux. The production static caller
now forwards to the App-specific `FrontierX::LoadScriptStringTable` function
in `AppStringTableLoad.h`. That function owns the existing DOS-root child
request, `ISaverLoader` query, root-directory selection and delegated load.
It returns a result to native callers while `MScript` continues its existing
void call path. The supported Windows `MScript` module has no public layout
or vtable change. Its include spellings for `Filesys.h` and
`TSmartPointer.h` were also corrected to match the repository.

The existing `portability.string_table_owner` check now invokes this same
function for both its valid generated archive and its generated archive with
a malformed second entry. This makes the production caller body executable
without adding a parallel test implementation. Passing the check decides
that the StringTable caller's archive and DACOM contracts are ready for a
later native `MScript` closure; it does not show that `MScript::Load` itself,
its campaign module loading, or retail data work on Linux.

GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug built and ran
`frontierx_string_table_owner_probe` successfully. Both full build and CTest
presets passed afterward. A focused Clang ASan/UBSan build and owner CTest
passed with LeakSanitizer disabled. The generated successful load returned
`Hello Ω`; the malformed archive returned failure without changing the
English or French dictionaries. Native Windows Debug/Final and affected
module ABI checks, retail runtime, and compilation or execution of the
`MScript.cpp` translation unit remain open.


## Profile-created game material and texture owners (2026-09-27)

**Blocker and caller.** A diagnostic copy of source `Conquest.ini` without
`IStreamer` and `IVideoSystem` stopped at `IMaterialManager`. Its native
factory existed but `NativeGameScene` registered it only after
`GameSystem::startup`. `ITManager` had the same ordering problem. The
falsifiable check was that the existing shell process suite could start an
11-service generated profile containing both, and its scene could resolve
those exact instances from the App container and draw without duplicate
aggregation. This is an integration change to existing production owners,
not a new probe or a claim that media is optional.

`NativeShell` now registers both factories before loading the profile.
`NativeGameScene` first queries the container, adds a manager only when absent,
and retains its existing identity checks against the Engine's system services.
After Engine consumers have stopped, a native-only material-owner reset
clears material/shader storage, releases its archive views and removes its
borrowed resource pointers. The texture owner flushes textures while the GPU
still exists and then detaches its pipeline. Profile-owned managers remain
in the container; dynamically added managers still undergo targeted removal.
No serialized field, public vtable or Windows code path changed.

The existing material probe now resets the loaded owner twice, verifies the
old material is absent, reloads through the same owner and continues its
normal material/mesh/draw checks. This guards stale material state when a
profile-created owner outlives a temporary GPU scene; the old single-lifetime
probe could not cover that regression. The existing shell suite checks both
profile entries and tests explicit failure at missing `IStreamer` and
`IVideoSystem`, before later components/window startup. Native container
messages now identify safe interface names; path-shaped invalid input stays
redacted, and the existing path-disclosure check still passes.

Linux x86_64 GCC 16.2.1 and Clang 22.1.8 Debug builds and their full CTest
presets passed. The full matrix is in [BUILD_GRAPH.md](BUILD_GRAPH.md).
The focused Clang ASan/UBSan material variants (normal, CR, missing-END)
passed with LeakSanitizer disabled. A generated profile containing
`IEventSystem`, `IRenderPrimitive`, `IRenderPipeline = RendSft`,
`IWindowManager`, `IViewConstructor`, `TextureLibrary`, `LightManager`,
`VertexBufferManager`, `IStringTable`, `IMaterialManager`, `ITManager` and
an empty `[Engine]` section opened under the dummy SDL driver. The same
profile with the existing CQ2 fixture ran visibly under GCC, Clang and
Clang ASan/UBSan on Wayland/Vulkan, each presenting one frame with 496
changed pixels and checksum `1466864583390317354`. GCC and Clang P6 files
were byte-identical, SHA-256
`e54ef6c12ff5ca0fd5da724725aae5da4e79f87074f75b6dd9ce26142e3b90d1`.
The sandbox attempt failed before window startup; only the subsequent
explicit desktop executions count as GPU runtime evidence.

The dynamic-owner legacy branch also ran visibly under GCC, with 496 changed
pixels and checksum `11826028615312451232`. An initial attempt used the
shared build-directory legacy fixture after CPU CTests had overwritten it
with the untextured variant; it correctly returned 7 for no bound texture.
The existing GPU material probe regenerated a textured fixture in a separate
external scratch directory; the rerun above passed. This fixture collision
must not be mistaken for a renderer success or hidden by relaxing the check.
The same GPU probe passed the reset/reload check and its CQ2 draws.

Commands executed (logs and generated files stayed under `/tmp` or ignored
build directories):

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j4
ctest --test-dir out/build/linux-gcc-debug -R 'native_shell' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_native_shell -j4
ctest --test-dir out/build/linux-clang-debug -R 'native_shell' --output-on-failure
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell -j4
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j4
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager' --output-on-failure
XDG_CONFIG_HOME=/tmp/frontierx-goal-profile/settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-goal-profile/Data --system-profile /tmp/frontierx-goal-profile/system.ini --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/game_material_gpu_fixture.utf.vertexcolor.mesh out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 150 --capture-ppm /tmp/frontierx-goal-profile/gcc.ppm
XDG_CONFIG_HOME=/tmp/frontierx-goal-profile/settings out/build/linux-clang-debug/frontierx_native_shell --data-root /tmp/frontierx-goal-profile/Data --system-profile /tmp/frontierx-goal-profile/system.ini --game-scene out/build/linux-clang-debug/game_material_gpu_fixture.utf out/build/linux-clang-debug/game_material_gpu_fixture.utf.vertexcolor.mesh out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 150 --capture-ppm /tmp/frontierx-goal-profile/clang.ppm
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 XDG_CONFIG_HOME=/tmp/frontierx-goal-profile/settings /tmp/frontierx-texture-handle-asan/frontierx_native_shell --data-root /tmp/frontierx-goal-profile/Data --system-profile /tmp/frontierx-goal-profile/system.ini --game-scene out/build/linux-gcc-debug/game_material_gpu_fixture.utf out/build/linux-gcc-debug/game_material_gpu_fixture.utf.vertexcolor.mesh /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 300
out/build/linux-gcc-debug/frontierx_game_material_manager_probe /tmp/frontierx-goal-profile/material.utf --gpu out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv
XDG_CONFIG_HOME=/tmp/frontierx-goal-profile/settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-goal-profile/Data --game-scene /tmp/frontierx-goal-profile/material.utf /tmp/frontierx-goal-profile/legacy_game_mesh_fixture.utf out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --smoke-ms 150
```

Only repository-generated fixtures were used for these rendering checks.
Native Windows Debug/Final rebuilds and ABI checks, retail rendering, mission
loading, normal unit input, simulation, save/reload and Windows image
comparison remain unperformed. The independent external archive inspection
is recorded in [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md).

## Nested mission UTF filesystem contract (2026-09-27)

**Blocker, owner and caller.** `menu.cpp` creates `Maps/Campaign` through
`IFileSystem`, and `Mission::LoadBriefing` opens its chosen mission through
that directory's `CreateInstance`. The Windows `BaseUTF` factory offers an
opened file to filesystem implementations, allowing a UTF mission within a
UTF maps archive. The native adapter returned only a byte-file view, which
could not expose `ParseData`, object data or the global save record. Simply
accepting archive names in the startup directory check would not fix this
production contract.

`UtfMetadata.cpp` now shares one stream-based parser between physical files
and bounded byte-backed archives. Both retain the existing signature,
version, range, directory-link, name, duplicate and payload checks. The
memory overload is limited to 64 MiB, matching the existing entry-read cap.
The native adapter recognizes the UTF signature when no implementation is
specified, validates the complete nested archive before publishing a child,
and keeps its bytes in shared immutable storage for directory and DOS-root
views. A malformed UTF-signature file fails; it is never silently converted
to a successful ordinary file. Explicit `UTF` requires UTF validation;
explicit `DOS` keeps raw bytes. Other implementations and non-OPEN_EXISTING
creation are rejected. Raw file views reject directory operations. The
nested logical filename includes its parent archive/entry path. No extraction,
writable archive, persistent layout or Windows implementation changed.

The existing `portability.string_table_owner` fixture now wraps its generated
StringTable UTF in `Campaign/Training.dmission`, with a separate truncated
UTF sibling. It opens the campaign and mission using production descriptors,
checks rejection of the truncated sibling and explicit DOS byte access,
releases the outer interfaces, removes the physical fixture, and calls the
same `LoadScriptStringTable` body used by `MScript`. Recovery of `Hello Ω`
therefore proves nested ownership and the DOS-root view remain valid without
the parent file. Its existing malformed-string rollback check still runs.
This extends an existing consumer test to disprove the named mission-file
blocker; the previous flat archive could not cover the nested contract.

GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug builds and full CTest presets
passed. The first full runs exposed an inventory assertion tied to the old
inline `ListUtfDirectory(path_, directory_)` call; the assertion now checks
the shared delegate and both physical/memory branches, without removing the
listing requirement. The full matrix is recorded in
[BUILD_GRAPH.md](BUILD_GRAPH.md). The focused Clang ASan/UBSan nested
StringTable owner test passed with LeakSanitizer disabled.

A temporary caller outside the repository linked each compiler's rebuilt
`frontierx_platform` library, called `OpenNativeUtfFileSystem` on the user's
external `DATA/maps`, then `CreateInstance("Campaign")` and
`CreateInstance("Sol_Train.dmission")`, released both outer interfaces, and
used `FindFirstFile`/`FindNextFile` on the remaining mission interface. Both
native executions returned the same five entries: `ParseData`,
`MT_SECTOR_SAVELOAD`, `ObjectList`, `MT_MUSIC_DATA`, `MT_GlobalData`.
These are minimum interoperability names, not copied retail contents. The
mission was never written to the repository or unpacked to disk. This proves
the filesystem call sequence on retail data; the actual `Mission.cpp`
translation unit, schema correlation and game simulation did not execute.

Commands executed:

```text
cmake --build --preset linux-gcc-debug --target frontierx_platform -j4
cmake --build --preset linux-gcc-debug --target frontierx_string_table_owner_probe -j4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.string_table_owner$' --output-on-failure
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_string_table_owner_probe -j4
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.string_table_owner$' --output-on-failure
c++ -std=c++17 -ICode/Libs/Include /tmp/frontierx-goal-profile/nested.cpp out/build/linux-gcc-debug/libfrontierx_platform.a -o /tmp/frontierx-goal-profile/nested
clang++ -std=c++17 -ICode/Libs/Include /tmp/frontierx-goal-profile/nested.cpp out/build/linux-clang-debug/libfrontierx_platform.a -o /tmp/frontierx-goal-profile/nested-clang
```

The final two executables were run with the same user-supplied external
`DATA/maps` path as their sole argument; the private installation path is
intentionally not recorded here. Temporary caller and logs stayed in `/tmp`.
No new GPU run, Windows Debug/Final build, cross-module ABI check, playable
mission, save/reload or retail rendering was performed for this change.
The next data boundary remains mounting packed data at native startup,
resolving the source-only `Mat` requirement without faking required services,
then feeding the opened mission into its production schema/global/object
loaders. The source profile still blocks at `IStreamer` independently.
