# Game Mesh and Material Ownership for Milestone 5

Latest save-format gate, 2026-09-27: the generated UTF writer bytes
round-trip through the native production reader, but no writable
mission `IFileSystem` or `SAVEDIR` is bound. The combined link still
fails before `Mission::Reload`; no Forger geometry, material draw,
input movement or persisted state was executed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-utf-save-format-encoder-gate-2026-09-27).

Latest mission-message gate, 2026-09-27: native `SCROLLTEXT` resolves
the selected script/App message global with an explicit optional-cue
skip. The combined mission link still fails on 28 symbols; the
training Forger has not been created, drawn, selected or moved. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-scrolling-message-and-explicit-cue-policy-gate-2026-09-27).

Latest mission UI gate, 2026-09-27: native `STATUS` and `HINTBOX`
resolve two production App resource globals and run in the external
training-mission shell. The combined link still lacks 29 production
symbols; `Mission::Reload` has not executed and no mission-owned
Forger mesh, material, input or movement reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-status-and-hint-resource-client-gate-2026-09-27).

Latest mission-load gate, 2026-09-27: the native `IPANIM` and source
build-version contracts now satisfy their temporary combined App link
references. `Mission::Reload` still cannot link or reach the selected
Forger's mesh/material and movement paths; 31 symbols remain absent.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-load-progress-and-source-build-version-gate-2026-09-27).

Latest packet-selection gate, 2026-09-27: the native equivalent of
`Search.asm`'s `dwordsearch` ran in the offline command-packet check.
The combined mission still does not link or draw the Forger; 33
production symbols remain absent. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-packet-selection-search-gate-2026-09-27).

Latest App localization/link gate, 2026-09-27: the native shell now
loads the source App strings before the external training mission's
parser path. This does not start `Mission::Reload` or render its Forger;
the combined App link still lacks UI, save and other production owners.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-app-string-resource-link-gate-2026-09-27).

Latest production-music-owner gate, 2026-09-27: GCC and Clang external
shell runs now initialize the original App `MusicManager` with the
required native streamer. This does not start `Mission::Reload` or
`Sol_T`, so the mission-owned Forger still does not reach the mesh,
material, input or movement paths. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-music-manager-owner-startup-gate-2026-09-27).

Latest mission audio-ownership gate, 2026-09-27: both external training
mission shells initialized the required `IStreamer` and bound the
external Music source to App globals. The shell still only parses the
mission source; it does not start `Sol_T` or draw a mission-owned
Forger. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-mission-audio-ownership-gate-2026-09-27).

Latest music save-layout gate, 2026-09-27: the external training
mission's saved music now round-trips through a checked fixed-width
converter under GCC and Clang. This does not link `MusicManager` or
start the mission, so no mission-owned Forger, input, movement or
material draw reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-music-save-layout-conversion-gate-2026-09-27).

Latest training-music gate, 2026-09-27: the selected external music
track now decodes and runs briefly through native `IStreamer`, while
the App music manager and mission remain unlinked. No mission-owned
Forger draw or movement reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-training-music-decode-and-disk-layout-gate-2026-09-27).

Latest selected-system-map gate, 2026-09-27: the native `ISystemMap`
fresh-load branch ran on the external training mission; its map
presentation methods remain explicit unsupported operations. The
combined App link still fails before `Mission::Reload`, so no
mission-owned Forger mesh, material draw or movement reached the
renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-system-map-fresh-load-gate-2026-09-27).

Latest combined mission-link gate, 2026-09-27: the native `Sol_T`
program no longer duplicates the host heap in a temporary App/script
link. That link still fails before mission execution, with 38 distinct
missing production symbols. No mission Forger geometry, material draw
or movement reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#combined-mission-script-heap-link-gate-2026-09-27).

Latest selected-script localization gate, 2026-09-27: the source
`Sol_T.rc` objective/subtitle strings now build and execute in a native
check under GCC/Clang. The external mission selects that script, but
the shell still stops before script and object startup. No mission-owned
Forger draw, input, movement or material path executed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-localization-and-fresh-loader-gate-2026-09-27).

Latest script-owner build gate, 2026-09-27: production `MScript.cpp`
now builds as a native object, but required module loading and
localization still stop mission startup. The shell has not created or
rendered a `Sol_Train.dmission` Forger, and no player movement reached
the material path. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-mscript-owner-build-and-required-resource-gates-2026-09-27).

Latest selected-script resource gate, 2026-09-27: the native `Sol_T`
build now carries the source-generated parser schema, and the parser
accepts its embedded bytes under GCC/Clang. The shell still stops
before `MScript` registration and mission objects, so no Forger mesh,
material or movement reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-native-schema-resource-gate-2026-09-27).

Latest selected-program compile gate, 2026-09-27: `Sol_T.cpp` and its
helper build as Linux objects, and the generated script schema passes
the production parser. Neither object is linked into the native shell;
the external `Sol_Train.dmission` still has no mission-owned Forger
mesh, material draw, or visible movement. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-program-and-schema-build-gate-2026-09-27).

Latest selected-script data gate, 2026-09-27: the source-derived
`Sol_T` `MissionData` layout now checks the Forger/Taos/Acropolis
reference offsets under GCC and Clang. The external mission has those
four QuickSave objects but still stops before native `MScript` and
object creation. No mission Forger material or mesh was drawn. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-sol_t-script-data-and-fresh-start-gate-2026-09-27).

Latest training-script reference gate, 2026-09-27: the selected
mission's script save data contains `MPartRef`; its native type now
retains the Win32 16-byte stride. This is a script-data ABI check,
not a render or mission runtime result. `Sol_T` still does not start,
so no Forger material reaches the game renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#training-script-mpartref-save-layout-gate-2026-09-27).

Latest spatial-iterator build gate, 2026-09-27: the production
`ObjMapIterator` that later feeds `ObjectList` and `MScript` mission
object queries builds under GCC and Clang. The external training
mission still stops before object creation, so no Forger mesh or
material draw followed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-spatial-iterator-build-gate-2026-09-27).

Latest mission-camera manager gate, 2026-09-27: the production
`MovieCameraManager` builds into the native shell and executes its
registrar/event/cleanup in the offline App check under GCC and Clang.
The selected external mission still stops before object creation;
there is no mission-owned Forger mesh, material draw, or visible
movement. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-movie-camera-manager-gate-2026-09-27).

Latest mission-camera template gate, 2026-09-27: the App frame,
transform and render templates used by `MovieCamera.cpp` compile
under GCC and Clang. The camera owner remains outside the shell and
no external training-mission Forger or camera view reached the
renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#camera-object-template-closure-2026-09-27).

Latest mission selection-render gate, 2026-09-27: the production
`circleLines` table used by `BaseObject` range and selection rings
now initializes and is checked under GCC and Clang. No mission-owned
Forger or selection ring was rendered: the external training mission
still stops before startup. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-selection-ring-table-gate-2026-09-27).

Latest simulation event-owner gate, 2026-09-27: the production
`EventScheduler.cpp` builds under GCC and Clang but is not in the
successful mission shell. External `Sol_Train.dmission` still stops
before Forger creation and material submission. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-event-scheduler-build-gate-2026-09-27).

Latest spatial-object gate, 2026-09-27: the production `ObjMap.cpp`
owner now compiles under GCC and Clang, but is not in the successful
shell. The external `Sol_Train.dmission` still stops before object
creation; no Forger material or mesh was submitted. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-spatial-object-map-build-gate-2026-09-27).

Latest mission visibility-owner gate, 2026-09-27: the production
fog-of-war update, render and save/load owner compiles under GCC and
Clang, but remains outside the executable mission link. The lawful
external `Sol_Train.dmission` still stops before object loading;
no mission-owned Forger material or mesh was submitted. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-fog-of-war-native-build-gate-2026-09-27).

Latest mission object-service gate, 2026-09-27: the original
`Nugget.cpp` production manager and factory now compile under GCC and
Clang, but are not linked into the successful shell. The selected
external mission still stops before object loading, and no
mission-owned Forger material or mesh was submitted. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-nugget-manager-native-build-gate-2026-09-27).

Latest mission UI-owner gate, 2026-09-27: the native App cursor
registrar links into the shell and its default/move/busy paths run on
the visible Wayland host under GCC and Clang. `Sol_Train.dmission`
still stops before mission object loading, so no Forger material or
mesh was submitted. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-cursor-service-2026-09-27).

Latest App draw-owner gate, 2026-09-27: native `PrimitiveBuilder2 PB`
now submits a generated App quad through the existing CPU render
adapter under GCC and Clang. The selected external mission still stops
before object loading, so no mission-owned Forger, material or mesh
reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-primitive-builder-owner-2026-09-27).

Latest mission registrar gate, 2026-09-27: the native App global slots and
offline mission file-transfer condition now compile under GCC and Clang,
but the real `Mission`/`ObjectList` owners still fail their combined
service link. External training data was parsed in shell processes; no
mission-owned Forger or material draw occurred. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-offline-mission-registrar-contract-2026-09-27).

Latest mission-input gate, 2026-09-27: production offline packet
dispatch executed through `NetPacket`, `NetBuffer` and EventSystem
under GCC and Clang. It remains outside the mission shell; no
mission-owned Forger, mesh or material reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-offline-packet-dispatch-2026-09-27).

Latest packet gate, 2026-09-27: the production offline `NetBuffer`
queue executed under GCC and Clang in a focused process. It remains
outside the mission shell; no mission-owned Forger, material or mesh
was submitted. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-local-packet-buffer-build-gate-2026-09-27).

Latest mission gate, 2026-09-27: the production local packet-buffer
owner builds under both Linux compilers, but remains outside the native
shell. The lawful external Forger type is still metadata only: no
mission-owned mesh or material was submitted, and no unit was visible.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-local-packet-buffer-build-gate-2026-09-27).

Latest operation-owner build gate, 2026-09-27: `OpAgent.cpp` compiles
natively, but the operation, mission and object owners remain outside
the linked shell. The external Forger GameTypes record is still
metadata only; no mission-owned material draw ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-operation-owner-native-build-gate-2026-09-27).

Latest shared App-startup gate, 2026-09-27: the original global
registrar/cleanup owner now links into the native shell, but the
mission and `ObjectList` owners remain outside its executable link
closure. External training data still stops at schema loading; no
mission-owned Forger mesh or material reached the renderer. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#shared-app-global-startup-and-cleanup-owner-2026-09-27).

Latest mission object-owner build gate, 2026-09-27: the production
`ObjList.cpp` compiles under GCC and Clang, but its unresolved App link
closure still prevents loading a mission-owned Forger. No mission
material/mesh draw ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-object-owner-build-and-link-closure-2026-09-27).

Latest mission material input gate, 2026-09-27: the native shell opened
the lawful external Forger's 856-byte GameTypes record through the
production `IFileSystem` contract. This is archetype metadata only:
`ObjectList` has not loaded a mission-owned Forger, and no associated
material or mesh was drawn. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-gametypes-source-binding-2026-09-27).

Latest native App event-owner gate, 2026-09-27: the real
`IEventSystem` is retained by the shell and the unlinked native
`ObjectList` source can subscribe to it. Direct source compilation
next stops on particle/fog and render-state owners; no mission-owned
Forger or material draw ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-object-list-event-owner-contract-2026-09-27).

Latest object frame-clock source gate, 2026-09-27: `ObjList.cpp`'s
native frame timing now uses the existing monotonic clock contract in
source. The owner still fails direct compilation on real App services,
and no mission-owned Forger or material draw ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-native-frame-clock-source-contract-2026-09-27).

Latest object-debug source gate, 2026-09-27: the optional Win32
performance report and editor dialog were separated from `ObjList.cpp`;
direct Linux compilation now stops at event ownership and frame timing. No mission-owned
Forger or material draw exists yet. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-debug-and-clock-declarations-2026-09-27).

Latest object-owner source gate, 2026-09-27: the actual `ObjList.cpp`
caller now parses into its body under direct Linux GCC/Clang syntax checks
after native header and include corrections. It still does not build or
create the selected mission's Forger, so no mission-owned material draw
has run. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#object-list-native-source-boundary-2026-09-27).

Latest mission-object input gate, 2026-09-27: lawful external
`Sol_Train.dmission` QuickSave records for Forger, Taos and Acropolis
are reachable through the native filesystem under GCC and Clang.
`ObjectList::Load` and its archetype/object factories remain unlinked;
the diagnostic Forger Engine mesh is still not a mission-owned visible
unit. No mission material draw or GPU gameplay ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-quicksave-object-input-boundary-2026-09-27).

Latest mission-owner source boundary, 2026-09-27: `Mission.cpp` now
passes its first native App-service and Win32 disk-space compile gates,
but still lacks required save, camera, nugget, music and fog owners.
Neither the real mission nor a mission-owned material draw has run; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-save-space-platform-contract-2026-09-27).

Latest App global-owner boundary, 2026-09-27: production `MGlobals.cpp`
and its object-watcher service now compile as a native CMake target under
GCC and Clang. The owner is still outside the linked shell because its
App mission/event/archetype and localization services remain unresolved;
no mission-owned material draw has executed. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#complete-mglobals-compile-and-link-closure-2026-09-27).

Latest App-global source boundary, 2026-09-27: `MGlobals.cpp` now has a
native branch that loads the real general values used by gameplay formulas
after excluding its Win32 development viewer. Direct GCC/Clang syntax
compiles still fail on later App services; the owner and mission remain
unlinked. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mglobals-global-values-source-contract-2026-09-27).

Latest App data dependency, 2026-09-27: the shell's production `GenData`
owner now loads the external general database and retains the required
1640-byte `GlobalValues` archetype under GCC and Clang. This is a required
App global-data dependency for mission startup, not a material or gameplay
draw; see [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-general-data-owner-2026-09-27).

Latest App-global boundary, 2026-09-27: the real `MGlobals.cpp` owner now
reaches its implementation body in direct native syntax compilation, but
its global-component and window dependencies still block linking. The
previously created external Forger mesh remains separate from a
mission-owned object; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mglobals-native-owner-compile-boundary-2026-09-27).
Entries below are earlier gates.

Latest mission schema gate, 2026-09-27: the external training mission's
`ParseData` now reaches the profile-owned DOCUVIEW parser in the native
shell before any later object or material loading. The Forger
Engine/MeshManager diagnostic remains separate from a mission-owned unit;
see [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-mission-parsedata-owner-contract-2026-09-27).
Entries below are earlier gates.

Latest App mission-source gate, 2026-09-27: the shell's unchanged source
profile now supplies a production parser for the corrected embedded App
schema before the selected external mission source opens under GCC and
Clang. The external Forger remains a separate Engine/MeshManager diagnostic;
no mission-owned or visible unit ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-parser-resource-binding-2026-09-27).
Entries below are earlier gates.

Latest App resource gate, 2026-09-27: the selected mission's repository
base schema parses under GCC and Clang with the production view constructor.
`Mission.cpp` still does not compile or load a mission-owned Forger; the
external Forger Engine instance remains a separate diagnostic. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#repository-app-schema-and-mission-source-body-2026-09-27).
Entries below are earlier gates.

Latest App/script boundary, 2026-09-27: the native command-packet and script
registration declarations now compile with the original values and checked
pointer-sized runtime layouts. Both compiler shells still created and
destroyed the external Forger Engine/MeshManager instance, but it remains
neither visible nor mission-owned. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#app-declarations-and-script-registration-abi-2026-09-27).
Entries below are earlier gates.

Latest mission-owner boundary, 2026-09-27: the public `IMission` header now
compiles under GCC and Clang, but the actual App mission and global owners
remain outside the native graph because their direct source compilation
still encounters Windows lobby/PCH dependencies. The existing external
Forger Engine instance is not a mission-owned unit. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#mission-owner-native-compile-boundary-2026-09-27).
Entries below are earlier gates.

Latest App material-source gate, 2026-09-27: the native App data binding now
supplies the explicit material root through `MATERIALDIR` alongside the
external `OBJECTDIR`. GCC and Clang combined shell runs used those pointers
to create and destroy the external Forger Engine/MeshManager instance after
opening `Sol_Train.dmission` through `SPMAPDIR` (13 face groups, exit 0).
No mission-owned unit or visible draw ran; see
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-mission-source-bindings-2026-09-27).
Entries below are earlier gates.

