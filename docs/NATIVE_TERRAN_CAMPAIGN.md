# First Terran campaign integration (2026-10-13)

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
The brief capture retry has not reproduced the colorful artifacts; their cause
and visible acceptance remain open. Complete P1b acceptance is not declared.


Scope: original campaign selection, briefing, first mission load, initial units,
source simulation and normal close. Full completion, Save/Load and all intended presentation/media acceptance are P1b;
next-mission transition and comprehensive repeated lifecycles are P1c. No objective completion, replacement mission or
synthetic success is added. User gameplay acceptance remains pending.

## Original owner path (source review)

Recovery note, 2026-10-14: source changes survive the reported host crash/reboot,
but earlier private temporary mission checkpoints do not. Rebuilt durable
external candidates retain all media requests and the explicit interim
single-font limitation. Genuine mission progression and new F7 saves must be
recreated through the original controls. Historical HQ/combat observations below
are not post-reboot execution or full P1b acceptance.

Fresh original controls now recreate unique Blackwell selection/movement,
Seattle/Tacoma arrival and normal three-ship selection, Gate2 proximity,
Austin/three-Frigate jumps and combat, Fabricator arrival and HQ construction.
Genuine F7 output retains an actual524-byte active-HQ record with hull2167/3500
and a separate completedHQ3500/3500 state. The later completed-HQ F8/F7/close
executes12753 source frames and returns4/native0, with audio0/idle hints and
advancing original portraits. It is not load-during-active-production evidence.
A further rebuilt BUILD-cursor ready-HQ F8/Turret preview/close executes2035
frames and returns0. Strict active-HQ sanitizer continuation is recorded below.
Private checkpoints remain outside the checkout/CI artifacts.

The strict active-HQ Clang ASan/UBSan retry now performs genuine F8 from the
unchanged2167/3500 source checkpoint,20 seconds of continuation, original F7
and normal close.830 source frames return4 with audio0/idle hints; whole-process
exit1 fails LeakSanitizer2988 bytes/seven allocations. No preceding invalid
access or UBSan diagnostic is observed, including the corrected asteroid sort
path; this is not a sanitizer pass. The first manual attempt is watchdog-killed
without final LSan/close and remains failed. Leak stacks include external DBus
and SDL PipeWire; unidentified frames are not conclusively attributed. No
suppression is applied. Complete missions and visible/audible acceptance remain
the user's responsibility, reaffirmed2026-10-14. Agent execution stays bounded.

`Menu_campaign::onButtonTerran` sets M_TERRAN and uses GameProgress movie/seen
bits, then calls `DoMenu_mission`. First-ever entry originally skips the selector
through its normal update path; returning players use the mission buttons.
Mission button0 selects `T_Mission1.dmission` (qmission fallback) and records
mission-seen, then `DoMenu_Briefing` calls `Mission::LoadBriefing`. That owner
loads parser/global records and MScript's original STARTBRIEFING program. Start
loads the original external map through Mission and runs STARTMISSION on the
source game-active transition. Seen/movie bits are not completed-mission bits.

The original source module is
`Code/App/Src/Scripts/Script02T/Script01T/Script01T.cpp` (nested upstream layout),
with `DataDef.h/.cpp`, `Script01T.rc` and `resource.h`. Lawful external first-mission
Globals actually requests `script01.dll`; the selected source module is Script01T.
The native binding explicitly recognizes this retail filename and the source
module name without changing the mission file or Windows loader.
`TM1_Briefing` and `TM1_MissionStart` supply original subtitles, briefing states,
mission name/objectives, initial resource/tech settings, triggers, visibility,
Blackwell jump entry and later scripted events. `prolog01.wav`, `M01*.wav`, UI
images/models/maps/textures and all other retail assets remain external.

Required production boundaries include Menu1/GT_MENU1 and its original mission
buttons/list/scrollbar/animation, GameProgress, native profile GameSystem/Engine/
render/material/texture owners, MGlobals/MScript/parser/string resources,
ObjectList, sector/camera/HUD/input, AirDefense (the AD!!T_Corvette launcher),
ScriptObject (Andromeda Beacon), TerranBuild/TerranDrone (CraneBot), MantisBuild/SpiderDrone, and the
previously integrated gunboat/fabricator/harvester/platform/planet/trigger/
waypoint/jump owners. Add further factories only when a real requested dependency
is identified. The original script later requests MANTIS_FRIGATE_RUSH player AI;
its original production owner and strategies now compile and the genuine
serializer executes in a synthetic archive check. Later mission AI execution
and victory/defeat remain unverified; no no-op or automatic success is supplied.

## Integration hypothesis and checks

The initial named blockers are the native Terran menu rejection and Sol_T-only
MScript binding. Native programs need separate per-module registration lists;
otherwise STARTBRIEFING/STARTMISSION flags would launch both missions. Bind only
the selected module, retaining original program names, order, flags and parser
records. Static native Script01 types/data are isolated in a namespace so they
cannot collide with Sol_T's DLL-local MissionData/data symbols. Windows keeps
its original module lifecycle and branches.

Extend existing layout/parser and shell regression checks for Script01 resource
bytes, persistent MissionData/briefing/start-record extents/field correlation,
localization IDs and module-list separation. The existing Sol_T-only schema check
cannot cover different Script01 records or detect a mixed start list. These checks
decide whether source module binding can proceed; they do not accept gameplay.
The first real ScriptObject load also exposed truncation of SCRIPT_SET_ARCH to
S32. Its private runtime archetype field/argument must use the existing native
pointer-sized animation handle; no persistent field is widened.

## Validation and handoff

P1a technical checkpoint: Linux Debug GCC 16.2.1 and Clang 22.1.8 optional
font/shared-decoder builds compiled `frontierx_native_shell` and the affected
layout/parser/data-root targets. Existing `portability.layout`, `data_root`,
`app_schema`, `script01_schema`, `sol_t_schema` and `native_shell` checks executed
successfully on both compilers before the ongoing P1b changes. The schema check
compares generated source bytes and the original 20-character mission title;
layout checks preserve MissionData (376 bytes) and briefing/start records.

Bounded external-data runtime on the ordinary Linux SDL/Vulkan host loaded
`T_Mission1.dmission`, presented a production object frame, ran the source modal
loop, completed Blackwell's scripted jump, and exercised normal mouse selection
and a selection voice. The timed run returned source QUIT and closed with zero
mission audio streams and idle hint ownership. Original campaign and returning-
player mission selection were inspected visibly; separate complete briefing and
full gameplay acceptance are not inferred from that observation. The user judged
P1a sufficient to proceed to P1b; this is not a full mission playtest result.

