# First native single-player acceptance path

Current status, 2026-10-12: **M2 Linux Sol_Train gameplay accepted in the
agreed scope**. Original entry, all23 training objectives, required HUD/
commands/map, pause, early and later F7/F8, source victory/Continue and repeated
mission entry/close execute through normal UI under GCC and Clang. Clang's
complete evidence is its own genuine early-save continuation with user-played
final combat, not a newly invented empty-root replay. Latest separate visible
GCC/Clang/ASan end/restart/early-reload/close runs confirm zero mission audio
streams and idle original hint owners at both closes. No ASan invalid-memory
or UBSan report occurs in the observed instrumented path; whole LSan still
FAILS3156B/7 known external NVIDIA/PipeWire/DBus allocations. This is not
leak-clean process acceptance. Ordinary Wayland probe resize and leak-enabled
GPU processes also remain known host validation failures; complete uninstrumented
GPU checks pass under explicit X11 conditions. These limits are retained, not
skipped into success. Optional media exclusions/Admiral availability below
remain in force. Windows/module ABI and retail/determinism comparisons are
explicitly deferred by the user to M3. Detailed final acceptance is at
[the final M2 record](#m2-acceptance-in-the-agreed-linux-scope-2026-10-12).
Earlier dated entries below are historical and their pending claims are
superseded only where the final executed evidence says so.

## Native end-statistics array boundary (2026-10-12; bounded repeat passed; full training open)

Actual ASan/UBSan regular campaign selector/briefing/Start followed by normal
P/Escape/F8 of the unchanged genuine final-two-platforms attack save reaches
original objective21 and Mission Successful. Ordinary OK then exits1:
`MenuEndGame::init` indexes overview title4 in `STATIC_DATA[4]`. This is a
native retail-data runtime counterexample, not a synthetic failure or success.
Hypothesis before modification: the initializer's hardcoded five-title loop
exceeds the original `TITLES_O=4` resource and control arrays; the smallest
countercheck is the same ordinary OK transition under UBSan. Native init now
uses `TITLES_O`; original Windows code and all fields/interfaces/resource IDs,
serialized formats and simulation/RNG remain unchanged. Existing app_schema
correlation extends the last valid overview title and adjacent first unit.
It proves layout correlation, not runtime menu acceptance.

Executed: `cmake --build /tmp/frontierx-voice-asan -j4`;
`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir
/tmp/frontierx-voice-asan --output-on-failure -R
'portability\.(app_schema|native_shell)$'` passes both checks. External
`build-five.py end-title-bound-current` rebuilds the five documented Linux
Debug default/optional/instrumented configurations and passes their existing
non-GPU checks (central totals in BUILD_GRAPH). The ordinary OK repeat passes this bound and exposes the subsequent
TabButton diagnostic type error below; statistics/Continue/close remain open. Captures/raw logs remain
outside the repository. Native Windows Debug/Final/module ABI and retail
statistics comparison remain unexecuted; this does not accept M2.

## Native tab-factory diagnostic type guard (2026-10-12; bounded repeat passed)

The real instrumented ordinary victory OK passes the corrected overview-title
initializer, then exits1 at `TabButtonFactory::CreateArchetype`: its diagnostic
name-only test casts a non-hotbutton TabControl record to `GT_HOTBUTTON` and
loads an invalid enum1164075380. Hypothesis before modification: requiring
`objClass == GBT_HOTBUTTON` before this diagnostic cast removes the invalid
read; the smallest countercheck is the unchanged ordinary end OK under UBSan.
Only the native diagnostic condition changes; production factory dispatch,
Windows code, layouts/interfaces/resources and simulation/RNG remain unchanged.
The screenshot after this failure contains the desktop and is not statistics
acceptance. The preceding Mission Successful was visually inspected.
Instrumented app_schema/native_shell focused checks pass;
`build-five.py tab-diagnostic-current` rebuilds all five documented Linux
Debug trees and passes their non-GPU checks. Visible end/return/close remains
pending. External logs are `tab-diagnostic-asan-{build,check}.log`;
`tab-diagnostic-before-runtime.log` retains the real counterexample.

The next ordinary instrumented OK passes TabButton's guarded diagnostic and
exposes the identical name-only cast in `HotButtonFactory::CreateArchetype`
(HotButton.cpp:776, invalid enum1164075380, exit1). Before modification the
same hypothesis/countercheck applies: require `GBT_HOTBUTTON` before this
native diagnostic cast, then repeat original OK under UBSan. The production
factory already checks that class. Source search finds these two matching
factory-debug blocks. Native HotButton's diagnostic now uses the same guard;
Windows expression, dispatch, layouts/resources/interfaces and RNG are intact.
The real failure remains external in `hotbutton-diagnostic-before-runtime.log`.
Visible statistics/return/close is still not accepted.

## Native explosion child-index overflow (2026-10-12; bounded repeat passed)

A further real unchanged final-two-platforms F8 combat repeat exits1 before
victory: `Explosion::doExplode` evaluates `1813359347 * 5` in signed int at
explosion.cpp:612. Hypothesis before modification: native RAND_MAX exceeds
the legacy intermediate capacity; widening only the multiplication to S64
preserves the intended integer child index and original single rand call/order.
The smallest countercheck is the same genuine saved combat continuation under
UBSan. Native expression now widens the intermediate; Windows expression,
fields/interfaces/serialized data and RNG consumption remain unchanged.
Other original random expressions here already multiply floating values.
This is actual native runtime on external data, not Windows/retail parity.
Instrumented focused app_schema/native_shell checks pass.
`build-five.py explosion-child-current` rebuilds all five documented Linux
Debug default/optional/instrumented trees and passes their existing non-GPU
checks (totals in BUILD_GRAPH). Visible end/lifecycle repeat remains pending.
The desktop-only `tab-diagnostic-victory.png` after process death does not prove
victory or statistics. Raw counterlog remains external as
`explosion-child-before-runtime.log`.

## Instrumented original victory / end return (2026-10-12; bounded close passed)

With the native title bound, both factory diagnostic guards and explosion
intermediate correction, `run-end-lifecycle-asan.sh` again uses the documented
optional Debug ASan/UBSan shell with `--mission-campaign`. Normal selector
Solar400,333 and Briefing Start350,543 enter objective1. Existing
`menu-reload.py end-lifecycle-asan reuse` sends regular P/Escape/F8 to the
unchanged whole original F7 final-two-platforms attack checkpoint. No new
attack/move order is issued. Actual damage/deaths execute, objective21 completes
and Mission Successful is visually inspected. Normal OK404,447 shows You Win!
with overview415/475/3415/3415 and time2:43:22; Continue639,472 returns visibly
to the original training selector. These numbers are observed presentation,
not an asserted score or Windows/cross-compiler parity. Player name is blank.
Same-process normal selector/briefing/Start reaches a new source objective1
and an inspected initial object frame. Its early capture has no visible HUD,
so repeated HUD restoration is not accepted from that capture. Ordinary
compositor close produces a newly verified exit0 and the process is gone,
without ASan/UBSan errors. TextureLibrary still reports dangling references.

All four current corrections pass instrumented focused app_schema/native_shell
and their affected five-tree Debug rebuild/non-GPU checks; totals/labels are
central in BUILD_GRAPH. System SDL/FreeType/shared FFmpeg/GPU-driver components
are uninstrumented. Leaks OFF; training speech/unit voices/unit alerts and the
existing documented media exclusions remain OFF in this instrumented run;
optional decoder stays ON for original mandatory Sector comm. This is actual
native runtime on lawful external retail data, bounded late reload-to-victory
and menu return, not a full fresh training or leak/media/retail-parity acceptance.
Captures/raw logs/saves remain external under the documented runtime root.

## Fresh independent GCC full-run attempt (2026-10-12; in progress)

`run-full-fresh-gcc.sh` uses the documented optional GCC font/shared-decoder
Debug shell with `--mission-campaign`, original external Sol_Train and an empty
`save-full-fresh-gcc` directory. It contains no checkpoint seed and uses all
normal documented exclusions, with training speech/unit voices/alerts enabled.
Normal selector400,333 → Briefing Start350,543 enters objective1, with visually
inspected HUD/subtitle. Earlier fixed clicks303,425 and the old lasso miss the
unit in this regular-entry camera. Normal original F and slash select/center
the real Forger; ordinary right-click624,300 completes1/2, selection voice/text
and speech execute. Subsequent ordinary lasso/fullscreen-map scouting and
original early F7/F8-to-end are being executed. External timestamped
`full-fresh-gcc-input-journal.txt` records regular SDL-producing actions;
source objective observations alone do not accept the full run.

Fresh GCC continuation: normal F/slash and full-field lasso5,25→790,410
select all three ships after speech; objective3 completes. Normal Taos
selection276,265, Insert, zoom out/in via real wheel, and map right-click
Solar4(527,245) scout and complete4/list5. Normal F7 retains the unchanged
whole early1–4 save externally, P/Escape/F8 visibly restores Solar4/scout and
original objective5 text. F/Insert with normal wheel navigation and Prime
left383,261/Insert shows original Prime resource hint and Forger HUD. Normal
Oxidator75,515 then green slot460,250 places the real order. Immediate normal
F7 records Forger mode1, working125/building35688, moveActive1, no buildee.
Whole `full-gcc-oxidator-order.mission` is retained unchanged; ordinary F8
reloads it without reissuing a build. Source continuation/ready5 is pending,
so this does not close the earlier moving-build reload regression yet.

Fresh GCC pending Oxidator F8 continues without build reissue to visually
inspected Construction Complete/source5 complete/6 listed. Normal subsequent
F7 records Forger pos44032,82944, mode0/working0 and preserved early objective
progress; whole ready checkpoint retained externally. This executes ordinary
pending-order reload through construction on the fresh mission, but does not
close the earlier Jumpgate-specific moving-build stall. Normal Oxidator
selection470,285 and Galiot30,525 production are now executing.

Fresh GCC normal Oxidator production now shows the original Galiot/H hint,
queue and resource reservation. Leaving to field removes that hint. Source6
completes and7 is listed, with visible Galiot/platform and original text/speech;
initial objectives1–4 are then removed by the original training script, not
reported as reload loss. Pause counter/restart and normal Bunker7 continue.

Fresh GCC pause uses original P, F7, five-second wait and second F7:
read-only original GlobalData decoding reports updateCount12672 in both whole
saves. Escape/three-second wait/F7 advances12775. Pause is visually inspected;
no script/debug setter supplies this result. The three unchanged saves remain
external. Bunker placement is still pending: F selects/centers Forger and
reopening Insert resets fullscreen map zoom, so reusing zoomed coordinates
without real wheel navigation moves the camera outside Solar. Those ordinary
failed placement attempts reserve no resources; the run uses visible map
navigation to return to Prime rather than modifying camera/planet state.

Fresh GCC Bunker normal industry30,552 and Prime green330,260 construct the
original1-slot Bunker, complete7/list8, with visible source STMG14/crew45 per
minute. Normal F7 retains the ready state externally. Military30,515 shows
Sentinel Tower/S and original2-slot requirement. Prime hover340,270 renders
green, but subsequent ordinary click/retry345,275 produces Can't build here;
normal F7 shows mode0/working0, no Sentinel and no resource reservation. No
construction acceptance or ownership fix is inferred from the preview alone.
Original render/selection source prioritizes first-pass units before planets;
the exact rejected target remains undiagnosed. The run continues with normal
map Forger movement to Solar3, which original Sentinel training permits.

Original ObjectComm source review corrects the subsequent navigation diagnosis:
regular world right-button-up cancels build cursor/BUILDARCHEID; Escape does
not clear this mode. Earlier presumed Solar3 map orders while it remained
active are not accepted movement. The run cancels via normal world right-click
and uses original minimap movement/camera controls; fixed fullscreen zoom
anchors also prove unsuitable after moving the camera outside the system.
No native owner change is justified by those unsuccessful inputs alone.

Source review of Planetoid confirms TestSlotHighlight requires bVisible,
and TestVisible gates it on actual player visibility before frustum testing.
A drawn/shadow planet or blue slot ring alone is insufficient. The fresh GCC
run normally cancels build cursor, selects idle military Taos with comma and
orders it to Solar3 via minimap539,498; real Taos selection/move voices execute.
This is a falsifiable normal gameplay visibility check before considering any
owner change. Source fog/slot/tech conditions remain unchanged.

Fresh GCC normal Taos scouting makes Solar3 currently visible. Normal F,
minimap left539,498, Sentinel30,515 and hover530,310 now show a green source
2-slot preview; ordinary left530,310 creates actual Forger mode1/working978/
building34808 and an active move to77625.1,67382. Normal F7 retains the whole
unchanged pending save; original marks reserve Solar3 bits2049. This real
result supports the visibility hypothesis for Solar3 and needs no owner patch.
Prime's earlier rejected hover remains separately unproven. Ready8/9 and
continued fresh training are pending.

Fresh GCC source8 now completes/list9 with visually inspected Sentinel
Construction Complete on Solar3. Original industry tab148,487 and Pavilion
120,515 show the resource hint and a green3-slot preview380,280 on that same
currently scouted planet; ordinary click orders construction. No condition,
slot count, resources or command packet is modified to permit these builds.
Normal continued source9/Helion10 is still being executed.

Fresh GCC later normal F7 during Pavilion9 preserves actual building mode8,
working1082/building34852/buildee58289 and moveActive0 at69380.4,70793.3.
Whole `full-gcc-pavilion-in-progress.mission` is retained unchanged, distinct
from the early1–4 scout state. Ordinary F8 reloads this exact save without a
new build/move order. Normal follow-up F7/read-only owner comparison and
construction-to-ready9 are in progress; eventual reload-to-victory remains
required, not inferred from load100% or draw counts.

Fresh GCC continuation after Pavilion in-progress F8 now completes original
objective9 and lists10 without reissuing a build or move command. Read-only
follow-up F7 records Forger mode0/working0/buildee0 and the ready Pavilion58289;
the completed platform is visually inspected. This is a later building-state
reload, distinct from early scout1–4. Eventual continuation to victory is still
required. Current normal advanced-tab Helion Veil hover shows the original
one-slot/450 ore/1200 gas/30 crew hint through Hintbox. Normal field cancellation,
Taos scouting and Forger selection precede a green free-slot hover500,355 and
ordinary left placement. No production or visibility condition is changed.

### Fresh GCC ESP/Citadel and Greater placement continuation (2026-10-12)

After the rejected first free-space order, ordinary defence-tab ESP placement
at480,300 constructs real GunPlat58561, completes11 and lists16. Ordinary
industry Citadel75,552/Prime340,300 completes16 and lists18. Visually inspected
platforms and Construction Complete accompany these source observations;
whole normal F7 checkpoints remain external. No movement/path/slot correction
was needed. Normal selection of the actual Forger and world right-click cancel
precede minimap Solar3 movement; merely selecting F or clicking the map while
construction mode remains active is not accepted movement.

**Local placement hypothesis / countercheck.** Greater requires three adjacent
free Solar3 slots in the original2100–3800 world-radius annulus. Read-only
child-debugger observation of ordinary mouse hover through
CAMERA::ScreenToPoint/TestSlotHighlight distinguishes an invalid pixel from a
missing production owner. Screen450,395 lies outside the annulus;520,320 is
inside it but selects occupied slot11. Screen480,245 maps to74480.60,72328.80,
centre8, whose neighbouring7/9 are free (original trueMarks2169). No debugger
call or state assignment supplies construction. The original GCC ordinary
left480,245 creates Greater58705, Forger FAB_BUILDING/work3322 and real platform
hull progress814→1929 across normal F7. Further ordinary wait/F7 reaches ready18 (objective63 complete,62 pending),
visually inspected Greater model and original Annex resource hint. Whole
`full-gcc-greater-ready.mission` remains external. Annex placement follows the
visibly green one-slot Solar3 hover535,280. Captures/journal/save remain outside
the repository. The separate
instrumented diagnostic closes normally from the background without refocus;
it is not a full training or Greater construction acceptance run.

Fresh GCC next source steps: advanced Annex30,552/Solar3 green535,280
constructs58817, completes17 (62), removes16 and lists22 (67). Advanced
Anvil75,552/green480,355 constructs58897, completes22 and lists23 (68).
Advanced Dock120,552/green440,355 constructs58977; normal F7 records its
real progressing hull206. Visible Solar3 models and original hints/green
previews were inspected. This is the same fresh chain after its earlier normal
scout and building F8, not a seeded checkpoint run; Dock completion and
reload-to-victory are still pending.

Fresh GCC Dock23 subsequently completes (68), lists24 (69), and visibly
renders the original Xeno/STMG33 teaching subtitles. Xeno's actual original
resource hint says3 slots/1500 ore/2000 gas/100 crew. A separate child-debugger
normal unchanged F8 and hover at Prime380,245 reads bVisible=true,
SP_Xeno.3db/slotsNeeded3, world40045.67,80890.94 and valid annulus distance;
it falsifies the provisional visibility/projection hypothesis. Prime only has
isolated free3/7/11, Solar3 is full: neither permits three adjacent slots.
No Planetoid/camera/movement change follows. The next regular action is Solar4
navigation, matching the earlier actual Clang Xeno placement. Diagnostic
normal background close exits0 without refocus or ASan/UBSan report; this is
not construction/full-training/leak acceptance. Whole save/captures/raw log
remain external under xeno-slot-gdb.

### Current original Jumpgate14 reload blocker (2026-10-12)

Actual native external-data GCC continuation, latest optional Debug binary:
`run-full-gcc-target-cache-continuation.sh`, same genuine fresh-chain save root,
regular selector/Brief Start/F8. Without new gameplay orders, the original
six-Polaris fight resumes439→50 hull and subsequently destroys both linked
hostile platforms; weapon beams and the unoccupied wormhole are visually
inspected. Speech, unit voices and alerts are enabled. This executes the native
automatic-target cache repair in GCC; a separate latest Clang repeat is still
required, and this saved-chain continuation is not an independent full run.

Normal F/slash, Defence222,487, Jumpgate75,515 and wormhole434,405 issue the
original building command: work6834/building40308, FAB_MOVING_TO_TARGET,
MS_PATHFIND, goal8.75,3.75. Normal F7/F8 without reissue followed by25s/F7
leaves the Forger at35840,11264, velocity0, pathLength1, node/current8.75,2.75,
moveAgent0/moveActive1, no buildee. Objective13 is complete and14 still pending.
The actual capture `full-gcc-jumpgate-moving-reloaded.png` and unchanged whole
save remain external. This is FAILED pending-build reload continuation.

Local hypothesis, before any movement edit: native loaded extent or position
control prevents original doPathMove from completing the final node. Smallest
countercheck is read-only inspection of actual box/boxRadius, slop, doMove goal
and control return at the unchanged normal F8 state. The negative box[5]
completion condition alone is insufficient evidence: doMove has an earlier
near-radius setPosition return. No tolerance, path flag, simulation or saved
state is changed, and Stop/reissue would not prove this criterion. A separate
instrumented child uses regular entry and an unchanged whole copy under
save-jump-move-gdb; no debugger state setters or gameplay calls are used.
Next: identify this original owner blocker, restore unchanged-order continuation,
then complete14→original victory/return and independent latest full runs.

**Read-only countercheck and zero-distance hypothesis.** Actual unchanged
instrumented F8 reads boxRadius353.77829, box[5]−460.937805, slop0,
position35840,11264,−200, cruise−200, auto movement true, move disabled false.
Original doMove near-radius branch receives goal0 and goalMag0 but setPosition
returns false. This falsifies the extent/sign hypothesis. Native MathCore
legacy_sqrt multiplies0 by its inverse-square-root approximation; float
intermediate overflow produces NaN. Local hypothesis: restore the square-root
zero input contract only at the native math adapter, retaining the existing
nonzero algorithm and original movement thresholds/RNG/order. Extend the
existing math_core check to assert exact zero for both signed-zero square roots; old adapter must fail before any repair.
A passing synthetic check alone cannot establish pending-build reload or
Windows parity; unchanged regular F8 and visible continuation remain required.

### Borrowed texture-ID ownership countercheck (2026-10-12; in progress)

**Local hypothesis / smallest check.** Shutdown texture-ID dangling counts
may come from TextureLibrary::get_texture_ref_texture_id incrementing its
object despite ITextureLibrary.h explicitly saying the lookup does not touch
the reference count. Actual TexInfo::set_texture queries its name while owning
only the texture-ref; it correctly does not release a borrowed ID. Extend the
existing state_material CTest with production TextureLibrary acquisition,
texture-ref creation, two ID lookups and release of both real owners. The
texture must then be absent; persistent presence falsifies ownership balance.
No new test target or parallel renderer is added. The old production implementation fails this focused instrumented check.
The native lookup now omits the unowned addref; the original Windows expression
is unchanged. The focused ASan/UBSan state_material CTest passes. No field,
serialized layout, vtable or module signature changes; simulation/RNG untouched.
All five affected Debug default/optional/instrumented graphs and existing
non-GPU suites pass (`borrowed-texture-id-current`). Actual bounded mission repeat now passes: external wrapper
`run-borrowed-texture-asan.sh` uses the same optional instrumented flags as
`run-inactive-close-latch-asan.sh`, separate `save-borrowed-texture-asan` seeded
only with the unchanged genuine normal three-Mimic/ready10 F7. Regular selector/
Brief Start → F8 → F7 → F8 → inspected platform/HUD → normal compositor close
exits0, with no ASan/UBSan or TextureLibrary dangling/active-reference reports.
Earlier identical owner graphs reported many such references. Leaks OFF,
training speech/unit voice/alerts and documented media excluded; external
SDL/FreeType/shared decoder/GPU uninstrumented. This bounds the actual lookup
fix and repeated mission reload cleanup, not fresh full training or media cleanup.
The initial separate focused `ASAN_OPTIONS=detect_leaks=1` state_material CTest failed
on8 bytes/one allocation: XMesh::read_material_library's CQ2 imaterial_list
at Mesh.cpp905. The borrowed lookup counter passes; full leak acceptance and
native Windows/retail comparison remain open.

**Remaining CQ2 list hypothesis / smallest check.** The8-byte focused LSan
allocation is the owning XMesh pointer array from read_material_library.
release_full_mesh deletes the old material_list but never imaterial_list.
MaterialManager::FindMaterial returns manager-owned materials; partial mesh
copies borrow static data and zero the structure at release_partial_mesh,
while full clones do not copy this list. Free only the native full-owner array,
not the pointed-to materials; rerun the same existing leak-enabled state_material
check. Any remaining allocation or invalid free disproves the proposed lifetime.
No shared Mesh field/layout/signature changes; Windows cleanup remains unchanged.
The native full-owner array delete passes the same existing state_material
CTest with `ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`.
This is synthetic production-owner leak evidence, not mission leak acceptance.
All five affected Debug graphs and existing non-GPU suites pass
(`cq2-material-array-current`). Actual regular selector/Brief Start → genuine
three-Mimic/ready10 F8 → F7 → F8 → normal close, with leak checking ON,
exits1: LSan reports93,399 bytes/841 allocations. No ASan invalid-access,
UBSan or former TextureLibrary dangling-reference reports are observed.
This is a FAILED bounded mission leak check, not cleanup acceptance. External
wrapper `run-cq2-material-array-asan.sh` uses a separate unchanged genuine save
and the same media exclusions/uninstrumented dependencies listed above.

**App MeshRender ownership hypothesis / smallest check (2026-10-12).**
The actual leak stacks include MeshRender::Init's fallback CQ2 pointer array
and NativeRenderPipeline index buffers from RestoreVertexBuffers. Source review
finds the failed texture-name branch nulls its newly allocated list without
freeing it; FaceGroupRender destroys index_list4 but omits the independently
created IB4. calloc initializes the face group; Clone copies neither IB4 nor
index_list4. Release these two native owned allocations without releasing
manager-owned materials or changing Windows code, fields, module interfaces,
simulation/RNG or persistent formats. The existing leak-enabled
`portability.game_material_manager` check reproduces both allocation roots:
before1377 bytes/25 allocations, after1229 bytes/17 allocations, with both
MeshRender allocation roots absent. The remaining Engine/PolyMesh archetype
leak means this whole focused LSan test STILL FAILS; it is not a pass or a
full-mission leak result. Affected builds and actual mission repeat are pending.

The remaining focused archetype leak is traced to four observational test
calls of ENGINE::get_instance_archetype. ENGINE.H explicitly returns a held
reference. Local hypothesis: balancing these test-only holds removes the
remaining1229-byte allocation without altering production Engine ownership.
The same existing leak-enabled game_material_manager check now passes after
returning each observational reference. No assertion was removed or weakened.
All five affected Debug/non-GPU suites pass `mesh-owned-buffers-current`;
the subsequent test-only balance rebuild is running.
Actual `run-mesh-owned-buffers-asan.sh` repeats the same regular entry/genuine
later F8/F7/F8/inspected frame/normal background close with leaks ON: exits1,
73,779 bytes/669 allocations. The prior index-buffer root disappears, while
MeshRender fallback arrays remain: source review identifies a second failed
material lookup branch that also nulls its list, and successful promotion
nulls the original legacy material array. This is FAILED mission leak evidence,
not a pass; external dbus/unknown-module roots are also present.

**Second lookup/promotion hypothesis (2026-10-12).** Real mission LSan
still identifies MeshRender::Init arrays after the first unnamed-texture fix.
A failed FindMaterial branch also drops imaterial_list; successful CQ2 promotion
drops material_list. Free the former at failure and delete the latter at
promotion (Material::~Material already owns its texture-ref cleanup), retaining
original zeroed material counts and borrowed CQ2 ownership. Existing leak-enabled
game_material_manager passes after both native-only fixes. All five affected
build/check graphs and the actual regular reload/close are being repeated.

Fresh GCC original Basic Eutromil165,515 and green Solar4 two-slot465,370
create59489/FAB_BUILDING/work5966. Normal in-progress F7/F8 without reissue
finishes13 (58), lists14 (59), shows the finished model and original red
Mission Alert/teletype/STMG24 subtitles, with training speech, unit voice and
alerts enabled. Earlier interior-ring520,300 clicks were rejected and count
as no construction evidence. The foreground capture is visually inspected;
whole saves/captures stay external. This is continuation of the original fresh
GCC sequence, not latest-binary independent full acceptance.

Final bounded `run-mesh-lookup-cleanup-asan.sh` regular selector/Brief Start →
genuine later F8 → F7 → F8 → inspected frame → P/normal background close
exits1 with68,515 bytes/435 allocations. No MeshRender fallback-list or native
index-buffer allocation roots, former TextureLibrary dangling reports, invalid
access or UBSan errors remain in this run. LSan still reports original
Channel/Anim allocations and external dbus/unknown-module roots: full mission
leak acceptance STILL FAILS. No suppression converts this into a pass.
`mesh-lookup-cleanup-current` rebuilds all five Debug graphs and passes each
existing non-GPU suite; focused game_material_manager leaks ON passes.

Fresh GCC regular in-progress Eutromil F8 finishes13, normal F7/P/close exits0
before the original bounded process timeout. `run-full-gcc-current-continuation.sh`
uses the same external genuine save root and flags with43200s process limit;
regular selector/Brief Start/F8 restores completed13 and continues14 on the
latest GCC binary. This is saved-chain continuation, not a new independent
latest-binary every-step run.

Current GCC original Solar3 camera, field lasso170,50→550,400 selects six
Polaris + Taos; normal Attack316,554 and fullscreen-map right350,365 reach the
Solar wormhole. Visible weapon beams/actual six ship target57026 lower hostile
JumpPlat800→439, with supplies consumed. Normal combat F7/F8 without reissue
retains target/Attack in the save but no further damage is observed over25s;
normal field reattack also hits the underlying forbidden Wormhole rather than
an enemy platform. This is a newly bounded reload/association/visibility
regression, NOT successful combat-command continuation. The read-only separate
instrumented GDB child `gate-reload-gdb` is being used to inspect original
Gunboat::resolve / JumpPlat owner state. No state setters/debug gameplay commands
are used. Earlier final-platform reload evidence remains a separate state and
does not prove this gate state.

**Loaded automatic target-cache hypothesis (2026-10-12; pending).**
Read-only instrumented normal F8 child: Gunboat59169 retains dwTargetID57026,
Attack stance and attackAgentID0 (automatic targeting during the saved group
move). FindObject resolves the real enemy. Original resolve subsequently clears
launcher targets for attackAgentID0; load initializes netTargetID to dwTargetID,
so existing getSyncTarget sees no change and does not bind that same target.
Invalidate only the native transient netTargetID cache for this normal automatic
resolved-target case, retaining saved target/stance/agent, special/artifact paths,
original general-sync order and all tick/RNG/weapon rules. No persistent field,
interface or Windows path changes. Smallest falsifier: regular entry/F8 of the
unchanged real439-hull gate combat checkpoint must resume damage without any
new attack/move/stance command; no damage, invalid access or altered target
falsifies the candidate. This is a restore-cache repair hypothesis, not proof
of Windows/retail parity; actual same-save GCC/Clang/instrumented checks required.
GDB source-entry prints before thunk this-adjustment showed invalid-looking
fields; stepping into the body confirms the correct Jumpgate36/Solar2/
owner57026/locked state. No ABI corruption is inferred from those entry prints.

The first unchanged439-hull actual candidate repeat succeeds locally:
`run-auto-target-cache-asan.sh`, regular selector/Brief Start/F8, no new gameplay
orders, wait20/F7; actual paired gates439→284 with ship supplies consumed and
visible beams inspected. No ASan/UBSan report. This confirms automatic target
rebinding through original general sync for this saved state; full combat/end,
latest separate GCC/Clang and leak/media acceptance remain open. Leaks OFF,
training speech/unit voice/alerts and documented media excluded, external
SDL/FreeType/shared decoder/GPU uninstrumented. `auto-target-cache-current`
passes all five affected Debug builds/existing non-GPU suites; instrumented
app_schema/view_parser/native_shell focused checks pass. Normal Escape after P and foreground wait30/F7 continue the same automatic
fight without gameplay reissue: both hostile gate-platform records disappear,
targets clear normally, all six Polaris remain185 hull with26–38 supply. The
unoccupied wormhole is visibly inspected. Normal P/unfocus/close exits0 with
no ASan/UBSan report. Original-chain latest GCC continuation is being executed.

Fresh GCC normal Solar4 minimap581,476, original Xeno75,515 and green420,245
produce real59057/FAB_BUILDING/work4671. Normal continuation reaches24 complete
(69) and12 pending (57); the ready Xeno model is visually inspected in
`full-gcc-xeno-ready-visible.png`. The earlier ready capture was overlapped by
a separate newly opened diagnostic selector and is not used as visual evidence.
Normal Solar3 camera539,498/Pavilion340,260 exposes original Polaris75,525
hint; six ordinary clicks queue production. Actual first Polaris59169/hull159
and normal in-progress F7/F8/Construction Complete are observed. Source12
completion now executes: F7 after the unchanged ordinary queue F8 shows six
real Polaris (59169/59249/59297/59345/59393/59441), each185 hull/130 supply.
Source12 completes (57), removes10 and lists13 (58); all six models are
visually inspected. Continued reload-to-victory is still required.

### Original Frigate Mimic production dependency (2026-10-12)

The fresh GCC Helion completion creates the original three Mantis Frigates and
logs the missing `MIMIC` provider. Source review resolves that real caller to
Gunboat's weapon archetype and original Mimic.cpp factory. Local hypothesis:
the existing native gunboat target omits this original registered owner.
The smallest falsifier is compilation plus normal load/build10/spawn/save/reload
using a genuine pre-Helion training save, not a direct factory or script call.
Mimic.cpp now joins that target through existing native globals/interfaces.
Native includes replace only the PCH dependency; file spellings match disk.
Unused Windows-only ObjectTeam inclusion stays on Windows; three original
VOLPTR extent queries use declaration/assignment, preserving QueryInterface
and reference handling. Existing MimicArchetype calloc/free pairing is already
correct. No simulation code, RNG calls, persistent field, resource ID, command,
interface or serialized format changes. Existing app_schema checks assert the
original eight-byte MIMIC_SAVELOAD/base, alias offsets0/4 and correlation of
aliasArchetypeID/aliasPlayerID/cloak bit. Optional Clang focused app_schema and
native_shell pass; five affected Debug builds/non-GPU suites pass at
`mimic-team-boundary-current`. Actual bounded production lifecycle now executes
as recorded below; active mimic ability and native Windows Debug/Final/module
ABI remain unproved.
The running GCC PID1513914 predates this owner integration and cannot count as
its runtime evidence. Missing-owner errors are not converted to success.

Fresh GCC Helion10 completes after ordinary green500,355 placement. Three
original attackers jump to Solar through the original gate. Normal Taos direct
selection and minimap scouting reveal the attackers and list11 with actual
STMG19/subtitle. A previous comma selection changed HUD but subsequent moves
were ignored until direct left selection; exact cause remains open, no owner
patch is inferred. The original ESP/L hint and ordinary defence30,552/free
field460,200 green placement now run; ready11 and continuation remain pending.

Fresh GCC ESP11 remains pending after normal placement: Forger mode1,
working1995/building33752, pathfind stage0, no buildee and moveAgent0 but
moveActive1 at44033.8,87032.4 (goal10.75,20.75). The genuine normal F7 checkpoint
is retained whole. Forty seconds did not prove completion; the mission is
normally paused during the separate Mimic check. This may overlap the earlier
pending moving-build stall; source moveToTarget waits for isMoveActive to clear
before free-space beeline. No simulation or completion flags are forced.
Countercheck: ordinary F8 of the unchanged pending save in a child-debugger
ASan/UBSan run, normal Forger selection and P/Escape/wait/F7. Read-only runtime
box {346.38,-353.78,129.92,-129.02,853.84,-460.94}, radius353.78 and zero slop
falsify a reversed/empty-bounds hypothesis. Both this repeat and the original
GCC normal continuation reach FAB_IDLE/moveActive0/pathLength0; neither save
contains a ready ESP, and objective11 remains pending. This is **not** successful
construction: original free-space range/occupied-grid rejection can return idle
and refund the order. The next regular action is a different green free-space
placement, not a movement/completion flag patch. Child-debugger normal close
while unfocused stalls until normal refocus/Escape, then exits normally; see the
native pump correction below. Original GCC remains live; no fresh-chain end is
accepted.
Separate Mimic ASan regular selector/briefing/pre-Helion F8/P/Escape/normal
selection/movement attempts/F7/close exits0 with no ASan/UBSan report, but
Mimic instances were not created and this is **not** its production acceptance.
Texture dangling references remain. Read-only existing GlobalData codec reports
player1/maxPlayers0/scriptUI0/ally1/visibility3 both in the source pre-Helion save
and subsequent instrumented F7. Direct ptrace attach is unavailable (Yama;
sudo needs a password); an ordinary child-debugger repeat is used only to read
runtime pause/host state, never to issue gameplay or alter internal state.

### Native selection projection conversion boundary (2026-10-12)

Actual regular ASan/UBSan child-debugger entry and genuine pre-Helion F8,
Forger advanced-tab/Prime green350,350 placement aborts on RenderMgr.cpp1207:
projected Y about4.16033e10 converted to int by find_visible_polys. The real
caller is Platform<REFINEPLAT_SAVELOAD>::TestHighlight → ObjectList::Render,
not a direct synthetic gameplay command. Debugger reads, without getters or
state setters, before/after F8 show RT/game pause0, active-1, PLAYERID/HOSTID0,
no migration and editor0, falsifying the pause/host/editor-state hypothesis.

The existing game_material_manager check now exercises the actual Engine,
RenderMgr and generated legacy PolyMesh fixture: a representable selection hull
must succeed; extreme off-screen projection must fail. Before the fix the
instrumented check aborts at the same conversion (-6.4e10 to int). Native
find_visible_polys now excludes coordinates outside the exact int range before
conversion; ordered double comparisons also reject NaN/infinity. Original
Windows branch, render geometry, simulation, RNG, persistent layouts and
module interfaces remain unchanged. Existing instrumented app_schema,
game_material_manager and native_shell checks pass after the fix. All five
existing affected Debug builds/49 non-GPU checks pass at
`selection-projection-current`. The separate normal instrumented repetition now
passes the previously failing production call chain: regular selector/briefing,
genuine pre-Helion Pavilion9 F8, normal Forger selection/advanced Helion button,
Prime green350,350 placement, actual construction and completion10, then original
three attacking Frigates and natural objective11 visibility. Ordinary F7 stores
three original Turret-2/MIMIC_SAVELOAD records of eight bytes; normal F8 followed
by F7 retains all three records (instance reindexing is expected). Normal window
close exits0 without ASan/UBSan report. Inspected captures show actual platform,
HUD and save result; counters alone are not acceptance.

Reproduction: `bash /tmp/frontierx-m2-runtime/run-mimic-projection-asan.sh`,
using its unique external save root seeded with the unchanged genuine
`save-checkpoints/full-gcc-pavilion-restored.mission`; selector400,333,
briefing Start350,543; wait for actual mission/objective1 before F8; Forger F,
Prime minimap499,487, advanced190,487, Helion30,515, green350,350/click;
F7, F8, F7, normal compositor window close. Exact shell arguments match the
instrumented end-lifecycle configuration: optional Clang Debug with native
fonts/shared decoder/GPU, ASan/UBSan, leaks OFF; training speech, unit voices
and unit alerts OFF alongside documented media exclusions. External SDL,
FreeType, FFmpeg and GPU driver are uninstrumented. External journal, captures,
`mimic-projection-asan.log` and `.exit` retain the concrete run; no retail
artifacts enter the repository. Texture dangling references still appear;
active Mimic ability, a fresh complete GCC/Clang training chain, native Windows
Debug/Final, module ABI and retail comparison remain unproved.

### Native unfocused-close pump boundary (2026-10-12)

Actual child-debugger normal compositor close after the genuine pending-ESP
F8/P/Escape/wait/F7 leaves the native pump waiting in SdlWindow::WaitEvent;
normal refocus/Escape releases it and the process exits normally. Hypothesis:
accepted Close/ Quit must return to the original mission/modal owner even when
AppIsActive is false; the source callback alone cannot end the native wait loop.
The smallest falsifier extends existing native_window_manager CTest with real
SDL focus-lost, accepted close and a queued later focus-gained event: foreground
ServeMessageQueue must return before consuming that later focus event, while
the close veto and queued-event processing remain intact. Old code fails
`inactive accepted close waited for a later focus event`.

Native private HandleEvent now reports accepted close/quit; ServeMessageQueue
returns immediately to its callback owner. Its already-waiting loop recomputes
focus after draining events; an initially nonwaiting poll retains its original
return behavior. An intermediate unconditional wait recomputation blocked the
existing ordinary Update test and was corrected. No persistent fields, public
interfaces, Windows paths, simulation or RNG change. Focused instrumented
`portability.native_window_manager` passes in0.72s; all five affected Linux
Debug configurations and existing non-GPU suites pass at
`inactive-close-current`. The first actual unfocused mission repeat still waits until normal refocus,
then exits0 without ASan/UBSan report; it is not unfocused-close acceptance.
A second-call countercheck now fails `a second pump waited for focus after
accepted close`. Native private close_requested state therefore prevents
subsequent foreground waits after accepted close/quit; background pumps retain
notification/event draining, and Startup resets the private state. Existing
instrumented native_window_manager passes in0.72s; all five affected Debug
build/non-GPU suites pass at `inactive-close-latch-current`. Actual second
production repetition now passes: `run-inactive-close-latch-asan.sh`, same
optional instrumented configuration/exclusions and unchanged genuine
Mimic-enabled objective11 save in a unique external root. Regular selector,
briefing Start, actual mission frame, normal F8, P, then focus the separate
GCC window and normal compositor-close the unfocused ASan window. No further
focus or gameplay input is sent to that window; wrapper/process exit0 with no
ASan/UBSan report. The pre-close actual capture was inspected. Leak detection
is OFF, external libraries are uninstrumented, and texture dangling references
remain; this is a bounded close-owner test, not fresh full-training acceptance. This private native backend state does
not change persistent layouts or exported module interfaces; synthetic success
is not lifecycle acceptance. Native Windows Debug/Final/ABI,
full training and texture-reference cleanup remain open.

## Full Sol_Train acceptance ledger, M2 (2026-10-12; Linux gameplay accepted)

M2's original training ledger is accepted for the documented Linux runtime
scope below. It follows original Sol_T.cpp and resources, not an invented
campaign objective or a two-objective smoke. M1 original field-hint visible
GCC/Clang evidence is retained; comparative Windows/ABI checks are M3 by
explicit user instruction. Existing dirty changes are preserved and all retail
inputs remain external. Known third-party whole-LSan/Wayland probe failures
remain explicit. No original training objective, balance, RNG-call order or
serialized format is modified to obtain completion.

| Training step | Normal user action | Original responsible owner | Script condition / expected state | Current evidence |
|---|---|---|---|---|
| Briefing / entry | Enter the training and start the mission | Campaign/Briefing menus, Mission, `SolTrain_Briefing/Start` | Briefing controls; source start installs tech/resources/triggers and disables Taos/Acropolis selection initially | Original campaign selector/briefing/Start executed with lawful external data; GCC empty-root full run; Clang fresh initial entry then its own genuine early F7 continuation. Replay/Cancel/return were separately executed. |
| 1 Select Forger | Left-click Forger | ObjectList, `SelectAndMoveForger` | Unique Forger selection completes objective 1 | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 2 Move Forger | Right-click open space | ObjComm, Fabricator, OpAgent | Forger not idle completes 2; enables both Taos selections | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 3 Select three ships | Drag-select Forger and both Taos; Ctrl+digit may create the narrated group | ObjectList lasso, ObjComm group hotkeys | All three selected **and STMG05 finished** completes 3; group membership itself is not a script gate | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 4 Scout | Move a Taos into unexplored Solar territory | ObjComm/Gunboat, FogOfWar, `SelectAndMoveForger` | After STMG06, at least one of Solar 2/3/4 visible to player 1; completes 4, unlocks Oxidator | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 5 Oxidator | Select Forger, build button, place around a planet; wait for construction | CMenu_Fabricator/Toolbar/BuildButton, ObjComm, Fabricator, RefinePlat/Trigger | Player-1 M_OXIDATOR not-ready then ready at TrainTrigger1; completes 5 | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 6 Galiot | Select Oxidator and produce Galiot | CMenu_ResBuild, RefinePlat/HarvestShip/Banker, Trigger | Player-1 M_GALIOT not-ready then ready; completes 6 | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 7 Bunker | Forger build/place on Earth or Swamp planet | Fabricator menu, Planetoid slot placement, build platform | M_BUNKER at Bunker1/Bunker2 trigger radius 4 becomes ready; completes 7 | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 8 Sentinel Tower | Build/finish Sentinel Tower | Same build/control/trigger owners | M_SENTINELTOWER ready; completes 8, unlocks Pavilion | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 9 Pavilion | Build/finish Pavilion | Same build/control/trigger owners | M_PAVILION ready; completes 9, unlocks Helion Veil | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 10 Helion Veil / supply / attack | Build/finish Veil, maintain Taos supply | Platform/Banker/supply, source HelionFinished | Completes 10; >=1 supplied Taos spawns three attacking Mantis frigates; attacker visibility unlocks ESP objective | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 11 ESP coil | Build/finish ESP coil | Fabricator free-space build, platform/Trigger | M_ESPCOIL ready completes 11; source initializes Citadel step if current speech finished | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 16 Citadel | Build/finish Citadel at Earth/Swamp slot | Planet slots, Fabricator, Trigger | M_CITADEL ready at TrainTrigger3/4 completes 16, unlocks Greater Pavilion | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 18 Greater Pavilion | Forger industry G, normal three-slot placement | Fabricator, BuildPlat, Trigger | Source in-progress initialization waits on current stream; ready completes 18, unlocks Annex | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 17 Munitions Annex | Build/finish Annex | Fabricator, Trigger | M_MUNITIONSANNEX ready completes 17, unlocks Anvil | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 22 Anvil | Build/finish Anvil | Fabricator, Trigger | M_ANVIL ready completes 22, unlocks Turbine Dock | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 23 Turbine Dock | Build/finish Dock | Fabricator, Trigger | M_TURBINEDOCK ready completes 23, unlocks Xeno Chamber | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 24 Xeno Chamber | Build/finish Xeno Chamber | Fabricator, Trigger | M_XENOCHAMBER ready completes 24; starts SixPolaris | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 12 Six Polaris | Pavilion produces at least six supplied Polaris | CMenu_BuildInd/ship production, Banker/supply | `CountObjectsInSupply(M_POLARIS,1)>=6` completes 12 (also marks 16), unlocks Eutromil | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 13 Eutromil | Build/finish Eutromil | Fabricator, platform/Trigger | M_EUTROMILL ready completes 13, unlocks jumpgate and invokes original AlertMessage | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 14 Jumpgate | Build on Wormhole to Kasse | Fabricator, ObjComm jumpgate placement, JumpPlat | M_JUMPPLAT ready completes 14, enables source wormhole | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 15 Jump to Kasse | Order force through wormhole; navigate system | ObjComm, jumpgate/Gunboat, Sector/SystemMap controls | Player has unit in Kasse; successive source updates spawn carrier/frigate/scarab/Tiamat waves, then complete 15 | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| Later recommendations / 20 Matrix | Build Talorean Matrix; optionally more force/supply | Source `4and2`, build menu, Matrix/Trigger | After speeches unlocks Matrix; M_TALOREANMATRIX ready completes 20, adds 21 | Normal visible GCC fresh full training and compiler-own Clang continuation complete this original step. Individual UI/model/voice and genuine reload details are in the latest dated runtime ledger; no direct script/objective setter. |
| 21 Combat / actual end | Reveal Kasse 2 and eliminate all Mantis units | Gunboat/weapons/damage/AI, `KillTheMantis`, Mission/end menus | Kasse 2 visible, then `!PlayerHasUnits(2)`; success speech ends, marks 21, emits success teletype; original Done calls `EndMissionVictory(0)` | GCC fresh full run and compiler-own Clang/user continuation finish actual KillTheMantis→21→WON2→Continue1→normal exit0. Agent-visible current final-checkpoint repeats verify end/return/restart separately. |
| Pause/resume | Original Pause key/menu | `menu.cpp::toggleLocalPause`, Menu_Pause, Matrix/Modal/audio | Preserve pause semantics, no simulation advancement while paused; resume original state | Executed GCC pause/updateCount6806 frozen across five seconds; Clang23742 frozen, then normal resume; completed training follows. |
| Early save/reload | Original save/load menu or quick-save hotkeys after selection/scouting | Menu/LoadSave, Mission, ObjectList/Matrix/MGlobals | Same ownership/positions/commands/script/objectives; continue to completion | GCC normal early F7/F8→scouting/builds→victory. Clang compiler-own early1–4 F8→all remaining goals→user-visible victory. Positions, ownership and objectives checked from genuine saves. |
| Later save/reload | Save/load during construction/production or in Kasse | Same owners plus triggers, Banker/supply, combat/media | Pending orders/builds, tech, triggers and script state restored; continue to victory | Original production queue, Eutromil/owned jump, Matrix construction and late supply-escort F7/F8 resume without reissuing orders; GCC and compiler-own Clang continuations reach21/victory. |
| Repeated entry/load/end | Repeat actual mission entry, UI reload and end return | Campaign/menus, GlobalStartup, EventSystem, source text/audio resources | No stale UI/event subscription/media owner; intended end return works | Current regular-close owner check and GCC/Clang/ASan end/restart/reload repeats are recorded in the final M2 ledger below; third-party LSan failures remain explicit. |


**Source-review details.** `MissionData` and each program's save record are
persistent/schema data and must remain unchanged. TrainTrigger1/2 use range
100; Bunker triggers 3/4 use range 4. Trigger filters distinguish player,
object class and readiness. Failures include lost Forger/Taos during initial
training, and the watchdog detecting no Acropolis **and** no Forger. Victory
is source `EndMissionVictory(0)`, not an automatic timeout/quit. Its Done
state tests the original `mhandle` for teletype completion; that source
behavior is recorded rather than silently repaired. Narrated control-group,
research, magistrate and later fleet recommendations must not be promoted
into nonexistent script gates.

**First local hypothesis / smallest check.** Original ObjectList lasso should
reach the three-ship selection condition without a replacement UI. Deliver
ordinary SDL left-down/motion/left-up across all three ships after STMG05;
observe original objective 3 completion. Read-only logging in the existing
shell source-modal observer reports list/completion changes for original
resource IDs 46..69. It creates no objectives or parts and dispatches no
script call; the observer cannot make a missing step succeed. The first drag attempts did not complete 3. A presentation hypothesis was
that native 800x600 ideal coordinates differed from source in-game 640x480:
source `System::ChangeInterfaceRes` confirms the in-game ideal contract, so
native entry now sets 640x480 before `CQE_ENTERING_INGAMEMODE`. Repeating the
same external helper still failed; the proposed causal explanation was
falsified. A debugger launched as the process parent showed ordinary
left-up reaching `ObjectList` with LASSO, focus and cursor ownership, but no
highlighted list. Attaching to an existing process was denied by host ptrace
policy; the parent-debugger run is diagnosis, not acceptance.

The external helper had used compositor cursor warps while holding left
mouse. Those did not supply an intervening regular SDL motion/render frame;
left-up's adapter refresh alone could update the rectangle too late for its
highlight pass. A persistent correctly classified uinput mouse with real
relative REL_X/REL_Y motion rectified the diagnostic input. At an 800x600
client, click Forger near (303,425), right-click open space near (624,480),
then drag from (100,360) to (650,540) using real relative motion while left
is held. Keep all three ships selected until STMG05 completes. Separate visible GCC and Clang runs completed original objective 3 and listed objective 4; the new STMG06 subtitle was inspected in both. No selection, script or simulation repair
was necessary. The shell's automatic post-modal checkpoint remains a
bounded source Save/Reload diagnostic, **not** M2 UI Save/Load acceptance.
Captures, raw logs and external input helpers remain under an external
runtime directory; none is added to the repository.

**Next owner hypothesis / compilation checkpoint.** Objective 5's real
`CMenu_Fabricator` uses original Toolbar controls to set `BUILDARCHEID`,
which `ObjComm` consumes for normal planet-slot placement. Its minimal
production dependencies are original `BaseHotRect`, `Frame`, Toolbar,
ShapeLoader/image readers and the actual control classes. The working
hypothesis is that these owners can use the existing EventSystem, source
resource priorities and native render/font adapter. The first falsification
check is compiling these actual owners, followed by real resource creation,
visible hit testing and normal construction; compilation alone does not
accept that hypothesis. `frontierx_app_toolbar_owner` now links the original Fabricator/individual-ship
command menus, Toolbar/FullScreen, FontFactory, ShapeLoader, image DrawAgent,
HotButton/BuildButton/ResearchButton/MultiHotButton, Static/ProgressStatic,
Edit2, HotStatic, ShipSilButton, TabControl/TabButton, Icon/QueueControl and
InterfaceManager into GPU-enabled native mission startup. These controls
are the actual source owners. Source callback priorities connect Mission
and ObjectList through FullScreen; standalone checks retain the existing
native EventSystem fallback. Toolbar hit regions exclude world highlight,
and original lasso toolbar slop is retained. No new UI implementation or
executable replaces these owners.

**Image/font resource hypothesis and executed diagnosis.** ShapeLoader binds
the actual external Interface archive and retains 32-bit SHP offsets. Its
native reader call supplies the known byte extent. The original image
DrawAgent keeps tiling, geometry, source scale, blend and texture-manager
pool ownership, adapting only upload/storage to the existing RGBA8 native
backend. Its 256-pixel software tile bound is not a fabricated hardware
capability. Missing render dependencies or unsupported framebuffer drawing
fail; they do not return a successful placeholder. Original FontFactory
creates fonts through the existing optional adapter, retains source resource
IDs/multiline/non-scaling flags and paired GenData archetype ownership.
The source numeric cyan font uses the same native face; optimized Windows
numeric artwork and retail font parity are not claimed.

The first real start falsified closure: Tab!!Test needed original TabControl,
and Edit!!Namearea rejected a transparent SHP frame. Source VFX assembly
review and a read-only parent debugger showed the frame's inverted min/max
bounds, which the original drawer treats as empty. Native decoding now
preserves that behavior and absent-palette transparency. Existing shell and
material-manager checks cover generated literal/run/skip packets, empty
bounds, absent palette and truncated/bad offsets. No external image bytes
were copied into those fixtures. The next actual frame exposed null
InterfaceManager at HotButton::draw; its original flash/lifecycle owner is
now connected. `--skip-sector-map` no longer suppresses the linked toolbar;
Sector still requires explicit no-GDI presentation exclusion and does not
subscribe its missing map drawing path. The two map panes remain empty,
so this does **not** satisfy the required M2 map/navigation acceptance.

A visible GCC frame showed original Forger name, Hull 300/300, tech icons,
commands and tabs from the prescribed external art. The previous movement
coordinate (624,480) now belongs to the HUD and is correctly consumed.
Current input uses free field (624,300), then a real-motion lasso from
(100,320) to (650,420); source objectives 1–3 complete and STMG06 appears.
These partial runs do not prove the build button/planet-placement path.
Initial runs crashed on shutdown: Static archetype destruction released its
font after the global GENDATA binding was cleared. A parent-debugger stack
identified this, rather than inferring it from counters. No systemd core was
available; memory pressure/OOM was not observed. The native composition now
executes the original Conquest.cpp close/update/Mission::Close/CleanupGlobals
sequence while render, event, data and heap dependencies remain bound,
then flushes unused GenData archetypes. Separate final visible GCC and Clang repeats showed source HUD and STMG06,
completed 1–3, preserved the moved-state diagnostic source Save/Reload and
exited 0 through cleanup. TextureLibrary still reports dangling texture
references on teardown; process exit does not prove complete resource
release. No crash run counts as a passed acceptance run.

Native `BaseHotRect` retains its original System.cpp posted-message FIFO,
resource release, connection unadvising and destructor queue cleanup. It
validates the existing pointer-sized SDL legacy input records before child
notification. Native Frame preserves its original focus/control/dialog
behavior; only the optional Windows development document viewer/menu is
unavailable. Native source event constants retain original values; runtime
Toolbar/Fullscreen pointers add no persistent fields. Windows declarations,
allocator paths and existing source methods remain selected there.
The existing font-smoke/`portability.native_shell` check now exercises actual
BaseHotRect bounds, missing/mismatched input rejection, focus loss, FIFO
flush, repeated flush and unflushed-message destruction. This synthetic
check uses an unattached root and does **not** prove resource acquisition,
child controls or visible gameplay UI. Optional GCC/Clang builds and that CTest pass. Initial Clang compilation
rejected implicit U32-to-RECT narrowing; explicit coordinate conversion
resolved it. The first ASan execution found malloc/operator-delete mismatch:
BaseHotRect's Engine-heap allocator had no matching native deallocator.
Adding its native `HEAP->FreeMemory` delete preserves the allocation family;
repeat Clang ASan/UBSan `portability.native_shell` passed with leak checking
disabled. SDL3/FreeType system libraries are not instrumented; FFmpeg is OFF
in this sanitizer configuration. The first integrated external-data sanitizer attempt, with training speech
and unit voices explicitly disabled because FFmpeg is OFF, stops at
ObjList.cpp:4280 before mission startup: noncanonical bool value 13 in
BASIC_DATA::bEditDropable. It is a failed instrumented run, not mission
acceptance. The resource/layout diagnosis and next mesh-overflow blocker are recorded below; neither failed run is acceptance. The extended material-manager ASan/UBSan check
with LeakSanitizer enabled failed with 1,409 bytes in 29 original PolyMesh
allocations. Its repeat with `ASAN_OPTIONS=detect_leaks=0` passed; the leak
finding remains open and is not converted into a leak-free result. No parallel UI or probe executable was introduced.

**Executed focused build/check commands (Linux x86_64, Debug).** Existing
optional GCC/Clang directories use GPU ON, FreeType ON, external font fixture
and shared-LGPL FFmpeg audio ON. The existing Clang sanitizer directory uses
GPU/FreeType ON, audio OFF and `-fsanitize=address,undefined
-fno-omit-frame-pointer` for source targets. `BUILD` below is the respective
external build directory, not a new configuration or a retail-data path.

```sh
cmake --build "$BUILD" --target frontierx_app_toolbar_owner frontierx_native_shell frontierx_game_material_manager_probe -j 6
ctest --test-dir "$BUILD" -R '^portability.(native_shell|game_material_manager)$' --output-on-failure
# Sanitizer repeat only:
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir "$BUILD" -R '^portability.(native_shell|game_material_manager)$' --output-on-failure
```

No serialized UI structure or public module vtable was changed. `RECT` stays
four 32-bit words; original resource/event IDs are preserved. Native runtime
pointers/message-width handling are outside saved layouts. Shared Frame
header and include-case changes still require supported native Windows
Debug/Final rebuild/ABI execution, which is unavailable here.

**First actual scout-command blocker.** The existing in-game edge scroll and
right-click field command were tested before assuming the complete map owner
was required for objective 4. With Forger and both Taos selected after STMG06,
a regular right-click at (600,100) reached `OpAgent::bufferedSend`. It requested
`Group!!Default`, but no provider was linked; source assertion `obj` at
OpAgent.cpp:5242 aborted the run (exit 134). This failed run proves neither
scouting nor objective 4. The local hypothesis is that linking original
`GroupObj.cpp` supplies the missing OC_GROUP factory and preserves original
group creation, operation buffering, lifetime and save/reload behavior. The
smallest falsification check is repeating that normal grouped movement. It
joins the existing ObjectList object target; no alternative group owner is
created. Native changes are declaration/include portability only; the
serialized `GROUPOBJ_SAVELOAD` remains two U32 words, now asserted as 8 bytes
with object count at offset 4. Optional GCC/Clang compilation and focused native_shell/data_root checks
pass. Separate visible repeats use two four-second right-edge scroll
intervals at (798,80), each followed by normal field right-click (600,100)
and ten seconds of source simulation. Original objective 4 completes, 5 is
listed, and STMG07 instructs Oxidator construction. Window close returns
exit 0 in both; this is not a victory or UI Save/Load result. Maps remain
explicitly excluded. No simulated success or scripted target change is used.

**Integrated sanitizer blockers and source-preserving fixes.** A read-only
parent debugger identified the first bool report as the `gametypes.h` text
leaf (162,122 bytes, leading CR/LF); ObjectList collects every archive leaf
and had evaluated its byte 4 as BASIC_DATA::bEditDropable. The native loader
now reads that editor flag through bounded unsigned-byte storage and
canonicalizes only the runtime flag. It leaves collected bytes, checksum,
count and enumeration order untouched; Windows selects its original read.
The existing data_root CTest covers generated zero, one, CR and 255 flags,
truncated input and null storage, and asserts no input mutation. The owning
source checks the persistent bool's offset 4 and width 1.

The next instrumented external start passed that boundary, then stopped at
PolyMesh/Mesh.cpp's existing pre-sort: Linux's 31-bit RAND_MAX makes the
signed `rand() * (face_cnt - 1)` overflow. The existing state_material
fixture was extended with three distinct-area faces. Before the source fix,
its ASan/UBSan run reproduced `1804289383 * 2` overflow at the same line;
no external mesh bytes are used. The native loader now widens only that
intermediate product to uint64_t, retains the source ratio and **one CRT
RNG call per face**, and leaves the Windows expression unchanged. The new
fixture checks sorted areas 2, 1, 0.5 and the next CRT RNG value after exactly
three draws. It changes no simulation RNG owner, call count, loop order or
save/mesh format; cross-platform RNG/retail determinism is not claimed.

Optional GCC/Clang complete builds and non-GPU CTests pass after these fixes.
Focused Clang ASan/UBSan data_root, state_material, game_material_manager and
native_shell repeats pass with `detect_leaks=0`, `halt_on_error=1`; the
integrated mission repeat is pending. Existing PolyMesh leak findings are
still open. These checks do not instrument system SDL3/FreeType, and FFmpeg
is OFF in the sanitizer build. Changed shared PolyMesh source still needs
the supported native Windows Debug/Final rebuilds and module ABI checks.

The subsequent integrated attempt found ObjectList's next archetype payload
unaligned after an odd-length archive leaf. Native runtime storage now
reserves up to max_align_t-1 bytes per leaf and aligns each payload before
typed factory access; persistent bytes, payload lengths, checksum and
enumeration order remain unchanged. Windows allocation remains original.
Existing data_root tests exercise every starting alignment and odd payload
lengths. Optional GCC/Clang graphs and focused Clang sanitizer checks pass.

The following integrated attempt reaches mission loading, then UBSan reports
null-pointer arithmetic at FogOfWar::UpdateFog. Source review found
`mapGrid` allocation commented out and its local `gridPos` never read:
only initialization and increment remained. Those unused local operations
are removed; all map-bit calculations, loops, rendering, members and saved
fields remain intact. No simulation or visibility condition is changed.
Its integrated sanitizer repeat is pending.

The next attempt found the same payload alignment defect in GenData when
a real HUD control loaded. Its native traversal now tracks padding separately
from validated bytesUsed; all byte/count/completeness checks and the checksum
still use original lengths and bytes. The first aligned payload stays the
traversal base; reserved padding is bounded by the existing file-count limit.
No GenData schema, stored offset, resource ID or Windows allocation changes.

After this, the instrumented run presents its first mission frame, then
finds TimingStruct::update writing timer_array[TIMING_END]. TIMING_END is
the sentinel/count, not a valid row. Initialization now starts at END-1;
valid timer reset order, simulation update order and all stored/public types
are unchanged. The next integrated repeat passed that boundary, then found
unaligned NETVALUES access in TPlatform::getSyncPlatData. After the native
aligned-copy fix, the following attempt found the same defect in
SpaceShip::getSyncShipData. Both native send/receive paths now copy the
unchanged four-byte record through aligned local storage. Static assertions
retain hullPoints offset 0, supplies offset 2 and size 4; writes precede the
same trueNet state updates. Windows selects the original accesses. These
single-player sync-buffer fixes introduce no multiplayer feature or packet
format change. The integrated repeat is pending; focused Clang ASan/UBSan
data_root/state_material/game_material_manager/native_shell passed after
the platform fix, with leaks disabled. No complete sanitized mission is
claimed.

The following instrumented normal Forger click passed both sync owners but
stopped at UnitComm's source null-member speech-index expression. Native
macros now use compiler-supported offsetof for the same byte/M_STRING index;
Windows retains its expression. Owning-source assertions retain selection 3,
movement 5, gunboat attack 9 and Fabricator construction-start 14. The repeat
passed that point, then reported Nugget's four-byte M_CAPS operator|= loading
an unaligned U32. Its native implementation copies both unchanged four-byte
values into locals, applies the same OR and copies back. The original Windows
branch and packed mission/capability declarations remain intact. Fresh
focused and integrated sanitizer validation is pending.

**Forger reacquisition hypothesis.** A new visible GCC run with planet UI
enabled reached 4/5 and showed STMG07 plus an original resource hint. Its
attempted slash/center input used a US physical key on this German-layout
host; the subsequent world click selected no object. This is not evidence of
construction failure or success. The original CMenu_FindButtons is now linked
for its HUD search controls and normal idle-civilian (period) hotkey, which
selects and centers the real Forger when idle. Native changes are exact-case
include/declaration and pointer-width adaptation; no alternate finder is
introduced. Its initial compile failed at the cleanup pointer conversion;
the native cleanup cast now matches the existing command-menu ownership.
Normal Stop (S), then idle-civilian (period) selected and centered the Forger;
its Hull 300/300 and original build buttons were visually inspected. At an
800x600 client, hover (33,515) shows Construct ACROPOLIS and clicking enters
the source planet-slot placement mode. The first Forger tab (152,487), second
button (75,515) shows Construct OXIDATOR and its source cost/info hint.
The other empty build tabs reflect this training's tech availability; their
emptiness alone is not a missing-control finding. No construction completion
is claimed. Separately the original Banker toolbar subscription/resource
display path is being restored now that its real Static controls exist;
source calculation/update/stall logic remains intact and native fixed-array
formatting is bounded. Its new visible resource-count check is pending.

**First actual placement blocker.** Normal Ctrl+1 stores the selected Forger;
Shipyard B selects Acropolis, the source center key Slash moves the camera to
it, and 1 selects Forger without moving the camera. On this German-layout
host, `wtype -k slash` supplies regular Wayland/SDL key events for the intended
keysym, unlike the US physical uinput slash key. No owner or camera state is
set by the helper. Clicking Oxidator (75,515), then hovering Solar Prime
(350,250), visibly shows its original resource panel (Earth Planet,
ore/gas 2500/2500, crew 3500/3500) and blue planet slots. The native planet-UI
exclusion was omitted for this run. Hovering the right slot (460,250) then
requests the actual Oxidator archetype for the original UnbornMeshList
preview and aborts at missing PLATREFINERY!!S_Oxidator (exit 139).
The hypothesis is that original RefinePlat supplies PC_REFINERY; it now
joins the existing build-support platform owner with native declarations,
exact-case includes and explicit objectpointer conversion. Unsupported editor
insertion still aborts explicitly, and its source Windows path is retained.
GCC/Clang-ASan compilation passes; the real preview/build repeat is pending.

The latest instrumented selection and right-click pass ObjComm's sorted
packet boundary, then stop at OpAgent::createSuperPacket's unaligned ID-array
read. Its native private parameter now takes byte storage and copies the
validated original number of IDs into aligned locals; group resolution,
source IDs, ordering and packet bytes are unchanged. The integrated repeat
then reached unaligned internal ObjSet storage. Native SUPERBASE_PACKET now
caches the original operand byte count outside its regular packet portion;
its ObjSet storage is aligned, and duplication preserves the actual storage
offset. All creation paths (user packet, saved RW header, host operation)
retain explicit source operand lengths, so allocation gaps and native header
tail padding cannot become saved operands. The private runtime header is not
serialized or exported. Static assertions retain BASE_PACKET size 16,
RWPACKET_HEADER size 24/object offset 20/final end 24, and original ObjSet
fields/size. Windows selects original storage. The subsequent real normal
selection/move/drag completes 1–3 under ASan/UBSan, then fails in the source
Debug VerifySet's unaligned array parameter on grouped movement. That native
check now reads a bounded aligned copy without changing its duplicate-ID
logic; the next integrated repeat is pending.

The next integrated Clang ASan/UBSan repeat passed the patched packet
boundaries and reached source checkpoint Save/Reload. Its observed selection
and move objectives completed; the subsequent lasso selected nothing, so
this particular run does **not** prove objective 3/grouped movement. Reload
then reported `new[]`/scalar-delete mismatch for the toolbar collision map:
`Menu_tb::loadInterface` allocates 99,840 bytes with `new U8[]`, whereas
`unloadInterface` used `delete`. The latter now uses `delete[]` in the shared
source path. No simulation or persistent layout is touched. Its instrumented
integrated repeat remains pending. The four focused checks pass with
`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir
/tmp/frontierx-voice-asan --output-on-failure -R
'^portability\.(data_root|state_material|game_material_manager|native_shell)$'`.
Running the same checks with leak detection enabled still fails the two
material checks on the already recorded original PolyMesh leaks; this is
not a leak-free result. System SDL3/FreeType remain uninstrumented.

A later visible GCC repeat shows the connected Banker resource counters:
command points 5/25, ore and gas 5000/12500, crew 200/4000. Normal selection
shows Forger hull 300/300 and the Oxidator button enters original planet-slot
placement mode. The return-to-home/control-group keyboard sequence was not
reliably accepted and no refinery preview was reached in this repeat; its
external timeout exited 124, which is not successful teardown evidence.
The existing shell input/mission observer now records ordinary SDL key
codes and delivered hotkey IDs to distinguish input delivery from command
acceptance without modifying selection, script or command state. The first
actual placement failure above remains the demonstrated integration blocker
until the newly linked RefinePlat owner executes in a real preview/build.

The SDL trace disproved the assumed focus blocker for the automated group
sequence: `wtype -M ctrl` sent a modifier mask but no SDL Ctrl key event;
only group-select hotkey 85 reached the source. A complete regular Ctrl
keydown/up delivers group-create 84. Holding group key 1 long enough for
repeat then invokes the source repeated-group camera centering; one short
`wtype -k 1` retains the home view. The helper now uses actual key events
`wtype -P Control_L -k 1 -p Control_L`, not only the modifier mask.

With a real stored Forger group, normal Oxidator-slot hover reaches the
linked RefinePlat archetype, but crashes in Engine compound-instance
creation (SIGSEGV, exit 139). A systemd core symbolized against the exact
GCC executable proves `parent_instance == nullptr` at the final root
insertion, reached through UnbornMeshList and Planetoid rendering. Two
referenced smoke-part archetypes had failed before the root insertion;
no successful preview/build is claimed. The extracted temporary core was
deleted and diagnostics remain external. Source review finds regular
`menu.cpp` binds `ENGINE->set_search_path2(OBJECTDIR)`, absent from the native
composition. The native mission now applies that original binding and
restores/releases its preceding factory at teardown; the normal slot-hover
repeat is pending. No missing compound part is replaced or silently declared
implemented. Original SolarianBuild now also joins the existing platform
owner: the real TFabricator construction caller requests `BUILD!!Solarian`
and its IBuildEffect, previously missing. GCC and Clang sanitizer builds
compile this unchanged gameplay/render implementation through native
declarations and exact-case includes; actual construction remains pending.

The next instrumented mission completes objectives 1–3 and sends normal
grouped movement, then saves its source diagnostic checkpoint. Reload passes
the collision-map array destruction and next reports malloc/scalar-delete
mismatch for HotButton text. Native DuplicateWide and Windows `_wcsdup`
both allocate CRT storage; HotButton destructor and both text setters now
use `free`. Static's corresponding strings already use `free`; the other
controls that include the adapter do not call DuplicateWide. Allocation,
IDs, text lifetime and interface layouts are unchanged. The integrated
repeat after this owner correction is pending.

The real GCC slot-hover repeat after OBJECTDIR binding and SolarianBuild
still crashes: the referenced smoke files now reach TextureLibrary, but their
archetypes are rejected. A subsequent normal Clang ASan/UBSan run reaches
1–4 and the same slot hover, reporting Engine's null parent access rather
than a successful preview. Read-only inspection through the existing UTF
metadata implementation finds `ssmoke.pte` uses a 64×64, 8-bit indexed
`partic3.bmp` texture. The native TextureLibrary format chooser admitted
only RGB565, and the native upload adapter admitted only RGB565/RGBA8.
Both now admit the original 8-bit indexed format, mapping its RGB palette
and optional separate alpha plane into the existing RGBA8 GPU texture owner.
Stride, dimensions, palette requirement and upload errors remain checked;
no serialized format, source particle update/RNG or Windows format selection
changes. Other texture formats remain unsupported explicitly.

Existing `portability.render_pipeline` adds a generated two-row indexed
texture with padded stride, palette entries including 255, exact RGB/alpha,
opaque fallback without alpha and rejection without palette while retaining
the previous committed texels. The initial fixture incorrectly selected
True-8-bit; it was corrected to the source loader's `init(8,0,0,0,0)`.
That corrected fixture was rerun against the preceding adapter and failed
as expected, then passed after restoring the conversion. Existing
`portability.game_material_manager` adds generated indexed UTF content to
its shared legacy mesh fixture writer and exercises actual TextureLibrary
load into the native pipeline capture. Its captured backend now distinguishes
live 2×2/4×4 handles and reuses only released IDs. The production-chain test
fails before the chooser extension. These remain deterministic synthetic
checks, not real particle rendering or training acceptance. Their final
GCC/Clang/sanitizer focused repeats now pass. The later real GCC run
completes 1–4, shows valid green planet-slot placement, and passes the
formerly crashing normal hover. Normal left-click (460,250) reserves 500
ore (HUD 5000→4500), exits placement mode, plays the original Forger order
response and sends it back toward Solar Prime. The original not-ready/ready trigger then completes 5 and lists 6, with
SOX3652/STMG09, visible completed Oxidator, selection voice/text and resource
production. Normal window close returns 0. This no-smoke run creates no
diagnostic save; the older moved-state checkpoint is not a late build save. Its initial narrow lasso selected nothing; regular End toggled the
original toolbar off, a larger real-motion field lasso (100,280)→(700,430)
selected the visible ships, and End restored the HUD. Step 3 then completed
through its original condition. The input helper's timeout is not mission
failure; the same living mission was continued with regular events.

Existing portability.native_shell now checks actual OpAgent createPacket,
duplicatePacket and saved-header createSuperPacket on generated operand
lengths 0–7 and two source-sized IDs. It checks aligned distinct set storage,
exact operand byte length, IDs and operand bytes after duplication/loading.
It submits no operation and changes no mission state. The check is part of
the existing synthetic sfx-smoke process test in GPU-capable builds, requires
no actual GPU or retail input, and its output is required by the Python
CTest. This targeted regression and the four focused leak-disabled Clang
sanitizer checks pass. UI later Save/Load with active commands remains required;
synthetic storage checks do not establish that acceptance condition.

**Current limitations / next work.** M2 remains in progress. Normal GCC input
completes 1–5, including real Oxidator construction; separate Clang input also completes construction 5 and reaches production;
integrated sanitizer ownership repeats remain required. Oxidator selection
shows an empty production area without original CMenu_ResBuild. Local hypothesis:
its missing static registrar prevents the original S_Oxidator panel and Galiot
command. Smallest falsification check: select the completed Oxidator normally and
inspect its original production button, then issue its command through SDL.
CMenu_ResBuild now joins the existing toolbar owner, with native declarations,
case-correct includes, pointer-width event payloads and bounded source text
formatting. Its original controls/events/production logic and persistent layouts
are unchanged. GCC/Clang and Clang ASan/UBSan native-shell builds and existing
`portability.game_material_manager`/`portability.native_shell` checks pass
(`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1` for sanitizer checks).
Separate visible Clang normal input completes 1–5, selects the finished
Oxidator (460,250) and displays source S_Oxidator production controls, location,
hull, ore/gas stores and Galiot button. Expected narration/subtitle/STMG09 and
SOX3652 play. Normal Galiot hover/click (30,525) reaches source RefinePlat production,
then missing `HARVEST!!S_Galiot` produces exit 139. This failed production run
is not lifecycle success or step-6 completion. diagnose-crash source/core review
confirms ObjectList::AddObject(null) at 2291 from RefinePlat::update at 1601,
after CreateInstance fails for the absent provider. No null success placeholder
is added. HarvestShip's original registrar now joins the existing spaceship
owner with native declarations/case-correct includes and source template
instantiation; unavailable editor insertion fails explicitly. Its actual
gas/metal tank creation requires original MeshObj, now compiled in the existing
mesh-render owner. This source-owned mesh lifecycle is retained, rather than
replaced by a dummy tank. GCC/Clang and Clang ASan/UBSan native-shell builds
and the two affected existing CTests pass; Normal Clang production repeat now creates the Galiot and queue/STMG10.
Original objective 6 completes and 7 is listed. During this run physical outputs
vanished: Hyprland reports only FALLBACK, leaving workspace 1 without a monitor.
Moving the same living window normally to mapped workspace 3 resumes rendering/
updates; the completed Galiot is visible in the captured SDL surface. This is
actual production/input evidence with external data, not a complete physical
output acceptance run. No mission state was edited or script invoked. Unloaded
FBALL/PART explosion effects reported by its archetype remain open dependencies.
A separate regular GCC launch also completes normal 1–6, shows the completed
Galiot, original Oxidator controls/hint/resource stores and closes with exit 0.
It uses the same lawful external data/optional FFmpeg/FreeType configuration,
no-smoke launch and normal helper events on FALLBACK. It creates no save.
Actual Galiot archetype initialization reports missing `FBALL!!Solarian` and
`PART!!BSB_WhiteBlue_MED`, `PART!!SB_WhiteBlue_MED`, `PART!!Ripple_WhiteBlue_MED`.
Local hypothesis: the original FireBall/ParticleObj registrars, absent from the
native closure, own these actual later combat dependencies. The smallest runtime
check is the same normal Galiot production with real archetype load and no missing
provider reports; actual effect spawning/rendering requires the subsequent
combat path. These original sources now join the existing blast owner; GCC/Clang and
Clang ASan/UBSan shell targets and focused material/shell checks pass. All four
default/optional graphs also pass at this compilation checkpoint. Actual effect
spawn/render and normal Galiot repeat without missing provider reports remain
required. No successful effect dummy is added.
The next normal Forger build menu shows the newly unlocked Bunker button,
with the original Earth/Swamp hint and 500-ore cost. Normal close of this
1–6 run exits 0; it creates no UI or diagnostic save. Initial keyboard touring
used press/release in separate wtype devices; subsequent complete same-process
key sequences and normal slash centering restore expected camera navigation.
No production input rule is changed on that observation alone. The subsequent
integrated Clang ASan/UBSan diagnostic attempt aborts at ObjMap's 32-bit
partial-pointer poison heuristic at line 140. No ASan memory-access error is
reported at that stop. Full-width poison versus a valid native address remains
a diagnosis hypothesis, not yet a proven fix. No core was generated by this
ASan process. A parent GDB conditional breakpoint and ordinary input reached
1–3, Save and Reload but its first 90-second cap expired during reload; this
is not a successful sanitizer/lifecycle result. The longer (240-second cap) parent-GDB diagnostic reaches normal input 1–3,
source modal exit, moved-state Save/Reload (Forger owner 1, changed position),
training checkpoint preservation and clear hint/timer/ownership close. The
inferior exits normally; GDB's wrapper returns 1 because subsequent diagnostic
`p obj`/backtrace commands have no live frame. The conditional breakpoint never
fires, so it does not prove the earlier pointer's validity. The observed mission
lifecycle passes the toolbar/HotButton corrected boundaries with no reported
ASan/UBSan error, under explicitly skipped speech/voice and disabled leaks on
FALLBACK; system SDL3/FreeType remain uninstrumented. This diagnostic checkpoint
is not UI save/load or complete training acceptance. A regular repetition with failure-only native pointer logging also completes
normal input 1–3, source moved Save/Reload, training preservation and hint/resource
close, returning 0 without a reported ASan/UBSan error. The pointer check never
logs/fails on that repeat; its earlier cause remains unproven and the check is
unchanged. Speech/voice are explicitly skipped, FFmpeg is disabled, leak checking
is off, and system SDL3/FreeType internals are uninstrumented. This passes the
bounded corrected allocation-family lifecycle, not construction/combat/UI Save
or complete M2. The existing start observer now reads each planet's original
mission name/system/position/projection once through IBaseObject::GetMissionData,
without changing the camera, fog, orders or goals, to diagnose the missing
normal-map/bauplatz navigation. Bunker's original archetype is PLATREFINERY!!S_Bunker,
already handled by RefinePlat; no GeneralPlat owner is introduced on a guess. The native template
closure hypothesis is checked by linking, then actual Galiot production remains
the required check. No HARVEST save record is changed. Core extraction used a
fresh temporary file, deleted after symbolization; captures/logs remain external. All four complete default/optional
GCC/Clang graphs and non-GPU checks passed after this menu addition; results/commands are centralized in BUILD_GRAPH.md.
All subsequent production/supply/combat, Kasse navigation, regular entry,
pause, UI early/late Save/Load and actual victory/return remain unperformed.
Existing shell-triggered checkpoint reload is diagnostic only. Presentation/media
exclusions remain explicit, with their impact on full M2 acceptance open.
Native Windows Debug/Final and module ABI tools are unavailable here and remain
an external execution gate, not a success. Linux missing production owners are
internal work, not an external blocker.

### Mission capability correlation correction (2026-10-11)

The first blocked post-reload map command was traced through normal SDL
right-click, original `SysMap::Notify/checkCursor`, selection and `MPartRef`.
Read-only parent-GDB observation found a selected Taos with `moveOk` and
`attackOk` cleared. This diagnosis is not a gameplay acceptance run.
The local hypothesis was that the saved schema disagreed with the actual
`MISSION_SAVELOAD` capability field. The smallest countercheck extended the
existing `portability.app_schema` CTest with production `CorrelateSymbol`:
it failed with schema/native offsets 72/70 and correlated move/attack 0/0.
The external original Sol_Train schema independently reports offset 70,
size 4, alignment 1 and a record type.

`DMBaseData.h`'s `_ADB` declaration and its checked-in `data.i` resource now
name the existing defining `MISSION_DATA_BIN::M_CAPS` record. The runtime
`MISSION_DATA::M_CAPS`, C++ layout, widths, IDs and actual bytes written by
SpaceShip are unchanged. This corrects the misleading U32 schema substitution;
it does not introduce a new save format or add capability initialization on
reload. The existing parser check exercises all current capability bits,
the named padding byte, adjacent group/operation fields and the inherited
mission record in GUNBOAT/FABRICATOR/HARVEST saves. The layout check pins
size 104, capability offset 70, groupID offset 76 and final-field extent.
The external schema check additionally correlates in both directions for all
retail-declared capability bits. Retail predates the final two CQ2 capability
fields; those absent fields are not claimed as an external round-trip.

Reproduction (existing Linux x86_64 Debug optional Clang tree):

```sh
cmake --build /tmp/frontierx-voice-clang --target frontierx_view_parser_probe frontierx_layout_probe -j4
ctest --test-dir /tmp/frontierx-voice-clang --output-on-failure -R '^portability\.(app_schema|view_parser|layout)$'
/tmp/frontierx-voice-clang/frontierx_view_parser_probe --mission-schema "$CFW_DATA_ROOT" Sol_Train.dmission
```

The focused checks and external schema correlation pass. All four default/optional
GCC/Clang Debug graphs and the Clang ASan/UBSan Debug tree built with
`cmake --build BUILD -j4` and passed their non-GPU CTest selection using
`ctest --test-dir BUILD --output-on-failure -E
'^portability\.(sdl3_gpu|game_material_gpu)$'`. Sanitizer CTests used
`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1`, with system
SDL3/FreeType uninstrumented and FFmpeg off; this is not full integrated
mission or leak acceptance. A fresh physical-output
Clang normal-input run repeats original 1–4, cancels the build preview, opens
P pause, resumes with Escape, writes F7 and loads F8 in a separate external save
directory. The reloaded Taos retains the visible movement/weapon command panel,
Hull 80/80 and Supplies 75/75. Normal right-click on the original map now shows
its move instruction, plays the original movement voice and visibly moves the
selected Taos out of the stationary pair. This is bounded early UI save/reload
and command evidence, not complete M2. Fresh construction continuation completes 5–10, including a later Bunker-state
F7/F8, as detailed in the next owner-chain checkpoint. Earlier Helion saves and reloads were created under the incorrect
schema; completed objectives/visible construction in those runs do not prove
preserved command capabilities. They are not used as repaired-state acceptance.
Native Windows Debug/Final rebuild and module ABI remain unavailable and open.
No retail files, captures or logs enter the repository.

### Original map texture and active-program reload chain (2026-10-11)

Source review and bounded native external-data runtime identified two owner
gaps after the capability correction. The fresh physical-output Clang run
continues from early normal F7/F8 through Oxidator/Galiot 5–6, Bunker 7,
a second normal F7/F8, Sentinel/Pavilion/Helion 8–10 and normal close (exit 0).
These are fresh saves made with the corrected capability schema. They do not
establish continuation to victory or the corresponding full GCC run.

The original SysMap ITManager caller initially had background/terrain/unit
handles zero in a read-only parent-debugger observation. NativeShell had not
bound `TEXTURESDIR`. It now binds the lawful external texture root through
the existing NativeAppDataSources AddRef/Release owner. The explicit existing
`--base-textures-only` option now also governs ITManager's source mip-count
request, with scoped restoration on shell exit. Native pipeline texture
creation supports the original DAOT opaque class and DAA1/DAA4 through the
RGBA8 fallback already allowed by the original D3D driver. Opaque uploads and
RGB-only locked icon updates retain alpha 255; alpha-class uploads retain
source alpha. Multiple mip levels remain explicitly unsupported.

The existing material-manager CTest now loads a generated 16x16 TGA through
this actual ITManager/directory chain, checks opaque and alpha pixels, locked
RGB updates, unsupported mip rejection and directory teardown. Its initial
opaque creation countercheck failed before integration. No external texture
bytes enter that fixture. Physical-output Clang F8 shows map background and
markers, and normal field selection/right-click map order moves a Taos with
visible commands, Hull 80/80 and Supplies 75/75. The displayed map content is
small because the explored region is bounded; this is not evidence of a
missing full map image. Source `Sector::Render` requires full-screen-map mode.
The original toolbar `IDH_FULLSCREEN_MAP` handler, commented in this CQ2
snapshot, is now enabled only for the native build. A separate physical-output
Clang run uses normal Insert to open the two-system map and Insert to restore
the HUD. Ordinary mini-map left-click centers Solar Prime; subsequent field
selection and right-click map orders visibly move a Taos. Full navigation,
Windows parity and the full training remain unaccepted.

More critically, the fresh Helion UI save contains its original active
program, but read-only diagnosis did not observe its Update after F8.
`MScript::loadProgramData` requests `\\Script\\Programs`; the native read-only
UTF owner rejected absolute virtual CreateInstance paths. The existing
data-root CTest now writes a synthetic active-program leaf, reopens the save,
requests that exact original directory from a different current directory,
enumerates `?*-*`, reads its bytes and rejects traversal. After correcting the
fixture's parent-directory setup, this countercheck fails against the old
owner and passes with archive-only absolute virtual paths enabled. Physical
absolute paths and traversal remain rejected; disk formats, script records,
program order and simulation are unchanged. The newly reachable native
program enumeration closes its original FindFirstFile handle after loading.
The repeated read-only parent-debugger F8 diagnosis now observes original
Helion Update in saved Attack state with a valid attacker reference; both
diagnostic processes close normally (exit 0). A regular visible continuation
is being rechecked; the synthetic/debugger results do not accept M2. Earlier
Bunker reload permitted fresh construction triggers, but had silently lost
active programs and therefore did not establish full script preservation.

Focused commands (Linux x86_64 Debug optional Clang):

```sh
cmake --build /tmp/frontierx-voice-clang --target frontierx_game_material_manager_probe frontierx_data_root_probe frontierx_native_shell -j2
ctest --test-dir /tmp/frontierx-voice-clang --output-on-failure -R '^portability\.(game_material_manager|data_root)$'
```

Native Windows Debug/Final rebuilds and module ABI execution remain unavailable
on this Linux host. No Windows/retail comparison, full training, integrated
sanitizer mission or leak acceptance follows from these focused checks.

### Empty active-program save storage (2026-10-11; bounded GCC reload verified)

The separate physical-output optional GCC run normally pauses/resumes and F8
loads the unchanged Clang UI Helion save, then segfaults in MScript::Update.
Its matching native executable/core identify active SolTrain_STowerFinished,
`saveOffset=0`, `saveSize=1`, with the vtable overwritten to STowerInProgress;
virtual Update then lands in typeinfo. No OOM/resource exhaustion is observed.
Temporary extracted core copies are removed after diagnosis. This is a failed
GCC runtime, not cross-compiler saved-state acceptance.

**Local hypothesis / smallest countercheck.** Native empty-base optimization
puts empty ProgSave at the CQBaseProgram vtable address. The original registration
macro saves sizeof(empty)==1 there. Extend the existing layout CTest using the
actual macro's native save-base selection: require the empty save span to be
after the polymorphic base and verify arbitrary one-byte record loading leaves
virtual dispatch intact. It must fail against direct empty inheritance, without
calling through a knowingly damaged vtable. Give only native empty Save bases
one real runtime storage byte; retain original persistent Save definitions,
sizeof/record lengths, registration layout and Windows macro path. The existing `portability.layout` countercheck first compiles and fails against
direct empty inheritance, then passes under GCC and Clang after the native-only
storage byte. The separate physical-output optional GCC repeat normally uses
P/Escape, F8 on the unchanged Clang UI Helion save, Insert open/return, another
P/Escape/F8, and normal window close (exit 0). Both loaded frames are visually
inspected; the former virtual-dispatch crash is absent. This verifies bounded
cross-compiler loading, not complete state preservation or end continuation.
The bounded integrated ASan/UBSan P/Escape/F8/map/repeated-F8/close repeat now passes, with the precise exclusions below; full mission/end and leaks remain separate gates.

### Integrated sanitizer load boundaries (2026-10-11; bounded repeat passes)

Environment: Clang Debug `/tmp/frontierx-voice-asan`, SDL GPU/FreeType enabled,
FFmpeg disabled; `ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1`.
System SDL/FreeType, Vulkan driver and external media libraries are not
instrumented. The ordinary bounded P/Escape/F8 repeat excludes training speech
and unit voice in addition to the documented visual/media exclusions. It is
not a complete training or leak check.

First attempts fail, rather than skip: the original BMP decoder dereferences
its header behind a 14-byte file prefix; the next attempt reaches a valid
64-bit ObjMap link whose middle byte falsely matches the old 32-bit poison
mask; the next F8 reaches an unaligned variable-length RWPACKET_HEADER in
PendingOps. Corrections remain at existing ownership/storage boundaries:
aligned native BMP input, full-width native poison detection (plus the old
sentinel), and aligned validated packet-header/record copies. Original Windows
branches, pending record lengths/bytes and command ordering remain unchanged.
Existing material check covers unaligned BMP colors/truncated header; existing
shell operation-storage check now feeds unaligned records with every supported
short operand length, preserving object IDs and operands. Those focused checks
pass under leak-disabled ASan/UBSan. The current integrated repeat then normally
uses P/Escape, F8 on the unchanged UI Helion checkpoint, Insert open/return,
a second F8, and normal window close; both loads reach source completion and
source 9/10 remain completed. Exit is 0 with no ASan memory-access report or
UBSan diagnostic. This does not accept construction/combat/end, voice/media,
leaks or complete saved-state parity. The early fixed-delay reload capture is
before load completion and is not counted as a loaded-frame sight check.
TextureLibrary dangling-reference warnings remain open. The separate ordinary
non-sanitized GCC/Clang visual evidence remains necessary.

### Original briefing entry owner (2026-10-11; bounded visible entry verified)

**Local hypothesis / smallest check.** The selected native training can enter
through original MenuBriefing and its regular Start button, instead of a direct
mission Load. Real source Menu_mission/Campaign selection calls
`DoMenu_Briefing`, which owns Mission::LoadBriefing, source briefing programs,
Button2/Static/Animate resources, then Start calls original interface transition
and Mission::Reload. Native CQDoModal currently rejects any dialog pointer;
Button2/Animate/MenuBriefing are outside the closure. Compile those original
owners in the existing toolbar target, connect the actual dialog parent to the
existing modal loop and preserve original interface-mode notifications at the
native composition boundary. The smallest runtime falsification is an unchanged
external Sol_Train selected-entry launch failing its original briefing resource
or normal Start/Cancel. No source program, objective, automatic Start input or
replacement UI is introduced. Persistent records and Windows branches must
remain unchanged. Compilation does not accept the regular entry.

**Executed integration.** `--mission-briefing` with visible `--mission-start`
now calls original `DoMenu_Briefing(nullptr, selectedName)`. Existing Button2,
Animate and MenuBriefing owners join the toolbar closure; native CQDoModal
subscribes to the real dialog parent and handles normal window close. Native
interface changes retain source IDs and leaving/entering notifications. The
native renderer clears menu frames before its existing begin/end/present cycle.
MScript binds the same built-in Sol_T source/resource module as mission load,
then invokes its original STARTBRIEFING entry; Start uses source Mission::Reload.
No automatic Start, source-program call from the shell, replacement menu or
objective manipulation is used.

First real failures identify the required image/layout dependencies: original
Static!!Briefing uses an 800-by-600 uncompressed 8-bit indexed TGA with a
256-entry 24-bit palette, zero map start. The existing source reader now validates
that bounded form before its original palette decoder. The existing material
CTest checks colors and rejects truncated/out-of-palette synthetic input.
Button!!Prim3D also exposes GCC/Clang's three-byte bitfield allocation versus
the four-byte Win32 schema: a native padding byte restores fontName offset 8,
shapeFile offset 49 and archetype extent 84. Existing layout CTest pins those
facts; Windows declaration is unchanged. IAnimate's native frame-rate method
is pure virtual, preserving its slot/signature and the real Animate override;
its source base declaration has no implementation. No successful base stub is
provided. These are runtime interfaces/archetypes, not changed save formats.
Native Windows Debug/Final rebuild and module ABI remain unexecuted.

Separate physical-monitor optional Clang and GCC launches visibly render the
original background, four TechLoop animations, subtitle/teletype and source
buttons. Normal Clang Cancel returns and closes cleanly; another launch's Start
loads the actual initial mission. Ordinary selection/right-click movement
completes 1 and 2, lasso selects the ships, then F7/F8 loads an early UI checkpoint
and normal close exits 0. GCC normally clicks Replay, then Start; initial HUD
and original first training instruction are visibly inspected; P/Escape and
normal close exit 0. GCC's first attempt ran an older executable while its
rebuild was in progress and hit the previous explicit briefing abort; that
attempt is failed, and only the subsequent current executable run is evidence.
This is a selected native mission entry with an original briefing. The full
campaign selection frontend, later briefing speech/portrait lifecycle, all
training/end states and return transition are still unaccepted.

Reproduction: append `--mission-briefing` to the ordinary visible mission
command in this document, retaining external data/font/profile, GPU shader,
string-pack/gen-data/game-types/hotkey/save-root arguments and the stated media
exclusions. At 800-by-600 normal window coordinates, Start is near (348,543),
Replay (510,543), Cancel (665,543); these are regular pointer inputs, not calls
into dialog/program state. Synthetic CLI checks reject duplicate briefing,
briefing without mission-start and hidden briefing. Host-dependent visibility
remains separate from CTest.

### Initial Mantis jump-platform dependency (2026-10-11; fresh-owner evidence, attack pending)

The ordinary mission-load diagnostic reports no production provider for
`PLATJUMP!!M_JumpPlat`. Original JumpPlat links to/owns its wormhole and is also
required later by training 14. **Local hypothesis:** omitting that initial
Mantis platform leaves the Kasse outgate unseen when HelionFinished immediately
orders the three created frigates; restoring the actual owner should permit
those unchanged orders. Compile original JumpPlat in the existing build-support
closure, retaining its simulation/update/save methods. The smallest runtime
countercheck is the same ordinary Bunker UI reload and normal 8–10 construction:
read-only command-path observation must show a valid route without artificial
visibility, retry or source-script changes. If still rejected, this hypothesis
is false. No acceptance follows from the build alone.

Original JumpPlat now compiles/links in the existing build-support closure.
Its gameplay, creation, sibling, gate/supply, save/load and update methods stay
original; native adapters provide existing global/render types and explicitly
reject unavailable editor insertion. A new ordinary selected briefing/Start,
then F7 save shows both actual Mantis JumpPlat instances (400-byte records),
with both original gate ends now explored by player 2. The older UI save had
neither platform and command-time gate flags zero. No object is injected into
an old checkpoint: runtime verification proceeds from a fresh mission. Existing
app_schema now correlates the actual JumpPlat gate ID and ownership/lock fields
and checks its C++ record extent against the generated source schema. Native
Windows/retail source comparison and full attack/end remain open. Fresh regular
entry continues via normal field selection, right-click movement, lasso and
map orders through 1–4, then original green-slot Oxidator/Galiot controls through
5–6. Normal F7/F8 preserves that fresh production checkpoint and continues to
the Bunker instruction. Both JumpPlat instances originate from the real initial
mission; previous owner-omitting checkpoints are retained only as bounded
historical evidence. Reload clears selection normally, so subsequent construction
requires a regular Forger field selection/control group.

The fresh physical-output Clang continuation completes original 7–9 with normal
Solar 3 placement. The fixed Pavilion coordinate from the earlier run is no
longer valid after this different placement: the input driver times out; normal
hover finds green (568,200), and that click actually completes 9. Helion tab
(174,487), icon (30,515), green slot (440,156) completes 10. All three original
script frigates then initiate and complete the source jump to system 2. A normal
F7 checkpoint shows them there with nonzero original move agents and changed
positions; no retry, order injection or artificial visibility is used. Source
objective 11 is subsequently listed, with the visible STMG19 subtitle directing
ESP construction. Ordinary F7 records that distinct later state in external
`fresh-jumpplat11-ui-clang.mission`. This is native runtime on external data,
not synthetic evidence; GCC fresh attack, ESP construction, later training,
reload-to-victory and full acceptance remain open.


### Zero-alpha map texture mask (2026-10-11; integrated sanitizer repeat pending)

Ordinary ASan/UBSan F8 on the actual ESP-complete UI save fails after load at
PixelFormat::init: `8 - INT_MIN` signed overflow. A separate stack-enabled repeat
confirms normal SysMap::DrawIcon -> NativeRenderPipeline::get_texture_dc /
lock_texture -> OpaqueRgbx8 -> PixelFormat(DDPIXELFORMAT). The absent alpha mask
never initializes awidth before the old shift calculation. **Local hypothesis:**
zero channel masks must be tested before reading their width; smallest check is
the existing material-manager CTest with a valid RGBX32 mask and previous alpha
width INT_MIN. The unchanged code reproduces the exact UBSan overflow; native
conditional mask checks pass this countercheck. DDPIXELFORMAT extent, class
members, valid-channel shift math and Windows code remain unchanged. Logs/captures
remain external. The two failed integrated runs exit 1, not acceptance; the fixed
instrumented actual mission repeat and affected full builds remain required.

### Gunboat packed target sync (2026-10-11; actual sanitizer countercheck)

The fixed zero-alpha ordinary ESP-save F8 repeat passes map texture locking,
then stops with UBSan at Gunboat::getSyncTarget's U32 store to a byte-packed
address. The symbolized normal chain is Mission::Update -> OpAgent::sendSyncMessages
-> FRAME_getGeneralSyncData -> Gunboat::getSyncTarget. **Local hypothesis:** this
raw-buffer sync boundary requires byte-safe copies of the same four-byte target
ID; smallest countercheck is the same ordinary F8/update under UBSan. Native
get/putSyncTarget now use memcpy at the existing raw byte boundary, retaining
Target/netTarget update sequence, source setAttackTarget calls, packet extent
and Windows typed accesses. No additional simulation call or packet bytes are
introduced. Focused existing checks and the fixed actual integrated repeat
remain required; the preceding exit-1 run is not sanitizer success.

### ESP archetype allocation pair (2026-10-11; second real F8 countercheck)

After the pixel-mask and Gunboat-sync corrections, the first ordinary ASan/UBSan
ESP-complete F8 reaches the real loaded platform/map/attacker state with no prior
diagnostics. A later ordinary F8 fails at Mission::close/ObjectList archetype
release: FancyLaunchFactory::OBJTYPE allocates through class calloc but has no
matching class delete, so global operator delete is called. This exit-1
alloc/dealloc mismatch is not disabled. **Local hypothesis:** class delete must
use the same allocation family; the smallest countercheck is ordinary repeated
F8 after loading/constructing the actual ESP. Native-only class delete/free is
added to FancyLaunch and Arc OBJTYPE, whose adjacent allocation is also calloc;
source destructors/reference release remain unchanged, Windows allocation code
and all fields/layouts stay intact. Focused existing instrumented checks pass;
repeated actual F8/close, other affected builds and Windows remain execution gates.

The fixed instrumented normal pre-ESP F8 repeat no longer reports these prior
errors. The first fixed clear-space construction order remains in original
Forger movement (normal F7: no ESP instance, FAB_MOVING_TO_TARGET, actual target
16.5/20.5); it is not counted as completed construction. Normal S cancels/refunds
the reserved resources/command points; normal center/L and a nearer clear-space
click (450,350) retry through production input. No pathfinding, source order,
trigger, object or mission state is injected or changed. This stalled approach
is an explicit runtime limitation until characterized; even a subsequent build
will not erase that evidence.

### Objective compaction overlap (2026-10-11; actual construction countercheck)

The nearer normal ESP retry reaches the actual ready callback, but the
instrumented mission exits 1 in SolTrain_TeslaFinished::Initialize ->
MScript::RemoveFromObjectiveList -> MGlobals::RemoveFromObjectiveList. ASan
reports overlapping memcpy on the original objective-ID compaction. This is
not completed sanitizer objective-11 acceptance. **Local hypothesis:** the
three overlapping ID/state/secondary-list shifts need overlap-safe copies
without changing ordering or any goal. The existing native_shell CTest now
calls the original add/complete/remove functions on a full isolated list,
checks middle removal, completion/secondary correspondence, tail clearing,
last removal and a missing ID, and restores its previous objective state.
The unchanged implementation fails with the same ASan memcpy-param-overlap;
native-only memmove for the three original shifts passes this countercheck
and focused native_shell/app_schema/layout/game_material_manager checks.
Source objectives, copy lengths, events, serialized global data and Windows
branches remain unchanged. Actual normal construction/repeated F8/close and
native Windows Debug/Final/module ABI remain required. All five affected trees
rebuild and pass their existing 49 non-GPU CTests (objective-final checkpoint,
commands/configurations in BUILD_GRAPH). This isolated check
is synthetic evidence, never a replacement for completing gameplay objectives.

### ESP sanitizer construction/reload checkpoint (2026-10-11; bounded pass)

After the four actual sanitizer corrections above, the separate visible
Clang ASan/UBSan mission repeat passes actual normal ESP ready/objective
transition: objective 9 removed, 11 completed, 16 listed. The physical
HDMI-A-1 output was inspected with the ESP model, Forger, map and original HUD.
This run starts through the documented native mission command in
`/tmp/frontierx-voice-asan`, with an unchanged ordinary-F7 pre-ESP checkpoint
as the external save-root's f0000000.mission. `ASAN_OPTIONS=detect_leaks=0`
and `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1` are active; allocation
mismatch checks remain enabled. The documented optional command adds
`--skip-training-speech --skip-unit-voice` because this instrumented tree
has FFmpeg off. Other explicit mission exclusions remain as listed above.

Normal P/Escape/F8 loads objective 11; F selects Forger and slash centers the
camera. Several ordinary placement attempts (450,350; 340,270; 300,150),
normal military tab (222,487)/L and another (300,150), followed by normal
S/slash/L and nearby (450,350), execute through the original command owners.
Multiple placements reserve command points; these are not a single minimal
placement recipe. The final source ready transition is observed before
acceptance; the earlier stalled approach remains recorded. F7 saves the real
completed-11 state externally (esp11-complete-ui-asan.mission). Two normal
F8 reloads of this same save complete; between them Insert opens the original
map, normal wheel zooms it and Insert returns. After the second reload,
F/slash, P/Escape and normal window close return the source modal and close
the mission. The process exits 0 with no ASan or UBSan diagnostics. Captures,
logs and checkpoints remain external (esp-objective-repeat-asan run).

This exercises changed map-mask, target-sync, ESP construction/objective
compaction and archetype destruction owners on actual retail-data native
runtime. It is not a full training, combat, speech or leak acceptance. System
SDL/FreeType/GPU-driver code is not instrumented; existing TextureLibrary
dangling-reference warnings remain. Windows Debug/Final/module ABI and actual
Windows/retail comparison remain unperformed.

### Original ESP owner dependency (2026-10-11; construction acceptance pending)

After natural objective-11 visibility in the fresh physical-output Clang run,
normal Forger selection, the original L ESP hotkey and a clear-space click
(300,150) reach the actual free-space fabrication path. First runtime failure:
no `PLATGUN!!S_ESPCoil` provider. **Local hypothesis:** the original GunPlat
factory is the missing owner; the smallest countercheck is the unchanged
ordinary F8 objective-11 checkpoint and the same normal input. Original GunPlat
is now in the existing platform closure, with explicit native object-pointer
conversions, existing globals/render adapters and unavailable editor insertion
rejected. Persistent/runtime gameplay members and Windows behavior are retained.
Existing app_schema correlates actual GUNPLAT_SAVELOAD target/attack/launcher
IDs and flags, verifies 432-byte extent/40-byte subrecord at offset 392 against
the generated source schema. Optional Clang focused native_shell/app_schema/
layout/game_material_manager checks pass.

The repeat loads the ordinary F7 checkpoint successfully and reaches GunPlat,
but reports missing `FL!!S_ESPLancer`; this is not functional ESP acceptance.
Original FancyLaunch is integrated next in the existing launcher closure.
The next same-input repeat aborts on missing `ARC!!S_Arc-!ESPCoil`. Its matched
native core confirms the ordinary ObjectComm::handleFabricate ->
UnbornMeshList::IsFullGrid -> GunplatFactory -> FancyLaunchFactory chain, with
`pBoltType` assertion. Original arc.cpp now compiles with existing MyVertex/
PrimitiveBuilder2 and native render/global adapters. Its three mixed-type
ceil/min expressions retain the original calculation with explicit double
comparison and final integer assignment; random calls and update ordering are
unchanged. These are source/compile and focused synthetic checks until actual
construction, attack, save/reload and cleanup execute. The optional CLI repeat
uses the previously documented visible mission command, external save-root
containing the unchanged `fresh-jumpplat11-ui-clang.mission` as `f0000000.mission`,
normal P/Escape/F8, F/center, L and the clear-space click. Full GCC, sanitizers,
Windows Debug/Final/module ABI and retail comparison remain open for these owners.

The Arc-enabled ordinary-input repeat now actually builds/completes ESP: source
11 is completed and 16 listed, with the visible ESP model and Citadel teletype
instruction. Normal F7 saves actual GUNPLAT_SAVELOAD (432 bytes) and nested
FANCY_LAUNCH_SAVELOAD (40 bytes) to external `esp11-complete-ui-clang.mission`.
This proves source construction on external data; it does not yet prove firing,
combat, GCC, sanitizer construction or continuation after this reload to victory.
GCC compilation exposed the original explicit TRANSFORM constructor invocation
in FancyLaunch::getSighting; the native branch now uses the same constructed
transform assignment as original Turret's existing native adapter. Windows
invocation is unchanged. All five trees subsequently rebuild and pass non-GPU
CTests at the esp-final checkpoint; source combat and Windows ABI stay open.
Normal Insert plus SDL wheel zoom also visibly executes original fullscreen map
navigation; zoom uses the existing SysMap wheel event, not direct camera state.
Solar 3 has no contiguous valid two-slot Citadel placement after these builds;
normal right-click on the zoomed map scouts Solar 4. Its actual original hint
identifies a Gas Giant, so it cannot accept Citadel. Source trigger save positions
are near Solar Prime and Solar 3; the next normal map order returns to Solar Prime
for free two-slot placement. No slot, planet, objective or script condition is
changed to simplify construction.

### Greater Pavilion placement/reload checkpoint (2026-10-11; visible Clang completion)

The Citadel-complete ordinary F7 state reloads in the visible optional Clang
mission, with original 16 complete and 18 listed. Normal F selects Forger;
Insert/fullscreen-map wheel zoom at (415,230), left Solar Prime (358,228),
Insert returns. The original industry tab (148,487), G Greater Pavilion hotkey
and slot hover/click (345,245) show a green original preview and create actual
PLATBUILD!!S_GreaterPavilion/BUILDPLAT_SAVELOAD (520 bytes). This is not an
additional owner port: the existing original BuildPlat handles this building.
Normal F7 records Forger mode 8, working/building/actual buildee, and a visible
building model; an external whole-file checkpoint preserves this in-progress
state (greater18-in-progress-ui-clang.mission). Normal F8 reloads this in-progress state and construction continues to actual
source-ready: 18 complete, 17 listed. The visible completed building and normal
F7 are preserved externally as greater18-complete-ui-clang.mission.

Earlier slot clicks did not create this building. Read-only inspection of the
normal F7 shows four contiguous unused Solar Prime slots (5–8 of the original
12), while prior clicks fell inside the planet or selected occupied neighbors.
A temporary extension of existing input observations reads Camera::ScreenToPoint
and PointToScreen before event delivery; (345,245) projects inside the source
2100–3800-world-unit slot annulus. No camera/hit-test/gameplay change is needed.
This temporary code is removed; the normal shell rebuild and existing shell
CTest pass. The runtime still reports missing BBUILDER!!T_CraneBot while
loading the source building drone dependency; this is not silently replaced
or accepted as complete production/retail parity. Full GCC, full training,
reload-to-victory, integrated later-state sanitizers and Windows remain open.

### Greater Pavilion production-menu dependency (2026-10-11; visible HUD integrated)

After actual objective 18, normal left selection of Greater Pavilion produces
its source selection voice/text and highlight, but leaves an empty HUD with
the stale Forger title. Source review assigns this industrial-platform menu
to original CMenu_BuildInd, absent from the native owner graph. **Local
hypothesis:** integrating that original toolbar subscriber restores its
S_GreaterPavilion resource menu, original build buttons/queue and ordinary
USRBUILD production actions. Smallest falsifier: reload the unchanged normal
F7 completed-18 state, select the same building normally, inspect the visible
menu and invoke its Annex production button. No direct script/state/test
command substitutes for this check. Use existing toolbar, BuildButton,
QueueControl, native fonts/resources and source BuildPlat owners; no parallel
UI is introduced. The real repeat restores a visible Greater Pavilion production HUD, original
ship buttons and queue through this owner. However, its buttons produce ships,
so the Annex-specific portion of this hypothesis is falsified: Annex belongs
to the original Forger advanced tab, whose bottom-left (30,550) normal hover
shows Munitions Annex (M), one slot and Greater Pavilion dependency. Source
GT_TOOLBAR/Toolbar!!Default is 100244 bytes, matching the native header;
read-only source-typed inspection confirms this existing advanced-tab record.
No resource ID, tech availability or toolbar layout change is needed. This
BuildInd owner is required by the original Pavilion/Greater Pavilion ship
production used later in the training; it introduces no substitute UI.

Focused optional Clang native_shell/app_schema/layout/game_material_manager
pass, then the buildind-current full affected rebuild/non-GPU CTest repeat
passes all five existing trees (49 checks each). This is source, compilation,
automated synthetic/process evidence plus actual visible selected-building HUD;
Annex construction is verified separately below; later ship production, owner
lifetime and integrated sanitizer execution of this menu remain open. Full GCC training, Windows Debug/Final/module ABI and retail parity
remain open.

### Original Annex construction (2026-10-11; visible Clang pass)

Normal Forger advanced tab (190,487), bottom-left Annex button (30,550)
shows the original Munitions Annex/M hint, one slot and Greater Pavilion
prerequisite. Normal Insert/fullscreen Solar Prime left click/Insert, the
same Annex button and slot (430,235) create actual
PLATBUILD!!S_MunitionsAnnex/BUILDPLAT_SAVELOAD (520 bytes). The visible
building finishes through the existing BuildPlat/Forger production path:
source 17 complete, 16 removed, 22 listed, original SMU3716 unit speech.
Normal F7 preserves annex17-complete-ui-clang.mission externally. The next
normal advanced-tab (75,550) hover shows Anvil/Z and one slot. Solar Prime
is now occupied; original map navigation/another planet is the next input.
No objective/tech/slot/state injection or data-format change is introduced.
Full GCC, later integrated sanitizer lifetime, reload-to-victory and full
training/end remain unexecuted. Cranes/retail visual parity remain open as
noted above.

### Anvil / Turbine Dock continuation (2026-10-11; visible Clang checkpoint)

The same optional Clang physical-output mission advances through original
Anvil construction on Solar 3: 22 completes and 23 is listed. Ordinary F7
preserves anvil22-complete-ui-clang.mission outside the repository. The Forger
advanced-tab Turbine Dock button (120,550) shows its original D/resource hint,
one-slot requirement and Greater Pavilion dependency. Source construction
then completes 23, lists 24 and plays STMG33; the native subtitle is visibly
rendered. Ordinary F7 preserves dock23-complete-ui-clang.mission. Read-only
inspection of that original save confirms Solar 3 Anvil/Dock slot ownership,
with slots 5, 7 and 8 still free. These are observed ordinary-input milestones,
not a fresh reproducible full training acceptance run. Additional live mouse
input was observed during this continuation; exact placement coordinates for
Dock require an isolated repeat rather than inference from a drawing counter.
Xeno, six Polaris, later construction/combat/end, GCC full repeat and
reload-to-victory remain open. No source/script/tech/serialized format change
was made for these two milestones.

### Helion attack command diagnosis (2026-10-11; not acceptance)

**Local hypothesis / smallest countercheck.** The ESP gate is waiting for a
real Mantis cross-system order, rather than another construction unlock. Trace
the original `OpAgent::sendHostMoveCommandFromSystem` path at the initial
three-frigate spawn while replaying 8–10 from the unchanged normal UI Bunker
save. A read-only parent debugger reports the computed path length and source/
destination; it neither issues commands nor changes script or mission state.
A valid path disproves the shortest-path rejection hypothesis and directs the
next check to original PREJUMP/JUMP ownership. An invalid path directs it to
original outgate visibility/System children. All three initial commands report source 1, destination 2 and invalid path
`0xFFFFFFFF`. A later read-only F8 observation finds reciprocal gate IDs 37/36,
correct system-link pointers and Kasse/Solar visibility flags 2/0. The unchanged
normal Bunker save has gate flags 0/0; the later normal Helion save has 2/0.
These facts distinguish command-time rejection from a permanently absent link;
the exact command-time visibility is still to be measured. One temporary
debugger script read beyond the 15-entry gate array and terminated diagnosis;
its corrected bounded repeat produces both systems without that error. Neither
is a gameplay acceptance run.

**Next owner hypothesis / smallest check.** Native OpAgent still subscribes
directly to EventSystem, whereas its Windows production path subscribes to
FullScreen with `EVENT_PRIORITY_MISSION`. Native Mission/ObjectList already
use that original owner. Restore OpAgent subscription/unsubscription to the
existing FullScreen owner when present, retaining direct EventSystem only for
the standalone closure. No persistent type, packet bytes or module interface
changes. Existing shell checks/builds first validate the closure; repeat the
unchanged ordinary-input 8–10 spawn to falsify a causal scheduling explanation.
The focused shell/input/window checks and all five non-GPU graphs pass.
The unchanged ordinary-input Sentinel/Pavilion/Helion 8–10 repeat still reports
three invalid paths, now with command-time gate flags 0/0 on a reciprocal
connection. This falsifies the causal scheduling explanation; the callback
owner integration does not accept ESP or repair the initial attack. No
artificial gate visibility, command retry or script transition is added.
Command-time visibility/delivery and the source/retail behavior still need
further diagnosis.

Source review establishes that HelionFinished creates three M_Frigates at
named MantisSpawn and orders each to named Move before its Attack state waits
for player-1 visibility. Independently decoded external original waypoint
facts and the normal saved waypoint records agree: MantisSpawn is in Kasse
(system 1), Move in Solar (system 2). This is an early required cross-system
move, before objective 14; changing either waypoint or the script is not a fix.
Read-only Helion-save diagnosis observes an attacker at its spawn, move active
but move/jump agents zero, no recall/completion, unchanged across later updates.
The whole ordinary UI save already contains these values before F8. The
existing app_schema correlation check now additionally preserves a complete
inherited spaceship prefix with nonzero move/jump IDs and active/completion/
final/sync flags. It passes; this falsifies loss of those fields through current
correlation as the explanation for the observed zero agents. It does not
establish working live jump commands or full late-state preservation.

The repeated normal-input checkpoint initially chose Acropolis in the saved
industry tab, yielding the original out-of-crew response. After explicitly
selecting the military tab, the actual Sentinel hint reports 150 ore/100 gas,
and the regular slot click completes 8 and lists 9. This input error is not a
missing production provider or permission to change resource generation.

### Physical-output Pavilion / Helion continuation (2026-10-11)

Native runtime on lawful external retail data, Clang Debug GPU/FreeType/shared
LGPL FFmpeg: after the early F7/F8 reload (1–4), normal construction 5–7 and
later Bunker-state F7/F8 reload, the same ordinary-input session completes
8–10. The physical HDMI display is available again; external captures are
visually inspected and remain outside the repository. Forger industry tab
(148,487), Pavilion (120,515), then a green three-slot Solar 3 location
(547,210) completes original 9 and lists 10. Forger tab (174,487), Helion
(30,515), then green one-slot (480,302) completes original 10. Visible source
Construction Complete text and STMG20 accompany its completed objective.
Coordinates refer to the 800x600 SDL client; the physical scaled capture is
1200x900. These are regular pointer events, not script/objective injection.

Reproduction continues the pause checkpoint's unchanged map-enabled command
and external save root. Original F7 saves again after 9, then after 10;
original P leaves the session paused. The source Helion script subsequently
requests `TUR!!M_Frigate R` while creating its real attack wave. This session
still lacks launcher providers and is therefore not accepted combat or full
M2 evidence. Required excluded production owners must be connected and a
fresh complete GCC/Clang run repeated before acceptance; no ignored enemy
archetypes may substitute for the original victory condition.

**Next local hypothesis.** Connect the original `Turret.cpp` factory to the
existing Gunboat owner: it accepts `OC_LAUNCHER/LC_TURRET`, including the
initial `TUR!!S_Scout` and this actual Mantis attack caller. The smallest
runtime falsification is an unchanged normal mission load still reporting
that launcher or failing its real weapon dependency. Compilation is only a
precondition. The production RangeFinder also needs the existing original
ISQRT calculation with a four-byte IEEE bit view on LP64; `unsigned long`
is eight bytes there. The existing math-core check exercises the original
lookup/refinement algorithm at multiple positive values. No replacement
sqrt, RNG calls, refire timing or simulation branch is introduced. The first updated normal load aborts at missing `GBEAM!!S_Gattling`;
a fresh core with the executed ELF confirms `TurretFactory::CreateArchetype`
for `TUR!!S_Scout`, called by original Taos mission load. After connecting
original GattlingBeam, normal P/Escape/F8 reaches the Helion save but aborts
at `PLASMAB!!Mantis!Frigate`; its fresh core confirms M_Frigate R launcher
creation during original `Mission::QuickLoadFile`. Original PlasmaBolt now
connects and that same F8 reload restores completed 9/10 without either
weapon-provider failure. Missing original construction-effect/drone providers
remain explicit; attack visibility, live weapons/damage and later training
are unproven. GCC first rejects a legacy direct constructor invocation;
its native transform assignment preserves the two original arguments and
Windows retains the old branch.

Current deterministic validation: all four default/optional GCC/Clang Debug
graphs build and pass their non-GPU CTests after these three owner additions.
Existing math_core/native_shell/legacy_input/native_window_manager and
render_pipeline/render_submission checks also pass under Clang ASan/UBSan
with `detect_leaks=0`, `halt_on_error=1`; system SDL3/FreeType are not
instrumented and FFmpeg is disabled. Compile-time assertions retain BT_TURRET
300 bytes (warmUpBlast 264), base turret save 40 bytes/final vector 28,
turret save 52, Gattling data 96/contactBlast 64/base save 60, Plasma data
380/bolts 188 and base/complete bolt save 56/96. The LP64 bit view retains
original seed/refinement arithmetic. Native Windows Debug/Final, cross-module
ABI and numerical retail comparison remain unperformed. An integrated ASan
normal CLI load followed by P/Escape/F8 restores this actual UI Helion save
without a reported memory/UB failure so far; live construction/combat, media,
The integrated normal P/Escape/F8, group-1 Forger selection, military-tab
Sentinel hover, another P/Escape and compositor window-close now exit 0 with
no ASan/UBSan report. The visible original resource hint is inspected. This
instruments launcher/weapon archetype creation, map/HUD/control reload and
teardown; it does not execute live shooting, late construction, media decode
or the full mission. Existing TextureLibrary dangling-reference diagnostics
remain on shutdown and the earlier PolyMesh leak issue is not declared fixed.
The early paused UI save is not a replacement for a final complete fresh
mission with all required owners.

Read-only debugger diagnosis of a subsequent normal Taos map order confirms
original `SysMap::Notify` receives SDL right down/up, focus is set, desired
and actual cursor/status resource flags are both 3, but mode is GOTO rather
than MOVE. A parent-launched debugger is used because attaching to the live
process is denied by host ptrace policy; no system setting is changed and no
game state is written. This diagnostic run is not gameplay acceptance. The
next smallest check reads the cursor-to-world/capability condition before
changing any map or command code.

### Original pause and quick-save input checkpoint (2026-10-11; in progress)

Local hypothesis: original `menu.cpp::MenuResource` and `Menu_Pause` can own
normal single-player Pause and QuickSave/QuickLoad through existing FullScreen,
Frame/Static, EventSystem and Mission interfaces without a Windows menu bar.
Smallest checks: link those actual owners, then normal P/F7/F8 events and visible
source pause/resource output. They now join the existing toolbar target; native
MENU uses original global cleanup, FullScreen priority and Status subscription.
Native external data-directory owners retain their existing ownership rather
than being registered twice by Windows menu startup. Windows menu/editor methods
and not-yet-integrated save/options dialogs explicitly abort when requested on
native; none reports successful replacement functionality. Source local/remote
pause flags, network pause events and Mission quick-save/load calls are retained.
This checkpoint does not implement regular briefing or game-options/end menus.

The first actual P shows original **Game Paused**. Escape initially fails to
resume: Frame consumes Escape/Tab from WM_CHAR, while SDL TextInput omits those
control characters. Local hypothesis: the existing SDL legacy adapter must
supply those original character events after normal control-key down. Existing
`portability.legacy_input` now checks ordered key/Unicode/character output for
Escape, Tab, Backspace and Return, no character on release, Shift-Tab/repeat and
no unrequested Ctrl-shortcut character. Printable SDL text remains on its
existing TextInput path. No new input or UI owner is introduced.

Actual external-data optional Clang on the restored physical HP HDMI output
(3840x2160, compositor scale 1.5, logical SDL client 800x600) now visibly executes
P, then Escape with removal of the pause overlay and continued source speech.
Before that restart, normal 1–4 completed and F7 produced original
`f0000000.mission`, with QuickSave source status visible. On the new normal CLI
mission entry, P/Escape again executes, and F8 loads that real UI save: original
objectives 1–4 are completed again, 5 remains incomplete, and source field,
planet resources and units are visible. Normal group/build controls after this reload complete original 5 (Oxidator), 6 (Galiot), and 7 (Bunker on Solar 3) with visible source panels/crew production. A second F7/F8 save/reload after Bunker completion restores that later mission and continues through Sentinel 8. Captures show the finished original Sentinel, Construction Complete and STMG15 Pavilion instruction, with original 9 listed. Completion of all remaining training and exhaustive saved-state comparisons remain required. The first save was made while paused, after scout
and before Oxidator construction, rather than by the shell smoke checkpoint.
Exact shortcuts are read from repository Hotkeys.utf: P pause (110), F7 quick
save (113), F8 quick load (112), F9 options (81; still unavailable).

Commands executed: `cmake --build /tmp/frontierx-voice-clang --target
frontierx_native_shell frontierx_legacy_input_probe -j4`; CTest filters
`^portability.(native_shell|legacy_input|native_window_manager)$` pass. Four
complete default/optional GCC/Clang graphs also built and passed non-GPU CTests
after adding menu/pause owners, before the control-character correction. Later
full repeats and integrated ASan/UBSan UI construction are required. The Clang sanitizer shell, legacy-input, pipeline and submission targets compile with the menu owners/control-character correction; the five focused native_shell/legacy_input/native_window_manager/render_pipeline/render_submission CTests pass with leaks disabled. Actual integrated construction/UI sanitizer reruns remain required.

Source review of the newly reachable QuickLoad owner also exposes an unclosed
FindFirstFile enumeration handle. The ownership hypothesis is that closing that
handle before Load preserves game state and prevents per-reload handle retention;
`QuickLoadFile` now retains/closes the original successful handle and reports a
missing SAVEDIR through its existing failure branch. Runtime repetition and
native Windows Debug/Final/ABI remain required. No save record changes.
Full M2 is not accepted: later construction/combat, regular entry/end menus,
later UI save/reload, both full compiler runs and lifetime/media cleanup remain
open. UI success status alone does not verify all saved state or victory.

### Original Sentinel production-owner checkpoint (2026-10-11; in progress)

Normal Forger military-tab selection displays the original Sentinel Tower hint
(150 ore, 100 gas, two slots). The tab is the fourth category, rather than the
empty research categories; the B hotkey cycles industry selection and is not
needed to open Forger construction. After objective 7, moving the normal build
preview onto Solar 3 reaches the source `Planetoid::Render` /
`UnbornMeshList::RenderMeshAt` / `createMesh` chain. The actual Clang run exits
139: original `ObjectList::AddRef` receives a null archetype after the missing
`PLATBUILD!!S_SentinelTower` provider. A fresh external core, symbolized using
the executed ELF saved before the crash, confirms that stack and the loaded
record's **PC_BUILDPLAT** class. No kernel OOM appears in the crash interval.
This is a real normal-input failure, not an accepted training step.

Local hypothesis: linking the original `BuildPlatFactory` and its existing
`Platform`/`TFabricator` production owner removes that specific missing-provider
failure and permits the same normal preview/construction. Smallest checks:
compile/link the actual owner, then repeat normal objective-8 placement and
observe the source not-ready/ready trigger. `BuildPlat.cpp` joins the existing
build-support owner with a native prefix matching the adjacent RefinePlat owner,
exact-case includes and an explicit unavailable-editor abort. Source factory
registration, build/research/upgrade/queue operations, resource ownership and
save/load methods are unchanged; no serialized type or public vtable changes.
Its existing DPlatform assertions protect the retail base-data contract; new source-owner assertions also check packed command sizes 1/5/2/6 and handle offsets 1/2, BT_PLAT_BUILD_DATA size 828/buildRate 820/maxQueueSize 824, and BASE_BUILDPLAT_SAVELOAD size 20/rallyPoint 4/buidTimeSpent 8/constructionID 12. These compile in GCC/Clang; native Windows execution remains open.

Executed commands: `cmake --build /tmp/frontierx-voice-clang --target
frontierx_native_shell -j4`, followed by the existing render_submission,
render_pipeline and native_shell CTests. They pass. All four existing default
and optional GCC/Clang complete graphs compile and their non-GPU CTests pass
(the central count is recorded in BUILD_GRAPH.md). The Clang ASan/UBSan shell,
render-pipeline and submission targets compile; their same focused CTests pass
with `ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1`, FFmpeg off,
system SDL3/FreeType uninstrumented. Actual physical-output Clang Forger military tab, original Sentinel hint and green two-slot placement on Solar 3 now construct the source Sentinel: Construction Complete/SSE3680 and STMG15 are visible; original 8 completes and 9 is listed. This is after a later normal F7/F8 reload with completed Oxidator/Galiot/Bunker. GCC and integrated sanitizer repetitions remain required; these checks do not establish combat,
full training, Windows Debug/Final or cross-module ABI/retail parity.

### Original system-map integration checkpoint (2026-10-11; in progress)

Local hypothesis: the missing original map owner is the immediate normal-input
navigation dependency for Earth/Swamp Bunker placement. The smallest falsifier
is a source `SysMap` normal hover/camera click and subsequent planet selection;
map draw counts alone do not establish navigation or construction.

`SysMap.cpp` now joins the existing toolbar owner. Its source resource priority,
mouse hit tests, camera transforms, orders, terrain/visibility rendering and
mission animation save fields are retained. Native startup selects this owner
when `--skip-sector-map` is omitted; the previous bounded no-GDI owner remains
only for the explicit exclusion. The first enabled-map launch actually exposed
an incorrect overwrite by that excluded owner (`RegisterIcon` abort); conditional
startup fixes the owner collision. The next launch reaches original
`SysMap::drawSystem` but aborts at its 18-vertex triangle fan. A symbolized core
confirms the original map/RenderBatcher caller. The native primitive now expands
fans to ordered U16 triangle indices without changing vertices/winding; the
existing submission CTest checks the actual registered owner and invalid fans.
The vertex-buffer fan restriction remains explicit and unchanged.

The original `Sector` toolbar/resource subscription, texture rendering, drawing
and normal map-command paths now use the same native texture-DC boundary. The
adapter draws source brushes/pens/circles/rings/hatching/rectangles/arcs/links and
copies source icon textures into owned RGBA shadows, committed through the
existing pipeline upload. No parallel UI or map state is introduced. Existing
render-pipeline CTest checks exclusive DC ownership, incorrect/stale release,
failed-upload retry, actual color/icon copy, clipped ellipse/hollow ring/link,
scaled rectangle copy and rejection of out-of-bounds/locked copies. Original
editor viewers remain Windows-only; no successful viewer replacement is added.
`SYSMAP_DATA` (1 byte), `SECTOR_DATA` (16 bytes, last field offset 12) and original
`GRIDVECTOR` map-animation bytes (2) have native compile-time checks. Source
save field widths/order and public virtual interfaces are unchanged. Native
Windows Debug/Final/dumpbin ABI, raster-image parity and map-animation reload
execution remain open.

Executed Linux x86_64 Debug commands:

```sh
cmake --build /tmp/frontierx-voice-clang --target frontierx_native_shell frontierx_render_pipeline_probe frontierx_render_submission_probe -j4
ctest --test-dir /tmp/frontierx-voice-clang -R '^portability.(render_submission|render_pipeline|native_shell)$' --output-on-failure
cmake --build /tmp/frontierx-voice-gcc --target frontierx_native_shell frontierx_render_pipeline_probe frontierx_render_submission_probe -j4
ctest --test-dir /tmp/frontierx-voice-gcc -R '^portability.(render_submission|render_pipeline|native_shell)$' --output-on-failure
```

These focused checks pass. The latest Clang sanitizer shell/pipeline/submission targets and focused CTests
also pass with ASan/UBSan, disabled leaks, system SDL3/FreeType uninstrumented and
FFmpeg off. Integrated construction/map mission sanitizers remain required.
Four complete default/optional GCC/Clang graph repeats pass; central totals
belong in BUILD_GRAPH.md.

Actual external-data Clang launch repeats the documented no-smoke FFmpeg/FreeType
`run-build-clang.sh` invocation with only `--skip-sector-map` omitted, logs/captures
outside the repository in `/tmp/frontierx-m2-runtime/map-clang*`. Source map art and
the known planet marker are visibly present in the SDL surface. Ordinary input
repeats objectives 1–6, including selectable Oxidator and completed Galiot, with
this map enabled. Galiot archetype load no longer reports the missing FBALL/PART
providers; actual explosion spawning/rendering remains unproved. Normal minimap
hover shows the original camera/move status, left-click changes the field view,
and right-click orders the selected Forger to that map position, with the source
movement voice and visible arrival. A second normal map click/order reaches
Solar 2, whose ordinary selection shows the original Ore Planet resource panel.
Normal group-1 Forger reselection (without B's industry-cycle hotkey) and normal
map orders reach Solar 3. Its ordinary resource panel reports Earth Planet and
crew stores. The original Bunker button/hint and green slot are visible; a normal
slot click starts construction, source not-ready/ready triggers complete 7 and
list 8, with actual completed Bunker, STMG13 subtitle and crew production visible
(45/min in the planet panel). No script/objective was set, no mission state was
edited and no save was made. The earlier normal trial on Solar 2 does not complete
7; normal Stop/map navigation leaves its incomplete platform in the mission.
Sentinel/tab selection, map reload/close, GCC visible repetition and integrated
construction sanitizers remain required. The host still has only compositor
FALLBACK; this is native external-data captured-surface evidence, not normal
physical-output/full-training acceptance.

An independent regular ASan/UBSan construction attempt (the explicitly map-skipped,
speech/voice-skipped FFmpeg-off configuration) reaches objectives 1–4 and actual
Oxidator build but stops with UBSan at `TPlatform::render`'s negative float-to-U8
health-color delta. Hypothesis: casting a negative color delta directly to U8 is
undefined; signed truncation followed by the same integer color offset preserves
the intended byte result. Native-only corrections apply to that original branch
and its identical TObjSelect/HarvestShip color calculations. Simulation/hull state
is untouched and Windows source arithmetic remains unchanged. The smallest
required runtime falsifier is another instrumented normal build 5, then Galiot
6; those corrected construction runs remain pending. The failed run exits 1
and is not a successful sanitizer result.

## Original gameplay Hintbox checkpoint, M1 (2026-10-11)

**Local hypothesis and smallest falsification check (source review).** The
latest preceding records are the same-day UnitComm alert and status/scrolling
checkpoints, not the older startup TODOs: they report zero gameplay hint
draws. `FieldManager` already owns a real source hint caller, independent of
`MultiHotButton`, toolbar buttons and hot rectangles. The hypothesis before
porting the camera access path was: normal mouse hover over a visible,
fog-eligible Cygnus field in Sol_Train can run the existing field owner into
the source Hintbox; only the original camera access and native owner cleanup
are needed for this selected flow. The smallest counterexample is a normal
hover on such a field producing no resource text after the source 600 ms
lead. The initial camera projection put the nearby Cygnus center outside the
viewport (approximately -134,618 at 640x480). Source inspection found that
`MScroll` was not in the native graph and the selected `ISystemMap::Scroll*`
branches aborted. This justified bringing in that bounded original access
path, rather than inventing a toolbar hint caller. The actual falsification
check used SDL mouse input, edge pan, stationary field hover and a visible
capture; font lifecycle tests alone could not answer it.

**Concrete production owner chain.** SDL motion goes through
`ShellWindowCallback` and the existing EventSystem/legacy message adapter to
`ObjectList::Notify`. Original `ObjectList::render` performs visibility and
second-pass highlight tests using its mouse selection rectangle.
`Nebula::TestHighlight` / `AsteroidField::TestHighlight` retain screen-to-world,
field hit-test and HardFog checks; they call `FieldManager::FieldHighlighted`
with the archetype's original `infoHelpID`. The next source update requests
`RF_HINTBOX` through `ResourceClient<>` / `TResClient.h`, at original
`RES_PRIORITY_MEDIUM`; `FieldManager::setHintbox` changes text only when the
resource ID differs. The font-enabled text-resource binding owns original
`HintResource`, which uses the existing native font/render adapter and
original history, 600 ms lead, 200 ms trail and mouse-reset behavior. Strings
are loaded from the prescribed external resources: Cygnus
`IDS_HINT_FIELD_CYGNUS` (3122), medium asteroid
`IDS_HINT_FIELD_ASTEROID_MED` (3117). Only the original field owner sets
gameplay hint text; the diagnostic observer injects none.

**Small integration and release changes.** Original `MScroll.cpp` is added
to the existing camera owner target, subscribed to the native EventSystem and
activated at source mission entry. Its original edge/hotkey decisions and
cursor resource priority (`RES_PRIORITY_HIGH - 0x1000`) are retained. Native
scrolling waits for a validated SDL mouse position, preventing automatic
pan from an uninitialized cached (0,0), and deactivates/releases on mission
close. Only SysMap.cpp's in-game camera scroll calculation (pane width,
configured scroll rate, frame time and fractional remainders, camera bounds)
is adapted into the existing native SystemMap contract. Fullscreen map
scrolling and other unsupported operations still fail. The existing
SystemMap/data-root check now links the real native UserDefaults dependency.
Directional cursor artwork remains outside this bounded camera access path;
no successful cursor placeholder was added. No toolbar/hot-rectangle UI was
needed for this caller.

Native FieldManager unsubscribes from the ObjectList connection it actually
subscribed to. Mission close clears its pending/desired hint and releases
original resources. Source Hintbox relinquishes ownership and clears text,
timers and history IDs on that event. Existing normal loss/target-switch
semantics remain source behavior. The shell asserts resource/text/timer idle
state immediately after actual source reload and close. Optional
`--mission-hint-check` requires a font-enabled mission and a drawn source
Cygnus resource before reload, then runs a six-second second source modal
interval requiring new Cygnus draws through normal hover. It neither injects
mouse input nor calls `SetText`; ordinary runs retain their existing loop.
The existing modal diagnostic's quit-request state is reset for that second
interval.

**Focused synthetic checks.** The existing `portability.native_shell`
font-smoke check now exercises the isolated original FieldManager/Hintbox
resource subscription: acquire Cygnus at medium priority, switch to Ion,
lose ownership on no highlight, close while owned, re-enter and release
again. Its existing Hintbox timing/font checks retain exact lead/trail and
mouse reset, duplicate-font history and complete font release. A bounded
MScroll check rejects missing mouse payload, prevents cold-position scroll
and verifies mission-close activation/resource reset. These are synthetic
checks, explicitly distinct from the real gameplay runs below.

**Visible native runtime on external retail data.** Separate final GCC and
Clang Linux x86_64 Debug runs use Wayland, SDL3 and Vulkan with the lawful
Steam Sol_Train data, external shared-LGPL FFmpeg decoder and external
`FONTS/bauhmib.ttf`; all retail data, logs, input helpers and captures remain
outside the repository in `/tmp/frontierx-hint-runtime`. A persistent Linux
uinput mouse with actual relative-motion/button capabilities delivers normal
SDL input. A compositor pointer warp alone, or a REL-only device without
mouse button capability, did not reliably deliver SDL motion; those failed
attempts are not acceptance evidence. Each final run uses one focused
800x600 window, normal left selection and right movement, then a brief
bottom-left edge pan and stationary hover. Visually inspected captures show
"CYGNUS NEBULA" with its movement/gas description, then "MEDIUM ASTEROID
FIELD" with its resource/slowing description, then Cygnus again. Moving onto
empty space removes the text. The first interval ends with Cygnus hovered;
source save/reload asserts clear ownership and timers, a second normal hover
visibly draws Cygnus again, and leaving again removes it. The second interval
also ends with Cygnus hovered; actual source mission close asserts complete
hint release. Counters are supplementary, not the visible acceptance.

Both runs returned source modal result 4 and exit 0. GCC executed 1090
first-interval frames / 1799 simulation updates, with 770 hint glyph draws,
249 scrolling-text draws and 612 subtitle draws; Clang executed 1083 / 1799,
with 764 / 250 / 612 respectively. Both retained normal player selection,
completed select/move objectives, preserved those objectives across source
reload, and saved/reloaded owner 1 with exact per-run moved state:
GCC `(50190.3,66563.2,-36.822)`, Clang `(50190.3,66563.2,-1000)`.
Each started two unit streams, completed one, stopped one and retained zero
active streams. The renewed source-modal hover and post-close idle assertions
passed in both. These wall-clock input runs are not an identical-input
simulation replay: the differing Z values are recorded, not called height
parity or cross-compiler determinism. Decoder coefficient warnings and
texture-library dangling-reference diagnostics still occurred; this record
does not claim complete asset/texture cleanup. The focused hint ownership
assertions and original font lifecycle checks passed. Build/check evidence,
including explicit GPU CTest exclusions and sanitizer scope, is in
[BUILD_GRAPH.md](BUILD_GRAPH.md#original-gameplay-hintbox-graph-m1-2026-10-11).

**Reproduction.** Build the existing optional FFmpeg/FreeType configuration
as documented in [native audio](NATIVE_AUDIO_STREAMER.md), then run from the
repository root with lawful external data and a writable external save root:

```bash
RETAIL=/path/to/external/Conquest-install
BUILD=/tmp/frontierx-voice-gcc # Repeat separately with the Clang tree.
SAVE=/tmp/frontierx-hint-save-gcc
mkdir -p "$SAVE"
"$BUILD/frontierx_native_shell" \
  --data-root "$RETAIL/DATA" --material-root "$PWD/Data/mat" \
  --system-profile "$PWD/Code/App/Src/Conquest.ini" \
  --ui-font "$RETAIL/FONTS/bauhmib.ttf" \
  --skip-movies --skip-notification-cue --skip-sector-map \
  --skip-space-background --skip-unit-labels --skip-planet-ui \
  --skip-animated-portraits --skip-unit-alerts --skip-mission-sfx \
  --base-textures-only --skip-legacy-lighting --skip-mission-music \
  --mission-source Sol_Train.dmission --mission-engine --mission-start \
  --mission-hint-check --smoke-ms 60000 \
  --mission-gpu "$BUILD/generated/shaders/sdl3_gpu_probe.vert.spv" \
    "$BUILD/generated/shaders/sdl3_gpu_probe.frag.spv" \
    "$BUILD/generated/shaders/sdl3_gpu_probe_mt.vert.spv" \
    "$BUILD/generated/shaders/sdl3_gpu_probe_t4.vert.spv" \
  --string-pack "$RETAIL/stringpack.db" --gen-data "$RETAIL/gendata.db" \
  --game-types "$RETAIL/gametypes.db" \
  --hotkeys "$PWD/Code/App/Src/Hotkeys.utf" --save-root "$SAVE"
```

Keep every named media/UI exclusion. Run one compiler at a time. In the
800x600 client area: left click (303,425), right click (624,480), allow the
selection/movement voices to complete; hover at the bottom-left edge (0,599)
for roughly 220 ms, then stop at (400,300). Hover (150,450) for at least
900 ms, switch to (50,265), wait, then return to (150,450); move to (500,200)
for 500 ms to inspect removal. Return to Cygnus until source reload reports
`reloaded hint observation started`; within the six-second interval inspect
empty space, Cygnus for one second, and empty space again, then hold Cygnus
until automatic close. Coordinates depend on viewport, frame time and edge
pan duration; select the visible field if they differ. A normal physical
mouse is sufficient; input automation stays external. Expected exit is zero
only after moved-state/objective checks, renewed source hover and clear
close ownership. Neither a skip nor a font-only run satisfies M1.

**Layouts, module ABI and remaining external gate.** Source review finds no
changed serialized records, save structures, resource IDs, shared interface
vtable or Windows-facing module declaration. Native-only runtime fields in
MScroll/SystemMap and diagnostics have no persistent representation. No
simulation update, RNG ordering or file format changed. Existing Linux
layout/schema/module-process checks pass in the full graphs; that is not a
native Windows cross-module ABI result. This Linux host has no native Windows
executor, VS2022 `cl.exe` or `dumpbin`; Windows Debug/Final and the required
native `Tools/abicheck` execution were not performed. On the supported native
Windows environment run `Build-Conquest.ps1 -Configuration Debug -NoDeploy`
and `Build-Conquest.ps1 -Configuration Final -NoDeploy`, then execute `Tools/abicheck/allmods.ps1 -DumpbinPath <VS-dumpbin>
-BuiltGameDirectory <Debug-or-Final-output> -ScriptDirectory
<external-retail-campaign-DLLs>` and `Tools/abicheck/symdiff.ps1
-DumpbinPath <VS-dumpbin> -RetailMissionDll <external-retail-Mission.dll>
-BuiltMissionDll <Debug-or-Final-Mission.dll> -ScriptDirectory
<external-retail-campaign-DLLs>` for both configurations; see the
[ABI check](../Tools/abicheck/allmods.ps1). This is the
concrete external gate, still open. Full mission sanitizers, comparative
Windows font/render/audio evidence and actual retail gameplay parity are
also unperformed. M1's selected Linux gameplay acceptance is demonstrated;
M1 is not reported as an unconditional cross-platform completion.

**Current next step.** Execute that Windows build/ABI gate when the supported
environment is available. Continue M2 with the original controls needed for
the next Sol_Train training objective (multiple selection/control-group and
command/HUD ownership), preserving this real field-hover regression path.
Do not expand this checkpoint into a replacement UI or multiplayer work.
Older dated records below are historical.

## Original UnitComm alert rendering checkpoint (2026-10-11)

**Blocker and hypothesis (source review).** Original `UnitComm::drawAlerts`
was a native abort and its factory required `--skip-unit-alerts`, although
original `MScript::AlertMessage` already calls `COMM_MISSION_ALLERT`.
The missing owner boundary was GDI font construction. The falsifiable
milestone was original alert history reaching native glyph submission,
retaining source colors, ten-font limit and expiry while the selected
mission still passes normal input and moved-state source save/reload.
The early training checkpoint does not naturally trigger an alert; an
explicit source-caller smoke is required to distinguish integration from
unexecuted code. No public interface, persistent structure or simulation
update/RNG ordering changed.

**Implementation.** UnitComm now uses the existing optional native font
adapter for its original context font, resource strings, color table and
alert list traversal. Native-only checks reject out-of-range active alert
IDs and terminate copied resource strings. Fonts initialize lazily when
an alert first draws; a later mode-enable event reuses them, and mode-disable
and destruction release them. The queue, ten-second goto expiry and
source ten-/thirty-second history classes remain unchanged. Windows retains the
original GDI path. Alert rendering requires an external `--ui-font`, or
explicit `--skip-unit-alerts`; the latter is no longer mandatory for voices.

An alert also requests a system/sector-map ping. The selected native mission
already explicitly excludes that presentation with `--skip-sector-map`
(`bNoGDI`). Native UnitComm now honors that existing exclusion for map-only
pings. No successful SystemMap implementation is substituted: its unsupported
ZoneOn remains an explicit failure when reached. This gate does not port
map pings, toolbar UI or alarm navigation.

**Focused automated execution.** The existing `portability.native_shell`
font-smoke CTest now executes original alert resource/color initialization,
repeated mode enable without extra references, active-history selection,
exact ten-second goto expiry, thirty-second ordinary-alert node removal and complete
release of ten fonts. Inputs are source resources, a synthetic history node
and the existing external font. The terminal test and previously disabled
alert path could not cover these contracts. Passing enables the source
factory without the mandatory skip; failure rejects the integration.
No separate probe or alternate alert scheduler was added.

Linux x86_64 Debug Ninja GCC 16.2.1 and Clang 22.1.8 built
`frontierx_native_shell` in `/tmp/frontierx-voice-{gcc,clang}` and passed
`ctest --test-dir <tree> -R '^portability.native_shell$' --output-on-failure`.
Clang Debug ASan/UBSan rebuilt `frontierx_native_shell` and
`frontierx_event_system_module` in `/tmp/frontierx-voice-asan`; the same
focused check passed with `ASAN_OPTIONS=detect_leaks=0`, without a reported
finding. Leak checking, system FreeType instrumentation and full mission
sanitizers did not execute. Complete default/optional graph results are
recorded in [BUILD_GRAPH](BUILD_GRAPH.md), not as component evidence here.

**Controlled native runtime on external retail input.** The existing
mission shell has an explicit `--mission-alert-smoke` option. It requires
unit voices, alert rendering, source Teletype and an external font. After
the first mission update it resolves the player Forger through the original
MScript part cache and calls original `MScript::AlertMessage` once. That
caller creates its red Teletype, plays the mission alert cue and queues
UnitComm's cyan mission alert. The queued filename has static lifetime,
matching UnitComm's borrowed-filename contract. The smoke supplies the
lawfully external Forger selection clip `SFO3053.wav` from the unit-speech
archive; the mission cue remains source `MSAlert.wav` in the separate
mission-speech archive. This is a controlled invocation, not an event
naturally generated by the selected training script or a combat result.
The smoke fails acceptance if no original alert glyph submission occurs.

The exact command is the preceding
[status/scrolling runtime command](#original-status-and-unit-scrolling-text-checkpoint-2026-10-11)
with `--skip-unit-alerts` replaced by `--mission-alert-smoke`; all other
options and the 45000 ms duration remain identical. GCC/Clang used the
optional Debug builds above, reviewed external shared-LGPL FFmpeg, lawful
external Steam data/font and isolated save directories. Normal left-click
selection and right-click movement followed the alert. These visible
Wayland/Vulkan runs briefly overlapped, so focus affected frame/update counts;
this is not a cross-compiler timing/determinism comparison.

GCC executed 968 source modal frames / 1059 simulation updates and recorded
273 alert glyph-draw calls; Clang executed 1280 / 1348 and recorded 292.
Inspected captures show the cyan UnitComm warning and red source Teletype
alongside training subtitles under both compilers; a later GCC capture
shows both alarm texts gone. Both completed selection/movement objectives,
saved/reloaded owner 1 and exact moved position `(50177.7, 66564.3, -1000)`,
returned modal result 4, closed the mission and exited zero. Each started
three unit streams, completed two, stopped one and retained zero active
streams, including the controlled alert clip. These results do not establish
terrain/height or full gameplay parity.

**Limits and next gate.** A normal mouse hover attempt through field
locations produced zero Hintbox draws; real gameplay hints remain
unverified, with synthetic timing/font lifecycle evidence only. Natural
combat alerts, alarm navigation, simultaneous alert capacity on GPU,
map pings, other named media/UI exclusions, Windows Debug/Final and ABI,
Windows font/audio comparison, full mission sanitizers and retail parity
remain open. No retail asset or capture was copied into the repository.
The next coherent gate is a real gameplay Hintbox caller and its resource
ownership, or the original toolbar/hot-rectangle closure needed to expose it.

## Original status and unit scrolling text checkpoint (2026-10-11)

**Blocker and hypothesis (source review).** Mission Subtitle and Teletype
were already executable, but `ObjComm` status resources still reached a
terminal substitute and `UnitComm::update` could not submit unit subtitles
without original `ScrollingText`. The local milestone was the original
status and unit-text owners running alongside training speech/subtitles,
normal selection/movement and moved-state source save/reload. The missing
contracts were their GDI font creation, FULLSCREEN callback subscription
and Windows wide-format substitution. These are runtime owners; no public
virtual interface, disk/save/packet layout or simulation order changed.

**Implementation.** Original `StatusBar.cpp`, `Hintbox.cpp` and
`ScrollingText.cpp` now compile in the existing mission-media object target.
The mission shell selects them when `--ui-font` supplies an external face.
Their existing EventSystem handlers and App draw boundary remain the owners;
Windows keeps its GDI, FULLSCREEN and menu branches. Native fonts initialize
lazily for the source game-active mode. Status keeps source resource IDs,
modes, name/text placement, gold color and shadows. Hint keeps the original
600 ms lead, 200 ms trail and mouse-reset rules. ScrollingText keeps its
four-entry capacity, source placement and strict greater-than-6000 ms expiry,
including the original erase-and-index-advance behavior: the next entry is
not aged on that same update. Notification audio has the existing explicit
`--skip-notification-cue` boundary. Native teardown unsubscribes callbacks
and releases fonts; mission close clears scrolling entries.

`UnitComm::update` now sends original selection/movement text to SCROLLTEXT.
Native formatting bounds the source name and substitutes the single source
`%s` name slot literally, avoiding Windows/POSIX wide-varargs differences.
Unknown formats and overflow fail explicitly. The original name trimming,
queue order, voice selection and RNG consumption remain unchanged.
The earlier mandatory combination of unit voices with `--skip-subtitles`
is removed. The source subtitle preference is still enabled temporarily and
restored by the shell. No dependency, standalone probe or replacement text
scheduler was introduced. Default builds remain FreeType-free.

**Automated execution.** Linux x86_64 Debug Ninja GCC 16.2.1 and Clang
22.1.8 rebuilt `frontierx_native_shell` in optional FFmpeg/FreeType trees
`/tmp/frontierx-voice-{gcc,clang}`. The existing process check used
`ctest --test-dir <tree> -R '^portability.native_shell$' --output-on-failure`.
Its extended `--font-smoke` path checks original status modes/reset, Hintbox
lead/trail/mouse reset, scrolling capacity, exact expiry/erase behavior,
bounded string termination, mission close and complete release of four
affected status fonts, two hint fonts and eight scrolling/player fonts.
It also checks the source unit format and leaves an undersized output
unchanged. Generated resources and the explicitly external font are the
inputs. The terminal-only test could not exercise these original owners;
extending this existing test protects their new mission integration.

Clang Debug ASan/UBSan rebuilt `frontierx_native_shell` and
`frontierx_event_system_module` in `/tmp/frontierx-voice-asan`; the same
focused CTest passed with `ASAN_OPTIONS=detect_leaks=0`, without a reported
sanitizer finding. The system FreeType binary was not instrumented, and no
full mission sanitizer or LeakSanitizer run occurred. Complete default and
optional CPU/process graph results and deliberately excluded GPU CTests
are recorded only in [BUILD_GRAPH](BUILD_GRAPH.md).

**External-data native runtime.** Separate 45-second visible Wayland/Vulkan
runs used lawful external Steam `Sol_Train` data, its external `bauhmib.ttf`
face, repository material scripts and the reviewed external shared-LGPL
FFmpeg decoder. No retail asset, capture or raw trace entered the repository.
The command below records the executed options with external installation,
font, build and isolated output paths parameterized:

```sh
XDG_CONFIG_HOME="$RUN_CONFIG" timeout -k 5s 120s "$BUILD/frontierx_native_shell" \
  --data-root "$RETAIL/DATA" --material-root "$REPO/Data/mat" \
  --system-profile "$REPO/Code/App/Src/Conquest.ini" --ui-font "$FONT" \
  --skip-movies --skip-notification-cue --skip-sector-map --skip-space-background \
  --skip-unit-labels --skip-planet-ui --skip-animated-portraits --skip-unit-alerts \
  --skip-mission-sfx --base-textures-only --skip-legacy-lighting --skip-mission-music \
  --mission-source Sol_Train.dmission --mission-engine --mission-start \
  --mission-gpu "$BUILD/generated/shaders/sdl3_gpu_probe.vert.spv" \
    "$BUILD/generated/shaders/sdl3_gpu_probe.frag.spv" \
    "$BUILD/generated/shaders/sdl3_gpu_probe_mt.vert.spv" \
    "$BUILD/generated/shaders/sdl3_gpu_probe_t4.vert.spv" \
  --string-pack "$RETAIL/stringpack.db" --gen-data "$RETAIL/gendata.db" \
  --game-types "$RETAIL/gametypes.db" --hotkeys "$REPO/Code/App/Src/Hotkeys.utf" \
  --save-root "$RUN_SAVE" --smoke-ms 45000
```

Normal left-click selection and right-click movement used existing desktop
input helpers. GCC executed 1013 source modal frames and 1350 simulation
updates; the final Clang run executed 1000 frames and 1349 updates. Both
completed select/move objectives, preserved the moved Forger owner 1 and
exact saved position through source reload, returned modal result 4, closed
the mission and exited zero. GCC recorded 304 original scrolling font draws
and 635 mission subtitle draws; final Clang recorded 291 and 632 respectively,
plus 1748 status draws and zero hint draws. These counters increment only
when the font adapter actually submits glyphs. Each run started two unit
voices, completed one, stopped one and retained zero active streams.

The inspected GCC capture shows gold source status at the bottom and both
selection/movement unit messages above the multiline training subtitle in
the live scene. The inspected Clang capture shows status and scene geometry;
its capture timing did not retain unit/speech text, so simultaneous visual
capture evidence is GCC-only. Clang's executed owner draw counters and
mission acceptance are separate evidence. Hintbox has synthetic lifecycle
execution only: no gameplay hint caller or visible hint was observed.

**Limits and next gate.** Unit alert text still requires
`--skip-unit-alerts`. Native player-attributed chat and networking remain
explicit unsupported paths; menu hot rectangles and framebuffer locking
are unported. Other named media/UI exclusions above remain in force.
Family/weight, Unicode, font/audio Windows comparison, Windows Debug/Final
and module ABI, full mission sanitizers and full retail parity are unverified.
Existing legacy texture/factory and texture-reference teardown diagnostics
remain unresolved. The next coherent text gate is original UnitComm alerts
and real gameplay Hintbox callers, with their existing status/resource owners.

## Original mission speech subtitles checkpoint (2026-10-11)

**Blocker and hypothesis.** The next text gate after Teletype included
`Subtitle.cpp`, whose real `MScript::PlayAnimatedMessage` / `PlayAudio`
callers already supply counted script strings and native sound handles.
The native shell left `SUBTITLE` unset and required `--skip-subtitles`.
The falsifiable local milestone was source-owned multiline subtitles during
selected training speech, normal selection/movement and moved-state source
save/reload. The GDI multiline font and FULLSCREEN callback subscription
were the missing platform contracts. These are runtime owners; no saved,
disk, packet or public virtual-interface layout was widened or changed.

**Change.** Original `Subtitle.cpp` now builds in the existing mission-media
object target and subscribes to native EventSystem. It retains the original
counted text, ordinary/briefing panes, pause/default visibility and
`ISoundManager::IsPlaying` expiry. Native teardown unadvises its token;
mission close clears pending text. GDI/FULLSCREEN remains the Windows path.
The optional FreeType `IFontDrawAgent` now supports multiline layout using
the original `MultiLineFont.cpp` space/newline and 20-character lookback
rule, bounded strings, inclusive pixel panes and clipped glyph spans through
the existing App render boundary. Duplicates retain multiline mode. The
single-line Teletype adapter still uses its original layout owner.
No new dependency, shader, overlay scheduler or standalone probe was added.

The mission shell binds that source owner when `--skip-subtitles` is absent
and requires an explicitly supplied `--ui-font` face. It temporarily enables
the original subtitle preference and restores it on cleanup. The font option
also works with subtitles when Teletype visuals are explicitly disabled.
Default builds remain FreeType-free and reject an unavailable font backend.
Training speech uses the already reviewed external shared-LGPL FFmpeg path.
`--skip-animated-portraits` still excludes the portrait. UnitComm text uses
`SCROLLTEXT`, not `ISubtitle`; its existing unit-voice requirement for
`--skip-subtitles` remains until that owner is ported. Thus these subtitle
runs explicitly use `--skip-unit-voice`; the earlier unit-voice checkpoint
remains available separately.

**Automated execution.** Linux x86_64 Debug Ninja GCC 16.2.1 and Clang
22.1.8 built `frontierx_native_shell` and both complete optional FFmpeg/font
graphs in `/tmp/frontierx-voice-{gcc,clang}`. The exact focused command was
`ctest --test-dir <tree> -R '^portability.native_shell$' --output-on-failure`.
The existing `--font-smoke` case checks a non-terminated counted `AB`,
ordinary/briefing state, inclusive narrow-pane wrapping, CRLF, multiline
font duplication, source clearing when the real sound adapter reports an
unknown handle finished, mission close and complete font release. No speech
file is read by the unknown-handle check: generated mission archive owners
are temporarily supplied and restored. The test also caught and fixed an
extra reference from assigning a newly created font through COMPTR's
AddRef operator; the final check requires font ownership to return exactly
to its starting count. Inputs are synthetic plus the explicit external font.

The complete default graphs in `/tmp/frontierx-mission-{gcc,clang}` also
built and ran CPU/process checks without optional FreeType or FFmpeg.
Exact full graph commands and counts are recorded in
[BUILD_GRAPH](BUILD_GRAPH.md). The two desktop GPU checks were deliberately
excluded; no skipped check is counted as GPU runtime evidence.
Clang Debug ASan/UBSan rebuilt `frontierx_native_shell` and
`frontierx_event_system_module` in `/tmp/frontierx-voice-asan`; the same
focused shell CTest passed with `ASAN_OPTIONS=detect_leaks=0`. The system
FreeType binary was not instrumented. LeakSanitizer and a full mission
sanitizer run remain unperformed.

**Visible external-data runtime.** Separate Wayland/Vulkan GCC and Clang
processes ran external `Sol_Train` with training speech and source subtitles,
repository profile/hotkeys/materials, generated shaders, isolated save/settings
roots and the existing explicit UI/SFX/movie exclusions. Starting from the
previous voice command, these runs removed `--skip-training-speech`,
`--skip-subtitles` and `--skip-teletype-visuals`, added
`--skip-unit-voice --ui-font <external-install>/FONTS/bauhmib.ttf`, and used
`--smoke-ms 20000`. Normal left selection and right movement completed
objectives 1 and 2 and preserved owner 1 and the moved Forger through original
mission save/reload. GCC observed 517 modal frames / subtitle draws and 599
source updates; Clang observed 479 frames / subtitle draws and 600 updates.
Both preserved position `(50174.9,66559.3,-200)`, returned source modal result
4 and exited zero. Wall-clock runs are not cross-build determinism evidence.
A temporary Clang desktop capture was inspected: white script subtitle text
wraps into five visible rows above the live mission objects. Retail assets,
logs and captures remain outside source control.

A final 45-second GCC run (`--smoke-ms 45000`) observed 1057 modal
frames, 1349 source updates, 666 subtitle draws and 1139 combined font
draws. Early and late temporary captures were inspected: five white
subtitle rows during speech disappeared, followed by yellow source
Teletype objective text. The final source reload retained owner 1 at
`(50176,66563.8,-36.7917)` and exited zero. The earlier standalone GCC
capture was taken after process close and supplied no visual evidence;
the delayed captures above replace it.

Runtime command (paths to lawful external data are parameters; substitute
`gcc` / `clang` and their generated shaders as actually executed):

```text
XDG_CONFIG_HOME=/tmp/frontierx-alpha-runtime/config timeout -k 5s 120s <tree>/frontierx_native_shell \
  --data-root "$LAWFUL_INSTALL_ROOT/DATA" --material-root "$REPO_ROOT/Data/mat" \
  --system-profile "$REPO_ROOT/Code/App/Src/Conquest.ini" \
  --ui-font "$LAWFUL_INSTALL_ROOT/FONTS/bauhmib.ttf" --skip-unit-voice \
  --skip-movies --skip-notification-cue --skip-sector-map --skip-space-background \
  --skip-unit-labels --skip-planet-ui --skip-animated-portraits --skip-unit-alerts \
  --skip-mission-sfx --base-textures-only --skip-legacy-lighting --skip-mission-music \
  --mission-source Sol_Train.dmission --mission-engine --mission-start \
  --mission-gpu <tree>/generated/shaders/sdl3_gpu_probe.vert.spv \
    <tree>/generated/shaders/sdl3_gpu_probe.frag.spv \
    <tree>/generated/shaders/sdl3_gpu_probe_mt.vert.spv \
    <tree>/generated/shaders/sdl3_gpu_probe_t4.vert.spv \
  --string-pack "$LAWFUL_INSTALL_ROOT/stringpack.db" \
  --gen-data "$LAWFUL_INSTALL_ROOT/gendata.db" --game-types "$LAWFUL_INSTALL_ROOT/gametypes.db" \
  --hotkeys "$REPO_ROOT/Code/App/Src/Hotkeys.utf" \
  --save-root /tmp/frontierx-subtitle-runtime/save-<compiler> --smoke-ms 20000
```

The explicit external Bauhaus face is still one face for all requested
resource families. The source subtitle descriptor requests Arial; this is
an explicit font substitution, not Arial metrics or Windows visual parity.
ASCII, monochrome glyph spans and this selected mission remain the bounds.
Briefing state/layout is exercised synthetically; no native briefing ran.
The combined font-draw diagnostic now names both text owners; with speech
active it must not be reported as independent Teletype execution evidence.

**Next gate.** Integrate original status, hint and scrolling-text owners,
including UnitComm subtitles/alerts, then validate simultaneous unit voice
and mission speech/text. Portraits, mission music/spatial SFX, other named
UI/render exclusions, Windows Debug/Final/ABI/font/audio comparison,
full-game behavior and retail parity remain open.

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

### Polaris beam dependency (2026-10-11; integration in progress)

Normal Pavilion selection restores original CMenu_BuildInd and its ship
buttons/hints. A normal Polaris button action aborts in original
TurretFactory::CreateArchetype at result->pBoltType: no provider for
BEAM!!S_Beam-!Polaris. The symbolized actual SIGABRT core follows
BuildPlat::updateBuild -> GunboatFactory -> TurretFactory -> LoadArchetype;
this is an executed production dependency, not an inferred combat requirement.
Local hypothesis: connecting original beam.cpp in the existing weapon-owner
closure allows that unchanged normal production action. Smallest falsifier:
reload the ordinary F7 state with Xeno construction on Solar 4 and issue the
same Pavilion Polaris command. Original BeamManager factory registration,
texture/sound/weapon lifecycle and beam behavior must remain intact. No
persistent record fields or projectile simulation/RNG order are changed.
Compile/schema checks and actual normal production/sanitizer/end continuation
remain to be executed; native Windows Debug/Final/module ABI stays open.

The beam-only repeat passes the former beam-provider load and then aborts at
FancyLaunchFactory::CreateArchetype, no MASS_DISRUPTOR provider. Thus the
complete-production hypothesis is falsified for the beam-only closure. The
original Polaris archetype preloads its Mass Disruptor launcher even without
research/use. Next local hypothesis: connecting original MassDisruptor.cpp
in the same closure permits that unchanged ship production. The same normal
Pavilion button after ordinary F8 is the smallest falsifier. Existing schema
correlation must preserve its 76-byte save / 36-byte base / transform offset.
The five beam-only graphs build and pass their non-GPU checks; actual ship
production still fails and is not recorded as accepted.

### Native hardpoint instance/archetype boundary (2026-10-11; countercheck)

Actual Polaris construction reports hp_lgun/hp_rgun missing, as initial
Mantis did for hp_mfgun1/2. Source review identifies native helpers passing
instance IDs to the original IHardpoint archetype-ID API. Active Windows
System.cpp uses HARCH, which gets/holds the instance archetype and releases
it. Local hypothesis: restoring that same conversion in native direct
lookup and enumeration removes these false misses without changing root/path
lookup or combat simulation. Smallest deterministic falsifier extends the
existing game_material_manager Engine fixture: a second real mesh instance
with a distinct ID, a production hardpoint on its archetype, direct lookup
and wildcard enumeration must return the instance and correct point. Old
code must fail this check; the production mission reload must then remove
the same warnings before any combat acceptance. No new parallel probe/UI.

The isolated hardpoint Engine countercheck fails against the old native
helper at lookup/enumeration after its fixture IDs are separated. Restoring
HARCH conversion in both native lookup and enumeration makes the existing
optional Clang game_material_manager/app_schema/layout/native_shell checks
pass. Active Windows FindHardpoint uses direct lookup (the recursive variant
is inactive); this correction preserves that direct lookup and child paths.
It does not switch gameplay to recursive matching. Actual mission warnings
and integrated sanitizers still require repetition.

### Six-Polaris production / Eutromil dependency (2026-10-11; continuation)

After Beam/Mass Disruptor integration, ordinary F8 of the unchanged Xeno
in-progress save, Insert/wheel zoom/Solar 3/Insert and Pavilion left selection
(505,290) restore the original menu. Normal Polaris hover/button (75,525)
shows its original M hint/resource cost and starts production; five more
ordinary clicks queue the remaining ships. Actual ship models and source
Construction Complete are visible. Xeno finishes on Solar 4 through the
user's original placement, completes 24 and starts STMG21/source objective12.
Normal F7 during pending ship production and F8 restore the models/queue;
production continues to six supplied Polaris, 12 completed, 10 removed and
13 listed. Ordinary F7 preserves polaris12-complete-ui-clang.mission outside
the repository. This passes this later reload continuation to objective12,
not reload-to-victory. Beam/Mass source and persistent correlation builds
pass all five affected non-GPU configurations; actual weapon firing, owner
lifetime sanitizers, full GCC and end remain open.

The next original Forger industry Eutromil hint (165,515) requests two slots.
Ordinary map navigation to Solar 4 and placement (465,345) attempts original
PLATREPAIR!!S_Eutromill and then crashes, with no provider for that archetype.
The actual core is SIGSEGV; its extraction was deleted after examination.
The executable was rebuilt before symbolization, so that core backtrace is
not reliably symbolized and is not quoted as such. The production log and
source factory declaration identify the missing RepairPlat owner. Local
hypothesis: connecting original RepairPlat.cpp permits the same normal
Eutromil construction after ordinary F8 of the completed12 save. Smallest
falsifier is that same industry button and real planet-slot placement.
No successful placeholder or changed training/script/save state is allowed.

Original RepairPlat now compiles in the existing platform target. Native
headers/explicit target pointer match existing boundaries; native editor
insertion fails explicitly (Windows editor hooks remain guarded). Existing
app_schema verifies original REPAIRPLAT_SAVELOAD size476/base36 and nested
base/transform offsets392/428 with real source-symbol correlation. Focused
optional Clang checks pass, then repair-hardpoint-current full affected
rebuild/non-GPU checks pass all five existing configurations. A normal
completed12 F8/repeated P/Escape/close exits0; the same saved Polaris reload
has none of the prior false hp_lgun/hp_rgun/hp_mfgun warnings. This supports
the adapter correction, not actual firing/combat parity. The next visible
optional Clang run omits --skip-unit-alerts to execute the natural original
Eutromil-ready AlertMessage path; unit alert acceptance is still pending.

### Multiple selection HUD dependency (2026-10-11; integration in progress)

Ordinary lasso selects the constructed Polaris and map right-click moves
them to Solar Prime, but multiple selection leaves the old Forger title
and an empty HUD. Source CMenu_MulMix is the original toolbar subscriber
for a non-admiral multiple selection; existing ShipSilButton controls are
already in the native graph. Local hypothesis: linking that owner restores
original group silhouettes and stance/command buttons for the same normal
lasso. Smallest falsifier: ordinary F8 of the current F7 state, navigate
Solar Prime and select multiple ships normally; inspect group HUD and use
an original command. Use existing controls, resources, priorities and
packet processing; no alternative fleet UI or invented training condition.
Runtime capability aggregation needs native byte copies for the original
four-byte packed M_CAPS, preserving each OR/AND and selection order. Windows
operators and persistent caps layout remain unchanged.

### Eutromil ready, natural alert and group HUD build (2026-10-11)

The `repair-alert-clang` physical-output run omits `--skip-unit-alerts`.
Normal Eutromil construction completes source13 and lists14. Inspection of
the actual frame shows the finished platform, original Platform Built and
Mission Alert text, red mission teletype and STMG24 jumpgate subtitle.
Ordinary F7 saves this state; normal pause/resume and window close exit0.
Actual Mantis GetEffectiveDamage events against the Citadel and beam-light
output occur after the HARCH correction; this is bounded firing evidence,
not full combat or weapon parity. Missing S_RepairBot visual builder and
TextureLibrary dangling references remain observable unresolved boundaries.

Original CMenu_MulMix now builds in the existing toolbar target. Native
update pointers retain full width; packed four-byte capability aggregation
uses byte copies with the original order. Its duplicate native getCaps
helper is translation-unit-local; Windows code remains unchanged. Focused
optional Clang native_shell/app_schema/layout/game_material_manager pass.
`mulmix-current` rebuilds all five existing configurations and each passes
its 49 non-GPU CTests (GPU checks remain separately host-dependent). Group
HUD execution, command use and integrated owner lifetime are still pending.

The external ordinary-input helper had a coordinate bug: its click action
ignored optional X/Y and clicked at the existing pointer. Corrected outside
the repository, it now emits a regular pointer move before such a click.
Previous placement coordinates are not reproducible evidence unless a
separate move and actual owner/result confirm them. The user's Xeno build
and saved ready/objective conditions are real, but an isolated fresh
normal-input repeat of recent construction remains required. Failed
Jumpgate clicks created no platform and count as no acceptance. Next repeat
uses checked pointer targets and actual original wormhole highlighting.

### Discovered sector link / line adapter (2026-10-11; hypothesis)

Corrected ordinary pointer clicks navigate the Forger near the Solar wormhole
(map zoom5,344/370), revealing its actual field model. Normal J and field
click375/286 reach the original Fabricator but construction is rejected;
read-only F7 reports gate36 owner57026/player2. Original training speech
expects a free buildable wormhole, so this ownership must be compared with
the unchanged initial mission instead of bypassing CanIBuild.

Normal Insert now reaches Sector::DrawLinks and aborts at native DA::LineDraw
for diagonal smooth line345/367 to315/604. Matching current executable/core
confirms Sector→SysMap→original event/render loop; memory has ample capacity.
The extracted temporary core is deleted and diagnostics remain external.
Local hypothesis: source DrawAgent's unlocked 3D PB_LINES branch restores
this real link using the already-supported native LineList path. Source
ignores bSmooth in that 3D branch; preserve that behavior and pane offsets,
half-pixel endpoints and batch-state reset. The existing App render fixture
now requests this diagonal smooth mode and fails against the old adapter;
its endpoint/topology/color countercheck and visible same-map repetition
are the smallest falsifiers. No format/interface/simulation change.

The corrected line adapter passes the existing focused optional Clang checks
and `map-line-current` rebuild/non-GPU CTest sequence in all five existing
configurations. The separate `link-clang` run normally F8-loads the current
F7 state, opens the discovered-gate fullscreen map and returns without the
previous line-mode abort. Normal Solar Prime lasso530/205→675/405 shows Group
with three original Polaris silhouettes and command/stance buttons. Hover
316/554 shows original Attack Stance, and its normal click emits hotkey134.
Normal fullscreen-map right-click344/370 issues group movement with Polaris
voice and returns to the wormhole field. Full battle and later reload-to-end
remain unaccepted. The untouched original mission archive contains one
Mantis JumpPlat quick-load record; source QuickResolveAssociations parks
it at a gate and original ParkYourself creates its paired sibling. The
observed owner is real source behavior, not justification for deleting a
unit or weakening the build condition. Next: inspect actual target/combat
and the initial ownership compatibility boundary through original owners.

### Wormhole depth-sort sanitizer boundary (2026-10-11; hypothesis)

`polaris-current-asan` starts the original mission then normal F8 loads an
unchanged whole F7 checkpoint containing Beam/Mass/Repair/Polaris and current
commands/objectives. Instrumented actual field render stops in
Jumpgate::StashVertices at negative float-to-U32 depth conversion (-14407.5),
through ObjectList::Render and CQDoModal; exit1, not a passed run. Local
hypothesis: explicit finite/range-checked signed truncation followed by U32
conversion preserves the existing 32-bit wrapped sort key and vertex/order
contract. Windows expression, array type, game state and RNG are unchanged.
Smallest falsifier is the exact ordinary F8/visible wormhole render under
ASan/UBSan; synthetic tests alone cannot accept this correction.

The corrected `gate-sort-asan` repeat reaches source12/13 completed and14
open after normal F8, visibly renders the wormhole and original Group HUD,
uses normal map/selection, F7, P/Escape and a second F8, then normal window
close exits0 without ASan/UBSan diagnostics. This loads/releases Beam/Mass
Disruptor/RepairPlat/industrial and multiple-selection owners in the actual
mission. It is bounded reload/owner evidence, not new production firing,
full training or leak-free cleanup. `detect_leaks=0`; training speech/unit
voice/unit alerts are explicitly skipped for this instrumented build.
System SDL/FreeType and GPU driver are uninstrumented; TextureLibrary still
reports dangling references on close. All five `gate-sort-current` affected
rebuild/non-GPU CTest sequences pass. Native Windows execution remains open.

### Legacy Turret sighting basis (2026-10-11; diagnosis/hypothesis)

An external debugger run ordinarily loads the unchanged F7 state. Pure
state inspection/single-stepping observes valid original turret target57026,
visibility3, outer range2.5 and available supplies. At actual checkLOS, the
re-aimed hardpoint frame reports angle -3.1415925 against original limit2,
so no beam/damage is emitted. No debugger calls create orders, change
mission state or set goals; this is diagnosis, never gameplay acceptance.

Source getSighting constructs the old -Z-forward frame with rotate_about_i
(90), then joint yaw/pitch; SuperTrans explicitly provides original
get_yaw_zbased/get_pitch_zbased for that frame. The ordinary -Y getters
produce a half-turn after this old frame is aimed. Local hypothesis: the
native legacy-engine Turret adapter must use those existing source -Z
getters for sighting/swivel/LOS, keeping navigation transforms, original
limits/ranges/damage, timing and all random/update calls unchanged. Windows
source expressions are preserved; exact supported-Windows/retail aiming
comparison remains unexecuted and no parity is inferred. Smallest falsifiers:
existing App/material fixture verifies legacy sight-frame heading over all
quadrants using its already-linked original SuperTrans owner;
repeat the same original normal group command without a debugger and observe
actual target damage/weapon output, then repeat integrated sanitizers.

### First Polaris firing / supply label and Beam strip boundary (2026-10-11)

The separate visible `turret-sight-clang` ordinary startup/F8 repeat computes
original Polaris-to-Mantis-Jumpgate damage: base10, armor0.5, effective5.
It then SIGSEGVs in Gunboat::DrawFleetMoniker at the absent optional debug
hotkey service. The instrumented `turret-current-asan` repeat independently
stops at the same null interface (UBSan exit1). No complete combat or
objective14 completion is inferred. The Turret four-quadrant fixture and
all five affected Linux non-GPU build/check configurations pass; supported
Windows/retail aiming remains unexecuted.

Local hypothesis: Gunboat's debug-font percentage block bypasses the existing
explicit `--skip-unit-labels` boundary. Native rendering now applies that
boundary to this block, retaining the base fleet moniker. No fake debug
hotkey/font service is installed; this is an exclusion correction, not
implemented unit labels. `supply-boundary-asan` repeats ordinary startup,
P/Escape/F8 on the unchanged whole F7 checkpoint. It passes the null access
and calculates damage, then explicitly aborts because original Beam blur
submits RPVERTEX TriangleStrip (six vertices) unsupported by SDL Submit.

Focused ASan layout/app-schema/native-shell checks pass with leak checking
on; game-material reports the known PolyMesh 1409 bytes/29 allocations,
so the combined run fails. Repeating those four with
`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1` passes. This does
not prove leak-free teardown.

Next local hypothesis: only the SDL RPVERTEX submission adapter needs a
wind-preserving strip-to-list expansion for original Beam blur. It now
expands indexed/direct strips with alternating triangle winding, checked
size and allocation failure. Existing host-dependent SDL GPU check compares
both strips with explicitly ordered triangle-list readback; this remains
separate from deterministic CTest and visible combat acceptance. No fields,
vtables, serialized layout, weapon damage/range/timing or RNG calls change.
The smallest gameplay falsifier is the same ordinary F8 combat under
ASan/UBSan with actual visible beam output; repeat is in progress.

The `beam-strip-asan` ordinary F8 repeat visibly shows the original beam
between Polaris and the hostile gate. Real F7 state records paired gate hull
595, later305 (initial800), and three attacking ships consuming supplies
to0. Normal fullscreen-map group movement to Solar4 (green planet at
503/210 at capped map zoom5; 453/208 is empty space) reaches the actual
Eutromil area. A later ordinary F7 records replenished supplies100/130/100.
F7/P and ordinary close exit0 without ASan/UBSan diagnostics. This executes
real weapon rendering/damage and original RepairPlat area resupply, but does
not yet destroy the gate, complete14 or reach victory. The same disabled
leak/media scope and uninstrumented system components apply. TextureLibrary
dangling references remain.

All five `beam-strip-current` and `unit-labels-current` rebuild/non-GPU
sequences pass. The new host GPU fixture initially used the smoke-scene's
`structureValid` bounds for a different strip rectangle, causing failure;
it now requires nonempty rendering and exact direct/indexed-vs-list
checksums. The repeated required visible GPU run passes that comparison,
then fails the existing resize gate because Hyprland tiles its window.
Full host GPU check remains open pending a floating-window repeat; no GPU
skip is counted as success.

Native supply percentages now borrow ObjectList::GetUnitFont, set the
original blue color and draw through the existing IFontDrawAgent adapter.
An absent optional debug hotkey service means no debug highlight override;
Windows retains its original debugfont/hotkey code. Ordinary object labels
use unchanged Font!!ObjectType loading/GenData resource ownership. Native
Shell permits this path only with --ui-font, or keeps explicit exclusion.
Local hypothesis and smallest falsifier: ordinary selected-object hover,
actual low-supply firing percentage, F7/F8 and close without
--skip-unit-labels, using the same source mission and unchanged whole save.
`unit-labels-asan` repeat is in progress; native labels are not yet accepted.

### Platform death watcher mask (2026-10-11; actual sanitizer blocker)

`unit-labels-asan` ordinary F8 visibly renders Forger hover text/life bars
through the real ObjectList font, with the unit-label skip absent. Normal
Solar4 lasso and capped-zoom map return restore combat; F7 observes hostile
paired gate hull245, later20. At lethal damage, UBSan exits1 in
Platform::SelfDestruct: complement of U8 shadowVisibilityFlags is promoted
to negative int before left shift. Local hypothesis: complementing U32
before shifting preserves all original 32-bit watcher-mask bits without
undefined behavior. Native SelfDestruct and the identical OnMasterChange
mask now use unsigned complement/shift; Windows expression is retained.
No persistent fields, death order, watchers selected, RNG or damage change.
Smallest falsifier is ordinary F8 of this unchanged genuine20-hull F7 state
and natural lethal damage under ASan/UBSan. Repeat pending; actual death,
gate release and14 are not accepted yet.

The floating visible GPU repeat also passes strip-vs-list image comparison
and the preceding existing fixture stages, but still fails existing resize
observation (640x360 unchanged). Thus host GPU overall remains failed, not
skipped or accepted; mission drawing evidence is separately scoped.

The `platform-mask-asan` ordinary F8 repeat passes natural lethal damage: a
normal F7 contains no hostile JumpPlat records and both gates have owner0.
Normal Forger selection/J and field click305/280 start original Jumpgate
construction, observed source STMG25. A normal F7 saves that actual pending
build. The run then aborts opening Sector::ComputeSupplyForPlayer comm
C9990.wav (exit134), not a sanitizer finding and not accepted completion.
Read-only archive inspection confirms externally supplied c9990.wav exists
in DATA/speech with WAVE format1026 (IAC), requiring the existing optional
decoder. The earlier instrumented configuration deliberately had FFmpegOFF.
No caller is skipped or replaced. All five `platform-mask-current` affected
build/non-GPU sequences and four focused ASan checks pass before decoder
configuration changes.

The instrumented tree now configures the same external shared LGPL decoder
as the optional GCC/Clang trees:

```sh
PKG_CONFIG_PATH="$FFMPEG_PREFIX/lib/pkgconfig" cmake -S . -B "$ASAN_TREE" \
  -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON
cmake --build "$ASAN_TREE" -j4
```

Existing Clang ASan/UBSan compiler/link flags, FreeType and GPU options are
retained. External shared FFmpeg, SDL/FreeType and GPU driver remain
uninstrumented. Training speech/unit voices/unit alerts are still excluded
from this bounded run; the direct original Sector comm is enabled. Repeat
the whole genuine in-progress F7 through ordinary F8. Completion14/15 and
full training remain open until the original source conditions execute.

Host GPU check synchronization hypothesis: installed SDL3 SDL_video.h
documents asynchronous SetWindowSize and recommends SyncWindow for an
immediate result. Existing sdl3_gpu_probe now synchronizes the real resize
request before its unchanged swapchain-size assertion. No renderer or
mission flow changes; required visible floating-window repeat remains
pending. This does not turn a failed resize into a skip.

Object-label ownership source review: ObjectList holds its existing
COMPTR<IFontDrawAgent>, GetUnitFont AddRefs each borrow, and source close
frees unitFont. NativeFont retains/releases the GenData archetype and owner.
No persisted unit/font fields, public vtable or module interface changed.
The existing native layout/app-schema checks execute; supported-Windows
Debug/Final/module ABI tools are not installed on this Linux host and
remain explicitly unexecuted.

`sector-comm-asan` ordinary F8 resumes the saved pending Anflug state, but
Forger stays in FAB_MOVING_TO_TARGET at its last cell; moving nearby Polaris
away does not by itself restart it. Normal Stop button315/487, J and the
newly observed gate365/360 place a fresh order (working7410), reaching
FAB_BUILDING with actual buildee59713. This is a normal gameplay recovery,
not accepted preservation/continuation of the earlier pending moving order.
That moving-build reload regression remains to reproduce/fix independently.

Actual original C9990.wav now starts through the existing decoder (speaker0);
external FFmpeg prints IAC coefficient warnings but playback begins. Real
S_JumpPlat ready triggers STMG26 and source objective14 completed,15 listed,
12 removed. A normal F7 records two player1 platforms, hull800, owners59713
and268495169 at the two gates. The ready checkpoint is retained unchanged
externally as jumpgate14-ready-ui-asan.mission; no retail/save data enters
the repository. Visible friendly gate model, original wormhole hover text
and normal Forger HUD are inspected. Training speech remains explicitly
skipped, so STMG26 here is source progression, not speech acceptance.
P/Escape and ready-state F8 repeat is in progress; Kasse15 remains pending.

Ready14 P/Escape/F8 repeat visibly restores the friendly model and original
wormhole label, with no sanitizer diagnostics. `sector-comm-current` all
five affected build/non-GPU CTest sequences pass49 each, now including the
external shared decoder in the ASan tree. Moving-build reload, actual
low-supply percentage visibility,15/waves/20/21/victory, fresh full
GCC/Clang runs and native Windows/retail comparison remain open.

### Kasse enemy Wing closure (2026-10-11; integration in progress)

Actual ordinary right-click366/365 on the friendly gate initiates Forger
warp agent7666 to system1 and completes it. Source15 completed and20 listed;
STMG27/29/34 are source calls with speech explicitly skipped. Original
wave construction reports missing FWING!!M_Scout, ENGTRAIL!!MPlasma,
RL!!M_StasisField, FWING!!M_Hive1 and FWING!!M_TiamatBomber. Thus these are
incomplete hostile production owners, never accepted combat or valid final
retail wave parity. Ordinary F7 retains this diagnostic incomplete state
externally; it will not be used as acceptance input after closure. Normal
close exits0 without ASan/UBSan diagnostics, with known texture references
and leak checking disabled.

Local hypothesis: the first real missing Wing caller needs original
FighterWing/Fighter/EngineTrail factories in the existing gunboat target.
Native prefixes, exact-case includes and original fighter ObjectControl
template instantiation now compile/link these owners; no new gameplay
implementation, fields, vtables, RNG/update calls or behavior is substituted.
Existing app_schema now checks original Fighter84/base44/transform44 and
Wing20/createTime8/end20, with production schema correlation of IDs, signed
supplies/timers, bitfields and transform. Four focused optional Clang checks
pass. Five affected builds and actual ordinary jump repeat are in progress.
Input is the unchanged genuine ready14 checkpoint from before waves;
ordinary startup/P/Escape/F8, then normal Forger/gate input must recreate
original waves with the factories present. Stasis and actual fighter
launch/combat/reload/lifecycle, native Windows Debug/Final/ABI and full
training remain open.

GPU resize synchronization repeat still fails unchanged640x360; SDL's real
request plus SyncWindow does not produce a resized swapchain on this host.
The hypothesis that a missing synchronization barrier alone explained the
failed host check is falsified. Strip/list exact-image comparison still
passes, but full GPU probe remains failed.

The first Wing/Fighter/Trail five-tree attempt fails GCC on old MSVC explicit
TRANSFORM constructor syntax; Clang/ASan build/check sequences pass. Native
Wing and Trail now assign the same construction result; Windows expressions
are preserved. `wing-trail-transform-current` corrected all five build/check
sequences pass49 each. No extra physics/RNG operations or persistent fields.

Actual visible `wing-trail-clang` normal startup/F8 reloads the unchanged
ready14 checkpoint, with unit labels, training speech and unit voice enabled.
A first right-click reaches the gate holding position; a second ordinary
right-click366/365 starts Forger warp agent7822. This is not evidence that
the first order always completes. The actual Wing/Fighter creation then
explicitly aborts at FighterFactory::CreateArchetype pWeaponType after
missing PROJ!!M_Bolt-!Fighter. System coredump confirms SIGABRT1329218;
exit134. Runtime executable had been replaced by parallel build completion,
so no mismatched extracted-core/source debugger analysis is claimed.

Read-only external gametypes metadata confirms this is BT_PROJECTILE_DATA
with WPN_BOLT; RL!!M_StasisField is BT_RECON_LAUNCH/LC_RECON_LAUNCH. Local
hypothesis: original ProjectileManager closes the real Fighter weapon
creation dependency. Its static CreateAutoCannon delegate is also required
to link the existing class, so both original sources now join the gunboat
target with native prefixes/exact-case includes. No weapon ranges, damage,
RNG, update order, persistent fields or interface change. Focused optional
Clang checks and same normal ready14-to-Kasse repeat are in progress.
No saved partial/incomplete-wave state is used as acceptance input.


### Fighter projectile and real Recon dependency countercheck (2026-10-11)

Original ProjectileManager/AutoCannon compile/link and the four focused
optional Clang checks pass; `fighter-projectile-current` completes all five
affected Linux build/non-GPU check sequences. Visible external-data
`fighter-projectile-clang` starts normally, reloads the unchanged pre-wave
ready14 checkpoint with ordinary P/Escape/F8, and takes normal Forger/gate
orders. The first order reaches the holding position; the second initiates
warp. Source15 completes and20 is listed; training speech and unit voices
are enabled. The live window remains responsive, and ordinary P visibly
shows Game Paused alongside the original Small Asteroid Field hint and HUD.
Normal window close exits0. This is bounded native runtime, not complete
combat: original RL!!M_StasisField, RL!!M_RepellentCloud and
RL!!M_RepulsorWave providers are still absent. Actual fighter firing,
late-wave save/reload and owner cleanup remain unproven.

Local hypothesis: adding the original ReconLaunch factory closes these
LC_RECON_LAUNCH dependencies without a substituted launcher. Native prefix,
exact-case includes and assignment of the same hardpoint TRANSFORM constructor
result preserve fields, interfaces, refire randomness and original updates.
Its existing Save/Load/ResolveAssociations retain probe and target ownership.
Four focused optional Clang checks pass before the schema extension.
The smallest runtime countercheck is ordinary startup/P/Escape/F8 from a
whole unchanged ready14 checkpoint, normal f and two right-click366/365
orders (`recon-launch-clang`). Actual Forger warp agent7632 reaches the
real factory and explicitly aborts on missing STBOLT!!M_Stasis at
ReconLaunchFactory::CreateArchetype pBoltType; exit134. No partial-wave save
is used as the next acceptance input.

The existing app_schema is extended for RECON_LAUNCH_SAVELOAD; an initial
32-byte assumption fails compilation (actual36). The unchanged original
fields place probeID24/targetID28; the corrected extent and production-schema
correlation are being checked. Next local hypothesis: original StatisBolt
closes the actual WPN_STASISBOLT child. It joins the same existing gunboat
target, with native prefix/exact-case includes and typed equivalents of
legacy mixed-type min/max expressions. No target/weapon parameters, update
order, RNG calls or save formats are changed. Build, focused schema, integrated
runtime/sanitizers, remaining Recon children and Windows Debug/Final/ABI
checks remain pending. Full M2 remains unaccepted.


Stasis/Recon optional Clang build and the four focused CTests pass,
including the existing production schema correlation: Recon36/probe24/target28,
Stasis336/base192/trans192/end336, target/probe IDs, signed refire delay,
grid-square and target arrays, ownership/cleanup bitfields and mission/transform.
This is synthetic layout evidence, not a late-combat reload. Actual
`stasis-bolt-clang` repeats ordinary P/Escape/F8 and normal gate orders from
unchanged ready14; Forger warp agent7637 loads the real Stasis archetype and
then aborts explicitly on REPEL!!M_RepellentCloud, exit134.

Local hypothesis: the original RepellentCloud factory closes that real Recon
child. It joins the existing gunboat target with native prefix/exact-case
includes and same-type float min/max comparison before the original U8 alpha
assignment; Windows expressions and gameplay updates remain unchanged.
Original blast children BLAST!!RepellentCloud/RepellentCloudDamage are retained.
The existing app_schema checks its348/base204/trans204/end348, square/target
arrays, direction vectors, owner/mission IDs, timers and cleanup flags.
Optional Clang compile/link passes; affected builds/checks and ordinary
pre-wave runtime repeat (`repellent-cloud-clang`) are underway. Stasis/Repellent
actual firing/cleanup and late save/reload, missing Repulsor child and other
original production providers remain open. No full training or Windows/retail
comparison is claimed.


`recon-stasis-repellent-current` completes all five affected Linux build and
non-GPU CTest sequences: default GPU GCC/Clang, optional font/shared-LGPL
decoder GCC/Clang and instrumented optional Clang (ASan/UBSan; leaks disabled).
Commands are `cmake --build TREE -j4`, then
`ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir TREE --output-on-failure -E '^portability\.(sdl3_gpu|game_material_gpu)$'`.
Host GPU overall resize remains failed, distinct from these checks.
Actual `repellent-cloud-clang` ordinary ready14 reload/gate jump agent7624
loads the Stasis and Repellent children, then aborts on RW!!M_RepulsorWave,
exit134. This is a real failure, not a successful wave/combat run.

Local hypothesis: original RepulsorWave closes the remaining observed Recon
child for the actual source Tiamat wave. It now joins the same existing
gunboat target with native prefix/exact-case includes; no substitute
projectile, gameplay change or persistent/interface change. Optional Clang
compile/link passes. Existing app_schema checks Repulsor268/base124/trans124/
end268 and correlates owner/mission IDs, target-square/target array, timers,
held/last-sent state and original cleanup/init flags. The four focused
Clang checks and full affected configurations are being repeated; actual
unchanged ready14 normal-input repeat is `repulsor-wave-clang`.

For all these bounded visible Clang repeats, use the existing optional
mission command above with external `--ui-font`, original Sol_Train.dmission,
original profile/Hotkeys.utf, external DATA/stringpack/gendata/gametypes and
unique external `--save-root`. Flags exclude movies, notification cue, space
background, animated portraits, mission spatial SFX, non-base textures,
legacy lighting and mission music; unit labels, training speech, unit voices
and unit alerts are enabled. Place the whole genuine externally retained
ready14 quicksave in that unique save root, then use normal P/Escape/F8.
After loading100%, Escape/f selects and centers Forger. At the inspected
800x600 field view, normal right-click366/365 reaches the gate holding cell;
a following normal right-click366/365 initiates warp. This saved-state
integration check is not regular-entry/full-training acceptance. Pending
moving-order reload, the apparent first-order gate behavior, original Mantis
Builder/CraneBot/Spider/RepairBot and Mimic/Synthesis missing providers,
actual complete waves/fighter combat, Matrix/end, full isolated GCC/Clang,
late reload/close sanitizers, low-supply labels and Windows/retail gates
remain open.


### Actual Fighter batch lifecycle failure (2026-10-11)

`recon-all-current` completes all five affected build/non-GPU sequences,
including Repulsor schema correlation. Actual `repulsor-wave-clang` normal
unchanged ready14 reload/gate entry now loads the three original Recon
children, completes Forger warp and source15, lists20 and visibly shows
STMG34 Matrix instructions/subtitles, original Forger HUD and field hint.
After normal f camera selection brings actual fighters into view, it aborts
at RenderBatcher::GetPrimBuffer `vert_format == desc->vertex_format`, exit134.
This is a failure, not completed fighter/combat acceptance.

System coredump1350670 confirms SIGABRT and the exact external-data command.
Read-only matching core analysis resolves original Gunboat::Render →
FighterWing::Render/renderFighterList → Fighter::SetupBatchRender stage0
(6 fighters) → RenderBatcher::GetPrimBuffer. The request is RPVERTEX322,
120 vertices/180 indices, exact-format managed buffer. Core state shows the
production VertexBufferManager aggregate uninitialized: render_pipeline null,
vertex_count_granularity/min/max0, no managed buffers. The available native
pipeline itself supports this format. Extracted cores are unique temporary
files, deleted after read-only analysis; logs/captures stay external.

Local hypothesis: the native composition omitted the source System.cpp
VB_MANAGER->initialize lifecycle before production batching. NativeShell now
calls the real initialize after GPU pipeline binding, before render-batcher/
global startup, rejects failure and pairs cleanup after original mission/
global/archetype close while the pipeline remains bound. No renderer
format substitution or assertion weakening. Original manager API, allocation
family and persistent/module layouts remain unchanged. The existing
vertex_buffer_manager CTest now checks the actual pre-init120-vertex exact
request is rejected, then the same initialized request allocates a real
128-vertex buffer and original unlock/release/cleanup destroys it. Existing
app_schema/layout/material/shell checks and affected builds are being
repeated; actual normal ready14 gate entry is `fighter-vb-clang`.
Full combat, lifecycle with active fighters, integrated ASan/UBSan, Matrix,
late reload-to-victory, independent full GCC/Clang and Windows/retail are open.


`fighter-vb-current` completes all five Linux build/non-GPU sequences. The
five focused optional Clang checks (layout, app_schema, vertex_buffer_manager,
game_material_manager, native_shell) pass. Actual `fighter-vb-clang` normal
ready14 entry/warp/camera f proceeds past the prior Fighter batch assertion,
remains responsive and visibly pauses. This clears the actual crash, but
an inspected Fighter battle scene remains a separate visual gate. Normal
Escape/F7 saves an actual Kasse15 state with original Recon owners; normal
map gate orders return Forger to Solar (warp agent7732). Map navigation/
Forger T selects Matrix; existing free Solar2 slot placement343/286 starts
its original GunPlat construction. Missing TL!!S_Talorian is reported;
this incomplete protection is not Matrix acceptance. Ordinary close exits0,
with existing texture-reference diagnostics still open.

Local hypothesis: source TalorianLauncher and its actual
TALORIANEFFECT!!S_SHIELD child close the Matrix protection owner. Both now
join the same gunboat target with native prefixes and exact-case includes.
Original source launcher Save returns TRUE without a record, Load FALSE;
ResolveAssociations is empty. This is preserved source behavior, not newly
implemented native persistence. Its Update recreates the transient shield
and rederives allied planet-slot targets; actual late reload must verify
protection reconstruction. The unused original declaration
TALORIAN_LAUNCHER_SAVELOAD remains56/targetIDs4/bShildOn52 in the existing
layout check. No save bytes or format are added, and no source return is
changed to conceal missing production functionality.

Optional Clang compile/link passes. Focused/affected checks and actual
normal F8 from the unchanged real Kasse15 pre-Matrix F7 state are in progress
(`talorian-matrix-clang`), followed by normal return/map/Forger T/free-slot
construction. Active Fighter/Recon associations in that later state are
part of this real reload countercheck; no manipulated mission data or
script call is used. Full isolated GCC/Clang, Matrix completion/visible
shield, combat/end, sanitizer lifecycle and native Windows/retail remain open.


`talorian-current` completes all five affected Linux build/non-GPU sequences;
the five focused optional Clang checks pass. Actual `talorian-matrix-clang`
normal startup/P/Escape/F8 loads the unchanged real Kasse15 save; original
Fighter damage again executes after reload (5.4 effective hit on the friendly
gate). Visible original Forger label/life bars, HUD and pause are inspected.
Normal gate right-click440/200 returns to Solar (warp agent7801); f/Insert,
normal map left-click403/165 centers Solar2. Forger T and actual free-slot
left-click343/286 issue original Matrix construction. Normal F7 shows mode1
moving to target56940/106496, working7840/building33620. Subsequent normal
F7 reaches FAB_BUILDING mode8, moveActive0, real buildee60257; the source
platform model is visibly present on the planet slot. No Talorian provider
failure or render assertion occurs. This is construction in progress, not
ready20 or protection acceptance. Source removes old13/14/15 objectives during
the construction trigger. Ordinary F8 from this actual building F7 is now
being checked for continued construction and completion; no Stop/reissue or
state manipulation is used in this countercheck.


Actual Matrix building F7/P-independent ordinary F8 restores buildee60257,
working7840, mode8 and original construction. Normal continuation with no
Stop/reissue completes original20 and lists21; Forger becomes mode0 with
working/buildee0. The ready Matrix model and actual animated Talorian ring
around Solar2 are visually inspected (`talorian-ready-check`); source
STMG36 progression is logged. Normal F7 preserves this later ready20 state
externally; finished-state F8/protection reconstruction is being repeated.
This is bounded native Clang external-data runtime, not full combat/end or
Windows retail comparison. Earlier moving-build reload remains a separate
unresolved regression; this successful building-stage reload does not erase it.


Finished-state ordinary F8 also visibly restores the Matrix model, original
Talorean Matrix hover label/life bars and recreated Talorian ring; natural
Polaris Enemy Sighted alert text remains visible. Original close exits0.
The first integrated `talorian-lifecycle-asan` starts normally and ordinary
P/Escape/F8 loads the unchanged later ready20 checkpoint. It fails UBSan,
exit1, in AnimInstance::Randomize at anim2d.cpp:398: real
StasisBolt::ResolveAssociations → ObjectList::ResolveAssociations →
Mission::Reload/QuickLoadFile. Native RAND_MAX allows signed int overflow
in rand()*4 (2026334764*4). No successful sanitizer lifecycle is claimed.

Local hypothesis: widening only this visual animation multiplication before
conversion to SINGLE removes the native overflow and retains original
mathematical rate computation and the same three RNG calls/order. The
Windows expression remains unchanged; no simulation RNG consumption, update
order, fields, persistent format or interface changes. The existing native
shell synthetic sfx-smoke contract now exercises real AnimInstance::Randomize
with a high first native random value, rate/phase/total-time checks and the
expected next RNG draw after three calls. It runs only outside mission-start;
no seeded test draws enter gameplay. The old expression fails the existing
instrumented native_shell CTest on1804289383*4; native S64 multiplication
passes that same CTest. Native Windows/visual retail comparison is still open.
Actual same unchanged ready20 normal F8 repetition is `talorian-random-asan`.
ASan/UBSan covers the native owners; leaks, training speech/unit voices/unit
alerts, movies, mission SFX/music, portraits, non-base textures, legacy
lighting and space background are excluded in this bounded run. Original
Sector comm can use the existing shared decoder; external SDL/FreeType/
FFmpeg/Vulkan driver remain uninstrumented. Affected five-tree sequences,
later integrated reload/close and full training remain in progress.


### 2026-10-11 — real repeated later-state reload: launcher archetype allocator

The actual `talorian-random-asan` ready20 checkpoint load passes the corrected
animation randomization and reaches source20 complete/source21 listed. The
second ordinary F8 fails ASan alloc-dealloc-mismatch, exit1: real menu
QuickLoadFile → Mission::close → ObjectList::Close/flushUnusedArchetypes →
TalorianLauncherFactory::DestroyArchetype. Its private OBJTYPE custom new uses
calloc; absent a matching class delete, destruction uses global sized delete.
Normal close of another run confirms the same owner failure. Allocator checking
remains enabled; neither failed run is accepted as successful lifecycle.

Local falsifiable hypothesis: adding native-only OBJTYPE delete/free, preserving
the original allocation and destructor/ARCHLIST release, fixes that exact close
boundary. The smallest countercheck is a second ordinary F8 of the same genuine
ready20 F7 checkpoint followed by normal window close under ASan/UBSan.
No fields, serialized formats, public interfaces, simulation or RNG calls change;
Windows source remains unchanged. Existing instrumented app_schema/native_shell
checks pass; the corrected-platform-guard `talorian-free2-current` five-tree
sequence and `talorian-free2-asan` real repeated F8/close are in progress.
The run uses the documented existing instrumented flags/exclusions, with leaks
disabled and external SDL/FreeType/shared FFmpeg/Vulkan uninstrumented.

Host GPU diagnostics remain failed: real Wayland SDL flags report MAXIMIZED
on the transiently floating probe; external-only SDL_RestoreWindow/SyncWindow
before resizing does not clear it or change640x360. Removing NOT_FOCUSABLE also
did not fix it. These unsuccessful environment counterchecks make no renderer
or gameplay acceptance claim and add no repository test replacement.


The corrected native Talorian free now passes its actual boundary; the next
second-F8 failure is ReconLaunchFactory::DestroyArchetype, again calloc/global
sized-delete. Source review finds the same private archetype allocation gap
in the directly connected RepellentCloud/RepulsorWave factories. Native-only
class delete/free now matches each original allocation; destructors and child
ARCHLIST/ENGINE releases remain unchanged. `recon-free-current` rebuilds all
five trees; focused instrumented app_schema/native_shell pass. Actual
`recon-free-asan` loads the unchanged genuine ready20 snapshot through normal
P/Escape/F8, inspects the real ring/hover label, saves with F7, visibly pauses
with P, resumes with Escape and loads again with F8. Source20 complete/21 listed
returns; the recreated ring/label are visually inspected after that second
load. Normal window close exits0 with no ASan/UBSan error. This is bounded
instrumented later-state native runtime, with the documented exclusions/leaks
OFF; TextureLibrary still reports dangling references. It does not satisfy
full-training or absence-of-owner/media-leaks acceptance.

### 2026-10-11 — original victory progress owner, DOS file boundary

Source review of the actual SolTrain_KillTheMantis Done state confirms
MScript::EndMissionVictory(0) conditionally invokes GAMEPROGRESS for Terran
players before posting the ending event. Sol_Train is Solarian, so this does
not establish a null dereference on its actual victory branch. That global
was null in the earlier native closure; its original owner is needed by the
regular menu/progress chain, including campaign selection. Original
GameProgress.cpp now joins the existing mission owner, registers its original
GlobalComponent and cleanup, and uses the explicit native SAVEDIR root in
place of the Windows named-player registry/directory lookup. Its three-U32
PROGRESS_DATA remains12 bytes (native static assertion); player.gpf, bit
semantics, public interface and Windows source behavior stay unchanged.
Native startup requires the owner and reads original completed/seen getters;
no mission-completion setters, synthetic script calls or changed objectives
enter gameplay. Actual victory persistence remains unexecuted.

The first local filesystem hypothesis is falsified by source review:
OpenNativeSaveDirectory accepts UTF children only, while GameProgress uses
explicit DOS raw files. The existing writable adapter now accepts precisely
DOS CREATE_ALWAYS/read-write and OPEN_EXISTING/read children at that root.
The existing raw reader handles DOS reads; existing writable-file state handles
raw WriteFile publishing without UTF wrapping. Unsupported accesses still
fail. Native GameProgress remains the original production owner, not a parallel
progress service. The existing portability.data_root check now exercises a
synthetic twelve-byte raw player.gpf through creation, actual write/release,
reopen/read and exact bytes/physical size alongside its previous mission UTF
round trips. It first fails the new raw write contract because archive-root
state is a directory; assigning the raw implicit-file state fixes that concrete
adapter defect. Instrumented existing portability.data_root/native_shell now pass. No synthetic
progress fixture is used for gameplay/end acceptance.

Remaining original end route is source Mission::EndMission → ending modal
result → original Menu_EndGame and Continue/front-end return. Native shell
currently closes/exits after the result, skips the success message and has not
yet integrated the original end menu/regular selector. Source GAMEPROGRESS
availability alone cannot close this gate. Full combat, every-step fresh visible
GCC/Clang and Windows Debug/Final/ABI/retail comparison remain open.


2026-10-12 continuation: `game-progress-dos-current` rebuilds all five
existing trees and passes the affected checks (central totals in BUILD_GRAPH).
Instrumented `game-progress-asan` starts through the documented normal entry,
reads original completed0/seen0, creates actual external12-byte player.gpf,
loads the unchanged ready20 checkpoint with F8 and closes normally, exit0.
No ASan/UBSan report; same explicit exclusions and leaks OFF apply. Actual
victory setters/end-menu/return and every-step GCC/Clang remain unexecuted.
`combat21-clang` resumes that genuine later checkpoint using the documented
font/decoder-enabled Clang command without training-speech/unit-voice/alert
skips. Normal Insert/full-map capped zoom and Solar Prime click, then original
Greater Pavilion model selection reveal its real source production menu.
Trireme/Monolith production and combat continuation remain in progress.


### 2026-10-12 — actual Trireme production closes its Destabilizer dependency

`combat21-clang` ordinary Greater Pavilion selection and source T production
aborts134. Matching core1390120 confirms BuildPlat::updateBuild →
GunboatFactory::CreateArchetype(GBOAT!!S_Trireme) →
ReconLaunchFactory::CreateArchetype(RL!!S_Destabilizer), pBoltType assertion:
real DESTABILIZER has no registered provider. Available memory excludes host
resource exhaustion; matched thread stacks implicate the production owner,
not SDL/FFmpeg/GPU. Extracted diagnostic core is removed after read-only use;
original system core/raw logs remain outside the repository. No queued ship
is accepted as completed.

Local falsifiable hypothesis: source Destabilizer.cpp WPN_DESTABILIZER factory
in the existing gunboat object target supplies this exact production dependency.
The smallest countercheck is the same normal Greater Pavilion T build after
loading the unchanged real ready20 checkpoint. Native prefix/exact-case
includes and equivalent std::max replace only platform compilation boundaries;
original object/factory lifecycle, fields, ability/update/RNG and persistent
formats remain unchanged. The private OBJTYPE already matches calloc/free.
Existing portability.app_schema now checks DESTABILIZER_SAVELOAD188/base44,
transform offset44/final mission extent188 and original schema correlation of
owner/target IDs, grid position, target counts, flags, timer and transform.
Optional Clang compile and existing app_schema pass. `destabilizer-current`
five-tree builds and `destabilizer-clang` ordinary production repeat are in
progress. Ability execution, its visual rendering, live save/reload and actual
full combat/end remain unproven; Windows Debug/Final/ABI/retail are unavailable
and open. No fake provider or skipped assertion replaces this source owner.


### 2026-10-12 — original end controls, compilation and pending actual victory

Local hypothesis: the already integrated original Frame/Button2/Static/
TabControl owners can render original CQMessageBox and MenuEndGame through
normal source modal handling. Native Menu_Confirm now uses those owners and
original resources/IDs/priorities/pause/cursor/scroll handling; only the native
bounded wide-string copy/format and unavailable OS MessageBox fallback are
adapted. Missing original resource throws an explicit error, never a successful
placeholder. Mission::EndMission now reaches the original success/failure OK
dialog on native platforms too. NativeShell follows the original mission-close,
front-end resolution, FULLSCREEN update and unused-archetype flush before
CreateMenuEndGame on a real WON/LOST modal result. No synthetic mission-result,
script call or goals are injected.

Original EndGame statistics/Continue/tabs/rollups remain production code.
The original listbox is created, initialized, then unconditionally hidden,
without item use; native omits that unused hidden control rather than supplying
a fake IListbox. Its field in GT_ENDGAME and Windows behavior remain unchanged.
No menu/persistent declaration layout or module interface is changed; native
only declarations expose the existing CQMessageBox overloads/constants.
Existing app_schema now checks original GT_ENDGAME/GT_MESSAGEBOX sizes and
field correlation of Continue/OK origins, screen extent and static dimensions.
Optional Clang compilation and existing app_schema/native_shell pass;
`end-menu-current` affected five-tree sequence passes (central counts in
BUILD_GRAPH). Actual success
OK, stats render/tabs/Continue and original selector/return remain unexecuted;
current native composition still exits after the end-menu result. This is
compilation/synthetic schema evidence only, not completed training. Native
Windows Debug/Final/ABI/retail comparisons remain unavailable/open.

Meanwhile actual `destabilizer-clang` same normal Greater Pavilion T production
passes the matching-core blocker; four regular Triremes reach full600 hull and
350 supply in genuine F7 state, visibly inspected models/HUD/Unit Built and
original cost hints. Original Trireme/Monolith hints show their dependencies,
costs and source600-ms hover behavior; insufficient-gas Monolith request
produces the real unit voice instead of bypassing costs. No Monolith completion
is claimed. Original Destabilizer special-ability use/visuals and integrated
ASan live new-unit reload remain open. `destabilizer-current` affected builds
and schema checks pass; actual main-weapon combat is continuing.


Actual optional Clang pause countercheck on the later four-ready-Trireme state:
normal P pauses, F7 savesA, wait10 real seconds, F7 savesB, Escape resumes,
wait10 seconds, F7 savesC, then P pauses again. Read-only inspection uses the
existing DecodeGlobalData3236-byte disk adapter (not a direct memcpy of the
3812-byte native wchar-expanded runtime type). updateCount A=B=209087;
C=209376. Existing read-only gunboat/gate facts show identical paused IDs,
players, systems, positions, hull, supplies, stances and targets. This directly
proves bounded original pause/resume simulation semantics in this actual run;
GCC/full-run repetition and reload-to-victory remain open. Genuine external
C checkpoint now includes four600-hull/350-supply regular Triremes. No bytes or
mission state were modified for the check.


Source-review correction: Sol_Train onButtonTraining selects M_SOLARIAN;
EndMissionVictory's progress setters are conditional on M_TERRAN. Therefore
no guaranteed Sol_Train victory null-dereference is inferred from the earlier
null GAMEPROGRESS global. The original GlobalComponent/regular menu owner and
raw-file countercheck remain valid integration; Solarian completion must not
be made to set a Terran progress bit. No such setter was added. Actual source
Menu_campaign has a dedicated IDS_SOLARIAN_TRAINING button calling original
onButtonTraining/DoMenu_Briefing; it does not require traversing Terran missions.
Regular entry/return closure remains the next menu integration boundary.


### 2026-10-12 — original training selector and initial global ownership

Local hypothesis: the original Solarian training button needs only the existing
Frame/Button2/Edit2/Static owners and original GT_MENU1 resource; Terran campaign
selection and multiplayer are not dependencies of Sol_Train. Menu_campaign now
joins the existing toolbar target. Native explicitly disables the unsupported
Terran/Mantis buttons; it retains the real Solar training button, Back and
original onButtonTraining → DoMenu_Briefing → Mission loading path. Missing
resources/unsupported campaign paths fail explicitly, with no successful
placeholder or alternate UI. Native --mission-campaign requires visible
--mission-start with Sol_Train.dmission and excludes --mission-briefing.

The smallest existing app_schema countercheck initially failed: GT_MENU1 native
size21588 versus original20052. The difference was exactly the unused three
arrays of eight computer names, each32 UTF-16 code units expanded by native
wchar_t. DMenu1 preserves those native resource-only names as U16[32];
Windows and original _ADB schema declarations are unchanged. The extended
existing app_schema checks the actual native whole record and Solar button
field correlation. No persistent format, public interface, script objective,
RNG call/order or simulation rule is changed. Native Windows Debug/Final/ABI
execution remains unavailable and open.

First native selector execution aborted before presentation at original
MGlobals::GetPlayerRace(playerID0). Core metadata confirms SIGABRT; the binary
was rebuilt before symbolization, so no mismatched-binary backtrace is claimed.
Source review follows Menu_campaign focus → MissionGetSinglePlayerRace and
the original Win32 GlobalViewer::init → initGlobalData boundary. The falsifiable
fix is native composition calling the existing MGlobals::New after
CreateGlobalComponents, before any campaign mission exists. This real owner
initializes the original defaults/player1 and global-data resources without
RNG calls or automatically setting mission objectives. Preloading a mission
would not test this entry and was not used.

Visible optional Clang repeat on the restored physical HDMI output, empty
external save directory: original Single Player selector renders Solar
resource caption “Celareon Training”; normal left click at(400,333) opens
original Sol_Train briefing with text/speech. Normal Start at(350,543) loads
source training, renders HUD/models/subtitles and lists original objective1.
Normal f selection completes1; ordinary right click completes2. Normal F7
creates the genuine early mission save and P visibly pauses. This is native
runtime on lawful external retail data, not a full training or Windows/retail
comparison. Captures/raw logs/helpers remain outside the repository.

Reproduction: use the existing optional font/shared-LGPL mission command in
this document, replace --mission-briefing with --mission-campaign, and supply
a newly created empty writable external --save-root. Use normal Solar training
button and Briefing Start. Native composition is also wired to original
CreateMenuEndGame after genuine WON/LOST, then original campaign selector;
a subsequent Start resumes the source modal mission loop. That winning
return/restart path remains unexecuted and must not be called accepted.

Affected campaign-owner five-tree compile/non-GPU checks pass in
campaign-entry-current and campaign-globals-current, including the last
composition initialization addition (central counts in BUILD_GRAPH). Host GPU resize remains failed, not skipped or passed. Next:
actual objective21 combat/Kasse2/all-enemy elimination, original victory OK,
stats/Continue/selector return; fresh every-step GCC/Clang with early and late
ordinary reload-to-victory, pending build reload and owner/media cleanup.
M2 remains not accepted.

Actual early optional Clang F8 repeat in the selector-started mission restores
completed1–2/incomplete3; normal lasso completes3 again and lists4. F7 preserves
the continued state. The subsequent combat diagnostic loads a previously
regularly played whole later checkpoint through normal F8, without modifying
its bytes; it is explicitly not a fresh full-run acceptance. Normal Insert,
wheel zoom and planet navigation visibly restore explored terrain, Solar
planets and the gate link. The earlier small-map appearance alone therefore
does not establish lost exploration. Normal lasso selects four original
Triremes; regular gate command jumps all four to Kasse/system1. Actual F7
records original hull600, supplies335–350 and natural attack targets.
Completion/Kasse2/victory remain under test.


### 2026-10-12 — original mouse-scroll activation across ordinary F8

Actual four-Trireme Kasse continuation exposes a native lifecycle regression:
normal F8 calls Mission::Close/Load within the existing game modal. The native
MScroll CQE_MISSION_CLOSE addition called SetActive(0), whereas the original
Windows owner retains its activation. Only Conquest/native initial entry
activates scrolling; ordinary quick-load does not re-enter that composition.
Visible old-binary map zoom/navigation works, but repeated Down and held
bottom-edge pointer do not pan. This blocks normal exploration of Kasse2.

Falsifiable local hypothesis: close should clear the cached mouse position
and release cursor resources while retaining the original activation. The
native close callback now clears desiredOwnedFlags/releases resources without
changing bActive. This preserves both intentionally script-disabled scrolling
and originally active scrolling across reload; no unconditional activation
is added to load. Existing CheckNativeMouseScroll in portability.native_shell
checks activation retained, cached input cleared, resources released and no
scroll/resource grab until new input after close. Optional Clang build and
that focused synthetic lifecycle check pass. No field/interface/disk format,
Windows implementation, script, simulation or RNG is changed.

The unchanged genuine normal F7 Kasse checkpoint contains four regular
Triremes in system1, natural attacks against Warlord Agar and source21 pending.
Affected scroll-reload-current five-tree builds/non-GPU checks pass;
new scroll-reload-clang visible ordinary F8 repeat reaches the genuine
Kasse checkpoint. Visible panning is still being verified. Native Windows
Debug/Final/ABI and original retail comparison remain unavailable/open.
This lifecycle check alone is not gameplay acceptance or M2 completion.

Visible optional Clang countercheck after the correction: ordinary F8 reaches
the unchanged Kasse checkpoint; normal Insert/wheel and held Down/Up visibly
pan the original fullscreen map in both directions. Normal map left-click
then navigates Kasse Prime. The old-binary failure is closed for this bounded
reload/pan path, with GCC/ASan visible repeats and full training still open.
Original fixed fullscreen scroll increments are unchanged.


### 2026-10-12 — Retail stance resource → original command-owner translation

Actual regular Kasse2 fighting exposes a more precise limit than the earlier
“Attack Stance click emits134” checkpoint: normal clicks posted the original
resource hotkey134, but actual F7 retained US_DEFEND0. Source ObjectComm handles
IDH_STANCE_ATTACK183;134 is a different CQ2 source-table command. Read-only
external GT_TOOLBAR inspection confirms all three races' original stance
resources: Attack134/text579, Defend135/text580, Stand137/text581, Idle136/
text582. Those semantic tooltip IDs match HBTNTXT's original source enum.
Stop/cancel6 already matches and is not remapped. No retail table/file bytes
or IDs are rewritten. This is a resource/source compatibility boundary,
not proof that all old toolbar commands were already functional.

Falsifiable hypothesis: native original HotButton must translate only these
four exact resource-ID/semantic-tooltip pairs when posting to the existing
CQE_HOTKEY/ObjectComm owner. Keyboard source-table events remain untouched.
HotButton now retains its original hotkeyID/text/resource/priority and maps
only that native command handoff; flashing and displayed key text use the
same resolved source command. Windows code remains unchanged. No control
layout, public interface, serialized field, command algorithm, simulation,
update order or RNG call changes. Unknown semantic pairs and already-native
command IDs pass through unchanged.

Existing portability.native_shell sfx-smoke now creates an isolated *real*
registered EventSystem aggregate, queues through the same HotButton posting
function, updates the actual event owner and inspects the exact CQE_HOTKEY
messages for four stance pairs plus unrelated/already-native cases. The
proper pre-fix countercheck fails with expected183/received134; the first
fixture attempts lacked correct aggregate-query/update setup and are not
claimed as that counterevidence. Corrected function passes optional Clang
focused native_shell. This is synthetic dispatch/lifecycle evidence, with
no production mission/script state injected. Actual ordinary click/F7
stance-state verification is under way; affected stance-current five-tree
build/non-GPU checks pass (central counts in BUILD_GRAPH).

The genuine regular Kasse2 checkpoint now contains three surviving Triremes
and actual Mantis damage; their before-state is US_DEFEND0. The diagnostic
continuation restarts via the original selector/Briefing Start and normal F8
loads that unchanged whole save. Captures/logs/readers/save copies remain
external. Actual victory/end/return, every-step GCC/Clang and Windows/retail/
ABI remain open; M2 is not accepted.

Actual optional Clang normal lasso/Attack-button click after unchanged F8:
original CQE_HOTKEY183 executes and F7 records all three selected Triremes
with stance1/US_ATTACK, replacing previous stance0. Visually inspected
original StatusBar now shows “Attack Stance (Shift+A)”, selected stance art
and original unit low-supply label47%. Source KillTheMantis saved state1
(FinishOffMantis) confirms Kasse2 visibility trigger executed; the read-only
reader uses the actual saved program filename, whose numeric index changes
across normal reload. No state bytes are edited. Actual further combat/end,
ASan/GCC visible repeats and full training remain open.


### 2026-10-12 — original Monolith launcher closure: Tractor

Normal original Greater Pavilion M production now reaches GBOAT!!S_Monolith
and reports missing TRAC!!S_Tractor. GunboatFactory's original launcher-load
loop breaks on that failure rather than failing the whole ship archetype;
the partially produced Monolith is explicitly not accepted as functional.
The before-fix partial-production save/capture stays external and is not
used as a successful capability. No missing provider is replaced with a
placeholder or a forced success. Monoliths are the original training's
fleet recommendation, not a new scripted count requirement.

Local hypothesis/smallest falsifier: original Tractor.cpp WPN_TRACTOR can join
the existing gunboat object target through already integrated animation,
hardpoint, weapon, original PrimitiveBuilder and render adapters; repeat
ordinary M from the unchanged genuine pre-production save. Only native
prefixes/exact-case includes are adapted. Factory/archetype/animation ownership,
launcher targeting/update/damage/refire/supply, draw and save behavior remain
original. No fields/interface, simulation/RNG or serialized formats change.

Existing portability.app_schema now protects original TRACTOR_SAVELOAD20,
field offsets0/4/8/12/final20 and real constructor correlation of target/system/
time/refire/mass. Optional Clang native_shell/app_schema pass. Affected tractor-current five-tree builds/non-GPU checks pass
(central counts in BUILD_GRAPH); visible tractor-clang normal selector/Briefing/
F8/M repeat remains in progress. Actual completed production, active ability/
visuals, new launcher save/reload/lifecycle and integrated sanitizers remain
unproven until executed; full training/Windows Debug/Final/ABI/retail remain
open. No general weapon/UI rewrite is introduced.


Actual optional Clang `tractor-clang` continuation (2026-10-12): original
campaign selector/briefing Start, ordinary F8 of the unchanged genuine
Kasse2 attack-stance save, Insert/wheel/map-left-click to Solar Prime,
Greater Pavilion selection and normal M now complete a Monolith. The
visually inspected selected original HUD shows Hull800/800, Supplies500/500
and the normal command controls; original F7/read-only save inspection
confirms player1/system2/hull800/supply500. No TRAC missing-provider report
appears. The whole ordinary save is preserved externally as
`monolith-ready-tractor-ui-clang.mission`; this is a diagnostic continuation,
not a fresh full training run. Normal F8/new-launcher lifecycle and integrated
sanitizer repetition are the next checks, with active Tractor ability and
actual combat/end/return still unaccepted. The Kasse Triremes have exhausted
supplies; further normal resupply/fleet commands are necessary rather than
counting stationary combat frames as completion.


Actual new-launcher lifecycle follow-up: `tractor-clang` normal F8 restores
the finished Monolith visibly; normal window close exits0. The preserved
whole F7 contains the original Monolith Turret-2/TRACTOR_SAVELOAD20.
`tractor-lifecycle-asan` uses the existing optional Clang ASan/UBSan tree
and unchanged external save copy, ordinary initial F8, selected Monolith
F7/P/Escape/second F8, visible selected HUD800/800 and500/500, normal close
exit0 with no ASan/UBSan diagnostic. Its new ordinary F7 also contains the
original20-byte Tractor record (instance index changes normally). This
executes real owner creation/load/save/reassociation/release; active ability
rendering/target retention and training-to-victory are not covered.

Reproduce with the documented optional sanitizer flags/environment and
existing `menu-reload.py tractor-lifecycle-asan`, then
`normal-ui.py click:l:331:187 key:F7 key:p key:Escape key:F8 wait:12`
and normal selected-ship inspection/close. Helpers and captures stay external.
ASAN_OPTIONS=detect_leaks=0; UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1.
Training speech, unit voices/alerts, movies, notification cue, space
background, animated portraits, mission SFX/music and legacy lighting remain
explicitly excluded; shared system SDL/FreeType/FFmpeg/Vulkan are not
instrumented. TextureLibrary dangling-reference messages persist at close;
this is not leak-clean ownership acceptance. Native Windows Debug/Final/ABI
and retail behavior/visual comparison remain unexecuted.


Separate optional GCC `tractor-gcc` visibly executes the original Solar
selector (retail caption Celareon Training), normal click400,333, source
briefing text/Start350,543 and initial objective1/subtitle/models. An
unchanged finished-Monolith checkpoint is then loaded through ordinary
P/Escape/F8 for the bounded later-state continuation. This closes GCC
*initial selector entry* evidence, not a fresh full GCC run or winning return.
The source starts the initial mission before that F8; no mission state is
preloaded to satisfy selector initialization.


### 2026-10-12 — actual Patrol button dispatch regression

Separate optional GCC normal selected Monolith toolbar click315,495 aborts
134. Matching original binary/core PID1444694 proves HotButton posts retail
Patrol81 to real EventSystem/CQE_HOTKEY, then MenuResource::Notify sees0x51/
source IDH_GAME_OPTIONS and explicitly aborts at NativeMenuUnavailable.
Resource exhaustion is not indicated (48GiB available/zero swap used);
worker thread tops are waiting, with actual failure in the source modal
main thread. Extracted core was held at a fresh temporary path and deleted;
metadata/logs/captures/genuine saves remain external. The last normal F7
preserves Monolith's Kasse transit; no lost unsaved progress is credited.
The clicked eye icon is the original Patrol control, not the Tractor ability.

Falsifiable local hypothesis: extend the existing native HotButton handoff
only for exact retail81/HBTNTXT::PATROL -> original IDH_PATROL109. Source
resource/tooltip fields, unknown81 pairs and already-native109 pass through
unchanged. Existing native_shell real isolated EventSystem check is extended
with those three cases; it fails expected109/received81 before correction
and passes after. Windows code, fields/interfaces/layouts, serialized IDs/
formats, gameplay algorithm, simulation/RNG/update order remain unchanged.
Actual correct ordinary Patrol/control cancellation/continuing movement and
affected builds are being repeated. No options placeholder is introduced;
this repairs an incorrectly dispatched production command.


Visible optional Clang `patrol-clang` countercheck: original selector/Start,
ordinary F8 of the genuine GCC Monolith transit save, selected ship and
normal Patrol315,495 now emit CQE_HOTKEY109. The original visible StatusBar
reads “Left-click the spot you want to patrol.” Escape leaves target mode,
and a subsequent normal mini-map right-click545,553 posts a real move:
original Monolith voice “Yes Sir”, F7 moveAgent11535/bMoveActive1. No options
dialog or abort occurs. The initial transit checkpoint had agent0 (automatic
gate exit), not a pending user-issued move; that checkpoint must not be
used to claim a moving-order reload failure or its correction. A new genuine
user-move F7/F8 countercheck is in progress. Active Tractor was not clicked:
the tested eye icon was Patrol; active-ability evidence remains open.
Affected patrol-current builds/checks pass, with central results in BUILD_GRAPH.


Actual optional Clang user-issued Monolith move reload countercheck:
normal mini-map right-click yields F7 position55303.9,71680/system1,
moveAgent11535/bMoveActive1. Without reissuing a command, ordinary F8,
15-second observation and F7 yield57084,67523.8/system1, same agent11535/
bMoveActive1; normal later map navigation shows arrival at Kasse2 and the
Monolith's ordinary blue attack beam. Whole before/after saves remain
external (`monolith-real-map-move-ui-clang.mission`,
`monolith-real-move-reloaded-ui-clang.mission`). This protects a real
Gunboat moving-order continuation; it does not close the separate earlier
Forger pending-build-order reload question. No script/goal/state bytes are
changed, and no active Tractor-special use is inferred from a normal beam.


### 2026-10-12 — original general-platform HUD owner

Normal optional Clang Eutromil selection576,345 after four regular ships
return to Solar4 for supply shows the correct field label/voice but retains
the Greater Pavilion HUD title. Original CMenu_BuildInd releases panel
ownership on non-production platforms; the original CMenu_GenPlat required
by Eutromil/JumpPlat/Matrix is absent from the native target, so no successor
updates the shared shipclass control. Likewise the observed Xeno selection
requires original CMenu_Research, still absent; no research objectives are
invented or marked complete from that selection.

Falsifiable local hypothesis/minimal repeat: compile original CMenu_GenPlat
into the existing toolbar object target and repeat the same normal Eutromil
selection, expecting original generic-platform HUD/title/location/hull and
release of the previous production panel. Native prefix, bounded existing
wide-format adapter, pointer-sized event decoding and original cleanup-slot
registration follow the adjacent CMenu_ResBuild/BuildInd boundary. No fields/
public interface, persistent layout, resource IDs, UI selection rules,
simulation/RNG, update order or save format changes; Windows code is retained.
The first compile fails on the cleanup-slot type; tests inadvertently run
against the older executable after that failure are not owner evidence.
Corrected typed native registration compiles, then existing optional Clang
app_schema/native_shell pass. genplat-current affected five-tree builds/checks
pass (central results in BUILD_GRAPH). Visible repeat and instrumented
selection/close remain required.

Ordinary supply is already actual, independently of this HUD change: real
F7 records the returning three Triremes restored to350/350 and Monolith500/
500 around existing Solar4 Eutromil. A second normal Monolith and fourth
Trireme finish through the original Greater Pavilion M/T queue, recorded
with800/500 and600/350. The original recommendation remains optional; these
ships support the real remaining battle rather than adding a script gate.


Visible `genplat-clang` ordinary F8/Eutromil click now releases the previous
production panel and displays correct original Hull600/600. Its name and
location initially render as single letters E/S: direct POSIX wide printf
interprets original Windows resource `%s` as narrow. This counterevidence
is not accepted as correct text. The existing NativeAppTextResources
FormatNativeUnitSubtitle adapter now handles the same two resource IDs,
name and location, as adjacent BuildInd/ResBuild/Ind already do. Bounded
numeric formatting is retained. Optional Clang focused build/native_shell/
app_schema pass; visible `genplat-format-clang` repeat and affected
`genplat-format-current` checks are under way. Normal original-owner close
in the pre-format `genplat-clang` exits0; instrumented lifecycle/Windows
Debug/Final/ABI and full training remain open.


Actual corrected general-platform text repeat: `genplat-format-clang`
normal original entry/F8/Eutromil click displays full Eutromil, Location:
Solar, Hull600/600 and released production controls. The next normal Xeno
click361,321 instead retains Eutromil title with empty information, directly
confirming missing original CMenu_Research rather than correct selected-unit
HUD. genplat-format-current all affected builds/checks pass; normal close
exit0. Integrated ASan/GCC HUD repeats remain required.

### 2026-10-12 — original research-platform HUD successor

Local hypothesis/smallest countercheck: add original CMenu_Research to the
existing toolbar target and repeat ordinary Pavilion/Eutromil/Xeno selection,
expecting original source panel transitions and Xeno resource controls.
Researching every narrated ability remains optional, not a script condition.
The original controller uses existing BuildButton/IActiveButton, IQueueControl,
IBuildQueue/IUpgrade and original USRBUILD/NETPACKET; no parallel controller,
forced goal or substitute successful provider is added. Native prefix/case,
bounded numeric and existing NativeAppTextResources string formatting,
pointer-sized decoding of scalar event payloads and typed cleanup registration
follow the adjacent owners. No fields/interfaces, resource IDs, persistent
layout/format, simulation/RNG or command algorithms change; Win32 code remains.
First compile exposes queue-removal scalar param truncation; the corresponding
native width conversion compiles and existing optional Clang app_schema/
native_shell pass. Visible research-clang repetition and affected builds are
under way; actual research queue/save/lifecycle/sanitizers and Windows/retail/
ABI remain unexecuted.


Actual optional Clang `research-clang` normal original entry/F8/Eutromil/
Xeno selection now shows Xeno Chamber, Location Solar, Hull400/400, original
stop control and five original research images. Previous Eutromil/production
controls release normally. This is visible native runtime on unchanged
external data/save, not only a draw counter. No research goal is injected;
normal research hover/queue and integrated lifecycle remain to execute.
research-current affected five-tree builds/non-GPU checks pass (BUILD_GRAPH).


Research hover follow-up: ordinary Xeno button72,524 after the source
600ms interval displays original “Research MASS DISRUPTER” hint text and
Polaris description, with original special-weapon resource costs/status.
Button115,524 actually displays original Shroud/Aurora research, not Tractor;
no Tractor use is inferred from that icon or external capture filename.
No direct SetText/script call supplies these gameplay hints. Normal empty
field click40,180 releases research controls/hint and shows the appropriate
Cygnus field hint, but leaves the old Xeno title. This exposes absent
original CMenu_None selection owner. `research-clang` normal close exits0.

### 2026-10-12 — original empty-selection HUD owner

Local hypothesis/smallest repeat: original CMenu_None must join the existing
toolbar target to claim the original `none` panel when GetSelectedList is
empty and clear the shared title. Repeat normal Xeno selection then empty
field click; expect empty title/controls, not the former selected platform
name. This is original production SetText from ordinary selection events,
not an artificial direct Hintbox/SetText acceptance call. Native prefix,
pointer-sized scalar event decode and existing typed cleanup registration
preserve all fields/interfaces, resource IDs, Windows behavior, algorithms,
simulation/RNG and formats. Existing optional Clang app_schema/native_shell
compile/run pass. Affected none-current builds and actual integrated
ASan/UBSan HUD/empty-selection/reload/close repeat are being run; Windows/
retail/ABI and full training remain open.


Actual HUD lifecycle first pass: `python /tmp/frontierx-m2-runtime/menu-reload.py hud-lifecycle-asan` loads the unchanged genuine research-hud-fleet-ui-clang F7 checkpoint through normal F8. Ordinary clicks Eutromil576,345 → Xeno361,321 → empty40,180 show Eutromil/Location Solar/Hull600/600, Xeno Chamber/Location Solar/Hull400/400 with original research controls, then empty title/controls and the appropriate Cygnus field hint. Captures are inspected outside the repository. This directly falsifies the former stale-title behavior after original CMenu_None integration. none-current affected builds/checks pass (BUILD_GRAPH). Second normal F7/P/Escape/F8 and close are being executed; no full-training or leak-clean acceptance is inferred.


Integrated instrumented repeat completes: ordinary F7/P/Escape/second F8 followed by the same Eutromil/Xeno/empty clicks visibly restores both correct platform HUDs and clears the empty-selection title. Normal compositor window close exits0, without ASan/UBSan diagnostics. Scope: five-tree native Debug rebuilds, instrumented App/platform owners; lawfully external unchanged genuine later save; `ASAN_OPTIONS=detect_leaks=0`, `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`, speech/unit voice/alerts and documented excluded media disabled, optional shared decoder still required for Sector comm. SDL/FreeType/shared FFmpeg/Vulkan are not instrumented. TextureLibrary reports dangling references at close; this is not a leak/complete-owner-cleanup pass, research queue/full training/Windows Debug/Final/module ABI/retail comparison remain open.


Normal combat continuation `combat-final-clang`: original Solar selector400,333 → briefing Start350,543 → actual initial mission frame, then normal P/Escape/F8 loads the unchanged genuine research-HUD F7 checkpoint. Ordinary world cursor/Down pans, lasso and actual Control_L down/mouse drag/up add both Prime ships and remaining Solar4 ships; the inspected source Group HUD shows six individual silhouettes (four Triremes/two Monoliths). No source command/list/script call is used. Original fullscreen map Insert/wheel/navigation then actual right-click Solar gate396,300 issues the six-ship jump order; read-only genuine F7 shows the original shared jump agent, not an injected position. This is a bounded later continuation, not a complete new every-step training acceptance.


Actual later combat success, optional Clang native runtime on external data: all six ships finish the original gate jump to system1; normal left gate click changes view, Group Attack316,554 posts183, right/left minimap545,553 moves/views Kasse2. Original ordinary damage eliminates all five remaining Mantis ships, then EyeStalk/Collector; right-click visible Cocoon620,200 attacks the remaining platforms. Genuine final-two-platforms-attack-ui-clang.mission records Niad302/Cocoon1211 and all six original ship hull/supply/attack targets. Normal F8 restores this checkpoint; **no new movement/attack order** is issued. The visible attack continues, Cocoon explodes, Niad finishes removal, original STMG38 speech executes, objective21 becomes completed and the original “Mission Successful / Press Ok to continue” dialog is visibly inspected. Source KillTheMantis/Success/Done/EndMissionVictory(0) reaches the real mission success modal, not timeout/quit, draw-count or synthetic victory. Actual OK/end-statistics/regular return and independent full fresh GCC/Clang acceptance are still being executed. RenderBatcher material-eviction traces appear during this combat; no renderer/retail parity is inferred. Captures/raw logs/genuine saves stay outside the repository.


Actual normal end return: optional Clang Mission Successful OK404,447 displays original Menu_EndGame “You Win!”, Ships415/Platforms475/Resources4906/Total5096 and Total Game Time2:43:28 from this genuine accumulated checkpoint. Player-name row is blank; no player-name/retail statistics parity is claimed. Source modal returns2 and Mission closes. Normal Continue639,472 visibly returns to the original Single Player/Celareon Training selector without crash. Hidden EndGame IListbox is explicitly source SetVisible(false); displayed score/name rows are original static controls, no replacement list provider. This executes the previously compile-only success/end owner closure. Fresh repeat entry/normal close, GCC end and full independent every-step GCC/Clang remain required.


### 2026-10-12 — real close-after-winning-return modal ownership failure

The successful visible Clang victory/OK/Continue return above is **not** a
clean process-lifecycle pass: subsequent ordinary compositor window close
exits139. Matching coredump PID1474595, current binary before rebuild, confirms
SIGSEGV at invalid virtual address0x1000 from DoMenu_campaign delete at438,
called by NativeShell after the end return. Worker threads wait; host has
48Gi available/no swap use, no memory-exhaustion evidence. The temporary
fresh extracted core is deleted; diagnostic logs remain outside the repo.
Source original Frame WM_CLOSE posts CQE_DELETE_HOTRECT for top-level frames;
FullScreen executes the posted Release while native CQDoModal returns0 on
Close, then the original DoMenu_campaign caller deletes its owned dialog.

Local hypothesis/smallest countercheck: native frames in the original modal
focus chain retain destruction ownership in the CQDoModal caller; ordinary
nonmodal close keeps existing posted destruction. Extend existing
CheckNativeHotRect/native_shell with original DAComponent<Frame> Close and
posted-message processing, observing modal deletion requests0 and nonmodal1.
This is an isolated synthetic event-handler countercheck, no gameplay proof.
Its parent intercepts deletion solely to observe requests; no production fake
provider is introduced. Initial synthetic setup wrongly hooks unavailable
STATUS/CURSOR/HINTBOX resources; core diagnosis identifies null resource owner.
Correct isolated construction avoids that unrelated hookup. Corrected
modal-close-counter-check3 fails with return6/source lifecycle rejection under
old Frame behavior; modal-close-final build and app_schema/native_shell pass
after the native-only modal focus-chain ownership guard. Frame fields/public
interfaces/Windows behavior/resources, persistent layouts, simulation/RNG and
command ordering remain unchanged. Actual visible selector/briefing/end-close,
affected builds and instrumented repetition are being executed; Windows
Debug/Final/module ABI/retail parity/full training remain open.


Actual focused optional Clang close repeats: original selector captured/normal compositor close exits0 (`modal-close-selector-clang`); repeated original selector → Solar click400,333 visibly opens Briefing, but normal close in that nested modal remains hung (`modal-close-briefing-clang`), not exit0. The earlier exit0 wording was premature and is corrected; the process is aborted only for a diagnostic stack, not successful normal exit. modal-close-current affected builds/checks pass (BUILD_GRAPH). End-return close and integrated sanitizer scope are repeated separately.


Nested-close follow-up: the first guarded Frame briefing close is actually
hung, not exit0; it was forcibly SIGABRT-stopped for diagnosis only. Matching
PID1486359 stack shows outer DoMenu_campaign/CQDoModal/EndModal → SystemContainer
Update → NativeWindowManager ServeMessageQueue flags0 → SDL WaitEvent/ppoll
following the consumed inner-dialog Close. Read-only attach is host-denied;
fresh extracted diagnostic core is deleted. Local hypothesis/countercheck:
accepted native Close must set results for the existing entire ModalEventCallback
stack: dialog0, source mission MISSION_END_QUIT4. Extend existing native_shell/
CheckNativeHotRect with actual DAComponent<ModalEventCallback> mission/selector/
briefing objects, no scene or alternative UI. Corrected build uses source
DAComponent wrapper; nested-close-before-check rejects old top-only Close
(return6). Native-only stack unwind passes existing app_schema/native_shell in
nested-close-after-check. No fields/interfaces/Windows code, formats, simulation
or RNG change. Actual nested briefing close/all builds/instrumented end repeat
are under execution.

Separate visible optional GCC `combat-end-gcc`: normal original selector/Start,
P/Escape/F8 loads unchanged final-two-platforms-attack-ui-clang.mission;
without new gameplay orders, original attacks remove remaining units, STMG38
runs, objective21 completes, inspected Mission Successful/OK/You Win/Continue
return to selector executes. Normal compositor close now exits0 with Frame
ownership guard. Observed score display differs from the earlier Clang
continuation; blank player name/statistics persistence/cross-compiler and
retail comparison are unproven. This is a distinct later-state end continuation,
not a fresh complete GCC training pass.


Actual corrected nested close: optional Clang original selector → Solar400,333, inspected Briefing, ordinary compositor close now exits0 in nested-close-briefing-clang; previous hung path is reproduced and removed, not skipped. nested-close-current affected builds/checks pass (BUILD_GRAPH). End-lifecycle-asan is prepared with the unchanged genuine final-two-platforms F7 and original campaign entry; actual execution follows.


Actual campaign-entry ASan/UBSan counterexample: end-lifecycle-asan normal
Solar/briefing entry reaches original SolTrain_Briefing ELAN stage; UBSan
halts with invalid bool value122 in MenuBriefing::PlayAnimatedMessage line243.
Original CQBRIEFINGITEM constructor sets only size; actual Sol_T caller sets
file/type/slot/highlight but omits bContinueAnimating. Before-change regular
native entry thus exits1, no end/lifecycle acceptance. Local hypothesis/smallest
repeat: initialize this presentation keep-image flag to false at the original
native Sol_T talking-head call, then repeat ordinary selector/briefing/Start.
No constructor/layout/fields, original Windows code, persistent script state,
objective, timing, stream ordering or RNG changes; existing shared header
asserts CQBRIEFINGITEM80. Instrumented compile/checks and actual repetition
are under execution; full mission/leak/media/Windows/ABI/retail remain open.


briefing-bool-current affected builds/checks pass (BUILD_GRAPH). Actual regular end-lifecycle-asan selector400,333/briefing Start350,543 now passes the previously failing ELAN talking-head update and displays initial objective1. Normal P/Escape/F8 loads unchanged final-two-platforms attack; original attacks continue without reissue and visibly reach Mission Successful/21 completed. Speech/voice/alerts are explicitly disabled in this instrumented repeat (STMG38 records exclusion), optional decoder remains ON for Sector comm, leaks OFF/external libraries uninstrumented. Actual end controls/return/normal close are being executed, not yet lifecycle accepted.

2026-10-12 native zero-distance adapter countercheck: the existing instrumented
math_core check fails before the zero-input repair and passes afterward with
ASan/UBSan and leaks ON. Only MathCore::legacy_sqrt returns its signed zero
input directly; all nonzero approximation operations remain unchanged. Original
ObjectControl/doMove/path tolerances and script/simulation/RNG are unmodified.
No field, disk layout, module interface or Windows x86Math expression changes.
All five affected Debug builds/non-GPU suites pass (`sqrt-zero-current`).
Actual unchanged pending Jumpgate F8 is being executed;
this synthetic pass alone is not M2 acceptance or Windows math parity.

Actual `run-sqrt-zero-asan.sh` optional Clang ASan/UBSan, regular selector/
Brief Start/F8 of the unchanged genuine pending Jumpgate save, without a new
build/move/stop command: Forger reaches FAB_BUILDING/buildee59633 and then idle;
normal F7 records two ready player1 JumpPlat800 (59633/Solar,
268495089/Kasse), objective14 complete59 and15 pending60. The visible original
platform and “Use wormhole to jump into Kasse.” teaching text are inspected.
This restores the previously stalled exact-endpoint path in this bounded state.
Leaks OFF; speech/voice/alerts and documented media excluded; external
SDL/FreeType/shared decoder/GPU uninstrumented. Normal close, separate latest
GCC/Clang full-audio repeats and independent full training remain required.

Separate latest optional GCC `run-full-gcc-sqrt-zero-continuation.sh` restores
the same genuine pending order through regular selector/Brief Start/F8 with
training speech/unit voices/alerts enabled. No build/move/stop reissue:
FAB_BUILDING then two ready800-hull player1 gates,14 complete59/15 pending60
at normal F7. The finished gate, Forger HUD and original STMG25 subtitle are
visually inspected. The previous paused GCC process normally closed0 before
restart. This confirms this saved-state repair in GCC and instrumented Clang;
independent latest every-step GCC/Clang and normal Kasse/end remain open.

Normal GCC fleet lasso275,135→515,285 selects six Polaris and Taos. Insert,
wheel−20/+15 produces the actual zoomed map; original right378,214 (green
Solar4) and left camera/Insert reach the Eutromil area. Normal wait40/F7 then
20/F7 records all six185-hull Polaris resupplied130 and Taos80-hull/75, with
actual fleet/Xeno/Eutromil frame inspected. This is original area supply,
not a resource setter or a skipped gameplay path. Normal map return toward
Solar wormhole248,343 follows; Kasse15/end remain pending.

Normal GCC gate hover/right414,306 after resupply issues the original group
jump. F7 records all selected ships in Kasse/system1,14 and15 complete59/60,
20 pending65. Visually inspected Kasse arrival shows original Polaris, hostile
ships/weapon action, Jump Gate Under Attack/Enemy Sighted and STMG34 Matrix
subtitles. This is the same genuine fresh chain, not a seeded final state.
The first Control_L/1/up group assignment follows an off-map Kasse click
that cleared selection; subsequent1 therefore selects no group. It is not
successful group evidence. Forger F/
slash returns Solar and normal minimap539,498 reveals Solar3 production HUD.
Further combat, Matrix20 and script victory/return are still pending.

GCC normal Solar3 Greater Pavilion470,220 exposes its original HUD; T four
times and M twice queue four Triremes/two Monoliths. Queue/model frame inspected,
normal F7 records first actual Trireme60193 and later normal production reaches
three (two ready600-hull/350-supply, third in progress). No build objective is
invented for narrated4and2. After HUD commands, moving the mouse back onto the
field restores original ObjectComm focus; slash then correctly recentres Taos
in Kasse. The earlier slash while the pointer owned a HUD hint did not recenter,
and is not treated as a successful camera action. Actual Kasse lasso followed
by real Control_L/1/up is being checked, no input-owner patch inferred.

GCC original Forger F, mouse onto field/slash, Solar2 minimap523,450,
Defence222,487/Matrix120,515, free385,305 accepts work8234/building33620.
Normal F7 records moving Forger49164.2,76686.8, goal14.25,26.25, actual
Matrix build destination and four Triremes (three ready600/350, fourth494 in
production). The whole real later checkpoint is preserved externally as
full-gcc-matrix-moving-capital-queue.mission. Normal F8 without reissuing either
building/production queue is now being executed; no completion evidence inferred
from mere acceptance or frame counts.

Actual later GCC F8 retains the moving Matrix order (work8234), continued
Forger motion, completed four-Trireme production and the remaining Monolith
queue. After moving the pointer onto the field, normal1/slash restores the
seven-member group in Kasse; original group1 labels and Group HUD are visible.
Thus original grouping survives this normal reload. Earlier empty selection
assignment was an input mistake, not patched with a replacement owner. This
late state still must continue to original victory/return.

Later normal GCC F7 after the unchanged moving Matrix/queue F8 records Forger
idle56320,107520, completed20/65 and pending21/66. All four Triremes now600/
350, first Monolith60417 ready800/500 and second60561 in progress250. Original
normal Kasse exploration uses Down5s then field-right400,300; combat/scouting
continues without source setters. Visible finished Matrix and ready-state
F7/F8 are now being checked; this completed objective alone is not visual
acceptance or victory.

GCC completed Matrix F7/F8 visibly restores the original full Talorian shield
ring around Solar2, platform model and normal alerts. Normal1/field/slash then
reveals the advanced Kasse battle and original Eye Stalk platform; the earlier
six-Polaris force suffers actual combat losses, leaving the displayed group
with two damaged survivors. This is a gameplay outcome, not a successful final
assault or a frozen-state counter pass. Regular withdrawal and the normally
produced capital reinforcements are necessary; no balance, objectives, supply
or hull values are changed. Original victory remains pending.

Normal current GCC later F7 confirms both Monoliths60417/60561 ready800/500,
all four Triremes60193/60241/60289/60337 ready600/350 after the unchanged queue
F8. Original initial Polaris/Taos force has now been lost in actual Kasse
combat; no save byte/hull repair or automatic objective setter is used. Forger,
Acropolis, own platforms and completed Matrix remain present, source21 pending.
Solar3 frame visibly shows all six capital models; normal lasso/Attack stance
and minimap actions prepare the remaining Solar hostile cleanup and Kasse
reinforcement. Actual final victory is still pending.

The first capital lasso/stance/navigation attempt visibly clears selection and
moves no ship; it is not counted as reinforcement. Countercheck: Solar3
minimap539,498 then explicit normal field hover600,180/wait1 before lasso
300,60→720,405 visibly selects exactly four Triremes/two Monoliths. A separate
normal316,554 click displays original Attack Stance hint, event183 and actual
F7 stance1 on all six. This disproves a missing stance owner; no speculative
UI patch is made. Real Control_L/1/up assigns the selected capital group,
then ordinary Prime navigation/combat is repeated.

Capital navigation countercheck: original caps.moveOk/jumpOk remain1 on all
six after normal reload. Prime minimap right499,487 records no move agent; an
initial field-right600,180 hits the friendly ESP Coil and is not a ground move.
Separate empty field-right650,350 succeeds: genuine F7 records shared original
moveAgent10057/moveActive1 and changed positions on all six. Original Monolith
weapon damage to a remaining Solar Frigate executes. No capability/schema or
movement patch follows these rejected/targeted clicks; final Solar cleanup and
Kasse assault are being continued through ordinary input.

Normal capital empty-field move/automatic attacks remove all three remaining
Solar M_Frigate records (58514/58530/58546). F7 retains all six capital ships
at full hull, consumed supplies (Monolith458/479; Trireme335–350), target0 and
positions around55296,71680. This clears actual Solar hostiles for the eventual
all-player2-units victory condition, not only the Kasse platforms. Solar gate
camera and regular jump input are the next actions.

Normal Solar gate hover/right460,405 gives shared original jumpAgent10206 to
all six capital ships; later ordinary F7 records all six in Kasse/system1,
full hull with consumed supplies. Field/slash visually centres them at the
own Kasse gate with original group1 markers. Normal Down2s and field-right
400,300 continue toward Kasse2; original Niad/enemy ships, fighters and Under
Attack messages are visible. Actual later F7 shows damage on all six (Monolith
782/452, Trireme594/120/504/537) and23 hostile records still present. This is
actual ongoing combat, not victory; normal retreat/resupply/reinforcement is
being attempted without altering balance, hulls or mission state.

2026-10-12 current visible GCC continuation: subsequent genuine F7 has twelve
remaining hostile records (nine ships and Cocoon/Collector/EyeStalk); Niad and
other enemy ships have actually been destroyed. The last first-wave Trireme
60241 also disappears in subsequent normal combat. This is not victory.
Normal Greater Pavilion hover visibly shows original Trireme cost250 ore/600
gas/80 crew and Monolith600/1000/300 through the real hint/resource owner.
Normal T four times queues four additional Triremes; visible queue and genuine
F7 show first in-progress ship60689, not four completed reinforcements.
Source review: Sector::setSystemInSupply propagates supply through ready owned
JumpPlat siblings; RepairPlat's original area resupply additionally requires
SystemInSupply and proximity within its data-defined supplyRange. Thus system
supply alone does not replenish ships in distant combat. Next ordinary action
is Forger movement/jump toward Kasse for a local Eutromil and further actual
combat. No resource, supply, hull, script or goal values are edited.

Source training narration countercheck: original IDS_SOL_SUB_STMG27 explicitly
recommends another Eutromill on Kasse Prime to complete the supply process,
while STMG23 explains area repair/supply and optional mobile supply ships.
Local Kasse Eutromil is therefore an original recommended action, not a new
script objective/count requirement. Its actual construction/resupply and
remaining combat are still unexecuted here.

Current latest GCC regular Kasse resupply preparation: Forger normal Solar
minimap right490,568 creates moveAgent12013; after arrival, real Solar gate
hover/right435,360 jumps him into Kasse. Prime minimap537,500 and original
Basic/Eutromil165,515 show expected resource hint500 ore/300 gas. Green
planet-slot hover430,370 followed by ordinary left-click creates work12509,
building36304. Normal F7/F8 without reissue progresses from actual moving
order to FAB_BUILDING with buildee60881 and visible Eutromil construction on
Kasse Prime. Four additional Triremes60689/60737/60785/60833 all finish
600 hull/350 supplies in Solar. No automatic resupply or final victory is
credited yet; next: complete Eutromil and regular fleet reinforcement.

2026-10-12 next required Eutromil dependency, before edits: actual normal
Kasse Eutromil loading reports absent BBUILDER!!S_RepairBot. Read-only external
gametypes.db observation identifies exact192-byte BT_BUILDERSHIP_DATA,
OC_SPACESHIP1/SSC_BUILDSHIP2, matching original BuildShipFactory admission.
Hypothesis: linking the original BuildShip owner into the existing platform
support closure supplies RepairPlat::initRepairPlat's real IShuttle dependency.
Cheapest disproof: compile that closure, then regular selector/briefing/F8 of
the genuine ready Eutromil state; inspect actual factory/instance creation,
visible drone and normal release. No direct CreateInstance/debug setter is
accepted as gameplay proof. Persistent data and interfaces must remain intact;
Windows build/runtime/ABI remains unavailable.

RepairBot closure countercheck: original BuildShip compiles under Clang, but
full linking rejects undeclared-definition IBuildShip::PhysicalUpdate/Render
(native _NO_VTABLE is empty); GCC also rejects two explicit constructor calls.
Native pure declarations preserve these vtable slots and require the real
BuildShip overrides, avoiding successful no-op implementations. Native value
construction preserves the two original transform arguments. Windows original
declarations/expressions remain conditional and unchanged. No persisted fields
or callback signatures/order change. Full rebuild and runtime remain pending.

RepairBot validation correction: first new schema compilation rejects
nonstandard-layout offsetof under the existing probe's warnings-as-errors.
The ensuing focused CTest inadvertently ran the preceding probe executable;
its three passes do not validate the new drone assertions. Offset checks are
retained using actual typed field addresses (type8, fileName12, final192),
with static192-byte extent and schema field correlation unchanged. Fresh
focused compilation/checks and affected builds must pass before acceptance.

RepairBot fresh corrected validation: optional Clang rebuilds the extended
view_parser_probe before existing app_schema/view_parser/native_shell CTests;
all pass. `repairbot-typed-layout-current` rebuilds default/optional GCC/Clang
and optional Clang ASan/UBSan, then existing non-GPU suites pass. Separately
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
ctest --test-dir /tmp/frontierx-voice-asan -R
'^portability\.(app_schema|view_parser)$' --output-on-failure passes. These
are schema/synthetic checks, not actual Eutromil gameplay acceptance. External
run-repairbot-asan.sh uses the previously documented optional instrumented
regular selector/briefing CLI with unique save-repairbot-asan containing a
whole unchanged genuine ready Kasse Eutromil F7 copy; ordinary F8/render/close
is underway. Leaks OFF, speech/voice/alerts and documented media excluded;
external SDL/FreeType/shared FFmpeg/GPU remain uninstrumented. No Windows
Debug/Final/module ABI execution is available; no interface slots/order or
shared/persistent fields are changed.

RepairBot actual bounded optional ASan/UBSan runtime: normal selector400,333/
briefing350,543, F8 of unchanged genuine later Kasse Eutromil/Matrix save,
F/slash, field selection, F7 and second ordinary F8 execute. Visible Prime
Eutromil/four ready Triremes render; exact individual drone appearance/retail
image parity is not established by this frame. New genuine F7 contains both
original RepairPlat repairTransform positions at their platform locations
(59489 Solar106496,83516;60881 Kasse57344,50748), written only by the original
save branch with a real repairShip. S_RepairBot missing-provider message is
absent; excluded Terran CraneBot/Mantis construction archetypes still report
missing providers and are not accepted production capabilities. Normal F7/P,
foreground other paused GCC window and compositor close return original modal4
and exit0, no ASan/UBSan error. Leaks OFF/media exclusions and external
uninstrumented components as above. Actual damaged-ship repair/work animation
remains unexecuted; full M2/mission leak/Windows/ABI/retail gates remain open.
Previous GCC continuation is normally closed before restarting latest linked
optional GCC on the same genuine chain.

2026-10-12 latest full-audio GCC repairbot continuation regular selector/
briefing/F8 resumes the same genuine20-complete/21-pending chain, renders
Kasse Prime Eutromil/four Triremes and normal F7. Initial two-lasso/Control1
regrouping loses selection and is not accepted; user subsequently confirms
accidental concurrent input. No speculative UI patch follows. Agent input is
held for coordination while source review continues. Original ship repair
path: ObjectComm mode detection recognizes allied ready repair/supply platform,
normal right-click reaches handleShipRepair/USRSHIPREPAIR through localSendPacket
and existing production command owners. Actual damaged-ship repair remains
pending. Native cl/MSBuild/dumpbin/pwsh are absent when rechecked; Windows
Debug/Final/rebuild/ABI are explicitly not executed.

2026-10-12 first actual Stratum production blocker, before edits: additional
physical keyboard input selects Acropolis and S queues SUPSHIP!!S_Stratum.
Optional GCC dies with SIGSEGV/exit139 at08:02:31. Read-only full-thread core
review confirms ObjectList::AddObject(obj=null) from BuildSupPlat::updateBuild
after failed original archetype/instance creation; other threads wait in
SDL/PipeWire/Nvidia, no resource exhaustion/OOM indication. Extracted core uses
a unique temporary file and is automatically deleted; raw core/logs stay outside
the repository. Whole genuine preceding F7 remains available. External actual
gametypes entry is652-byte BT_SUPPLYSHIP_DATA, OC_SPACESHIP1/SSC_SUPPLYSHIP10,
matching existing original SupplyShipFactory. Hypothesis: integrate that
original leaf into the existing spaceship owner; the same regular Acropolis S
production must create a real supplied ship, render, and preserve its original
state through ordinary F7/F8/close. Cheapest disproof is focused closure build
then this exact normal saved-mission input under ASan/UBSan. No AddObject-null
skip or successful placeholder is used to claim production. SupplyShip's real
mobile resupply/escort and complete combat remain subsequent runtime gates.

Stratum persistent-layout countercheck: first existing app_schema extension
fails on schema464/native468, not an ignored failure. Source review identifies
original empty final SUP_SAVELOAD sup_SL: original SpaceShip::Save zeroes and
writes sizeof(SaveStruct), while Load calls Mission::CorrelateSymbol by named
record. ViewCnst explicitly accepts empty records with no datatype fields. All
actual state ends at native offset464 (baseSaveLoad424/extent40); sup_SL starts
464, native empty member size1 contributes tail padding to468, parsed empty
record size0 contributes no fields. Existing check now asserts both distinct
original extents, parsed empty0/native empty1 and field positions, then real
mode/working/target/escort/platform/stance/timer/flag correlation. No original
header, serializer, schema or format is changed. Native Windows compiler/retail
file comparison remains unperformed; ordinary actual saved Stratum extent and
F8 preserved state are the next runtime counterchecks.

Stratum corrected extent check: freshly built optional Clang view_parser,
app_schema and native_shell pass. All-five stratum-current first pass finds
GCC rejecting the original in-class SupplyShip::ResolveAssociations declaration;
Clang/default/optional/instrumented builds and non-GPU suites pass. Local
hypothesis: remove only redundant declaration qualification, leaving signature,
slots, fields and behavior intact; GCC leaf rebuild is the cheapest disproof.
That syntax-only correction is under all-five stratum-qualified-current checks.
The first actual instrumented normal-production run uses the already linked
Clang implementation before this declaration-only spelling correction.

Stratum affected-build result: stratum-qualified-current passes all five
Linux Debug builds/non-GPU CTest suites. Separately executed ASAN_OPTIONS=
detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 ctest --test-dir
/tmp/frontierx-voice-asan -R '^portability\.(app_schema|view_parser)$'
--output-on-failure passes both freshly built deterministic checks. This
proves compilation/schema correlation, not actual ship production or victory.

2026-10-12 actual bounded Stratum ASan/UBSan run: external wrapper
run-stratum-asan.sh uses unchanged whole genuine later F7 copy as save root.
Normal selector400,333/briefing350,543/F8, field pointer/B/field pointer/slash/S
queues original Acropolis production (hotkey30). First ordinary F7 contains
SUPSHIP!!S_Stratum ID61025, native468 bytes, Solar/player1, hull246/supply0
during construction. Ordinary F8 without reissue finishes hull350/supply2000;
ship is visibly rendered and normal click569,321 shows original Stratum HUD,
350/350 and2000/2000 plus original supply controls/range. Second ready-state F8
and normal selection reproduce that visible HUD and same ship identity/state.
F7/P/background compositor close returns original modal4 and exit0; no
ASan/UBSan error. Leaks OFF, training speech/unit voice/alerts and documented
media exclusions; external SDL/FreeType/shared FFmpeg/GPU not instrumented.
This run used the successfully linked Clang implementation before the later
GCC-only redundant declaration spelling correction. It proves construction,
normal selection, original render, in-progress and ready-state reload/close;
actual mobile transfer/escort/combat and complete latest GCC/Clang training
remain unexecuted, mission LSan remains failed. No retail file/log/capture is
added. Latest full-audio GCC is separately entering the same genuine chain
and repeating regular Stratum production before reinforcements.

2026-10-12 separate latest full-audio GCC Stratum continuation: normal
selector/briefing/F8 resumes the genuine preceding save; field/B/field/slash/S
queues Stratum and F7 records468-byte ID61025 at hull244/supply0. Further normal
play finishes350/2000. F/field/slash/Kasse Prime minimap537,500/basic148,487/
Oxidator75,515/green495,270 creates real work13551, then real refinery61073
finishes while original gas increases. Subsequent attempted T keys after a
minimap click still in Kasse have no Greater selection/production and are not
accepted queue evidence; full map Insert identifies Solar and is used to
return normally before actual ship production. Original gameplay ion-nebula
hint also visibly renders through normal hover in this run. No resource or
mission state is patched. Independent latest complete training remains open.

Latest genuine reinforcement continuation: original Trireme hover identifies
ore250/gas600/crew80/CP3. Eight T requests at the selected Greater accept only
two with actual insufficient-gas status for the rest, not eight completed ships.
Kasse Oxidator61073 has1100 hull and original resource gain executes. Normal
Solar4 hover shows original10000/10000 gas,0/min before any refinery. Green
three-slot hover535,350 with Forger still in Kasse produces a rejected direct
remote build (no highlighted target/SFO3061/no work state), not a successful
cross-system construction. Normal F/field/Kasse gate minimap533,465 then
right-click actual visible gate450,300 is used to return the Forger through
the original jump command before retrying local Solar4 construction. No
speculative remote-build UI replacement or ship relocation is added.

Solar4 placement follow-up falsifies a complete remote-visibility explanation:
the Forger returns through original jump agent14155 to Solar and moves normally
with agent14221 to111612,87038.7. Initial local535,350 still rejects; a subsequent
normal hover535,335/button-only click creates original work14643 (building
35688) and moves the Forger to110843,84851.6. No code change follows these input
counterchecks. A real F7 of this pending build plus original five-Trireme queue
is being normally F8-loaded without reissuing build/production. Successful
work state, not green color alone, determines accepted construction evidence.

Latest GCC genuine pending-build/queue F8 countercheck: ordinary F8 without
reissue advances real Forger work14643 from moving to original FAB_BUILDING8,
creates actual Oxidator61457 on Solar4 and visibly renders its model. Original
ready Triremes61185/61233/61281/61329 retain600/350; fifth61377 continues
construction499/0. Stratum61025 remains real350/2000. Exact original resource
cost/insufficient-gas rejection and production queue execute; sixth/later
reinforcements are still being built. This is continued normal gameplay on
external data, not an independent fresh latest full GCC training acceptance.

Original mobile supply escort now actually executes under full-audio GCC:
normal B/field/slash selects Acropolis; normal click569,305 selects Stratum,
Defend control315,525, Solar3 minimap539,498, hover640,260 and button-only
left-click target real Trireme61233. Original status explicitly says left-click
to escort/defend; actual F7 records native468-byte Stratum61025, mode1
SUP_RESUPPLY_ESCORT, working15423, escort61233, stance1, supplies2000; position
59834.3,75782.6 shows movement from Prime toward the selected escort target.
Original eighth Trireme61665 is constructing588/0 while the other seven are
600/350. Subsequent ordinary active-escort F8 is under verification; actual
ammo transfer and complete training are still not established by this state.

Latest normal active supply-escort F8 preserves actual mode1/working15423/
escort61233, advances Stratum position to79217,75777.7 near its target without
reissue, and retains350/2000. Eight original new Triremes61185/61233/61281/
61329/61377/61569/61617/61665 are now all600/350. Normal Solar3 field lasso
selects a visible nine-unit group including the supply ship; no count/script
state is assigned artificially. Original combat/transfer/end remain pending.

Latest GCC normal group command: visible nine-unit lasso group includes the
eight ready Triremes and Stratum. Original Attack stance control315,554 sets
all eight actual saved gunboat stances1; supply escort remains mode1. Real
Control_L+1 dispatches original group84, then hover/right-click visible own
Solar gate431,312 reaches highlighted target and original jump voice. Jump
and subsequent normal F7/F8 progression are being checked before battle.

Latest ordinary group-jump F8 without reissue completes all eight Trireme
jumps and Stratum into Kasse/system1. Real F7 confirms all eight identities
and350/2000 supply ship at51262.3,75914.2. Original manual jump cancels its
previous supply-escort operation (mode0/working0/escort0), preserving stance1;
this is original order replacement, not stale ownership. Normal field1/slash
restores visible nine-unit group and original group1 markers. Whole genuine
prebattle F7 is retained externally as full-gcc-eight-triremes-kasse-prebattle.
Initial normal minimap right547,523 creates move15923 while some warp-out
ships have not yet adopted movement; a subsequent ordinary move follows after
arrival, not used as a replacement for the already completed no-reissue jump
reload evidence. Actual final combat/transfer/end remain pending.

Latest GCC actual first final-battle progress: common original minimap move
15980 takes all eight ready Triremes toward goal15.5,9.5; original Attack
automatically destroys all three remaining Scarabs59874/59890/60178 and
Collector57394. All eight remain alive,61329 hull462 and others600. Actual
Stratum supplies2000→1995 and original passive supply-range loop is active;
next F7 has all clients350 and Stratum2000 again in the allied supply system.
This does not isolate mobile transfer from the Eutromil's concurrent supply
network. Three Tiamats950 each, Cocoon2730 and EyeStalk1 still remain; original
21/end not reached. GCC is normally paused while a separate unchanged whole
combat F7 copy enters regular selector/briefing/F8 under optional ASan/UBSan
with leaks OFF and documented speech/voice/alert/media exclusions.

2026-10-12 actual Stratum-combat ASan/UBSan countercheck fails, exit1:
regular selector/briefing/F8 loads the unchanged genuine GCC combat save,
field1/minimap right522,547 advances the original group. Visible Tiamat and
Trireme missiles/damage execute. UBSan halts at AnimNugget::Update line365,
left shift of negative -4, through NuggetManager::Update/ObjectList::Update/
EndModal/CQDoModal. This is not a successful sanitizer combat result. Source
review: original saved shadowVisibilityFlags is U8; promotion makes its
complement signed int before shifting. Original watcher consumer accepts U32
and tests special/player bits. Hypothesis before edit: native U32 complement/
shift preserves exact original32-bit player/special mask, callbacks and order
while removing undefined signed shift. Only native expression changes; Windows
branch, U8 layout/save and RNG/tick/gameplay remain untouched. Cheapest actual
countercheck is repeat regular F8 of the same whole combat save and same normal
move, not disabled UBSan or removed watchers. Focused instrumented shell leaf
is under rebuild; full M2/mission leaks/Windows remain open.

Nugget mask corrected countercheck: focused optional ASan shell leaf builds;
app_schema/view_parser pass with ASAN_OPTIONS=detect_leaks=1 and UBSAN_OPTIONS=
halt_on_error=1:print_stacktrace=1. nugget-watch-mask-current rebuilds all five
affected Linux Debug default/optional/instrumented graphs and existing non-GPU
CTest suites pass. New bounded run-nugget-mask-combat-asan.sh uses a fresh
whole copy of the same unchanged GCC seed, then identical regular selector/
briefing/F8/field1/minimap right522,547. It runs past the prior failure point
without disabled UBSan, renders real Trireme/Tiamat missiles and destroys
Tiamats60130/60114. Subsequent genuine F7 has all eight player ships alive,
Stratum350/1735, clients largely350, one320; last Tiamat60162 remains950.
Thus actual mobile supply drains beyond the earlier small concurrent Eutromil
refill observation, in the normal combat owner chain. Normal P/continuation
and close are still under verification; not a full training/sanitizer/leak
acceptance. Native Windows/ABI/retail remain unexecuted.

Nugget corrected replay remains a failed integrated sanitizer run: further
normal movement after P/Escape brings the last Tiamat60162 to389 hull and
removes EyeStalk57346, but UBSan subsequently halts at TObjExtent.h685 on
index-1 for RECT[10], through Explosion::InitExplosion/SpaceShip::explode/
OpAgent::processHitList. No sanitizer exclusion is added. Source review finds
extent slices selected from X or Y depending on dx>dy, while restored
Explosion::InitExplosion and update call GetExtentRect with boom_pos.z.
Before edits, hypothesis: explosion sampling coordinate lies outside the actual
selected extent-axis domain. Cheapest disproof is read-only child-GDB capture
of value/min/step/bZ and caller box/sample from the whole unchanged genuine
last-Tiamat combat F7; no debugger calls or state setters replace gameplay.
Native Windows/retail axis comparison is still unexecuted.

Read-only debugger repeat (no code change yet): regular selector/briefing/F8
loads whole genuine last-Tiamat combat seed. Original group1/minimap right
522,560 completes the remaining Tiamat; normal right550,563 then attacks
Cocoon. Eight Triremes survive; actual objective21 completes/STMG38 and
visible Mission Successful appears. Conditional extent breakpoint does not
fire in this repeat. This does not erase the prior integrated UBSan failure
or accept complete fresh training. Ordinary OK/return is under verification;
explosion-axis diagnosis still requires actual owner values.

2026-10-12 read-only child-GDB confirms explosion coordinate mismatch on an
actual destroyed Tiamat. At original InitExplosion lookup, boom=(1347.49,0,
-1380.16), bbox X±1395.94/Y±1081.65/Z±1513.53. Actual GetExtentRect has
min=-1395.94/step279.289/bZ1 (X slices). The particular captured sample is
valid slice0, but the unchanged Z draw has a larger domain and can produce
the prior negative index. Original Damage uses X/Y slice coordinates and
rect.top as Z. boom_pos is only a secondary-blast visual placement; Blast
update owns effects/light/media, not damage/commands. Before edit, hypothesis:
native explosion X/Y sampling and matching slice-axis lookup, with Z surface
height, removes this incompatible domain without extra/reordered RNG calls.
Native reflection must use the actual X/Y bounds: the legacy100-unit inner
margin can reflect a valid near-edge position outside the mesh. Windows path,
IExtent vtable, EXTENT_DATA, persistent records and other explosion physics
remain unchanged. Cheapest production countercheck: rebuild instrumented
shell, regular selector/briefing/F8 of unchanged three-Tiamat GCC combat save,
normal group/minimap combat to original21/end/return/close with UBSan enabled.
No synthetic direct explosion call is acceptance evidence.

Explosion axis focused optional instrumented shell build passes.
explosion-axis-current rebuilds default GCC/Clang, optional FreeType/shared
FFmpeg GCC/Clang and optional Clang address/undefined Debug trees; all
existing non-GPU CTest suites pass. No field, enum, parser/save extent or
IExtent slot changes; native Windows Debug/Final/ABI execution unavailable
and not replaced by Linux compilation. New run-explosion-axis-combat-asan.sh
uses whole unchanged paused GCC three-Tiamat save, regular selector/briefing
then normal F8/group1/minimap right522,547. Leaks OFF, speech/voice/alerts/media
exclusions and external SDL/FreeType/shared FFmpeg/GPU uninstrumented as above.
Visible real combat/end/return/close countercheck is in progress.

New explosion-axis regular ASan/UBSan combat countercheck, still in progress:
visible original missiles/first Tiamat explosion execute. Normal F7 at first
Tiamat851; P/Escape/F8 resumes without command reissue and destroys60130.
Further F7 has60114 hull620/60162 hull950 and Stratum supplies1860. A second
normal F8 with no new order reduces60114 from620→158; source Attack targets
and all eight own ships remain, Stratum supplies1805. This demonstrates actual
late damaged-combat reload/continuation and passive supply, not only record
parsing. Leaks OFF/media exclusions; other affected existing math_core,
app_schema/view_parser, state_material and game_material_manager CTests pass
with ASAN_OPTIONS=detect_leaks=1 and UBSAN_OPTIONS=halt_on_error=1. Final
combat/end/return/close and complete fresh GCC/Clang remain pending.

Corrected no-debugger instrumented combat advances past all three original
Tiamat explosions: normal group1/right522,560,25-second continuation/F7
leaves only Cocoon2730 among player2 platform/ship saves. Actual image shows
last ship debris/secondary blast and all eight live Triremes; Stratum350/1535.
No ASan/UBSan report so far. Normal move550,563 proceeds to final Cocoon;
original21/menus/normal close still pending for this corrected run.

Corrected combat run reaches original21/STMG38 after Cocoon combat, with no
ASan/UBSan report. However the externally bounded process expires during the
user interruption: exit124, not accepted normal close or visible end-menu
result. Last normal F7/image has Cocoon1785 and all eight player ships alive.
run-explosion-axis-end-asan.sh uses a whole copy of that genuine save with
a longer43200-second host bound; regular entry/F8 must resume the original
attack without reissue and verify visible victory/return/normal close.

Corrected end-asan repeat: regular selector/briefing/F8 of genuine Cocoon1785
checkpoint resumes fire without order reissue, destroys remaining Cocoon and
reaches original21/STMG38. Normal OK leads to visibly inspected You Win menu;
no ASan/UBSan report. Current Continue/regular same-process restart/close
countercheck remains in progress. Prior corrected combat exit124 remains
recorded separately and is not converted into normal exit.

Corrected end-asan repeat returns through visible You Win/Continue to the
original selector. Normal Start in the same process initializes original
objective1/initial Forger and both Taos, then normal F7/P/background compositor
close returns source modal4 and process exit0, without ASan/UBSan report
(leaks OFF/media exclusions). However its pre-pause restart image has no HUD/
minimap. Initial script reset is observed; restored restart UI is NOT accepted.
Next local owner check: original toolbar load respects bNoToolbar/bMovieMode,
whereas original System3D entry resets both flags. Native composition resets
bNoToolbar only at first process startup. Before any reentry edit, hypothesis:
end/briefing visibility flags survive into the next mission and hide original
HUD; cheapest disproof is read-only child-GDB values at both actual toolbar
loads, regular initial entry and normal victory/Continue/restart. No UI
replacement or debugger state setter is permitted.

2026-10-12 actual HUD-reentry owner diagnosis: read-only child-GDB toolbar
loads at regular first entry and combat F8 have noToolbar0/movieMode0; normal
You Win/Continue/Start restart has both signed bitflags-1 and bInvisible1.
This confirms the retained movie-hidden HUD hypothesis. Existing
CheckNativeHotRect now counterchecks frontend→ingame with both flags set and
requires the original System default reset; repeated same-mode call must
preserve a user's hidden state. No new probe/CLI/replacement UI. Instrumented
portability.native_shell CTest fails the old transition with source text
lifecycle failure. Before production edit: native ChangeInterfaceRes ingame
entry should perform original System3D's bNoToolbar/bMovieMode=0 reset before
notifying original consumers. This changes only native runtime entry flags,
not script, simulation/RNG, resources, persisted formats or signatures.
Cheapest counterchecks: same existing CTest, then identical real end/F8/
Continue/Start with visible restored HUD and normal selection/movement/close.
Native Windows execution remains unavailable/open.

HUD-reentry-current passes all five affected Linux Debug builds and existing
non-GPU suites. Corrected no-debugger regular entry/Cocoon1785 F8 reaches
original21/visible You Win/Continue; ordinary Start now visibly restores the
original HUD/top resource bar/minimap, unlike the prior unchanged image.
Normal field-F Forger selection and right624,300 movement complete1/2 and
list3, with visible Forger300/300 HUD, source instruction/teletype/status.
Normal F7 confirms original objective46/47 completed,48 pending/player1.
Early F8/continued normal command/close remain under verification. These are
actual external-data bounded owner checks, not complete fresh training.
ASan/UBSan enabled, leaks OFF/media exclusions/external libraries unchanged.

Corrected HUD-reentry-asan early F8 visibly retains HUD/minimap and completed
1/2/pending3; ordinary field-F/right640,280 moves again. F7/P and normal
background compositor close return source modal4/process exit0 with no
ASan/UBSan report. This bounds actual final combat/end/Continue/restart/early
reload/selection/movement/close, leaks OFF and documented media exclusions.
Old genuine GCC Stratum continuation closes normally0, retaining its own
unchanged three-Tiamat F7; that old binary still reports texture dangling
references on shutdown, recorded rather than claimed clean. Current optional
GCC run-full-gcc-explosion-axis-continuation.sh enters through regular selector
and briefing, same save-full-fresh-gcc root, with training speech/unit voices/
alerts enabled. It must finish its own genuine chain by normal combat/end;
ASan progress is not transplanted into this GCC root. Independent latest
fresh entry-to-end GCC/Clang and mission leak/Windows/ABI/retail remain open.

Latest optional GCC genuine-chain continuation: regular selector/briefing/F8
loads its own preserved three-Tiamat save, original group1/right522,547 moves
eight Triremes and Stratum. Real missiles/combat are visibly inspected; next
normal F7 has first Tiamat60130 hull290 (previous950), other two950, all eight
own ships alive, Stratum350/1950. Training speech/unit voices/alerts enabled;
no ASan state or external byte edit is used. Normal damaged-combat F8 and
right522,560/end continuation are in progress. This chain spans the documented
GCC integrations; it is not yet an independent latest single-build1→end run.

Existing optional instrumented portability.native_shell lifecycle also passes
with ASAN_OPTIONS=detect_leaks=1 and UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
after HUD entry correction. This is the registered synthetic/dummy-driver
check including original hot-rectangle mode transitions, not a mission leak
result. The earlier real mission LSan failure remains open. Latest genuine
GCC damaged-combat F8 continues normal fire; next F7 has only last Tiamat92
and Cocoon2730, Stratum350/1680 and all eight own ships alive.

Latest optional GCC genuine-chain final combat: ordinary move550,563 destroys
the remaining Cocoon after all Tiamats/EyeStalk/Collector/Scarabs are absent.
Original script21 completes and Mission Successful is visibly inspected on
physical HDMI output. Training speech/voices/alerts/subtitles are enabled;
external IAC decoder still emits coefficient cw_len warnings, not a clean
codec/retail-parity claim. Normal OK/You Win/Continue return is in progress.
The externally preserved late checkpoint full-gcc-genuine-final-cocoon is a
whole ordinary F7, not changed mission data. The genuine1→end chain spans
previous documented GCC binaries and normal reloads; independent latest
single-build GCC/Clang full repetitions remain required.

Latest genuine GCC chain normal OK shows You Win; Continue returns the
original selector and Back ends the process normally0. Thus its documented
regular-entry1→21 construction/reload/reinforcement/combat chain reaches actual
script success and return, across prior GCC integration binaries. Independent
run-m2-latest-fresh-gcc.sh now starts with an initially empty external
save-m2-latest-fresh-gcc root and the current optional GCC binary, training
speech/voice/alerts/font enabled; no earlier mission checkpoint is supplied.
Matching current Clang launch/empty root is prepared but not yet executed.
All original training steps, early/late save/reloads and end/return must be
repeated; neither old chain nor bounded sanitizer checks completes M2 alone.

Independent latest optional GCC fresh run: original selector/briefing/Start
with empty external save root; normal field-F/right624,300 completes1/2;
real lasso5,25→790,410 and ordinary speech completion complete3/list4. Visible
source UnitComm text, training subtitle, Forger300/300 and group HUD execute.
Real Control_L+1 stores the original control group. Normal P/F7 at update6806
and a second F7 after five seconds both record update6806 (pause freezes
simulation); normal Escape/F8 is under verification. Whole early checkpoint
m2-latest-gcc-early-1-3 is retained externally. No old mission save is used
for this fresh run; early file is created solely by its normal F7.

Latest fresh GCC early Escape/F8 visibly restores original HUD/minimap and
completed1/2/3, pending4, unit positions/ownership/control-group markers.
Normal left276,269 selects one Taos, minimap right539,498 scouts Solar3;
original4 completes/list5, with visible Taos80/80,75/75 supplies and original
STMG07 subtitle. Other Forger/Taos remain in Solar Prime area. No old save
is introduced; subsequent saves/reloads belong to this single current build.
Original Forger/basic-tab/Oxidator hint and placement are next.

Latest fresh GCC5: normal field-F/minimap left499,487/basic148,487, hover
75,515 shows original MultiHotButton Oxidator hint/resource text visibly.
Normal button-only click enters source build mode; hover460,250 shows green
three-slot placement around Solar Prime and original planet resource panel.
Button-only confirmation starts original construction. No direct SetText,
script trigger or goal setter is used. Build-ready/next Galiot are pending.

Latest fresh GCC original Oxidator becomes ready, completes5/lists6. Normal
platform click470,285/Galiot hover30,525/button-only production creates and
finishes original Galiot;6 completes/lists7 and source removes1..4 from the
current objective list. Observer listed0/completed0 for removed objectives is
that original removal, not failed earlier goals. All actions remain normal UI
in this current-binary fresh run; Bunker placement is next.

Latest fresh GCC7: original Forger/basic/Bunker hover30,552 and green Solar
Prime330,260/button-only confirmation construct Bunker; source7 completes
and8 is listed. This run's existing normal early F8 retains progression and
ready/in-progress production; no prior binary's mission file is imported.
Forger is normally moved via minimap539,498 to already scouted Solar3 for
required Sentinel/Pavilion construction.

Latest fresh GCC8: normal field-F/right539,498/left539,498 moves/views Forger
at Solar3; defence222,487/hover30,515 shows original Sentinel hint, green
530,310/button-only confirmation constructs it. Source8 completes/lists9.
No changes to code, scripted goals, balance or timing are made during this
independent current-binary run. Original Pavilion is next.

Latest fresh GCC9: original Basic/Pavilion hover120,515, green Solar3
380,280/button-only confirmation starts it. Normal in-progress F7/F8 restores
visible construction/HUD; it completes9/lists10 without a reissued build
command. This is distinct from early1..3 save/reload. Original Forger advanced
Helion hint30,515 is reachable;495,250 is not a valid slot in this camera view
and is not confirmed. A normally hovered free slot is being selected.

Latest fresh GCC10: original advanced Helion Veil uses valid green Solar3
430,335, confirmed by button-only click. Source10 completes; original
attacking Frigate/Mimic creation/jump and real Taos2 combat execute naturally.
No enemy state, supply, target or script is set through a diagnostic path.
Required ESP is next after source visibility unlock.

Latest fresh GCC11: original defence/ESP hover30,552 and normal free-space
click600,340 construct the ESP Coil; original11 completes/lists16. Real
Solar combat continues: saved Frigates target Acropolis, which remains2076
hull; one original Taos remains80/75 at Solar3, Forger remains intact. No
mission-fail state is bypassed. Normal Forger move/view499,487 returns to
Solar Prime for the original Citadel slot path.

Latest fresh GCC16: normal Forger/Prime view/basic Citadel75,552, visible
green335,335/button-only confirmation constructs it; after actual build
completion16 completes/lists18. Acropolis survives visible natural attack
status while Forger remains300/300. No elapsed-time shortcut or scripted
completion substitutes for ready state. Normal Solar3 return/Greater Pavilion
control/slot inspection is next.

Latest fresh GCC18: original Basic Greater Pavilion120,552/green Solar3
480,245/button-only confirmation construct it. In-progress normal F7/F8
continues without build reissue; later F7 has ready Greater2100, idle Forger
working0/buildee0. Source18 completes/lists17. Natural Enemy Sighted text
is visibly inspected during continuation; Acropolis still1668. Whole ready
state is this current-binary run, not an imported checkpoint.

Latest fresh GCC17: original advanced Annex30,552/green535,280/button-only
confirmation construct it at Solar3. Source17 completes/lists22. Hints,
normal slot input and source ready conditions continue in the same current
optional GCC process; original Anvil control/slot is next.

Latest fresh GCC22: original Anvil75,552/green Solar3 480,355/button-only
confirmation constructs it; original22 completes/lists23 and Forger returns
to idle working0/buildee0. Turbine Dock is next through its original advanced
control. All progression remains normal input/current-binary source logic.

Latest fresh GCC23: original advanced Dock120,552 is reached;440,355 is not
green in this run's occupied-slot layout and is not confirmed. Normal hover
390,330 shows a free green slot; button-only confirmation starts actual Dock
construction. The change is ordinary placement, not modified planet slots.
Ready23/next24 remain pending at this checkpoint.

Latest fresh GCC23 completes/lists24 after real Dock readiness. Normal
Forger/right581,476/left581,476 moves/views Solar4, revealing it through
regular gameplay for original Xeno Chamber construction. Source-required
construction sequence1..11,16,18,17,22,23 has now executed in this current
binary fresh run;24 onward and full completion are still pending.

Latest fresh GCC24: normal Forger Solar4 move/view581,476/advanced190,487/
Xeno hover75,515, visible green420,245/button-only confirmation construct
Xeno Chamber. Its source ready condition/next12 are still pending at the
first ordinary F7. No planet slots or training requirements are changed.

Latest fresh GCC24 reaches actual completion/lists12 after Xeno build; the
preceding F7 snapshot315 hull is an in-progress observation, not a ready
claim. Original source ready transition is subsequently observed. Required
six Polaris are next through normal Pavilion production; no units are spawned
or mission goals set externally.

Latest fresh GCC12 normal Pavilion selection340,260/hover75,525 visibly
identifies original Polaris control and cost125ore/300gas/30crew/2CP.
Hover30,525 identifies Taos and is not used as Polaris production. Six
ordinary button-only clicks on the confirmed Polaris control start the
required production queue; actual six-ready/supplied condition remains pending.

Latest fresh GCC12 production creates original Polaris records (first185
hull/130supplies; second initially3/0 during construction). Ordinary source
Construction Complete/Unit Built text is visibly inspected. While the six
production queue completes, normal Forger/Solar4/basic Oxidator75,515 and
green535,335/button-only confirmation build a second original refinery on
untouched Solar4 resources for later fleet supplies. This is a normal auxiliary
resource action, not an added training requirement or altered balance.

Latest fresh GCC12 completes/lists13 with six original Polaris59201/59249/
59297/59345/59393/59441 each185hull/130supplies in Solar. The auxiliary Solar4
Oxidator59489 finishes1100; Forger working0/buildee0 confirms ready/idle
before the next order. Original Eutromil control165,515/slot inspection is
next for13, with ready second-refinery resource owner preserved.

Latest fresh GCC13: original Basic Eutromil165,515/green Solar4 465,370/
button-only confirmation builds it. Normal in-progress F7/F8 repeats actual
ready13/list14 after load, without build reissue. Solar4 refinery150gas/min
remains visible/active and six Polaris have already satisfied12. Original
Forger/minimap490,568 approaches/views Solar gate for14; ready Jumpgate and
normal unit transit are still pending in this independent run.

Latest fresh GCC14 original gate approach completes normal movement;
Forger idle at33785.5,17397.3 near Solar gate. Defence222,487/Jump Plate
hover75,515/normal wormhole hover431,312 visibly names Wormhole to Kasse
and source build instruction. Button-only confirmation starts original
JumpPlat construction; pending-build F7/F8 and ready/transit are being checked.

Latest fresh GCC14 first build attempt is NOT accepted: normal hover names
the wormhole but the current original paired Mantis JumpPlats57026/268492482
are still player2 with800 hull. F7/F8 leaves Forger idle/working0 and no owned
Solarian JumpPlat;14 remains pending. This is the original hostile-gate
precondition already encountered in the older chain, not successful build
evidence. Next normal step: select the produced Polaris, attack hostile gate
through ordinary input, then retry actual Jump Plate construction. No owner/
HP/goal/state is changed externally.

Latest fresh GCC14 initial combat input correction: comma cycles idle Taos/
Polaris normally; slash is original IDH_CENTER_SCREEN/event7, not all-type
selection. It changes camera to Solar3. Consequently the attempted right
431,312 addresses empty space there, leaves all six Polaris targets0 and
hostile gates800; no attack success is claimed. Next ordinary Solar3 minimap
view/lasso groups the visible ships, then returns view to Solar gate before
a normal hostile-target right click. No code/input semantics are changed.

Latest fresh GCC14 normal field movement, not the earlier map/camera clicks,
creates agent5841 for five Polaris and Taos toward9.75,3.75. They arrive by
ordinary movement and original gunboat logic fires at hostile paired plates;
next genuine F7 has no player2 JumpPlat, visible plate geometry disappears,
Polaris supplies22/2/10/14/0 (the unselected sixth remains130 in Solar3).
No HP/owner/script is manipulated. Regular Forger/defence/Jump Plate hover
and wormhole click now retry actual construction on the cleared gate.

Latest fresh GCC14 actual retry produces owned Solarian paired JumpPlats
59745/268495201; ordinary F7 has both504 hull during construction and original
Forger working6312/buildee59745. Subsequent source14 completes/lists15.
Normal F8 of that pending-build F7 is being checked without build reissue;
15 transit has not been performed yet. Original Acropolis/Stratum and combat
fleet preparation remain ordinary UI/resource actions, not injected units.

Latest fresh GCC14 pending-build F8 resumes without a new construction order;
normal F7 confirms both owned JumpPlats800, visible completion text and source15
pending. Regular B/Acropolis S queues actual Stratum59921 (normal in-progress
F7 hull141); later HUD command points20/35 confirms finished production.
Normal Solar3 field click470,220 selects Greater Pavilion2100; original
MultiHotButton Trireme hint visibly reports250 ore/600 gas/80 crew/3CP and
original production dependencies. Five ordinary button presses queue
reinforcements within the original resource/command limits. No units, goals
or resource values are injected. The fresh run has not yet completed15.

Latest fresh GCC15: individual field click640,250 misses (selected=no);
small ordinary lasso620,235..670,270 selects Taos plus one Polaris. Normal
right-click431,312 on the owned wormhole sends both through original warp;
source15 completes and visible STMG29 then STMG34 unlock Matrix20. Slash
centers on the actual group in Kasse. No warp/goal state is modified.
Regular Forger selection and Solar2 minimap movement precede original
Matrix control120,515, whose visible hint reports1500 ore/1500 gas, two
planet slots and original Xeno/Annex dependencies.

Latest fresh GCC Matrix: early click385,305 while Solar2 was still obscured
creates no build. Normal Forger arrival/slash reveals Solar2; visible
free two-slot hover370,360 turns green and shows the original resource
panel. Ordinary click starts20; an in-progress F7/F8 is checked without
construction reissue. The old camera coordinate is not assumed valid.

Latest fresh GCC20 actual normal in-progress F7/F8 preserves original
Forger construction60641; source20 completes and lists21 without reissue.
All five normal Triremes59969/60017/60065/60545/60593 finish600 hull/350
supplies, command points35/35. Edge-to-edge lasso initially scrolls the
view and selects nothing; ordinary Solar3 recenter plus interior lasso
50,50..740,395 selects five Triremes, one Polaris and fixed ESP. This is
recorded as actual selection (seven original HUD icons), not an assumption
from rendered ship positions. Normal attack stance and Solar Prime field
movement initiate Solar enemy cleanup. The original victory21 remains pending.

Latest fresh GCC21 movement check: previous Solar Prime right480,245
highlights an object; only Polaris moved/engaged, Triremes remained idle.
Single actual Trireme selection (visible600/350 HUD), then normal empty-field
right600,180 creates original move8367. Repeating with interior group
selection creates8375 for all remaining Triremes. This disproves a missing
Trireme movement owner; no code is changed. Subsequent ordinary F7 has no
Solar player2 Frigates (Polaris naturally kills all three), five living
Triremes and completed Matrix20/21 pending. Original gate right-click is
now used to move/jump the selected Triremes toward Kasse.

Latest fresh GCC map investigation: earlier map clicks after group selection
did not move the view/units; normal pause/continue restored an effective
left-camera click. A separate read-only GDB child uses a whole genuine F7
copy (no setter/call) and normal entry/F8/lasso/map events. It observes
original ObjList lasso KillFocus then SetFocus, SysMap focus restored and
map clicks functioning. Thus a missing lasso focus owner is not established.
The diagnostic child closes normally (inferior0); no diagnostic progress is
used in the fresh root. In the main run, normal five-Trireme-only lasso and
map hover490,568 for two seconds before button-only click shows Solar gate
with all five original HUD icons. Source SysMap updates its cursor mode
at CQE_RENDER_LAST3D; the previous timing/selection combination is not
claimed as a confirmed production defect, and no code is changed.

Latest fresh GCC21 transit/combat: five Triremes receive original jump8530
through normal owned-wormhole hover/right click; all five arrive system1.
Stratum is directly selected from its visible field model (350/1323 HUD)
and normally jumps; genuine F7 confirms59921 system1,350/1234 supplies,
passive supply already serviced the waiting Taos/Polaris. Normal Control_L+1
stores the five Triremes, selected-only attack stance updates all to1.
Original Kasse map move522,547 initiates actual missiles, enemy ship damage
and debris visibly inspected. One Trireme is lost, four continue. Period
cycles Galiot/Forger, not Stratum; intervening group selection restarts the
cycle. Key1 alone selects a cross-system group without changing camera;
Slash explicitly centers in its system. These normal control semantics
explain later wrong-system camera attempts and are not patched. Regular
1/Slash then Kasse gate map533,465 shows the actual Stratum; normal field
585,315 selection and map522,547 move bring supplies to battle. A later
normal in-progress F7/F8 is checked without reissuing that movement.

Latest fresh GCC late normal F7/F8 resumes battle and Stratum movement
without movement/attack reissue. Subsequent F7 has four original Triremes
(597/600/600/600 hull,45–75 supplies), Stratum350/1215 en route at52381,56698,
Matrix20 complete/21 pending, two Tiamats and five Hive Carriers plus four
original Kasse platforms remaining. This genuine whole late save is kept
externally as m2-latest-gcc-late-combat-supply-moving.mission. All earlier
Solar enemy Frigates and the other original combat-wave losses remain
absent after reload. Visible missiles/debris and preserved ordinary group1
selection are checked separately from read-only save facts.

Latest fresh GCC late supply/combat: Stratum reaches51199,26625 and
passively replenishes four Triremes to295–325, then350. Visible original
Defend/Escort control315,525 and friendly Trireme hover400,295 create
SUP_RESUPPLY_ESCORT mode1/working9826/escort60593. Normal resumed attack
map522,560 reaches the last two Tiamats; actual active-fire/escort F7/F8
without command reissue leaves all Tiamats gone, three Hive Carriers and
four platforms. Four Triremes remain597/600/600/600,245–285 supplies,
Stratum350/0 retains mode1/working9826/escort60593. Source21 is pending.

Latest fresh GCC21 platform combat: normal field Collector hover600,180
and right click attack; original waves, Niad and EyeStalk are removed,
Collector550/Cocoon2485 remain, then Collector is removed. Only Cocoon1750
remains; two Triremes have0 supplies. Unexpected native mouse/seek-event messages change selection
before an attempted Acropolis S, so that attempt produces no second ship;
this is not counted as construction. Normal B cycles Pavilion/Greater/Acropolis
(visibly confirmed1668 HUD), then S/Slash queues actual second Stratum60849.
Ordinary pending F7 hull145/0 becomes ready350/2000, command points34/35.
This uses the original2CP/resource limits; no unit/resource/cap is altered.
Four original Triremes keep attacking the sole Cocoon through normal direct
hover650,250/right click; all hulls survive, supplies eventually exhaust.
New supply ship is directly selected at575,300 and normally routed through
the existing Solar owned jump plate to finish the original battle.

Latest fresh GCC end (single current optional GCC binary, initially empty
save-m2-latest-fresh-gcc root): regular second-Stratum Solar gate/right
jump reaches Kasse350/2000; normal map550,563 move brings supply to four
empty surviving Triremes. Original attacks remove the sole Cocoon735,
source21 completes, STMG38 and the actual Mission Successful modal execute.
Normal OK400,449 shows You Win (source total game time2:30:50), Continue640,471
returns to original selector; its retail caption Celareon Training routes
through original IDS_SOLARIAN_TRAINING/onButtonTraining to Sol_Train.dmission.
Normal selector400,333/briefing350,543 restarts in the same process; HUD/map/
subtitles restore, normal Forger selection/right624,300 completes1/2 again.
Original pause and early F7/F8 then normal compositor close exit0. During
reload a temporary host Not Responding overlay appears; source reload reaches
100%, normal save/pause and subsequent visible paused frames work. This
warning is recorded, not counted as a skip or a clean responsiveness proof.
Full close reports surviving TextureLibrary references/dangling texture
references; criterion9 is not accepted from exit0. Existing actual mission
LSan failure remains open. The independent fresh Clang full run follows;
M2 overall is not accepted. User denies parallel input, so unexpected input
messages are not attributed to user actions; their origin is unresolved.
External automation now refreshes target geometry after focus/per action
to detect stale-coordinate hypotheses; no production input code changes.

Independent latest optional Clang full run starts with empty
save-m2-latest-fresh-clang (no GCC/ASan mission copy). Original selector/
briefing, normal Forger/right624,300 and interior three-unit lasso complete
1/2/3; original early F7/F8 restores those objectives and4 pending. Normal
P/F7 then five-second hold/F7 has updateCount9235 in both whole saves,
confirming paused simulation without a clock/state setter; Escape resumes.
Normal individual Taos hover277,260/selection and original map right/left
539,498 reveal Solar3 and complete4/list5. HUD, subtitle and source hints/
voices are visible/active in this independent native Clang process1839787.
Original Forger/basic/Oxidator control and green Solar Prime slots are next.
No full Clang success is claimed at this checkpoint.

## User-operated Clang continuation and newly reported regressions (2026-10-12)

The independent latest Clang process1839787 was handed to the user after
normal Oxidator5 completed/Galiot6 was pending. Its full training acceptance
failed: systemd records SIGSEGV at14:58 EEST. Read-only core diagnosis confirms
RefinePlat::update1454 -> ObjectList::AddObject2291 with obj=null after
FSHIP!!S_Blanus lacks a provider. This is the original Admiral research
completion path, not normal shutdown, an OOM kill, or a successful training
end. Other core threads show audio/GPU/service waits. The extracted temporary
core was deleted; no raw retail data, core, log or capture is added here.
The source Sol_T required-step ledger does not require Admiral research;
this reachable optional command remains a production integration blocker,
not an accepted function or an excuse to claim a full Clang pass. The last
actual F7 file in this root predates the user continuation, so its later
progress is not claimed as recoverable.

User observations: Rally Point does not function; the ordinary minimap does
not update; platform orientations around planets change after quick reload.
All three require bounded normal-input counterchecks before acceptance.
For the minimap, read-only crash state has realRenderTime0.157911, an active
refresh accumulator0.501626 and all inspected map textures unlocked/no DC
ownership: a permanently stuck texture-lock hypothesis is disproved at
this checkpoint. This does not disprove the observed stale display.

Rally hypothesis before modification: the original external toolbar uses
resource86 with RALLYPOINT text486 whereas ObjectComm expects
IDH_RALLY_POINT114; the existing native command adapter misses this pair.
A read-only external GT_TOOLBAR/Toolbar!!Default decode confirms the same
pair for Acropolis, Pavilion and Greater Pavilion. Extending the existing
CheckNativeHotButtonCommands makes portability.native_shell fail before
repair (expected114/received86). The native adapter now translates only
that confirmed pair. It preserves86 with NOTEXT and the already-current
114/RALLYPOINT pair; no resource bytes, UI replacement, simulation, packet
layout or serialized format change. Optional Clang narrow build/check passes
after repair; broader builds/sanitizers and visible actual Rally command
execution remain pending.

Rally narrow follow-up: all five affected Debug configurations build and
pass their non-GPU suites (exact label rally-current). Optional ASan/UBSan
portability.native_shell/app_schema/math_core also pass with detect_leaks=1,
halt_on_error=1; these are synthetic/focused checks, not instrumented retail
mission proof. Actual optional Clang normal F8 of a whole genuine late GCC
F7 copy in save-user-regressions-gdb, Escape/B/Slash, toolbar315,498 hover/
left click and field600,200 left click reaches original onRallyEvent ->
OpAgent::setRallyPoint57329. Read-only debugger confirms actual Acropolis
BuildSupPlat rally(47,79,system2); a mistaken initial BuildPlat downcast was
rejected after dynamic type inspection and is not state evidence. Normal
S builds real Stratum60801, hull216 ->350/supply2000, moving from the
Acropolis production position to46730.9,83874.5. No script setter, packet
injection or save edits. F7 records this bounded diagnostic continuation.
Actual ordinary Forger F/Slash/right600,180 moves the visible ship and
changes its minimap dot between inspected before/15-second-after captures.
MAP_REFRESH continues at the source period. Thus this specific Solar case
does not reproduce a universally frozen minimap; the reported intermittent
black/stale map on system change/reload remains open, including the earlier
GCC Kasse display that only became populated after normal Insert toggling.
Normal paused window close exits0 in this diagnostic (full-close retained
resources remain separately open). No full training acceptance is inferred.

Platform reload hypothesis before repair: ObjectTransform saves one angle
and reloads it onto the platform initialization basis; a planet's full
slot basis comes from original Planetoid::GetSlotTransform and cannot be
reconstructed from that angle. Actual unchanged late F8 has Acropolis57329
slot7/planet57264, preserved position41607.0469,75409.1875 but first matrix
column(1,0,0), inconsistent with that radial slot. The smallest falsifier
is the same ordinary F8 and visible planet/readonly full-basis comparison.
Native Platform::ResolveAssociations now takes only the orientation from
that existing planet/slot after FRAME_resolve, preserving loaded translation.
It uses original SetTransform/Engine update and deliberately does not call
ParkYourself, allocate a slot, recompute supply or reset rally state. The
Windows branch, saved fields/widths, packet layouts, virtual interfaces and
RNG calls are unchanged. Optional Clang narrow build plus native_shell/
app_schema/view_parser pass; visible before/after, GCC/ASan repeats and
broader configuration results are pending.

Platform orientation bounded Clang repeat: original F8 of unchanged genuine
user-regressions-rally-and-moved-forger.mission now visibly restores radial
Acropolis/Oxidator/Pavilion/Bunker models. Read-only actual Acropolis
BuildSupPlat matrix first column(-0.965925813,-0.258818954,-1.13133378e-08)
matches original slot7 basis; translation41607.0469,75409.1875,z-0.000105555,
planet57264, slot7 and rally(47,79,system2) are retained. Normal F7/F8 repeats
the arrangement. All five affected Debug builds/non-GPU suites pass, label
platform-orientation-current. No Windows/retailed image equivalence claim.

Minimap hypothesis is now narrowed and reproduced: normal group1/Slash
switches to Kasse; after eight seconds its minimap is black beneath the
camera rectangle although visible Triremes and Fields render normally.
FogOfWar::UpdateMapTexture after current frame draw calls native
set_texture_level_data -> TextureUploadCallback -> ReplaceTexture. Read-only
GDB finish of actual handle485 returns GR_NOT_IMPLEMENTED because its old
texture is referenced by captured frame draws. Normal Solar movement can
update underlying unit dots while the stale/opaque fog overlay hides them
on Kasse. The source caller/order is retained; moving fog update before
render or forcing original visibility would change the intended contract.

The existing host-dependent sdl3_gpu probe now checks replacement after a
captured draw, current-frame checksum unchanged, next-frame checksum changed
and release at the next frame. Old renderer fails the new exact check.
Native SdlGpuRenderer now retains a sampled old texture until the next
BeginFrame (after GPU idle) or shutdown, while installing the replacement
for subsequent draws. Failed replacement rolls back retirement; no public
interface, persistent format, simulation/RNG or original owner order change.
Optional Clang focused build passes; the texture-lifetime portion now passes
with readback, but the overall GPU CTest still FAILS afterward on host
swapchain resize (1236x1390 unchanged). No skip or partial test is counted as
a whole GPU pass. Visible real mission, other configurations and integrated
sanitizer repeats are pending.

Latest combined visible regression evidence (2026-10-12):
`python /tmp/frontierx-m2-runtime/run-user-fixes.py clang`, `... gcc`,
`... asan` launch the same regular selector/briefing command family and
whole genuine rally/moved-Forger F7 copy in independent save-user-fixes roots.
Builds are optional FreeType/shared-LGPL decoder Debug, `/tmp/frontierx-voice-*`.
No mission state bytes, debug order or objective injection; copied checkpoint
continuations are explicitly not independent fresh full-training runs.
Normal selector400,333/brief350,543/F8, P/B/Slash inspect Solar Prime;
Escape/1/Slash then five/eight-second normal holds inspect Kasse. No Insert
full-map toggle is used. Both GCC and Clang visibly show live populated
Solar/Kasse maps, restored radial planet building arrangement and original
HUD/selection/voices. P/F7/F8/P/B/Slash in GCC and ASan repeats the saved
arrangement. Each bounded normal paused window close exits0.

Actual optional ASan/UBSan PID1879768 executes regular entry, late F8,
Solar platform restore, cross-system original control group/camera, live
Kasse map/fog refresh, late F7/F8 and normal close0 without an ASan/UBSan
report. Leak detection is OFF for this mission repeat; shared SDL3, GPU
driver/Vulkan, FreeType, FFmpeg decoder and host audio/DBus dependencies are
not rebuilt/instrumented. This is not a leak-clean whole mission claim.
Focused optional ASan/UBSan native_shell/app_schema/view_parser/render_pipeline
pass with detect_leaks=1/halt_on_error=1. All five affected Debug builds and
non-GPU suites pass, label map-replacement-current. Existing GPU CTest fails
after its new texture replacement/lifetime subcheck passes, due to unchanged
host swapchain extent; a bounded external window-floating attempt does not
resolve it and is not counted as success. Required native Windows
Debug/Final/module ABI execution is unavailable and remains explicitly open.

Next: complete original independent Clang required training path (the
optional Admiral command is outside Sol_T required steps and is still
unimplemented/crash-capable), repeat actual full-close owner diagnosis, and
retain the external Windows/GPU/retail gates. No M2 completion claim.

## Native mesh-archetype ownership countercheck (2026-10-12; repair pending runtime)

After the user-reported fixes, optional actual ASan/UBSan normal selector/
brief/F8 of unchanged whole genuine late save in save-platform-mesh-before-asan
then paused normal close has LeakSanitizer FAIL:68570 bytes/407 allocations,
including ChannelArch::load -> ScriptSet -> MeshManager::CreateMeshArch ->
PLATFORM_INIT<BT_PLAT_BUILD_DATA>::loadPlatformArchetype. Shared host/DBus
allocations and platform extent arrays also appear; no leak-clean claim.
ASAN_OPTIONS=detect_leaks=1, UBSAN_OPTIONS=halt_on_error=1.

Local hypothesis before repair: platform loader acquires an IMeshArchetype
from the existing MeshManager but its destructor releases only the borrowed
GetEngineArchtype index. Missing mesh-owner Release retains the channel/
script owner; the cheapest falsifier is the identical normal late F8/close
with LSan enabled and comparison of this root. Adjacent spaceship loader
also obtains that borrowed index; its destructor already releases meshArch
and then releases the same engine index again without a separate hold.
Native platform teardown now preserves the mesh-owned engine reference
through dependent render cleanup, then releases meshArch once. Spaceship
teardown invalidates the borrowed alias after its existing mesh release,
preventing a second release. Windows branches, runtime fields/virtual
interfaces, serialized layouts, order of simulation/RNG and required
training objectives remain unchanged. Focused ASan build/check and same
actual repeat are next; remaining owners are not assumed clean.

The subsequent empty-root current Clang regular-entry run reached1/2/3
through normal selection/movement/interior lasso, with original early
F7/F8 and a later genuine completed3 F7. It is paused/closed before this
ownership integration; no full training acceptance is claimed.

Mesh-owner repair results: all five affected Debug configurations build
and pass non-GPU suites, label mesh-owner-release-current. Focused optional
ASan native_shell/app_schema/view_parser pass with leaks ON/UBSan halt.
Actual same genuine seed/regular entry/F8/close with leak detection ON
after repair still FAILS:52920B/359 allocations, exit1. Prior platform
ChannelArch/ScriptSet leak roots no longer appear; MeshRender::SetupMeshInfo
instance buffers reached through ObjectExtent remain, alongside DBus and
an unsymbolized80B allocation. Additional normal mouse actions arrived in
the after-run/user handoff, so the exact byte reduction is not claimed as a
controlled attribution to one edit. No source LSan suppressions or disabled
leak gate; criterion9 remains open. Next specific owner chain is original
ObjectExtent MeshInfo setup/tree deletion and per-instance buffers.

User requests control again. Latest regular optional Clang PID1890276,
save-m2-user-handoff-clang, is launched with a whole copy of that compiler's
own preserved early F7 (save-m2-latest-fresh-clang/f0000000.mission), not a
GCC or debug-created state. Normal selector/brief/F8 restores completed1–4
and in-progress Oxidator5. Native platform basis/minimap fixes are visible
in the handoff; user normal mouse selection/movement arrives and automation
stops after preparing the paused window. This is a continuation of the
independent Clang evidence, not an empty-root latest-binary full pass.
The optional Admiral path remains unimplemented; required original training
continuation and early/late reload/end proof remain pending.

## Remaining per-instance MeshInfo cleanup (2026-10-12; visible repeat pending)

Source review of the still-failing real LSan after-run identifies a second
owner mismatch: ObjectExtent::initExtents restores CreateMeshInfoTree and
MeshRender::SetupMeshInfo, but its destructor omits the original
ObjectRender's DestroyMeshInfoTree. MeshInfo destruction already owns fgi,
faceRenders, effect channels and its renderer reference. The counterexample
is the prior actual late-F8/close LSan MeshRender::SetupMeshInfo roots via
ObjectExtent; the smallest falsifier is the same actual owner closure.

Native ObjectExtent now initializes its owned tree pointer and destroys
that tree at teardown. Its Engine instances remain owned by ObjectTransform,
matching existing MeshChain root/child ownership. Native Explosion's original
commented SetMeshInfoTree calls are restored at both split replacements and
final Bust. This is necessary before enabling cleanup: otherwise the extent
owner retains the already-destroyed pre-split tree and would double free it.
No random call, explosion physics/order, saved field or module interface
change; Windows branches are unchanged. Focused instrumented compilation
and existing native_shell/app_schema/view_parser/game_material_manager checks
pass with leak detection ON, but these synthetic checks do not exercise the
whole exploding-object teardown.

The user is actively operating the separate preserved Clang continuation.
A hidden actual-mission countercheck cannot run through the existing CLI:
--mission-gpu explicitly requires a visible window and --mission-start
requires --mission-gpu. The rejected invocation is neither a runtime pass
nor a skip. No validation rule is weakened. Required actual LSan/normal
F8/close and ASan normal death/split/end repeat waits for a separate visible
run after the user's control handoff, so the new cleanup remains unaccepted
at runtime. The running user binary is retained externally for accurate
core symbolization; it predates this tree cleanup and is not claimed to
exercise the new destructor. Next reproducible input is the unchanged
whole late combat/rally F7 and final Cocoon735 supply-moving F7, through
regular entry/F8/normal close or actual end, with leaks ON where appropriate.

Extent-tree independent validation completes: all five affected Debug
builds/non-GPU suites pass, extent-tree-owner-current. Actual visible
explosion/late-close evidence remains pending as described above. Source
only: checked-in Hotkeys.utf Shift+R encodes current114; retail toolbar86 is
the separately repaired button route. User normal Clang continuation logs
now reach required Helion10/ESP11/Citadel16 and list Greater Pavilion18;
logs alone do not replace passive visual inspection or completed full flow.

## Native Admiral command exclusion (2026-10-12; runtime check pending)

The second independent user Clang continuation terminates at 16:05:53 EEST
in the same optional Admiral completion: RefinePlat::update passes the
missing FSHIP!!S_Blanus instance to ObjectList::AddObject. The externally
preserved running binary matches this core. Read-only core inspection
identifies RESEARCH_ADMIRAL on M_CITADEL, not the two concurrently building
Bunkers. The user confirms the flagship completed at termination. This is
an integration defect, not a prohibited user action. The original Sol_T
required training goals contain no Admiral research. No later F7 exists;
the compiler's own retained continuation remains completed1–4/Oxidator5
in progress. Later logged steps do not reconstruct a saved state or prove
a full completed Clang run.

Before modification: the local hypothesis is that original ResearchButton
permits this command despite the absent native Flagship/Fleet production
closure. Smallest falsifier: regular Citadel hover/click/double-click and
context hotkey must leave this order unqueued. Native ResearchButton now
latches RESEARCH_ADMIRAL as unavailable, retains its original disabled
shape, and explains the exclusion through original StatusBar. EnableButton
cannot re-enable it; left/right/double-click actions are blocked and
IsActive is false for the parent context-hotkey path. Other original
research handling, resource IDs, hints, event priorities and resource
ownership are unchanged. This deliberately reports a missing function;
it neither supplies a successful placeholder nor implements Fleet gameplay.
It cannot rescue a save already containing an Admiral order. No saved
field or public/module interface changes; the new private flag is native
only. Windows source branches remain unchanged; native Windows rebuilds,
ABI and retail comparison remain unavailable/open.

Existing instrumented native_shell/app_schema/view_parser checks pass with
ASAN_OPTIONS=detect_leaks=1 and UBSAN_OPTIONS=halt_on_error=1. These are
synthetic lifecycle/schema checks, not Admiral gameplay acceptance. Actual
normal-button and context-hotkey execution, and latest full Clang training,
remain pending. External late-F8/close and final Cocoon checkpoints also
remain required to accept the preceding ObjectExtent tree teardown.

Native Admiral availability/extent-close actual repeat: external
run-extent-tree-after.py asan clones the unchanged whole genuine
user-regressions-rally-and-moved-forger.mission into save-extent-tree-after-asan,
uses the previously documented regular campaign arguments and current optional
Clang ASan/UBSan binary. Normal selector/brief/F8, Escape/B/Slash, Solar
Prime minimap and Citadel selection show original Blanus hint and disabled
shape plus the explicit unavailable StatusBar line. Ordinary left/right
click, F1 and a real two-click SDL double-click do not enqueue it. Normal
F7/F8 retains the same unavailable state. Read-only production-layout UTF
inspection of the actual F7 finds Citadel58609 queueSize=0, construction=0,
researching=0, building=0. No direct order/script/objective injection. This
bounded instrumented visible native retail-data test is not a full Clang
training pass. All five affected Debug/non-GPU suites pass under label
admiral-availability-current; default font/decoder-free builds compile the
same gate. Original hints/resources and normal building selection work.

Normal window close returns the source modal/mission cleanup, then exits1
because real LeakSanitizer still fails3156B/7 allocations. Prior
ObjectExtent/MeshRender::SetupMeshInfo faceRenders/fgi roots no longer appear,
and no invalid-free/UBSan report occurs in the observed reload/close path.
Remaining symbolized roots are DBus plus an unsymbolized80B allocation;
this is not a leak-clean process. No suppressions, leaks ON. Host shared
SDL3/Vulkan/NVIDIA, FreeType, FFmpeg/audio and DBus are not instrumented.
The actual final Cocoon split/death/end/Continue/restart repeat is now
running separately from the unchanged genuine final F7; its result remains
pending. Captures/raw logs/checkpoint copies are external only.

ObjectExtent final-death actual validation: run-extent-tree-end.py asan
clones the unchanged m2-latest-gcc-final-cocoon735-supply-moving.mission,
regular selector/brief/F8, Escape/1/Slash. Existing saved supply and attack
orders resume normally. Four Triremes, Stratum supply and Cocoon destruction
are visibly inspected; original21 completes, Mission Successful/ordinary
OK/You Win (gameTime2:31:22)/Continue returns to selector. Ordinary restart
initializes1 with original text/HUD/map. The restart capture also shows a
transient host Application Not Responding prompt, so that capture alone is
not clean restart interaction acceptance. Normal window close reaches source
modal4 and cleanup. No ASan invalid free/use-after-free or UBSan report
through the observed death/end/restart/close; leaks remain ON and the whole
process again exits1/3156B/7 allocations. The runtime pre-close /proc maps
resolve the formerly unknown80B caller to external
libnvidia-glcore.so.610.57.04 offset0x9e74a1. Other remaining roots are DBus.
This is an external host leak blocker; do not suppress it or claim a clean
whole-process sanitizer pass. Prior App per-instance MeshInfo/channel roots
are absent in both bounded actual closes. Native Windows/retail comparison
and full independent Clang training remain open.

The subsequent current optional Clang run uses a fresh external clone of
its own actual completed1–4/Oxidator5-in-progress F7, label
m2-admiral-gated-continuation-clang. It is a genuine continuation, not a
fresh full latest-binary pass or a GCC save substituted for Clang evidence.
Remaining required goals will be operated through normal UI.

Current genuine Clang continuation bounded results: normal Oxidator readiness5,
Oxidator selection/Galiot production6 and Forger Basic/Bunker placement7
execute; source8 is listed next. Original Galiot hint/status and produced
unit/model are visually inspected. Early normal P/F7 then five seconds/
F7 preserves simulation updateCount23742, identical objective records;
Escape resumes and F8 reloads that real state. Whole early paused F7 is
preserved externally as m2-gated-clang-early-paused.mission. Subsequent
normal Forger/minimap movement reaches Solar3 for8. This supplements prior
Clang1–4; later goals/full acceptance remain pending.

Keyboard evidence qualification: the observed ordinary F1 emits hotkey49
(IDH_FLEET_GROUP_1 in the checked-in source table), outside the parent's
context-A/Z interval12–37. Thus its no-order result proves the actual F1
input remains harmless, not execution of CMenu_ResBuild's context-research
branch. IsActive's unavailable guard covers that source caller, but this
route has only source review in the bounded gate check. No remapping or
fake hotkey dispatch is introduced to manufacture a runtime proof.

Clang continuation next verified normal sequence: Forger movement to Solar3,
Defense/Sentinel8, Basic/Pavilion9, Advanced/Helion10 and Defense/open-space
ESP11 reach their source completion flags. Each resource hint and placement/
produced model is visibly inspected before continuing. Existing original
Taos/frigate combat runs; no debug orders or objective setters. Normal F7
records each reached state. Original16 (Citadel) is listed next. These are
current native retail-data gameplay observations, not a draw-counter or
synthetic-test substitute; later full training remains pending.

Current Clang continuation16: original Basic/Citadel placement on SolarPrime
is visually inspected and completes16; original18/Greater Pavilion is then
listed. Ordinary Forger/minimap movement back to Solar3 and its real basic
build button select the next valid slot. No optional Admiral order is used.
First enemy frigates fight through original Taos/default combat; the game
continues normally. Full required later sequence/end remains pending.

Current independent Clang18: normal Greater Pavilion three-slot build on
Solar3 completes18 and lists17/Munitions Annex. Its real Advanced control
and green free-slot preview are visually inspected. Source goals1–4 derive
from this compiler's preserved actual early run; current normal continuation
completes5/6/7/8/9/10/11/16/18. Later steps and end remain pending.

Current independent Clang17/22/23: normal Advanced Munitions Annex535,280,
Anvil480,355 and Turbine Dock390,330 builds on Solar3 complete the source
steps in order. Original resource hints, green slots and produced models
are visually inspected; F7 records each. Source24/Xeno Chamber is listed
next. Normal Forger/map movement to Solar4 is underway. No Admiral/Fleet
order, debug command, direct script call or state edit is used.

Current independent Clang24/production reload: original Solar4 Xeno Chamber
420,245 completes24 and lists12. Normal Pavilion selection/Polaris75,525
and six ordinary clicks create the real production queue. F7 contains
ready Polaris59201 plus in-progress59249 and four queued entries; the whole
external checkpoint is m2-gated-clang-polaris-in-progress.mission. Normal
F8 without reissuing production restores those IDs and the remaining queue;
59249 becomes ready, next59329 builds normally and four icons remain in the
visible queue (one active/three waiting). Read-only production save-layout
inspection confirms59201/59249 player1/system2/hull185/supply130. The paused
other early state and this different production state both resume; full
six-supplied count, later combat reload and end remain pending.

Current independent Clang12 completes after the original production reload,
without reissuing its queue: six ready/supplied Polaris59201,59249,59329,
59377,59425,59473 are found in the actual normal F7 (player1/system2,
hull185/supply130), and the visible production count/finished ships agree.
Original13/Eutromil is listed. The normal optional additional Oxidator on
Solar4 repeats the already-integrated original5 owner for ordinary economic
play; it is not an invented training goal or a new production implementation.
Later jump/battle/reload/end remain pending.

Current independent Clang13: normal Basic/Eutromil165,515, Solar4 green
465,370 two-slot placement; in-progress F7/F8 without reissuing the build
continues to source13 completion and14 listing. Original Mission Alert,
subtitle/teletype and finished Eutromil model operate visibly. The genuine
external in-progress whole F7 is m2-gated-clang-eutromil-in-progress.mission.
The normal extra Oxidator supplies the Solar4 economy. Forger and military
units now approach the original Solar/Kasse transition through normal UI;
jump/battle/end remain pending.

Current independent Clang14: six Polaris normal movement to Solar/Kasse
wormhole and default combat destroy the original Mantis Solar platform.
Original panel Shift-click deselects the two stationary defense objects
from the normal lasso group; Ctrl+1 stores the six actual ships. Normal
Forger Defense/Jump75,515/wormhole431,312 starts owned construction. Actual
in-progress F7/F8 without reissue reaches14 completed/15 listed; original
paired S_JumpPlat59777 (player1/system2) and268495233 (player1/system1)
retain ownership in the actual F7. Completed platform/models are visibly
inspected. No script trigger or artificial target state is set. Before
15's regular transit, normal heavy-ship/supply production prepares combat;
remaining15/20/21, late combat reload and end remain pending.

Current independent Clang15/20: normal stored six-Polaris group1/right
wormhole transit reaches Kasse and source15 completion. Four normal Greater
Pavilion Trireme commands and one Acropolis Stratum command produce real
combat/supply actors. The initial post-system-switch map capture has a gray
unrefreshed area; ordinary subsequent gameplay updates the Kasse fog/map
without Insert/full-map toggle. This first-refresh latency is not measured;
no new map repair is assumed from that transient capture. F7/P preserves
the actual state, then Escape resumes. A residual normal build-preview
consumes the first right-click; ordinary right-click cancellation and
reissued Forger movement reach Solar2. Normal Defense/Matrix120,515 and
valid535,325 placement, in-progress F7/F8 without reissue, completes20 and
lists21. Matrix model/planet/map are visually inspected. Genuine external
checkpoints are m2-gated-clang-kasse20-four-triremes.mission and
m2-gated-clang-matrix-in-progress.mission. Full combat21/end remain pending.

Current Clang late reload verifies the actual escort state: Stratum59953,
player1/system1, escort60001, mode1/working8303 is preserved by ordinary
F7/F8 and continues moving with the Trireme group. Four Triremes60001,
60481,60529,60577 initially arrive in Kasse; Stratum supplies drop1925 to
1625 then1185 through real transfers. Normal advance to Kasse2 is a tactical
loss: four Triremes, six Polaris and Stratum are destroyed; several enemy
Hive/frigate actors die and Scarabs take damage, while all four Tiamats
remain. Ordinary retreat/pause arrives too late. No mission completion or
combat acceptance is claimed. Acropolis/Forger survive; source21 remains
pending and the game normally permits economic rebuilding. New normal
Greater commands queue Triremes/Monolith and Acropolis commands queue
replacement Stratum. No balance/RNG/simulation change or fabricated victory.
The genuine pre-attack checkpoint remains external; no state reconstruction
or deliberate code change to ease the fight is made. Current full Clang
acceptance remains open despite all source build/train steps through20.


## Latest Clang user completion (2026-10-12, 19:14; native retail-data runtime)

The user takes over the remaining combat in the already-running optional
Clang Debug process1914585 and reports normal, successful mission completion;
the window remains until leaving. No automated input or mission-state
manipulation substitutes for that final user play. The unchanged executable
is the Admiral-gated continuation described above, using its compiler-own
whole early Clang F7 through regular selector/briefing/F8 entry. Prior
ordinary construction/production and late supply-escort reloads are recorded
above. This is a completed compiler-own training chain across the genuine
early checkpoint, not an empty-root run of one latest executable.

Independent read-only verification after user play:

```sh
cat /tmp/frontierx-m2-runtime/m2-admiral-gated-continuation-clang.exit
rg 'Native training objective .*completed=1|source modal returned|mission closed after modal|original end menu returned' /tmp/frontierx-m2-runtime/m2-admiral-gated-continuation-clang.log
/tmp/frontierx-m2-runtime/read-objective-global /tmp/frontierx-m2-runtime/save-m2-admiral-gated-continuation-clang/f0000000.mission
```

The runtime records completion of every original objective1–18,20–24 (there
is no original19), finally21 completed, source modal2=MISSION_END_WON,
mission close, and original end-menu1=Continue. The subsequent normal menu
input and wrapper exit0 confirm regular process termination, not the earlier
Admiral crash. The final victory display is user-observed; no agent final
capture was taken while the user controlled the game. No exact victory
statistics or final ship composition can be inferred from this log.

The last genuine F7 is18:54, before final user combat: objective65/state1
(Matrix20),66/state0 (combat21), updateCount236882. It is not a saved victory
or a later user save/reload proof. Raw logs, saves and captures stay external.
The known optional provider diagnostics (including T_CraneBot/SYNTHESIS and
Mantis builder types) remain limitations, not successful production stubs.
Exit0 of this uninstrumented run does not prove leak-free shutdown.

| Acceptance item | Latest evidence | Remaining qualification |
|---|---|---|
| Original training sequence and final21 | Agent-visible regular Clang actions through20; user-visible combat/end; runtime all original goals complete | Genuine early Clang checkpoint continuation, not latest empty-root single-build replay |
| Original victory and return | WON2, normal mission close, Continue1, process exit0 | Final display user-observed; restart after this specific victory not executed |
| Early/later F7/F8 and pause | Earlier current Clang normal paused, production and supply-escort reloads then continuation to victory | Final user phase contains no newer F7 |
| Lifetime/sanitizers | Earlier bounded instrumented death/end/close has no ASan/UBSan memory error and removes App MeshInfo roots | Whole LSan still fails3156B/7 external DBus/NVIDIA allocations; latest Tiamat tree-split instrumented repeat open |

Next reproducible work: use the documented genuine late checkpoint for a
focused instrumented Tiamat split/death repeat and normal end/Continue/restart
interaction; then close the latest-build GCC/Clang entry/flow qualifications.
Host GPU resize still fails, and native Windows Debug/Final/ABI and actual
Windows/retail comparison cannot be executed on this host. M2 remains
unaccepted as a whole; this new successful Clang combat/end is no longer an
open gameplay step.


## Technical revalidation after user completion (2026-10-12)

No gameplay/source edit is made in this verification pass. The existing
native ObjectExtent/MeshArch release, platform basis, Rally, texture
retirement and Admiral availability changes are preserved alongside all
other dirty-tree work. Raw evidence and whole genuine saves remain external.

Local falsifiable checks selected before execution:

- Restored ObjectExtent tree ownership might still retain or double-free
  split ship trees. Smallest actual countercheck: regular entry/F8 of the
  unchanged late-combat save, destroy its two remaining Tiamats, repeat F8,
  then close with ASan/UBSan and leaks enabled.
- The3156-byte residual may respond to SDL's documented diagnostic DBus
  shutdown hint. Repeat the same actual lifecycle with that environment hint,
  without suppressions or source changes. This does not change production
  policy; SDL explicitly documents this hint as a diagnostic-only option.
- GPU failure may be a denied compositor resize rather than a swapchain
  recreation defect. Keep the existing registered test/resize/readback
  assertions; compare ordinary Wayland with X11 after normal floating-window
  setup and initial mapping. No simulation or renderer substitution.

Commands for current CPU/build coverage:

```sh
python /tmp/frontierx-m2-runtime/build-five.py technical-current
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-voice-asan --output-on-failure -R '^portability\.(native_shell|app_schema|view_parser|layout|native_streamer|legacy_input|native_window_manager)$'
ctest --test-dir /tmp/frontierx-voice-clang --output-on-failure -R '^portability\.(sdl3_gpu|game_material_gpu)$'
```

The focused regex executes six existing checks; there is no separately
registered native_streamer test under that name. Layout/schema correlation,
normal input/WindowManager and existing original text/hint/UI lifecycle
checks pass with leaks ON. CPU suite totals and exact build configurations
are maintained in BUILD_GRAPH.md. These are synthetic/automated checks,
not independent training or Windows module ABI evidence.

Actual native retail-data Tiamat repeat:

```sh
python /tmp/frontierx-m2-runtime/run-technical-tiamat-dbus.py asan
FRONTIERX_UI_PID=1983190 python /tmp/frontierx-m2-runtime/full-ui.py technical-tiamat key:Escape key:1 key:Slash move:522:560 click:r wait:25 key:F7 key:P
```

The wrapper clones the whole unchanged
m2-latest-gcc-late-combat-supply-moving.mission into its external save root,
uses the earlier documented original campaign selector/briefing/Start/F8
with optional font/shared-decoder profile, ASAN_OPTIONS=detect_leaks=1,
UBSAN_OPTIONS=halt_on_error=1 and SDL_SHUTDOWN_DBUS_ON_QUIT=1. Subsequent
normal Escape/attack/wait/F7 leaves one Tiamat92; normal F8/resume/attack
removes that last ship. Read-only actual F7 and visible models confirm both
remaining Tiamats are gone; no scripted victory, fabricated order or state
write is used. Normal window close yields source modal4 and mission close.
No ASan invalid access/free/use-after-free or UBSan diagnostic occurs in this
bounded observed ship-death/reload/close path. Whole LSan nevertheless
FAILS3156B/7 allocations, exit1, exactly as without the diagnostic hint.
Prior App MeshInfo/Channel roots remain absent. The hint hypothesis is
therefore rejected; it is not integrated as a production workaround.

Visible GPU commands, with host setup explicitly external:

```sh
SDL_VIDEODRIVER=x11 LD_PRELOAD=/tmp/frontierx-m2-runtime/window-map-wait.so python /tmp/frontierx-m2-runtime/float-gpu-check-both.py technical-floating-x11-default-gcc /tmp/frontierx-mission-gcc
SDL_VIDEODRIVER=x11 LD_PRELOAD=/tmp/frontierx-m2-runtime/window-map-wait.so python /tmp/frontierx-m2-runtime/float-gpu-check-both.py technical-floating-x11-default-clang /tmp/frontierx-mission-clang
SDL_VIDEODRIVER=x11 LD_PRELOAD=/tmp/frontierx-m2-runtime/window-map-wait.so python /tmp/frontierx-m2-runtime/float-gpu-check-both.py technical-floating-x11-optional-gcc /tmp/frontierx-voice-gcc
SDL_VIDEODRIVER=x11 LD_PRELOAD=/tmp/frontierx-m2-runtime/window-map-wait.so python /tmp/frontierx-m2-runtime/float-gpu-check-both.py technical-floating-x11-optional-clang /tmp/frontierx-voice-clang
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 SDL_VIDEODRIVER=x11 LD_PRELOAD=/tmp/frontierx-m2-runtime/window-map-wait.so python /tmp/frontierx-m2-runtime/float-gpu-check-both.py technical-floating-x11-gpu-asan /tmp/frontierx-voice-asan
```

The external harness sets only the probe's mapped window to floating through
Hyprland's ordinary window dispatcher. The external preload calls the real
SDL_CreateWindow, pumps events/delays for one second, then returns the same
window; it does not replace a render submission, size request or assertion.
No persistent desktop configuration is edited. Four uninstrumented default/
optional GCC/Clang runs pass both registered checks: real Vulkan641x360 to
800x450, depth recreation, captured-texture retirement, readback and repeated
scene assertions execute. Normal Wayland still fails unchanged1236x1390;
Wayland floating plus mapping delay still leaves640x360 unchanged. Plain
X11 without the initial setup also fails. These are explicit host-conditioned
results; the normal Wayland default CTest is not now green.

Leak-enabled X11 GPU functional checks execute, but whole processes fail:
material64413B/924 allocations, sdl3_gpu84369B/939. A separate mapping-at-quit
attribution repeat reports64456B/924 and84417B/939, with mapped X11 allocation
callers; some other unknown-module/anonymous/unmapped roots remain unresolved.
Do not assert all these larger X11 counts are proven NVIDIA-only or compare
them as an App mission regression. No invalid-access/UBSan report occurs in
these observed GPU paths; neither whole GPU CTest passes with leaks ON.
Shared host SDL3/X11/DBus/Vulkan/NVIDIA, FreeType and FFmpeg/audio are not
instrumented. Only NVIDIA's Vulkan ICD is installed; no available independent
software/other-driver repeat was inferred or downloaded.

Persistent/ABI review: the last ownership fixes add no persistent fields or
public virtual slots. ResearchButton's native availability latch is private
runtime-only; platform reload restores basis from the saved original slot,
retaining position/rally bytes. ObjectExtent tree cleanup and native Explosion
tree reassociation leave IExtent and EXTENT_DATA layouts unchanged; no new
format, RNG call, simulation ordering or Windows branch is introduced by this
verification pass. Current native layout/app_schema/view_parser checks pass.
This is native layout/compile/source review, not Win32 DLL ABI validation.
cl/MSBuild.exe/dumpbin/pwsh are absent on this Linux host. Required next native
Windows actions remain build-debug.cmd and build-final.cmd, followed by
Tools/abicheck/allmods.ps1 with -DumpbinPath, -BuiltGameDirectory and
-ScriptDirectory, and symdiff.ps1 against external retail Mission.dll and
the rebuilt module. No Wine substitute or Windows success is claimed.


Instrumented end/return/restart/reload final result:

```sh
python /tmp/frontierx-m2-runtime/run-technical-end-restart.py asan
FRONTIERX_UI_PID=1987920 python /tmp/frontierx-m2-runtime/full-ui.py technical-end key:F8 key:Escape key:1 key:Slash
```

This separate wrapper clones the unchanged genuine final-Cocoon735/supply-
moving whole GCC save, regular selector/briefing/Start then F8, current
optional Clang ASan/UBSan with leaks ON and no DBus debug hint. The original
saved Stratum60849 movement resumes: position64344,50960 then65766,37796,
then63483,10246; supplies2000 to1750. Four Triremes replenish normally and
Cocoon735 falls to35, then disappears. Visible Mission Successful, ordinary
OK, You Win/gameTime2:31:08, Continue, training selector and regular Start
are inspected. New HUD/map/subtitle/teletype and normal Forger selection/
movement reach source1/2 complete,3 listed. Early original F7/F8 preserves
46/state1,47/state1,48/state0 and normal continuation advances updateCount
801 to833. The save root is subsequently an early restarted-state save,
not a final victory snapshot; the unchanged original late checkpoint remains
external. Initial synchronous load captures include a transient host not-
responding prompt; steady restart/HUD/input/reload captures have no such
prompt. No forced termination or debug/script/objective shortcut is used.

Normal window close gives source modal4 and mission close after the prior
WON2/Continue1. No ASan invalid-free/use-after-free/access or UBSan report
occurs in this observed end/restart/reload/close. Whole LSan still FAILS,
exit1,3156B/7 allocations. Pre-close maps resolve all formerly unknown
frames in this repeat: NVIDIA libnvidia-glcore.so.610.57.04 (including80B
caller offset0x9e74a1), PipeWire libpipewire-0.3.so.0.1608.0, its rt module
and libspa-support. The remaining allocations are DBus messages/buffers
through those host callers plus the NVIDIA allocation. Prior App MeshInfo/
Channel/animation roots are absent, and the bounded restart adds no new
reported App root. This is not a proof that every possible resource or
future cycle is leak-free; shared host dependencies are uninstrumented.
No suppressions, leak disabling or forced global DBus cleanup are integrated.

| Technical gate | Executed result | Open condition / next reproducible action |
|---|---|---|
| Native owner release and split/death | Two Tiamats, combat F8/close; separate Cocoon/end/Continue/restart/early F8/close; no invalid memory/UBSan report; prior App roots absent | Whole3156B/7 host leak failure remains; repeat the same unchanged checkpoints on a host with corrected NVIDIA/PipeWire/DBus lifetime or an independently available driver/audio backend |
| Deterministic focused checks/default/optional builds | Current affected builds and focused leak-enabled checks pass | Native runtime is bounded to documented exclusions and checkpoints |
| GPU resize/retirement/readback | Four GCC/Clang configurations pass the complete registered checks under mapped floating X11 setup | Ordinary Wayland still denies resize; reproduce normal ctest, obtain an actually resized Wayland surface and rerun unchanged assertion |
| Instrumented GPU release | Functional assertions execute; whole LSan fails with external X11/DBus and unresolved driver/module roots | Attribute remaining anonymous/unmapped GPU roots on a symbolized host; counts above are failures, not passes |
| Persistent layout/native schemas | Current layout/app_schema/view_parser pass; release changes add no saved field/public virtual slot | Windows rebuild/module ABI and actual Windows/retail comparison need native Windows tooling/external data |

The locally executable technical checks are now run. No new production patch
is justified by these external residuals. M2 as a whole remains unaccepted:
external lifetime/Wayland/Windows gates and the documented latest-single-build
flow qualification are not silently converted to success. The successful
user Clang completion and historical full GCC gameplay evidence remain valid;
this technical work does not require the user to replay that successful run.


## SDL process shutdown and isolated residual leak counterchecks (2026-10-12)

User requested removal of the remaining host leak reports. Source review
finds a real process-lifetime omission: SdlWindow and NativeStreamer release
VIDEO/AUDIO with SDL_QuitSubSystem, but NativeShell never calls final SDL_Quit.
Installed SDL3-3.4.16 SDL_init.h explicitly requires SDL_Quit even after all
subsystems are closed. Before editing, the falsifiable hypothesis was that
missing global cleanup retains some of the3156-byte residual; the smallest
actual check is the same genuine late F8/normal-close lifecycle with leaks ON.

The launcher main scope now owns a process cleanup guard, which
calls the SDL platform boundary ShutdownSdlRuntime after Run and all its
component/render/window/audio owners have unwound, including error returns
and exceptions. The platform function calls SDL_Quit. Individual window/audio
owners retain their existing reference-counted subsystem cleanup. No global
DBus shutdown hint, leaked-object pinning, sanitizer suppression or private
library-pointer free is integrated. No simulation, RNG, persistent field or
existing virtual/module signature changes. New free function is native
platform API; Windows retail projects do not reference SdlWindow/NativeShell.
Native Windows rebuilt/ABI proof remains unavailable.

Commands/results:

```sh
cmake --build /tmp/frontierx-voice-asan -j4
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-voice-asan --output-on-failure -R '^portability\.(native_shell|sdl3_window|legacy_input|native_window_manager)$'
python /tmp/frontierx-m2-runtime/build-five.py global-quit-current
python /tmp/frontierx-m2-runtime/run-global-quit-after.py asan
python /tmp/frontierx-m2-runtime/run-global-quit-dbus-diagnostic.py asan
```

All four focused existing checks pass with leaks ON; all five affected Debug
build/non-GPU suites pass (central totals in BUILD_GRAPH.md). The actual
patched-binary regular selector/briefing/Start/F8 run clones the whole
unchanged user-regressions-rally-and-moved-forger.mission. Normal B/Slash
reaches original Acropolis/voice;20 remains complete/21 pending. Normal window
close reaches modal4/mission close, then final global SDL cleanup. The
capture falls during synchronous reload and includes the transient host
not-responding prompt, so it is not a clean final HUD visual acceptance
capture. No ASan invalid-access/free or UBSan diagnostic occurs, but whole
LSan remains3156B/7 allocations and exit1. Thus the leak-reduction hypothesis
is rejected despite correcting the documented SDL lifetime omission.

A second patched-binary diagnostic closes normally from the original selector
before starting any mission, with SDL_SHUTDOWN_DBUS_ON_QUIT=1 solely in the
external wrapper. It still reports exactly3156B/7, exit1. The residual is
therefore observable before gameplay and not removed by this documented
SDL debug option. No production hint is added.

To distinguish App resource ownership from shared-library interaction, two
independently written minimal external SDL API reproducers were compiled
with Clang ASan/UBSan and shared system SDL3, outside the repository:

```sh
clang++ -g -fsanitize=address,undefined -fno-omit-frame-pointer /tmp/frontierx-m2-runtime/sdl-audio-cleanup-repro.cpp -o /tmp/frontierx-m2-runtime/sdl-audio-cleanup-repro $(pkg-config --cflags --libs sdl3)
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-m2-runtime/sdl-audio-cleanup-repro
clang++ -g -fsanitize=address,undefined -fno-omit-frame-pointer /tmp/frontierx-m2-runtime/sdl-gpu-audio-cleanup-repro.cpp -o /tmp/frontierx-m2-runtime/sdl-gpu-audio-cleanup-repro $(pkg-config --cflags --libs sdl3)
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-m2-runtime/sdl-gpu-audio-cleanup-repro
SDL_SHUTDOWN_DBUS_ON_QUIT=1 ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 /tmp/frontierx-m2-runtime/sdl-gpu-audio-cleanup-repro
```

Audio-only Init/OpenAudioDevice/CloseAudioDevice/QuitSubSystem/Quit on
PipeWire exits0 without a leak report. Combined VIDEO+AUDIO, ordinary
window, Vulkan GPU device/claim/release/destroy, audio open/close and SDL_Quit
reproduces3204B/6 DBus-related allocations, exit1, twice including the debug
hint variant. It links no FrontierX App/platform/Engine code and reads no
retail input; it renders no mission. Counts differ from the mission's3156B/7
and do not reproduce every NVIDIA80-byte allocation. This independent
counterexample shows that at least the combined SDL/library DBus leak class
exists without our resource owners. It does not establish a precise upstream
root cause or that all host allocations share one defect. Host versions:
SDL3-3.4.16, PipeWire1.6.8, DBus1.16.2, NVIDIA610.57.04; shared libraries are
not sanitizer-instrumented.

Remaining blocker/next reproducible step: fix or validate the same combined
minimal SDL case in the dependency/driver stack, then repeat unchanged
native late F8/close and end/restart/close with leaks ON. The existing shared
library/driver allocations expose no caller-owned release handle here;
freeing captured private pointers or bypassing the normal audio/GPU owners
would be unsafe and is not a valid port fix. No system dependency upgrade,
GPU driver replacement or leak suppression is performed. The launcher
lifetime correction is complete and checked; the3156-byte mission leak
report is still open, and M2 is not fully accepted.

Primary upstream reference reviewed during residual diagnosis:
[SDL3 release-3.4.16 D-Bus owner](https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.16/src/core/linux/SDL_dbus.c).
Its SDL_DBus_Quit implementation handles connection close/unref and optional
library-global shutdown. This source review is not proof of the exact installed
binary or the cause of the observed combined-backend leaks; no upstream code
is copied or library retention workaround added. The independently executed
counterchecks above remain the actual evidence.


## Final M2 Linux lifecycle verification (2026-10-12; guarded repeats executed)

Scope clarification from the user: Windows/native DLL ABI and comparative
retail/determinism validation belong to M3. They remain unexecuted, not waived
as passed, and no longer block the requested Linux M2 acceptance. Existing
GCC fresh-entry full gameplay and the compiler-own Clang early-save/user
completion chain remain the full-training evidence; the following current
binary repeats are bounded lifecycle checks, not new every-step playthroughs.
A mandatory new empty-root replay after each lifecycle/log-only edit is not
an original training requirement. Source/compile evidence alone cannot
replace the documented normal gameplay and visible completion.

Local hypothesis before the final observation change: CQE_MISSION_CLOSE
already clears original HintResource/FieldManager ownership/timers and the
native source-mission sound manager's live streams, but regular victory/quit
returns lacked the existing smoke-path close assertions. Smallest check:
read those existing owners immediately after real MISSION->Close on normal
victory and window-close paths. NativeShell now fails7 if live mission streams
or font-enabled hint ownership survive, and logs actual zero/idle success.
It does not flush, reset, manufacture completion or repair an owner itself.
Font-disabled paths explicitly report hint ownership unchecked, not passed.
No saved field, existing virtual slot, simulation/RNG or input path changes.

```sh
python /tmp/frontierx-m2-runtime/build-five.py m2-regular-close-final
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-voice-asan --output-on-failure -R '^portability\.(native_shell|app_schema|view_parser|layout|native_window_manager)$'
python /tmp/frontierx-m2-runtime/run-m2-regular-close-final.py gcc
python /tmp/frontierx-m2-runtime/run-m2-regular-close-final.py clang
python /tmp/frontierx-m2-runtime/run-m2-regular-close-final.py asan
```

Each wrapper creates a fresh external save root from the whole unchanged
m2-latest-gcc-final-cocoon735-supply-moving.mission, regular original selector/
briefing/Start/F8, current optional Debug font/shared-decoder binary and
previously documented lawful Steam data/options. All input journals, captures,
saves and logs remain external. Normal resumed supply/attack orders destroy
the sole remaining Cocoon; source21/Mission Successful/OK/You Win/Continue,
selector/regular Start, Forger selection/movement1/2, early F7/P/F8/resume
and normal window close are exercised. No fresh Clang full-flow proof is
inferred from using a GCC checkpoint here; its separate own-chain evidence
is m2-admiral-gated-continuation-clang and the user's completed play.

Current GCC and Clang visible success/end/restart/early-reload/close both
exit0. Each actual victory close and final restarted-mission close reports
mission audio streams=0; hint ownership=idle. Genuine restarted early saves
retain46/state1,47/state1,48/state0 and moved Forger; normal reload displays
new HUD/map and current teletype. Old combat21 is not carried into the new
mission. Original UnitComm/sound close handlers reset queues/close handles;
source modal observer connection disconnects before close and reconnects
for the new mission. Earlier actual Tiamat/Cocoon instrumented death/reload/
end/restart closes had no invalid-memory/UBSan report or prior App roots;
the current guarded ASan/UBSan repeat also executes successfully at the
mission-owner boundary, with the whole-process LSan qualification below.

All five affected Debug/non-GPU suites and five focused leak-enabled checks
pass; exact totals/configurations remain in BUILD_GRAPH.md. Complete GPU
assertions previously pass under explicit mapped floating X11 conditions.
The Wayland focusability diagnosis still leaves640x360 unchanged, rejecting
that proposed cause. Normal Wayland fixed-size mission gameplay works;
compositor-denied probe resizing remains a host validation limitation, not
a gameplay pass or a test skip. External combined-backend LSan failures
are retained in full; no suppressed/synthetic leak-clean process is claimed.


## M2 acceptance in the agreed Linux scope (2026-10-12)

The user explicitly moves Windows/module ABI and comparative retail/
determinism validation to M3. M2 is accepted for the documented native Linux
training slice, using the compiled original owners and lawful external data.
The acceptance is based on executed gameplay plus current owner/lifecycle
checks, not on a synthetic check, GPU skip or arbitrary goal setter. It does
not assert complete game/media support, Windows parity or leak-free host
libraries. Third-party process-global allocations are distinct from obsolete
mission streams/hint ownership: the latter are measured zero/idle on six
actual closes, while the former remain reported failures.

| M2 criterion | Executed evidence | Scope / qualification |
|---|---|---|
| 1 Regular entry | Original selector/briefing/Start, lawful external Steam data; GCC empty save root and original Clang entry | Native launcher options below; no internal state manipulation |
| 2 All original training steps | Both compiler logs/visible action ledgers complete1–18,20–24, including source21 | GCC full fresh run; Clang its own initial1–4/early F7 plus regular F8 continuation and user final combat; no original19 |
| 3 Required UI and field mediation | Original menus/toolbar/HotRect/build/production/group/map owners; green slots, hover hints, selection/movement/build/combat | Font-enabled optional configuration; no parallel replacement UI |
| 4 Actual completion and transition | KillTheMantis: player2 platform/spaceship population gone, STMG38 finished,21, WON2, Mission Successful/You Win, Continue1, selector | Latest guarded bounded GCC/Clang/ASan final-save repeats visually inspected |
| 5 Pause/resume | Original P/Escape; GCC updateCount6806 frozen over five seconds, Clang23742 frozen; normal continuation | No tick/rate or script edit |
| 6 Early and later save/load→end | Original F7/F8, early scouting/selection and later queue/build/jump/Matrix/supply-escort/combat states, continued original goal chain→victory | Whole genuine saves/read-only ownership/command/script checks; no fabricated saved victory |
| 7 Hint/status/subtitle/teletype/voice | Original hover callers/resources and natural training/selection/combat presentation observed in both compiler chains | Training speech/unit voices/alerts enabled; excluded portraits/music/spatial SFX remain stated |
| 8 Separate visible GCC/Clang completion | Full original compiler-specific chains above; latest separate success/end/restart/reload visual checks | Current lifecycle changes validated by bounded repeats; no claim of a new every-step ASan or latest empty-root Clang run |
| 9 Repeated entry/load/end ownership | Each current GCC/Clang/ASan victory close and restarted early-reload close checks actual audio map size0 and original source/field hint idle; source modal subscription disconnected before each close; fresh1/2 without old21; ASan no invalid-memory/UBSan report or App leak root | Whole LeakSanitizer still FAILS3156B/7 external allocations; shared libraries uninstrumented; no future/unbounded-cycle claim |

Latest guarded repeat results (not substituted for the full goal chains):

| Optional Debug runtime | Source/end result | Visible end time | Restart / early reload / close | Process result |
|---|---|---|---|---|
| GCC |21 complete, WON2, Continue1; close audio0/hints idle |2:31:22 | Fresh1/2, F7/P/F8/resume, final modal4; audio0/hints idle | exit0 |
| Clang | Same original completion/owner checks |2:31:17 | Same regular restart/moved early reload/close | exit0 |
| Clang ASan/UBSan | Same original completion/owner checks; no invalid access/free or UBSan report |2:31:11 | Same normal restart/early reload/close; prior App roots absent | exit1 solely with LSan3156B/7 external roots |

The end times come from the three actually inspected statistics captures and
are not a determinism comparison: input timing/waits differ. Pre-close maps
resolve all unknown residual frames in the current sanitizer close to NVIDIA,
PipeWire/rt/spa callers; DBus message/buffer roots remain. No suppression,
forced private-pointer free or driver/library pinning is used. Five affected
build/non-GPU suites and five focused leak-enabled checks pass; detailed
central totals/configurations are in BUILD_GRAPH.md. Actual instrumented
Tiamat split/death and Cocoon/end/restart/reload scopes are the bounded runs
already recorded, not an instrumented entire fresh mission. SDL3/Vulkan/
NVIDIA/X11/PipeWire/DBus/FreeType/FFmpeg internals are not instrumented.

Reproducible supported M2 entry from the repository, with user-supplied lawful
installation root and an external writable save directory:

```sh
/tmp/frontierx-voice-clang/frontierx_native_shell \
  --data-root "$LAWFUL_INSTALL_ROOT/DATA" --material-root Data/mat \
  --system-profile Code/App/Src/Conquest.ini \
  --ui-font "$LAWFUL_INSTALL_ROOT/FONTS/bauhmib.ttf" \
  --skip-movies --skip-notification-cue --skip-space-background \
  --skip-animated-portraits --skip-mission-sfx --base-textures-only \
  --skip-legacy-lighting --skip-mission-music \
  --mission-source Sol_Train.dmission --mission-engine --mission-start --mission-campaign \
  --mission-gpu /tmp/frontierx-voice-clang/generated/shaders/sdl3_gpu_probe.vert.spv \
    /tmp/frontierx-voice-clang/generated/shaders/sdl3_gpu_probe.frag.spv \
    /tmp/frontierx-voice-clang/generated/shaders/sdl3_gpu_probe_mt.vert.spv \
    /tmp/frontierx-voice-clang/generated/shaders/sdl3_gpu_probe_t4.vert.spv \
  --string-pack "$LAWFUL_INSTALL_ROOT/stringpack.db" \
  --gen-data "$LAWFUL_INSTALL_ROOT/gendata.db" --game-types "$LAWFUL_INSTALL_ROOT/gametypes.db" \
  --hotkeys Code/App/Src/Hotkeys.utf --save-root "$EXTERNAL_SAVE_ROOT"
```

This is the exact option set executed through the external wrappers, with
provenance paths parameterized here. Use voice-gcc for the matching GCC
binary/shader tree; source original selector label is Celareon Training and
selects the supplied Sol_Train. P/Escape pauses/resumes; F7/F8 quick-save/load
through original command handling. Fresh entry needs no precreated mission
save; bounded verification wrappers intentionally use whole genuine saved
states and are explicitly distinguished above.

Remaining exclusions: full-screen movies, animated portraits, spatial mission
SFX/music, space background, non-base mip use and legacy lighting are omitted
through explicit existing flags. They affect audiovisual parity, not original
training conditions/commands; no training/unit speech, alerts, HUD, map, hints
or objectives are skipped. Optional Admiral research is visibly unavailable
while Flagship/Fleet is unported, and optional T_CraneBot/SYNTHESIS/Mantis
builder providers remain outside this required training path. No placeholder
success is claimed for them. Full campaign/multiplayer support is not part of
M2. Normal Wayland mission runs retain fixed-size UI; Wayland probe resize and
whole-process host/GPU LSan reports remain open technical limitations, not
passed tests. They are retained in the backlog separately from the completed
mission-owner gates. M3 next: comparative/determinism work, including the
explicitly deferred native Windows Debug/Final/module ABI and retail checks;
no M3 comparison is performed or claimed here.