Latest production Forger instance gate, 2026-09-27: after material,
texture and mesh-manager initialization, GCC and Clang used the lawful
external Forger archetype to create and destroy an Engine/MeshManager
instance. A valid Engine root existed during its lifetime and disappeared
after release; each run reported 13 face groups and exit 0. The internal
MeshInstance owner now propagates `create_instance2` failure. This remains
an object-data and CPU instance result, not a visible or mission-owned
unit. See [NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-forger-engine-instance-2026-09-27).
Entries below are earlier gates.

Latest retail-object archetype gate, 2026-09-27: the native shell's bounded
`--mesh-object` diagnostic initializes profile-created `ITManager`,
`IMaterialManager` and `IMeshManager`, then calls the production
`CreateMeshArch` owner with a lawfully external Forger object. GCC and Clang
each returned 13 face groups and exited 0; compiler-specific validation is in
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-forger-mesh-archetype-2026-09-27).
The MeshManager now rejects a failed Engine archetype instead of returning
a non-renderable mesh and releases an animation script set acquired on that
failure path. No mesh instance, visible draw or mission ran. Entries below
are earlier gates.

Latest production animation-data gate, 2026-09-27: the source-profile
`IAnimation` owner and registered `IChannel2` decoder loaded the external
`ssforger.cmp` `Sc_build` script under GCC and Clang (1.6 seconds). This is a
Forger object-data prerequisite for the selected mission; at that gate,
`IMeshManager` had not created or rendered the Forger instance. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-channel-and-forger-animation-data-2026-09-27).
Entries below are earlier gates.

Latest production startup gate, 2026-09-27: the unchanged source profile now
completes `GameSystem::startup` under GCC and Clang on lawfully external Steam
data after linking the original Anim, HardPoint and Optics components. This
does not initialize the App `IMeshManager`/`IMaterialManager` contracts or load
a mission mesh. See
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md#production-animation-and-hardpoint-startup-2026-09-27).
Entries below record previous gates.

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

Latest composition gate, 2026-09-27: the native shell registers the production
game material/texture factories before `GameSystem::startup`. Its scene now
reuses profile-created `IMaterialManager`/`ITManager` instances, verifies
container identity, and resets borrowed resources before GPU teardown.
The existing material check covers reset/reload of the same owner. Visible
GCC, Clang and focused Clang ASan/UBSan CQ2 shell runs retained 496 changed
pixels and checksum `1466864583390317354`; GCC/Clang captures match exactly.
The dynamic-owner legacy branch also passed visibly under GCC. These remain
generated scenes, not gameplay. Exact commands and the initial nontextured
fixture rejection are in
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md#profile-created-game-material-and-texture-owners-2026-09-27).
The external retail installation has no CQ2 `Mat` directory. Packed startup
archive mounts now work; material-source composition and mission integration
remain open, as detailed in
[NATIVE_SINGLEPLAYER_SLICE.md](NATIVE_SINGLEPLAYER_SLICE.md).

Status: 2026-09-27 source review, GCC/Clang production owner compilation,
synthetic CPU checks and visible Vulkan execution. The linked check loads
`Engine`, `RenderMgr`, `PolyMesh`, `TextureLibrary`, the game
`IMaterialManager` and `ITManager` through production factories. A generated
legacy mesh reaches visible indexed `PolyMesh` output with an RGB565 texture.
A generated CQ2 mesh resolves a production `VertexColor.fx` material and TGA,
then reaches visible output through both direct `Material::DrawVB` and the
App `MeshRender::Render` caller. Bounded GCC and Clang P6 captures match byte
for byte per path. The original `Synthetic.fx` fixture still rejects its
draw because that effect has no native implementation. The GPU-enabled native
shell now repeats a CQ2 fixture through the Engine created by
`GameSystem::startup`, with render and material interfaces forwarded through
the production App system container; see
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md). These are synthetic
integration results, not retail runtime. The detailed evidence and unperformed
gates are below.

## Active Windows path

The supported game's `Code/App/Src/Conquest.ini` loads `PolyMesh.dll`,
`TextureLibrary.dll` and `MaterialManager.dll`, requests `TextureLibrary` and
`IMaterialManager` from the system container, and registers `PolyMesh` with
the render manager. `Conquest.cpp` acquires `IID_IMaterialManager` and
initializes it with `MATERIALDIR`, `PIPE` and `TMANAGER` after startup. The
application project builds `Code/App/Src/MeshRender.cpp`; its mesh path looks
up named materials through `MATMAN->FindMaterial`, calls `Realize`, and later
submits `iMat->DrawVB` using D3D9 vertex and index buffers. The `PolyMesh`
component also requests `IMaterialManager`; `XMesh::read` calls
`read_material_library`, which loads texture entries and selects either the
CQ2 material list or the older `Material library` format. These are source
review facts, not evidence that the current Linux shell can execute them.

The app mesh vertex format is `D3DFVF_XYZ | D3DFVF_NORMAL |
D3DFVF_DIFFUSE | D3DFVF_TEX5`. Its `StandardVertex` is declared in
`IMaterialManager.h`. The current native submission adapter accepts only
RPVertex/RPVERTEX2 shapes, which have no normal and at most two UV pairs.
The real game path also depends on model/view transforms, material scripts,
texture manager, D3D9 vertex and index buffer ownership, and state families
beyond the six native depth/blend states. A native game mesh therefore cannot
be claimed from the current RPVertex fixture or by making
`SimpleMaterial::apply` return success for unsupported lighting/material calls.

## Native pipeline texture switch and next render stop (2026-09-26)

Source review of the supported `Direct3D_pipe.cpp` and `StateInfo.h` found
that `RP_TEXTURE` is a cached pipeline boolean, initially enabled. The real
`PolyMesh::render` caller reads it to choose `RWM_DONT_TEXTURE`. The native
`IRenderPipeline` now implements exactly that state with initial value `1`,
accepts `0` and `1`, and rejects invalid values or other pipeline states
without changing the cached value. This does not enable any new texture stage
or game vertex format. The existing render-pipeline regression executable
checks those transitions through the public vtable.

At this gate, the production `PolyMesh` path passed that state lookup but
the registered `NativeRenderPrimitive` still returned
`GR_NOT_IMPLEMENTED` from `set_modelview`; `PolyMesh` checked the result and
returned `VS_UNKNOWN` before geometry submission. The synthetic CPU game
fixture then observed an inverse camera transform query and zero draw
callbacks. This established the next transform contract; the later sections
record its implementation and the indexed CPU draw callback.
`RenderMgr` currently ignores the component visibility result, so its outer
return cannot serve as a success signal for this boundary.

Built `frontierx_render_pipeline_probe` and
`frontierx_game_material_manager_probe` with both Linux x86_64 GCC and Clang
debug presets. Four focused CTests per compiler passed: the pipeline check
and the standard, CR and missing-END generated game UTF variants. The three
game variants also ran under Clang ASan/UBSan with `UBSAN_OPTIONS=halt_on_error=1`
and no finding. Inputs were repository-generated synthetic data. Commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_pipeline_probe frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.(render_pipeline|game_material_manager)' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_pipeline_probe frontierx_game_material_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.(render_pipeline|game_material_manager)' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

Both full Linux debug builds also completed. The full CTest matrix is
recorded in [BUILD_GRAPH.md](BUILD_GRAPH.md). The GPU test skipped in this
command sandbox; it is not GPU execution evidence. Native game model/view,
game vertex submission, `IMaterialManager::DrawVB`, visible game scene,
Windows Debug/Final and module ABI checks, retail runtime and same-input
Windows/Linux image comparison remain open.

## Native model/view and projection boundary (2026-09-26)

The next production stop was `IRenderPrimitive::set_modelview` in
`PolyMesh::render`. The native primitive now forwards model/view and
projection through optional backend callbacks and caches each value only
after that callback succeeds. With no callback it still returns
`GR_NOT_IMPLEMENTED`. The native shell and SDL GPU probe register the SDL
renderer callbacks. That renderer validates finite matrix elements, converts
the row-major game transforms into a column-major projection × model/view
matrix, and pushes it as a vertex uniform. Both checked-in SPIR-V vertex
shader sources apply the matrix. Their default identity state preserves the
existing clip-space smoke input. The GPU probe now checks a rightward image
centroid shift, a distinct projection image, and exact checksum restoration;
it has **not executed** in this command sandbox because SDL video/device
initialization skips there. Shader compilation and linking are compilation
evidence, not a successful transform image.

The earlier CQ2 fixture has an `IMaterialManager` list but no legacy
`Material` list; it is unsuitable for the `PolyMesh::render` face-group
loop. Advancing beyond `set_modelview` exposed that mismatch and then missing
normal and UV arrays and a smooth-shading flag under Clang ASan/UBSan. A
separate generated old-format mesh now supplies those required structures
for the production PolyMesh render caller; the CQ2 fixture remains in the
game-material loader check. The same generated Engine/RenderMgr/PolyMesh
instance now invokes a model/view callback, and the CPU probe verifies that
the primitive caches its projection and a nonidentity camera model/view
translation of `z = -5`. No accepted game
draw callback occurred at this stage. Source review identified the next
indexed call as `MTVERTEX_FVFFLAGS` (position, diffuse, specular, two UV
pairs), which the primitive did not yet adapt. The component also ignored
the draw result. The later section records the CPU adapter and callback;
this result was not a native
game scene.

GCC and Clang Linux x86_64 debug full builds compiled the native shell,
shader outputs, GPU probe, production game owners and generated fixture.
Both full CTest presets ran 40 checks and skipped the SDL GPU check. The
three generated game UTF variants passed under Clang ASan/UBSan with
`UBSAN_OPTIONS=halt_on_error=1` and no finding after the fixture correction.
Relevant commands:

```text
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

Native Windows Debug/Final and ABI checks, actual GPU transform/readback,
game `MTVERTEX` GPU submission, App `IMaterialManager::DrawVB`, retail runtime,
and same-input backend comparison remain unperformed.

## Production PolyMesh indexed draw at native CPU boundary (2026-09-26)

The exact production call in `POLYMESH::render_facegroup_with_material` uses
`MTVERTEX_FVFFLAGS`: D3D9 `XYZ | DIFFUSE | SPECULAR | TEX2`. The 36-byte
`MTVERTEX` stores BGRA diffuse and specular colors followed by two UV pairs.
The native submission contract now has that exact layout with fixed size and
offset assertions. Its adapter preserves both color words and both UV pairs,
validates U16 indices against the vertex count, and rejects a wrong FVF or
unsupported topology. A third optional callback on `NativeRenderPrimitive`
receives this format; an owner without it still returns
`GR_NOT_IMPLEMENTED`. The established public `IRenderPrimitive` vtable is
unchanged. The existing submission check covers positive indexed transfer,
out-of-range rejection, exact FVF, callback data and callback error
propagation.

The synthetic production Engine/RenderMgr/PolyMesh instance uses the
separate generated legacy mesh described above. Its test camera now supplies
64-pixel horizontal/vertical projection constants, which are needed to keep
the fixture triangles above PolyMesh's half-pixel area threshold at the
nonidentity view distance. With that real caller, the callback receives an
indexed triangle-list submission with six U16 indices and matching primary
and secondary UV coordinates. It deliberately returns
`GR_NOT_IMPLEMENTED`, so this proves a CPU call through the production
component and native adapter, not a rendered game scene. The App `IMaterialManager::DrawVB` path remains a
distinct unexecuted caller.

Built `frontierx_render_submission_probe` and
`frontierx_game_material_manager_probe` with GCC and Clang Linux x86_64 debug
presets. Four focused CTests per compiler passed: the submission check plus
the standard, CR and missing-END generated game variants. Both full Linux
debug builds and CTest presets completed with 40 executing checks each and
the SDL GPU check skipped before device initialization. The three generated
game variants ran under focused Clang ASan/UBSan with
`UBSAN_OPTIONS=halt_on_error=1` and no finding. Commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_submission_probe frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.(render_submission|game_material_manager)' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_submission_probe frontierx_game_material_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.(render_submission|game_material_manager)' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

No GPU game format, native Windows Debug/Final or ABI check, retail runtime,
or same-input Windows/Linux game image comparison was executed.

## Production draw failure and SDL game-vertex pipeline (2026-09-26)

The production `POLYMESH::render_facegroup_with_material` now checks the
result of each indexed `MTVERTEX` and RPVERTEX specular draw. A failed draw
clears the material batching key and stops the remaining passes and face
groups through `POLYMESH::render`. The outer `RenderMgr` still ignores the
component visibility result, so this is local failure propagation, not an
application-level error report. The generated Engine/RenderMgr/PolyMesh
fixture continues to send six U16 indices to a callback that returns
`GR_NOT_IMPLEMENTED`; no additional game draw is inferred.

The SDL GPU owner now has an optional third vertex shader and graphics
pipeline for the exact 36-byte game transfer layout. It binds position,
normalized BGRA diffuse/specular, and both UV pairs at fixed offsets. The
fragment shader selects stage-zero UV set 0 or 1 and adds RGB specular when
`D3DRS_SPECULARENABLE` is set; alpha remains the texture-stage result.
The native primitive accepts those two validated states and commits them
only after its backend callback succeeds. The SDL owner validates them and
rebuilds all configured graphics pipelines transactionally when a render
state changes. Existing RPVERTEX and RPVERTEX2 vertex shaders supply zero
specular; the RPVERTEX2 probe now explicitly selects UV set 1 before its
second-UV image check and restores set 0 afterward.

The existing GPU probe registers the third submission callback. Its new
bounded readback checks that a game-format indexed draw has a visible
footprint, that choosing UV set 1 changes pixels, and that enabling a
nonzero specular color changes pixels. These GPU assertions **compiled but
did not execute** because SDL video/device initialization is unavailable
in this command sandbox. No actual `PolyMesh` call was connected to this
GPU probe. The new SPIR-V files are repository source, with no retail data.

Validation on Linux x86_64 used GCC 16.2.1 and Clang 22.1.8 debug presets:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

Both compilers built the GPU probe and all four shaders; the existing
render-submission check exercised state get/set, invalid-value rejection
and the third callback on synthetic vertices. The generated standard, CR
and missing-END game inputs completed under Clang ASan/UBSan with no
reported finding. The full CTest matrix is in [BUILD_GRAPH.md](BUILD_GRAPH.md).
Native Windows Debug/Final, module ABI/layout checks, retail runtime,
visible GPU game-format output, a native game scene, App `DrawVB`, and
same-input Windows/Linux game-image comparison remain unperformed.

## App StandardVertex buffer and primitive boundary (2026-09-26)

Source review of the active `MeshRender::RestoreVertexBuffers` and
`Material::DrawVB` calls found the next native rejection at
`IRenderPipeline::create_vertex_buffer`: the App requests
`D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX5` and writes its
`StandardVertex` layout before asking `IMaterial::DrawVB` to draw from the
buffer. The native pipeline previously owned only RPVERTEX/RPVERTEX2
buffers. The game format has a 68-byte runtime stride: position, normal,
BGRA diffuse, two main UV pairs and six tangent/binormal float components
that occupy the remaining three D3D9 texture-coordinate pairs. It is a
runtime vertex layout, not a persistent UTF or save schema.

The native pipeline now creates, locks, unlocks and destroys this exact
buffer format. It checks triangle-list topology, slice bounds, index count,
U16 index range and flags before handing the unchanged byte slice to the
real `IRenderPrimitive` vtable. The native primitive has an optional fourth
submission callback and copies the complete 68-byte layout into checked
typed storage before that callback. Without a fourth callback it reports
`GR_NOT_IMPLEMENTED`. This avoids treating a game normal or tangent as a
diffuse/UV field in the existing smaller pipelines. No shader or visible
game draw is implied.

The existing render-pipeline probe now executes allocation, lock/unlock,
nonzero-base indexed slice forwarding, exact byte preservation and negative
index/topology/locked-buffer cases. The existing render-submission probe
executes the fourth callback with distinctive normal, color, UV and tangent
values, checks byte equality and callback error propagation, and checks
missing-callback rejection. All data is synthetic and repository-owned.

GCC 16.2.1 and Clang 22.1.8 full Linux debug builds completed and each
CTest preset executed 40 checks successfully; the SDL GPU check skipped on
this host. Focused commands:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_pipeline_probe frontierx_render_submission_probe
ctest --preset linux-gcc-debug -R '^portability.(render_pipeline|render_submission)$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_pipeline_probe frontierx_render_submission_probe
ctest --preset linux-clang-debug -R '^portability.(render_pipeline|render_submission)$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_render_pipeline_probe frontierx_render_submission_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_render_pipeline_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_render_submission_probe
```