Native Windows Debug/Final and affected module ABI execution, actual CI runs,
full mission gameplay, complete graphics/audio, and all P1b save scenarios remain
open. Existing host resize and instrumented whole-process leak failures remain
recorded in the technical CI evidence. Ongoing P1b edits require fresh validation
before these earlier build results apply to a candidate.

## P1b work in progress

2026-10-14 renewed user feedback: presentation looks very good; wheel zoom,
Harvester beam origin and low FPS are the reported remaining issues. Native
original camera-wheel handling and render-only muzzle alignment are corrected.
The existing original campaign/F8 path now also executes bounded high-FPS
rendering, actual wheel zoom and normal close after idle-safe GPU buffer reuse.
The optional original uncapped configuration at normal game speed reaches
approximately60 source FPS on the measured quiet initial/loaded scenes; no full
mission outcome, combat-throughput or presentation acceptance is inferred. Full
user mission tests remain open, as do the explicit media/font/host/Windows gates.


- Gameplay: original `MANTIS_FRIGATE_RUSH` dispatch and production player AI,
  later unit/production/ability factories, original victory/defeat, and early,
  active-order/production/combat save continuation. Audit pointer-bearing AI save
  records against fixed Win32 disk layout before enabling native saves.
- Presentation: every required mission asset and material/shader pass, texture
  stage/format, lighting, backgrounds and combat/ability effects. Successful
  loading is insufficient evidence of correct visible output.
- UI/media: both original minimaps, animated selection and communication
  portraits, lip frames, spatial effects, music and intended movies. Existing
  exclusions are intermediate limitations and prevent complete P1b acceptance.
- Validation: independent review, refreshed default/optional GCC/Clang checks,
  focused sanitizers and explicit bounded host smokes; Windows/ABI and actual CI
  runs remain required but unexecuted locally. M3 full comparisons remain separate.
- User: full original victory/defeat and visible/audible mission inspection,
  including all required save/reload scenarios, remain pending on a documented
  candidate. No final candidate without the required presentation exists yet. Private inputs, cores, captures, traces, saves
and detailed failures stay outside repository and CI artifacts.


### Bounded P1b gameplay and presentation checkpoint

An optional Clang Debug SDL/Vulkan run on lawful external mission data exercised
normal F7/F8 early and after selecting Blackwell and issuing the original mouse
movement command. A subsequent read-only debugger observation confirmed two
actual `Mission::QuickLoadFile` returns of1; this does not inject gameplay state
or fulfill objectives. The source modal loop continued and closed normally.
Private captures showed Blackwell's original communication portrait, subtitles,
selected-unit HUD and the source three-ship selection instruction after reload.
A second bounded run removed the background exclusion and showed the original
sector-map grid/current-system indication after reload following native normal-
map composition. This is bounded retail-data execution, not full user gameplay
or complete visual/audio acceptance. Correct undiscovered/discovered links,
normal map interactions, complete background appearance and later combat remain
unverified. Early/active-command load execution does not accept all production,
combat and save-continuation scenarios.

Original `SPlayerAI` and strategy factories, plus `MineLayer`, now compile in the
native closure without replacing strategy decisions. Native assignment writes
use a fixed Win32-shaped128-byte record, retaining a four-byte unused link slot
instead of writing a native pointer. The existing synthetic shell lifecycle
check uses the real source serializer inside a writable UTF mission archive,
checks records and rejects invalid/truncated input; native Windows/ABI evidence
is still pending. The production `PLAYERAI_SAVELOAD` extent and nested storage
contracts are checked by the existing layout probe.


### Further bounded original-control progression (2026-10-14)

An optional Clang run used original group selection and map movement to reach
the Gate2 frigate encounter. Actual damage executed, Austin was destroyed,
source command points changed from12 to10, and original instructions continued.
The real Fabricator subsequently arrived with Alpha/Beta. Original F (find
Fabricator) and Slash (centre selection) selected and centred it; normal movement
to the planet triggered the source `TM1_BuildBase` instruction. This is original
source progression on external data, not an accepted mission victory or defeat.
Combat was mostly outside the camera, so effect appearance is not accepted.

The subsequent original HQ control entered placement mode, but a usable slot
was not yet confirmed and no HQ construction started. That private session
reached its600-second watchdog and was terminated: it is a failed bounded test,
not normal close or a passed playtest. Slot presentation/control is under
source-owner investigation. Earlier successful normal close observations do
not erase this failure. No post-production/combat save continuation is claimed.

