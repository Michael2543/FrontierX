# Terran campaign expansion

## Current delivery boundary (2026-10-16)

The user explicitly confirms TM3 is completely finished and accepts the final
Weaver repair on 2026-10-16. Record TM3 as accepted in the delivered native
Linux scope, including its reported complete run. TM4, **Friend or Foe?**, is
now prepared in original campaign order for a complete user playtest. This acceptance does not
invent new automated, Windows/ABI or hosted-CI execution. The earlier dated
pending TM3 notes below are implementation history superseded by this decision.

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
| 4. Friend or Foe? | `Script04T` | Rebel Siphons, six systems, recurring waves and Alto ambush; Heavy Industry/Academy/Battleship; BlackHole terrain and RGB888 ring; Stasis/Tempest/KerTak | Complete user run, late mechanisms/media and actual victory/Continue pending; carried host leak/Windows/ABI/CI gates below | `6242dc1` + local TM4 changes; GCC16.2.1/Clang22.1.8 full-media Debug | **ready for playtest**, user acceptance pending |
| 5. Frontier Wars | `Script05T` | Original module/schema/resources linked only to prepare TM4 Continue and the original next briefing | Complete mission mechanics/media/save-load not validated | GCC/Clang schema/module checks and actual standalone full-media briefing | **in implementation**, transition preparation only |
| 6. Allies | `Script06T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 7. Supply Lines | `Script07T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 8. Lost Sheep | `Script08T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 9. Nine Lives | `Script09T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 10. Betrayal | `Script10T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 11. The Traitor | `Script11T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
| 12. Survival of the Fittest | `Script12T` | Not yet reviewed | Native script/schema/resource owner not linked | None | **untested** |
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