Both focused Clang ASan/UBSan executables reported success with no sanitizer
finding; LeakSanitizer was disabled for this host's ptrace environment.

At this 2026-09-26 gate, `Material::DrawVB` had not executed: the native
`IDirect3DIndexBuffer9*` pipeline overload and App index-buffer creation
were unsupported. The later section records their native CPU integration.
The App
`MeshRender.cpp` translation unit is not in the Linux graph. Windows
Debug/Final, affected module ABI/layout, retail runtime, visible game
scene and same-input Windows/Linux game-image checks remain unperformed.

## Distinct material system already exercised natively

`Code/Libs/Src/Shading/Materials/MaterialLibrary.cpp` owns the separate
`IMaterialLibrary`/`IMaterial` system. `TriMesh` and the tools refer to that
interface; the reviewed `TriMeshInstance.cpp` calls `IMaterial::apply` and
`render`, but no `TriMesh` project is present in the supported game project
set or listed in `Conquest.ini`. The earlier synthetic native checks use
the production `MaterialLibrary`, `SimpleMaterial`, `StateMaterial` and
`TextureLibrary` owners. A separate executable now exercises the game's
`IMaterialManager` loader and its production `IMaterial::DrawVB` method on
synthetic CPU buffers. The two
interfaces define distinct
`IMaterial` types and cannot be substituted by name or cast.

## Production Material::DrawVB with native index ownership (2026-09-27)

The App's `MeshRender::RestoreVertexBuffers` calls the renderer's
`create_index_buffer`, writes U16 indices through the returned object's
`Lock`/`Unlock`, and passes that `IDirect3DIndexBuffer9*` to its material.
The native renderer now returns a native-only index object in the same
pointer-shaped vtable slot. Its storage owns U16 elements, validates byte
length and aligned lock ranges, prevents drawing while locked, and has
explicit reference release. The native indexed-buffer overload checks the
requested index count and forwards the unlocked data to the same checked
vertex-slice path. Windows retains its existing D3D9 object and methods.
No pointer is converted into a serialized or runtime `U32` handle.

The existing generated game-material check now registers an optional
68-byte primitive callback before constructing the native pipeline. After
the production material is loaded and realized by `XMesh::read`, the check
allocates and fills a four-vertex `D3DFVF_STANDARD` buffer and a three-U16
index buffer through the real `IRenderPipeline` vtable. It calls the
production `Material::DrawVB` method with a native typed vertex-buffer
token, a nonzero first vertex, and the native index object. The registered
callback receives exactly three indices and three `StandardVertex` records;
their complete 204 bytes match the source slice, including normal, packed
color, UV and tangent fields. The buffers are then released. The three
standard, CR and missing-END synthetic UTF variants execute this path.

This is **synthetic CPU execution of the production material method**, not
the App `MeshRender.cpp` caller or a GPU game draw. The native
`load_effect` hook still returns null, so no D3DX shader pass executes.
The `Material::DrawVB` method returns `void` and currently does not report
an underlying pipeline failure to its caller. The App translation unit
still needs its Win32/D3DX dependencies isolated and a Linux build target.

GCC 16.2.1 and Clang 22.1.8 full debug builds completed on Linux x86_64.
Each full preset executed 40 passing CTests and skipped the SDL GPU CTest
before device creation. The existing render-pipeline probe additionally
checks odd index-buffer byte counts, unaligned/double locks, an oversized
indexed draw, lock-state rejection and release. Clang ASan/UBSan executions
of the render-pipeline and render-submission probes and all three generated
game-material variants completed without a reported finding; LeakSanitizer
was disabled for this host's ptrace environment. Commands:

```text
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_render_pipeline_probe frontierx_render_submission_probe frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_render_pipeline_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_render_submission_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

No native Windows Debug/Final or cross-module ABI/layout check, retail
runtime, visible game scene, production App draw caller or same-input
Windows/Linux game image comparison was performed for this integration.

## Next falsifiable integration gates

1. Continue from the now executing Engine/RenderMgr/PolyMesh instance into
   the active App mesh caller and complete `render_instance`. Supply the
   required camera and render-state contracts and execute the newly compiled
   model/view path with observable GPU output. Isolate Win32/D3DX9 dependencies behind existing
   engine boundaries. Preserve the Win32 project and module ABI. The earlier
   inactive `TriMeshInstance.cpp` attempt stopped at a case-mismatched
   `DACOM.h` and is not evidence for the active game path.
2. Execute the compiled `MTVERTEX` SDL pipeline with a visible GPU and then
   route the production `PolyMesh` caller through it. Extend
   `IRenderPipeline` for the App game vertex layout once that caller compiles.
   Test normal/UV interpretation, transform and material-state behavior with
   bounded CPU and visible GPU results. Unsupported combinations must continue
   to fail explicitly.
3. Compile the active App `MeshRender.cpp` caller into the native graph and
   route its `IMaterialManager::DrawVB` call through the now executing CPU
   material boundary. Port the D3DX material effect semantics before claiming
   visible game parity. Execute a native game scene and compare the same input
   with the supported Windows D3D9 path and run affected native Windows
   Debug/Final rebuild and ABI checks before accepting Milestone 5.

The next gate to disprove is whether this production `XMesh`/game-material
closure can reach the actual app draw caller and submit its engine instance.
The mesh geometry, CQ2 directory,
named texture and lookup contract execute in the synthetic CPU closure,
but this does not justify a synthetic renderer as a substitute for the active
material and draw owners.

A read-only Clang syntax preflight of a temporary copy of the active app
`MeshRender.cpp`, with its precompiled header replaced by native legacy
declarations and the include case corrected, stopped in
`Code/App/Src/Include/Globals.h` at Win32 `__declspec(dllimport)` and window
types. This is a source/compile blocker inventory, not an app object build.
The app global/import boundary must be isolated before the actual draw caller
can be compiled into a Linux target.

### App MeshRender include closure preflight (2026-09-27)

The newly executing production `Material::DrawVB` CPU path leaves the
active `Code/App/Src/MeshRender.cpp` caller as the next source owner.
Source review identified its `MeshRender.h` caller, `FaceGroupRender` index
allocation, and the Windows PCH/global-header path. The falsifiable local
hypothesis was that exact-case includes and a native declaration subset
would expose the next real compiler blocker before creating a native App
target. The read-only preflight uses a temporary copy of the production
translation unit outside the repository. That copy substitutes `CQTrace.h`
and the native legacy declarations for `pch.h`, and substitutes only the
App globals referenced by `MeshRender.cpp` for the Windows `Globals.h`.
It does not replace any production draw implementation.

The owning source and adjacent headers now use exact Linux include paths
for `ArchHolder.h`, `ObjList.h`, `renderer.h`, `lightman.h`,
`IMaterialManager.h`, `ENGINE.H`, `SYSTEM.H`, `VFX.H`, `FDump.h` and
`../Src/resource.h`. The production `TRANSFORM` constructor no longer uses
MSVC's redundant in-class qualification. Native-only calling-convention
spellings and `MEXTERN` declarations let the compiler reach `IObject.h`
and `MGlobals.h`. `IObject.h` now includes its mission enum owner and has
native forward declarations for the PCH-supplied `OBJID` and `OBJPTR`
types. The native `Globals.h` enum spells its existing `int` underlying
type explicitly to match that forward declaration; Windows retains the
original enum declaration. `IExplosion.h` now declares its `MeshChain`
reference, and `DTypes.h` includes the wide-string declarations it calls.
These changes do not alter disk fields or public Windows vtables. The
supported Windows build was not available for this source change.

GCC 16.2.1 and Clang 22.1.8 preflights now reach a broader App dependency
closure. The native x86_64 `GridVector::F2LONG` path uses GCC/Clang inline
`flds`/`fistpl` with a 32-bit result, preserving the Windows instruction
pair and the current x87 rounding mode. Its import annotation is empty on
native builds. A temporary diagnostic executable exercised six positive
and negative conversions under nearest, downward, upward and toward-zero
rounding with GCC and Clang at `-O2`; all matched the expected 32-bit
results. This is synthetic Linux runtime evidence, not Windows parity or
an integrated App test. The native `ARMOR_DATA` branch moves the named
`ARMOR_DAMAGE` type outside its anonymous union while preserving member
order; size 20, damage offset 4 and final extent 20 are compile-time
asserted. The Windows and `_ADB` declarations remain unchanged.

`MeshRender.h` now includes the native renderer declaration that owns
`ARCHETYPE_INDEX`, and `ArchHolder.h` includes the camera enum owner before
`ENGINE.H` on native builds. Both resolve further dependencies without
changing the Windows include order. The preflight consequently reaches
`ObjList.h` as its first header-level error.

The continued diagnostic output after these fixes identifies
`ObjList.h`'s PCH constants and enum, `CQLight.h`'s global owners, and
`MeshRender.cpp`'s D3DX matrix/effect facilities. Clang's previous
`MISSION_DATA` 72-byte assertion failure no longer appears after the
native union fix. These are compile findings, not native object execution.
The `GridVector` instruction pair has no native Windows comparison yet;
the `DBaseData` layout has native assertions but no cross-module Windows
ABI check. With a temporary
copy of the production source transformed as described above, and a
temporary rounding driver with the six cases described above, the
diagnostic commands were:

```text
c++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -I Code/Libs/Src/RendComp/PolyMesh -I Code/Libs/Src/TextureManager /tmp/frontierx-meshrender-preflight.cpp
clang++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -I Code/Libs/Src/RendComp/PolyMesh -I Code/Libs/Src/TextureManager /tmp/frontierx-meshrender-preflight.cpp
c++ -std=c++17 -O2 -I Code/App/Src -I Code/Libs/Include /tmp/frontierx-grid-rounding.cpp -o /tmp/frontierx-grid-rounding-gcc-o2 && /tmp/frontierx-grid-rounding-gcc-o2
clang++ -std=c++17 -O2 -I Code/App/Src -I Code/Libs/Include /tmp/frontierx-grid-rounding.cpp -o /tmp/frontierx-grid-rounding-clang && /tmp/frontierx-grid-rounding-clang
```

Both complete Linux GCC and Clang debug preset builds and CTest suites were
rerun after the include/declaration changes: each executed 40 checks
successfully and skipped the SDL GPU test before device creation. This
regression result covers the existing native graph, **not** the unbuilt
App translation unit. No native Windows Debug/Final, cross-module ABI,
retail runtime or game-image comparison was performed for these changes.
The next implementation decision is to replace the remaining PCH globals
and D3DX matrix/effect dependencies before compiling the App owner, then
compare its rounding and layout behavior against the Windows baseline.

### Direct production App translation-unit preflight (2026-09-27)

The temporary copy above established that three `ObjList.h` declarations
depended on values in the Windows PCH. Their one authoritative definition
is now `Code/App/Src/Include/AppObjectContracts.h`: selected-unit capacity
22, the `CQTIMING` slots and the complete `OBJID` interface-ID enum.
`Globals.h`, `IObject.h`, `ObjList.h` and `MGroupRef.h` include it. A source
comparison against the previous `Globals.h` found both enum bodies
byte-for-byte identical. Native compile-time assertions check the 32-bit
interface-ID size, the first two interface values and the timing end slot;
a tiny GCC/Clang executable checked the shared header's selected capacity
and representative values. The native forward declarations in that header
also let `ObjList.h` parse with GCC and Clang when the native legacy types
are preincluded, after correcting its `DACOM.H` include case. No saved or
network structure was widened.
Windows still needs its native Debug/Final rebuild and affected ABI checks.

`MeshRender.cpp` now selects its original PCH and `Globals.h` on Windows,
and directly includes the existing native legacy types plus
`NativeAppGlobals.h` on Linux. That focused declaration header lists the
App global pointers and flags used by this production caller and its
`CQLight.h` dependency, matching the Windows declarations without defining
or initializing them. The `vb_mgr` declaration matches the production
`VertexBuffer.cpp` owner. The unused `MungeFPCW` x86 control-word helper
remains Windows-only; its only call in this source is commented out. A
block around the existing model/view local keeps its construction at the
same render point while making the later visibility `goto` legal C++.
Native filename fallback now lowercases ASCII letters in place at the two
existing `strlwr` calls; the Windows function remains in use on Windows.
The native `WORD` declaration retains its 16-bit width for this source's
index locals.

GCC 16.2.1 and Clang 22.1.8 now syntax-check the **production source file
directly**, without a transformed copy. It still fails: the first missing
type is `D3DXMATRIX`; later diagnostics identify D3DX vectors/functions.
Source review also finds material effect calls requiring native behavior.
These are source/compile findings,
not a native object build or game runtime. The next change must replace
the D3DX matrix/vector operations explicitly and preserve the material
effect semantics before this App caller can enter the native graph.

The temporary `frontierx-objlist-header.cpp` contained only
`#include "FrontierX/LegacyFileTypes.h"` and `#include "ObjList.h"`.
Commands from the repository root:

```text
c++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -I Code/Libs/Src/RendComp/PolyMesh -I Code/Libs/Src/TextureManager Code/App/Src/MeshRender.cpp
clang++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -I Code/Libs/Src/RendComp/PolyMesh -I Code/Libs/Src/TextureManager Code/App/Src/MeshRender.cpp
c++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include /tmp/frontierx-objlist-header.cpp
clang++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include /tmp/frontierx-objlist-header.cpp
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
```

The full existing native graph built under both compilers; each CTest run
executed 40 passing checks and skipped the SDL GPU check before device
creation. This does not execute `MeshRender.cpp`. The input for the
existing game-material checks was generated repository data. Native Windows
Debug/Final, module ABI, retail runtime, visible game rendering and
same-input game-image comparison were not performed.

### Native App mesh math and 64-bit face storage (2026-09-27)

