# First Terran mission: presentation and media

TM3 presentation is accepted by the user on 2026-10-16 with explicit complete
mission confirmation after the final Weaver repair. Earlier pending visual
statements below are retained history; technical execution limits remain.

## TM4 required terrain and next briefing (2026-10-16)

TM4 adds the original BlackHole terrain owner and its authored ring texture.
The required RGB888-plus-Alpha8 path now passes production CPU and visible
GCC/Clang GPU checks, including full-image comparison against independent RGBA8
reference pixels and exact restoration after an opacity change. A genuine TM4
checkpoint loads without the earlier ring/model resource failures. Active warp,
late effects, both maps and portraits across late gameplay remain user checks;
resource admission is not complete visual acceptance. Details and the mission
matrix are in [the P2 ledger](NATIVE_TERRAN_P2.md#tm4-production-integration-2026-10-16).

The original next briefing requires the actual uppercase `SCRIPT05.dll` binding.
A genuine standalone TM5 briefing then exposed a native Teletype boundary error:
its authored objectives end in LF. Native line splitting now preserves empty,
CRLF and trailing rows without discarding following content, while retaining
strict length/progress bounds. Windows retains its original path. Existing
font-enabled owner checks exercise exact content, row positions, final-row
flags and persistent-display cleanup. The real briefing reaches all three
objective lines and closes normally, with speech and portraits enabled. This
is bounded next-briefing evidence, not a TM4 victory/Continue or TM5 gameplay
pass. No authored text or mission predicate is changed.

## Weaver screenshot follow-up (2026-10-16)

The user's new screenshot and genuine active-build checkpoint reproduce the
wrong Weaver tilt after the earlier heading and END-restoration fixes. The
previous negative-Z sample did not prove correct facing. A full-media Clang
run loads that checkpoint through normal F8 and reproduces the supplied view.
Read-only observation confirms the direct Engine hull branch, no MeshInstance,
the expected heading, and an unchanged ship position.

**Source contract and repair:** the original exporter writes root motion in
model coordinates. `FullStream` applies an Object delta in its owning Engine
frame, while `SpaceShip::renderSpaceShip` adds the model-axis correction `C`
afterward. The old rendered root is `heading * authoredMotion * C`; the
required composition is `heading * C * authoredMotion`. The native Animation
owner now accepts an explicit basis on the selected script's root FullStream
channels. It converts relative translation by `C` and orientation by
`C * rotation * inverse(C)` before existing offset, blend and start handling.
Joint channels, shared archive bytes, unconfigured instances and Windows keep
their original behavior. Validation completes before any channel is changed;
release destroys the basis with its script instance. No public vtable, saved
field, gameplay rule, RNG call or simulation ordering changes.

`TFabricator::initFab` configures only Weaver `Sc_build`, using the authored
mission class from the initialization record. An initial integration run
exposed that the runtime class is not set yet at this point. That run retained
the wrong pose and is not fix evidence. The corrected metadata gate activates
the real channel before its first start. The earlier explicit base-frame
restoration at END remains necessary for active-build reloads.

**Automated execution:** Linux x86_64 GCC16.2.1 and Clang22.1.8 full-media Debug
build `frontierx_native_shell` and `frontierx_game_material_manager_probe`.
The existing `portability.game_material_manager` check now loads an independent
root/child fixture through real Animation, FullStream and Engine owners. It
compares the complete rendered matrix with `heading * C * authoredMotion`,
an asymmetric front anchor toward the planet, and the unchanged child joint.
Four headings and repeated reloads cover END, reverse, stop, release and fresh
orders. An unconfigured sibling keeps the old default after release. A second
motion covers converted translation, root offsets and position/orientation
changes. This replaces the insufficient Z-sign assertion. Focused layout,
app/script01/script02/script03/Sol_T schema, material and supplied-font shell
CTests pass under both compilers. Fatal Clang ASan/UBSan with leak detection
also passes the existing parser, data-root and net-buffer checks.

**Retail-input runtime:** the corrected full-media Clang shell loads the exact
same immutable checkpoint and camera through normal F8. The actual channel
reports its basis enabled and the render-time root matrix changes as expected.
The inspected after-capture shows the Weaver leaning right toward the planet,
where the before-capture shows the reported upright, outward pose. Position is
unchanged; the run continues and closes normally with zero mission audio
streams and idle hint ownership. The checkpoint copy remains byte-identical.
Short real Clang regressions also execute normal F7, F8, continuation and
regular close in accepted bounded training and full-media TM1. Both reload
through the original mission owner and close with idle hints and zero mission
audio streams. These are bounded visual/load observations, not a complete
mission acceptance or proof of every interrupted build order. The original Hawkes movie repair
and its separate runtime limits below remain unchanged.

The candidate is ready for a narrow user recheck of the saved construction,
normal completion/reverse and a new order. User media acceptance, native
Windows Debug/Final/module ABI, hosted CI, known whole-process leak limits and
full campaign acceptance remain open. Retail saves, captures and raw logs stay
external.

## Weaver build pitch and Hawkes rescue film (2026-10-16)

This earlier repair record is retained for its load and film evidence. Its
Z-sign interpretation is superseded by the complete model-basis follow-up above.

The user confirms the other delivered repairs work and reports two remaining
TM3 presentation defects: the Weaver leans toward space during construction,
and Admiral Hawkes' rescue film does not appear. This is gameplay feedback,
not complete media acceptance. The earlier saved-yaw repair remains valid but
did not resolve the construction-animation pitch.

**Weaver source and synthetic execution:** `TFabricator::resolveFab` restored
`Sc_build` at END on a yaw-only loaded root without supplying its base frame.
`FullStream::start_without_error_check` then subtracts the END rotation to
preserve the current pose. Reversing that animation consequently pitches the
Weaver toward space. The native `M_WEAVER` load branch now supplies the existing
`script_start` API with the loaded base transform explicitly. Other races,
Windows, ordinary animation directions, build rules and saved bytes are
unchanged. No model rotation or substitute animation is introduced.

The existing material probe now loads an independently generated 65-degree
Object-root channel through real `IAnimation`, `FullStream` and Engine owners.
It reproduces the old neutral-END/upward-reverse failure and requires the fixed
downward END, level reverse, stopped/released order, fresh forward build and
unchanged position across four headings and repeated yaw-only reloads. Joint
door animations were not covered by the previous heading-only regression;
this test protects the specific integrated root-animation lifecycle.

**Film source binding:** the original TM3 capture sequence correctly calls
`EndMissionVictory(2)`. `MenuEndGame::checkForMovie`, mission ID 2 on victory,
had its `cq_1.mpg` call commented out while still setting movie 1 seen. The
native full-video build now calls the real `MovieScreen` before marking it
seen, and does neither when movies are explicitly excluded. Decoder/playback
errors cannot fall through to the seen setter. The original unlocked movie-1
button in `Menu_mission` also calls `MovieScreen`, without changing progress.
This lets the user replay the earned rescue film without another complete TM3
run. Existing seen bits are not reset; other film bindings and Windows remain
unchanged. The existing shell suite protects these source bindings explicitly
as source evidence, not as a synthetic victory or successful video stub.

**Local validation:** Linux x86_64 GCC16.2.1 and Clang22.1.8 full-media Debug
rebuild `frontierx_native_shell` and `frontierx_game_material_manager_probe`.
Focused layout, app/script01/script02/script03/Sol_T schema, material and
font-enabled shell CTests pass. Fatal Clang ASan/UBSan with leak detection also
passes these owners plus the existing parser/data-root/network-buffer checks.
An actual Clang full-media `--movie-source cq_1.mpg` diagnostic displays the
lawfully external rescue film, including separated frame captures, and returns
normally after natural EOF in about 86 seconds. This exercises original
MovieScreen, decoder, GPU and audio lifecycle; it does not execute a victory.
An automated selector attempt instead entered unintegrated TM4 and aborted at
the existing missing-Script04 prerequisite guard. That attempt is a failure,
not movie-replay evidence; TM4 remains the next P2 integration boundary.

**Bounded retail-input execution:** an immutable genuine TM3 checkpoint with an
actively building Weaver is loaded by normal F8 in the full-media Clang shell.
A read-only parent debugger observes the real `TFabricator::resolveFab` call
and the authored FullStream output: the model-front vertical component is
about -0.9063 at the restored build endpoint. A subsequent natural closing
sample is -0.7375, progressing toward level rather than tilting upward.
Position remains unchanged across the samples. Observation stops after three
outputs; the run continues and closes normally with exit 0, audio streams zero
and idle hints. The input copy is byte-identical afterward. An initial observer
watched the unused direct-interpolation overload and saw only the actual
resolve call; it is not pose evidence. The corrected observer reads the real
stored-output path without writing gameplay state.

The checkpoint available during that earlier run contained an idle Weaver,
not an active build. The subsequent screenshot checkpoint above contains an
active build. Source
review finds no animation start in idle initialization/load; its next regular
order begins at BEGIN from the loaded level pose. New/rapidly interrupted
orders still need user visual confirmation; no speculative pose reset is added.
Renewed user visual/film-trigger confirmation remains separate. No mission objective, progress bit,
checkpoint or retail asset is fabricated. Native Windows Debug/Final/module
ABI, hosted CI, whole-process leak limits and full campaign acceptance remain
open. Raw media, saves, captures and detailed runtime logs stay external.

## Weaver construction facing after load (2026-10-16)

The reported reversed Weaver pose exposes a concrete native saved-heading
reconstruction defect. The original Mantis model front and construction-facing
rule already agree; no model-specific flip is added. Native load now restores
the stored minus-J heading correctly. Production-math roundtrips, compilation,
sanitizers and pending visible confirmation are recorded in
[the heading ledger](GAME_MATERIAL_PATH.md#saved-ship-heading-and-weaver-construction-facing-2026-10-16).
Existing incorrect saved headings are retained rather than guessed from targets.

## Collector upgrade effect ownership (2026-10-15)

The user's later crash exposes an original Mantis Collector upgrade effect
receiving no mesh tree. Native extension ownership now transfers the initialized
add/remove subtrees, retaining their renderers for the original growth/morph
effect. Completion, cancellation and active upgrade reload use that same tree;
no required effect or submodel is hidden. The independently traced F8 callback
crash is repaired at AI record serialization, with no portrait/media teardown
workaround. Exact source, CPU/sanitizer checks and runtime limits are in
[the crash repair evidence](NATIVE_TERRAN_CAMPAIGN.md#collector-upgrade-and-repeated-f8-crash-repairs-2026-10-15).

## TM2 user completion and damage-smoke follow-up (2026-10-15)

The user reports that Mission 2 ran perfectly, with one small presentation
defect: smoke floats above damaged Corvettes. The native original damage
post-render owner now corrects its raw mesh anchor into the direct hull's model
frame while preserving plume orientation, simulation, RNG, stored state and
Windows behavior. Failing-before/passing-after material regression and rebuilt
GCC/Clang/fatal-sanitizer evidence are in
[the focused renderer record](GAME_MATERIAL_PATH.md#corvette-damage-smoke-anchor-2026-10-15).
Record user acceptance of the reported mission run; corrected visible contact
remains a narrow retest. No full replay is requested and unreported individual
scenario, Windows/ABI/hosted-CI and whole-process leak evidence remains pending.

## P2 TM2 required presentation inventory (2026-10-15)

P2 begins with `T_Mission2.dmission` and the original `Script02T` owner. The
existing P1c acceptance remains recorded above the historical evidence below.
The user's earlier TM2 completion and accepted P1c do not establish each newly
requested P2 media/save/defeat check. The [campaign matrix](NATIVE_TERRAN_P2.md) records the current
mission status; no whole-campaign presentation pass is inferred here.

**Source review:** `Script02t.cpp` and its real speech/briefing consumers in
`Scripts/Helper/helper.cpp` identify these required intervals:

| Interval | Required content and original owner | Remaining evidence |
|---|---|---|
| Briefing and opening | Original animated briefing slots, Halsey speech, Blackwell speech/head, authored cameras and start view | Existing bounded entry/reload evidence remains valid; renewed candidate assessment includes fonts and speaker switch/clear |
| Alkaid exploration and attacks | Later-created Mantis Frigates, Scout Carriers/fighters and Zorap supply ship, their hulls, animations, weapons, impacts and spatial audio | Catalogue admission alone does not prove late spawned instances or all effects; assess these during genuine combat and reload |
| Construction and capture | Ballistics Lab, Missile Cruisers, Troopship upgrade and capture, build/research UI and corresponding models/effects/audio | Exercise actual production/research/capture checkpoints; retain the existing VLaunch, animation and command-owner regressions |
| System discovery and travel | Original system enable/visibility changes, jumpgates, hard fog and both maps, including system connections | Compare hidden and discovered systems, ordinary map navigation, jumps and restored state after F7/F8 |
| Victory and defeat | Original camera transfer, Blackwell/Halsey communication, Marine/Intel audio, uplink sound, result teletype and normal result/next entry | User completion does not separately confirm every late speaker/media state or defeat/retry; use genuine outcomes |

**Read-only external metadata inspection:** the supplied lawful speech,
MSpeech, Music and interface UTF archives contain every one of the 37 active
literal WAV requests in TM2, after excluding source comments. This includes
the late victory/failure requests and uplink. Both source-selected music files,
`determination.wav` and `danger.wav`, are present. Of the 36 distinct speech
requests, 34 have matching TXT companions. The two without companions are the
original `CHAR_NOCHAR` briefing lines `PROLOG02.wav` and `M02HA01b.wav`.
This establishes availability by archive entry name, not decoding, playback,
lip synchronization, appearance or correct source-state restoration.

The original Helper tables intentionally map `CHAR_MARINE` and `CHAR_INTEL`
to empty portrait archetype names. Their victory speech is audio-only through
the original fallback. A fabricated Marine/Intel portrait would change authored
behavior. Blackwell and Halsey use the original animated message archetypes;
their switch, clear and reload behavior remains required.

This inventory changes no production code, persistent schema or module ABI and
adds no build/runtime pass. It does not close missing Harvester depletion media,
whole-process leak findings, unverified unit coverage, hosted CI or native
Windows Debug/Final/ABI checks. Missing models in the broader catalogue remain
tracked in [unit coverage](NATIVE_UNIT_COVERAGE.md); they become mission blockers
when an actual required TM2 owner requests them. Required presentation remains
enabled in final candidates, including later generated content.

## P1c accepted by the user (2026-10-15)

The user explicitly closes P1c in the delivered native Linux scope. P1b was
already accepted; P2 campaign expansion is next. This documentation-only update
adds no test execution. Earlier pending milestone statements are historical;
unperformed scenario checks, missing media, unit coverage, whole-process leaks,
Windows/ABI, hosted CI and separate M3 findings retain their recorded status.
See [the current acceptance ledger](NATIVE_TERRAN_CAMPAIGN.md#p1c-completed-by-user-acceptance-2026-10-15).

## Original-family font input supplied (2026-10-15)

The user now provides all four Arial Narrow faces privately. The local source
folder is explicitly ignored and no font is tracked. Runtime uses copies and a
nine-entry mapping outside the repository: actual original Arial Regular/Bold,
Arial Narrow Regular/Bold and the original available Bauhaus face at the source
resource weights. Font files are not bundled in source, builds or CI artifacts.
This supersedes earlier same-day statements that the Narrow input is unavailable.

The supplied OpenType family groups Narrow under typographic Arial (nameID16),
while its original GDI family Arial Narrow is nameID1. FreeType reports Arial;
the bold PostScript name also carries its style suffix. The strict native owner
now additionally validates bounded authored UTF16BE legacy family names from
Unicode/Microsoft nameID1 records. It does not infer families from filenames,
remove arbitrary suffixes, substitute faces or relax weight validation. The
first strict startup correctly refused the old incomplete identity comparison;
that failed interval is not mission-rendering evidence.

Corrected strict startup and the entire original catalogue execute under fatal
ASan/UBSan with the supplied family map and no media exclusions. The prior native
allocation-pair errors are repaired; the interval still fails final LSan3124bytes/
sixallocations with DBus allocation stacks, including unresolved unloaded callers.
This is not a whole-process sanitizer pass or a campaign resource-growth pass.
Two actual Clang F8/F7 cycles with the new strict map now complete and exit0,
with correct original objective-dialog captures after reload, visible fleet/HUD
and both minimaps, audio0/idle hints on close, and original portrait frame/draw
activity. Full player visual/audible assessment remains open.
Strict glyph/layout appearance after ordinary save/load and user acceptance are
recorded separately; supplying fonts alone does not close presentation acceptance.

## P1c transition media ownership (2026-10-15)

Latest user feedback confirms Cruiser construction and save/load, and reports
incorrect missile trajectories and a nonworking objectives menu. Native missile
pitch/muzzle ownership and the original objective dialog are now connected;
visible weapon/UI behavior and restoration after repeated F8 remain explicit
runtime checks. Broader unit-owner integration retains models, textures, effects,
portraits and sound requests; compiling a factory does not prove complete media.
The retail flagship and Wormhole persistence gaps are recorded in
[unit coverage](NATIVE_UNIT_COVERAGE.md).


The user now reports the preceding briefing/muzzle/camera corrections work and
continued TM2 play reaches a Missile Cruiser construction crash. The diagnosed
missing original VLaunch/CloakLauncher factories are now in the native graph;
new-cruiser model/weapon/audio/save restoration still requires the genuine
queue/completion/reload test. Existing user feedback does not close strict fonts,
effect/depletion-audio findings, process leaks or full transition-resource checks.

Native feedback repairs now retain the original briefing border pixels/palette
and corners at the actual animation-cell extent, convert cinematic camera angles
through the exact runtime `TRANSFORM` convention, and align fixed gunboat muzzle
frames with the original hull-model axes. Fresh post-opening capture before F8
shows planet, stations and the starting fleet in the playable view; actual F7/F8
also executes. Required media stays requested, with the existing interim-font
limitation. Precise inactive-frame appearance, moving weapon origins, ordinary
selection/orders/zoom and complete media after reload remain user retest items.
See the [feedback contracts](NATIVE_TERRAN_CAMPAIGN.md#feedback-repair-contracts-2026-10-15)
and [muzzle owner](GAME_MATERIAL_PATH.md#second-terran-fixed-barrel-origin-feedback-2026-10-15).

Latest user feedback confirms successful TM1-to-TM2 transition and mostly working
briefing/opening, but reports undersized inactive portrait frames, below-hull
Mantis weapon origins and an empty/unusable view after the cinematic. These are
open correct-start/presentation blockers; prior bounded render/save evidence
does not constitute visual or player-control acceptance.

P1c retains every required presentation/media path. The original materials and
lighting configuration now applies to a direct second-mission diagnostic as well
as the first-mission campaign entry; ordinary transitions retain that same
configuration. No media exclusion option is added to the transition loop.
Mission close now retires detached spatial one-shots and looping playback,
clears status text/name ownership, and clears global system-map fullscreen/input,
rally/zone and old-system ping state. Existing owner regressions cover those
concrete stale-resource defects. The original speech/portrait/subtitle/teletype
close notifications, map/terrain texture cleanup and music load/playlist owners
remain in use. Source review of them does not prove visible/audible transitions.

Original TM2 briefing/Start, opening, F7/F8/F7 and normal close now execute
through 3005 source frames in the full-media Clang candidate. Original speech
requests find their real files; two portraits execute253 frame changes/750 draws.
The original authored cinematic cameras load and ChangeCamera no longer uses an
unavailable-owner path. Mission close reports audio0 and idle hints. This remains
bounded render/lifecycle evidence with the explicitly interim single external
font, not strict typography or full visible/audible acceptance. Complete TM2
models/effects, both minimaps, portrait switch/clear and speech/music/spatial sound
before/after genuine result-driven transitions remain user checks.
A strict whole-process second-mission ASan/UBSan close still fails LSan;
recorded unloaded frames are mapped to NVIDIA/PipeWire/SPA boundaries alongside
DBus stacks. Allocation cleanup cause remains open. The focused synthetic
sanitizer checks do not turn that failure into a pass, and no suppression is used.
The user confirms no strict font map is currently available; original Arial
Narrow remains an external input requirement. Colorful impact/explosion artifacts,
missing depletion recordings and whole-process mission LeakSanitizer findings
are carried forward explicitly. See the
[P1c acceptance ledger](NATIVE_TERRAN_CAMPAIGN.md#p1c-integration-and-acceptance-ledger-2026-10-15).

## P1b completed by user acceptance (2026-10-15)

The user explicitly requested that P1b be marked completed after the full
first-Terran mission run, reported original victory and save/load, and the
subsequent presentation fixes. Record this as user acceptance of the delivered
Linux mission scope. The next active delivery step is P1c: original result/
return/progress, next-mission entry and defeat/retry/restart/teardown.

This milestone decision does not turn unresolved defects or unexecuted checks
into passes. Retain the colorful impact/explosion artifact investigation,
unavailable Harvester depletion voices, strict original-family font mapping
and whole-process mission LeakSanitizer finding as explicit follow-ups. Actual
GitHub CI and native Windows Debug/Final/module ABI execution remain pending;
M3 remains separate and unaccepted. Normal victory teardown and detailed defeat/
retry evidence have not been independently established and remain P1c work.
No additional gameplay, runtime, build or test execution is claimed by this
documentation-only status update. Earlier dated open/P1b-blocker statements below
record the implementation history; this acceptance entry supersedes their
milestone status, not their unresolved technical findings.


Latest narrow speaker feedback (2026-10-14): native SpaceShip's red communication
half-circle marker used a J-axis tilt and a displaced old-model-space anchor.
Its render-only owner now keeps the center below the corrected hull footprint
and uses the unchanged phase about local K in that plane. Four-phase actual-math
regression, complete standard/full-media GCC/Clang builds and focused fatal
sanitizers pass; visible stability while speaking/moving/reloaded remains a user
check. Texture, blend/depth, highlight/audio lifecycle, simulation, persistence
and Windows are unchanged. This does not resolve the other documented P1b
media/effect gaps. See [speaker-marker evidence](GAME_MATERIAL_PATH.md).


## User mission run and remaining feedback (2026-10-14)

The user reports completing the first Terran mission with its regular victory
and exercising save/load. The precise production/combat save timing and regular
defeat were not separately confirmed. The user reports small selection corners,
keel-up emergence after wormhole jumps, and brief very colorful impact/explosion
artifacts. These remain presentation checks, not accepted passes.

The window disappearance is a confirmed SIGABRT in native unit communication,
not verified regular victory teardown. The core identifies a Harvester request
for a speech filename absent from the supplied retail archives. Native missing
communication now returns the original Windows failure ID0 with an explicit
diagnostic; mandatory scripted speech remains strict. The missing depletion
voices remain an open media requirement; no alternate recording is substituted.
See [audio diagnosis](NATIVE_AUDIO_STREAMER.md).

Native new warp-ins now use the current ship-control axes rather than the old
CQ2 -K-forward/J-up initializer. This intentionally repairs initial runtime
orientation and the corresponding saved orientation; layout, heading, gate
position, timers, velocity formulas, call order/RNG and Windows remain unchanged.
Already-active faulty jumps in earlier saves are not normalized. Focused math
checks do not replace a fresh visible jump/user retest. Selection investigation
has disproved the initial instance/archetype-ID suspicion: HARCH already performs
the conversion. No bounding-box ownership change is justified by that suspicion.
Native DrawSelected now uses the corrected direct-hull footprint for local
selection corners, retaining original proportional width/cap and cached gameplay
bounds. The former raw model X/Y footprint used model height as ship length.
Platforms, fast hulls, special rendering and Windows keep their original bounds.
The brief capture retry has not reproduced the colorful artifacts; their cause
and visible acceptance remain open. Complete P1b acceptance is not declared.


Status: 2026-10-14. P1b is open. This is a production-owner inventory and
integration record, not visual or audible acceptance. The selected mission is
`T_Mission1.dmission`, whose script module `script01.dll` is restored from
`Scripts/Script02T/Script01T/Script01T.cpp`.

## Required production paths

Latest reported remaining visual defect is the Harvester beam origin. Its native
render-only authored muzzle now shares the exact hull visual transform; existing
asymmetric numeric/production-math checks execute, while renewed visible
harvesting remains user-operated. Wheel zoom reaches the original camera owner.
Native rendering removes unused fixture uploads and safely reuses retired
buffers; all required textures, passes/effects and media stay enabled. Quiet
bounded full-media timing reaches the original30-FPS cap or approximately60 FPS
with the explicit original uncapped option. Later combat/complete presentation
are not accepted from those timings. See [feedback evidence](GAME_MATERIAL_PATH.md).


Concrete ambient-animation owner repair: original Platform::initPlatform used
`PLATFORM_INIT::animArchetype`, which remains the obsolete S32 value-1 while
MeshArch owns the genuine pointer-sized script set. Native platforms now borrow
`meshArch->GetAnimArchtype()` for the unchanged named ambient script and
LOOP/BEGIN request, matching the existing Fabricator correction. Windows keeps
its original expression; no saved fields, interface, animation timing or RNG
change. Missing required named scripts fail explicitly in the original-legacy
mission configuration. Source/resource review identifies Turret `Sc_radar`,
Mantis EyeStalk `Sc_pump` and BioForge `Sc_claw` as real affected dependencies.
GCC/Clang compilation covers13 production platform factories; instrumented
syntax checks cover GunPlat/BuildPlat/RefinePlat. Independent final builds also
link all five standard/optional/instrumented graphs; existing focused checks
pass as recorded in BUILD_GRAPH. Actual radar/pump/claw motion,
reload restoration and full visual acceptance remain pending user/runtime
checks; successful compilation is not animation evidence.


Current feedback follow-up: 27 original source-owned context/map/edge cursors
now load through NativeAppCursor with deterministic decoder and real GCC/Clang/
sanitizer host evidence. Native fixed GPU state reuse passes captured-lifetime
and exact-image checks under both compilers. Actual ready-HQ F8/short rendering/
normal close succeeds, but its measured11.2 FPS remains low; no smoothness fix or
full user visual/audio acceptance is inferred. See [render evidence](GAME_MATERIAL_PATH.md).

Leak attribution remains a failure: strict standalone SDL Wayland/PipeWire/
Vulkan clear/present and repository-shader variants reproduce2908 bytes/six
DBus allocations without FrontierX or retail inputs, despite balanced public
teardown and final SDL_Quit. Same-process module maps identify NVIDIA/PipeWire
frames before unload. Audio-only lifecycle exits0. This isolates a dependency
leak class, not its cleanup cause; the actual mission's additional80-byte direct
allocation remains unattributed. The mission2988-byte/seven-allocation failure
is neither suppressed nor accepted.


Post-reboot recovery, 2026-10-14: the source repairs and recorded bounded
observations survive. Private temporary builds, captures and saves do not.
Rebuilt durable external binaries must recreate the original campaign flow;
neither a missing checkpoint nor a successful asset load is presentation
acceptance. Current technical results are recorded in [BUILD_GRAPH](BUILD_GRAPH.md).

The recreated original flow reaches Austin/Frigate combat, Fabricator arrival
and original HQ production. Private genuine F7 copies distinguish active
HQ2167/3500 from completedHQ3500/3500; normal completed-HQ F8 and close succeed.
The new source-owned BUILD cursor integration fixes a proven prerequisite of
free-space previews, preserving the original game condition. Actual optional
Clang original ready-HQ F8/Turret selection now displays green free-space and
red invalid-position preview geometry through the original three-child legacy
material path. Normal close succeeds. The remaining cursor visual comparison against Windows and full user
presentation remain open;
see [render-owner evidence](GAME_MATERIAL_PATH.md). The actual loaded Debug
HQ scene still measures10.866FPS over30 seconds; rendering-cost attribution
and a controlled performance gain remain unproven.

The inventory must include the initial map, embedded terrain maps, archetype
dependencies and later construction/spawning, not only successfully loaded
initial assets. Retail files, extracted asset inventories and runtime captures
stay outside the repository. The following independently reviewed source facts
establish required paths; they are not an exhaustive retail asset census.

| Demand | Original owner / caller | Current boundary |
| --- | --- | --- |
| Initial ships, planets, wormholes, terrain and later built units | Mission/GenData, archetypes, Engine/PolyMesh, MeshRender, MaterialManager, TextureLibrary | Native loading/rendering is bounded. Complete submodels, attachments, animation, material techniques and texture-format/mip requirements remain to be measured. |
| Later enemy frigates and scout carriers | Script01 `CreatePart("GBOAT!!M_Frigate", ...)`, `M_SCOUTCARRIER` attack/stance branches and `MANTIS_FRIGATE_RUSH` | Later assets and AI-created units are required even if absent from the initial camera view. |
| Background | SpaceEnv/SpaceEnvironment, nebula archetypes, `StarBk.tga` | Production owner is linked. Active background rendering uses stage-zero diffuse blending, texture sampling, alpha test and nebula geometry. The source two-UV/star-background stage-one branch is commented out; its state contract does not establish an active runtime demand. Intended high-background reference coverage remains open. Excluding intended background content is not acceptance. |
| Lighting, shading and multipass materials | Game lights, material/effect owners and native render boundary | Original scalar light/material rendering and authored UV2 now reach the native receiver. Complete mission effect-technique and visual coverage remain open. |
| Weapons, hits, explosions, shields, engines and abilities | Unit archetypes, EffectPlayer, production particle/projectile owners | Load success does not establish visible output. Later combat and ability coverage remain open. |
| Mission/briefing communication portraits | Script01, SoundManager, LFParser, GENDATA IAnimate, original Animate hot rectangle | Native lip parser and communication lifecycle execute in focused checks and bounded mission captures, with source frame advancement/draw counters. Complete visible animation, switching, completion and reload fidelity remain unaccepted. |
| Selection presentation, HUD, markings and both maps | Menu_Toolbar and its resource controls; original selection/sector-map owners | Both maps, system links and original selection presentation require runtime inspection before acceptance. Communication animation does not prove selection presentation. |
| Speech | Script01 `Halsey`/`Blackwell` briefing animation and `Halsey2`/`Blackwell2` mission messages | Original WAV speech with matching TXT lip frames must resolve. External optional shared-LGPL decoder is required for retail IAC speech. |
| Music | Script01 `PlayMusic("mystery.wav")`, production MusicManager save/load | Mission music must remain enabled and be verified audibly, including reload. |
| Scrolling-text notification cue | Original ScrollingText `SetText`/`SetTextString` and SFX::BEACON | Original source/retail mapping agrees. Native decoded playback executes after normal selection/movement without the cue exclusion; audible acceptance remains open. |
| Spatial effects | Original SFX::Play camera/system/fog routing, attenuation, pan and looping ownership | Native routing, stereo pan and the bounded instance-pool/cache/RNG synthetic checks pass. Cutoff-tail/cursor behavior and full mission audible playback remain open. |
| Movies | MovieScreen/IVideoSystem, campaign/resource requirements | A genuine native video owner has generated CPU/SDL audio lifecycle evidence. Native video-enabled first-time Terran campaign entry now calls original MovieScreen for cq_intro.mpg at the original production position; actual GPU caller, visible/audible intro fidelity and a retail reference remain pending. |

Read-only lawful-resource follow-up inspects22 Terran/Mantis ship/platform
models selected by original production paths. These use legacy diffuse/emissive
materials and native-supported eight-bit indexed/palette textures; no concrete
chrome/CQ2 shader demand was established in this sample. This is a bounded
sample, not the complete mission dependency closure. Other initial actors,
upgraded/extended platform parts, launched fighters/mines, contact/explosion/
shield effects and AI-triggered Mimic/Khamir/RepellentCloud presentation still
require complete mappings and actual visible checks. Generic unsupported shader
boundaries become mission blockers only when a real selected resource demands
them; absence in22 models does not prove complete support.

The gameplay-owner source audit expands the later asset closure further:
Mantis retains the full technology tree, and the enabled AI requests light and
medium ship production, platforms, harvesters, scouts, minelayers and
fabricators. `SPlayerAI::initArchetypeIDs` includes Seeker, Scout Carrier,
Frigate, Khamir, Spinelayer, NIAD and Zorap archetype choices. The Terran tree
later unlocks refinery, outpost and light-industry construction while its
selected ship options are Fabricator, Corvette and Harvester. Their dependent
textures, weapons, mines, submodels and effects belong in the mission inventory.

`--base-textures-only`, `--skip-legacy-lighting`, `--skip-space-background`,
`--skip-animated-portraits`, `--skip-mission-sfx`, `--skip-mission-music`,
`--skip-movies` and other UI exclusions may describe interim candidates only.
None may hide intended mission contents in the final P1b handoff.

## Communication animation dependency

Named blocker: Script01's first Blackwell communication reaches
`ISoundManager::PlayAnimatedMessage`, while the native sound owner previously
required excluded portraits and returned no animation to the original briefing
caller. Existing speech playback alone cannot protect lip-frame parsing and
ownership transfer to `MenuBriefing::PlayAnimatedMessage`.

The original LFParser now builds natively. Native mounted-filesystem reads are
synchronous; the original Win32 overlapped-I/O, virtual allocation/protection
and CD retry branches remain on Windows. Native runtime frame storage uses a
private U32 heap array and is not serialized. The source frame grammar and
record order remain unchanged. Filename lookup follows the native speech
owner's bounded unambiguous case matching for uppercase script references.

The native sound owner creates the original GENDATA animation, reads its actual
lip-frame resource, initializes the original Animate with the briefing or
FULLSCREEN parent and source fuzz setting, then plays the speech. The stream
retains the parser while the frame array is used. `GetAnimation` implements the
original single ownership transfer for briefing continuation. Stop, completion
and flush defer destruction or pause a transferred animation, matching the
original SoundMan node lifecycle. Resource and parser failures remain explicit.

The existing shell lifecycle check supplies independently generated PCM and a
three-record lip file. It requires the production parser to return exactly
`[3, 3, 12]`, including the source hold-frame marker, and checks reinitialization
with an uppercase filename. This disproves the missing parser dependency; it
does not represent retail portrait animation or complete mission presentation.

Compilation evidence: the optional Clang Debug
`frontierx_native_mission_media_owner` target compiles LFParser and the native
sound communication lifecycle on Linux x86_64. Executable checks, GCC,
sanitizers, visible/audible retail runtime, transferred-animation teardown and
native Windows Debug/Final/affected ABI execution are pending until their
results are recorded. Full user mission and presentation checks remain open.

Bounded retail runtime: the same optional Clang build entered the source TM1
loop with `--skip-animated-portraits` removed and closed normally, with zero
mission audio streams and idle hint ownership. That first timed run did not
reach Blackwell's arrival/speech trigger and therefore supplies no portrait
evidence. A subsequent gameplay-owner run used normal selection/movement and
early and active-state F7/F8. Its private owned-window capture shows the actual
Blackwell2 communication portrait and source multiline subtitle after reload.
The media owner independently inspected that capture. A later bounded original
controls/save-reload run records two real portrait starts, 138 source frame
changes and 232 source shape draw calls before normal close. These counters
establish execution of the retained animation owner, not correct animated
pixels or timing. Switching/clearing across all messages, briefing continuation ownership and
full mission audible output still require checks. No capture or retail data
was added to the repository.

External resource inspection through the existing UTF metadata reader confirms
legacy openFLAME mesh/material nodes for the selected Corvette and Blackwell
models, including diffuse/emissive/ambient/specular fields and attachment
hardpoints. The App MeshRender path can promote a legacy material through
MATMAN when the candidate material directory resolves its derived script name;
legacy resource format alone does not prove which rendering branch executes.
The compiled Script01 speech census contains forty distinct animated messages
and six audio messages. All eighty-six required speech and lip-frame entries
are present in the lawful external `MSpeech` archive. This is presence evidence,
not decoder, parser or whole-mission playback evidence. The original disabled
`#if 0` message annotated as a missing sound file is excluded rather than
replaced. Detailed resource listings remain private outside the repository.
Production CPU `ILightManager::light_vertices_U8`, material color folding and
`LIGHTS::ActivateAmbientLight` must be respected when integrating lighting.
Further source review identifies a decisive distinction: the default
`FaceGroupRender::Render` contains `if (false && ...)`, so it does not call
the original `MMaterial<RPVertex>::Render` CPU-lighting routine. Its ordinary
non-CQ2 branch draws the normal-bearing vertex buffer with lighting disabled
and texture-factor color; vertex-buffer construction itself assigns random
RGB values. Therefore registering game lights in `ILightManager` alone does
not establish visible model lighting. Optics particles and the legacy
PolyMesh caller explicitly disable hardware lighting. Adapting the retained
CPU-lighting routine requires evidence for the selected material, UV and
emissive paths and an actual light-to-pixels check; intended retail shading
remains an open reference and runtime gate.
Selected external model inspection confirms a concrete second-UV dependency:
Blackwell and selected Mantis models reference emissive maps with original
UV1 wrapping and contain second texture-index arrays. Additional production
model candidates also carry those arrays and emissive references. The inspected
materials have no chrome names. This bounded census establishes that routing
all legacy materials through the first-UV path is insufficient; it does not
yet close the exact archetype-to-model inventory for every later AI production
choice. Detailed material scalars and resource listings remain private.

The native legacy material adapter now submits the retained source
`ILightManager` colors and exact diffuse/emissive folding through RPVERTEX2,
with the selected source textures, alpha/depth/blend states and independent
per-corner second UV coordinates. Capturing the original corner coordinates
does not change vertex cooking or its random calls. Original explosion
`SplitFaceGroup` propagates those coordinates using its existing returned edge
ratios and emitted triangle order, including repeated splits. No persistent
fields are added: original Explosion/Debris object interface maps expose no
save interfaces and the object-list writer omits those transient owners;
normal restored objects derive the runtime coordinates from their archetype.

Independent optional GCC/Clang and Clang ASan/UBSan material fixtures execute
the checked native draw, exact source lighting/material colors and states,
unchanged cooking/RNG, authored UV2 seams and first/repeated explosion splits.
The optional Clang synthetic GPU fixture also passes actual readback for
ambient restoration, textured emissive addition/suppression and those split
paths; all seven split/seam mesh owners are released. This is synthetic
production-owner evidence, not complete retail presentation or a Windows
comparison. Native full-mode chrome/CQ2 materials and unexpected Portion/Puffy
paths fail explicitly. The selected Mantis strategy has no established
Solarian MassDisruptor/Synthesis or Vyrium teleport demand; ordinary mission
gate travel uses `TObjWarp` rather than the clipped teleport owner.

The communication portrait enables its original fuzz overlay outside briefing.
`Animate::InitAnimate` requests `videoFX.tga` through the DAA4 format class when
source frame locking is disabled. The native owner now aborts on a missing
texture handle instead of silently omitting that required effect. The original
D3D9 DAA4 class explicitly permits RGBA8888 as a supported alternative to
RGBA4444, so RGBA8 class resolution does not itself require four-bit
quantization. Optional Clang Debug `frontierx_app_toolbar_owner` compilation
passes; actual overlay pixels and complete portrait runtime remain unverified.

The native sound owner now also restores source-derived music/effects gain
ramping (0.0005 per millisecond), immediate dialogue ducking and communication
versus chat gains. This fixes the concrete native `TweekMusicVolume` boundary:
it previously set music gain to zero without any update restoring the source
ramp. The owner's explicit manager-disabled states are retained. The existing
sound lifecycle fixture additionally checks a communication level change
reaches the real stream volume. That assertion and actual sound completion,
explicit stop, speaker-highlight cleanup and repeated lip-parser initialization
execute in the independent GCC/Clang default and optional shell checks and
optional Clang ASan/UBSan with leak detection enabled. Audible whole-mission
gain and dialogue balance remain unverified.

## Original notification cue

The source `sfxdata.xmf` maps BEACON ID146 to `Beacon.wav`, volume 1.0 and
cutoff 1.0. Read-only external inspection of the lawful `globals.dll` SFX
resource independently confirms that interoperability mapping. The generated
native table includes the entry; its SFXCHUNK2 entries precede ordinary chunks,
so inspecting only its tail would incorrectly suggest absence. No resource
table bytes or external sample have been added to the repository.

The external sample uses IAC WAV tag 0x0402, mono 22050Hz. The existing reviewed
optional shared-LGPL decoder is required. Native source ScrollingText now
checks the real SFX owner, opened handle and playback result, failing explicitly
instead of ignoring a missing cue. Windows and the terminal-only text policy
are unchanged.

Bounded external runtime on the optional Clang Debug SDL/Vulkan build, with
FreeType and `/tmp/frontierx-voice-lgpl`, removed `--skip-notification-cue`.
Normal minimap camera movement, ship selection and right-click movement
triggered two real BEACON playback successes after actual stream restart,
observed by a private read-only breakpoint. The source loop ran 599 frames
and closed normally with zero mission audio streams and idle hint ownership.
Background, mipchains, legacy lighting, portraits, SFX and music were enabled.
The single external font and movie exclusion remained interim limitations.
This demonstrates the production cue owner, not user-confirmed hearing or
complete mission audio. Logs and owned-window captures remain private.

## Spatial audio boundary

The legacy `IStreamer` vtable has no pan slot. Its public/module ABI is retained;
a separately queried native-only `INativeSpatialAudio` interface carries the
source DirectSound signed hundredths-of-decibel pan value. Each native stream's
separate SDL logical device receives a postmix callback applying attenuation
to the opposite stereo channel. The callback uses an atomic pan value, owns no
game state and has the stream/device's lifetime. Nonstereo output is rejected
explicitly; changing to unsupported hardware marks the stream invalid.

The existing generated sound fixture extends its production stream check with
the native interface, valid positive/negative pan, invalid range rejection and
normal stop/close. The actual callback's sample transform is checked with
positive/negative samples at plus/minus 6000 and full attenuation endpoints,
protecting the logarithmic gain semantics rather than a linear balance model.

NativeSfx now uses source current-system and game-active gating, normal versus
hard fog visibility, the camera's rotated view rectangle, asymmetric vertical
distance attenuation, source offscreen/distant flags and signed stereo pan.
`NODROP` retains a silent spatially excluded sound as intended by the original;
that return value is not audible execution evidence. Camera updates revisit
active/looping handles in the source reverse-creation order without restarting
live sounds. The enabled-state exclusion remains explicit.

Compilation only: optional Clang Debug native window-manager/streamer,
native SFX and native mission-media owners compile with these additions.
The original ten-instance rotating sound pool, duplicate limits, victim
priority, cutoff ownership and its random starting slot are still absent from
the earlier native per-handle adapter and remain P1b fidelity blockers.
Actual positional playback, pan audibility, hardware-format migration,
callback teardown and sanitizer execution for this latest increment are
pending. These source/compiler facts do not establish complete mission SFX.

Independent automated execution (same date): existing native-shell and
native-window-manager checks pass with the latest spatial increment in GCC
Debug optional font/audio, Clang Debug optional font/audio and Clang
ASan/UBSan optional font/audio with leak detection enabled. They execute the
native pan query, valid/invalid pan ranges, logarithmic stereo sample gains,
normal stream stop/close, the real scoped ObjectList speaker lifecycle and
the production LFParser frame/reset assertions. No sanitizer report or skip
occurred. This supersedes compilation-only status for those synthetic paths;
audible retail spatial behavior and the source instance-pool/RNG contract
remain open.

The next 2026-10-14 increment implements the missing source pool/cache logic:
ten recency-linked instances, matching/unowned and offscreen/distant victim
selection, two duplicate ordinary samples or one looping sample, and the
source's single random starting slot whenever an instance must be assigned.
Live `NORESTART` playback retains its instance without consuming another
random number. Looping instances are excluded from cutoff checks. A cutoff
releases the owner while its audible tail remains live until natural finish
or instance reuse; a nonlooping handle close also retains the tail. Disable
stops all instances, including unowned tails. Playback status and cutoff
progress come from the actual native stream and SDL logical-device postmix
consumption; hardware buffering/cursor equivalence is unverified.

SFX preload now caches decoded PCM and its exact filesystem owner without
retaining a logical playback device/stream. Playback makes its own PCM copy;
forgetting the source cache cannot invalidate live playback. The cache owner
releases the filesystem reference and checks temporary stream close errors.
This removes the earlier native 64-stream exhaustion merely from opening many
source SFX handles. It does not broaden the audio decoder or supply missing
files. Existing source ID/filename/volume/cutoff data are retained.

The existing source SFX fixture adds seventy real logical handles, actual
looping playback with its original duplicate-limit drop, and exact C-random
call-count checks on assignment and live `NORESTART`. A source pool drop is a
verified priority behavior, never an audible pass for the dropped sample.
Independent compilation of the pool/cache increment passes GCC and Clang
Debug, both default and optional font/FFmpeg configurations, and optional
Clang ASan/UBSan. Narrow instrumented shell, window-manager, pipeline and
submission checks pass. The relinked `native_shell` fixture passes under
optional GCC, optional Clang and Clang ASan/UBSan with its explicit source-pool
success marker enforced: seventy logical handles, real loop duplicate/drop,
cache ownership, allocation RNG and live `NORESTART` assertions execute.
This bounded synthetic execution does not establish nonloop cutoff-tail
behavior, hardware playback-cursor parity or audible retail validation.

## Required font families

Source resource inspection identifies a mission presentation blocker that the
single-face interim font adapter does not solve. `Globals.rc` selects Arial
for subtitles, hints, money and planet text; Arial Narrow for status and
toolbar text; and BauhausMdITCTT for teletype/objective text, with different
weights. The interim single-face configuration uses one external face for all of
them; its family/weight limitations are addressed by the strict mapping below. The selected lawful retail font directory
contains only the Bauhaus face. Correct family and weight selection needs
lawfully external faces and native resolution; substituting the Bauhaus face
for all mission text cannot satisfy complete P1b presentation. Existing font
fixtures remain bounded adapter evidence, not this typography acceptance.

Native strict resolution is now implemented alongside the accepted single-face
M2 mode. An explicitly external mapping file uses rows such as
`Arial,400=/external/fonts/arial.ttf` and
`Arial Narrow,700=/external/fonts/arialnb.ttf`. Relative paths resolve against
the mapping file's directory. Mapping registration validates the face's actual
FreeType family/postscript identity, regular/bold style, duplicates and bounded
input; the sole shipped Bauhaus family face retains the original available-face
resolution. Each font owner selects the exact source family/weight key and
loads that face's metrics. Missing entries fail explicitly. Source resources
without an explicit weight use the original normal-weight default.

The Bauhaus available-face choice is an inference from the source resource
requests, the shipped face metadata and Microsoft's
[GDI font-mapping contract](https://learn.microsoft.com/en-us/windows/win32/gdi/font-creation-and-selection),
which chooses a closest available physical font. Its exact Windows raster
result is unexecuted and remains a visual comparison/user gate.

Optional Clang Debug mission-media compilation passes for the mapping path.
A broader read-only inventory now finds external Arial Regular and Bold in
the installed Proton Experimental `files/share/fonts` directory, confirmed
by font metadata. The earlier claim that no Arial exists on this host was too
broad. No Arial Narrow face was found in the checked system, user, Steam,
download/document or mounted font paths. A different narrow family is not an
accepted substitute. Actual strict-family mission rendering and glyph/user
acceptance remain blocked on Narrow and subsequent strict-map execution. No font files or private mapping were copied
into the repository.

Independent automated execution on GCC and Clang Debug optional font/audio
and Clang ASan/UBSan optional font/audio rejects invalid mapping syntax and
weights, duplicate actual FreeType family/style entries and a genuine
mismatched family. The existing shell fixture observes the specific owner
diagnostics. These tests protect strict rejection; they do not exercise
original-family glyph rendering without its external faces.

The source communication animation now also reports native-only counters for
recorded-frame changes and actual source draw iterations through the existing
mission audio statistics. This addresses two identical captures during a
communication without injecting text, input or a replacement animation.
`IAnimate` slots, source timing, frame selection and RNG consumption are
unchanged. Counts alone do not prove animated pixels or correct portraits;
the original full visual/user acceptance remains required.

## Both map owners and alert persistence

Source review identifies the two production owners as `SysMap` (local system)
and `Sector` (system connections). Both remain in the native toolbar/sector
closure. A single visible system at initial TM1 entry is not evidence that
connections are missing: original visibility filters suppress unrevealed
systems and their connections. Later revealed-system drawing, original map
click/command interactions and visible restoration need actual runtime/user
checks.

TM1's original Beacon alert calls `MScript::StartAlertAnim`, which owns its
identifier through `SysMap::CreateMissionAnim`. The original Mission save
callers did not write the matching `SysMapSaveload/SysMap` record. Native
Mission save callers now invoke the production writer. Native `SysMap` writes
the existing fixed-width header and ten-byte alert records, including a
canonical zero-count state. Its loader checks exact extent, reads, system
bounds and identifiers before replacing live alerts; failed loads retain live
state. It preserves original record/prepending order and adds no disk fields.
Original Windows implementations and public interface slots are unchanged.

The native toolbar resource-enable phase now retains restored alerts; teardown
clears them and resets the next identifier. Previously resource creation
cleared a list that Mission had just loaded. The generated shell check exercises
that same extracted lifecycle phase without allocating GPU map textures; it
does not establish visible restored map markers. Saved regular TM1 candidates
require their canonical alert record. Earlier unaccepted intermediate TM1
saves lacking it are explicitly incompatible; original initial maps and the
accepted legacy training scope retain optional absence. Alerts are never
regenerated from objectives.

Optional Clang Debug compilation and `portability.native_shell` execution pass.
The positive phase uses source Save, real `IUTFWriter::Flush`, physical read-only
UTF reopening and original alert restoration/order. The check also executes
actual read-only rejection, next identifier, transactional rejection of
truncated/count/duplicate/system errors, empty replacement, strict missing-state
rejection, optional initial absence, failed-save retention, interface lifecycle
and private archive cleanup. The initially failing physical save-directory
`DeleteFile` owner was fixed; omitted explicit archive Flush in the fixture was
corrected to match Mission's production caller. Actual compatible TM1 save/load
with visible Beacon and both map interactions, independent GCC/sanitizer
validation and affected Windows checks remain open.

## Original initial-map asset closure

Private inspection of the original nested TM1 ObjectList identifies 25
archetype groups and 19 distinct authored model roots, including original
Mantis platforms and Hive/Scout carriers present before later AI production.
This expands the earlier ship-only sample; script-created Frigates alone are
not a complete resource census. Those roots contain 37 secondary-UV arrays
and 20 emissive-map references. Eight platform roots also require a separate
per-pixel alpha plane on every palette mip level. Raw inventories and retail
bytes remain outside the checkout.

These are source/resource inspection facts, not proof that every resource is
loaded or rendered. Full attachment, turret, animation, later production,
weapon and ability closure remains open. Required named texture failures in
legacy material loading must propagate rather than return success with a zero
reference. The selected platform alpha-mip path needs focused execution before
complete presentation can be claimed.

The native palette decoder already forwards separately authored alpha bytes to
the RGBA8 mip upload. Its presence check now requires each existing alpha plane
to have exactly that mip's pixel count; empty, truncated or oversized planes
cannot silently become opaque. Optional Clang compilation passes. Independent
per-level color/alpha and actual blend-image fixtures are pending; source
inspection alone does not certify platform transparency.

Independent optional Clang SDL GPU execution now preserves exact authored
palette colors and alpha 0/96/255 at every one of nine mip levels, and observes
original alpha blending, mutation and exact image restoration. The existing
fixture also rejects present mip alpha planes with empty, truncated and
oversized extents before texture publication. Its initial lookup assertion
was corrected to inspect publication before the original `get_texture_id`
missing-name reservation side effect; source loader rejection itself was not
weakened. These are generated fixtures, not actual platform-image acceptance.

Native texture format selection now distinguishes authored opaque images from
images with an alpha plane while retaining four-byte backend storage. This
prevents `Material` from classifying every opaque palette image as an alpha
pass merely because storage uses RGBA8. Optional Clang compilation passes;
independent material-format/pass regression remains pending.

## Required active effect owners

The resource audit now reads source-derived packed field offsets instead of
searching printable byte runs, which can incorrectly merge a float byte with
an adjacent name. Actual initial ship/platform mesh names, explosions, trails,
extensions and launchers resolve through the original fixed-width fields.
Hardcoded production dependencies also matter: the active low-hull `Damage`
path creates smoke and spark instances, and platform construction creates its
race-specific build effects. These cannot be excluded from the mission by a
record-only traversal.

In the native complete-mission configuration, spaceship/platform archetype
loading now rejects missing active named explosion/trail owners, smoke/spark,
required rendering textures and construction effects. Ship shield animations
are required when an actual shield mesh is loaded. Windows and the accepted
bounded training configuration retain their original branches. The dormant
fire-animation field is not promoted into an active requirement without a
consumer. Optional Clang compilation passes; actual full mission owner loading
and later damage/construction/effect rendering still require runtime evidence.
Source `Damage` initialization enables mesh shields only for `OC_SPACESHIP`;
its active shield-hit branch does not use the uninitialized platform animation
field. No additional platform shield animation is invented from that field.
Ship shield assets and visible hits still need complete runtime acceptance.

## Compound shared-library ownership

A fresh strict TM1 load exposed required unresolved planet and platform maps.
The actual owning compound files store these textures at their outer `Texture
library`; individual child mesh directories do not contain them. Scoped load
logs showed no attempt to load the missing names, so this was not evidence for
an unsupported image format. The original compound viewer explicitly loads the
owning file's texture/texture-animation libraries before creating its Engine
instance. The game MeshManager's CQ2-era path called Engine first, while XMesh
loaded only its child-local library.

Native MeshManager now loads libraries declared by the exact opened owning
model file through the actual GameSystem TextureLibrary before Engine visits
its compound parts. It does not search arbitrary ancestors or substitute
cached textures. Files without an outer library keep their original child
loading path; empty declared libraries retain source semantics. Required named
maps still undergo strict material validation. Windows and shared InitInfo
layouts are unchanged. Complete-mission ship/platform initialization also
requires the real mesh consumed by original extent construction, preventing a
nonempty archetype proxy from proceeding with absent required geometry.

Optional Clang shell compilation and the existing shell lifecycle regression
pass. A fresh lawful TM1 bounded observer now requests all seven previously
unresolved required texture names through the production library. No texture
load failure or required material/component error was reported; the original
mission rendered 132 source frames and closed normally with audio resources
released and idle hints. Communication counters recorded one animation start,
18 frame changes and 22 draws. The earlier failing load remains a failure; this
new execution resolves its specific shared-library blocker. It does not prove
correct visible models, cloud/alpha appearance, complete animation or audible
output. Independent shared-library fixtures, later AI/effect resource closure,
full mission/user acceptance, required fonts, selected movie/selection-portrait
references and affected Windows/ABI execution remain open.

The original SFX cutoff owner detaches a nonlooping sound's logical owner after
its playback cursor crosses the cutoff; it does not stop the buffer's remaining
tail. NativeSfx implements that same detach-and-retain behavior and excludes
loops. Its cursor is derived from actual SDL postmix consumption, not an
invented elapsed wall-clock success. Source review found no newly demonstrated
cutoff-tail defect requiring a replacement. Hardware cursor timing and audible
mission tails remain unverified fidelity, alongside the user's complete audio
acceptance; they are not described as absent implementations.

The existing standalone mesh diagnostic already uses production MeshManager,
texture/material owners and real instance creation/destruction. Moving that
check after genuine GPU binding permits separate resource-owner processes;
it never preloads extra models into a playable mission, where cooking could
change RNG consumption. A strict Seeker diagnostic initially failed because
its verifier treated Engine's compound-only root list as a list of all simple
instances. Actual debugging found a real three-group archetype and instance.
The native verifier now inspects the real owning instance index, checks its
exact simple archetype or original first compound-root-part archetype, destroys
it and checks both index removal and root cleanup. Face-group assertions are
retained. The next process reached the successful owner marker but failed later
because the generic presentation loop had no submitted scene; that whole run
remains a failure. Separate owner-mode teardown and repeated model checks are
pending, with no rendered-pixel acceptance inferred.

A publicly distributed
[original manual on Steam](https://cdn.akamai.steamstatic.com/steam/apps/578670/manuals/Conquest_Frontier_Wars_-_Manual.pdf?t=1484669872)
describes camera relocation and selected-unit movement through the system map,
and switching revealed systems and ordering wormhole travel through the sector
map. These original interactions belong in the user checklist. The manual's
separate opening-movie option does not settle first-campaign intro sequencing.
The lawfully external `cq_intro.mpg` contains MPEG-1 video and stereo MP2 audio;
its presence and the commented campaign call require reference clarification,
not a claim that native video works. An
[older Mission 1 gameplay recording](https://www.youtube.com/watch?v=vlatI3itx5E)
is available as a reference candidate; its frames have not yet been inspected,
so it supplies no visual-parity or selection-portrait acceptance here.

## Later selected media owner guards

Source review confirms active later production paths beyond the initial scene:
Seeker sonar uses the original system-map ping; Khamir's attack uses its original
alias mesh and impact blast; Scarab stasis uses mesh, ring/tendril geometry and
ANIM2D; Spinelayer mines use their regular ANIM2D and blast; Siphon harvesting
renders separate gas/ore tank meshes and its beam/animation. Zorap uses the
original supply ship owner. Dormant mine halo/explosion-animation loading is
not promoted into a requirement merely because names occur in its record.

Native complete-mode factory guards now reject missing cloud textures and its
spray/damage blast owners, required stasis textures/animation, active mine
animation/blast, and harvesting beam/animation/named tank geometry. Failed
attempts release their acquired texture/buffer owners; successful execution
preserves original construction, order and RNG. These changes are frozen for
joint compilation and are not yet runtime evidence.

ANIM2D's original unchecked texture-frame result could expose an uninitialized
backend handle even after a nonzero reserved texture ID. Native loading now
checks the actual frame result and handle before use, validates the source
texture index and exact UV record, and restores/freezes its ownership on a
focused parse failure. Partial construction cannot release a nonexistent
texture array. Texture-name bounds protect the same parse before frame lookup.
Original UV layout, frame ordering, capture timing and Windows branches remain
unchanged. The existing generated parsed-animation fixture executes under GCC and Clang
Debug and their ASan/UBSan configurations: a genuine shared texture frame and
source UV record load successfully; unresolved frames, out-of-range texture
indices, short UV records, unterminated names and the 65-name boundary reject
without a draw, restore the filesystem root and release the partially built
owner. This is synthetic owner evidence, not a rendered retail animation.
The integrated shell build and later resource-owner processes remain pending.

Independent Clang execution of the existing GPU receiver fixture now verifies
both color and alpha multiplication for the selected stasis/cloud two-stage
states, their original WRAP/CLAMP sampling and exact restoration. This extends
the earlier emissive-ADD evidence. It proves the native receiver contract on
generated textures; actual stasis/cloud event draw callers and visible mission
effects remain unexecuted.

## Separate later-model owner execution

The integrated optional Clang shell now completes nine separate lawful resource
processes with successful exit and explicit load/create/destroy completion:
Seeker (`MOcellus.3db`), Khamir (`MZealot.3db`), Scarab (`Msithkar.3db`),
Spinelayer (`MAtok.3db`), Siphon (`MSiphon.3db`), Zorap (`MZorap.3db`),
both Mantis gas/ore tank models, and the initial Hybrid Center
(`MBioForge.cmp`). The compound has ten face groups and exercises the exact
original root-part archetype predicate rather than simple-instance assumptions.
Each process uses GameSystem's actual MeshManager, texture/material owners and
GPU-bound resource receiver, then checks instance removal and root cleanup.
It does not start a mission or preload additional models into gameplay.

The initial nested planet attempt stopped before startup because `--mesh-object`
accepted a single object name. The composition root now accepts checked
relative object paths; the subsequent `planets/earth4_lg.cmp` process exits zero
with the exact load/create/destroy completion marker, completing the tenth
owner check. Rendering remains untested. Earlier failed input, Seeker
verification and presentation runs remain failures. These successful owner
checks do not draw model pixels, exercise later weapon/ability events, or prove
complete visible/hardware presentation. The selected mission's complete
transitive rendering and user acceptance remain open.

## Retail flat ANM layout correction

A strict actual Terran startup reported an ANIM2D directory rejection before
mission loading. The lawfully external selected ANMs store `Texture names`,
`UV Rects` and their texture library directly under `Animation`; they do not
contain the nested `UV Set` used by the generated fixture. Original Windows
loading leaves its directory unchanged when that optional directory entry
fails. The initial native guard incorrectly rejected this valid retail shape.

Native loading now explicitly supports both original flat and nested forms.
The flat form must contain both real frame/name records; only an entered nested
directory is restored on scope exit. Exact record extents, name bounds, texture
indices and actual backend frame checks remain required. Diagnostics identify
the animation source. This correction executes in the existing material-manager and shell fixtures
on GCC and Clang, each with default and optional font/audio builds, and both
optional ASan/UBSan builds. The four focused CTests are
`portability.game_material_manager`, its `_cr` and `_missing_end` variants,
and `portability.native_shell`; each configuration passes all four without
skips or sanitizer reports. Nested and flat positives resolve genuine frames
and exact UVs; all earlier malformed cases and the missing flat record reject,
restore the root and destroy the owner. The earlier actual failed mission run
remains a failure, and the independently observed mission-load failure is
still under diagnosis. These generated checks are not retail visual acceptance.

## Remaining selected-mission acceptance work

The unaccepted visible/audible movie path and required Arial Narrow input are
concrete remaining integration boundaries. Native video-enabled Terran entry with original movie-seen and mission-seen
counts both zero now restores the
`cq_intro.mpg` MovieScreen request at its original source position, before the
existing movie-seen update. Script01 and briefing contain no separate movie
call. The Windows source branch remains unchanged. Actual retail intro
sequencing and presentation still need a reference and user acceptance. The external MPEG exists, but neither source silence nor
file presence proves intended retail sequencing. A public Mission 1 reference
excerpt could not be inspected because its media request returned HTTP 403.
No substitute movie, portrait or font is accepted.

The remaining rendering work is primarily an execution/closure gap, not proof
of another absent renderer: later models and tank owners load, but their
animation/attachments, actual mine/stasis/cloud/ram/sonar events, shield hits,
weapon beams/projectiles, damage, explosions, construction and destroyed-mesh
fragments must execute with original material/light/mip/alpha states and remain
correct after combat/production reload. Existing generated split/UV2 and
two-stage arithmetic fixtures protect their receiver contracts only.

Communication portraits have bounded visible/source advancement evidence;
UnitComm's selection-sound path calls `PlayCommMessage` without an animation
argument. A retail reference must establish the intended selected-unit
portrait owner/switching/clearing before adding another animation route. Both
minimaps have production owners and persistence checks; initial frames do not
prove revealed system connections, switching systems or original map travel
orders, nor nonzero mission-alert restoration. These require original normal
controls and subsequent reload. Complete speech/music/spatial output, subtitle
and UI appearance, all later media, mission victory/defeat and user acceptance
remain open. No asset load or synthetic fixture is a substitute for those
visible and audible results.

## Genuine video owner integration in progress

The source profile requests `IVideoSystem` unconditionally through
`SysContainer::LoadSystemComponents`; only the explicit movie-exclusion flag
bypasses it. Removing that entry to make startup pass would not resolve the
owner. Original `MovieScreen` loads/starts a movie, updates it on `CQE_UPDATE`,
gets its texture/dimensions/UV extent for `DA::DrawTexture`, and releases it on
completion or Escape. The snapshot video implementation accepts only Bink and
casts pointers into 32-bit handles; it cannot supply the external MPEG intro.

A new native aggregate preserves the existing video interface and uses a typed
32-bit handle table. It decodes genuine bounded MPEG-1/MP2 through a separate
reviewed shared-LGPL FFmpeg configuration, converts decoded frames with
libswscale and uploads through the existing pipeline texture owner. SDL owns
actual decoded audio. Frame counts are obtained by decoding, not inferred as a
successful estimate. Audio-queue timing, device-fragment latency and the
monotonic drained-audio tail govern streamed video updates; pause, source
one-based seek, looping, EOF and release retain real decoder/resource owners.
Missing resources/codecs and failed uploads do not produce a successful handle.

The separate external dependency enables MPEG program/elementary demuxing,
MPEG-1 video, MP2 audio and swscale alongside the existing WAV/IAC configuration.
The original audio prefix is unchanged. No FFmpeg source, external movie or
legacy Bink binary is included here. Initial standalone Clang compilation
passes; integrated compilation, generated A/V owner/lifetime/texture/timing
checks and actual audible/visible output remain pending. The existing shell
fixture needs this extension because its movie exclusion never exercised the
aggregate, decoder, texture updates or audio clock. Successful checks would
permit genuine no-exclusion startup; they would not settle original Terran
intro sequencing or constitute full movie/media acceptance.

The new provider also requires genuine MPEG video/audio parsers at aggregate
initialization. The upstream MPEG program demuxer marks PES streams for full
parsing; the first external configuration lacked those parsers and is not
accepted as ready. Required parser availability is checked alongside decoder,
demuxer and library-license availability. The source SDL audio API defines
available output as a resampled view of queued input; the clock counts input
bytes once and uses available output only for the final drain predicate. This
source correction still needs executable timing/EOF evidence.

The optional video-enabled Clang shell builds, and a bounded lawful startup
reaches genuine movie-directory/pipeline initialization with no movie exclusion.
Its overall process still fails on the independent saved-global owner boundary;
this establishes aggregate initialization only. No movie has yet been loaded,
rendered or heard through this new owner. The earlier setup failure is retained.

## Compound geometry guard correction

A subsequent actual first-Terran run exposed another native guard error: valid
platform/ship compounds were rejected because the guard queried only their
outer Engine archetype's own renderer mesh. Original compounds can have a
hardpoint-only outer/root with visible child meshes. The real Hybrid Center
separate-owner check already demonstrated ten original face groups, disproving
that predicate. Native ship/platform complete-mode guards now require a valid
MeshManager archetype and its original full-tree `GetNumFaceGroups()` result.
The manager realizes that geometry before returning the archetype. Missing
models, all-locator geometry and failed declared compound parts remain rejected;
no child load failure or assertion is bypassed. Windows, persistent formats,
initialization order and RNG are unchanged. Source is frozen for rebuilding;
actual original factory admission and save/reload require the next bounded run.

The original MovieScreen caller now has a native compile path, retaining its
source viewer, modal event loop, frame-limit changes, music muting and cursor
lifecycle. Missing video/source UI data or an unavailable frame fails explicitly;
scoped native cleanup releases the movie and restores UI/audio state on failure.
Windows behavior remains unchanged. Its missing native `DA::DrawTexture` owner
now follows the original unlocked textured-quad states, UV order and pane rules
through the existing checked 2D draw boundary. Both sources compile standalone
under Clang; integrated caller execution and visible output remain pending.

A native-only read-only diagnostic reports real decoded sample count, rate,
channels, peak/RMS and frame count from the genuine provider. It adds no legacy
interface slots. This enables the existing generated A/V fixture to falsify
actual audio decoding rather than infer a tone from successful playback setup.
One-based seeking uses the requested decoded frame and its actual timestamp,
including audio/video start offsets, instead of assuming frame number divided
by FPS identifies the same frame. Pixel identity, timing and audible acceptance
still require execution.

Initial generated video-owner execution rejects missing, corrupt, truncated,
audio-less and video-less inputs, but the positive fixture fails at the final
MPEG-1 picture's absent timestamp. Independent decoding of the same mathematical
25-frame stream confirms that the final delayed picture legally has no PTS/DTS.
The owner now preserves known timestamp offsets/order and reconstructs an
untimestamped decoded picture only from an established timeline and positive
decoded duration/sequence cadence. This is a format correction, not a successful
placeholder frame. Rewind resets that timeline; executable retry remains pending.

Video teardown now queries both supported texture bindings, unbinds only its
own texture and checks actual destruction. Other bindings are preserved.
Captured-frame GPU references still need the original caller's actual close
execution; a CPU fixture alone cannot prove that backend lifetime.

The genuine video-enabled Clang owner fixture now passes with the mathematical
25-frame MPEG/MP2 input: exact red/blue pixel and one-based seek identity, actual
decoded PCM rate/channels/peak/RMS, SDL dummy-device clock/pause/EOF/loop, invalid
inputs/stale handles and both-stage owned-only unbinding execute. This resolves
the earlier positive final-timestamp failure; it does not show GPU frames or
prove audible output. GCC and sanitizer execution remain pending at this entry.

## User presentation checklist for the complete candidate

Use the final handoff configuration without content-reducing flags and with the
strict original-family font map. Record problems with mission stage, unit,
command/effect and whether a reload preceded them; keep captures/saves external.

- Start through fresh Terran campaign entry (original seen counts zero), observe the intro
  and its audio/letterboxing, then original selection and briefing. Check
  portrait motion, subtitles, completion and normal dismissal.
- Select Blackwell and other units, switch/deselect, issue commands and verify
  intended portrait/highlight clearing. Communication source advancement is
  already observed; selected-unit portrait fidelity still needs reference.
- Inspect initial planets/platforms/gates/ships and later construction/produced
  units, including visible submodels, turrets, animations and tank attachments.
  Check texture detail, lighting/emission, transparency and backgrounds.
- During original combat inspect beams/projectiles, shield hits, trails,
  particles, low-hull smoke/sparks, explosions and broken geometry. Observe
  later original AI units/abilities when naturally reached; model-owner loading
  alone does not prove mine, sonar, ram, mimic, stasis or cloud presentation.
- Use both maps through normal controls: relocate the local camera and order
  movement, reveal and switch systems, inspect connections and order wormhole
  travel. Check original mission pings/alerts without automatically revealing
  anything. Initial map frames do not establish these interactions.
- Save/reload early, during commands/production and in combat; repeat the above
  appearance, portrait switching, map/alert, music and spatial-sound checks.
  Source/synthetic persistence evidence does not replace correct resumed play.
- Complete original victory and regular defeat tests, then quit normally.
  Check audible speech/music/cues/effects, relative volume and positional
  sound throughout. Full user results and media acceptance remain open.

The first actual original MovieScreen GPU attempt fails before drawing because
its real movie-file open is rejected. The exact external `cq_intro.mpg` exists;
its size exceeds the native physical filesystem's generic 64 MiB read bound.
The movie decoder's own input bound is larger, so this is a physical I/O owner
boundary, not a missing/case-mismatched asset. No substitute clip or data import
is proposed. Bounded movie-file I/O and a subsequent actual caller retry remain
required; no GPU, visible or audible movie result is claimed.

After scoped physical movie-file I/O support, the actual original MovieScreen
caller renders the external intro with letterboxing on SDL/Vulkan. A private
owned-window capture shows the clip. Normal Escape then aborts because the
backend rejects destruction of its last captured texture. The process remains
an actual failure, not a successful movie smoke. This confirms the previously
identified captured-frame lifetime boundary; it does not justify ignoring a
destroy result. Backend ownership retirement, successful original close/EOF,
audible synchronization and full intro/user acceptance remain open.

After backend captured-texture retirement, the actual original MovieScreen
retry shows the external intro for a bounded interval and exits zero after
normal Escape, returning through the source modal caller without release errors.
A later capture-helper attempt against the already-closed window fails separately;
it does not change the tracked game process result. Full natural EOF, audible
output/synchronization, original-reference comparison and user acceptance remain
pending. The previous release-abort run remains a failure.

Faint desktop text in black bands needs host-policy separation: the SDL window
requests no transparency, and the native primary target clears/preserves opaque
alpha. The active host's general Omarchy rule applies partial opacity to windows
with its default tag despite global opacity options being one. A live owned-window
rule check and opaque GPU readback can distinguish compositor policy from a
rendering defect. No persistent desktop setting or production behavior has been
changed merely to hide this observation.

Read-only inspection during the tracked natural-EOF run confirms the movie's
own Wayland window carries the host's default-opacity tag and its actual
compositor opacity is 0.985. This establishes an applied host rule, rather than
inferring opacity from a screenshot alone. No window property or persistent
configuration was changed by that inspection. Raw opaque target verification
and a scoped test-window comparison remain separate from full visual acceptance.

The complete external intro now reaches natural EOF through original MovieScreen:
the tracked process exits zero after about 185 seconds, including approximately
six seconds of initial decoding/validation and the original 179-second playback.
No Escape or termination is used. This proves the original caller's bounded
full-duration decode/timing/EOF/teardown path on this host; it does not establish
user-confirmed hearing, A/V fidelity or a complete mission result. Initial
whole-clip validation can briefly delay the menu before playback.

The subsequent test-window opacity commands return success, but their effective
properties were not rechecked before the window closed. A faint-band capture
therefore remains insufficient to claim the override worked or to identify a
new renderer defect. The earlier read-only applied-opacity measurement remains
valid for that earlier window. No persistent desktop configuration was edited.

A later coordinated scoped-opacity comparison does not execute: the installed
compositor rejects the standalone property-change request with `unknown request`
while its CLI exit status alone is not sufficient to detect that rejection.
Read-only rechecks confirm the owned window retains opacity 0.985/0.96 and both
override flags false. No property or configuration change occurs, and only the
before capture exists. This is a failed host comparison, not a renderer result;
the verified applied compositor policy remains separate from engine acceptance.

### Gray planet investigation (2026-10-14, open)

A genuine post-encounter, complete-content campaign run shows Tau Ceti Prime as
a smooth gray gradient. This is an open presentation blocker. Source review of
`Planetoid::Render` finds no intended fog-based gray-material override: visibility
gates the draw, then the original ambient-light owner and Engine/PolyMesh path
render the authored model.

A separate VIDEO-on Clang process on lawful external data loads the genuine
current campaign checkpoint with normal F8 and closes normally. Read-only GDB
observations reach the original planet draw and identify `Planets\\earth1_LG.cmp`.
Both Earth and cloud material frames have genuine nonzero bound texture handles;
textures and emissive textures are enabled, and `RP_TEXTURE` permits texturing.
Stage zero uses source color and alpha MODULATE(TEXTURE, DIFFUSE), coordinate set
zero, with varying original UV coordinates. Stage one is disabled. The Earth
material disables blending; the cloud material retains the authored alpha flag
and enables SRCALPHA/INVSRCALPHA blending. These observations rule out the tested
missing-frame, disabled-texture, degenerate-UV and missing-cloud-blend hypotheses
at that production boundary. They do not establish correct backend sampling.

Independent private inspection confirms the authored base Earth image contains
many distinct colors and the cloud alpha plane contains transparent and partial
alpha values. The final Earth mip is a nearly gray single texel. Incorrect mip
selection or a later native backend submission/upload defect therefore remains
a falsifiable hypothesis, not a diagnosed cause. The final observer screenshot
has the planet outside the restored camera view and is not a before/after pixel
comparison. All captures, resource statistics and detailed runtime state remain
outside the checkout; no replacement material or gameplay preload was added.

Independent source review subsequently identifies a concrete launch-composition
error: `Tools/ci/playtest.py` supplies the specular game (`_mt.vert`) and extended
(`_t4.vert`) shader arguments in the opposite order to NativeShell's existing
`VERTEX_SPV FRAGMENT_SPV EXTENDED_SPV GAME_SPV` contract. The extended vertex
format supplies UVs at location two; the game MTVERTEX format supplies specular
color at location two and UVs at location three. The incorrect game shader
therefore reads zero specular components as UVs despite the correctly populated
original vertex data. This explains a constant texture sample multiplied by the
observed lighting gradient without any missing retail texture or substituted
material. The required argument order is `.vert`, `.frag`, `_t4.vert`, `_mt.vert`.
The shared command helper and its regression are owned by validation; an actual
corrected-order campaign pixel comparison remains required before this blocker
can be marked resolved. Earlier owner-only model-load checks still prove their
load/create/destroy contracts, but do not establish presentation with the old
shader order.

The root agent's actual corrected-order campaign retry resolves the gray-planet
regression for this bounded caller: changing only the two launch shader arguments,
then using normal F8 and the original system-map camera interaction, makes the
Earth surface display blue/brown continents and cloud structure; nearby HQ
textures are visible as well. The genuine source loop executes 179 frames and
returns through QUIT4 with native process exit zero, no remaining mission audio
streams and idle hint ownership. The private before/after captures remain outside
the checkout. No backend, binary, material or asset replacement was needed.
This closes the observed gray-planet command-composition defect, while complete
mission rendering, audible output and user visual acceptance remain pending.

### Ship exhaust and cargo attachment alignment (2026-10-14)

The user reports exhaust above the hull and rotated Harvester cargo tanks.
Original source and lawful resource inspection identify a concrete presentation
space mismatch. The direct Engine hull branch applies its existing model-space
rotations (90 degrees about i, then 180 degrees about j), while glow placement
and cargo drawing previously used the uncorrected ship transform. The selected
Harvester's four authored cargo attachments lie along model Z, with near-identity
attachment orientations; its glow points also lie mainly along model Z. Neither
an invented offset nor a replacement asset is needed.

The native-only `NativeShipVisualTransform.h` helper retains those exact original
rotations. `ObjectGlow::glowPostRender` uses the corresponding presentation
transform for the glow origin and direction. Only the four cargo drawing branches
in `HarvestShip::renderHarvestShip` use it for tank geometry. Their predicate
matches the original direct Engine hull path; the MeshInstance fast path retains
its original transform. Windows bodies, simulation transforms, stored hardpoints,
weapon/harvest beam endpoints, recovery physics, persistent records and RNG calls
are unchanged. Root integration uses the same correction in the hull caller.

Production-command Clang VIDEO-on syntax checks for HarvestShip and SpaceShip
succeed, including their actual ObjectGlow template instantiations. A focused
existing regression must still execute the transform/attachment contract, and
normal visible exhaust and cargo behavior after save/load still require actual
runtime and user checks. EngineTrail projectile consumers are not rotated by this
ship-only repair. Original beam and glow callers submit world-space vertices after
`Camera::SetModelView`; their perspective-state restoration is a separate owning
Camera/batcher contract, not a reason to alter simulation endpoints.

The existing game material/math fixture now executes the shared correction under
a nonidentity world transform, checks the exact original rotation basis and
attachment composition, verifies tank-local Z follows the corrected hull axis,
and confirms the input world transform and disabled direct-hull branch are
unchanged. All three Clang material variants pass; broader GCC/sanitizer results
must be recorded separately when executed. This is a mathematical production
helper contract, not a pixel reference comparison.

Read-only review of a subsequent private genuine campaign capture shows two blue
Fabricator glows at the visible ends of its two thruster tubes, rather than above
the hull. Another private capture shows a selected Harvester with original HUD
Ore 250/250, Gas 0 and visible cargo components whose projected long axis follows
the ship's long axis. The cargo row appears below the hull in that view; exact
attachment placement still needs a suitable original reference or user assessment.
This gameplay interval includes partly uncontrolled user input and is not a
controlled construction/harvest recipe pass. These bounded observations establish
visible glow and full-cargo geometry, not complete effect, save/load or mission
presentation acceptance. No independent UI input or compositor change was made
by the media review.

### Selected weapon and free-standing platform callers (2026-10-14)

Read-only typed lawful archetype inspection identifies the first Terran Corvette
and Blackwell Corvette weapons as original WPN_BOLT geometry (`Bolt2.3db`) through
`Projectile.cpp`'s `Bolt::Render`. The Mantis Frigate's two launchers request the
original PlasmaBolt owner (`bolt8.3db` plus its configured billboard/effect path).
Beam/GattlingBeam observers therefore do not characterize those first-mission
weapons. The preserved accepted training checkpoint instead contains original
Solarian Taos units; their Scout turret requests GattlingBeam (`band2.tga` and
its Spark contact effect). These are source/resource classifications, not proof
of actual weapon rendering, correct endpoints or combat outcomes.

The initial Terran mission's allowed free-standing platform is the laser Turret:
its original `tlaser.cmp` record requires zero planet slots. The Sensor Tower
requires one slot; Outpost requires two. Other Terran zero-slot records exist but
are not initially allowed by this mission's source tech mask. Original build
button activation separately uses the authored button `techDependency`, while
platform `techActive` controls mission-allowed visibility/provided tech. The latter
must not be mistaken for minimum construction prerequisites. The exact embedded
button dependency and visible free-placement behavior still need their owning
caller checks; no tech bypass, automatic placement or fabricated green preview
was added by this inventory.

### Scoped construction-preview material repair (2026-10-14)

Source review confirms `UnbornMeshList::RenderMeshAt` already supplies original
valid/invalid green/red diffuse and emissive multipliers with alpha 128 to the
per-instance face-group material. The bounded flag-off native legacy fallback
instead uses its old texture-factor/VB branch, which does not consume those
per-instance colors and alpha. Correct assets and preview placement alone cannot
prove the requested preview color.

The native-only `TreeRenderNativeLegacyPreview(MeshChain&)` entry scopes only
this synchronous draw through the existing genuine nonchrome legacy material
submission. Its private RAII state restores the previous preview scope after
normal return or an exception, without enabling the mission-wide full-material
flag. Original cooking, geometry, source face order, RNG and the Windows caller
remain unchanged. The unused CQ2 constant mirror is skipped only for a wholly
nonchrome legacy mesh in this preview scope. Existing chrome and CQ2 paths are
unchanged: lawful Solarian Hydrofoil resources really contain chrome environment
materials, so the repair does not substitute a nonchrome material or imply that
those presentation paths are complete.

Production-command Clang VIDEO-on syntax checks for MeshRender and UnbornMeshList
pass. The existing material fixture is being extended to execute exact green/red
and alpha through this scoped caller with the mission-wide legacy flag disabled,
and to reject a draw while restoring the ordinary caller's scope. Actual preview
color, user reference fidelity and broader compiler/sanitizer execution remain
pending. Separate inspection finds UnbornMeshList reuses a stored render-owner
array against its new runtime child count; an actual mismatch has not yet been
observed, so no speculative array or shadow-owner correction was included.

### Fabricator construction-wing animation (2026-10-14)

The user's missing opening wings have a concrete original-caller integration
cause. `SPACESHIP_INIT::animArchetype` is initialized to -1 and the current
MeshManager-based spaceship loader never assigns that obsolete field.
`TFabricator::initFabricator` consequently requests `Sc_build` through an invalid
script set. The original `tfabricator.cmp` contains four genuine `Sc_build` joint
channels, and MeshManager already creates and owns their real script set.

Native initialization now borrows the pointer-sized script-set handle directly
from `meshArch->GetAnimArchtype` for the original script-instance requests. It
does not store a pointer in the S32 init field, alter that structure or release
the borrowed archive. Production animation instances retain their own archive
references and the existing Fabricator destructor releases them. Missing required
`Sc_build` on the selected full-mode Terran spaceship Fabricator fails explicitly.
The authored Terran resource has no `Sc_construct`; that original optional loop
remains absent rather than becoming a fabricated animation.

Original start, reverse/stop and animation-update hooks remain intact. The
existing `resolveFab` already starts `Sc_build` at its END pose when restoring an
active buildee, so no new saved field or reload-progress rule was added. Windows
initialization, simulation order, RNG consumption, persistent layout and original
construction state are unchanged. The production-command Clang VIDEO-on
Fabricator syntax check passes. Actual script admission/child motion, visible
wing opening/closing and restoration during active construction still need
execution after the integrated rebuild and user confirmation.

### User feedback and pending follow-up (2026-10-14)

The user reports that building construction now looks very good. This is actual
user feedback for that observed presentation scope, not acceptance of the full
mission. The user still reports incorrect weapons and missing Fabricator wing
opening; the wing script-owner repair remains awaiting visible execution. The
scoped green/red placement-preview repair has source/compile evidence only until
its regression and real caller execute.

The root agent's bounded uninstrumented frame sample records approximately 600
source frames over 60 seconds (about 10 frames/second) for that candidate/scene.
It is a performance observation, not a performance fix or a complete benchmark.
Independent source review finds an unconditional GPU-idle wait before each native
captured submission even though each draw retains fresh upload resources until
BeginFrame's existing synchronization and retirement. The owning renderer change
must remain limited to the captured path and preserve noncapture replacement and
frame/shutdown synchronization. SDL's documented [submission ordering](https://wiki.libsdl.org/SDL3/SDL_SubmitGPUCommandBuffer)
and [upload timeline](https://wiki.libsdl.org/SDL3/SDL_UploadToGPUBuffer) support
that lifetime review. Actual multi-draw/retirement checks and a comparable real
scene measurement remain required before claiming safety or improved frame rate.

Read-only review distinguishes the training Gattling endpoint path from the
Terran Bolt/Plasma pitch issue. Original `GattlingBeam::InitWeapon` takes the
explicit or target world position as its goal; launch orientation supplies the
start position, not a pitch-derived velocity. PhysicalUpdate reconstructs that
start from the original owner-relative offset and computes world endpoints from
the authored sweep, range, collision and RNG logic. Render builds a camera-facing
world-space quad from the retained endpoints after `Camera::SetModelView`.
There is no source basis to apply the Bolt/Plasma pitch repair to this caller or
alter its original range, sweep, collision result or RNG consumption. Actual
current-binary training endpoints, projection and visible contact behavior remain
unverified until that caller executes; an old user report does not prove the
latest projection/caller state remains broken.

The first scoped-preview fixture attempt fails because its original direct-mesh
setup supplies only `MAINCAM`; the genuine TreeRender caller also requires the
App `IBaseCamera` binding used by SetCommonStates. This is a missing test dependency,
not an observed game crash or a reason to weaken the production camera contract.
The fixture must bind a correctly typed camera receiver with a real position and
explicit failure for unsupported operations; no unsafe interface cast or source
null fallback is introduced. The preview regression remains pending that setup.

### Background asteroid distance-key overflow (2026-10-14)

A genuine active-construction F8 sanitizer run completes mission loading, then
halts on float-to-U32 overflow in `AsteroidField::RenderBackground`: the squared
camera distance is approximately 5.17852e10. This is a real failed run, not a
successful reload/close or leak result. The field uses that conversion only as
an ascending visual RadixSort key.

The native correction retains the existing U32 sort array, production RadixSort
and returned index list. A checked `NativeAsteroidDistanceKey` validates finite
nonnegative IEEE binary32 input, floors it to retain the original defined integer
conversion's ties, normalizes signed zero, and copies the positive float bits
into the sort key. Positive binary32 bit ordering is monotonic, extending the
same stable distance ordering beyond the old U32 conversion range without
saturation, integer overflow or new per-frame sort buffers. Windows conversion,
serialized fields, simulation updates and RNG are unchanged.

The immediate private Clang ASan/UBSan/float-cast-overflow check executes the
observed large distance, maximum finite float, integer-floor ties, signed zero
and explicit negative rejection without a sanitizer report. Production-command
Clang cloud syntax passes. The existing material/math fixture is being extended
to compare the genuine original RadixSort order for bounded keys against the new
keys and to exercise large/invalid distances. The actual strict campaign F8
sanitizer retry and full rendering acceptance remain pending.

A later genuine active-construction reload reaches original `resolveFab` with a
valid Terran Fabricator animation instance and buildee, and executes its existing
FORWARD/END restart. Read-only private capture review shows extended Fabricator
wings beside the HQ. This establishes the repaired script admission and a visible
open pose in that interval; it does not yet establish a controlled complete
opening/closing sequence, all reload timings or full user presentation acceptance.

### Planet hover and construction-slot visibility (2026-10-15)

User feedback identifies missing planet transparency under the mouse, especially
when building. Native Planetoid now uses its original `bMouseOver` condition in
normal play and construction, and the original faded-pass contract forwards
per-instance alpha to PolyMesh. It does not modify shared materials, saves,
simulation or Windows. The selected64/255 opacity is provisional: neither an
exact retail opacity value nor its historical interpolation curve is established.

**Bounded retail runtime:** the strict full-media Clang22.1.8 Debug candidate
loads a separate, unchanged copy of a genuine ready-HQ TM1 save. Actual compositor
pointer events show a faded planet on hover and opaque restoration on leaving;
private captures retain ships, background, HUD and both maps. Normal F7/F8
followed by renewed pointer input restores the hovered appearance; regular close
returns result4/exit0 after4285 frames with zero mission-audio streams, idle hints
and active portrait playback. No objective/progress value was modified. A first input
attempt only warped the pointer and an over-eager focus loop recentered it; those
images are not pointer-leave evidence. A compositor responsiveness notice during
synchronous load clears when loading completes; no permanent hang is inferred.

Existing material CPU/host-Vulkan tests pass under GCC16.2.1 and Clang22.1.8;
fatal focused Clang ASan/UBSan also passes, without suppressions. See
[the exact owner and regression scope](GAME_MATERIAL_PATH.md#native-planet-hover-opacity-2026-10-15).
Construction-slot visibility/clickability, exact fade appearance and full TM2
visual acceptance remain user checks. This does not close P1c, all-unit admission,
whole-process leak, native Windows/ABI, actual CI or M3 gates.

### HUD actions and Capture cursor feedback (2026-10-15)

The user clarifies capture works on click, while the old forbidden cursor remains.
The native cursor table omitted original Capture.cur135; embedding now preserves
its actual glyph and hotspot(3,0), with exact XOR/AND bitmap extents. Existing
source decoder and actual GCC/Clang host-SDL cursor checks pass. Capture and
research rules are unchanged; renewed in-game target feedback remains a user check.

Exact retail semantic/command mappings also restore Objectives, Repair, Sell
and Assault buttons, their hints and command events. Clang genuine latest-TM2
checkpoint runtime opens Objectives by real mouse click before and after normal
F7/F8, displaying six original goal/completion rows. Normal close returns exit0,
audio0/idle hints with active portraits. Repair/sell final execution remains a
user check. Both maps, background, models, HUD, fonts and media stay enabled.
GCC/Clang additionally start original TM3 briefing/map, F7/F8 and close; this is
bounded entry evidence, not full TM3 audiovisual or mission acceptance. Details,
failed early attempts and remaining Windows/ABI/CI/P1c gates are in
[the campaign ledger](NATIVE_TERRAN_CAMPAIGN.md#tm2-completion-hud-commands-capture-cursor-and-tm3-entry-2026-10-15).

### Reproduced cross-mission effect texture artifacts (2026-10-15)

The user's TM3 screenshot and new genuine F7 checkpoint narrow the persistent
effect report. A fresh source TM3 intro reproduces a rectangular hull texture
on a transient effect; the App batcher's stale texture cache after direct
Engine/Optics draws is independently reproduced and repaired. Original textures
and effects stay enabled. Before/after intro captures and the existing real
batcher regression support this exact fix, not blanket acceptance of every
impact/explosion artifact. See
[owner and validation evidence](GAME_MATERIAL_PATH.md#effect-textures-after-direct-ship-draws-2026-10-15).

The user confirms that the texture artifacts are gone in the corrected build,
but fighters remain invisible and Hive Carriers appear to deal no damage.
The earlier 40-vertex/60-index CPU batch observation did not prove GPU output
or correct fighter gameplay. A separate read-only debugger run with the genuine
TM3 checkpoint now reproduces launched Scout fighters descending from Z20 to
below Z-53000. Sampled weapon attempts return without firing and retain supplies.
The fighter/control frame mismatch is under repair; Hive Carrier damage and
visible fighter acceptance remain open. External saves, captures and fonts stay
outside source control and CI.

### Fighter flight-frame regression (2026-10-15)

**Reproduced retail-input runtime:** the unchanged genuine TM3 battle checkpoint
launches Scout fighters, but the old native build drives them from Z20 below
Z-53000. Sampled firing attempts fail before damage is scheduled. The original
fighter uses -K for launch, propulsion, formations, collisions and its model;
shared control reads J-based navigation angles and applies roll about J.

The native exact `FIGHTER_SAVELOAD` control specialization now selects the
existing z-based angles and K roll axis. Fighter steering and target-facing
checks use the same frame. Authored launch poses, model transforms, acceleration,
timing, weapon conditions, random calls and persistent layouts are retained.
Ordinary ship control and Windows expressions retain their existing behavior.
No fighter or weapon is fabricated or substituted.

**Automated synthetic execution:** the existing material-manager probe already
executes production TRANSFORM/quaternion contracts for native weapon frames.
Its extension protects this concrete fighter regression in four heading
quadrants, signed elevation/bank, physical pitch/roll rotation signs and the
unchanged ordinary-ship selector. Restoring the previous J selector in an
external temporary header makes that same check fail; the corrected selector
passes. This checks the frame contract, not the complete ObjectControl loop.
No new standalone probe or fake simulation owner is introduced.

**Bounded retail-input runtime:** the corrected Clang full-media build loads
the same unmodified checkpoint. Observed fighters remain near the battle plane,
consume supplies during attacks and return; actual 40-vertex/60-index fighter
geometry reaches the GPU with positive clip W and projected in-viewport bounds.
Normal close releases mission audio streams and leaves hint ownership idle.
The corrected GCC full-media build also loads the user's newer unchanged TM3
checkpoint. A genuine Hive Carrier launches fighters and delivers eight original
five-point damage events to a Terran Missile Cruiser: its actual hull decreases
from 150 to 110 through EventScheduler and Gunboat damage application. No
observer error occurs in that GCC combat run.

A second corrected Clang run executes F8 into the original battle, then F7 and
two further F8 loads within the same process, and closes normally after 2261
source frames with audio streams zero and hint ownership idle. This bounded
cycle does not establish resource-growth or result-transition acceptance.
Four synchronized successful Present captures are tied to the actual fighter
GPU submissions and verified owned window. Correct SDL NDC-to-window Y and
physical/logical scaling place one fighter behind the HUD and another partly
behind a large hull; the selected frames do not establish identifiable pixels.
An auxiliary damage observer in this Clang capture run cannot resolve one DWARF
member and disables that observer; the independent GCC damage proof above is
unaffected. Renewed user visual/combat confirmation remains pending.

**Technical validation:** Linux x86_64 GCC16.2.1 and Clang22.1.8 full-media graphs
build (`p1c-fighters-clang-20261015` and `frontierx-p1c-gcc`, with FreeType and
shared-LGPL FFmpeg audio/video enabled); their existing non-host suites pass.
Execute with `cmake --build BUILD`
and `ctest --test-dir BUILD -LE host --output-on-failure`. The affected existing
`portability.game_material_manager`, `portability.view_parser`,
`portability.layout` and `portability.native_shell` checks pass in the fatal
Clang ASan/UBSan graph with `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. These synthetic/shell checks
are distinct from complete-mission sanitizer evidence. Native Windows
Debug/Final/module ABI, actual hosted CI and full M3 comparison remain unexecuted.

The subsequent **plain fatal sanitizer retail-input run** uses the same genuine
Scout checkpoint with full media, first F8, ordinary F7 and second F8. It reaches
1573 source frames and normal mission cleanup with audio streams zero and hints
idle, without an invalid-access or UBSan report. Whole-process LeakSanitizer
still fails at exit: **3604 bytes in ten allocations**, exit1. Of these, 3284
bytes/six allocations have DBus allocation stacks, including SDL/PipeWire device
startup; 320 bytes/four allocations end in an unloaded unknown module and remain
unattributed. This is a failed leak check, not a clean sanitizer mission pass or
proof that mission resources do not grow. Earlier 3316/3396-byte findings remain
separate; no suppression, disabled leak detection or weakened assertion is used.

### HUD command identity after ship/building selection (2026-10-15)

The broader Sell/Repair/F8 report exposes incorrect bindings in additional HUD
resources, including unlabelled and mutable action buttons. Native HotButton
and MultiHotButton now translate the complete reviewed retail command enum at
initialization. Displayed shortcut hints, flashing and clicks share one stored
current command; tooltip changes do not change ability slot identity. Physical
F8 remains the original Quickload event. The external audit covers all 1039
button fields across the three retail races; unit capabilities and original
repair/selling rules are preserved. See
[the command owner, regression and runtime ledger](NATIVE_TERRAN_CAMPAIGN.md#hud-command-bindings-across-all-ship-and-structure-panels-2026-10-15).
The user confirms the previously failing Infiltrator action works; the real
Clang runtime emits canonical generic-ability events without Quickload. Other
ship/building actions remain pending gameplay confirmation.