The renderer now carries authored mip chains and exact RGB5/A1 precision; see
[renderer checks](RENDERER_BACKEND_PROTOTYPE.md#authored-mip-levels-background-precision-and-opaque-primary-frames-2026-10-14).
Regular Terran entry initializes the original effects-option defaults, which
were previously omitted because their source owner was Win32-only. An initial
full-background run exposed a zero StarBk handle; native SpaceEnv now fails that
required load explicitly. Actual background reinspection after the format fix
is pending. Full external family/style font mapping is implemented, but lawful
Arial/Arial Narrow regular/bold faces are not available on this host. The
single Bauhaus-face mode remains an explicitly bounded interim configuration.

### HQ construction and active-production continuation (2026-10-14)

**Retail runtime, bounded:** the optional Clang 22.1.8 Debug
`frontierx_native_shell` with SDL/Vulkan, FreeType and the reviewed external
shared decoder reached actual HQ construction through the original controls:
F to select the Fabricator, Slash to centre it, the normal system-map camera,
the Fabricator build tab, the HQ icon, then left-click on the original placement
preview beside Tau Ceti Prime. The original cost was deducted exactly:
4500 ore, 2500 gas and 1500 crew. The original Fabricator construction voice
and production compound mesh executed. No mission flags, objects, objectives,
RNG calls or simulation state were supplied by a test replacement.

Read-only debugger return observations confirmed that the original
`Mission::QuickSaveFile` and `Mission::QuickLoadFile` both returned 1 while this
construction was active. A fresh process subsequently loaded that save,
continued the Fabricator's movement and advanced the construction mesh. It
performed another successful source save/reload and closed normally with zero
mission audio streams and idle hint ownership. The later run executed 336
source modal frames; its wall-clock interval is not a simulation-duration claim.
Private saves, captures and debugger logs remain outside the repository.

This continuation enabled authored mip levels, space background, animated
communication portraits, source mission SFX and music. It retained explicit
legacy-lighting, movie and notification-cue exclusions and the single external
font face, so it is not a final P1b configuration. The source music owner
requested `mystery.wav`. Natural Enemy Sighted text, a Fabricator alert voice
and enemy map markers appeared after reload. Source review identifies the
original `TM1_HQInProgress` path, which creates two Frigates and orders them to
attack the HQ once its hull reaches one third of maximum. This observation does
not establish strategic AI activation, audible spatial fidelity or visible
combat effects. The earlier communication interval recorded two source portrait
starts, 138 frame changes and 232 draws; correct visible animation and every
portrait transition still require inspection.

The original slot owner loads `slotholder.3db` and `planet_inside.3db` through
the Engine. A read-only observation confirmed the real visible Tau Ceti
`Planetoid::Render` caller, a valid slot instance and HQ `BUILDARCHEID` while
placement was active. The original pointer-based slot test produced an HQ
preview and accepted the construction click. Empty slot colors were not
visible and the preview was white; these remain presentation checks, rather
than evidence of a missing construction command. The earlier failed
600-second interval remains a failure; the later bounded successes do not
retroactively accept it.

An additional ordinary-control continuation loaded the active-production save
successfully, executed real Blackwell/Seattle/Tacoma damage against the source
Frigate attack wave and reached HQ readiness: command-point capacity changed
from 0 to 25 and maximum crew storage from 2000 to 3000. Its final source
quicksave returned 1 and it closed normally after 916 source modal frames with
zero mission audio streams and idle hint ownership. The earlier construction
checkpoint was retained externally for rollback. This run did not expose a
missing unit factory or fatal render/media owner. The camera saw the ships
after combat; it does not accept weapon/hit/explosion appearance.

The next bounded run also reached the original refinery construction through
the Fabricator build tab and refinery icon. Its two-slot placement consumed the
displayed 400 ore, with gas and crew unchanged. Source quicksave and quickload
both returned 1 during refinery construction; after loading, the source
construction continued to the natural Refinery: Construction Complete alert,
increased ore/gas storage maxima by 1000/500 and played Blackwell's original
Marine Training Facility instruction. The run closed normally after 966 source
modal frames, with zero mission audio streams and idle hint ownership. Its
source portrait counters recorded two starts, 159 frame changes and 246 draws.
It used the same bounded presentation configuration described above and
exposed no missing refinery factory or fatal media/render owner. An additional
HQ-selection capture attempt raced the planned window close and failed; that
capture is not evidence of a working HQ build panel. A subsequent fresh process
successfully loaded the original save, selected the completed HQ through its
normal model hit area and showed its source HUD (3500/3500 hull, Tau Ceti
location), selection voice, command controls and Fabricator production icon.
It closed normally. Production from that HQ icon was not exercised.

Strategic AI roster creation,
active-combat save continuation, original victory/defeat, full presentation and
audible output remain unaccepted. The active-production save/load observation
does not prove every production queue, ability or combat save scenario. Native
Windows Debug/Final/module ABI and actual CI execution remain pending.

### Required ability-factory closure audit (2026-10-14)

**Source review and private retail-resource inspection:** the selected Mantis
strategy can produce Seeker, Khamir, Hive Carrier and Scarab. Their original
gunboat records reference `PL!!M_Ping`, `ZEALOT`,
`RL!!M_RepellentCloud` and `RL!!M_StasisField`, respectively. The last two
launchers resolve to `REPEL!!M_RepellentCloud` and `STBOLT!!M_Stasis` through
the existing `ReconLaunch` factory. Original `RepellentCloud`, `StatisBolt`,
`Mimic`, fighter-wing and projectile factories were already in the native
target. Seeker's `PingLauncher` and Khamir's `Zealot` factories were absent;
their original owners are now included in `frontierx_app_gunboat_owner`. GCC,
Clang and Clang ASan/UBSan narrow builds compile that closure. The existing
layout and schema checks cover their fixed-width records. Actual Seeker/Khamir
ability execution and combat continuation remain pending.

The ports retain original ability logic, RNG calls and Windows branches. The
native Zealot serializer checks the original 64-byte record extent, exact I/O
counts and schema-correlation result. `BASE_ZEALOT_SAVELOAD` remains 20 bytes,
`TRANS_SAVELOAD` 40 bytes and the completed record has four-byte alignment.
The original Ping launcher has an empty storage type: its `Save` writes no
payload and its `Load` returns false. `Gunboat::Load` recreates and initializes
the launcher. Its native path now validates this genuine stateless restart: the
actual Ping dynamic type, bound original archetype, launch-owner identity,
initialized supply/technology fields, clear transient event state and canonical
empty `Turret-N` directory must match. The original `PingLaunch::Load` remains
false. Other launchers must return success from their real loaders. Unknown,
misbound or nonempty Ping state fails. Native Gunboat save failures and
persisted-object load failures now reach the original mission result and
quickload feedback, while intentional save omissions by other owners and
Windows paths remain unchanged.

The cloud requires `cloud_weapon3.tga`, `neb1.tga`, its original spray/damage
blast archetypes and source launch SFX. Stasis requires `Missile1.cmp`,
`stasis_multimap.tga`, `fractal_tile.tga`, `stasis.anm` and its original blast.
These are externally supplied resources, not repository fixtures. Successful
registration or loading does not accept the corresponding visible effects,
audio or save continuation. Scarab's required ability is Gravity Well;
Solarian Polaris Mass Disruptor and Taos Synthesis are not selected by this
Mantis production path. Actual required resource references remain the scope
authority rather than assuming that every full-tree ability is used.

## Native builder-drone array ownership (2026-10-14)

Actual strict Clang ASan/UBSan mission loading exposed a malloc/delete[]
allocator mismatch in `TerranDroneFactory::OBJTYPE`: its engine pool was
allocated as `ObjectImpl<EngineTerranDrone>[]` through the COM heap but stored
and deleted through `EngineTerranDrone*`. The same original owner pattern also
exists in each builder's ShipNode and in SpiderDrone, needed by the selected
Mantis builder closure. Native runtime pool/borrower pointers now retain their
actual wrapper-array types through indexing and destruction. Windows fields
and paths, saved records, simulation constructors and RNG calls are unchanged.

The existing shell lifecycle regression now allocates actual Terran and Spider
factory archetype/pool arrays and destroys their production owners. It executes
on optional GCC 16.2.1, Clang 22.1.8 and Clang ASan/UBSan with the required
allocated-owner marker and no sanitizer report. No retail archetype is needed
for this ownership check. The real mission retry is separate; a synthetic
ownership pass does not prove whole-mission cleanup or complete gameplay.
Affected native Windows builds/ABI and full user P1b acceptance remain pending.

## Native persisted-owner failure and system-map restoration (2026-10-14)

**Source review and compilation:** the original Gunboat loader discarded child
load failures, and ObjectList also discarded failed persisted-object loads.
Native Gunboat now propagates base/child failure. Its required consecutive
launcher slots must contain actual original factory instances: native initial
creation fails explicitly on factory or interface failure, and restoration
returns failure instead of accepting a weaponless unit. Native ObjectList destroys the
not-yet-added failed object and aborts loading. A concrete Gunboat save failure
cannot become an intentional omitted object. Mission save, quicksave and dynamic
save propagate the affected bool results and require the real UTF writer before
publication. Quickload failure reports the original failure text. The actual
UTF writer owns strict write/flush/validation and atomic publication; no second
serializer or successful placeholder is used.

The selected script's Beacon alert is stored by the original SystemMap owner,
but the production mission writer had no call to its existing serializer.
Native mission save paths now call it; regular and dynamic reload restore the
saved map records through the checked owner, including the dynamic overlay
after loading initial terrain. Records keep the original fixed-width schema
and list order. Initial retail mission data and historical M2 saves may lack
this optional record. New first-Terran `.mission` checkpoints require its
canonical header even with zero alerts; older intermediate Terran checkpoints
without it are diagnostic rollbacks, not P1b-compatible saves. Present malformed
or failed PlayerAI records also abort native mission reload; legitimate absent
AI before its script trigger remains valid. Windows source bodies are unchanged.

The cheapest regression extends the existing native shell lifecycle, because
its AI serializer fixture has no typed stateless launcher. A synthetic original
Ping factory and `InitLauncher` must accept its empty restart while rejecting
wrong owner/type/directory and unexpected payload; its unchanged original
`Load` must still return false. A negative-only persisted child must propagate
failure through the same native Gunboat branch and be called exactly once. No
simulated unit or alternate mission is used. These new fixture bodies and the
Mission/ObjectList changes pass Clang 22.1.8 syntax checks with their exact Ninja
compile flags. Automated execution of this new snapshot, fresh compatible
retail checkpoint/reload, strict sanitizer retry, Windows Debug/Final/module
ABI and user acceptance remain pending at this checkpoint.

### Selected active-ability persistence guards (2026-10-14)

**Source review and compilation:** original Mimic, RepellentCloud and StasisBolt
loaders ignored I/O byte counts or schema-correlation failure before applying
saved state. Cloud/Stasis restoration then iterates serialized square/target
counts. Their native save/load branches now require exact original record
extents and I/O counts, successful original correlation and bounded counts
before applying state. Cloud uses the original maximum of 24; Stasis uses 26.
Their `lastSent` indices must also fit those original arrays. Original record
sizes remain 8 bytes for Mimic, 348 for Cloud and 336 for Stasis, with native
compile-time assertions. Windows branches, simulation order and valid saved
bytes are unchanged.

The three actual owners pass Clang 22.1.8 syntax checks with their existing
Gunboat target flags. Existing source-schema fixtures already round-trip
maximum Cloud/Stasis counts and verify the corresponding record extents;
those correlation checks are distinct from executing these owners' Save/Load.
The existing shell lifecycle now has a connected source-owner fixture: actual
clear-allocated Mimic/Cloud/Stasis objects write their original records to a
physical UTF archive, reopen and inspect them, reject read-only saves, and
reject truncated/oversized loads. Cloud/Stasis also reject out-of-range original
counts before correlation or frame state is applied. The fixture supplies no
fake Mission or successful replacement loader and performs no simulation. It
temporarily binds the existing genuine ObjectList fixture owner, because the
original ObjectMission destructor removes even a zero ID through that owner;
all serialized objects are destroyed before releasing the binding.
Its final bodies pass the same Clang syntax checks; their automated execution
and sanitizer retry remain pending. Actual ability use, successful active-ability
retail save continuation and complete visual/audio acceptance remain separate
open checks.

### Connected fixture execution and fresh strict-load blocker (2026-10-14)

**Automated synthetic execution:** the optional Clang 22.1.8 native-shell
lifecycle now executes the genuine typed Ping restart, failed Gunboat-child
propagation, physical SysMap persistence and real Mimic/Cloud/Stasis writer plus
malformed-record checks described above. The rebuilt shell completed that
focused regression successfully. These checks do not accept actual ability
combat, Beacon visibility or full mission save continuation. Updated independent
compiler/sanitizer confirmation remains separate.

**Bounded retail runtime failure:** the subsequent fresh first-Terran load used
original data with authored mip levels, legacy lighting, background, source SFX,
music, notification cues and communication portraits enabled. It retained the
explicit movie exclusion and single external font face. Loading failed before
a playable window or any F7/F8 action: the initial `PLATBUILD!!M_HybridCenter`
record references `MBioForge.cmp`; its source BuildPlat initialization reached
`TObjExtent::initExtents` with a null resolved Engine mesh and dereferenced it.
Required texture-format errors and ignored compound-part failures preceded the
fault. The archetype index and MeshManager archetype existed, so those values
alone do not establish a usable model.

Read-only debugger reproduction confirmed the original ObjectList mission-load
caller and the null mesh. It did not alter objectives, simulation or saved
state. No OOM evidence was found; no core was extracted. The first batch debugger
returned zero after stopping at SIGSEGV, which is explicitly a failed game run;
subsequent diagnostic runs returned nonzero. No save/reload or normal-close pass
is claimed. The fresh private save directory is empty and earlier diagnostic
checkpoints remain unchanged outside the repository.

This is an open P1b blocker at the required material/texture and Engine compound
creation owners. The fix must reject failed required components before actor
initialization and integrate their actual formats; a null guard, absent geometry
or replacement mesh does not accept the mission. Fresh compatible retail
checkpoint/reload testing must resume after that owner fix.

### Fresh compatible moved-state checkpoint after compound texture fix (2026-10-14)

**Bounded retail runtime:** after the Engine/compound-owner fix loads the outer
compound TextureLibrary before its required parts, a fresh first-Terran run
completed initial loading, original selection and movement, three successful
source F7 returns and four successful source F8 returns, then normal close. The
read-only debugger observed the real QuickSave/QuickLoad return values; normal
SDL mouse controls selected Blackwell and issued the movement command. The scene
continued after the moved-state reload. This optional Clang 22.1.8 candidate at
`/tmp/frontierx-p0-validation/frontierx-m3-vbl25jg1/build` executed 1,783 source
modal frames, original `QUIT4`, and exited normally with zero remaining audio
streams and idle hint state. Portrait counters recorded three original starts,
81 frame changes and 126 draws. Private owned-window captures retain the
selection HUD, communication portrait, world and both initial maps.

This run enabled authored mip levels, source legacy lighting/materials,
background, mission SFX, music, notification cues and communication portraits.
It retained only the explicit movie exclusion and one external font face. It
used direct source mission startup for the bounded checkpoint test; final entry
through the original campaign selection and briefing still needs its separate
handoff check. Asset loading, these captures and portrait counters do not accept
complete visual or audible presentation.

The fresh private checkpoint contains the original eight-byte SysMap header
with `lastID=0,count=0`, Blackwell's 464-byte `GUNBOAT_SAVELOAD` with actual
`bMoveActive=true,pathLength=1`, and Beacon's 156-byte
`SCRIPTOBJECT_SAVELOAD`. These are read-only observations of real production
Save output, not regenerated state. Earlier reduced-configuration checkpoints
remain outside the repository and were not loaded into this compatible run.
The preceding HybridCenter null-mesh load blocker is resolved for this bounded
fresh-load path; its recorded failure remains useful owner evidence.

No nonzero Beacon alert, revealed system connection, later strategic-AI trigger,
active ability/combat checkpoint, original victory or defeat was reached in this
interval. Full mission completion, gameplay/visual/audio user acceptance and
affected Windows Debug/Final/module ABI execution remain open.

### Checked native base-object and script-state persistence (2026-10-14)

**Source integration and Clang syntax checks:** original SpaceShip and
ScriptObject Save now require the real writer's success and exact byte count.
Their native Load reads the exact extent of the *saved* record schema, verifies
the current schema against the real destination, and uses the production
ViewConstructor's actual internal correlation result. Output reaches the actor
only after successful correlation. This keeps genuinely supported older schemas
eligible for correlation rather than assuming every input has the current C++
size. The public void ViewConstructor vtable and Windows bodies are unchanged.
ObjList propagates failed writes from these specific persistent owners; it
preserves other owners' intentional omission semantics.

Native MScript restores saved ParseData, GlobalData and active program records
with checked exact reads and genuine schema correlation. Saved `.mission`
records require their script state, while absent initial `.dmission` ParseData
remains a legitimate fresh-mission case. Unknown program factories/types,
malformed enumeration names, truncated records and failed correlation cannot
become successful reloads. The private native wrapper returns failure to Mission
and calls the original MScript cleanup on a partial load. The original Windows
path, public interfaces, record widths, iteration order and simulation callbacks
remain unchanged.

The original empty save-program contract is one disk byte, although its parsed
record has zero fields. The existing native `NativeScriptSaveBase` owns that
byte outside the vtable; the actual source factory constructs it as zero.
Loading validates the genuine known factory/type and one-byte extent and uses
real zero-field correlation. It neither copies padding into a program vtable nor
initializes goals. The existing campaign-registration regression now creates
and destroys the actual empty Sol/Terran programs, checks their original schemas
and nonzero save offset, and exercises this provider path without invoking
Initialize.

A connected native-shell regression uses a real temporary Mission and the
production App schema to physically save/reopen a synthetic source-shaped
ScriptObject record, correlate it, and reject shorter/longer records and an
incomplete saved schema before changing the destination. This protects the
actual leaf Load helper rather than implementing a separate serializer. The
focused ViewConstructor check independently covers canonical/reordered records
and missing required fields. Latest owner syntax checks pass under the optional
Clang configuration; execution of the newly extended shell checks, updated
sanitizers and a fresh retail reload with these stricter base/script owners
remain pending. Previously completed retail F7/F8 evidence above belongs to the
snapshot before these latest persistence changes.

The same checked persistent record reader now serves the original Platform
owner used by headquarters, refineries and selected enemy defence/production
platforms. Its native Save checks exact IO. Source review found nine actual
Platform-derived owners; all inherit this persistent writer except Gunplat,
whose override adds turret/special-launcher directories and has no deliberate
omission path. Consequently ObjList treats their `OC_PLATFORM` Save failure as
failure rather than silently dropping a production actor. Other owners'
intentional omissions remain unchanged. Gunplat now propagates genuine base
and child Save/Load failure; the original known stateless Ping restart contract
is shared with Gunboat. A missing required nonempty full-mission launcher cannot
produce an apparently successful weaponless actor. Original launcher order,
empty terminators and Windows branches remain intact.

The connected MissionRecord fixture also checks the real nested
`BUILDPLAT_SAVELOAD` schema with active operation/construction/time fields and
rejects a truncated platform record. This is synthetic input through the
production record reader. It does not create a fake initialized platform or
claim that a synthetic actor writer/positive actor Load ran: an actual platform
lifecycle requires the real animation, sector, object-map and mission owners.
Latest Clang syntax checks pass for Mission, Gunboat, Gunplat, BuildPlat and
ObjList; the coordinated rebuilt fixture and bounded original-control
construction/save continuation remain pending.

**First execution of the new record fixture:** optional GCC and Clang shell
checks rejected its setup. The fixture attempted an unsupported physical
`OPEN_EXISTING` read/write archive reopen, and its platform branch initially
read the just-written file at end of file. The corrected setup retains the real
writable archive for negative cases, opens a separate physical read-only reader,
and reopens platform children before loading, exactly as the production caller
does. Bounds, byte comparison, malformed-schema and transactional checks are
retained; precise fixture-stage diagnostics were added. This is a test setup
correction, not a production permission or seek change. Its latest Clang syntax
check passes; the corrected execution remains pending.

**Independent corrected execution:** the validator rebuilt the actual Mission
object after both fixture setup corrections under standard and optional
GCC/Clang configurations. The native-shell source markers and all empty-program
factory checks execute successfully in each configuration. Both optional GCC
and Clang ASan/UBSan builds also execute the connected persistence checks
successfully with strict leak checks; no suppression or assertion relaxation
was introduced. The focused production ViewConstructor regression executes the
canonical/reordered/missing-field cases. These results resolve the new
synthetic-fixture setup failures above. Actual first-Terran reload after these
stricter actor/script paths, active production/combat/ability continuation,
Windows module/ABI execution and full user acceptance remain separate open
checks.

**Actual stricter initial-load retry:** the subsequent independent optional
Clang ASan/UBSan first-Terran load returned failure before the modal scene loop.
No new ASan/UBSan invalid-memory report accompanied that return; the existing
external-host leak report remains separate. The progress indicator's 100%
cleanup frame does not establish completed mission loading. An Anim2D resource
error appears earlier during global startup, but no owning failed mission
predicate has yet been identified. Consequently the stricter retail actor/script
load is a current blocker despite successful synthetic records and compiler
checks. Read-only original-loader return diagnostics are prepared; no assertion,
IO bound or required media check has been weakened. The previous 1,783-frame
runtime belongs to the earlier snapshot and cannot accept this new one.

### Original Globals schema migration exposed by truthful correlation (2026-10-14)

**Read-only retail diagnosis:** both normal Clang and instrumented initial loads
first failed in `MGlobals::Load`, before ObjectList or mission scripts. Original
retail and current `MT_GlobalData` are both 3,236-byte records. The original
schema lacks the later `maxPlayers` nibble, and each of its eight eight-byte
`CQGAME::_CQGAME::SLOT` records lacks `compChalange`. The added four-bit slot
field shifts the following state/race/color/team/zoneSeat bit positions, while
the record stride remains eight bytes. The additional global nibble similarly
moves lighting/UI flags and the following byte arrays. These are actual schema
observations; unchanged total size alone did not establish compatibility.

The native Globals owner now invokes the genuine provider with an explicit,
validated migration for only these two known additions. It validates the
original/current complete record and nested slot-array widths, counts, strides
and packed signatures. Only the exact current metadata/difficulty symbol
identities may remain zero, matching the original Windows provider's cleared
destination behavior. Other missing fields, altered extents and invalid slot
shapes still fail. `compChalange` affects standard AI difficulty and is not a
cosmetic field; no AI rule, difficulty setter, mission initialization or RNG
call has been added to compensate for it.

The old schema also represents several packed CQGAME fields as enums while the
restored current schema uses integers. The supported Windows
`DataViewer::CopySymValue` passes a known enum's symbolic identifier to integer
`SetSymValue`/`strtoul`, which yields zero and a successful conversion. The native
counterpart previously left the same zero output but returned failure because
it attempted an enum-name lookup on a basic type. It now implements that exact
same-width, bounded, known-identifier conversion. It does not reinterpret the
old enum's numeric value, change enum-to-enum mapping or accept floating/string
conversion. Windows implementations, public vtables and serialized extents
remain unchanged.

**Focused synthetic execution:** independent Clang provider/schema checks use
the complete original App schema, genuine correlation to encode the authentic
older packed shape, and the actual owner API to restore it. All eight slots'
other fields/IDs and global flags remain preserved for equivalent scalar types;
only the proven absent metadata/difficulty fields become their Windows zero
default. Canonical present values remain intact. Missing required fields,
wrong record ownership, altered extent, array count or stride still reject.
Separate genuine-provider checks protect Windows enum-name/integer output,
packed neighbors and invalid width/type/bit-bound rejection. Updated GCC and
instrumented confirmation for this latest second migration remain pending.

**Latest bounded retail result:** with this migration the fresh scene reaches
original Blackwell jump and 1,163 modal frames. Original F7 returns success, but
F8 returns failure; normal owned close exits zero. This is explicitly a failed
save/load acceptance attempt, not a playable P1b pass. Required model-geometry
diagnostics appear during preloading and need ownership/roster confirmation;
normal initial rendering and surviving counters cannot prove that every
required actor loaded. A corrected full-compound geometry predicate and the
next saved-reload owner check remain pending. Earlier private checkpoints are
unchanged and no partial mission has been accepted.

### Canonical moving-ship reload and unnamed union correlation (2026-10-14)

**Read-only retail diagnosis:** after the compound-geometry owner correction,
the remaining F8 failure was the actual `GUNBOAT_SAVELOAD` provider operation,
with saved/current/file extents all 464 bytes. Its nested `TOBJMOVE` contains
an unnamed four-byte union at offset 48 with `overrideSpeed` and `overrideYaw`
float members at offset zero. Both separately parsed schemas describe identical
storage, but the original name-only lookup rejected the unnamed union. The
native lookup now requires one unique matching unnamed union with exact field
extent, offset, member names, member extents/modifiers and resolved types. The
original union byte-copy operation then executes. Ambiguous or changed storage
rejects; no serialized name, field, default or mission logic was introduced.
Native unknown packed enum values also reject explicitly rather than reporting
success without writing a value; known enum conversions retain the documented
Windows source semantics.

**Bounded retail execution:** the integrated optional Clang shell successfully
loads the original scene and restores the previously failing private checkpoint
through normal F8. A new canonical F7/F8 pair and a subsequent normal Blackwell
selection/right-click movement F7/F8 pair all return success from the original
mission owners. Independent private saved-record inspection confirms Blackwell's
464-byte record has `bMoveActive=1` and `pathLength=1`; the canonical SysMap record
is eight bytes with zero animations. The post-reload owned-window capture shows
the selected Blackwell HUD, source movement acknowledgement, communication
portrait, script instruction and both map panels. The original modal loop runs
1,894 frames, returns its normal quit code 4 and closes with actual process exit
zero; source portrait counters record two starts, 19 frame changes and 32 draws.

This interval enables authored texture levels, legacy lighting, background,
mission sound/music, notification cue and communication portraits. Movies remain
explicitly excluded and the font uses one external face. It does not establish
full presentation parity, nonzero Beacon-alert/map restoration, active production
or combat continuation under this latest snapshot, later AI, victory/defeat or
user acceptance. Additional matcher rejection checks were tightened immediately
after this run; their independently rebuilt provider tests and subsequent
instrumented retail retry remain pending. Required Windows Debug/Final and module
ABI execution are also pending.

**Subsequent independent technical execution:** the final provider/schema
checks pass under GCC and Clang, including instrumented video-enabled builds.
They preserve nonzero canonical `TOBJMOVE` union payloads and reject changed
member names, outer/member extents, offsets, basic types and ambiguous matches.
Unknown packed enums reject without modifying destination bytes; the original
nonpacked raw-copy contract remains exercised. These checks cover the final
strict matcher increment; they do not establish a full retail mission run or
replace the still-pending instrumented retail continuation and Windows gates.

**Original roster completeness:** read-only private UTF enumeration of the
original `T_Mission1.dmission` finds 64 actual dynamic QuickSave payload records
in 25 archetype groups. Its `Count` header says 67 and is stale loading-progress
metadata, rather than evidence of three missing payloads. The observed source
dynamic load count 64 therefore matches the complete physical roster, including
five Terran Corvettes, Blackwell, the Fabricator, 15 Mantis Frigates, four Hive
Carriers, two Scout Carriers, all authored platform groups, six waypoints, one
trigger and the Beacon. The separate terrain owner loads all 109 recorded terrain
objects. No header, actor creation, ordering or acceptance check was changed to
force these counts.

### Video-enabled original entry and scouting checkpoint (2026-10-14)

**Bounded retail execution:** the separate optional Clang video-enabled build
uses the reviewed external shared-LGPL decoder prefix and no presentation
exclusion flags. On the available Wayland host, the original Terran selector,
animated Radio briefing and normal Start button lead to the source mission
scene. Early, Blackwell-movement and subsequent three-ship scouting F7/F8 pairs
all return success from the original mission owners. Normal selection/movement
advances the original diagnostic objectives: its actual `SelectGroup` condition
requires Blackwell and the two specified Corvettes, and then enables the original
four-grid-unit WormholeTrigger. Ordinary system-map orders reveal additional
terrain and move the group; no goals, actors or script states were injected.

The source modal loop runs 3,375 frames, returns normal quit code 4 and closes
with actual process exit zero. Source communication portrait counters report
four starts, 257 frame changes and 374 draws. The private fresh scouting checkpoint
is preserved for continued testing. This run uses one external Bauhaus face,
which remains a font-presentation limitation. It does not independently prove
campaign-intro playback, correct spatial audio or complete authored rendering.
Production and combat were not reached during this interval and remain
unverified for this latest candidate; further bounded continuation first needs
the actual original trigger location/control path, rather than guessed scouting.
Full victory/defeat and user presentation acceptance remain open.

An initial explicit-X11 attempt returned from the campaign selector without
entering the scene or receiving test input. Its process exit zero is an original
menu-cancel return, not successful gameplay evidence. The normal Wayland retry
above succeeds; the X11-specific menu/startup cause remains uncharacterized.

### Natural first encounter and active HQ production (2026-10-14)

**Read-only private checkpoint diagnosis:** the actual enabled trigger's program
is `TM1_EnterTheMantis`; a similarly named dormant trigger is a different object.
The persisted original trigger position and the current player ships identify
the normal proximity order needed for continuation. Neither trigger data nor
simulation state was modified. The selected Blackwell moves through the original
system-map controls and activates the actual encounter. Original Austin and the
three attacking Frigates jump into Tau Ceti, and the source Mayday/mission
instructions execute. Visible weapon/projectile effects and hostile map
indications accompany genuine Blackwell damage, consumed supplies and kills.
Encounter and damaged-state F7/F8 operations return success. This is bounded
original combat evidence, not full weapon/effect presentation acceptance.

After the encounter, the original Halsey/Fabricator phase advances naturally.
The Fabricator jumps in, receives normal selection/orbit orders, and the original
HQ construction instruction appears. Earlier placement clicks outside the source
planet-slot annulus create no HQ and deduct no resources; they are failed input
attempts, not production evidence. A subsequent normal click within the original
slot annulus shows the actual green HQ preview, creates the source HQ construction
mesh, deducts 4,500 ore/2,500 gas/1,500 crew, and starts the original
`TM1_HQInProgress` program and speech. The genuine active-production F7/F8 returns
success. The private checkpoint contains the actual 524-byte HQ record and the
original one-byte `TM1_HQInProgress_Save::attackLaunched` state. This record is
not an empty program; its boolean field must survive the genuine correlation.

The no-exclusion-flag, video-enabled optional Clang continuation runs 4,064 source
modal frames and closes normally with quit code 4, actual process exit zero,
zero mission audio streams and idle hint ownership. Source portraits record seven
starts, 438 frame changes and 699 draws. The last capture may precede completion
of the F8 resource upload; it does not prove later construction advancement or HQ
completion. Refinery production, the later HQ attackers, strategic AI, nonzero
Beacon-map persistence, victory/defeat and full user acceptance remain open.
The planet visibly retains a gray gradient in this full-flag run; its production
material/render owner is an explicit presentation blocker under investigation.
The single external font face also remains limited. Private encounter and active
HQ checkpoints/logs are retained outside the repository for bundled continuation.

**Targeted post-load continuation:** a separate short process restores the
existing active HQ checkpoint through original F8, runs ordinary focused frames
without issuing gameplay orders, saves through F7 and closes normally. Both
original operations return success; the source runs 149 modal frames, returns
quit code 4 and exits zero with zero mission audio streams and idle hint ownership.
Private read-only decoding with the original fixed-width headers compares the
same 524-byte HQ record before/after: hull advances from 394/3,500 to 1,068/3,500,
with `bReady=false` in both records. The actual `TM1_HQFinished` trigger remains
enabled. This establishes resumed source construction after load; it does not
establish completion or the later one-third-hull enemy-attack trigger. The
25-second wall interval includes resource loading and is not claimed as 25 seconds
of simulation. No mission logic, production order or completion flag was supplied.

### Original construction display hookup (2026-10-14)

**User-reported blocker and source diagnosis:** construction advances, but its
original animation is absent for ships and buildings. `TFabricator` already
creates the actual `BUILD!!Terran` owner at construction start and after reload.
The builtee's original `ObjectBuild` calls connecting that owner to `SetupMesh`
were commented out in the CQ2 snapshot. Its percent/update/physical-update and
completion hooks remained active but had no connected effect. Both the mobile
Fabricator and BuildPlat/BuildSupPlat shipyards use this same production path.

The native full-material Terran path now reconnects the genuine effect at start,
reload and reverse construction. It borrows the existing initialized
`ObjectExtent` mesh tree and its original App MeshRender owners; no parallel tree,
serialized field or public virtual slot is introduced. Assisting fabricators
share an initialized primary effect rather than resetting its tree. The display
caller selects that tree during construction, allowing original hidden/building
face states and the `GirderRender` channel to draw together. Completed meshes
retain their regular caller. Original progress, costs, timing, fabrication orders
and Windows branches remain unchanged. Direct-Engine ship display uses the same
model-axis correction for its mesh, girder overlay and authored weld/drone visual
positions; this does not change the hull's simulation transform.

The genuine `tinnards.tga` and `BLAST!!Weld` resources are required, with explicit
native failure if either is absent. No random choice is introduced or removed.
The existing drone constructor chooses its engine variant with `rand()`, and
the engine-drone archetype initializer chooses spark rotation with `rand()`;
these are initialization calls already reached through the existing factory,
rather than newly added physical-update choices. The restored original weld
owner runs its authored effect programs. Cross-build RNG-count/determinism
comparison remains a separate unperformed gate; this source integration does
not assert that comparison passed.

**Compilation:** the final optional Clang production commands syntax-check
TerranBuild, BuildPlat, BuildSupPlat and SpaceShip successfully. Integrated
retail rendering, completion/reload, focused sanitizer execution and affected
Windows rebuild/module-ABI checks for this increment remain pending.

**Freestanding preview audit:** the existing original caller is
`ObjectList::Render` → `UnbornMeshList::Render`. Valid deep-space placement
requires the actual build cursor, screen-to-world conversion, an in-system grid
and the original empty-terrain callback. It then calls `RenderMeshAt` with green
RGB and alpha 128; that owner colors the real mesh tree and renders it. The
suspected player-color cache failure was disproved: `lastPlayer` remains zero,
so player-one draws always refresh the requested preview colors. No cache change
was made. Actual placement and backend draw/state evidence are still required
before attributing or accepting the missing free-platform preview.

**First integrated bounded retail interval:** the final optional Clang build
restores the preserved active HQ checkpoint with original F8. Read-only original
header decoding confirms its 524-byte record begins at hull 1,068/3,500,
`timeSpent=27.4667`, building=true, whole=false and ready=false. A later original
F7 record contains the same HQ with hull 3,500/3,500, `timeSpent=90`, building=false,
whole=true and ready=true. The real mission advances to its refinery instruction;
the original two HQ-attacking Frigates also initiate their jumps. Original F8,
the subsequent F7/F8 pair and final F7 return success. The original scene runs
1,789 modal frames, returns quit code 4 and exits zero with zero mission audio
streams and idle hint ownership. Communication portraits record six starts,
340 frame changes and 519 draws. Corrected production shader argument order
also makes the actual planet texture visible in this interval.

The initial capture shows a partial HQ and later captures show its completed
model. No effect-channel draw counts or sufficiently resolved girder/weld
capture were obtained, so this is construction continuation/completion evidence,
not acceptance of every construction animation. The process receives ordinary
mouse actions outside the scripted test recipe. Refinery completion and a
Harvester carrying 250/250 ore appear later, but their controls were not supplied
by this agent and are not claimed as a controlled recipe. The source input log
and checkpoint remain private. Weapon direction, girder/weld rendering, shipyard
construction animation, freestanding preview and Harvester attachment alignment
still need the separate targeted owner observations and user assessment. No
full mission or presentation acceptance follows from this interval.

**Separate construction-owner observation:** a read-only debugger run on the
same genuine partial-HQ checkpoint reaches Platform → TreeRender → MeshRender →
GirderRender and the original textured effect channel. The short observation
records 55 vertices and 60 indices using the genuine innards texture, without
submission rejection. A separate longer optional VIDEO-enabled Clang interval
runs 1,596 modal frames, records 1,311 girder calls and 455 construction-channel
draws, and closes normally after original F8/F7 with zero remaining mission audio
streams and idle hints. This establishes actual connected construction draws,
not a synthetic effect. Its 70 preview observations are planet placement;
freestanding placement is not established by those counts. Subsequent user
feedback describes building construction as visually good; Fabricator wing
animation remains a separately identified owner gap.

### Projectile sight-axis correction (2026-10-14)

**Retail runtime failure:** a read-only observer during actual user input reaches
43 Bolt initializations/2,989 renders and 15 PlasmaBolt initializations/843 renders.
Both original weapon classes fire, but the user reports their direction is still
wrong. Their actual initial velocity predominantly follows the vertical axis
despite a predominantly horizontal target displacement. This is a physical
initialization error before the display caller, not evidence of correct rendering.
The observed process subsequently aborts at the original missing-Fabricator
script assertion. Its historical early checkpoint lacks that required actor;
it predates the corrected complete compound-tree checks. This run does not close
normally, and that checkpoint must not be reused as a compatible gameplay stand.
No assertion is weakened and no missing actor is manufactured.

**Source cause and repair:** the native turret already extracts its sight angles
from the original -K-forward hardpoint frame. Bolt and PlasmaBolt instead used
the J-forward `get_pitch()` extractor before their unchanged relative pitch
rotation. Their native initialization now uses the shared
`NativeWeaponSightPitch` wrapper around the production `get_pitch_zbased()`.
Windows retains its existing expression. Target selection, original pitch
formula, range, speed, collision/update logic, RNG calls and stored fields are
unchanged; this does not replace aiming with normalized-target steering.

**Executed math falsifier and compilation:** an external private check links the
actual production SuperTrans object and registered scalar math provider. Five
target directions cover all horizontal quadrants and positive/negative elevation.
The unchanged correction formula with the old extractor leaves the resulting
forward direction perpendicular to the target; the corrected extractor produces
a direction dot product of one with each normalized target. Both modified owners
pass their optional Clang production-command syntax checks. The connected existing
material-owner regression is being extended to protect this contract. Integrated
corrected weapon runtime, affected Windows rebuild/ABI checks and full comparative
determinism remain unperformed. The training Taos uses GattlingBeam, so its separate
reported weapon issue is not declared fixed by this Bolt/PlasmaBolt change.

### Corrected original weapons and Fabricator runtime (2026-10-14)

Optional Clang22.1.8 Debug VIDEOon restores the complete genuine active HQ
checkpoint through original F8. The selected Fabricator has a real animation
instance252 and builtee43441; the existing FORWARD/END reload call executes. A
private capture shows extended wings. Early samples from unrelated factory
instances with invalid animation IDs are not attributed to this selected actor.
The source continues HQ construction and its original attackers naturally.

Actual original Bolt and PlasmaBolt InitWeapon/Render execute in the resulting
HQ encounter. Four Bolt and two Plasma post-initialization samples compare the
real explicit target position with the actual start and velocity: direction
cosine is effectively1. Each original renderer executes four sampled calls.
The limited observer disables sampled breakpoints rather than imposing a
per-render stop throughout play. This is corrected original physical-direction
and render-execution evidence; complete weapon visuals/user acceptance and the
separate training Gattling path remain open. No actor, target, order, goal or
completion is manufactured. Ordinary input beyond the scripted camera/wait
recipe occurs; it is not claimed as an automated gameplay sequence.

The user releases the window after playing. Root saves through original F7,
preserves the resulting user checkpoint separately and closes normally. Actual
GDB inferior exit is0; source runs2559 modal frames, returnsQUIT4, and closes
with zero mission audio streams and idle hints. Portraits record11 starts,487
frame changes and726 draws. The earlier genuine active F7 record remains
preserved separately: HQ524 bytes, hull3007/3500, ready0, timeSpent77.3335,
building1 and whole0. It is an appropriate active-construction sanitizer input,
not a replacement mission or automatically completed state. Shipyard display,
construction cancellation/death/assist cases, whole-process cleanup, complete
user outcomes and affected native Windows/ABI/actual CI remain open.