The direct `MeshRender.cpp` preflight above named D3DX math as its first
compiler blocker. Source review found only identity, scale, translation,
axis rotations, ordered multiply, transpose, vector length, cross and
normalization in this owner. `FrontierX/NativeD3dxMath.h` implements that
bounded set on the native path; Windows keeps the SDK D3DX implementation.
The native matrix stores 16 floats in row/column order, applies the
left operand before the right, and supports the source's in-place multiply
and transpose calls. Native vector code explicitly copies the game's
`Vector` components instead of aliasing them through an unrelated D3DX
type. The implementation follows Microsoft's documented
[matrix layout](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrix),
[left-to-right multiplication](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixmultiply)
and [Direct3D 9 transform convention](https://learn.microsoft.com/en-us/windows/win32/direct3d9/transforms).
The replacement is not yet compared against Windows floating-point output.

The existing `frontierx_math_core_probe` now checks row-vector scale and
translation composition, all three axis-rotation signs, in-place matrix
multiply and transpose, aliasing cross product, unit length and zero-vector
normalization. GCC 16.2.1 and Clang 22.1.8 Linux x86_64 debug builds both
compiled and executed the focused CTest. These are synthetic numeric cases;
they do not run a game mesh or shader effect.

After this replacement, direct GCC/Clang syntax checks of the production
App source exposed a 64-bit pointer-to-`U32` cast used as a material
batching ID and incomplete `ID3DXEffect` calls. The native renderer does
not yet implement normal normalization, texture factor or the effect pass
semantics; adding the verified D3D9 numeric constants 143, 60 and 3 to
the native declaration header only allows these call sites to be inspected.
The native backend continues to reject unsupported states. Microsoft's
[render-state values](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3drenderstatetype)
and [texture-factor argument](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dta)
are the references for those declarations.

`RenderBatcher::GetPrimBuffer` and `BMPOOL::get_material_from_state_id`
use `RPR_STATE_ID` as a shared material cache key. Other active App callers
set that key from texture IDs, while `MeshRender` adds offsets up to seven
to a material-derived base. The native `MeshRender` owner now maps each
material pointer to a stable `U32` base in the upper half of the state-ID
space, reserving eight IDs per material. This runtime map does not put a
pointer in `FaceGroupInfo::unique_id` and retains bases for process lifetime,
so pointer reuse has the same cache-identity caveat as the original Windows
pointer key. The native pipeline rejects and destroys newly created texture
handles above `0x7ffffff0`; the SDL owner independently stops allocating
above that bound. Existing App texture-ID offset callers therefore have
space below the `0x80000000` material range. The existing
`portability.render_pipeline` check now proves rejection and cleanup of a
callback that returns a material-range handle and acceptance of the last
permitted texture handle. This is a synthetic CPU contract check; no game
batch or material-ID reuse ran.

The three App D3DX effect-pass branches now return after render-state
cleanup on the native path; the Windows SDK effect calls remain in their
original branches. This is an explicit unsupported path, not a native effect
implementation. GCC and Clang direct syntax checks now pass, and the new
`frontierx_app_mesh_render_owner` CMake object builds under both compilers.
It is not linked into a native game process, and its draw methods did not
execute. The remaining effect, shader and render-state semantics must be
ported before this owner can render a game scene.
`nm -u -C` on the GCC object found 71 unresolved symbols before filtering
standard-library and runtime imports. The production closure includes App
globals (`BATCH`, `CQBATCH`, `PIPE`, `REND`, `ENGINE`, `MATMAN` and others),
`SetupDiffuseBlend`, `TRANSFORM::rotate_about_j` and `ICQImage` diagnostics.
The next link step must supply those actual owners and the associated
`MeshInfo` lifecycle; no placeholder definitions were linked for this check.

The same App source had seven face-buffer alignment sites that converted
a native allocation pointer to `U32`. All now use pointer-sized
`std::uintptr_t` arithmetic and preserve 32-byte alignment. Three clone
sites also aligned the *source* buffer instead of the newly allocated
clone buffer; they now use each clone's own `faces_ptr`. The two vertex
array destructors now delete through their allocation element types.
`PremultiplyTexture` uses `U32` dimensions as required by its pipeline
interface, and native-only legacy declarations include the actual render
state constants. These owner changes have object-compilation evidence only:
`MeshRender.cpp` is not yet linked into a native executable, and clone ownership
has not executed under a sanitizer. Native Windows Debug/Final and ABI
checks are still required for the shared App source.

Commands from the repository root:

```text
cmake --build --preset linux-gcc-debug --target frontierx_math_core_probe -j4
ctest --preset linux-gcc-debug -R '^portability.math_core$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_math_core_probe -j4
ctest --preset linux-clang-debug -R '^portability.math_core$' --output-on-failure
c++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -I Code/Libs/Src/RendComp/PolyMesh -I Code/Libs/Src/TextureManager Code/App/Src/MeshRender.cpp
clang++ -std=c++17 -fsyntax-only -I Code/App/Src -I Code/App/Src/Include -I Code/App/DInclude -I Code/Libs/Include -I Code/Libs/Src/Include -I Code/Libs/Src/RendComp/PolyMesh -I Code/Libs/Src/TextureManager Code/App/Src/MeshRender.cpp
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_app_mesh_render_owner -j4
cmake --build --preset linux-gcc-debug --target frontierx_render_pipeline_probe -j4
ctest --preset linux-gcc-debug -R '^portability.render_pipeline$' --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_app_mesh_render_owner -j4
nm -u -C out/build/linux-gcc-debug/CMakeFiles/frontierx_app_mesh_render_owner.dir/Code/App/Src/MeshRender.cpp.o > /tmp/frontierx-app-mesh-undefined.txt
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
```

Both full native graphs built and their CTest presets passed;
the SDL GPU test skipped before device creation (see `BUILD_GRAPH.md` for
the full preset count). Existing game fixture data is repository-generated.
No retail data, native Windows runtime, GPU
game draw or same-input image comparison was executed for this change.

### Native App mesh lifecycle and angle owner (2026-09-27)

`MeshRender.cpp` calls the production `MeshInfo` constructor, destructor,
copy and split paths. `MeshInfo.cpp` was therefore added to the same App
object target. Its first direct GCC/Clang syntax checks stopped at the
Windows PCH, then at the missing `IEngine`/`vis_state` declarations. A
native include branch and the production `ICamera.h` then `ENGINE.H` order
resolved those compile blockers without changing Windows includes or mesh
storage. The exact production `MeshInfo.cpp` now compiles under both
Linux x86_64 compilers; no constructor, copy, split or destruction path
has executed natively.

The first manual link of these App objects with the existing production
game-material/Engine/RenderMgr/PolyMesh fixture executable failed with
24 distinct undefined symbols and no duplicate definitions. The unresolved
App transform method `TRANSFORM::rotate_about_j` is owned by
`SuperTrans.cpp`. Its native include branch now compiles that production
file in the App object target. The same source calls the Windows assembly
`get_angle` routine; `NativeAppMath.cpp` now supplies its native scalar
counterpart. Source review of the supported Windows routine establishes
`atan2(x, y)` quadrant ordering and a rounded `1.570796` axis value;
at the origin it returns the negative axis value. The existing
`portability.math_core` check now exercises four quadrants, both signed
axis results and the origin through the native function under GCC 16.2.1
and Clang 22.1.8. This is synthetic numeric execution, not a Windows
floating-point oracle or game simulation determinism result.

A second manual link, using the generated GCC game-material executable
link line plus the three App objects and `libfrontierx_native_app_math.a`,
reduced the distinct undefined set from 24 to 23 with no duplicate
definitions. The remaining names are App global service pointers and
flags, `SetupDiffuseBlend`, the App `ICQImage` diagnostics and `camMoved`.
The native shell currently defines only some of these (`CQFLAGS`,
`CQRENDERFLAGS`, `CQEFFECTS`, `PIPE` and diagnostics) inside its process;
the game-material fixture does not link that shell. Connecting the real
service owners in a native App composition is the next link gate. Neither
manual link produced an executable; there was no App mesh draw.

Focused commands from the repository root:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_app_mesh_render_owner frontierx_math_core_probe -j4
ctest --preset linux-gcc-debug -R '^portability.math_core$' --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_app_mesh_render_owner frontierx_math_core_probe -j4
ctest --preset linux-clang-debug -R '^portability.math_core$' --output-on-failure
ninja -C out/build/linux-gcc-debug -t commands frontierx_game_material_manager_probe
```

Both full Linux debug builds and CTest presets were rerun after adding
the three App objects and native angle owner; their rolling totals are in
`BUILD_GRAPH.md`. The GPU test skipped before device creation. Inputs were
repository source and generated synthetic fixtures. Native Windows
Debug/Final, ABI/layout, Windows angle-oracle, sanitizers on App mesh
ownership, retail runtime, visible game scene and same-input image
comparison remain unperformed for these changes.

A second read-only Clang syntax preflight used a temporary copy of the
production `PolyMesh/polymesh.cpp` with native legacy declarations and
case-corrected includes. After resolving five additional include-case
aliases outside the source tree, compilation reached shared `ENGINE.H` and
failed at `DAVariant.h`'s duplicate `S32`/`int` and `U32`/`unsigned int`
constructors on LP64, an incomplete `vis_state` enum declaration,
`JointInfo.h`'s `stricmp`, and `Tfuncs.h`'s undeclared `g` in `Tgain`.
This is an inventory of the component's shared-header closure, not a
successful `PolyMesh` object build. The temporary aliases were not added to
the repository, and no game component or scene has executed on Linux.

## Production PolyMesh component compile and factory gate, 2026-09-26

The read-only preflight identified actual C++17 blockers in shared headers.
Production `PolyMesh/polymesh.cpp` now compiles as
`frontierx_polymesh_component_owner` with GCC and Clang on Linux x86_64,
without temporary include aliases. The native build corrects the `std::map`
allocator value type in `handlemap`; Win32 retains its original default.
`DAVariant` omits redundant native `int` overloads because native `S32` and
`U32` already alias `int` and `unsigned int`. Its fixed-width `PROPERTY`
conversion preserves 32-bit integer values but returns an empty variant for
pointer-shaped values that cannot fit in that field on LP64. The linked check
observes both cases. No `PROPERTY` field width or public vtable changed.
`JointInfo` compares its ASCII joint names without a platform CRT call on
native Linux while retaining `stricmp` on Windows. `Tgain`'s undeclared `g`
was corrected to its `b` parameter; that template has no call in this
component. Relevant includes now match on-disk case.

The component's old-material render path formerly cast `Material *` to `U32`
for `RPR_STATE_ID`. Its native path now retains a typed pointer key and
assigns a monotonic 32-bit batch-state ID when a material or its `unique`
revision first appears. Each material reserves eight IDs for the passes in
that function and reports exhaustion before wraparound. The Windows key
expression is unchanged. This mapping is compiled but has not been exercised
by a draw. Distinct component instances, material lifetime and batching
across components need runtime evidence before acceptance. The native
`IRenderPrimitive::set_state` still reports unsupported for this slot.
Missing D3D9 sampler, filter, address and `D3DTOP_ADD` declaration values
were checked against Microsoft's
[sampler](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dsamplerstatetype),
[filter](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dtexturefiltertype),
[address](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dtextureaddress),
and [texture-operation](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dtextureop)
enumerations. These declarations compile the owner; they do not implement
native sampler or multi-stage behavior.

The component now registers its production `DAComponentFactory` with the
native DACOM manager. The existing linked game-material probe calls that
factory with the real `RendCompDesc` class name and deliberately missing
engine services. Construction reaches `POLYMESH::init`, returns
`GR_INVALID_PARMS`, and releases the instance without a sanitizer finding.
This check answers whether the production component can link and enter its
factory contract; the previous material probe could not cover that boundary
without the component object. It is synthetic runtime, not successful
component initialization, `render_instance`, a game draw or a native scene.
The next integration decision depends on providing actual `IEngine`,
`IRenderPipeline`, `IRenderPrimitive`, `ITextureLibrary` and
`IMaterialManager` services. The app `MeshRender.cpp` header and vertex-buffer
closure remains open.

Executed with repository-generated UTF material, mesh and texture data:

```text
cmake --build --preset linux-gcc-debug --target frontierx_polymesh_component_owner -j2
cmake --build --preset linux-clang-debug --target frontierx_polymesh_component_owner -j2
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
cmake --build --preset linux-gcc-debug -j2
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
ctest --preset linux-clang-debug --output-on-failure
```

Both compilers built the component object and linked the separate production
game-material executable. All three focused CTests per compiler passed, and
all three focused Clang ASan/UBSan runs printed `production game material
load passed`. LeakSanitizer remained disabled for this host's ptrace
environment. A build-preset audit found that the new game-material executable
was missing from both fixed target lists. It was added to both presets;
subsequent full GCC/Clang debug preset builds rebuilt the linked executable
from its current sources before CTest. Each preset executed 40 CTests; the
sandbox GPU CTest skipped before initialization.
Native Windows Debug/Final rebuilds, affected module ABI checks, retail
runtime, a valid game engine-service composition, game `DrawVB`, visible GPU
scene and same-input Windows/Linux comparison remain unperformed for this
component change.

## Production Engine and PolyMesh archetype closure, 2026-09-26

The next named blocker was `POLYMESH::init` requesting `IID_IEngine` from
its engine-services descriptor. The production `Engine/engine.cpp` owner now
compiles as `frontierx_engine_owner` under GCC and Clang on Linux x86_64.
Its native path uses exact-case includes, standard finite-value checks for
compound bounding spheres, a bounded anonymous-archetype name formatter and
a native DACOM factory instead of its Win32 DLL entry. The old Win32 entry,
CRT checks and formatter remain in the Windows branch. `Engine2.h`,
`PERSISTCOMPOUND.H` and `EngineArchetype.h` include names were corrected to
their on-disk case. `COMPTR` now accepts only zero in its native integer
comparison, avoiding fabrication of a pointer from a 32-bit integer; its
Windows behavior is unchanged. No engine interface vtable, persisted
compound layout or Win32 project source list changed.

The existing game-material executable now links the production engine object,
registers its `IEngine` factory and creates an actual engine service. It also
creates the registered native `IRenderPrimitive`, while its submission
callback explicitly returns `GR_NOT_IMPLEMENTED`. A small test service
router supplies that primitive, the existing native `IRenderPipeline`, and
the production `TextureLibrary` and `IMaterialManager` to the real
`PolyMesh` factory. `POLYMESH::init` returns `GR_OK`. This is synthetic CPU
composition with real production owners and a test router, not game startup
or a rendered scene.

The generated game-mesh UTF archive now places its original CQ2 content
under the exact `openFLAME 3D N-mesh` child directory requested by
`POLYMESH::create_archetype`. The direct `XMesh::read` check opens that child
view; the component receives the archive root and opens the child itself.
With the real material and texture owners, `create_archetype(1, meshArchive)`
returns true, `get_archetype_mesh(1)` returns a mesh, and
`destroy_archetype(1)` releases it. The native `IRenderPipeline` does not
provide the legacy device-info or blend-matrix queries; `get_pipe_info` now
initializes its outputs and uses `RP_D_UNKNOWN` when device info is
unsupported, so this archetype path does not read indeterminate data.
The test exercises production component loading and release, but does not
create an engine instance, call `render_instance`, execute game `DrawVB`,
produce GPU pixels, or compare with Windows.

The linked check extends the existing material fixture because that fixture
already supplies the exact game manager, texture archive and `XMesh` input.
Its pass changes the integration decision from supplying `IEngine` to
connecting an engine instance and the app/primitive draw boundary. The
test router is not a replacement for the supported game's `RenderMgr` or
`Conquest` startup graph.

Executed on Linux x86_64 using only repository-generated UTF input:

```text
cmake --build --preset linux-gcc-debug --target frontierx_engine_owner -j2
cmake --build --preset linux-clang-debug --target frontierx_engine_owner -j2
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

GCC and Clang compiled and linked the actual engine/component closure. All
three focused CTests per compiler passed, and all three focused Clang
ASan/UBSan runs printed `production game material load passed` without a
reported finding. LeakSanitizer remains disabled for this host's ptrace
environment. Both full Linux debug presets rebuilt successfully after this
engine/archetype change; their matrix result is recorded in
`BUILD_GRAPH.md`. Native Windows Debug/Final rebuilds, affected Engine/PolyMesh
ABI checks, retail runtime, engine-instance rendering, App `MeshRender.cpp`,
hardware GPU output and a same-input Windows/Linux comparison were not
performed for this change.

## Unclaimed Engine archetype cleanup, 2026-09-26

Source review of the next production `IEngine::create_archetype` call found
that its no-component-success path erased a `handlemap` entry and then read
the invalidated iterator to delete the archetype. `Engine/engine.cpp` now
retains the allocated pointer before erasing the entry, then deletes it.
This changes neither the engine interface nor its archetype and instance
layouts. The existing game-material probe supplies the real production
`IEngine` and UTF archive; it requests an archetype that no registered engine
component claims and verifies `INVALID_ARCHETYPE_INDEX` plus absence from
name lookup. This check represents the actual archetype owner and failure
caller. The prior successful `PolyMesh` archetype check cannot exercise the
no-component branch. If this rejection fails, the component-loading boundary
is not safe to extend to engine instances.

After the change, the focused Clang game-material CTests passed for all three
generated inputs, and all three focused Clang ASan/UBSan executable runs
reported `production game material load passed` without a finding. Both
full GCC 16.2.1 and Clang 22.1.8 Linux debug build presets completed; their
matrix result is recorded in `BUILD_GRAPH.md`. Commands:

```text
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

This is synthetic CPU execution with repository-generated inputs. Native
Windows Debug/Final rebuilds and module ABI checks, retail data, engine
instance rendering, hardware GPU pixels and cross-backend comparison were
not performed for this cleanup change.

## Production RenderMgr and engine instance closure, 2026-09-26

The supported `Conquest.ini` selects `IRenderer` in `[Engine]` and `PolyMesh`
in `[RenderManager]`. The actual owner is
`EngComps/RenderMgr/RenderMgr.cpp`: `ENGINE::load_engine_components` aggregates
it, it creates `PolyMesh` through DACOM, and it forwards engine archetypes and
instances. The earlier direct `PolyMesh` check did not cover this dispatch.
The falsifiable gate was whether the linked production owners could load
those profile entries, create one game mesh archetype and instance, expose
its mesh through `IRenderer`, then release them. A failure would move work
back to the engine/component ownership boundary before attempting a draw.

`frontierx_render_manager_owner` now compiles the production `RenderMgr.cpp`
and convex-hull helper under GCC and Clang. Its native registration keeps the
existing `IRenderer` DACOM factory contract; the Win32 DLL entry is unchanged.
Exact-case include corrections and Windows-only header/diagnostic guards
allow compilation. The local LOD count now uses `U32`, matching
`ReadAllocVector`'s fixed-width count; this is a local variable, not a disk
or module layout change. Visible-rectangle extrema now use 32-bit `S32`
limits instead of LP64 `long` limits. LOD directory names use bounded decimal
formatting, and profile entries with leading spaces are parsed as intended.
The native D3D9 declaration of `D3DRS_CLIPPING = 136` matches Microsoft's
[render-state enumeration](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3drenderstatetype);
this declaration does not implement clipping in the native backend.

The native DACOM manager now holds an explicitly configured production
`IProfileParser` service and returns an owned reference from
`QueryInterface`. The existing game-material executable initializes the real
parser from a generated `[Engine]`/`[RenderManager]` profile and binds it to
the process-global native manager. With the existing production material,
texture, pipeline, primitive, Engine and PolyMesh services, the production
Engine loads one `RenderMgr` component, loads `PolyMesh`, creates a CQ2 mesh
archetype, creates an instance through `create_instance2`, and retrieves a
non-null mesh for that instance through the real `IRenderer`. It replaces the
default owned handler once, destroys the instance and releases the archetype.
The parser binding is currently made by this synthetic CPU composition;
the native shell does not yet initialize this game-component profile.

Clang ASan initially reported a new/delete type mismatch in
`ENGINE::destroy_instance_tree`: the Engine allocated `LocalEngineInstance`
but deleted it through `IEngineInstance`, which has no virtual destructor.
Both owned-handler release sites now cast to the known `LocalEngineInstance`
type under the existing `EI_F_DELETE_INSTANCE_HANDLER` flag. The public
interface/vtable and client-owned handler path are unchanged. Repeated
focused ASan/UBSan runs passed for the generated standard, CR and missing-END
inputs; the replacement call covers the second release site. LeakSanitizer
was disabled for this host's ptrace environment. The same focused CTests
passed with GCC and Clang; both full debug build and CTest presets completed,
with their overall matrix result recorded in `BUILD_GRAPH.md`.

Executed on Linux x86_64 with repository-generated profile, UTF mesh,
material script and texture data:

```text
cmake --preset linux-gcc-debug
cmake --preset linux-clang-debug
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

This is synthetic CPU execution of production owners, not a native game
scene or a retail runtime result. `render_instance`, the active App
`MeshRender.cpp`/`IMaterialManager::DrawVB` caller and GPU output remain
unexecuted. Native Windows Debug/Final rebuilds, affected module ABI checks,
retail runtime and same-input D3D9/Vulkan comparison were not performed for
this change. The next production gate is the camera, state and game-vertex
boundary required to call `render_instance` without fabricating success.

## First production render-instance call and math provider (2026-09-26)

The existing generated engine instance was the real caller for the next
gate. The game-material executable now passes a minimal `ICamera` test
implementation through production `IEngine::render_instance`, `RenderMgr`
and `PolyMesh`. Its measured behavior is a reached inverse-transform query
and no native draw callback. An initial focused Clang UBSan execution stopped
in `Transform::multiply` because `MATH_ENGINE()` had no registered
`I3DMathEngine`; this identifies a production service, not a camera or mesh
fixture defect. The standard scalar `x86Math/x86math.cpp` provider now builds
and registers for this executable. A bounded transform-composition check
observes the expected translated result before the render call. Detailed
provider provenance, numerical limits and remaining Windows comparison are
in [MATH_BACKEND.md](MATH_BACKEND.md).

At the time of this first render-instance gate, the native `IRenderPipeline`
rejected `RP_TEXTURE` state lookup. The newer pipeline contract and next stop
are recorded above.
`POLYMESH::render` now initializes the output and returns `VS_UNKNOWN` when
that lookup fails, before submitting a face group. Its optional
`ILightManager` is checked before requesting active lights; the existing
`POLYMESH::init` contract permits that service to be absent. The test
camera proves traversal into the production render owner; a zero draw count
proves this run did not submit geometry. `RenderMgr` retains its existing
visibility return semantics and currently ignores the component's return
value, so an outer visibility result is not evidence of a completed draw.
The camera is test scaffolding for this named blocker, not a native game
camera or an alternative renderer.

After the provider and render-state rejection changes, focused GCC/Clang
game-material checks, both full Linux debug presets, and the generated
standard/CR/missing-END Clang ASan/UBSan inputs passed. The full preset
result is recorded in [BUILD_GRAPH.md](BUILD_GRAPH.md). Commands:

```text
cmake --preset linux-gcc-debug
cmake --preset linux-clang-debug
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

Inputs remain repository-generated UTF/profile/texture data. This is a
synthetic CPU result, not game scene, GPU, Windows or retail runtime.
Model/view transform propagation, lighting and texture states, the active
`IMaterialManager::DrawVB` path and visible GPU output remain unimplemented or
unexecuted at this game boundary. Native Windows
Debug/Final rebuilds, module ABI checks, retail checks and same-input image
comparison were not performed for this change.

## Production game material manager, 2026-09-26

The named integration blocker was the supported game's `IMaterialManager`
owner: `XMesh::read_material_library` and the app `MeshRender.cpp` both call
its `FindMaterial`, and the app later calls `IMaterial::DrawVB`. The prior
mesh check supplied a counting interface instead of this owner. The new
`frontierx_game_material_manager_owner` target compiles the exact production
`MaterialManager.cpp`, `Material.cpp` and `Modifier.cpp` sources. A separate
`frontierx_game_material_manager_probe` links and registers the production
DACOM aggregate, production `TextureLibrary`, game `ITManager`, and production
`XMesh` owner.
It stays separate from the shading-library probe because
both unrelated systems declare a global `IMaterial` type; linking them into
one program would cross that type boundary without a valid contract.

The first GCC object build stopped at Win32 headers, exact-case includes,
MSVC's nested anonymous-union type extension, and D3D9 constants absent from
the native declaration header. The sources now compile with the existing
native legacy declarations; Windows keeps its original includes, dialog and
DLL registration. The `Modifier::ColorValue` type declaration moved outside
its anonymous union without moving or changing the union member. The native
header gained the exact D3D9 fill, shade, cull, render-state and texture-stage
values needed by this owner, checked against Microsoft's published
[render-state](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3drenderstatetype),
[fill-mode](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dfillmode),
[culling](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dcull) and
[texture-stage](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dtexturestagestatetype)
enumerations. No serialized material structure or Win32 public vtable changed.

`Material::DrawVB` exposed the expected LP64 pointer-to-`U32` cast. The
native `MeshRender.cpp` call site now passes a typed
`IDirect3DVertexBuffer9` compatibility object containing the existing
`IRP_VERTEXBUFFERHANDLE`; the native `DrawVB` owner reads that field instead
of converting an address to `U32`. The app caller is not yet a Linux build
target, so this is source-level wiring plus material-owner compilation, not
an executed draw. Windows retains the D3D9 pointer path. Native D3DX effect calls are not made, and the Windows material editor
and DLL entry remain Windows-only. This is a compile boundary, not a native
`DrawVB` execution or successful game render. The native indexed-buffer
overload and full `StandardVertex` format remain unsupported by the current
renderer.

The first linked synthetic check discovered another real owner contract:
`MaterialManager::parseShader` and `Material::Load` request file
`IFileSystem` instances and read them using a null file handle. The native
UTF adapter previously created only directory views. It now returns a
read-only file view for an exact-case UTF entry and serves `GetFileSize`,
`ReadFile`, position and seek through that view's implicit stream. The
existing material-library check verifies the six-byte `Type` file view, and
the new game-manager check reads both script and material through the
production calls. Wrong-case and traversal requests still fail.

Source review of the supported game's `TextureManager.dll` identifies the
next texture boundary more precisely. Its production
`TManager::CreateTextureFromFile` calls `createTextureFromFile`, which obtains
an `IFileSystem` file view, calls `CreateFileMapping`/`MapViewOfFile`, passes
the bytes to a TGA reader and uploads through `IRenderPipeline`. The native
UTF file view previously returned unsupported for mapping, so a material
texture could not reach that owner even with `ITManager` linked. It now
supports a bounded read-only, whole-file mapping on an exact-case file view;
map/unmap/close state is checked by the existing material-library probe using
the six-byte `Type` file. The mapping check alone is synthetic CPU execution
of the filesystem contract; the subsequent linked check below executes the
production TGA upload. The native mapping rejects writable, offset and
oversized view requests.
The native `PAGE_READWRITE=0x04` rejection value matches Microsoft's
[file-mapping protection](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createfilemappinga)
declaration.

The generated UTF 1.01 archive has `shaders/Synthetic.fx`,
`textures/Tile.tga` and `materials/Base.txt`. Its shader script declares
`Synthetic`, a code-register float `Gain` with default `0.25`, and a channel
zero `Diffuse` texture. The saved material supplies `Gain=0.75` and the
`Tile.tga` name. The test observes production DACOM creation,
`Initialize`, `FindMaterial("Base.txt")`, `GetName() == "Base.txt"`,
`GetFloat("Gain") == 0.75`, and no result for `Missing.txt`. A second generated
UTF archive has the same mesh geometry as the older-format fixture, but its
`CQ2_Material_Library` contains `Base.txt`. The production `TextureLibrary`
loads its empty mesh texture directory; `XMesh::read` loads five vertices and
two faces, resolves the material through the production manager, and calls
its `Realize`. The check observes the exact same `IMaterial` pointer in
`mesh.imaterial_list[0]` and the unchanged saved `Gain` value after that call.
`Realize` then calls the production `ITManager`, which reads the generated
TGA through a UTF file mapping, decodes it, creates a pipeline texture and
uploads all 16 expected RGBA pixels. The check observes handle 64, one
create/upload, and one destroy after `Flush`; a second explicit load with
`DA::UNKTYPE` infers the TGA extension and again uploads and destroys the
texture. This is synthetic CPU execution of the actual game
mesh/material/texture-owner closure. The native pipeline's `load_effect`
returns null; `IsRealized` and the texture upload do not establish shader
compilation or a game draw. No app `MeshRender.cpp`,
`DrawVB`, GPU output or native game scene executed.

### Production game texture closure

The concrete blocker was `Material::Realize` calling the supported game's
`ITManager::CreateTextureFromFile`, while the linked Linux check had passed a
null `TMANAGER`. The new `frontierx_game_texture_manager_owner` target
compiles the production `TextureManager.cpp`, `Tgaread.cpp` and `BmpRead.cpp`
sources for Linux x86_64. The existing game-material probe registers that
production DACOM aggregate, initializes it with the native pipeline and
passes it to `IMaterialManager::Initialize`. Windows keeps its DLL entry,
precompiled header and existing factory path. VFX uses Win32 assembly
functions and remains explicitly unsupported in the Linux TManager closure;
the standalone VFX source was not linked or presented as a native capability.

The generated TGA is uncompressed BGRA 4-by-4, with all 16 pixels set to
`(R,G,B,A)=(17,34,51,255)`. The game material script and saved material
both name the same texture parameter, so the production `Material::Realize`
call reaches the real game texture manager. That owner opens `Tile.tga`
through the native UTF mapping and invokes its production TGA reader.
The renderer's `PF_4CC_DAA8` format class maps to RGBA8 on the native
pipeline, matching the supported D3D9 owner's declared preference for
32-bit 8:8:8:8 within that class. The probe's bounded backend callback
records one 4-by-4 RGBA8 upload, verifies all 64 bytes, and observes
texture destruction after `ITManager::Flush`. It reloads the texture with
`DA::UNKTYPE` to exercise the production extension-selection branch and
observes a second create/upload/destroy sequence. This is a CPU callback;
there is no visible Vulkan or Windows same-input TGA capture yet.

The first focused Clang ASan/UBSan execution of that production path
identified two undefined operations: unaligned 16-bit loads in the TGA
header reader and a signed left shift in `nearestPower`. The TGA header
reader now assembles little-endian values bytewise, preserving the Win32
x86 data interpretation, and `nearestPower` uses checked unsigned shifts.
The extension selector also reads four suffix bytes directly instead of
reinterpreting a possibly unaligned filename tail as `U32`; it preserves
the original ASCII case-insensitive `.BMP`, `.SHP` and `.TGA` choices.
Unknown suffixes and failed mappings now release any opened file mapping
before returning. `BITMAPINFOHEADER` and `RGBQUAD` have native fixed-width
declarations with 40-byte and 4-byte layout assertions; no on-disk header
width, game-material public vtable, or supported Windows project source
list changed. TGA decoding beyond this generated uncompressed 32-bit
sample, BMP decoding, VFX, mip levels beyond one, effect compilation,
game material `DrawVB` and visible GPU output remain unverified.
The production file loader now obtains `IFileSystem::GetFileSize` from the
mapped file view and passes the size through its private decode helper to
`IImageReader::LoadImage`. On native Linux, the TGA reader validates the
18-byte header, origin, 24/32-bit uncompressed true-color mode, 2048-pixel
dimension limit, ID field and complete pixel payload before reading any
pixel. `createTextureFromMemory` checks the reader result and dimensions
before uploading. A truncated 4-by-4 TGA with only the header is rejected by
the production reader in the existing game-manager probe; the valid generated
TGA still uploads all 64 RGBA bytes through the production manager. This
is synthetic CPU execution with repository-generated data. Compressed,
indexed and paletted TGA file input now fails explicitly on the native path
until a bounded decoder is integrated. The public unsized
`ITManager::CreateTextureFromMemory` entry and BMP/VFX readers do not have
equivalent byte-bound guarantees. Windows keeps its existing TGA decode
behavior; native Windows rebuild and retail texture-format checks remain
unperformed.

The first CR-only script run exposed an out-of-bounds read in the production
parser: it unconditionally skipped one byte after each carriage return and
continued beyond the EOF marker when `END_MAT_SCRIPT` was missed. Both
`parseShader` and `reloadShader` now skip a line-feed only when present and
stop their inner loops at EOF. The registered CRLF and CR fixtures both
preserve the saved `Gain` value. The focused Clang ASan/UBSan run originally
reported the `getNextLine` heap-buffer-overflow for CR; both inputs now finish
without a reported finding. The test is justified by this actual parser
regression and the missing production material closure; the earlier
shading-library test cannot cover the game interface.

The saved-material reader had a separate EOF boundary: `IFileSystem::ReadFile`
can succeed with zero bytes, but `readLine` used its return status alone and
the `Material::Load` loop depended on an `END` line. The reader now checks the
byte count, reports whether a line was read, and stops the parameter loop at
EOF. The existing production-manager probe has a third generated archive
whose material ends after the saved float without `END`; it still resolves
`Gain=0.75` and terminates. This is a malformed-input regression check for
the actual game loader, not evidence of a valid retail material format.

Executed on Linux x86_64 with repository-generated UTF bytes and no retail
media:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_texture_manager_owner -j2
cmake --build --preset linux-clang-debug --target frontierx_game_texture_manager_owner -j2
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.game_material_manager' --output-on-failure
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe frontierx_game_material_manager_probe -j2
cmake --build --preset linux-clang-debug --target frontierx_state_material_probe frontierx_game_material_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.(state_material|game_material_manager)' --output-on-failure
ctest --preset linux-clang-debug -R '^portability.(state_material|game_material_manager)' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe frontierx_game_material_manager_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
cmake --build --preset linux-gcc-debug -j2
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
ctest --preset linux-clang-debug --output-on-failure
```

All three focused CTests and both full debug presets passed; the central build
document records the full CTest count and sandbox GPU skip. All three focused
sanitizer runs printed `production game material load passed` with no
reported finding; LeakSanitizer was disabled for this host's ptrace
environment. The existing material-library CTest initially failed because
it still expected file-view creation to be rejected; its expectation was
updated to verify the now-required implicit file stream, and both full
presets passed on rerun. The read-only mapping check also passed a focused
Clang ASan/UBSan run. Native Windows Debug/Final rebuilds, module ABI
checks, retail runtime, a native game-scene mesh draw, `DrawVB` runtime, hardware GPU
output and a same-input Windows/Linux comparison remain unperformed for this
change.

## CQ2 lookup and material-count bounds, 2026-09-26

The existing generated mesh UTF fixture now also emits a
`CQ2_Material_Library` directory with one material-name child and a declared
count of one. Synthetic CPU execution of production `XMesh::read` rejects a
null `IMaterialManager` and restores the parent directory. Reopening the same
archive with a counting test implementation of `IMaterialManager` observes
exactly one `FindMaterial("Base")` call. The test returns no `IMaterial`, so
there is no script parsing, `Realize`, `DrawVB` or game-material execution.
This check protects the actual production mesh caller to the material-manager
interface while its Windows/D3DX owner is still absent from the Linux graph.

The CQ2 and older material branches now require a readable nonnegative
`Material count`, initialize absent CQ2 pointer entries, and stop before a
material directory would exceed the declared allocation. An older-format
fixture with count zero and one material child returns failure rather than
writing past a zero-length allocation. `XMesh::read_material` also rejects a
missing or out-of-range `Material identifier` before using the entry. These
are malformed-input boundaries; the valid older-format mesh continues to
load its material and production texture frame. The CQ2 success path now
restores the parent directory, matching the older-format branch. No mesh
storage layout, on-disk width, public vtable, valid material order or Windows
project source list changed.

Executed on Linux x86_64 with generated repository-owned UTF bytes and no
retail media:

```text
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R '^portability.state_material$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_state_material_probe -j2
ctest --preset linux-clang-debug -R '^portability.state_material$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
cmake --build --preset linux-gcc-debug -j2
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
ctest --preset linux-clang-debug --output-on-failure
```

Both focused CTests passed, including the valid older material, null-manager
CQ2 rejection, one CQ2 name lookup and count-mismatch rejection. The focused
Clang ASan/UBSan executable completed without a reported finding;
LeakSanitizer was disabled for this host's ptrace environment. Both full
Linux debug presets built and passed their non-GPU CTests; the GPU CTest
skipped at SDL video initialization in the command sandbox and is not GPU
runtime evidence. Native Windows
Debug/Final rebuilds, module ABI checks, production CQ2 manager execution,
retail runtime, game rendering and a same-input Windows/Linux comparison were
not performed for this change.

Source review of `MaterialManager.cpp`, `Material.cpp` and the active app
`MeshRender.cpp` narrows that gate. `MaterialManager::Initialize` obtains
`shaders`, `textures` and `materials` child filesystems from `MATDIR`, then
parses scripts and material definitions. The CQ2 mesh branch calls
`FindMaterial` and `Realize`; its draw caller passes a D3D9 vertex-buffer
pointer to `IMaterial::DrawVB`. `Material::DrawVB` currently casts that
pointer to `U32` for `IRenderPipeline::draw_indexed_primitive_vb`. Such a cast
would truncate a real pointer on Linux x86_64, while its D3DX effect and
editor-dialog code also require separate owners. The next native adapter
must distinguish a pipeline handle from a D3D9 pointer at the existing
boundary and reject values it cannot represent; it must not put native
addresses into serialized or runtime `U32` handles. This is source-review
evidence only: no CQ2 material-manager source compiled or executed natively.

## Executed XMesh UTF and face-sort path, 2026-09-26

The active owner `Mesh.cpp` was linked into the existing material CPU check,
alongside its production `sortQ.cpp` owner and the already integrated
`TextureLibrary`. The `pm_simple_map_gen` function was moved unchanged from
`polymesh.cpp` to `Mesh.cpp`, where the `XMesh` constructor actually references
it. Windows still builds both sources and retains one definition. The native
math accessor supplies the existing `3dmath.h` service. No new game or
renderer implementation was introduced.

The generated UTF 1.01 mesh fixture has the production `Face groups`,
`Vertices`, `Material library` and `Texture library` directory names. Its one
group contains two triangles: the smaller face appears first in storage.
`XMesh::read` reads the face and vertex arrays through the native
`IFileSystem`, calls production `TextureLibrary::load_library` on the empty
texture directory, creates the older `Material library` entry through
`XMesh::read_material` and `Material::load_from_filesystem_old`, computes
face areas and sorts the group. The check
observes five object/batch vertices, two faces, descending areas 2.0 and
0.5, the larger face's original vertex chain at index zero, a 0.6/0.6
centroid and positive radius. It observes material name `Base`, its
0.25/0.5/1.0 diffuse bytes after legacy conversion, 0.5 transparency and
the group-to-material index zero. The old-format `Texture name` field refers
to `Tile`, which the production texture owner already loaded from the other
synthetic archive. `Material::update` resolves that reference to native frame
handle 64; the later owner-lifetime checks observe its final destruction
after all references are released. The test constructs and destroys the
production `XMesh` owner. The mesh archive's own texture directory is empty;
`IMaterialManager`, CQ2 material scripts, game drawing and GPU output are
not covered.

A second generated archive names a texture absent from the production
library. The old-format loader originally reached `PixelFormat::has_alpha_channel`
with an uninitialized local format when the named image could not be resolved.
It now checks that a format was obtained or replaced with its existing RGB565
fallback before consulting the alpha channel. The production texture owner
creates a temporary named placeholder for an unresolved `get_texture_id`;
the check therefore requires no published native frame, no alpha flag, and
removal of that placeholder after `XMesh` destruction. This is a failed-name
rollback check through the real loader, without changing the serialized
material fields or the successful texture path.

The sort exposed a real LP64 layout defect before execution: `Swap32` cast
four-byte face indices, normals, properties and float fields to
`unsigned long *`, which addresses eight bytes on Linux. It now swaps exactly
one `U32` of bytes with `memcpy`, preserving the Win32 four-byte exchange and
avoiding cross-type aliasing. The production `sortQ` definition now uses
`size_t` for the two parameters already declared as `size_t` by its caller;
this equals `unsigned` on supported Win32 and fixes the LP64 function type.
It rejects counts beyond the callback's `int` index range and multiplication
overflow before pointer arithmetic. No mesh field, serialized value,
simulation update order or normal successful face ordering changed. The
fixture contains distinct face areas, so its final ordering does not depend
on the sort's existing random pre-shuffle.

Executed on Linux x86_64 using generated repository-owned UTF bytes with no
retail media:

```text
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R '^portability.state_material$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_state_material_probe -j2
ctest --preset linux-clang-debug -R '^portability.state_material$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
cmake --build --preset linux-gcc-debug -j2
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
ctest --preset linux-clang-debug --output-on-failure
```

The focused GCC and Clang CTests passed, including the new production mesh,
legacy material, texture-frame, missing-name cleanup and reorder assertions.
The focused Clang
ASan/UBSan executable printed
`production state material draw passed` without a reported sanitizer finding;
LeakSanitizer was disabled for this host's ptrace environment. Both full
presets and CTest suites passed, with the sandbox GPU CTest skipped; the
central count is in [BUILD_GRAPH.md](BUILD_GRAPH.md). No Windows Debug/Final
rebuild, ABI/layout check, retail
runtime, native game scene or same-input Windows/Linux image comparison was
performed for this integration. Those gates remain open.

## Production mesh-owner compile, 2026-09-26

The initial GCC syntax attempt on `PolyMesh/Mesh.cpp` stopped at
`<windows.h>`. The owner now selects existing native legacy file
declarations on Linux while retaining its Windows include path. It uses
exact filename case through its `Mesh`, `XMesh`, material, facegroup,
renderer and material-manager headers. Unused transitive includes were
removed from `Mesh.cpp`; `renderer.h` remains for the production
`SA_SPLIT_EXACT` flag. Native legacy declarations now supply HRESULT
success/failure helpers and the fixed Windows path bound used by this owner.
`Mesh.h` uses standard `std::max` for its existing LOD clamping expression.
The method declarations and sort callback keep their Windows calling
convention and use the ordinary C++ convention on Linux. No mesh storage
field, public vtable, method order, serialized width or Windows project
source list changed.

The `frontierx_polymesh_mesh_owner` CMake object target compiles the exact
production `Mesh.cpp`, without a replacement implementation. Executed on
Linux x86_64 with GCC 16.2.1 and Clang 22.1.8:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_polymesh_mesh_owner -j2
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_polymesh_mesh_owner -j2
cmake --build --preset linux-gcc-debug -j2
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
ctest --preset linux-clang-debug --output-on-failure
```

Both focused object builds and full preset builds succeeded. Both preset
CTest runs executed 37 passing non-GPU checks; the GPU check skipped at SDL
video initialization in the command sandbox. Neither CTest run executes the
new mesh object. GCC reported legacy `STDDAT.H` template and mutable-string
warnings. Clang also reported nontrivial `memset`/`memcpy` warnings and an
unhandled `PS_SPAN` switch value. These warnings were not suppressed in the
configured target. `nm -C -u` on the GCC object identifies
`pm_simple_map_gen`, `sortQ` and `FDUMP` as direct nonstandard unresolved
symbols; linking the active component still needs their owners and its
material/texture services. Inputs were repository source only, with no
synthetic archive or retail media for this compile check. Native Windows
Debug/Final rebuilds, cross-module ABI/layout checks, game runtime and
Windows/Linux image comparison were not performed for this change.

## Audit evidence and limits

Source review on Linux x86_64, 2026-09-26, used `rg` and `sed` on
`Conquest.ini`, `Conquest.cpp`, `Conquest.vcxproj`, `MeshRender.cpp`,
`IMaterialManager.h`, `PolyMesh/Mesh.cpp`, `PolyMesh/polymesh.cpp`,
`TriMeshInstance.cpp` and the material-library sources. GCC 16.2.1
`-fsyntax-only` attempts on the active `PolyMesh/Mesh.cpp` and inactive
`TriMeshInstance.cpp` initially stopped at `<windows.h>` and `DACOM.h`
respectively. The active owner's later successful GCC/Clang compilation is
recorded above. No generated or retail media was used. No new
runtime, sanitizer, Windows, ABI, layout or image-comparison check ran; those
remain required for the later integration gates. The earlier Linux synthetic
material and Windows fixture results are documented in
[TEXTURE_LIBRARY_PORTABILITY.md](TEXTURE_LIBRARY_PORTABILITY.md) and
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

## App mesh composition and borrowed-instance lifecycle (2026-09-27)

The next link blocker was the active App mesh caller's process globals,
diagnostics and `SetupDiffuseBlend`, after the production `MeshRender.cpp`,
`MeshInfo.cpp` and `SuperTrans.cpp` objects compiled. The Windows
composition provides these through `Globals.h`, `System.cpp` and the App
project. Native-only `NativeAppGlobals.cpp` now owns one set of service
pointers and runtime flags shared by the shell and game-material fixture;
`NativeAppDiagnostics.cpp` owns the fatal `ICQImage` diagnostics previously
local to the shell. `TextureManager.cpp` refers to that shared native
`DACOM` pointer, while retaining its Windows definition. `Planetoid.cpp`
retains the Windows `camMoved` definition; the native global is owned once
by the new App composition file. This changes no serialized structure or
public interface layout.

The native `SetupDiffuseBlend` applies the active `System.cpp` render,
texture-stage and sampler sequence through the existing `IRenderPrimitive`
vtable. It stops with a fatal diagnostic on any unsupported state. The
native-only D3D9 declarations include `D3DRS_COLORVERTEX = 141`,
`D3DRS_DIFFUSEMATERIALSOURCE = 145` and `D3DMCS_COLOR1 = 1`, matching the
[Microsoft render-state enumeration](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3drenderstatetype)
and [material-color-source enumeration](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dmaterialcolorsource).
The current primitive still rejects the first of these states, and the
sampler path is unimplemented. This helper is link composition, not a claim
that the App face draw works. Implementing those exact states and the
material/effect path is the next executable App render gate.

The existing game-material probe could load and render the generated
PolyMesh instance but could not exercise App-side instance ownership while
the App objects were unlinked. It now links those exact objects, assigns the
native `ENGINE` and `REND` services, calls production `CreateMeshInfoTree`
and `DestroyMeshInfoTree`, and verifies the root has the generated instance,
mesh, whole-object flag and positive radius. It then queries the Engine
instance again to establish that teardown did not destroy an instance it
only borrowed. Source review found an uninitialized `bOwnsInstance` bit in the
production `MeshInfo` constructor. The constructor now initializes that bit,
other lifecycle flags, the instance sentinel, face count and radius before
tree construction. The existing synthetic fixture was extended because it
already owns the Engine/RenderMgr/PolyMesh instance and the later render
call; a separate probe would duplicate that composition. Pass or failure
decides whether App mesh lifecycle can safely wrap the active engine owner.
It does not prove App rendering, effect processing, native scene startup or
retail behavior.

The next real caller, `MeshRender::Init`, is also exercised on the same
generated legacy mesh after the independent PolyMesh render check. A narrow
test `ICQ_VB_Manager` registers the production `MeshRender` object; the
existing native pipeline, production texture library and game material
manager supply its other services. `Init` builds nonempty App vertex and
face arrays and calls its production `RestoreVertexBuffers` path. The test
then calls `SetupMeshInfo` on a newly created App tree and verifies a face
group and face-render storage. Tree destruction releases its `IMeshRender`
reference; a final release unregisters the renderer exactly once, with the
Engine instance still valid. This proves synthetic App mesh setup and
buffer-lifecycle execution, not `MeshRender::Render` or a visible draw.
The next call requires the App camera/light and batching state composition;
the diffuse helper's required state sequence is covered separately below.

Executed on Linux x86_64 with repository-generated UTF profiles, mesh,
material and texture data, no retail media:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe
ctest --preset linux-gcc-debug -R '^portability\.game_material_manager' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe
ctest --preset linux-clang-debug -R '^portability\.game_material_manager' --output-on-failure
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
```

All six focused CTests passed, three per compiler. Both complete builds
passed, with the central CTest totals in [BUILD_GRAPH.md](BUILD_GRAPH.md).
The three Clang ASan/UBSan runs each printed `production game material load
passed` without a reported finding; LeakSanitizer was disabled for this
host. Windows Debug/Final rebuilds and module ABI/layout checks, native
GPU/App draw, retail runtime and a same-input Windows/Linux game-image
comparison were not performed.

## App diffuse-state sequence and SDL sampler (2026-09-27)

The active `System.cpp::SetupDiffuseBlend` call previously stopped at
`D3DRS_COLORVERTEX` because the native primitive did not represent the
shader's fixed vertex-color/unlit mode. Source review of the SDL fragment
shader showed that it already reads the vertex color and has no fixed
lighting. The primitive now accepts only `COLORVERTEX=1`,
`DIFFUSEMATERIALSOURCE=COLOR1` and `LIGHTING=0` for that shader; opposing
values remain unsupported. It also accepts disabling color/alpha operations
and clearing texture bindings in unused stages 1 through 3. This does not
implement active additional texture stages.

The helper's remaining sampler calls now reach a stage-zero native sampler
snapshot. It accepts only point/linear minification, magnification and mip
selection and wrap/clamp U/V addressing. The SDL owner creates a candidate
GPU sampler, waits for idle, then replaces the old sampler and cached state.
Failure retains the previous sampler. The existing game-material executable
calls the actual `SetupDiffuseBlend` with generated test services and checks
all six stage-zero combine fields, linear filtering, point mip selection,
clamp/wrap addressing and texture binding. The native submission check
verifies unsupported values and callback-error rollback. No App
`MeshRender::Render` call or game frame is established by these checks.

GCC 16.2.1 and Clang 22.1.8 Linux x86_64 full debug builds and CTest presets
each passed 40 executing checks with the SDL GPU check skipped in the
command sandbox. Focused Clang ASan/UBSan runs of the render-submission check
and all three generated game-material UTF variants passed with
`ASAN_OPTIONS=detect_leaks=0` and `UBSAN_OPTIONS=halt_on_error=1`. The same
SDL GPU CTest was run against the desktop Wayland/Vulkan session outside the
command sandbox under GCC, Clang and focused Clang ASan/UBSan. All three
passed with 2,496 changed pixels and final checksum
`7951318278861886233`; the test additionally observed a different linear
sampler checksum and exact restoration with point filtering. A formerly
unexecuted transform assertion initially rejected a moved image because it
applied the unshifted fixture's center/background structure rule. The check
now requires valid RGB readback with changed pixels, a measured horizontal
moment shift, a distinct projection readback and exact restored checksum;
all three visible runs passed. This is synthetic SDL/Vulkan hardware evidence,
not an App game scene, retail runtime or a same-input game-image comparison.
Native Windows Debug/Final and ABI checks remain unperformed for this change.

Commands executed for this gate (the final GPU CTests had desktop-session
access; the same commands skip before device creation in the command sandbox):

```text
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --preset linux-gcc-debug -R '^portability\.sdl3_gpu$' --output-on-failure -V
ctest --preset linux-clang-debug -R '^portability\.sdl3_gpu$' --output-on-failure -V
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_render_submission_probe frontierx_game_material_manager_probe frontierx_sdl3_gpu_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_render_submission_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_fixture.utf
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_cr_fixture.utf cr
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-texture-handle-asan/frontierx_game_material_manager_probe /tmp/frontierx-texture-handle-asan/game_material_manager_missing_end_fixture.utf missing-end
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability\.sdl3_gpu$' --output-on-failure -V
```

## Production PolyMesh reaches SDL/Vulkan (2026-09-27)

The next blocker was the gap between the existing CPU-only
`Engine::render_instance -> RenderMgr -> POLYMESH` receiver and the visible
SDL backend. The same generated legacy mesh and production component closure
now run in an optional visible GPU mode of `frontierx_game_material_manager_probe`.
Its existing six-index `MTVERTEX` callback forwards the typed submission to
`SdlGpuRenderer`; production model/view and the probe's bounded projection
flow through the primitive callbacks. Stage-zero render, combine, sampler
and texture state also use the SDL owner. The GPU mode requires a valid RGB
64-by-64 readback with nonzero changed pixels and a presented window frame.
The CPU modes retain their rejecting draw receiver and still test error
propagation. This extends an existing production-owner check; its previous
CPU assertion could establish submission but could not establish visible
game-mesh output.

The legacy fixture has no diffuse texture. Its production draw binds texture
handle zero. SDL now maps that value to an owned opaque white 1-by-1 GPU
texture, without exposing the internal allocation as a caller texture handle;
the stage-zero diffuse selection is therefore defined and visible. The
native primitive accepts the exact fixed `D3DCULL_NONE` setting already used
by its GPU pipeline and retains the `RPR_STATE_ID` batching key for queries
while submitting immediately. Other culling and batching controls remain
unsupported. The generated check verifies these exact values.

Linux x86_64 GCC 16.2.1 and Clang 22.1.8 full debug builds passed. Three
generated material CPU variants passed under each compiler. Both full
command-sandbox CTest presets passed their executing checks and skipped the
two GPU checks before video initialization. Against the desktop
Wayland/Vulkan session, `portability.game_material_gpu` executed under both
compilers with 496 changed pixels, checksum `10105697032999492819`, and a
presented frame. The existing `portability.sdl3_gpu` also passed under both
compilers after the null-texture change. The three CPU fixture variants and
visible game GPU mode passed in a focused Clang ASan/UBSan build with
LeakSanitizer disabled for this host. Inputs are repository-generated
UTF/material/mesh fixtures; this is synthetic Linux GPU runtime of a
production game mesh caller, not a loaded retail scene.

Commands executed for this change:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --preset linux-gcc-debug -R '^portability.game_material_gpu$' -V
ctest --preset linux-clang-debug -R '^portability.game_material_gpu$' -V
ctest --preset linux-gcc-debug -R '^portability.sdl3_gpu$' --output-on-failure
ctest --preset linux-clang-debug -R '^portability.sdl3_gpu$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager(_cr|_missing_end)?$' --output-on-failure
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' -V
```

## Embedded game texture in the visible PolyMesh draw (2026-09-27)

The preceding visible draw used handle zero and proved game geometry and
fixed material state, but it could not prove the active game texture loader.
The generated older-format mesh now optionally embeds a 2-by-2 RGB565
`Tile` under its actual `Texture library` directory and names it from the
production `Material` entry. The existing visible game-material mode uses
that fixture. `XMesh::read` invokes production `TextureLibrary::load_library`,
which creates and uploads the RGBA8 conversion through the same
`NativeRenderPipeline` texture callbacks used by the game manager. The
callbacks forward GPU resource operations to `SdlGpuRenderer` while retaining
the existing 4-by-4 TGA capture checks. `Engine::update` invokes the
production `PolyMesh` material update that resolves the frame into a runtime
texture handle before the real indexed draw.

The visible check now requires at least three texture creations and uploads
across the earlier TGA exercises and the embedded mesh, a live nonzero
texture handle at the game draw, and a changed 64-by-64 RGB image. It then
binds handle zero against the same submitted geometry and render state: the
white-fallback checksum must differ from the textured checksum before the
original handle is restored for presentation. GCC 16.2.1, Clang 22.1.8 and
focused Clang ASan/UBSan visible Wayland/Vulkan runs each reported texture
handle `9`, 496 changed pixels, textured checksum
`11826028615312451232` and white-fallback checksum
`10105697032999492819`. The three generated CPU material variants passed
under both regular compilers and focused Clang ASan/UBSan. Both complete
Linux debug builds passed; each command-sandbox CTest preset passed its
executing checks and skipped the two GPU checks before video initialization.

The source and fixture inputs are repository-generated; no retail media
entered the build. This is synthetic runtime evidence for the active
`PolyMesh` texture/material draw chain. It does not execute the App
`MeshRender::Render`/`IMaterial::DrawVB` GPU path, a complete game scene,
Windows D3D9 for this same mesh, retail gameplay, or native Windows
Debug/Final and ABI checks. Those remain the next Milestone 5 gates.

Commands executed for this change:

```text
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --preset linux-gcc-debug -R '^portability.game_material_gpu$' -V
ctest --preset linux-clang-debug -R '^portability.game_material_gpu$' -V
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager(_cr|_missing_end)?$' --output-on-failure
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' -V
```

## Production App MeshRender and IMaterial CPU draw (2026-09-27)

The linked App test previously stopped at `MeshRender::Init`,
`RestoreVertexBuffers` and `SetupMeshInfo`; the direct synthetic
`IMaterial::DrawVB` invocation did not prove that the App face-group caller
selected the material or issued its draw. The same executable now invokes
`MeshRender::Render` through both a generated older-format Engine instance
and a second generated CQ2 material instance. The latter's face group must
point to the exact `IMaterial` already loaded through production
`MaterialManager`. Its `FaceGroupRender::Render -> IMaterial::DrawVB` call
reaches the existing native 68-byte `StandardVertex` callback with six U16
indices. The older-format App branch independently reaches the same typed
CPU receiver. Each branch must add exactly one draw and provide a vertex
slice whose count is one more than its largest index; both App renderers
release their buffer-owner registrations, and both Engine instances remain
under their original ownership until explicit teardown.

The first execution crashed at a null `MAINCAM` in the test composition,
identified by UBSan at `MeshRender.cpp:946`. Supplying the already owned
synthetic camera then exposed a real caller defect: `FaceGroupRender` passed
`maxVert[0]` as `num_verts` to the pipeline, although it stores the largest
index. The native adapter rejected an index equal to that count, and the
production `CQASSERT` stopped the run. The App caller now passes
`maxVert[0] + 1` to both `IMaterial::DrawVB` variants and the older-format
indexed draw. This also matches the supported D3D9
`DrawIndexedPrimitive` parameter contract; the Windows runtime effect of
the correction has not been measured. The App effect-loop time read is now
performed only when there is an effect channel. Effects retain a single
time sample for their loop; no-effect mesh draws no longer require an
unrelated `IObjectList` service.

GCC 16.2.1 and Clang 22.1.8 full Linux x86_64 debug builds and CTest
presets passed with repository-generated UTF material/mesh inputs. All
three focused CPU material variants passed under Clang ASan/UBSan with
`ASAN_OPTIONS=detect_leaks=0` and `UBSAN_OPTIONS=halt_on_error=1`.
The visible `portability.game_material_gpu` also passed under GCC, Clang
and focused Clang ASan/UBSan after the App CPU caller was added; its
production `PolyMesh` textured readback retained checksum
`11826028615312451232`, white-fallback checksum
`10105697032999492819`, and 496 changed pixels. Those images come from
`PolyMesh`: the App's 68-byte callback is CPU-only. Its material shader,
fixed-function fallback states and full game composition are not yet
implemented in SDL. Native Windows Debug/Final rebuilds, affected module
ABI checks, retail runtime and same-input App image comparison remain open.

Commands executed for this gate:

```text
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --preset linux-gcc-debug -R '^portability.game_material_gpu$' -V
ctest --preset linux-clang-debug -R '^portability.game_material_gpu$' -V
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager(_cr|_missing_end)?$' --output-on-failure
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' -V
```

## Visible App StandardVertex geometry (2026-09-27)

The preceding App check ended at the typed CPU receiver. The existing
`RenderSubmissionView4` already validates the 68-byte `StandardVertex`
transfer layout, but `SdlGpuRenderer` selected pipelines only for the
24-, 32- and 36-byte vertex formats. The renderer now creates a fourth,
transactionally rebuilt pipeline with the 68-byte stride and explicit
position, BGRA diffuse, first UV and second UV offsets. It reuses the
existing two-UV vertex shader; normal and tangent fields are transferred
but are not consumed by that shader. Submission, presentation, bounded
readback and teardown select or release the new pipeline by format.

The first version of the App GPU check was **not valid App draw evidence**:
`BATCH` addressed the GPU primitive, but App `PIPE` still addressed the
separate CPU material primitive. Its image changed because App stage
state affected the previously submitted `PolyMesh` geometry. A state
query inside the App callback exposed this split. The older-format
App renderer now owns a native pipeline directed at the same primitive
as `BATCH`; it creates and releases its vertex buffers through that
pipeline. The CQ2 branch retains the production material's original
CPU pipeline because its shader effect is not realized on Linux.

The native primitive and SDL shader now carry the D3D9 texture-factor
color and accept `D3DTA_TFACTOR` at stage zero. The internal render
snapshot starts with opaque white, matching the [documented D3D9
default](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3drenderstatetype).
`D3DCULL_CW` and `D3DCULL_CCW` select SDL back- and front-face culling
respectively, matching the [D3D9 winding definitions](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dcull).
These snapshots are native runtime state, not persistent or shared
Windows module layouts. No game simulation order or serialized data
changed.

The generated check sets factor `0xFF40A0E0` and observes, inside the
production App draw callback before its cleanup changes the states,
that the same primitive holds clockwise culling and factor-based color
and alpha arguments. It reads back and presents at that point. The
64-by-64 image has 496 changed pixels and checksum
`2133805298751023491`, distinct from the prior textured `PolyMesh`
checksum `11826028615312451232`. On the same submitted App geometry,
switching to counterclockwise culling yields checksum
`7642797035764753283`; restoring clockwise culling and replacing the
factor with white yields `12405162864621168803`. GCC 16.2.1,
Clang 22.1.8 and focused Clang ASan/UBSan visible Wayland/Vulkan runs
passed those bounded comparisons. Both complete debug builds and 40
executing CTests per preset passed; the two command-sandbox GPU tests
skipped before video initialization and were run separately on the
desktop. The existing visible SDL GPU regression passed under both
compilers. Three generated CPU material input variants also passed
under both compilers and focused Clang ASan/UBSan; leak detection was
disabled for the sanitizer host.

The generated UTF and geometry are repository-owned synthetic inputs;
no retail data was used. This is a real App legacy geometry and
texture-factor/culling synthetic runtime result. The backend still
holds one latest draw rather than a composed game frame; other legacy
states, normal/tangent lighting, CQ2 shader effects, native game-scene
composition, same-input Windows D3D9 comparison, native Windows
Debug/Final and affected ABI checks, and retail runtime remain open.

Commands executed for this change:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe
ctest --preset linux-gcc-debug -R 'portability.game_material_manager' --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' -V
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe
ctest --preset linux-clang-debug -R 'portability.game_material_manager' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_gpu$' -V
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.sdl3_gpu$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.sdl3_gpu$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager(_cr|_missing_end)?$' --output-on-failure
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' --output-on-failure
git diff --check
```

After the focused runs, both complete Linux debug builds succeeded. Each
full CTest preset executed 40 checks successfully and skipped its two GPU
checks before video initialization in the command sandbox; those skips
were not counted as runtime evidence. The separate visible
`portability.sdl3_gpu` regression passed under GCC and Clang after the
fourth pipeline was added.

## Bounded game and App image artifacts (2026-09-27)

The existing visible game-material executable can now write its actual
production `Engine/RenderMgr/PolyMesh` image and the App image captured
inside the `MeshRender::Render` draw callback as 64-by-64 P6 files with
`--capture-prefix PREFIX`. It requires `--gpu` and appends `-game.ppm`
and `-app.ppm`; ordinary CTest execution writes no capture. The command
fails if either image is invalid or cannot be written. This extends the
existing integration check to retain the exact pixels needed for a
later same-input Windows comparison; it is not a new rendering path.

GCC 16.2.1 and Clang 22.1.8 visible Wayland/Vulkan runs each produced
12,301-byte P6 files with header `P6\n64 64\n255\n` and 12,288 RGB
bytes. The GCC and Clang files were byte-identical for each draw:
`bed238d526e7f3ae36cd5c55a1a6fdea20bcf1a955553e7e4925be00acba1bfc`
is the PolyMesh P6 SHA-256, and
`8ece1c8a6246f36a2c1b7584d5aaacda7ac4b82cd4023d247aa094e50640d04d`
is the App P6 SHA-256. These are repository-generated synthetic inputs;
the P6 outputs stayed in `/tmp` and were not added to source control.

Source review explains why the CQ2 material draw cannot yet use this
image comparison. Native `RenderPipelineAdapter::load_effect` returns
null. `Material::Realize` still loads its TGA through production
`ITManager`, but `Material::DrawVB` calls `SetShaderConstants` only when
`shaderEffect` exists; with a null effect it skips the pass's texture
binding and shader constants. Routing that draw to a GPU callback would
therefore show geometry with inherited states, not the material.
The supported D3D9 implementation loads shader source through
`IFileSystem`, calls D3DX effect compilation, selects a pixel-shader
technique and returns a restoration slot. The next material gate is a
native effect replacement at this existing pipeline boundary with a
bounded supported script/technique contract, followed by the production
App `IMaterial::DrawVB` GPU path. The existing repository
`Data/mat/shaders/VertexColor.fx` documents one fixed technique with
stage-zero texture/diffuse modulation, but that source review is not an
implemented replacement. Native Windows Debug/Final, ABI checks,
same-input D3D9 captures, retail runtime and a composed native game
scene were not performed.

A source inventory of the eight repository `Data/mat/shaders/*.fx`
files found seven `PS11`/`PS20` techniques and one `fixed` technique
(`VertexColor.fx`). All eight contain an assembly vertex-shader section.
The supported D3D9 loader chooses `PS00`, `PS11` or `PS20` by device
pixel-shader version, so the inventory does not establish that its
`fixed` technique is selected at Windows runtime. This narrows the
native effect work to an explicit supported-technique contract and a
same-input Windows observation before claiming parity. The inventory
is source review, not compilation or shader execution.

The native SDL renderer now avoids rebuilding graphics pipelines for
texture-factor or specular-enable changes alone. The linked App GPU
check asserts that its production texture-factor call leaves the
pipeline generation unchanged; its bounded factor and culling images
remain distinct. See
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md) for the
focused implementation and validation record.

