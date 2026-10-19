# Terran campaign expansion

## TM10 construction reservation follow-up (2026-10-19)

The next actual crash is a second, confirmed batching defect: a full alpha pool
evicts a still-locked Solarian construction descriptor when reserving its
second buffer. Native selection now skips locked entries, preserving the
pending blend/modelview state and existing delay-stage handling. The original
three-entry-pool regression exercises both live reservations and exact completed
submissions, including a failing old-behavior countercheck. See the
[owning evidence](GAME_MATERIAL_PATH.md#tm10-locked-construction-reservation-eviction-2026-10-19).
The next private candidate retains genuine saves and all earlier TM10–TM12
repairs; actual construction/F7/F8 acceptance and Windows/ABI remain pending.

## TM10 quicksave rejection and delayed construction draw (2026-10-19)

**User reproduction and source diagnosis:** F8 returns to the selector and the
load menu reports failure. With the private runtime debug directory present,
ObjectList identifies `PLATGUN!!S_TaloreanMatrix`, instance 132, as the rejecting
owner. The genuine quicksave passes the production UTF index/entry reader;
its global record is 3236 bytes, saved App schema 164710 bytes and gun-platform
record 432 bytes. Its actual `132/Turret-0` directory is present and empty.
Recopying the same valid archive cannot repair its loader.
The archive and genuine progress remain preserved outside the repository.

`TalorianLauncher::Save` writes no payload; Gunplat creates its empty child
marker. Its original `Load` unconditionally returns false, which the native
Gunplat correctly propagates. The native launcher now accepts only an empty
marker on a freshly initialized, correctly typed launcher with a bound owner.
It rejects payloads and an already populated target list. Original Update
rebuilds its transient planet cache, shield and target memberships after
restoration; these effect flags are not serialized. This preserves the existing
save format and simulation owner. Windows keeps its existing branch. The
existing Ping fixture supplies a genuine launcher owner and UTF archive to
exercise the actual Talorian Save/Load methods, including rejection and retry.

A separate actual SIGABRT core stops at RenderBatcher's post-flush invariant.
GDB finds alpha-depth `materials_used_cnt=3`, `mats_rendered=2`, `maxDelay=1`:
`render_depth_sort` marks the pool empty after delay zero even though original
SolarianBuild queues reveal geometry at delay one. Native completion now belongs
to `render_no_sort`, which already counts all stages. The assertion is retained.
The existing batcher regression queues real alpha-depth draws at delays zero
and one and requires both exact submissions, their order and no assertions.

Focused validation uses Linux x86_64 full-media Debug builds with Clang 22.1.8
and GCC 16.2.1 and fatal Clang ASan/UBSan. All six selected checks pass in each
configuration:

```sh
cmake --build "$BUILD" --target frontierx_native_shell frontierx_render_batcher_runtime_probe -j 4
ctest --test-dir "$BUILD" --output-on-failure \
  -R '^portability\.(render_batcher_runtime|native_shell|layout|script(10|11|12)_schema)$'
```

Fatal sanitizer execution sets `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`
and `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. A controlled negative
run restores the old early-empty line: the same new regression fails with the
actual post-flush invariant and missing delayed submission. Restoring the fix
passes. The fixture explicitly enables `BM_M_F_DELAY_ON`, as SolarianBuild
does; merely setting RPR_DELAY would not test a delayed material. These
generated fixtures do not prove complete loading
of the user's TM10 scene. Its exact F8/menu recovery, construction lifecycle,
Windows Debug/Final and module ABI checks remain pending. The next private
candidate preserves the original quicksave byte-for-byte and starts the normal
selector with all earlier TM10–TM12 repairs.

## TM10 playtest crash and visual feedback (2026-10-19)

The user's full-media TM10–TM12 candidate reaches `Script10.dll` and later
segfaults in the original Mass Disruptor hull effect. The current log and genuine
quick/named saves are backed up privately. GDB locates the missing target mesh
chain at the disrupt-stage draw; its live extent binding and native Puffy path
are repaired together. The same batch repairs missing Solarian construction
beam setup, per-instance destruction markings for all three races, and the
actual `DumbReconProbe` draw frame. The probe fix leaves simulation/save fields
unchanged. See the [owning material/effect evidence](GAME_MATERIAL_PATH.md#tm10-construction-debris-probe-and-mass-disruptor-feedback-2026-10-19).

Focused GCC/Clang full-media CPU, fatal sanitizer and generated-fixture GPU
checks are distinct from the pending exact mission retest. No full TM10–TM12
completion, actual repaired construction lifecycle, Windows/ABI or retail
comparison is inferred. The replacement private candidate includes all prior
TM10–TM12/video fixes and copies the newest genuine saves/progress without an
artificial unlock. The first F8 attempt exposed the Talorian rejection
documented above. Batch the four visual checks in the same
playthrough; TM13 remains outside this candidate.

## TM10–TM12 and Celereon movie preparation (2026-10-19)

**Source integration and automated CPU execution; full mission entry and user
acceptance pending.** The original `Script10T`, `Script11T` and `Script12T`
owners now link into the native shell with separate program registries, resource
identities, counted strings and preprocessed schemas. Their native mission
records retain 356, 496 and 220 bytes; Steele remains at offsets 40, 140 and 24.
The existing parser regression checks these contracts, final-field extents,
resource separation and saved-admiral correlation. The shell registration check
also verifies original briefing/start entries and empty saved-program records.
TM11 starts with `TM11_InGameScene`, not its later `TM11_MissionStart` program.
Native initialization resets each mission record and TM11's briefing descriptor.

The original `Mission.cpp` system-map admission and `MScript` load/briefing/string
owners accept the new module aliases, including the actual `SCRIPT11.dll`
spelling. Direct mission entry retains original effects, lighting and materials.
TM12's old MSVC loop-variable scope is expressed with a local `int x` in each
loop, preserving loop bounds, spawn order, timer behavior and RNG consumption.
No public interface or disk schema is widened; Windows branches remain viable
in source but have not been rebuilt here.

**Movie caller repair:** internal mission IDs are zero-based. Original outcome
case 8 is TM9 and plays `cq_2.mpg`, **Exchange of Technology**, the Terran/Celereon
peace film. Case 10 is TM11 and plays `cq_3.mpg`, **Traitor in the Midst**.
The formerly commented native calls now use original `MovieScreen`, respecting
`bSkipMovies`, and mark seen after playback returns. Both unlocked movie-selector
replay buttons use the same owner without modifying mission completion. The
Windows result behavior is preserved. No movie or other retail asset is copied
into the repository or the private binary candidate.

Validation on Linux x86_64, GCC 16.2.1 and Clang 22.1.8, full-media Debug and fatal
Clang ASan/UBSan configurations:

```sh
cmake --build "$BUILD" --target frontierx_native_shell frontierx_view_parser_probe -j 4
ctest --test-dir "$BUILD" --output-on-failure \
  -R '^portability\.(script(09|10|11|12)_schema|native_shell|layout|native_window_manager)$'
"$BUILD/frontierx_view_parser_probe" --mission-schema "$RETAIL/DATA" T_Mission10.dmission Script10.dll
"$BUILD/frontierx_view_parser_probe" --mission-schema "$RETAIL/DATA" T_Mission11.dmission SCRIPT11.dll
"$BUILD/frontierx_view_parser_probe" --mission-schema "$RETAIL/DATA" T_Mission12.dmission Script12.dll
python3 Tools/ci/test_ci_gates.py
```

The build directories are the private `p1c-clang-20261015`, `p1b-video-gcc` and
`p1b-video-clang-san` configurations; font mapping and reviewed shared-LGPL FFmpeg
remain external. Generated CPU fixtures exercise the real script/parser and
video/window lifecycle owners. The real external mission maps retain the
3236-byte global record, zero update count, exact module identity, music and
capability correlation. The optional explicit expected-script argument extends
the training-focused parser command without relaxing its exact identity check.
An initial TM11 invocation with `Script11.dll` correctly failed; the actual
external spelling is `SCRIPT11.dll`. Source caller regressions check both movie
outcome/replay branches. The technical sanitizer CI selection includes the three
new schema checks; local CI driver tests pass, hosted CI was not run.

A hidden GPU mission attempt was rejected by the existing visible-window
requirement before startup; it supplies **no mission runtime evidence**. No
scripted gameplay input or additional automated gameplay window was used.
Full TM10–TM12 briefings, scenes, cloak/regeneration/capture, late waves, combat
saves, victory/defeat and actual movie audio/video EOF still require user testing.
Native Windows Debug/Final, module ABI, hardware leak/resize and cross-platform
determinism checks remain pending. TM13 is not linked; TM12's next briefing is
outside this candidate's admitted closure.

The private pinned test candidate preserves the newest genuine saves and
progress without synthetic unlocks. It starts the original campaign selector;
TM10 is available from genuine TM9 progress, and TM11/TM12 unlock through normal
wins. The previously seen Celereon film can be replayed from its existing movie
button. Use F7/F8 during active orders and check the original result/movie/return
paths; complete gameplay is not inferred from these CPU checks.

## Current delivery boundary (2026-10-17)

The user explicitly accepts TM5, **Frontier Wars**, and TM6, **Allies**, as
working flawlessly on 2026-10-17. TM1–TM6 retain their individual reported
acceptance; no unreported scenario, Windows/ABI, hosted CI, process-leak or M3
result is inferred. The genuine TM6 quicksave and progress record are preserved
privately. The specific later battle shown in the user's screenshot has been
overwritten and is unavailable. Follow-up fighter textures, Mantis player
colors, laser-satellite weapon direction and a fighter-damage audit are tracked
below separately from mission acceptance. TM7 and TM8 are now integrated for playtest in original order. The TM9 entry
is linked as TM8's Continue dependency; its full gameplay remains unreviewed.
The current integration and validation evidence is recorded below.

## Earlier delivery evidence (2026-10-15 through 2026-10-16)

P1a, P1b and P1c retain their recorded user acceptance in the delivered Linux
scope. P2 starts by reconciling mission 2 against the complete per-mission
requirements. The user now reports that TM2 ran perfectly and identifies only
floating damage smoke on Corvettes. Record this as acceptance of the complete
reported mission run, with the narrow smoke correction and visible retest
tracked separately. No unreported scenario is invented and no later mission is
accepted by association. Full M3 comparison remains separate.

The original `Menu_mission.cpp` selector constructs `T_Mission1.dmission` through
`T_Mission16.dmission`; `Globals.rc` supplies the titles below. Per-script resource
titles contain development placeholders and are not used as campaign names.
The script column names the original production source owners. Native retail-module bindings through mission 4 and the next briefing are
recorded below; later gameplay remains subject to its own integration.

The subsequent AI build-site watcher crash and shared selection/sampler
optimizations are tracked in the [campaign crash record](NATIVE_TERRAN_CAMPAIGN.md#ai-build-site-watcher-crash-2026-10-16)
and [performance record](GAME_MATERIAL_PATH.md#large-selection-and-sampler-churn-2026-10-16).
They do not extend mission acceptance or replace the missing complete TM3 test.
The later Rally Point tooltip abort has a native font-owner repair with real
font-enabled checks. The reproduced selection-FPS defect has an upload-pool
retirement repair, genuine group-selection/F7/F8/close evidence and pending
user confirmation. A native saved-heading repair addresses the 180-degree
reload defect relevant to Weaver construction; its visible retest remains open.
Later user feedback confirms the other repairs and reports the remaining
Weaver construction tilt and missing Hawkes rescue film. Active-build root
animation restoration and the original film/replay callers now have focused
repairs and bounded runtime evidence. A subsequent screenshot exposed the
remaining root/model-axis mismatch; the corrected full-media build now shows
the same saved Weaver leaning toward the planet. See the [basis follow-up](NATIVE_TERRAN_MEDIA.md#weaver-screenshot-follow-up-2026-10-16).
TM3 remains pending narrow media
confirmation; the report does not invent unmentioned defeat or other scenarios.

## Mission matrix

Statuses are **untested**, **in implementation**, **ready for playtest** and
**accepted**. A blocker is recorded separately from status. Ready means a usable
technical candidate with explicit limits, not complete media or user acceptance.
Unknown requirements remain unknown until that mission's original entry and
later scripted/AI-created content have been reviewed.

| Mission | Script owner | Additional production components / review scope | Current blocker | Checked build | Status / user acceptance |
|---|---|---|---|---|---|
| 1. Uncharted Territory | `Script01T` | Existing campaign baseline | Carried technical follow-ups below | P1b/P1c recorded GCC/Clang builds | **accepted**, P1b/P1c user decision 2026-10-15 |
| 2. The Search for Hawkes | `Script02T` | Ballistics/Cruiser production, boarding/capture, Mantis waves, extension trees and saved AI/script state | Smoke, Collector-upgrade and repeated-F8 owner repairs; narrow visible retest and carried technical gates remain | `6242dc1` + P2/native repairs; GCC16.2.1/Clang22.1.8 full-media Debug and focused fatal sanitizers | **accepted**, user complete run 2026-10-15; subsequent concrete crash repairs retain their own bounded evidence, unreported scenarios remain unverified |
| 3. Prisoner of War | `Script03T` | Existing entry/schema; recurring four-Frigate production queues, waypoint triggers, Infiltrator tech, outpost/Troopship upgrades, Hawkes prison capture; Mantis AI/fighter combat; authored root build animation and rescue film | No outstanding user acceptance blocker; carried unexecuted technical gates remain | GCC/Clang full-media, fatal sanitizers, real active-Weaver F8/pose/close and actual rescue-film EOF; source caller binding check | **accepted**, user explicitly confirms complete mission and final Weaver repair 2026-10-16 |
| 4. Friend or Foe? | `Script04T` | Rebel Siphons, six systems, recurring waves and Alto ambush; Heavy Industry/Academy/Battleship; BlackHole terrain and RGB888 ring; Stasis/Tempest/KerTak | No outstanding user acceptance blocker; carried host leak/Windows/ABI/CI gates below | `a8f0cf7` rebuilt Clang full-media Debug; earlier GCC/Clang focused evidence | **accepted**, explicit user decision 2026-10-17 |
| 5. Frontier Wars | `Script05T` | Original module/schema/resources; dynamic ObjectGenerators, original KerTak admiral/fleet contract, Mantis production/AI, Blackwell sacrifice and gated systems | No mission acceptance blocker; shared TM6 feedback and carried technical gates below | GCC/Clang full-media Debug, focused fatal ASan/UBSan, real TM5 scene/catalogue and original menus/save-load | **accepted**, explicit user complete-run decision 2026-10-17 |
| 6. Allies | `Script06T` | Original linked module/resources and 440-byte schema; user complete run | Shared fighter texture/player-color/satellite feedback below; carried technical gates | Delivered full-media Clang TM5/TM6 candidate; GCC/Clang schema/shell checks | **accepted**, explicit user complete-run decision 2026-10-17 |
| 7. Supply Lines | `Script07T` | Original briefing/mission/events, Propulsion/AWS labs, Mantis AI counterattacks and Lemo HQ/supply route; 264-byte mission state | Full mission/media, active research/combat saves, defeat/victory and genuine TM8 transition await user | GCC/Clang full-media and fatal sanitizers; actual early F7/F8/close; original briefing Start | **ready for playtest**, no user acceptance |
| 8. Lost Sheep | `Script08T` | Original fourteen-system rescue, Benson timer, Mantis waves and Solarian reinforcements; 380-byte mission state | Late rescue/waves/media, active combat saves, full result/retry and genuine TM9 transition await user | GCC/Clang full-media and fatal sanitizers; actual early F7/F8/close; original briefing cancel | **ready for playtest**, normal unlock through TM7; no user acceptance |
| 9. Nine Lives | `Script09T` | Original next entry/module, 896-byte schema and array-expression repair; native Chrome/Solarian material owner and fixed-function/App shader-state isolation after real entry failure | Complete gameplay, late production/media and persistence coverage remain unreviewed | GCC/Clang material/shell/schema/layout and visible GPU checks; fatal sanitizer CPU scope | **in implementation**, focused crash-retest candidate; no complete gameplay acceptance |
| 10. Betrayal | `Script10T` | Original entry reached in user playtest; 356-byte native record, Solarian construction, debris markings, probe frame and Mass Disruptor repair | Exact repaired combat/build scene, active saves and full outcomes await user | Genuine crash core; GCC/Clang/fatal sanitizer CPU and generated GPU material regressions | **focused retest ready**, no complete mission acceptance |
| 11. The Traitor | `Script11T` | Original module/briefing/resources and 496-byte native mission record linked | Actual entry, full gameplay/media, active saves and outcomes await user; TM13 not linked | GCC/Clang and fatal sanitizer schema/layout/shell, real external map correlation and synthetic video lifecycle | **prepared for user entry**, no mission runtime acceptance |
| 12. Survival of the Fittest | `Script12T` | Original module/briefing/resources and 220-byte native mission record linked | Actual entry, full gameplay/media, active saves and outcomes await user; TM13 not linked | GCC/Clang and fatal sanitizer schema/layout/shell, real external map correlation and synthetic video lifecycle | **prepared for user entry**, no mission runtime acceptance |
| 13. Remember the Alamo! | `Script13T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 14. Super Weapon | `Script14T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 15. Major Offensive | `Script15T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 16. Final Countdown | `Script16T` | Not yet reviewed; original campaign ending must execute | Native script/schema/resource owner and campaign-ending runtime unverified | None | **untested** |

## Ownership and delivery order

The main agent owns native composition, build integration, this matrix, the active
plan and the user handoff. The gameplay agent owns script/gameplay owners and
their persistence contracts. The media agent owns rendering, audio/video and
the focused media evidence. The validation agent owns existing CI/playtest
tooling and independent focused checks. Shared headers, composition and CMake
changes require coordination through the main agent before edits. No concurrent
agent writes the same owner or build directory.

Finish one usable mission candidate at a time in original order. While a user
playtest is pending, prepare the next mission technically; never mark pending
feedback accepted. A shared repair gets targeted training/accepted-mission
regression checks rather than an automatic full campaign replay request.
Retain the earlier reported complete TM2 run and save/load evidence. This
reconciliation asks for missing scenario checks and any reproduced regression,
not an automatic full TM2 replay merely because P2 has started.

## TM2 original-owner acceptance contract

**Source review:** `Script02T/Script02t.cpp` owns original briefing, opening,
system visibility, construction objectives, capture, repeated attacks and
result conditions. The existing native binding must execute this owner, not a
mission-specific substitute. Script02 resource/schema/layout execution and
bounded genuine-checkpoint runs are recorded in
[the integration ledger](NATIVE_TERRAN_CAMPAIGN.md).

The user checklist must cover:

1. Select unlocked mission 2 normally, play briefing and opening, and inspect
   original starting units, objectives, camera and HUD.
2. Construct Ballistics/Cruisers and use boarding/capture with original orders,
   supply, navigation and system transitions. Check later Frigate waves, Scout
   Carriers and their fighters, and Zorap supply ships as they occur naturally.
3. Save/load early and during production, boarding and later combat. Check
   objectives, queues, orders, supply/unit state and continued scripted waves.
4. Inspect models, attachments, animation, materials, backgrounds, lighting,
   effects, both maps and their interactions. Recheck map visibility/connections,
   portrait speaker changes/clear and sound after loading and switching missions.
5. Reach original victory, result/Continue and TM3 briefing/start; verify original
   progress after process restart. Exercise genuine defeat/retry separately,
   without adding victory progress. A retained real late checkpoint avoids
   repeating the complete successful run for the separate loss check.

The construction gate counts six Corvettes and three Cruisers. Later technology
enables Troopships and their capture research. Original victory requires at
least one Thripid, Cocoon, BlastFurnace or Eyestock under player ownership. Destroying
both Mantis Eyestocks or losing Blackwell triggers original defeat. The script's
initial attack enters its `Wave2` state directly; the unused `Wave1` branch is
not an additional wave to implement. These are source-review facts from
`TM2_BuildCruisers`, `TM2_MantisAttack`, `TM2_MantisBase`,
`TM2_ObjectBuilt`, `TM2_ObjectDestroyed` and `TM2_MissionSuccess`, not new
runtime evidence. Save/load checks must include capture research, pods in flight,
enemy production queues and cross-system orders when real checkpoints exist.

All required presentation and media remain mandatory, including later-created
content. No content-reducing launch flags qualify as final acceptance. A strict
full-media launch requests these owners but does not prove their completeness.
The gate includes submodels, attachments, turrets and animations; all required
texture formats, mips/stages, materials, shader techniques and passes; authored
lighting/backgrounds; projectile, impact, explosion, shield, drive, particle and
ability effects; complete HUD/text/icons; original visibility and interaction
on both maps; animated selection and speaking portraits with switch/clear;
speech, music, spatial sound, briefing media and cutscenes. The same content
must work after loading and changing missions.

## TM2 acceptance and smoke follow-up (2026-10-15)

The later reported Collector-upgrade and repeated-F8 crashes are separate
concrete regressions. Native repairs restore initialized extension-tree ownership
and isolate AI disk padding from live subscription tokens. Keep the actual
checkpoint execution and remaining limits in
[the focused crash ledger](NATIVE_TERRAN_CAMPAIGN.md#collector-upgrade-and-repeated-f8-crash-repairs-2026-10-15).

The user reports a perfect TM2 run, with a small damage-smoke position defect.
The original damage render owner now maps the raw extent anchor through the
visible hull's axis correction while preserving the plume orientation. Existing
material regression fails with the old placement and passes with the repair;
GCC/Clang shell/material builds and focused checks, plus fatal Clang ASan/UBSan,
pass. The exact scope and commands are in
[the smoke evidence](GAME_MATERIAL_PATH.md#corvette-damage-smoke-anchor-2026-10-15).
Only the corrected visual contact needs a short user check. The next campaign
mission is TM3; this acceptance does not establish its completion or invent
unreported TM2 defeat/retry/resource-growth evidence.

## Initial TM2 candidate evidence (2026-10-15)

**Compilation and synthetic automated execution:** Linux x86_64 GCC16.2.1 and
Clang22.1.8 full-media Debug builds at `/tmp/frontierx-p1c-gcc` and
`/tmp/frontierx-p1c-clang` complete. Both execute existing `layout`, `data_root`,
`view_parser`, `app_schema`, `script01_schema`, `script02_schema`,
`script03_schema`, `sol_t_schema`, `net_buffer`, `game_material_manager` and
`native_shell` checks without skips. The same focused selection passes under
Clang ASan/UBSan with fatal errors and leak detection outside the sandbox. The
initial sandbox run fails LeakSanitizer process inspection and is not a pass.
No suppression or assertion change is introduced. Exact build/check commands
and the playtest-tool regression are in the
[CI evidence](LINUX_PLAYTEST_CI.md#p2-mission-handoff-and-focused-validation-2026-10-15).

The local candidate requests native fonts, audio and video with the reviewed
external decoder and strict original-family map. Private handoff records pin
the revision plus local changes, binary/cache/shaders/fonts, original external
TM2 checkpoint checksum, build path and exact start command. Save/config roots
are isolated copies; original progress and the immutable checkpoint are unchanged.
The existing `playtest.py --mission terran1 --campaign-mission 2` launches the
original selector. The new target argument is checklist/provenance only and
does not select, unlock or complete a mission. A different target is rejected
when checking the candidate record.

**No new GUI mission runtime:** desktop approval was canceled before the new
smoke ran. Existing P1c bounded genuine-runtime evidence remains applicable to
the unchanged production owners, with its recorded limits. This candidate adds
no complete mission/media acceptance and no fresh early/late gameplay evidence.
The user handoff contains the exact local start script and a targeted checklist;
missing scenario checks may use real checkpoints while retaining the earlier
reported complete TM2 run. Required media remain enabled and unverified late
content remains an acceptance gate.

## Carried limits

The existing [unit coverage ledger](NATIVE_UNIT_COVERAGE.md) records failed
whole-catalogue completeness, rejected retail flagship contracts, missing model
inputs and unsupported Wormhole persistence. Determine actual mission use before
calling these a blocker of an individual mission; do not silently omit them.
Recorded whole-process leak failures, ordinary compositor resize, hosted CI and
native Windows Debug/Final/module ABI remain open. Focused clean sanitizer checks
do not erase whole-process failures. P1 acceptance does not create missing
scenario evidence. Detailed history remains in the
[campaign](NATIVE_TERRAN_CAMPAIGN.md), [media](NATIVE_TERRAN_MEDIA.md) and
[CI](LINUX_PLAYTEST_CI.md) ledgers.

P2 closes only after all 16 mission rows have complete original gameplay,
save/load, presentation/media and progress evidence, explicit user acceptance,
and the original final campaign ending. Skirmish and multiplayer are outside
this delivery scope. No commits, pushes or publication are authorized.

## TM4 production integration (2026-10-16)

**Source review and compilation:** the original `Script04T` module, generated
Win32-shaped schema and string resources now enter through `Script04.dll`, the
original selector and `TM4_Briefing`/`TM4_MissionStart`. No objectives, mission
timers, wave order, RNG calls or authored victory/defeat predicates are replaced.
The fixed mission record remains 436 bytes, including all 64 exit IDs. The
separate 604-byte Script05 record/resources are linked for the next briefing;
this does not establish TM5 gameplay readiness. Its actual uppercase
`SCRIPT05.dll` header has an explicit native alias.

The required BlackHole terrain now uses its original production factory,
billboard/morph/animation, ring and map-icon owners. Required resource failures
reject construction and release partial resources; native briefing Start also
propagates a failed mission reload instead of continuing a partial mission.
BlackHole full/base/quick save layouts remain 180/36/20 bytes. Windows paths,
public interfaces and persistent formats remain unchanged; native Windows
Debug/Final and affected ABI execution are pending.

The real ring requires an embedded RGB888 texture with separate Alpha8 and
only its authored mip. The TextureLibrary admission and native upload boundary
now decode that exact RGB24 mask/stride contract into RGBA8. Existing material
regressions check both rows, separate alpha, rejected short stride/malformed
alpha, retained previous uploads and single release. Existing visible GPU
checks compare every RGB readback byte against an independently uploaded RGBA8
reference, then require an opacity change and exact restoration. GCC and Clang
host `portability.sdl3_gpu` and `portability.game_material_gpu` both execute
without skips. No missing texture is replaced by a successful placeholder.

**Bounded retail runtime:** original mission selection, authored TM4 briefing
and Start execute on external retail data. Ordinary F7/F8 creates and restores
a genuine opening TM4 checkpoint. A later ordinary save contains natural Rebel
loss/damage and advancing source-script state. The unattended run then reaches
an original defeat and retry briefing; a second requested F8 did not execute a
second successful reload. No victory or progress was manufactured. The first
run exposed the missing BlackHole provider and later ring-format errors; these
are failed intermediate presentation evidence. A fresh RGB888 build restores
the genuine early checkpoint without those texture/component failures and
closes normally. Active BlackHole transit and its mid-transit save/load,
late waves/abilities, victory and actual Continue remain user checks.

**Asset/source scope:** all seven newly selected production/technology roots
load in the existing external catalogue diagnostic. The whole diagnostic still
fails two other catalogue roots and is not a global completeness pass. Selected
terrain dependencies, direct speech, music, KerTak portraits and briefing art
are present; presence/source review does not prove every late render or sound.
No TM4-specific fullscreen film is authored. Both original minimaps, animated
portraits and all media remain enabled and mandatory for user acceptance.

**Automated execution:** full-media Debug `frontierx_native_shell`,
`frontierx_view_parser_probe`, `frontierx_layout_probe`,
`frontierx_game_material_manager_probe` and `frontierx_sdl3_gpu_probe` build on
Linux x86_64 with GCC16.2.1 and Clang22.1.8. Existing layout/parser/app,
script01–05/Sol_T schemas, data-root/net-buffer, shell and material checks pass
under both compilers and fatal Clang ASan/UBSan with leak detection. Inputs are
synthetic/repository-owned except explicitly external font and bounded retail
runs. These focused passes do not erase the carried whole-process leak/resize,
hosted-CI, Windows/ABI or separate M3 gates. Final candidate provenance and
mission-specific user checks remain separate from these technical results.

Shared-change regressions also execute the accepted bounded training scope and
full-media TM1 through ordinary F7, F8 and close; both restore their real save
and release mission audio/hint ownership normally. A standalone original TM5
briefing completes all authored objective lines and closes with speech and
portraits enabled after the counted Teletype repair. Exact bounded text spans,
empty rows, CRLF and final LF are checked by the existing font-enabled owner
lifecycle, including a non-NUL-terminated allocation under sanitizers. This
does not substitute for TM4's actual victory/Continue path.

The private TM4 candidate uses an immutable local runtime binary/shader snapshot
of the Clang full-media build and independent save/config roots. Its original
selector target is metadata only; copied earned progress and the latest user
quicksave remain byte-identical at their source. Two genuine technical TM4
checkpoints are retained separately without replacing that user quicksave.
The handoff includes exact start command, revision/configuration and dependency
fingerprints, notices and a short mission checklist. Provenance/launch-expansion
preflight executes; it is explicitly not game runtime evidence. The concrete transit findings below are repaired; the host leak gate remains
open. TM4 is **ready for playtest**, not accepted. TM5 remains transition preparation only.

## TM4 genuine transit checkpoint sanitizer finding (2026-10-16)

A fresh full-media fatal ASan/UBSan run loads the genuine opening checkpoint,
then fails at `Sector::GetSectorEffects`: index 128 exceeds its 16-system row.
The actual checkpoint contains seven Gunboats with system ID 129, the original
`1 | HYPER_SYSTEM_MASK` transit encoding. The stack is Gunboat weapon range,
AirDefense target search and ordinary launcher update. This is a failed runtime
check, not a clean sanitizer pass or a corrupted-save workaround. The original
warp owner intentionally creates that tag; fixed save formats must preserve it.

The original terrain owner routes transit to its separate hyperspace map. It
provides no basis for applying the destination system's buffs before arrival.
Skipping launcher updates would change refire/update behavior. The narrow
native correction therefore belongs at the Sector effects domain boundary:
valid transit gets neutral effects, real system effects remain unchanged, and
arbitrary invalid IDs are not silently mapped. Neutrality is inferred from the
existing source domain, not measured Windows retail parity. The existing real Gunboat/Sector boundary regression now covers first/last
player and real/transit IDs, real-system buff preservation and isolation from
the API's writable return. The subsequent fatal sanitizer rerun and remaining host leak failure are
recorded below.

The first repaired genuine runtime passes the Sector lookup and resumes its
original transit sequence, then exposes a second fatal conversion in
`Trail::PhysicalUpdate`: the final fade crosses zero before the later `Update`
removes the trail. Converting that negative length into the U8 section count
is undefined. Native Trail now zeroes only terminal derived render geometry;
original length, source position, fade, counter and removal ordering remain.
Its existing no-draw condition already excludes zero/negative length. The
existing shell lifecycle executes the actual Trail owner through positive,
exact-zero and negative terminal phases, checking unchanged update/removal
semantics and no backend access for invisible terminal geometry. No new probe
target, saved field, RNG or Windows branch changes. Both failed runtime checks
remain documented; neither is reported as a clean whole-process run.

**Final repaired genuine runtime:** the same unmodified checkpoint restores
under fatal ASan/UBSan, resumes seven saved transit ships and subsequent
scripted jumps, and reaches ordinary close with zero mission audio streams and
idle hint ownership. Neither earlier Sector/Trail failure recurs. LeakSanitizer
then reports 3,052 bytes in six DBus allocations, including the SDL PipeWire
audio-init stack and an unresolved host-module stack; process exit is 1. This
is consistent with the carried host-library leak boundary and remains a failed
whole-process leak gate, not a clean sanitizer pass. No suppression or leak
check disabling is used. No new BlackHole/Trail allocation appears in that
report. The final focused owner checks pass with leak detection under both
normal compilers and Clang sanitizers after correcting their pre-world Sector
composition. Full Windows/ABI, hosted CI and complete mission gameplay remain
unperformed/pending as recorded above.

**Final normal-build confirmation:** the genuine later TM4 checkpoint restores
through ordinary F8 in the final Clang full-media build, continues normal
mission simulation/combat alerts and both visible maps, and closes with exit 0,
zero mission audio streams and idle hints. No required texture/component failure
is logged. After the Sector/Trail changes, the bounded training and full-media
TM1 ordinary F7/F8/close checks are repeated and pass. The final TM4 snapshot
passes actual fingerprint/input and launch-expansion preflight; source user
progress/quicksave hashes remain unchanged. These bounded checks establish a
usable user candidate, not late-game, active-ability or complete media acceptance.

## Reported TM4 attack volume audit (2026-10-16)

The user reports excessive Mantis attack volume. Read-only source review and
minimal functional checks against the lawfully external retail Script04 module
find the same wave parameters: Bekka selects up to three existing Scout
Carriers every 240 script updates; Luxor selects up to two existing Frigates
every 120 updates and adds one existing Scarab on alternating dispatches.
These early programs do not create hulls. At the native normal-speed cadence
of 8/30 seconds per script update, those intervals are approximately 64 and
32 seconds. The source's minute-based comments do not describe the actual
constants. Retail DLL agreement establishes the script parameters, not a newly
measured retail executable wall-clock comparison.

After the Alto event, the original script and retail module both create a
20-ship fleet when its 60-update timer expires and fewer than 20 Mantis units
are already in Perilon. That fleet contains seven Frigates, four Scarabs, five
Scout Carriers and four Hive Carriers. Ships still travelling are excluded
from the Perilon count, so multiple fleets can accumulate before arrival.
The separate retreat check replenishes when global Mantis count falls below
eight. No timer or balance edit is made on the basis of this report.

The user's subsequent genuine F7 checkpoint is independently decoded and
compared with the original external mission map: phase `TalkWithRebels`, zero
dispatched waves, only ControlProgram and TalkWithRebels active, no duplicate
wave programs, no active player AI or queued construction. All 28 initial Scout
Carriers and nine initial Scarabs remain in their origin systems; 25 of the
27 original Frigates remain, after the two opening Perilon Frigates are killed.
No additional Mantis hull exists and none is yet in Perilon in this checkpoint.
It therefore predates the reported overwhelming attack and cannot establish
or rule out a later dispatch, fighter or transit defect. Preserve the report
as pending a genuine checkpoint during the excessive attack; do not treat this
audit as user acceptance or proof that all later waves behave correctly. Raw
retail analysis, checkpoint and unit/queue records remain private outside the
repository. No game code, saved state or campaign progress was changed.

## TM5 production integration (2026-10-17)

**Source review and compilation:** TM5 uses the original `Script05T` module,
604-byte mission schema and resources. Its `TM5_ObjectConstructed` caller starts
both original generator programs after the first player platform completes.
`TM5_SendMantisWave1` force-generates one Scout Carrier and two Hive Carriers;
`TM5_SendMantisWave2` force-generates one Frigate and two Scarabs. The variable
names and comments do not change those actual calls. Original timing, RNG,
command limits, location checks and enemy orders remain in the source owners.
The native composition now links the production `ObjectGenerator` factory.
Its render initialization record belongs to the factory archetype and outlives
instances. Full/base/quick save extents remain 168/24/60 bytes, with checked
correlation and transfer errors rather than accepting incomplete records.

`TM5_IntroduceKerTak` creates the original `FSHIP!!M_KerTak` later in the mission.
The native Flagship owner now retains its decoded 744-byte retail archetype for
IAdmiral bonuses, passive traits, toolbar metadata and original hotkey. Favored
ship bonuses replace general bonuses. Shields enter the existing damage owner;
retail admirals expose no CQ2 command-kit research. Original AdmiralBar and
Fleet menu owners are linked, including authored retail faction ability buttons.
No public IAdmiral slot, save field or Windows gameplay branch is widened.
Native Windows Debug/Final and affected module ABI execution remain pending.

Original in-game options and save/load menus now run through the existing
MenuResource callers. Save descriptions decode the fixed UTF-16 global record
without loading or changing mission state. The existing shell regression writes
a physical synthetic UTF archive and checks the full description, bounded copy
and zero-capacity behavior. Native rejected saves remain in the dialog;
rejected loads return out of the closed world. General settings remain disabled
because their native owner is not integrated. Campaign/mission return is wired
through the original selector; source wiring alone does not prove all flows.
Script06 module, resources and 440-byte schema are linked for TM5's next entry,
without claiming TM6 gameplay readiness.

**Automated execution:** Linux x86_64 Clang22.1.8 full-media Debug in the existing
`p1c-clang-20261015` build and GCC16.2.1 full-media Debug in `p1b-video-gcc` build
`frontierx_native_shell`, `frontierx_layout_probe` and
`frontierx_view_parser_probe`. Both execute this exact focused selection without
skips:

```sh
ctest --test-dir BUILD -R '^portability\.(native_shell|layout|view_parser|script0[1-6]_schema|app_schema|sol_t_schema)$' --output-on-failure
python3 Tools/ci/test_ci_gates.py
```

The checks retain original generator extents, retail favored-bonus selection,
Script01–06/Sol_T schema/resource/reset contracts and physical save-description
bounds. Generator records round-trip their countdown, generated archetype and
enabled bit through the production loader; short/oversized inputs leave the
output unchanged. The CI driver tests include TM5's checklist and target provenance;
`--campaign-mission 5` does not choose or unlock a mission. The first resumed
build exposed missing native includes and an invalid UTF-writer call in the
unfinished save-description check. GDB then identified reuse of its already
released writer. Reacquiring the archive's writer before flushing resolves that
failure; the final focused runs above pass. No assertion was weakened. The same focused selection also passes in the
full-media Clang ASan/UBSan build with
`ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. This is focused synthetic
execution with an external font, not a whole-process GPU leak pass.

**Bounded retail runtime:** the full-media Clang shell opens the actual external
`T_Mission5.dmission`, selects `SCRIPT05.dll`, presents production object frames,
advances the source mission loop and closes with exit 0, zero mission audio
streams and idle hints. The separate catalogue diagnostic executes actual
IAdmiral bonus/passive queries and loads KerTak plus every listed ship. Its
whole result remains exit 6 because `PLATGUN!!T_DefenseBattery` and
`PLATBUILD!!M_AntennaBeacon` lack their model inputs. These are not a global
catalogue pass. The private TM5 reference audit reports no missing selected
archetype/model roots; references alone do not establish late media rendering.
The ordinary campaign run renders the original campaign/mission selector and
TM5 gameplay with both maps and an animated Blackwell portrait. Additional
manual inputs arrived during this run; do not attribute unobserved menu,
save/load or gameplay scenarios to the bounded automated smoke.

Complete generator-wave gameplay, KerTak fleet/ability behavior, early and
active-state normal save/load, pause/cancel/retry/return, victory/progress and
actual TM6 transition remain runtime acceptance checks. No victory bit or
later checkpoint is manufactured. Complete gameplay and media acceptance,
whole-process host leaks, hosted CI and native Windows/ABI remain separate
gates. The user confirms they are operating the live TM5 test window. It stays
open and receives no further automated input during the user run. The user
subsequently confirms that its regular close was manual.

**Private candidate:** a fixed full-media Clang Debug snapshot at revision
`a8f0cf7` plus the recorded working changes uses `--mission-campaign`, strict
external font families and reviewed external audio/video dependencies. Its
manifest pins the binary, configuration, shaders, fonts and changed source
files. The genuine TM4 progress record is copied without modification; no
quicksave from another mission is installed as the TM5 F8 save. Launch-command
preflight verifies original selector entry and absence of media exclusions.
Pending user scenarios above remain checks, not passes.


### TM5 user feedback and menu repair (2026-10-17)

The user clarifies that the first live window was closed manually, consistent
with its exit 0 and regular resource release. No crash is attributed to that
close. A subsequent actual in-game menu click does abort. GDB core inspection
identifies the title static's `APP_NAMETM` resource. Its source contains a UTF-8
replacement sequence, decoded by the CP1252 resource path as three invalid
characters. `Globals.rc` now uses the explicit CP1252 trademark escape; the
native font admits that specific Unicode glyph and still rejects unrelated
unsupported characters. The existing actual font regression checks the exact
source title, added width and visible trademark spans. General Unicode support
is not claimed. The shared source resource changes Windows title bytes too;
native Windows validation remains pending.

The next genuine load-menu run exposes a second abort: `Listbox::GetString`
calculates a native wchar count but copies only two bytes per character. GDB
shows the selected save description followed by uninitialized stack characters
in `Edit2::SetText`. Copying `sizeof(wchar_t)` bytes per character fixes the
owning function; the Win32 two-byte copy remains identical. No font rejection,
validation or error is suppressed to hide the invalid text.

**Bounded retail runtime:** ordinary selector entry, briefing cancel/re-entry
and Start execute with all media enabled. F7 writes a real opening checkpoint
and F8 restores it. F10 creates the genuine named `TM5 menu regression`
checkpoint. In the final Listbox build F11 shows its complete description in
both list and edit field, Return restores the checkpoint, source gameplay
continues, and F9 opens the original options menu again without either font
abort. Resign returns to the original mission selector; Back/Back then exits
with code 0, zero active mission audio streams and idle hints. The genuine
completion bits remain 11 before and after; only the normally viewed TM5 seen
bit is added. Complete active-generator/KerTak save-load and full mission acceptance
remain separate. The reported Battleship aiming/shot defect has its native
owner repair and numerical regression in
[the weapon-frame record](GAME_MATERIAL_PATH.md#tm5-jointed-battleship-weapon-frame-2026-10-17);
visible user confirmation remains pending.

After turret/title changes, the existing focused selection adds
`portability.game_material_manager` to the eleven checks above. It passes with
GCC/Clang and fatal ASan/UBSan. After the one-line Listbox repair,
`frontierx_native_shell` is rebuilt and its existing lifecycle CTest passes in
all three builds. The precise Listbox failure and repair are exercised by the
actual named save/load path above, not inferred from that synthetic lifecycle.


## TM9 Chrome/Solarian closure repair (2026-10-18)

The refreshed candidate reaches original Script09 and exits with the explicit
unsupported chrome/Solarian material exception. Successful beacon-resolution
logs distinguish it from the repaired audio failure. The latest genuine
quicksave and progress are secured; the save is still late TM8 and the genuine
progress unlocks TM9. No unsaved TM9 checkpoint is invented.

The native FaceGroupRender owner now submits original update-driven chrome
normal coordinates through the checked legacy path, retaining lighting/colors,
UV1, emissive behavior and source effect timing. Fixed-function RPVERTEX2 draws
also receive neutral UV/color uniforms when interleaved with App effect c30/c31.
Existing CPU and visible GPU probes protect the real caller, rotation, complete
cycle, state isolation and continued App effect/other render contracts. GCC/Clang
and fatal sanitizer CPU checks pass; both compiler GPU checks execute without
skips. See the [exact owner and validation record](GAME_MATERIAL_PATH.md#tm9-chromesolarian-production-material-path-2026-10-18).
A separate pinned full-media candidate targets original TM9 selection and retains
both saves. Full TM9 gameplay/media acceptance and native Windows/module ABI
remain pending. This repair does not claim full TM9 integration or TM10 readiness.

## Exact Kalidon beacon restored for retest (2026-10-18)

The user explicitly requests restoration of the cue. The native PlayAudioMessage
owner now selects the exact existing SFX entry only for the authored beacon name
when MSpeech lacks it, retaining mission-speech overrides and explicit exclusion.
No other missing speech is redirected. Generated-PCM lifecycle checks pass under
GCC/Clang and fatal sanitizers; the real external 8-bit PCM cue also completes
through IStreamer/PipeWire in approximately one second. See the
[exact audio validation](NATIVE_AUDIO_STREAMER.md#exact-kalidon-beacon-restoration-2026-10-18).
This supersedes the earlier missing-beacon presentation gap. The separate refreshed
candidate carries the genuine user checkpoint for F8 continuation. Actual
in-mission audible confirmation and native Windows/module ABI remain pending.

## User-run Kalidon beacon abort (2026-10-18)

The user reports a late mission-9 closure. The actual log and retained latest
quicksave identify `SCRIPT08.dll` / TM8, with failure opening the recurring
Kalidon beacon's `specialDenied.wav` through MSpeech. Keep those facts distinct
from the user's ordinal. Both quicksaves and campaign progress are preserved.
The native sound owner now returns the original Windows playback failure ID
zero, reports and counts the missing entry, and retains subsequent playback.
Existing generated-PCM lifecycle checks fail before and pass after the repair
under GCC, Clang and fatal sanitizers; script08/09 schemas and layout also pass.
See the [audio owner and exact validation](NATIVE_AUDIO_STREAMER.md#kalidon-beacon-audio-open-abort-2026-10-18).
A refreshed full-media candidate carries the user's latest actual TM8 state for
F8 continuation. The unavailable beacon cue remains an explicit presentation
gap; archive selection and authored script are unchanged. No full TM8/TM9 or
unreported TM7 acceptance is invented from this crash report. Genuine continued
Kalidon runtime and Windows/module ABI checks remain pending.

## Renewed TM6 completion and TM7/TM8 handoff (2026-10-18)

The user reports that TM6 runs completely through and asks to proceed directly
with TM7 and TM8. Retain complete TM6 acceptance and the confirmed fighter-trail
repair. Do not infer a separately reported Battlecruiser special-weapon retest
from this general completion statement. Its technically validated repair is
included in the refreshed full-media TM7/TM8 candidate.

The refreshed candidate pins the latest Clang build, configuration, shaders and
working-source overlay, retaining the accepted original TM6 victory progress.
TM7 starts through normal campaign selection; TM8 follows original TM7 victory.
The genuine early TM7 F8 checkpoint and separate TM8 diagnostic checkpoint remain
available without modifying progress or inventing an unlock. Both complete
mission playtests remain pending. The original candidates and user saves are
preserved independently.

## Battlecruiser special-weapon follow-up (2026-10-18)

The user confirms the fighter-trail correction works, then reports a crash on
Battlecruiser special-weapon activation. The actual candidate log identifies
AreaEffectBolt's asserted RPVERTEX2 triangle-strip draw. The native SDL owner
accepted only triangle lists for this two-UV layout. It now expands direct and
indexed strips through the same complete-vertex, alternating-winding path used
for the existing one-UV Beam caller. Weapon damage and gameplay are unchanged.
The existing GPU probe fails before the repair and passes after it, including
exact readback through the production 42-vertex PIPE buffer path. Full-media
GCC/Clang and fatal sanitizer CPU/shell checks pass; synthetic GPU execution is
separate from pending genuine special-weapon retest. See the
[owner and exact validation](GAME_MATERIAL_PATH.md#battlecruiser-special-weapon-strip-submission-2026-10-18).
The new private candidate preserves the same genuine checkpoint and full media.
Native Windows/module-ABI and carried host gates remain open.

## TM6 visible fighter-trail defect repair (2026-10-18)

The user reproduces the remaining bright silhouettes in a manually controlled,
unbounded full-media session, leaves the game paused and saves with F7. The
new checkpoint is secured separately from the earlier copy. A read-only capture
of the real 373-draw frame locates an affected engine-trail submission during
alpha-pool eviction with additive ONE/ONE blending. Authored Terran hull texture
bytes and all mip levels are intact. The user also identifies the trails as the
likely visible problem; this is diagnosis, not fixed-build acceptance.

The original RenderBatcher now preserves pending material/modelview state while
allocation submits an older cached draw. It keeps the three-entry pool and
original draw order. The existing production batcher regression fails before
and passes after this correction, proving four distinct queued effects retain
their own blend/depth states across eviction. GCC/Clang full-media shell builds,
focused batcher/header/layout/shell execution and fatal Clang ASan/UBSan pass.
See the [owner and exact validation record](GAME_MATERIAL_PATH.md#tm6-fighter-trail-material-eviction-2026-10-18).
A separate pinned full-media candidate carries the new visible-defect checkpoint
for F8. The user confirms the trail repair works; no complete mission replay
is required. TM7/TM8 readiness and earlier mission acceptance remain unchanged.
Native Windows Debug/Final/module ABI and existing host leak limits remain open.

## TM6 feedback confirmation and retained fighter-texture defect (2026-10-17)

**User gameplay evidence:** the user tests the separate TM6 feedback candidate
while TM7/TM8 integration proceeds and reports that everything except fighter
textures works well. This confirms the reported player-color and laser-satellite
repairs in that run. It does not accept TM7/TM8 or add a measured damage/DPS
comparison. The primary-UV restoration is a valid state-contract correction but
has not resolved the reported intermittent texture appearance; retain the defect
as open instead of treating the earlier synthetic check as a visual pass.

The current TM6 `f0000000.mission` differs from the installed opening checkpoint
and is copied to a separately fingerprinted private diagnostic directory before
any reload. The original candidate, quicksave and progress remain untouched.
The user does not need to recreate that checkpoint. A separate read-only GDB
run loads its copy via normal F8; the user then manually launches fighters in
the analysis window. Attribute that launch to the user, not automated gameplay.
Factory samples
show valid legacy face groups and nonzero fighter texture handles, with no CQ2
material promotion in the sampled Mantis/Terran meshes. Thus neither the earlier
UV-state test nor an assumed promoted-material/uninitialized-texture path proves
the remaining root cause. The bounded GPU observer also captures the expected
Terran fighter texture handle at actual indexed submissions, primary UV0,
white vertex diffuse, finite authored UVs and 24-byte vertices. These samples
rule out assumed missing bindings for those draws; they do not establish the
visible appearance or remaining root cause. The timed diagnostic windows close
regularly; no crash is attributed to that timeout. After the user confirms live
control, owned input automation is stopped and completed controllers are reaped.
Further evidence is required before another repair or visual-success claim.

## TM5/TM6 acceptance and shared rendering feedback (2026-10-17)

**User gameplay evidence:** the user confirms both missions work flawlessly,
then reports intermittent untextured fighter silhouettes, authored-looking
friendly colors on Mantis Weavers/Siphons and some buildings, a wrongly aimed
laser satellite, and apparently weak Terran fighters. This accepts the reported
mission runs, not every unmentioned validation scenario. The exact screenshot
battle save was accidentally overwritten. No equivalent battle is fabricated.

**Source review and implementation:** native direct Engine SpaceShip/Platform
rendering consumes legacy mesh materials, while `ObjectTeam::SetColors` only
updated CQ2 MeshManager modifiers. The native render-state owner now applies
the original markings diffuse/specular/emission policy through the existing
`IRenderer::get_unique_instance_mesh` owner, recursively over compound children.
The archetype and other players' instances remain shared and unchanged until
their own local copy is requested. Gameplay player IDs, alliances, authored
textures, Windows paths, persisted data and public module interfaces do not
change. Weaver/Siphon/building assets contain these markings; this identifies
a concrete rendering mismatch, not a gameplay allegiance defect.

`SetupDiffuseBlend`, called by both Fighter render branches, now explicitly
selects primary UV0. Existing deform/multitexture callers can leave UV1 active;
RPVertex fighters provide only UV0. The existing material check seeds UV1 and
requires the real setup to restore UV0 while preserving texture/sampler state.
This is a reproduced state-contract defect. The exact intermittent screenshot
appearance remains unverified without the overwritten battle state.

`Platform::initPlatform` installs Rx90 before the original transform loader.
The earlier native ship-yaw repair reset this frame for platforms as well,
turning horizontal model -K weapons toward world Z. Native platform loading now
preserves its initialized rest orientation and applies the original saved-yaw
rotation, leaving ship reconstruction unchanged. Actual laser-satellite model
review identifies model-Y revolute joints and a model -K muzzle, consistent
with this root-frame defect. Turret angular speed, joint constraints and
projectile rules are unchanged. The numerical regression checks five saved
headings, horizontal muzzle rays and unchanged translation; the existing ship
and jointed Battleship regressions remain passing. The later user feedback above confirms the reported satellite correction;
this does not add unmentioned platform or cross-build scenarios.

**Damage audit, source and external-data review:** original Terran Fleet and
Takai fighter projectiles contain damage 6, velocity 8500 and fighter refire
period 0.6. Station fighters use damage 3 and refire period 0.1. The production
`Fighter::updateWeapon` reads those original weapon records and queues the same
damage through `EventScheduler`, which dispatches it to the existing target
damage owner. Supply, aiming/range, shields/armor and flight-cycle behavior
remain original. No balance or damage expression is changed. This audit finds
no specific damage regression; it is not a measured TM6 battle DPS comparison.
The read-only TM6 observer records no fighter firing samples and therefore
adds no damage-runtime evidence.

**Compilation and synthetic automated execution:** Linux x86_64 Clang22.1.8
full-media Debug (`p1c-clang-20261015`), GCC16.2.1 full-media Debug
(`p1b-video-gcc`) and fatal Clang ASan/UBSan (`p1b-video-clang-san`) build
`frontierx_native_shell` and `frontierx_game_material_manager_probe`. The exact
focused shell/material/layout/parser/script01–06/App/Sol_T CTest selection in
the TM5 record above passes without skips, with an external font. Sanitizers
use `detect_leaks=1:halt_on_error=1` and `halt_on_error=1:print_stacktrace=1`.
The material check executes real Engine/RenderMgr unique meshes, two distinct
player colors, recoloring isolation, preserved texture handles and unchanged
archetype colors; it also checks UV-state restoration and platform weapon rest
frames. The explicit Clang `portability.game_material_gpu` runs on SDL/Vulkan
through the existing Hyprland/X11 host setup and passes actual fixture readback;
this is synthetic GPU evidence, not the user's combat image.

**Bounded retail runtime:** the repaired full-media Clang shell opens actual
TM6 with a private genuine quicksave available. Normal F8, pause/resume
and F9 inputs are sent to the owned test window; the bounded source loop closes normally with zero active
mission audio streams and idle hints. No full battle, exact texture comparison,
player-color image comparison or satellite combat pass is inferred. Native
Windows Debug/Final/module ABI, hosted CI and whole-process host-library leak
checks remain unexecuted/open as previously recorded.


**Private feedback candidate:** `p2-tm6-fixes-20261017/start.sh` under the
external FrontierX playtest directory is an immutable Clang full-media Debug
snapshot of revision `a8f0cf7` plus its recorded working-tree source overlay.
The manifest fingerprints binary/cache/shaders/fonts, preserves genuine TM6
progress and the earlier quicksave, includes focused test logs and documents
the four narrow checks. Launch preflight verifies the original selector and
absence of media exclusion flags. Select **Allies** and then F8 for that earlier
state; the overwritten screenshot battle is not reconstructed. TM7 must not be
started in this candidate because its module is not linked. No complete replay
is required solely to check these shared rendering repairs. The accepted TM5
candidate and its user saves remain untouched.


## TM7/TM8 production integration and TM9 next entry (2026-10-17)

**Scope and source review:** the user requests TM7 and TM8 while independently
testing the earlier feedback candidate. Native `Script07T` and `Script08T` now
retain their original programs, event flags, globals and resources in separate
module namespaces. `Script09T` is linked only because original TM8 Continue
requires the next briefing. Separate native initialization resets module state
before a new load, and TM8's additional briefing-animation record is reset too.
No objective, timer, AI personality, research/build gate, victory bit or RNG
sequence is replaced. The Win32 loop variable in TM7 is declared once before
its two original loops, preserving its legacy function scope with standard C++.
Windows-specific paths remain viable but unexecuted here.

TM7 uses `MANTIS_SWARM`, original construction/destruction notifications and
target-system logic. It introduces Propulsion/AWS labs, extends the route
through ten systems, and requires a player HQ in Lemo plus removal of Mantis
platforms. Benson/Ker Tak survival and original result/next-entry programs are
retained. TM8 uses `MANTIS_FORWARD_BUILD`, fourteen systems, Benson's rescue
trigger/death timer, the authored Kracus/Sendlor/Varn/Talos waves, repeated
attacks and later Solarian assistance. Late factories include Benson's
Battleship, Blackwell's chrome ship and Solarian Monolith/Taos/Trireme. Source
review identifies these requirements; it does not prove their full gameplay.

CMake uses the existing `_ADB` preprocessing, CP1252 `llvm-rc`, schema embed and
RT_STRING extraction pipeline for all three original modules. MScript program
registration, load/reset/briefing/resource dispatch, native scene admission,
full lighting/effects selection and original system-map restoration all admit
the actual retail aliases, including uppercase `SCRIPT08.dll`. Original disk
and public module interfaces are unchanged. The existing layout and parser
checks pin TM7/TM8/TM9 mission sizes to 264/380/896 bytes, actor offsets and
final-field extents; the shell checks isolate original briefing/start entries,
state reset and genuine empty one-byte program records.

**Concrete next-entry blocker and repair:** the new TM9 schema check first
fails with a parsed global size of 292 instead of the C++ record's 896 bytes.
Read-only GDB locals identify the exact mismatch. `DOCUVIEW::parse_array`
consumes the first number and then blindly consumes one token as `]`, losing
`PlanetTrigger[3*12]` and both `[12+1]` arrays. Native parsing now evaluates
nonnegative integer sums/products with precedence and parentheses, verifies
closing delimiters and rejects count/byte-extent overflow. This is a source
parser-owner repair, not a normalized replacement schema or weakened size
check. The Windows parser branch is unchanged. Existing parser regressions
check counts 36/13/14/20, following-field alignment, rejected fractional/count/
extent overflow and complete TM9 schema/correlation. The earlier empty-program
check could not detect an incorrect complete global extent; the dedicated
schema check now does. It remains registered with the existing CTest owner.

**Compilation and synthetic automated execution:** Linux x86_64 Clang22.1.8
full-media Debug (`p1c-clang-20261015`), GCC16.2.1 full-media Debug
(`p1b-video-gcc`) and fatal Clang ASan/UBSan (`p1b-video-clang-san`) build
`frontierx_native_shell`, `frontierx_view_parser_probe` and
`frontierx_layout_probe`. The following exact focused selection passes without
skips in each build with an external font:

```sh
ctest --test-dir BUILD -R '^portability\.(native_shell|layout|view_parser|script0[1-9]_schema|app_schema|sol_t_schema)$' --output-on-failure
```

The sanitizer environment is
`ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`.
The existing technical sanitizer CI selection now includes Script06–09; the
CI driver regression verifies that selection and distinct TM7/TM8 handoff
checklists. `python3 Tools/ci/test_ci_gates.py` passes. This is local execution,
not a hosted CI result or full-process GPU leak pass.

**Bounded retail-input runtime:** separate full-media Clang runs enter actual
TM7 (`Script07.dll`) and TM8 (`SCRIPT08.dll`), present production object draws,
advance the original source loop with animated portraits and close with exit 0,
zero mission audio streams and idle hints. Own-window Wayland input performs
real early F7/F8 without focusing or sending input to the user's independent
test. Physical saves contain 264/380-byte Script/GlobalData plus the actual
active original program records. TM7 preserves MarchToLemo and MissionStart;
TM8 preserves AeonVisited, AIManager, NearKalidon and MissionStart. Both restore,
continue source frames and close normally with unchanged genuine victory
progress. These early checkpoints do not cover late waves/rescue or research.
The original TM7 briefing Start and TM8/TM9 briefing Cancel execute with normal
key events in separate full-media runs; they are distinct from direct-entry
source-loop evidence. No campaign victory or unlock is manufactured.

The first hidden-GPU attempt is rejected by the existing visible-window
precondition and is not runtime evidence. Separate X11 attempts retain the
known host resize failure (`Native full-target clear viewport differs from
window pixel size`) before save input; no assertion is relaxed. Successful
Wayland runs use the supported candidate path. Transient preparation affects
only owned test windows; external user saves and the earlier feedback snapshot
remain untouched.

The separate production unit-catalog diagnostic loads the late TM8 ship
factories above, including their dependent launchers/assets. The whole catalog
still exits 6 for the previously missing unrelated DefenseBattery/AntennaBeacon
model inputs. That failure is retained; model/factory admission is not late
combat/rendering evidence. Complete TM7/TM8 gameplay/media, late saves,
defeat/retry, genuine victory/progress/transitions and user acceptance remain
pending. Native Windows Debug/Final/module ABI, hosted CI, whole-process host
leaks and M3 comparison are unperformed/open as previously recorded.


**Private candidate:** `p2-tm7-tm8-20261017/start.sh` under the external
FrontierX playtest directory pins revision `a8f0cf7` plus the recorded source
overlay, Clang full-media Debug binary, configuration, shaders and strict
external font mapping. Original campaign entry uses unchanged genuine TM6
progress; choose **Supply Lines**. Mission 8 becomes available through genuine
TM7 victory, with no unlock override. The private early TM7 quicksave is
installed as F8; the early TM8 save is retained separately as a checkpoint.
Both mission checklists and technical/runtime logs are included. This candidate
also contains the earlier fighter/color/platform feedback repairs; the user's
independent TM6 feedback candidate and saves remain untouched. Source/module
integration and early runtime are verified; complete mission acceptance remains
pending user gameplay. TM9 is a next-entry dependency, not a complete prepared
mission-9 gameplay claim.