Commands executed:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe
out/build/linux-gcc-debug/frontierx_game_material_manager_probe /tmp/frontierx-gcc-game-capture-fixture.utf --gpu out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-prefix /tmp/frontierx-gcc-game-capture
out/build/linux-clang-debug/frontierx_game_material_manager_probe /tmp/frontierx-clang-game-capture-fixture.utf --gpu out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-prefix /tmp/frontierx-clang-game-capture
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
git diff --check
```

The P6 equality check used Python to read both compiler outputs, require
the exact header and `64*64*3` RGB bytes, and compare each complete file
byte for byte. Both full CTest presets executed 40 checks and skipped
their two GPU checks in the command sandbox; the capture commands above
are the actual desktop GPU executions, not those skips.

## Reject CQ2 draws with an unavailable effect (2026-09-27)

The preceding source review exposed a false integration result: the
synthetic `Synthetic.fx` is parsed and its texture is uploaded, but the
native `load_effect` slot returns null. `Material::DrawVB` previously
submitted the indexed geometry anyway, without `SetShaderConstants`,
pass setup, texture binding or the effect. The App CQ2 caller therefore
reached a CPU callback while rendering no material semantics. A game
frame could have appeared to work using state inherited from an earlier
draw.

The production `Material::DrawVB` now rejects a native draw when its
material has a configured shader but no effect. This is limited to the
non-Windows branch; the supported Windows effect path is unchanged.
The existing generated UTF check verifies the direct material call and
the App CQ2 caller produce no CPU draw. It then submits the same indexed
`StandardVertex` buffer directly through `IRenderPipeline` and compares
the exact callback bytes and indices, retaining separate evidence that
the geometry adapter works. The rejection is synthetic automated
execution, not shader or game-scene execution. The next gate remains a
native effect implementation at `IRenderPipeline::load_effect`, including
the necessary constant, pass and texture-state behavior, followed by a
visible production CQ2 material draw. A direct pipeline draw is not a
substitute for that gate.

GCC 16.2.1 and Clang 22.1.8 Linux x86_64 Debug built the focused
`frontierx_game_material_manager_probe` target and passed its
`portability.game_material_manager` CTest with the repository-generated
UTF shader, TGA and mesh inputs:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_manager$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_manager$' --output-on-failure
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager(_cr|_missing_end)?$' --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' -V
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_gpu$' -V
```

Both full debug presets built and ran 40 checks; their two GPU checks
skipped before video initialization in the command sandbox. The three
generated material variants passed under focused Clang ASan/UBSan with
LeakSanitizer disabled on this host. Separate visible Wayland/Vulkan
game-material runs passed under GCC and Clang, preserving the older-format
Engine PolyMesh checksum `11826028615312451232` and App checksum
`2133805298751023491`, each with 496 changed pixels. Neither image is a
CQ2 material-effect result.

Native Windows Debug/Final rebuilding, affected ABI checks, retail
runtime, a CQ2 effect GPU run, a composed native game scene and same-input
Windows D3D9 comparison were not performed for this change.

## Native c30/c31 material-vertex constants (2026-09-27)

The next effect blocker was concrete: `Material::SetShaderConstants` sends
its compiled vertex-register span to `IRenderPipeline::set_vs_constants`,
but `RenderPipelineAdapter` returned `GR_NOT_IMPLEMENTED`. The repository's
`VertexColor.fx` source uses `c30.xy` as UV offset, `c30.zw` as UV scale,
and `c31` as vertex-color modulation. The native pipeline now accepts only
register `c30`, `c31`, or their combined contiguous span through an optional
backend callback. A missing callback, other registers, null input and
out-of-range spans fail explicitly. The SDL renderer validates finite values
before replacing either register and uploads a vertex uniform with identity
UV and white modulation defaults. The native App `StandardVertex` shader
consumes those fields; the other vertex shaders carry the same uniform
layout but retain their previous output. No serialized type or supported
Windows rendering path changed.

The existing linked game-material GPU check configures its App pipeline
with that callback. It uses a vertex-color stage to show that `c31` changes
the 64-by-64 App readback, then restores the exact baseline checksum. It
separately selects a patterned repository texture to show that shifting
`c30.x` changes the readback and that resetting `c30` restores the exact
baseline. An unsupported register is rejected. These are synthetic native
pipeline and SDL/Vulkan results on older-format App geometry. The
production CQ2 `Material::SetShaderConstants` call is still gated by the
missing native effect and was not executed. A passing constant callback
does not establish shader-technique, pass, texture-state or CQ2 material
behavior.

Commands executed on Linux x86_64, GCC 16.2.1 and Clang 22.1.8 Debug:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' -V
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_gpu$' -V
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.sdl3_gpu$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.sdl3_gpu$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager(_cr|_missing_end)?$' --output-on-failure
```

The focused game-material GPU runs executed on a visible Wayland/Vulkan
desktop under both compilers. Both retained the earlier production
PolyMesh checksum `11826028615312451232` and App checksum
`2133805298751023491`, with 496 changed pixels each, before the
constant-specific image comparisons. Both separate SDL GPU regressions
passed. Each full preset executed 40 checks and skipped two GPU checks
before video initialization in the command sandbox. The three generated
material variants passed the focused Clang ASan/UBSan CPU run with
LeakSanitizer disabled on this host; the new GPU constant path did not run
under sanitizers. The next gate is an effect object/technique at the
existing pipeline boundary that allows the production CQ2 material to
apply these constants and its pass state to a visible draw. Native
Windows Debug/Final, affected ABI checks, retail runtime, a native game
scene and same-input Windows D3D9 comparison remain unperformed.

## First production CQ2 effect draw through the App caller (2026-09-27)

The remaining integration blocker was `Material::Realize` receiving a null
native effect and `Material::DrawVB` consequently rejecting its shader.
The real caller is `MeshRender::Render` over a CQ2 `XMesh` face group.
The existing test could prove indexed geometry, TGA upload and isolated
`c30`/`c31` uniforms, but none of those demonstrated the production
material applying its pass. The falsifiable check was a generated CQ2
mesh naming a saved material that uses the repository's actual
`Data/mat/shaders/VertexColor.fx` source: `Realize` must load an effect,
`DrawVB` must apply the material constants and pass state, bind its own
TGA handle, and submit through both the direct production material caller
and the App caller to different visible images. Failure would keep the
native effect boundary and game-scene composition open.

The Linux-only effect slot now has a pipeline-owned one-pass implementation
for exactly this source revision. `load_effect` reads the shader through
the supplied `IFileSystem`, bounds it at 64 KiB, normalizes CR/CRLF/LF,
and requires FNV-1a hash `eb95bc07e124d705` of the 2,422-byte normalized
repository source. It caches a stable restoration-slot pointer by filename.
Any other filename or changed source returns null; the earlier
`Synthetic.fx` fixture still rejects its draw. The supported pass sets
stage-zero texture/diffuse modulation, disables stage one and applies
wrap addressing, as specified by that source. `Material::DrawVB` now
requires native constant and pass success before submission. The fixed
disabled alpha-test state and its reference value are accepted by the
native primitive; enabled alpha testing is still unsupported. Windows
continues using its original D3DX effect methods. This is a bounded
translation of one known shader, not a D3DX compiler or backend decision.

The generated UTF archive now contains the original synthetic scripts,
the repository `VertexColor.fx` source with the line endings needed by
the production parser, `VertexColor.txt`, and `Tile.tga`. A second
production `MaterialManager` uses the visible App pipeline; a second
production Engine profile loads `RenderMgr` and `PolyMesh` with that
manager. The direct `Material::DrawVB` check submits a three-vertex
triangle. The separate CQ2 mesh archetype resolves the *same* material
pointer and executes `MeshRender::Render` through its six-index branch.
This avoids treating a preexisting GPU draw or an unrelated manager as
material evidence. The generated input and all captures stayed outside
the repository.

Visible Wayland/Vulkan synthetic runtime under GCC 16.2.1 and Clang
22.1.8 produced matching 64-by-64 bounded readbacks:

| Production path | Changed pixels | RGB checksum | P6 SHA-256 |
|---|---:|---:|---|
| Direct `Material::DrawVB` with TGA | 512 | `1566906783361949827` | `42e35143761829c2fc0d9eb6201deb83801123133201b01ae6b79c6dace8cda1` |
| Direct draw with texture unbound | 512 | `630059529881354115` | not captured |
| CQ2 App `MeshRender::Render` | 496 | `17301943790166719418` | `98df7af052c62e5bc04be94657083debfe8f9a2391a2c7b452634b2ef90a45df` |

The direct draw's texture handle was the production `ITManager` handle;
unbinding it changed the image. Both compiler captures were byte-identical
for the direct and App CQ2 paths. Each P6 has the exact
`P6\n64 64\n255\n` header and 12,288 RGB bytes, 12,301 bytes total.
Visual inspection of a point-scaled diagnostic copy showed a clear
triangle for the direct material. The App mesh appeared very dark with
the generated lighting/color inputs, so this result is not yet an
inspectable native game scene. The former older-format Engine/App captures
remained byte-identical with their previous hashes.

Commands executed for this change:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_manager$' --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' -V
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_gpu$' -V
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_manager(_cr|_missing_end)?$' --output-on-failure
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' --output-on-failure
out/build/linux-gcc-debug/frontierx_game_material_manager_probe /tmp/frontierx-gcc-vertexcolor-fixture.utf --gpu out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-prefix /tmp/frontierx-gcc-vertexcolor
out/build/linux-clang-debug/frontierx_game_material_manager_probe /tmp/frontierx-clang-vertexcolor-fixture.utf --gpu out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_mt.vert.spv --capture-prefix /tmp/frontierx-clang-vertexcolor
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.sdl3_gpu$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.sdl3_gpu$' --output-on-failure
```

Both complete Linux debug builds passed. Each full preset executed 40
checks and skipped two GPU checks before video initialization in the
command sandbox; the separate visible GPU commands above are runtime
evidence. Three generated CPU material variants and the visible GPU
material path passed focused Clang ASan/UBSan with LeakSanitizer disabled
on this host. The separate SDL GPU regression passed visibly under both
compilers. At this gate, native Windows Debug/Final rebuilding and module ABI
checks, a same-input Windows D3D9 effect capture, retail runtime, other shader
techniques and texture formats, full `set_default_constants`/c66 behavior,
error rollback across partially applied effect states, a composed native
game scene and production backend selection remained open. Later sections
record the shell composition and `VertexColor.fx` rollback. The Windows
loader's PS00/PS11/PS20 technique selection for this file has not been
observed at runtime; no parity claim follows from this Linux result.

## Production game mesh in the native shell (2026-09-27)

The shell now has an explicit single-frame `--game-scene` path that reads
supplied material and legacy mesh UTF files, loads the four SDL shader
variants, constructs production `IMaterialManager`, `ITManager`,
`TextureLibrary`, `IEngine`, `RenderMgr` and `PolyMesh` through DACOM, and
calls `IEngine::render_instance`. This is the same owning call chain as the
earlier linked PolyMesh GPU check, now under the shell's normal startup,
window and shared component manager. Its indexed 36-byte `MTVERTEX`
submission binds the embedded RGB565 frame and presents before the Engine
instance, archetype and renderer are released. GCC and Clang visible shell
captures matched the linked PolyMesh P6 byte for byte: 496 changed pixels,
RGB checksum `11826028615312451232`, SHA-256
`bed238d526e7f3ae36cd5c55a1a6fdea20bcf1a955553e7e4925be00acba1bfc`.
A focused Clang ASan/UBSan visible shell run passed with LeakSanitizer
disabled. Inputs and captures remained outside the repository. The initial
combined run exposed a destructor-symbol collision between the unrelated
global legacy `Material` and CQ2 `Material` classes; the Linux CMake CQ2
owner now uses `NativeGameMaterial` as its implementation symbol. The
Windows type and public `IMaterial` interface are unchanged. Commands,
ownership details, GDB evidence and remaining gates are in
[NATIVE_SHELL.md](NATIVE_SHELL.md).

This one-frame legacy mesh is a native game-owner rendering composition,
but it is not gameplay or retail-runtime evidence. The production CQ2
At this earlier gate, the `VertexColor.fx` App draw executed only in the
linked integration check. The following section records the shell
composition; same-input Windows D3D9 comparison remains open.

## CQ2 App material in the native shell (2026-09-27)

The shell's existing `--game-scene` option now branches on the production
mesh's CQ2 `imaterial_list`. That branch uses the same Engine, RenderMgr,
PolyMesh, `IMaterialManager`, `ITManager`, native pipeline and registered
primitive as the legacy branch, then creates the actual App `MeshRender` and
borrowed `MeshInfoTree`. It calls `MeshRender::Render` through the resolved
production `IMaterial`, with `VertexColor.fx` source pinning, `c30`/`c31`
constants and the production TGA. The App callback captures/presents before
its state cleanup; it also reads an unbound-texture image and requires a
different checksum before restoring the handle. The shell checks live
texture ownership and balanced App buffer registration. This checks the
previously unproven App ownership and effect behavior in the real shell,
using the already integrated production caller.

Visible GCC 16.2.1, Clang 22.1.8 and focused Clang ASan/UBSan Wayland/Vulkan
runs each reported 496 changed pixels and RGB checksum
`1466864583390317354` for the generated CQ2 UTF mesh. GCC and Clang P6
captures were byte-identical, SHA-256
`e54ef6c12ff5ca0fd5da724725aae5da4e79f87074f75b6dd9ce26142e3b90d1`.
The legacy shell branch retained checksum `11826028615312451232` after the
callback changes. A CQ2 mesh with the unsupported `Synthetic.fx` effect
returned exit code 7 with no success report. The generated input and
captures stayed outside the repository. Exact commands, ownership and
unperformed gates are in [NATIVE_SHELL.md](NATIVE_SHELL.md).

The later shell loop now repeats this synthetic game-owner composition while
pumping window and EventSystem events, with a stable bounded image required
for every frame. It does not establish simulation updates, campaign behavior, retail compatibility or
cross-backend parity. Other D3DX9 effects and multi-material scenes remain
unsupported; a same-input Windows D3D9 material capture, native Windows
Debug/Final rebuilds and affected module ABI checks have not run.

## Transactional native VertexColor pass setup (2026-09-27)

The first native `VertexColor.fx` pass applied two sampler fields followed by
six stage-zero texture fields in order. If a later backend callback failed,
`BeginPass` rejected the draw, but already accepted fields stayed changed.
The affected production caller is `Material::DrawVB`, reached directly and
through App `MeshRender::Render`; a later mesh draw could inherit the partial
state. The existing `portability.game_material_gpu` target could exercise the
real effect but had no failure between those state writes. Its callback now
rejects the fourth stage update exactly once after three successful updates.
The check starts from clamp sampling and a diffuse-only color stage, observes
the rejection, requires both sampler values and the earlier color-stage
values to match their pre-pass state, and requires the 64-by-64 GPU readback
checksum to be unchanged. The next `BeginPass` succeeds and the production
CQ2 material proceeds to its visible draw. This is a regression check on
the integrated effect owner, not a second effect implementation.

`NativeVertexColorEffect::BeginPass` now reads the affected native primitive
states before changing them and restores successfully applied fields in
reverse order on failure. A restoration failure marks that effect instance
unusable, so later passes cannot claim success with unknown backend state.
The stage-one disable calls in this native primitive are validated no-ops;
stage zero and the two samplers are the mutable fields in this contract.
The implementation does not provide a generic D3DX effect compiler or
atomic recovery from a backend callback that itself changes hardware state
and then reports failure.

Executed on the Linux x86_64 Wayland/Vulkan host:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe -j4
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_manager' --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe -j4
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_manager' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.game_material_gpu$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe -j4
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' --output-on-failure
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
```

GCC 16.2.1 and Clang 22.1.8 compiled the production pipeline and material
probe. The three CPU material variants passed under each compiler. Each
visible GPU check passed the injected failure, unchanged image, restored
state, later successful pass and production CQ2 draw. The focused Clang
ASan/UBSan GPU check passed with LeakSanitizer disabled for this host. Both
full Linux Debug presets passed, with the two GPU checks skipped in the
command sandbox; their visible game-material runs above supplied runtime
evidence. A 1500 ms visible GCC shell run after the full build presented 73
CQ2 frames with the unchanged checksum `1466864583390317354`. Inputs
were repository source and generated synthetic UTF/material/texture fixtures;
no retail data was used. Native Windows Debug/Final rebuilding, affected
module ABI checks, retail runtime, same-input Windows D3D9 effect capture,
multi-pass effects and other shader techniques remain unperformed.
