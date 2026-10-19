# Game Mesh and Material Ownership for Milestone 5

## TM10 locked construction reservation eviction (2026-10-19)

**Actual user runtime/core diagnosis:** the replacement candidate aborts in
`BATCHEDCALLPRIMITIVE::render` during `SolarianBuild::renderRevealed`'s second
`GetPrimBuffer`. GDB confirms slot zero is locked, with zero committed vertices
and indices, while the three-entry alpha-depth pool is full. Allocation always
evicted slot zero, submitting its unfinished reservation. Native draw correctly
returns `GR_INVALID_PARMS`; the existing assertion then terminates the process.
The other two materials are unlocked. This is separate from the earlier
post-flush delay-stage completion defect.

Native material allocation now selects the first unlocked eviction candidate.
It retains pending material/modelview restoration and never relocates live
material/descriptors. If every candidate is locked, allocation reports failure;
GetPrimBuffer propagates that result without dereferencing a missing material.
The Windows slot-zero selection remains unchanged. No persistent structure,
public vtable, simulation, damage, timing or RNG changes.

The existing batcher runtime fixture fills the real three-entry alpha pool,
reserves two writable buffers through original ICQBatch, checks distinct live
material/buffer pointers and exactly two completed evictions, then writes and
releases both descriptors and checks the remaining three submissions. No
unfinished buffer is submitted. Restoring the original slot-zero selection in
a controlled negative run reproduces the actual draw assertion and locked
material violation. The fixed run passes. Existing trail blend preservation and
construction delay-stage checks remain active.

Validation: Linux x86_64 Clang 22.1.8 and GCC 16.2.1 full-media Debug, plus fatal
Clang ASan/UBSan. Build `frontierx_native_shell` and
`frontierx_render_batcher_runtime_probe`; run CTest with
`^portability\.(render_batcher_runtime|native_shell|layout)$`. Sanitizers use
`ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. Final results are recorded
with the private pinned candidate. All three selected checks pass in all three
configurations without skips or sanitizer findings. These generated CPU checks are separate
from the pending actual TM10 construction/reload retest, Windows Debug/Final,
module ABI and retail comparison. The genuine saves and debug log were backed
up privately; extracted core copies were deleted after inspection.

## TM10 delayed construction pool crash (2026-10-19)

An actual follow-up SIGABRT exposes an additional production batching defect:
the alpha-depth wrapper marks its pool empty after delay zero, dropping the
Solarian reveal geometry scheduled for delay one. Native completion now stays
with the existing per-material stage counter; assertions remain enabled.
The existing batcher CPU regression checks both exact submissions in order.
See the [TM10 save/draw follow-up](NATIVE_TERRAN_P2.md#tm10-quicksave-rejection-and-delayed-construction-draw-2026-10-19)
for the core observation, validation and pending actual-scene checks. Earlier
material-only GPU fixtures did not exercise this original construction delay.

## TM10 construction, debris, probe and Mass Disruptor feedback (2026-10-19)

**Real crash diagnosis:** the user's pinned TM10–TM12 candidate receives
SIGSEGV during `MassDisruptor::Render`, at the `mc->mi` access in the disrupt
stage. The original `mc` initialization and reload association were commented
out in this snapshot. The native owner now queries the live target's `IExtent`
and borrows its current chain for the draw. It retains neither an uninitialized
chain nor a pointer across reconstruction. Non-mesh/removed targets have no hull
to distort; the existing blast, damage amount, three-second timer and weapon
lifecycle are unchanged. The private core was inspected with GDB and its
extracted temporary copy deleted. The original systemd core and genuine saves
remain outside the repository.

The repaired caller then needs the previously excluded Puffy material virtual.
The native `FaceGroupRender` override implements the original ellipsoid/radial
formula, `clamp(-2+3*dot(normal,look),0,1)` alpha, diffuse/emission lighting and
separate white textured overlay. It retains face-local UV2 and uses checked
fixed-function triangle submissions with source alpha blending and disabled
depth writes. The original private scratch-array ceiling is avoided through
bounded source-index validation and vectors. A zero ellipsoid axis uses the
finite limiting coordinate. A native tree adapter inserts the same ship model
correction as the normal hull, including child transforms; platforms retain
their existing initialized basis. The existing virtual/layout contracts and
Windows material branch remain intact; Windows builds remain unexecuted.

**Debris color:** `ObjectTeam::SetColors` updated the unique Engine mesh, but
its old extent-material color loop was commented out. Destruction switches to
those separate `MeshInfo::fgi` owners, explaining the observed color loss.
`SetNativeLegacyTeamColor` now also updates only `MM_MARKINGS` diffuse/emission
in the instance's extent chain before destruction. Existing `CopyMeshInfo` and
`SplitMeshInfo` already preserve these complete records. This owner is shared
by Terran, Mantis and Solarian ships/platforms; no race palette, friendship,
texture, archetype material, damage or destruction rule changes. Original
legacy color code corroborates the intended retained markings; a new Windows
retail comparison has not been performed.

**Solarian construction:** `ObjectBuild` only admitted the native Terran build
effect, leaving the original `SolarianBuild::SetupMesh`/beam owner disconnected.
Native setup now also admits the Solarian race during build, reload/assist and
reverse construction through the existing lifecycle. Its original reveal/grid,
sweeping beam, authored hardpoints and mesh-plane intersection code execute.
Model-frame correction places the fabricator hardpoints, target grid and
revealed child meshes in the same presentation frame as their ordinary hulls.
The original source clipper mutates a private clone: native-only metadata now
interpolates UV2 from each original face (preserving seams), initializes clipped
normals before use and resets corner metadata each reveal frame. Transparent
groups, which the original construction code excludes from its private buffers,
remain absent until the intact mesh returns at EndBuild. No retail media is
added, and no parallel substitute beam or simulation is introduced.

**Carrier probe:** GDB identifies the real session's probe objects as
`DumbReconProbe`. Its `PhysicalUpdate` translates toward saved `targetPos`
independently of the inherited launch orientation. A native draw-only frame
aligns the authored -K nose with that actual travel direction and keeps +J
upright. Arrival has a zero-vector fallback. Launch commands, movement,
serialization, timing and RNG are untouched; the unrelated `ReconProbe` owner
is unchanged. The same frame is derived again after load.

Validation on Linux x86_64 with GCC 16.2.1, Clang 22.1.8 full-media Debug and
fatal Clang ASan/UBSan CPU configurations:

```sh
cmake --build "$BUILD" --target frontierx_native_shell frontierx_game_material_manager_probe -j 4
ctest --test-dir "$BUILD" --output-on-failure \
  -R '^portability\.(game_material_manager|game_material_manager_cr|game_material_manager_missing_end|render_batcher_runtime|native_shell|layout|script(09|10|11|12)_schema)$'
```

The private configurations are `p1c-clang-20261015`, `p1b-video-gcc` and
`p1b-video-clang-san`, with the previously reviewed external FFmpeg/font setup.
The existing material probe exercises the real Puffy virtual through native
submission: radial displacement, both passes, white overlay, alpha, UV2 and
live depth/blend state. Its real cloned mesh checks interpolated build corners
and reset; its real extent/split checks confirm team diffuse/emission survive
both debris halves without recoloring the shared archetype. Existing shader,
chrome phase, trail-pool, schema and layout checks remain in the focused run.
Probe frame checks cover heading/elevation quadrants, orthogonality, upright
orientation, unchanged position and arrival. Source caller checks guard the
live-target chain, Solarian setup and probe draw binding. These extend existing
regressions because ordinary intact hull draws cannot expose these transitions.

Both compiler GPU configurations execute `portability.game_material_gpu` and
`portability.sdl3_gpu` without skips through the existing owned-window host
setup. This is generated-fixture GPU evidence, not user mission evidence.
The initial CPU test selected the fixture's origin as a radial vertex; its
observation now selects a nonzero corner of the same triangle. An initial shell
run overlapped relinking and was rejected by the filesystem executable mode;
verification was repeated only after the build completed. Neither event was
counted as a pass. `python3 Tools/ci/test_ci_gates.py` and `git diff --check`
pass. Native Windows Debug/Final/module ABI and full hardware leak/resize checks
remain pending.

**Required user retest:** restore the preserved real TM10 checkpoint, repeat the
Mass Disruptor encounter, construct a Solarian building with a Forger, inspect
Terran/Mantis/Solarian destruction markings and launch a carrier probe. Repeat
F7/F8 during active construction/combat. The exact fixed mission scene, complete
construction animation and audible/visual user acceptance remain pending;
source/fixture checks do not establish them.

## TM9 Chrome/Solarian production material path (2026-10-18)

**User retail-input runtime:** a later closure in the refreshed full-media
candidate reaches `Script09.dll` and reports `Native legacy chrome/Solarian
rendering is not integrated`. This is an explicitly thrown native renderer
capability error caught by the shell, not an inferred memory fault or audio
recurrence. The preceding beacon requests resolve successfully from SFX. No
current systemd core exists. The genuine latest quicksave and campaign progress
are retained separately: the save still contains late TM8 programs, while real
progress unlocks TM9. Unsaved TM9 state is not reconstructed or fabricated.

**Production owner:** native `FaceGroupRender::RenderSolarian` now enters the
existing checked legacy submission path. It implements original
`MMaterial::RenderSolarian`'s UV0 calculation from the source vertex normal rotated
by the shared `chromeTrans`, preserving `u=0.5+0.5*normal.x` and
`v=0.5+0.5*normal.y`. The existing `ObjList` / `UpdateRenderEffects` clock and
rotation order are unchanged. Original non-flickering emissive behavior, source
lighting, per-instance colors/alpha, hidden/building face selection, authored
second UVs and geometry are retained. Rendering does not advance the effect or
consume RNG. Unsupported chrome vertex formats and CQ2 materials still reject;
the original explicit guard is narrowed only for this implemented RPVertex owner.
Windows code, persistent layouts and public interfaces are unchanged.

**Adjacent backend contract:** RPVERTEX2 fixed-function draws and App
StandardVertex effects share the extended shader, but only the latter consume
c30/c31 UV/color constants. The SDL backend now chooses neutral UV/color uniforms
for fixed-function draws in immediate readback/presentation and captured frames,
without changing the stored App constants. This protects environment coordinates
and source-prelit colors when the two production paths interleave.

**Synthetic automated execution:** the existing game-material probe flags a
source-cooked face group as chrome and invokes the actual `MeshRender::Render`
caller. The old owner rejects it; the repaired owner proves normal-derived UVs,
unchanged positions/colors/UV1, non-flickering emissive state, stable repeated
rendering, the original signed J quarter turn and complete-cycle UV restoration.
Existing source lighting, UV seams, explosion splits, texture isolation, malformed
material rejection and App effect tests remain enabled.

**Synthetic visible SDL/Vulkan runtime:** the same production caller samples a
nonuniform repository fixture as environment texture. An eighth turn avoids the
wrapping 2x2 tile's identical bilinear samples at U=0 and U=0.5. The image changes
and restores the exact initial checksum after a full cycle; deliberately collapsed
App UV constants and tinted c31 do not alter its fixed-function image. Existing
App c30/c31 effect checks still pass. Both GCC16.2.1 and Clang22.1.8 execute the
material GPU probe and the full SDL GPU probe without skipping, retaining the
Beam/Battlecruiser strips, sampler, texture and frame-lifecycle checks. The measured
chrome checksums agree across these host builds: start/restored
17741359002509319875 and rotated 16175359857324974307. These are generated inputs,
not a complete retail-ship screenshot comparison.

```sh
cmake --build "$BUILD" --target frontierx_native_shell frontierx_game_material_manager_probe frontierx_sdl3_gpu_probe -j 6
ctest --test-dir "$BUILD" --output-on-failure -R '^portability\.(game_material_manager|game_material_manager_cr|game_material_manager_missing_end|native_shell|layout|script09_schema)$'
ctest --test-dir "$BUILD" -V --output-on-failure -R '^portability\.(game_material_gpu|sdl3_gpu)$'
```

The focused CPU/shell/schema selection passes under full-media Debug GCC/Clang
and fatal Clang ASan/UBSan with leak detection. GPU checks use the established
explicit Hyprland/X11 owned-probe-window setup; sanitizer GPU execution is not
claimed. No gameplay inputs are injected. A new pinned full-media TM9 candidate
preserves genuine progress and both available saves. The last F8 save belongs
to TM8; normal TM9 selection is available without an invented unlock. Actual
fixed-build TM9 gameplay/visual confirmation, full late-content coverage and
native Windows/module ABI checks remain pending. Existing host leak limitations
are unchanged; shutdown texture diagnostics are not treated as this error's cause.


## Battlecruiser special-weapon strip submission (2026-10-18)

**User runtime evidence:** the user confirms the fighter-trail correction works,
then reports an abort after activating the Battlecruiser's special weapon. The
full-media candidate log ends at `AreaEffectBolt.cpp`'s assertion after
`draw_primitive_vb(D3DPT_TRIANGLESTRIP, ...)`. No current systemd core is available;
the diagnosis uses the actual application assertion, source and an executable
render reproducer. No OOM or driver crash is inferred.

**Source review and repair:** the weapon's shared ring owns 42 RPVERTEX2
vertices with two distinct UV pairs. Native PIPE and primitive adapters accept
the strip, but `SdlGpuRenderer::Submit(RenderSubmissionView2)` previously rejects
it with GR_NOT_IMPLEMENTED. The original assertion deliberately stops execution.
The backend now uses the existing single-UV strip expansion for the two-UV
vertex layout as well, preserving complete vertices and reversing each odd
triangle's first two indices. Valid direct and indexed strips become triangle
lists before upload. Unsupported topologies, invalid inputs and allocation
failure still return errors. No assertion, original weapon effect, damage,
simulation, save layout, public interface or Windows rendering path is changed.

**Synthetic visible SDL/Vulkan runtime:** the existing `frontierx_sdl3_gpu_probe`
fails on the old backend at the new two-UV strip check. The fixed backend's direct
and indexed strips have exactly the explicit triangle-list readback checksum,
with UV1 selected and nonempty output. An additional generated 42-vertex ring
goes through production PIPE create/lock/update/unlock/draw_primitive_vb/destroy
and matches all 40 explicit triangles. Existing single-UV beam-strip and full
material, texture, sampler, state and frame-lifecycle checks remain enabled.
Generated fixtures contain no retail media. GCC16.2.1 and Clang22.1.8 full-media
Debug builds execute the GPU CTest on the Linux x86_64 host without skipping.

```sh
cmake --build "$BUILD" --target frontierx_native_shell frontierx_sdl3_gpu_probe -j 6
ctest --test-dir "$BUILD" -V --output-on-failure -R '^portability.sdl3_gpu$'
ctest --test-dir "$BUILD" --output-on-failure -R '^portability\.(native_shell|layout|render_batcher_header|render_batcher_runtime)$'
```

GPU execution uses the existing explicit Hyprland/X11 owned-probe-window setup;
no gameplay input is injected. The selected CPU/shell checks also pass in the
fatal Clang ASan/UBSan configuration with leak detection. Its GPU target is
compiled, not executed as sanitizer evidence. A separate pinned Clang full-media
candidate retains the genuine TM6 checkpoint for a narrow weapon/F7/F8 retest.
Actual fixed-build weapon gameplay, native Windows and module-ABI validation
remain pending; this synthetic readback is not complete weapon acceptance.


## TM6 fighter-trail material eviction (2026-10-18)

**Retail-input Linux runtime diagnosis:** the user leaves the visible defect
paused in the manually controlled full-media TM6 process and saves it with F7.
The new checkpoint is retained separately. A read-only GDB capture of one actual
373-draw frame identifies the bright curved silhouettes as engine trails, not
missing fighter hull textures. The Terran hull texture and all seven authored
mips match the external asset bytes exactly; sampled hull draws use the expected
handle, primary UVs and wrap/linear/point-mip sampler. The trail occupying the
reported screen region instead reaches SDL with ONE/ONE blending. Its real
submission stack passes through `BMPOOL::allocate_material` eviction.

**Source review and repair:** `EngineTrail::SetupBatchRender` requests
SRCALPHA/INVSRCALPHA. The three-entry alpha-depth material pool can fill while
glows and trails are queued. Allocation then restores and submits an older
material before its caller snapshots the incoming one. That submission used to
leave the older glow's blend state in PIPE, so the new trail captured additive
blending and lost its intended fade. `BMPOOL::allocate_material` now saves the
pending material state and PIPE modelview before eviction and restores them
afterwards. It retains the original pool size, eviction/draw order and material
ownership. No simulation, damage, RNG, save record, public type or vtable changes.
This shared production repair also compiles in the Windows source path; native
Windows Debug/Final execution remains unperformed.

**Synthetic automated execution:** the existing
`frontierx_render_batcher_runtime_probe` now fills the real three-entry alpha
pool with three distinct additive glows and queues a fourth alpha trail. Earlier
fixtures did not exhaust that pool. The regression fails before the repair at
the pending blend-state check. After repair it proves one genuine early eviction,
four total submissions, retained caller state and each submitted texture's exact
blend/depth contract. Existing texture-binding, two-stage, geometry and view
barrier checks remain enabled. No new probe or parallel rendering implementation
is added.

Linux x86_64 full-media Debug GCC16.2.1 and Clang22.1.8 build
`frontierx_native_shell` and `frontierx_render_batcher_runtime_probe`. Their
focused execution, and the fatal Clang ASan/UBSan configuration, use:

```sh
cmake --build "$BUILD" --target frontierx_native_shell frontierx_render_batcher_runtime_probe -j 6
ctest --test-dir "$BUILD" --output-on-failure -R '^portability\.(native_shell|layout|render_batcher_header|render_batcher_runtime)$'
```

The selected checks pass without skips, including fatal sanitizers with leak
detection for their bounded test scope. The genuine frame is diagnosis of the
old behavior; it is not a fixed-renderer image comparison. A new private Clang
full-media candidate retains the visible-defect checkpoint for the narrow user
retest. The user subsequently confirms the correction works. Native Windows
and module-ABI checks remain pending; whole-process GPU/library leak limits are unchanged.

## Corvette damage-smoke anchor (2026-10-15)

**User retail-input runtime:** the player reports that TM2 ran perfectly, with
one small defect: damage smoke begins above a Corvette instead of at its hull.
This report accepts the mission run and identifies a narrow presentation follow-up.

**Source review and local hypothesis:** `ObjectDamage::updateDamage` selects a
smoke anchor from mesh extent slices. `SPACESHIP_INIT` is calloc-initialized;
its runtime `extent` is populated from raw archetype vertices in
`ObjectExtent::initExtents`, not loaded from a persistent mission record.
`damagePostRender` previously submitted that raw point through the simulation
transform alone. The direct `SpaceShip::renderSpaceShip` hull additionally applies
Rx90/Ry180. A raw model-Z displacement therefore appeared as world height rather
than following the visible hull. Existing glow/muzzle checks did not protect
the distinct smoke requirement: correcting its anchor while retaining its
already initialized plume orientation.

The native damage post-render owner now corrects only the anchor through the
existing hull model transform. It keeps the original `world * smokeLocal`
orientation. Platforms, special rendering and an active fast instance-mesh path
retain their original placement. The Windows statement is unchanged. No RNG,
damage selection, thresholds, timers, simulation transforms, local fire records,
save layouts, fields or vtables change. This is a presentation-only repair.

**Synthetic automated execution:** the existing
`frontierx_game_material_manager_probe` now checks an asymmetric anchor on a
rotated/translated hull against independent coordinates `(7,19,32)`, instead of
the old height `33`. It also checks unchanged plume basis, nonselected-path
output and immutable input transforms. With the old placement this check fails;
with the anchor correction it passes. It does not establish visible plume contact
on every retail hull.

Linux x86_64 Clang22.1.8 and GCC16.2.1 full-media Debug builds execute:

```sh
cmake --build "$BUILD" --target frontierx_native_shell frontierx_game_material_manager_probe -j 4
ctest --test-dir "$BUILD" -R '^portability\.(game_material_manager|native_shell|script0[123]_schema|sol_t_schema|app_schema|layout)$' --output-on-failure
```

`$BUILD` is `/tmp/frontierx-p1c-clang` or `/tmp/frontierx-p1c-gcc` for these
executed local builds. All selected checks pass without skips. The rebuilt
Clang full-media ASan/UBSan configuration at `/tmp/frontierx-p1c-san` also passes
the existing expanded focused selection, outside the sandbox with fatal errors
and leak detection enabled:

```sh
env ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
  UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  ctest --test-dir /tmp/frontierx-p1c-san \
  -R 'portability\.(layout|view_parser|app_schema|script0[123]_schema|sol_t_schema|data_root|net_buffer|native_shell|game_material_manager)$' \
  --output-on-failure
```

The updated private full-media handoff retains an unchanged genuine TM2
checkpoint and isolated save/configuration copies. No new visible mission run
is claimed. Only a short damaged-Corvette visual check is pending; no complete
TM2 replay is requested. Native Windows Debug/Final/module ABI, hosted CI,
ordinary-compositor resize and previous whole-process leak findings remain open;
the focused sanitizer result does not clear them. No content is hidden or
reduced for this correction.

## TM6 feedback retest (2026-10-17)

The user confirms the player-color and laser-satellite repairs but still sees
fighter texture loss. Primary-UV restoration alone is insufficient; keep the
visual defect open. A new genuine TM6 quicksave is secured separately and
produces visible fighters in a read-only debugger run. Sampled dummy meshes use
legacy face groups and nonzero texture handles, excluding assumed CQ2 promotion
as an explanation for those samples. See the [feedback confirmation](NATIVE_TERRAN_P2.md#tm6-feedback-confirmation-and-retained-fighter-texture-defect-2026-10-17).

## TM6 player markings, fighter UVs and platform weapon frame (2026-10-17)

The native direct Engine owner now uses per-instance legacy markings for
ObjectTeam player colors. The common Fighter diffuse setup restores UV0 after
multitexture callers, and platform loading preserves its initialized Rx90
weapon basis instead of applying ship-only heading reconstruction. Existing
material/Engine regressions cover color and texture isolation, polluted UV
state and loaded horizontal weapon rays. Full-media GCC/Clang and fatal
sanitizer checks pass; the explicit Clang SDL/Vulkan material fixture passes
readback. These are focused synthetic checks; exact TM6 combat confirmation
remains pending. The [campaign feedback record](NATIVE_TERRAN_P2.md#tm5tm6-acceptance-and-shared-rendering-feedback-2026-10-17)
contains owner analysis, unchanged fighter damage, commands, bounded real-save
runtime and remaining Windows/ABI gates. The user separately accepts TM5/TM6.

## TM5 jointed Battleship weapon frame (2026-10-17)

The user reports that Battleship turrets fail to aim and their shots travel
below the map rather than toward the target. Source and external-model review
identify Midas model-Y revolute joints and model -K muzzles. The earlier fixed
barrel repair excluded jointed turrets. Their raw Engine sight can point down
world Z while the rendered hull receives its model-axis correction.

Native `Turret::getModelNodeTransform` now applies the same existing rigid-root
correction to both fixed and jointed Gunboat nodes. `getSighting`,
`swivelToTarget` and distance-based target leading consume that frame, retaining
the complete Engine joint/child pose. Authored pivots, joint state and limits,
angular speed, firing/RNG order, projectile algorithms and saved records remain
unchanged. This repairs native positional behavior; it is not a determinism or
Windows parity claim. Windows code paths and module interfaces remain intact.

**Synthetic automated execution:** the existing material/math check covers 25
ship-heading/joint-angle combinations with the Engine's actual quaternion
rotation convention, independent numerical muzzle rays, world pivots and zero
target-yaw error. The shell source contract pins all three real Turret consumers
to this shared frame. The first test implementation used the distinct
`rotate_about_j` convention; GDB exposed the incorrect test expectation, which
was replaced by Engine's quaternion-based joint construction without changing
production math. Full-media Debug GCC16.2.1 and Clang22.1.8 build
`frontierx_native_shell` and `frontierx_game_material_manager_probe`; the focused
shell/material/layout/parser/script01–06/App/Sol_T selection passes under both
compilers and fatal Clang ASan/UBSan with leak detection. Exact check selection
and private candidate are recorded in the [TM5 ledger](NATIVE_TERRAN_P2.md#tm5-production-integration-2026-10-17).

**Bounded retail runtime:** the repaired Clang build enters actual TM5 and
restores a genuine named checkpoint through the original load menu. This is
not a precise weapon-image comparison. The user's visible Battleship
retargeting and projectile check remains required; native Windows rebuilds and
module ABI execution remain pending.

## Second-Terran fixed-barrel origin feedback (2026-10-15)

The user observes Mantis weapons firing below their hulls in the second-mission
opening. Actual retail-type inspection identifies fixed gunboat barrels using
PlasmaBolt. `Turret::getSighting` now inserts the original ship-model axis
conversion at the rigid ship root while retaining the complete Engine child
frame and authored hardpoint. It corrects actual native launch position/axes;
it does not add a visual offset trajectory. The classification is independent
of graphics settings. Targets, call/RNG order, timing, saved fields, jointed
launchers, platforms and Windows remain unchanged. Native positional behavior
is intentionally repaired, not claimed bit-identical to the previous faulty run.

The existing material/math regression checks an asymmetric rotated ship root,
rotated/nonuniformly scaled child and rotated muzzle, with independent numerical
world-point/basis expectations and unchanged input/nonselected paths. GCC/Clang
complete graphs, affected non-host checks and fatal Clang ASan/UBSan execute as
recorded in [BUILD_GRAPH](BUILD_GRAPH.md). The fresh actual TM2 opening executes;
precise visible weapon-origin acceptance remains the user's focused retest.
No cross-platform determinism or native Windows/ABI execution is inferred.


## Native speaking-marker anchor (2026-10-14)

User feedback identifies the red two-half-circle speaker symbol swinging like a
pendulum instead of staying below the ship. Source review connects real native
sound-owner bSpecialHighlight to SpaceShip::renderSpaceShip's original hiliteTex
quad. Its old J-axis oscillation tilts the native current -J-forward/K-up plane
and rotates the below-hull displacement around the ship origin. Raw model Y/Z
bounds also disagree with the native direct Engine hull's visual axes.

Native marker drawing now uses the same corrected hull footprint as selection,
anchors its own center below that footprint, and applies the unchanged original
oscillation about local K in the marker plane. Center and below-hull height no
longer depend on the animation phase. Preserve the explicit Transform-to-TRANSFORM
copy so the legacy Matrix assignment cannot drop the translated anchor. Original
texture, blend/depth settings, phase increment, highlight lifecycle and Windows
branch remain unchanged. This is render-only: no simulation transform, save
layout, control sequence or RNG changes.

The existing production material fixture adds asymmetric offsets and four phase
values, checking exact stable below-hull center, all quad-corner heights and
unchanged ship transform through the actual math provider and TRANSFORM copy.
The old tilted/translated layout fails these invariants. Final standard and full-media Debug GCC16.2.1/Clang22.1.8 and full-media
Clang ASan/UBSan graphs rebuild/link completely. Existing material variants,
layout, batcher header/runtime and native shell lifecycle execute without failures
or skips. Fatal ASan/UBSan with leak detection reports no error in this focused
synthetic scope; the historical whole-process mission leak remains open.
Visible speaker stability remains user-operated. Actual CI and native Windows
Debug/Final/module ABI execution remain pending. No full mission is automated.


## Completed user run: selection, warp and effect follow-up (2026-10-14)

User retail-runtime feedback reports regular first-Terran victory and save/load,
with small selection corners, keel-up warp emergence and brief colorful impact/
explosion artifacts. The latter are not reproduced or accepted as repaired.
Root reviewed the real Blast/Anim2D/Explosion/MeshExplode submission paths and
native captured-buffer/state handling; no causal effect change is justified yet.
A bounded original campaign/F8 capture retry executes964 source frames and
normal close (result4, audio0, idle hints, exit0), but captures do not establish
actual relevant impact/explosion reproduction. This is not combat acceptance.

Native ObjectWarp::startWarpIn now initializes the current -J-forward/K-up
right-handed control basis. The old CQ2 -K-forward/J-up basis fed a90-degree
pitch into current SuperTrans getters and subsequent ObjectControl targets.
The new native-only initializer retains original stop_heading/gate translation,
call sequence, warp timers and velocity formulas/RNG; Windows is untouched.
New jumps intentionally store corrected orientation; already-active faulty
jumps in prior saves are not normalized. The existing material fixture checks
four headings through real SuperTrans getters, zero pitch/roll, handedness and
corrected hull up/forward, explicitly rejecting the former pitched basis.

Full-media Debug GCC16.2.1, Clang22.1.8 and strict Clang ASan/UBSan execute the
existing material variants, layout, batcher header/runtime and native shell
checks (fatal sanitizers, leak detection enabled). A fresh Clang original-entry
runtime executes the actual Blackwell jump to system2, presents geometry in
60 private captures, and closes normally after2266 source frames (result4,
audio0, idle hints, exit0). The ship is small in those captures; final upright
appearance remains a short user retest, not a full mission replay. Capture
load reduces measured throughput and is not a steady FPS benchmark.

The reported disappearance is separately confirmed as a native communication
abort on an unavailable Harvester voice, before any verified regular victory
teardown; see [audio diagnosis](NATIVE_AUDIO_STREAMER.md). Actual GitHub CI,
native Windows Debug/Final and applicable module ABI checks remain unexecuted;
M3 comparisons remain separate. All saves/captures/retail media stay external.


## Wheel, Harvester muzzle and frame-rate feedback (2026-10-14)

Final bounded retail runtime also enters through original controls, loads the
genuine existing F7 slot with F8, and renders the approximately99-draw scene at
59.6–59.75 source FPS in settled five-second windows. After that interval, actual
wheel+1/-1 events produce original positive/negative MoveForward updates and
visible before/in/out zoom captures. Normal close executes1638 source frames,
returns4, releases mission audio to0/idle hints and exits0. This is load/zoom/
render/close evidence, not a complete mission run. The separate first automated
attempt failed on a not-yet-mapped window; the corrected original-control retry
waits for the actual owned mapped window. Its failure remains retained.


User feedback judges the current presentation very good and identifies three
remaining reported defects: mouse-wheel zoom, the Harvester beam origin and
low frame rate. Full mission outcome/media acceptance is still user-operated.

Native Camera::Notify now admits the real legacy mouse-wheel record, explicitly
decodes its signed high16 delta and calls the unchanged original onMouseWheel
under the existing focus/game-active conditions. The SDL input/dispatcher already
carried that event correctly; the production camera case was excluded by its
Windows guard. Windows keeps its MSG handling. Native renderNuggeting now
transforms the authored Harvester muzzle through the same original Rx90/Ry180
hull visual axes and direct-hull predicate as cargo/glow attachments. Targets,
beam widths, harvesting, physical hardpoints, simulation/RNG and Windows remain
unchanged. Existing numeric material-fixture cases check an asymmetric muzzle
with a nonidentity ship rotation, alongside unchanged simulation/fast-hull
coordinates; GCC/Clang and strict focused sanitizer execution pass.

Performance measurements use optional --frame-stats on the existing original
CQE_ENDFRAME subscriber, with native backend CPU-wall counters. These include
driver waits and are not GPU timestamps or proof of displayed unique frames.
Instrumentation is off by default and adds no M3 control or gameplay input.
On the current RTX3060/SDL3.4.16/NVIDIA610.57.04/60Hz Wayland host, full-media
Clang22.1.8 Debug (no compiler optimization added, no NDEBUG/fast-math) shows:

| Quiet first-Terran scene, approximately79 draws | Source frame rate | SDL submission CPU time |
|---|---|---|
| Shared render pass, exact sampler reapplication reuse | 13–14 FPS | approximately53 ms/frame |
| Omit unbound per-draw fixture textures | 18–19 FPS | approximately38 ms/frame |
| Reuse idle-retired vertex/index/transfer buffers | approximately30 FPS, original limit | approximately3–4 ms/frame |
| Same buffer reuse, original No Frame limiting at normal speed | approximately59.6–59.9 FPS | approximately3–4 ms/frame |

The buffer pool admits only resources retired after the existing GPU-idle
BeginFrame boundary. Best-fit selection checks capacity and indexed/direct
shape; current captured draws retain distinct owners. Reserve failure preserves
the current frame and returns failure. Unused pooled resources are bounded by
256 sets/64MiB and are released once at shutdown. Registered/null texture stages
keep their actual bindings; only explicit fixture-handle consumers retain the
per-submission fixture upload. Immediate submission retains its previous path.
All source draw order, material/effect draws, color/depth/alpha state, geometry,
texture mips and required media remain enabled. No shader/debug/assertion gate
is weakened. The existing SDL GPU fixture adds mixed indexed/direct geometry,
growth/shrink, actual reuse and equal readback across three frames. Its final
scene is now different, so its final checksum must not be compared to the older
single-scene checksum; the relevant image invariants remain explicit.

A capability-checked MAILBOX experiment still spent approximately53 ms in SDL
submission and did not resolve the bottleneck; it was reverted. Default VSYNC
remains. [SDL3.4.16 Vulkan submission](https://github.com/libsdl-org/SDL/blob/release-3.4.16/src/gpu/vulkan/SDL_gpu_vulkan.c)
includes presentation and deferred destruction, so the
coarse call duration alone cannot identify a particular driver function. The
measured improvement after removing allocations identifies resource churn as a
concrete port bottleneck, not insufficient GPU memory or game complexity.
Earlier debugger intervals affected by Wayland unresponsive/focus dialogs,
user interactions and the separate240s diagnostic watchdog failure are retained
but excluded from steady performance evidence. Attempts to capture wheel output
after bounded windows had already closed are inconclusive, not passes.

--no-frame-limit exposes the existing original option only for a mission start,
requires original normal game speed0 (the original option otherwise bypasses
its multiplier), and restores the stored bit before final preference save and
on return/exception unwinding. Original intermediate UI stores still operate;
use a separate private configuration for this candidate. It changes original
pacing/elapsed-time normalization and is not described as timing-neutral.
Global simulation/render constants, update algorithms and RNG calls are untouched.
Candidate metadata pins this option and rejects mismatched launches. The existing
CLI/helper checks protect rejection/provenance; actual GCC/Clang/sanitizer early
failure runs protect stored flags and nonzero-speed rejection. 60 FPS across
large combat, later production and all mission media remains a user test.


## Bounded feedback follow-up (2026-10-14)

The native SDL backend now retains immutable graphics pipelines under the exact
seven fixed-state inputs used by its pipeline constructor. Revisited states
restore the complete uniform snapshot without recreating pipelines or waiting
for GPU idle. Captured draws borrow live cached pipelines; eviction occurs only
at an idle frame boundary, retaining the active set and reducing the cache to
64 sets. A current frame may temporarily exceed that limit. Failure remains
transactional. No public interface, persistent layout, simulation/RNG, material
pass or Windows implementation changes.

The existing production SDL GPU fixture now checks exact state reuse, fragment
uniform restoration, 80 distinct states while a captured draw retains its
pipeline, rejection rollback, idle eviction/regrowth and identical readback on
the following frame. GCC16.2.1 and Clang22.1.8 full-media Debug execute that
fixture and `portability.game_material_gpu` on actual X11/Vulkan, twice through
scene reinitialization, without skips. Three earlier narrow fixture attempts
failed because the new full-target clear inherited the 64x64 diagnostic
viewport; restoring the actual window viewport fixes the fixture while retaining
the renderer's strict clear validation. Those failed attempts remain recorded.

Actual lawfully external retail runtime: the same genuine ready-HQ save loads
through original campaign/briefing/Start and F8, with all graphics/media enabled
and the documented single-font interim. A 30.001-second interval advances 336
source modal frames (11.200 FPS), compared with 326/30.002 (10.866 FPS) before
the cache. No input or per-frame breakpoints execute during either interval.
This small uncontrolled difference does not establish a meaningful performance
gain. Normal close records823 source frames, return4, audio0/idle hints and
process exit0. No full mission playthrough is claimed. Per-draw allocation/upload,
sampler replacement waits and frame-idle serialization remain in the backend;
source review identifies costs, not measured percentages or a proven dominant
bottleneck. Smoothness and renewed weapon-flight user assessment remain open.

The original cursor adapter now builds 27 source-owned context, minimap and
edge-scroll resources from `Globals.rc`, preserving original IDs, dimensions,
hotspots, palette orientation and AND masks. The monochrome arrow uses the
original XOR/AND truth table. Unknown requests fail without changing the current
cursor. The deterministic source-decoder check validates malformed inputs and
bitmap semantics; the real `portability.app_cursor` checks all 27 IDs and
ownership under GCC, Clang and strict Clang ASan/UBSan on Wayland. Exact visual
Windows comparison and full user presentation remain pending.


Native mesh pre-sort sanitizer correction, 2026-10-11: an instrumented
external Sol_Train start found signed overflow in the original
`rand() * (face_cnt - 1)` intermediate with Linux's larger RAND_MAX. The
native path widens only that product; the Windows expression and source
one-draw-per-face order remain unchanged. The existing state_material
generated fixture reproduced the overflow before the fix, then checked
three distinct sorted areas and the next CRT RNG value after exactly
three draws. Focused GCC/Clang and leak-disabled Clang ASan/UBSan checks
pass. Existing original PolyMesh leak reports, native Windows rebuild/ABI
and cross-platform RNG/retail comparison remain open. See
[the M2 sanitizer record](NATIVE_SINGLEPLAYER_SLICE.md#full-sol_train-acceptance-ledger-m2-2026-10-11-in-progress).

Current playable mission render, 2026-10-10: the production Engine,
RenderMgr, texture/material/mesh owners and original `ObjectList::Render`
presented a visible external `Sol_Train` frame under GCC and Clang on
Wayland/Vulkan. The first frame submitted 28 source draws and changed 171
pixels in the bounded 64-by-64 readback. Repeated frames retained separate
captured draws and rendered source selection line lists; a player-owned
Forger could be selected by a normal click and received a move through the
original operation path. Both compiler runs completed the training
checkpoint and save/reload. Base textures, some legacy lighting and
optional UI visuals remained explicitly excluded; this is a playable
slice rather than full material parity. See
[the focused acceptance record](NATIVE_SINGLEPLAYER_SLICE.md#playable-solarian-training-checkpoint-2026-10-10).
Entries below are historical snapshots.

Current external mission draw, 2026-10-10: the active GameSystem/Engine and
original `ObjectList::Render` on lawfully external `Sol_Train` data submitted
28 production draws. The native primitive now honors source camera
`set_perspective`; explicit SDL frame capture preserves each submitted
buffer, texture/material state and transform until presentation. GCC and
Clang visible Wayland/Vulkan runs each produced 171 changed pixels in a
64x64 readback and a live capture showed mission geometry and small ships.
The Forger is player-owned and source-visible, but individual GPU pixels
have not been attributed to it. The existing SDL GPU probe's two-draw
regression ran visibly under both compilers. Normal input, repeated scene
frames and gameplay acceptance remain open. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-perspective-and-multi-draw-mission-frame-2026-10-10).
Entries below are historical snapshots.

Current external mission render gate, 2026-10-10: the original planet factory
now loads its nested mesh through the production `MeshManager` and a bounded
native UTF child-path contract. GCC/Clang visible external `Sol_Train` runs
complete mission load and one source update with a real Forger. The first
`ObjectList::Render` pass submits no draw: source projected-bound selection
sets the Forger invisible after camera framing. No mission material or mesh
GPU submission is claimed. An initial decorative dust point-list draw was
unsupported; explicit `--skip-space-background` now suppresses that optional
visual layer on native builds. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-planet-owner-game-active-update-and-first-object-render-blocker-2026-10-10).
Entries below are historical snapshots.

Current platform material boundary, 2026-10-10: the original
build-support Platform factory and mesh owner are linked into GCC/Clang
native shells. Its source archetype initialization had left `archIndex` at
`-1`; it now stores the validated engine mesh index. This is a CQ2/retail
restoration correction shared with Windows, where rebuild and runtime checks
remain open. Visible external `Sol_Train` runs stop at missing planet
placement before mission-owned GPU rendering. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#source-mission-object-chain-through-platform-placement-2026-10-10).
Entries below are historical snapshots.

Current Forger material result, 2026-10-10: the original Forger factory and
ship closure now instantiate one production `FAB!!S_Forger` in the external
`Sol_Train` object list under both GCC and Clang. Native UTF archive lookup
now resolves the case difference between the GameTypes `SSforger.cmp` name
and the retail `ssforger.cmp` entry; before this, `MeshManager` returned no
mesh and Forger extent initialization crashed. The source game-active
transition then stops at a missing jumpgate script part. The Forger mesh has
not been submitted to the GPU, so visible mission rendering remains open.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#original-forger-instance-and-next-script-blocker-2026-10-10).
Entries below are historical snapshots.

Current Forger render gate, 2026-10-10: original `Fabricator.cpp` compiles
as a native object but cannot yet link with its `SpaceShip`/movement owners.
No mission-owned mesh or material draw ran in this step; the last visible
retail-data runs ended at the missing Forger postcondition. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#forger-object-compile-and-first-production-link-inventory-2026-10-10).
Entries below are historical snapshots.

Current mission material result, 2026-10-10: visible GCC and Clang
Wayland/Vulkan runs passed the external training mission's texture-loading
boundary and completed the original `Mission::Load`. They did not submit a
mission-owned Forger mesh: `ObjectList` found no `FAB!!S_Forger` provider.
The existing explicit base-texture and legacy-lighting limits remain. The
next render gate is a real Forger instance through the original factory,
mesh/material owner and native GPU presentation. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-sol_train-load-completion-before-playable-objects-2026-10-10).
Entries below are historical snapshots.

Current mission texture result, 2026-10-10: external
`astnugget.16.tga0` decoded as RGB565 plus a separate alpha plane; the
production TextureLibrary/native pipeline now converts and uploads that
combination as RGBA8. A four-texel regression in the existing pipeline CTest
passed under GCC and Clang. The native SDL GPU owner currently exposes one
texture level, so explicit `--base-textures-only` loads the highest-resolution
source level; nine-level creation still fails without it. Visible external
mission runs passed this texture and reached 88% load, then stopped at
SoundManager/Teletype prerequisites before a mission draw. No Forger geometry
was presented. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#external-sol_train-load-reaches-the-script-boundary-2026-10-10).
Entries below are historical snapshots.

Current mission material blocker, 2026-10-10: production mission load
reached ObjectList under GCC and Clang on lawfully external `Sol_Train`
data, but the active TextureLibrary could not load
`astnugget.16.tga0` for Anim2D. The resulting zero render texture caused
an assertion at 25% load progress. This is not evidence of a rendered
mission unit. Its data/format/backend cause still needs identification;
see [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-sol_train-load-through-objectlist-2026-10-10).
Entries below are historical snapshots.

Current Sector compile gate, 2026-10-10: source-level native preparation
of the original `Sector.cpp` still fails at its Win32 sector-map UI and
event boundary. No mission-owned mesh or material draw ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#sector-source-closure-inventory-2026-10-10).
Entries below are historical snapshots.

Current Sector terrain prerequisite, 2026-10-10: production
`TerrainMap.cpp` compiles and links into the GPU shell under both native
compilers, but its methods have not executed. `Sector.cpp`
and its sector-map UI/render contracts remain outside the executable
mission link. No mission-owned Forger mesh or material draw ran in this
step. See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-terrainmap-compile-boundary-for-sector-2026-10-10).
Entries below are historical snapshots.

Current mission render/start boundary, 2026-10-10: the source-profile
render interfaces now bind to SDL GPU in explicit `--mission-start` mode, and
GCC/Clang visible Vulkan processes entered production App global startup on
external `Sol_Train` data. App light initialization stopped at the missing
production `Sector` owner before mission reload or any Forger draw. No
mission material, selection or movement result exists yet. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-app-startup-attempt-and-sector-blocker-2026-10-10).
Entries below are historical snapshots.

Current App 2D caller boundary, 2026-10-03: the native App fill/hash
functions now submit six-vertex rectangles through the production
PrimitiveBuilder to the existing CPU render receiver under GCC and
Clang; orthographic and viewport values were checked. The existing
visible material GPU probe also passed under both compilers, but it
did not draw those rectangles or a mission-owned unit. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-app-2d-and-combined-mission-link-2026-10-03).
Entries below are historical snapshots.

Current native 2D backend boundary, 2026-10-03: native
`IRenderPrimitive::set_ortho/set_viewport` now forward real projection
and viewport state; SDL GPU applies viewport and scissor to a bounded
target. GCC/Clang CPU contract checks passed. GPU checks skipped in
the command sandbox but ran visibly on Wayland/Vulkan under both
compilers with clipped readback and restoration. `DrawAgent` rectangles, `OrthoView`, mission
geometry and App light draws remain unexecuted. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-orthographic-and-viewport-contract-2026-10-03).
Entries below are historical snapshots.

Current App light and 2D rendering boundary, 2026-10-03: the original
`light.cpp`, `blast.cpp` and `MorphMesh.cpp` compile under GCC and
Clang, including the source Catmull-Rom basis used by anti-matter.
The native render adapter still rejects dynamic/ambient App light
states and its 2D ortho/viewport calls; the production `DrawAgent`
rectangle owner also depends on legacy VFX. No light, rectangle,
camera or mission-owned unit draw ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-light-profile-and-fleet-closure-2026-10-03).
Entries below are historical snapshots.

Current mission field object boundary, 2026-10-03: the original
`MineField.cpp` and `AntiMatter.cpp` compile under GCC and Clang. Their
production render paths have not executed through the native backend;
the combined mission link and startup remain open. The native D3D9
compatibility declarations now include the source anti-matter stage
values; unsupported backend states still need runtime verification.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-isfx-and-field-object-closure-2026-10-03).
Entries below are historical snapshots.

Current field-render boundary, 2026-10-03: the original field and App
2D animation owners compile under GCC and Clang, while unsupported
stage-one smooth blending fails explicitly on Linux. Neither a field
draw nor a mission-owned Forger has reached the native renderer; the
combined mission link still lacks the SFX owner and other dependencies.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#field-and-animation-owner-compile-boundary-2026-10-03).
Entries below are historical snapshots.

Current mission-object closure, 2026-10-03: `UnbornMeshList` and the
ObjectList compile natively, with Win32 part-debug-menu calls excluded
from Linux. The App shell now binds the production particle aggregate
but has not initialized App particles or rendered a mission-owned
Forger. The combined mission link still lacks field/effect owners.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-command-tracker-and-app-particle-binding-2026-10-03).
Entries below are historical snapshots.

Current App render-owner closure, 2026-10-03: native
`DisableTextures` now applies the production diffuse-only state
through the registered primitive and passes the existing GCC/Clang
CPU material check. Production `UnbornMeshList.cpp` compiles under
both compilers, but its construction-preview and queued-mesh callers
have not run. The combined mission link still fails before a Forger
unit reaches the material path. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-mission-ai-and-app-render-owner-boundary-2026-10-03).
Entries below are historical snapshots.

Current mission-render boundary, 2026-10-03: the existing writable
UTF save owner now passes its create/flush/reopen check under GCC and
Clang after a temporary-path fix. The selected training mission still
has not entered `Mission::Reload`; the combined diagnostic App/script
link remains unresolved. Consequently its Forger has not been
created, drawn, selected or moved by the native game. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#writable-save-flush-and-current-mission-link-check-2026-10-03).
Entries below are historical snapshots.

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

## Regular Terran native legacy material path (2026-10-14, P1b open)

Source review identifies the original App `MeshInstance`/`TreeRender` path,
including `Mimic` through `UNBORNMANAGER::RenderMeshAt`, as distinct from the
Engine/PolyMesh and CQ2 effect paths above. Its old native adaptation bypassed
original material lighting and second texture coordinates. A successful base
texture load could therefore not establish the selected mission's presentation.
The existing game-material probe now represents this real caller rather than
adding a separate renderer.

The regular mission enables the native original-material path unless legacy
lighting is explicitly excluded; accepted training retains its documented
bounded mode. FaceGroupRender uses the original LIGHT vertex calculation,
material diffuse/emission arithmetic and alpha, stage-zero texture modulation
and stage-one emissive ADD with independently retained per-corner UV2. The
original cooking branch, vertex count, random calls and source face traversal
remain unchanged. UV2 follows original split ordering and interpolation through
repeated explosion splits, including the source's unusual edge ratio. No save
fields change. Native light registration/activation uses the original LIGHT
owner; the Windows D3D mirror remains unchanged. Unsupported chrome, CQ2 or
unintegrated special mesh modes fail explicitly.

Automated synthetic runtime: the existing game_material_manager CPU variants
pass on GCC 16.2.1, Clang 22.1.8 and focused Clang ASan/UBSan. They exercise the
real source LightManager/material caller, exact ambient/material/alpha values,
independent UV seams, repeated splits and unchanged next RNG value. The
existing `portability.game_material_gpu` executes on Clang SDL/X11/Vulkan,
requiring dim/light/restore and emissive/suppressed/restore images and opaque
primary output. Generated repository fixtures contain no retail assets.
Bounded externally supplied regular-mission load/reload/close also executes
with this path enabled; it does not verify every required asset or effect.

Native Windows Debug/Final rebuilds and affected module ABI checks, suitable
retail reference captures, complete mission presentation before/after reload
and user visible/audible acceptance remain pending. Current strict mission
sanitizers stop on separately documented owner defects; synthetic renderer
success is not a clean whole-mission sanitizer result. See
[NATIVE_TERRAN_MEDIA.md](NATIVE_TERRAN_MEDIA.md) and
[LINUX_PLAYTEST_CI.md](LINUX_PLAYTEST_CI.md) for current evidence.

## PolyMesh material state verification (2026-10-14, synthetic checks executed)

Source review identifies a separate selected-mission blocker in the ordinary
Engine/PolyMesh caller: native `IRenderPrimitive::verify_state` always rejected
verification. The original material owner could consequently choose fallback
passes after already configuring a supported diffuse/emissive pair. Its state
writes also ignored native rejections, allowing an unsupported additional stage
to leave a previously accepted state in place. Existing base-texture GPU input
cannot disprove duplicated or omitted emissive passes.

Native verification now revalidates accepted render/texture-combine states,
both samplers and both texture bindings through the bound backend callbacks;
missing callbacks and stale/rejected bindings fail. PolyMesh checks native
state writes and stops before drawing on rejection. The Windows calls remain
unchanged. The existing fixtures now supply a recording backend and execute
supported paired textures, single-pass selection, stale bindings and rejection
before draw while retaining their exact source lighting/error assertions.
Optional GCC 16.2.1 and Clang 22.1.8 Debug CPU checks pass; focused GCC/Clang
ASan/UBSan checks pass with leak detection enabled. The existing optional GPU
fixtures execute the paired-texture image checks on the explicit X11/Vulkan
host. The first fixture run failed because its old descriptor supplied no
state callbacks; that failure prompted fixture integration, not weaker state
validation. These results do not establish complete mission rendering.

A separate read-only debugger observation on the existing optional Clang
Wayland/Vulkan shell executes six distinct original PolyMesh material owners,
including diffuse/emissive bindings, with GR_OK results. The bounded eight-
second source interval executes 103 frames and closes through original QUIT,
with zero audio handles, idle hints and advancing communication portraits.
This observation precedes the verification correction and does not prove
correct pass images. Single-font and movie exclusions remain explicit; actual
intended shading and complete mission/media acceptance remain open. Two
attempted X11 observations failed after one frame with modal result zero and
provide no material evidence; the fixture-only host setup did not prepare the
native shell window. Raw logs remain private outside the checkout.

## Required compound parts and material loads (2026-10-14, P1b open)

A fresh strict external-data Terran load exposes unresolved required material
maps and then a null Hybrid Center mesh at actor initialization. Native material
loading rejects missing formats/frames and XMesh returns its failed-load
contract. The Engine now rejects a declared compound when its required root or
listed part fails, releases acquired parts through their original owner and
does not fall back to a simple model. Windows behavior remains unchanged.

The existing generated compound fixture uses real part files and fixed joints.
It checks a connected root/child model, a missing root, a missing listed part
and failure after earlier parts loaded; a simple root mesh is also present to
expose forbidden fallback. Optional GCC/Clang Debug and both focused sanitizer
configurations execute the state-material and three game-material checks with
no reports or skips. A part claimed by another Engine component could still lack
the required renderer. Native RenderMgr now distinguishes a declared mesh or
MultiLevel directory whose loading failed from an absent renderer. A private
native failure signal reaches Engine, which releases the archetype and returns
INVALID; the BOOL32 interface, Windows path and persistent layouts remain
unchanged. Genuine hardpoint-only nodes retain the original absence result.
The corresponding real hardpoint/compound fixture extension is pending.

The first fresh scoped texture log contained no load request for seven required
map keys and no unsupported-format failure. Resource inspection confirms those
palette/alpha maps reside in the outer planet/Hybrid Center compound libraries.
Original compound-viewer callers load that outer library before creating the
Engine instance. Native MeshManager now loads the exact opened object's shared
texture/animation library through the GameSystem TextureLibrary owner before
Engine creation; files without outer libraries retain child loading. There is
no ancestor search, substitute material or widened texture-format policy.

Actual optional Clang Debug Wayland/Vulkan retry after this failure propagation
rejects the first required planet map and its compound root. It then reaches
the unchanged Planetoid required-mesh assertion (SIGABRT), rather than accepting
the partial model and reaching the later null-mesh fault. This is still a
failed retail load, not a gameplay or teardown pass. The texture dependency
repair remains required. Private inputs and logs remain outside the checkout.

The subsequent optional Clang Debug Wayland/Vulkan retail retry with that
dependency repair requests all seven maps and records no texture-load or
required-material/component errors. The original mission loop executes 132
frames and returns original QUIT (4), with zero mission audio streams and idle
hints after close. A communication portrait starts and advances eighteen frames
with twenty-two draw calls. This is bounded load/close owner execution, not
correct-pixel, audible, full asset-census or complete gameplay acceptance.
Fresh save/reload, native Windows Debug/Final, affected ABI execution, actual CI
and complete user mission/presentation checks remain pending.

The existing `--mesh-object` production-owner diagnostic may now run with
`--mission-engine --mission-gpu` without `--mission-start`. Its genuine
MeshManager load/create/destroy runs after SDL GPU texture callbacks are bound,
so later compound models can be checked in separate processes. The ordinary
non-GPU diagnostic and playable mission ordering remain unchanged. Deliberate
later-model loading must stay out of the playable process because original
mesh cooking consumes RNG. Compilation and separate external-model execution
of this diagnostic extension are pending; successful loading alone will not
prove correct rendered output.

## Pooled mission effect texture states (2026-10-14, validation pending)

Original `StatisBolt::Render` and `RepellentCloud::Render` set both stages'
color/alpha arguments and the second UV index. Native RenderBatcher pooled
flushes previously replayed only stage-one COLOROP/ALPHAOP and omitted both
stages' TEXCOORDINDEX, so an earlier backend state could survive a queued draw.
The backend's direct two-stage checks cannot disprove this caller regression.

Native replay now includes the supported arguments and UV index for both
stages. Pooled setters reject unsupported fields/values before caching; Windows
replay and shared layouts remain unchanged. The existing
`portability.render_batcher_runtime` check is being extended with the real
RenderBatcher/BMPoolJr caller, exact queued state/binding restoration and rejected
setter rollback. Its outcome decides whether the selected ability state path
can be used; it does not simulate the ability or accept its visible effect.
Optional Clang 22.1.8 Debug `frontierx_render_batcher_object` compilation passes.
The existing real pooled regression passes optional Clang/GCC Debug and both
ASan/UBSan configurations with leak detection: two queued RPVERTEX2 draws
retain all fourteen stage fields, distinct texture bindings and exact geometry;
unsupported UV/op writes leave the later submitted snapshot unchanged. Failed
backend callbacks and stale cached ownership after genuine unbind/destruction
reject verification. Native verification rebinds cached active textures and
propagates failed state/binding calls before claiming success.

The existing Clang X11/Vulkan GPU pair and GCC selected GPU check also execute
the bounded Stasis/Repellent color-and-alpha MODULATE, WRAP/CLAMP and original
blend-factor image predicates with exact restoration. That receiver test uses
one triangle for its independent single-fragment numeric oracle; an initial
overlapping-triangle fixture correctly failed because blending occurred twice.
It retains earlier ADD/mip/alpha/resize predicates. Actual ability rendering,
Windows Debug/Final/affected ABI and user inspection remain required.

2026-10-14 separate owner diagnostic follow-up: optional Clang's genuine
GameSystem/MeshManager/Engine/TextureLibrary load/create/destroy path also
executes the original nested planet compound successfully. The CLI accepts
confined slash-separated relative object names, rejecting absolute paths,
traversal, empty components and Windows-style paths. Together with the nine
previous later-unit/attachment checks, this closes the named load-owner
checks; none submits a scene or establishes correct geometry/material pixels.
Real mission rendering, later abilities and full user presentation acceptance
remain separate. The extended existing native_shell CLI regression passes under optional Clang,
including absolute/traversal/dot/empty-component rejection and a nested path
reaching the genuine owner. Windows/actual CI remain pending.


## Original movie captured-texture teardown (2026-10-14)

The real optional MovieScreen Esc path released its video texture after the
source EndFrame had captured its draw, while native slots were already unbound.
The old backend rejected that release and the source owner aborted. Native
texture destruction now retires captured SDL storage until the existing
idle-waited BeginFrame/Shutdown boundary, while invalid, stale and bound handles
still reject destruction. Logical ownership is removed immediately; retirement
allocation failure leaves ownership unchanged.

The existing `portability.sdl3_gpu` fixture captures a unique green texture,
checks bound rejection, destroys it after unbind, rejects stale operations and
requires identical image/readback and actual presentation before retirement.
Its next frame retires storage and restores the baseline. GCC16.2.1 and
Clang22.1.8 optional VIDEOon Debug execute this and the existing production
`portability.game_material_manager_gpu` check on X11/Vulkan. Actual external
cq_intro MovieScreen now closes through Esc and natural EOF with native exit0.
This is owner-lifetime and bounded movie execution evidence, not user audiovisual
acceptance. Native Windows Debug/Final/affected ABI and actual CI remain pending.


## Planet pixels and shader argument contract (2026-10-14)

Actual optional VIDEOon Clang22.1.8 Wayland/Vulkan diagnosis of the original
Planetoid/Engine/PolyMesh caller confirms diffuse/cloud bindings, nonzero UVs,
MODULATE, authored cloud alpha and SRCALPHA/INVSRCALPHA. The native pipeline
retains colored 128x128 diffuse data with eight authored levels and 64x64 cloud
data with seven levels and varying alpha; the SDL registration and captured
draw preserve those dimensions, bindings, stages and blend states. This proves
submission/loading facts, not correct sampling by itself.

The launch helper reversed the final `--mission-gpu` arguments. NativeShell and
SdlGpuRenderer require EXTENDED then GAME: `_t4.vert` reads RPVERTEX2 UV at
location2, while `_mt.vert` reads MTVERTEX specular at location2 and UV at
location3. The wrong game shader sampled specular zero as UV, explaining the
gray illuminated planet despite valid resource/state data. Correct only this
command composition; changing materials, mip content or renderer behavior is
not justified. The existing CI helper regression pins the source argument and
shader-input contracts.

A same-build, no-presentation-exclusion original F8 and normal system-map camera
retry visibly restores colored continents/cloud structure and HQ textures,
then closes through source QUIT4/native exit0 after179 frames with audio0 and
idle hints. Private before/after captures and checkpoints remain outside the
repository. This closes the named launch defect, not complete later assets,
abilities, maps, audiovisual fidelity or user acceptance. Actual CI and affected
native Windows/ABI remain pending; historical incorrectly configured captures
are not upgraded into full rendering evidence.


## User-reported world effects and construction follow-up (2026-10-14)

The user reports weapon fire toward the lower map edge for Corvette, Blackwell,
Mantis and earlier training Celeron units, absent ship/platform construction,
missing green free-space previews, displaced exhaust and rotated Harvester cargo
tanks. These observations reopen presentation acceptance; compilation or asset
loading does not resolve them.

Original Beam/GattlingBeam/PlasmaBolt and Harvester beams submit world vertices
through Camera/RenderBatcher. The existing real Batcher fixture now uses its
actual scalar math provider and records receiver projection/viewport/modelview.
A preserved pre-fix owner fails the getter/nonmutation, all four world-to-HUD
view transitions and modelview rejection/identity predicates. Native getters
now return cached output without mutating it; nonbatched setters update cache
only after successful application and reapply identity after external changes.
Native projection/orthographic/perspective/viewport changes drain source pools
before changing the shared receiver state. Windows bodies and persistent/module
interfaces are unchanged. GCC/Clang and both ASan/UBSan focused checks pass,
including a genuinely rejected new projection leaving the old receiver state.
These are synthetic receiver-contract checks; actual weapon direction remains
a separate live test.

Original construction uses the existing ObjectExtent tree and TerranBuild
GirderRender channels. The restored native ship construction caller applies the
same original local model-axis correction as its regular Engine hull draw.
Native EffectChannel respects that existing scaled TreeRender transform rather
than replacing it with the raw Girder instance transform. Platforms retain their
original uncorrected model convention. Original masks, face/UV order, build
progress and authored resource requirements remain authoritative. The genuine
owner/start/reload and live lifetime record belongs in
[NATIVE_TERRAN_CAMPAIGN.md](NATIVE_TERRAN_CAMPAIGN.md); exhaust/cargo attachment
scope is in [NATIVE_TERRAN_MEDIA.md](NATIVE_TERRAN_MEDIA.md).

**User execution update:** the user personally operated the weapon-observer
scene and confirms that building construction looks correct, but weapon flight
remains unchanged. The Fabricator's original opening-wing animation is missing,
and perceived frame rate is low. These are user results, not scripted checks;
the five reported fixes cannot be declared complete. Actual Bolt/PlasmaBolt
initialization and rendering now execute in the observed combat interval. A
missing-Fabricator script lookup subsequently aborts the run and remains an
additional gameplay blocker.

**Performance source review:** the candidate is an unoptimized Debug build. Its
weapon observer stops in Python/GDB at every Bolt/PlasmaBolt render even after
printed samples are limited; that run is unsuitable for uninstrumented frame-rate
acceptance. Independently, native SdlGpuRenderer::SubmitBytes waits for GPU idle,
allocates per-draw geometry/transfer/texture storage and submits uploads for each
draw. BeginFrame also waits before releasing captured-frame resources. Frequent
original Batcher material eviction is observed in the private run. These are
concrete synchronization/allocation paths and observed churn, not measured
percentage attribution. An uninstrumented matched-scene profile is required
before declaring the dominant cost or choosing a performance correction.

The abort above is not attributed to a new source lookup defect: read-only UTF
inspection proves that the selected archival pre-encounter checkpoint has no
`FAB!!T_Fabricator` record. It predates the required compound-geometry restoration.
The current genuine construction checkpoint contains its 604-byte Fabricator.
Discard the incomplete checkpoint as a test input; preserve the original required
lookup/assertion and do not manufacture the actor. No current core was emitted
when the debugger intercepted SIGABRT.

A subsequent debugger-free optional Clang VIDEOon fresh Terran bounded run
executes600 original end-frame callbacks over the configured60-second source
modal interval (approximately10 frames/second), then returnsQUIT4/native exit0
with zero mission audio streams and idle hints. Source portrait counters are
5 starts,289 changes and425 draws. Normal user input occurs during the interval,
so it is not a fixed-camera benchmark or an automated gameplay recipe. A
separate20.004-second process sample consumes16.14 aggregate CPU seconds
(approximately81% of one CPU core, summed over process threads). This proves
that the low rate remains without the weapon debugger; it does not isolate GPU
waits, CPU lighting, uploads or decoding individually. The gprofng attempt ends
withSIGKILL during initial loading, without a kernel OOM entry; its paused
experiment provides no usable attribution and is not a runtime pass.

**Scoped captured-upload correction:** independent owner reviews verify that
SubmitBytes allocates distinct resources for every captured draw and retains
them until the existing idle-waited BeginFrame boundary. Native captured mode
now omits only the redundant idle wait before each upload. Immediate mode still
waits before replacing its resources; frame retirement, shutdown, invalid-handle
and failed-submission checks remain intact. Upload commands are submitted before
Present, following the SDL GPU command-ordering contract. The optional Clang
shell and existing GPU fixture rebuild15/15 successfully. GPU image/lifetime
execution and a comparable post-fix frame-rate measurement remain pending;
the preceding10-FPS interval is the pre-correction observation. No textures,
effects, lighting or animation are omitted to improve this rate.

**Corrected bounded retail execution:** the current optional Clang VIDEOon HQ
continuation executes actual Bolt/Plasma initialization and rendering. Four Bolt
and two Plasma samples align actual velocity with the actual explicit target
displacement (direction cosine effectively1), rather than the earlier near-Z
flight. Fabricator reload has a real Sc_build instance and a capture shows
extended wings. Original F7 and normal close succeed after the user releases
the window;2559 source frames returnQUIT4/native0. This does not accept complete
visual fidelity, training gatling or a quantified performance improvement.

**Post-reboot recovery, 2026-10-14:** both fresh optional GCC/Clang Vulkan
fixture pairs execute the captured-upload/resource-retirement and image
contracts without skips, as recorded in BUILD_GRAPH. Original campaign,
normal movie Escape, animated briefing, Start and an early original F7 save
execute again on the rebuilt Clang all-media interim candidate. Saves now use
durable external storage. Earlier HQ and weapon captures/checkpoints are lost;
their recorded observations remain historical, not current continuation.
A proposed two-stop source-frame-counter measurement cannot attach because
the host rejects ptrace; the game remains running and the policy is unchanged.
That attempt provides no FPS result. A bounded source-modal interval or a
parent-owned debugger run is still needed for post-correction measurement.
Independent source review confirms training Gattling uses original world-space
target/endpoints and does not consume the corrected Bolt/Plasma pitch helper.
Actual training endpoint/receiver observation remains open; no speculative
Gattling simulation/RNG change is made.

**Actual free-space preview owner correction:** after original ready-HQ F8,
Fab selection and defence/Turret selection, genuine relative mouse motion still
does not reach UnbornMeshList::RenderMeshAt. Read-only parent-GDB execution
finds BUILDARCHEID33400 present, but NativeAppCursor::GetCursor returns DEFAULT104.
Original ObjComm requests BUILD127; the native adapter rejects that resource,
so the unchanged original Unborn cursor condition skips rendering entirely.
The earlier scoped material correction alone could not fix this gate.

NativeAppCursor now embeds the existing source-owned Res/build.cur at build
time with strict original CUR shape/extent/palette/mask/hotspot checks and uses
SDL's color-cursor owner. Failed requests preserve the prior resource, and no
game condition, Windows branch, saved type, interface, simulation or RNG changes.
The existing real-host cursor fixture exercises BUILD127 and transactional
unknown-resource rejection under GCC, Clang and strict Clang sanitizers.
Actual rebuilt optional Clang all-media/single-font original F8 then shows
the Turret preview green in free space and red in an invalid location. Four
bounded original RenderMeshAt samples use arch33400, RGB0/255/0, alpha128 and
three children; genuine legacy submissions include24/150/216 vertices.
Normal close returns4/native0 after2035 source frames with audio0 and idle hints.
These are bounded retail execution/captures, not original-image or full user
presentation acceptance. Other original cursor resources remain a P1b UI gap.

**Measured remaining performance:** a parent-owned debugger observes only the
source frame counter at the start/end of one30.002-second resume, with no inputs
or per-frame breakpoints. The genuine loaded ready-HQ all-media Debug scene
executes326 source frames (10.866 FPS). It differs from the earlier fresh-mission
interval, so it proves continued low frame rate, not an attributed improvement.
One stopped stack is in SDL/GPU Present; source review retains per-draw resource
allocation/copy, state-change pipeline creation/idle waits and repeated material
eviction as concrete costs. Their dominant contribution remains unmeasured.

### Native ship selection footprint correction, 2026-10-14

**Source review and production-owner compilation:** original
`ObjectSelection::DrawSelected` in `TObjSelect.h`, called by
`ObjList::drawSelectedList`, draws its corner triangles in the object's local
X/Y plane. The direct Engine hull in `SpaceShip::renderSpaceShip` applies the
original 90-degree I and 180-degree J model correction: authored model Z becomes
the visible local Y length, while authored model Y is height. Previously the
markers used the uncorrected model height as their planar length. This is a
footprint mismatch, not a measured DPI or line-width failure.

The native draw now obtains a local transformed box through
`NativeShipSelectionBounds` in `NativeShipVisualTransform.h`, under the same
direct-hull predicate: spaceship, no special render, and either no instance mesh
or FastRender disabled. It transforms all eight box corners using the existing
hull correction. Other paths copy the original box. The original marker width
formula `min(0.1 * (L2 - L1), 200)` and triangle geometry remain unchanged.
The cached box, `GetObjectBox` gameplay consumers, Windows branch, object layout,
saved schema and simulation transform are unchanged.

**Falsified hypothesis:** the initial suspicion that `ComputeCorners` confused
instance and archetype IDs was disproved. Its original `HARCH` RAII owner resolves
and holds the instance archetype correctly. The existing material probe now
calls production `ComputeCorners` with deliberately divergent IDs and compares
the result with the actual renderer's origin-expanded bounds. This passes with
the unchanged `corners.h`; no Engine bounds-contract repair is claimed.

**Synthetic CPU execution:** the existing `game_material_manager_probe` checks
asymmetric authored bounds `{7,-3,1,-2,30,-10}` against the corrected bounds
`{3,-7,30,-10,1,-2}` within floating-point tolerance. The visible planar length is
40 rather than the raw model height 3, without added padding or scale. It also
checks the unchanged non-direct path/input and correction of genuine
`ComputeCorners` output. On Linux x86_64, GCC 16.2.1 Debug, Clang 22.1.8 Debug and
the full-media Clang sanitizer configuration each built
`frontierx_game_material_manager_probe`, `frontierx_app_gunboat_owner` and
`frontierx_app_build_support_platform_owner`, compiling the production
`DrawSelected` templates. Commands for each existing configuration were:

```sh
cmake --build "$build" --target frontierx_game_material_manager_probe \
  frontierx_app_gunboat_owner frontierx_app_build_support_platform_owner -j2
ctest --test-dir "$build" --output-on-failure --no-tests=error \
  -R '^portability\.game_material_manager.*$' -LE host --output-junit "$report"
python Tools/ci/check_ctest.py "$report"
```

The three existing CPU material variants passed in each configuration with zero
skips. Sanitizers used `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. Inputs were generated repository
fixtures; no retail asset was added. The subsequent full-graph builds failed:
nonship callers such as Waypoint, jumpgate and Trigger included `TObjSelect.h`
without the `CQEFFECTS` declaration supplied by ship callers. The native-only
include now explicitly imports its owning `DEffectOpts.h`; full-graph rebuilds
after that correction are pending at this entry. The earlier narrow passes prove
bounds math and those production templates, not the complete graph or execution
of the complete selection draw. Full-media production relinks, bounded actual
selection capture, native Windows Debug/Final and affected module-ABI checks
remain separate gates.

Final root integration: all five native graphs rebuild after the owning
DEffectOpts include; the final focused layout/batcher/material/native-shell
scope executes7/7 per configuration without skips or failures, including fatal
Clang ASan/UBSan with leak detection. Root retains the initial full-build failures
and the first selection input attempt that missed the actual ship; neither is
reported as a selection pass. Source waveform/channel, simulation bounds and
all original marker width/cap values remain unchanged. Windows/actual CI and
renewed user visual selection acceptance remain pending.

Final bounded Clang retail runtime: fresh original campaign entry executes the
Blackwell jump to system2, then a normal corrected-coordinate mouse click reports
highlighted=yes/selected=yes and plays original BLA5501.wav. Wheel zoom and
private normal/zoom captures execute, followed by normal close after594 source
frames (result4, audio0, idle hints, exit0). No presentation-exclusion flags are
used. This proves selected-source/render/input/close execution, not full corner
visibility acceptance or effect correctness; the user checks those next.

## Native planet hover opacity (2026-10-15)

**Concrete user regression:** planets must reveal otherwise occluded construction
slots when hovered. Source review finds the original `TestHighlight` mouse-over
owner and `IsRenderingFaded` contract, but no assignment to `alphaMod` in the
published source and commented-out faded-pass gates in ObjectList. The mouse-over
condition is independent of `BUILDARCHEID`; terraform transitions retain their
existing early return. Exact retail opacity/ramp has not been established.

The native Planetoid now initializes/reset its presentation alpha and applies
64/255 opacity while hovered. ObjectList restores its original opaque/faded pass
gates. A scoped native render-instance flag forwards opacity through the existing
Engine/RenderMgr/PolyMesh call, multiplying authored alpha and selecting source
alpha blending without depth writes for faded instances. Shared materials and
sibling instances remain unchanged. No simulation order, RNG, saved field, object
layout, virtual signature or Windows branch is changed. The provisional opacity
requires visual acceptance; this is not a measured retail fade-curve match.

The existing material probe represents this actual Engine/RenderMgr/PolyMesh
caller: it checks opacity160/0/255/repeat160, authored-alpha multiplication,
submission-time blend/depth state, adjacent ordinary draws and no shared material
mutation. Its existing GPU variant checks faded/zero/restored pixels and an
ordinary sibling. No separate probe or gameplay completion mechanism is added.

**Automated synthetic execution:** Linux x86_64 GCC16.2.1 and Clang22.1.8 Debug
full-media builds pass `ctest --test-dir "$build" -R
'^portability.game_material_(manager|gpu)$' --output-on-failure`, with
`SDL_VIDEODRIVER=x11` for actual host Vulkan execution (neither GPU result skipped).
The Clang ASan/UBSan build passes the existing layout, parser, app-schema, material
and shell checks with `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. Inputs for these checks are
repository-generated fixtures. Genuine-save host captures additionally show opaque/hovered/restored rendering
and renewed hover after normal F7/F8; the focused media entry records that bounded
retail execution. Construction-slot interaction, exact opacity acceptance and
native Windows Debug/Final/ABI remain distinct gates.

## Effect textures after direct ship draws (2026-10-15)

**Concrete user blocker:** intermittent ship/fire/explosion artifacts across
missions; a TM3 screenshot shows large flat ship-texture shapes, with possible
invisible fighters. A fresh original TM3 intro independently reproduces a flat
Mantis hull-atlas rectangle among normal explosions. The bounded debris observer
finds finite local vertices and rigid instance transforms in the sampled pieces;
no geometry-clamping or split simulation repair is justified by those samples.

**Confirmed owner defect:** Engine/PolyMesh and Optics bind textures directly to
PIPE. App RenderBatcher retains a separate `commitedTextureID` cache. A following
effect requests the same texture it previously used, so the batcher can skip the
bind even though PIPE now holds a ship texture. Turret/PlasmaBolt/Trail/anim2d
callers explicitly select their correct textures; substituting effect textures
or changing their geometry would hide the ownership fault.

The seven existing App batcher binding paths now reassert native texture
ownership, consistent with the already defensive native verify_state path.
Windows keeps its original cached comparison. Texture data, transforms, lighting,
shader selection, simulation/RNG, serialized data and public ABI are unchanged.
This is a shared rendering correction, not a mission-specific effect exclusion.

**Falsifiable regression:** extend the existing real App batcher runtime probe.
Effect A draws, a direct PIPE owner binds hull B, then the same BATCH A draws
again. Submission-time texture bindings must be A on both supported stages.
Immediate direct, indexed and pooled GetPrimBuffer/Release/flush paths each fail
against the preserved pre-fix owner (0/0/0), then pass with the repair. Existing
invalid-handle/state/transform checks and production assertions remain intact.
The first new fixture left unused stages2/3 unterminated; fatal ASan exposed
four original texture-validity assertions and early-exit leaks. Explicitly
disabling unused stages, as the real SetupDiffuseBlend caller does, repairs the
fixture; the corrected fixture still fails against pre-fix code. No assertion,
leak check or stage validation is weakened.

**Bounded retail-input runtime:** source TM3 intro capture before the repair
contains the rectangle; the subsequent native GCC full-media40-screenshot interval
contains original blasts/debris without that reproduced rectangle. This is not
a full cross-platform or determinism comparison. A separate Clang run loads the
user's unchanged genuine TM3 battle checkpoint. The original Scout carrier
launches two fighters; both are marked visible and prepare40vertices/60indices
with finite positions/UVs and source-consistent mesh/facegroup counts20/10.
That CPU observation alone proves neither GPU submission nor correct fighter
appearance or gameplay. The texture repair changes no Fighter gameplay code. A first wing observer failed by reading `this` in a static
helper and terminated its own debug run; the corrected read-only observer
completes normally. Later screen captures without active-window verification
are not visual evidence while the user's old game is also open. A follow-up
verified four battle captures, then stopped when its owned window lost focus.
The user subsequently confirms that the texture artifacts are gone, but fighters
remain invisible and Hive Carriers appear ineffective. A read-only debugger run
on the same genuine checkpoint reproduces a separate fighter flight fault:
launched fighters descend below Z-53000 and sampled firing attempts fail before
damage is queued. CPU batch counts do not establish GPU output or correct combat;
the fighter/control owner repair and renewed acceptance remain open.

Exact local compiler/build/CTest/sanitizer/host-GPU results and the retained
ordinary-compositor resize failures are in BUILD_GRAPH. Native Windows
Debug/Final/module ABI, actual CI, all-effect coverage and P1c remain open.


### Saved ship heading and Weaver construction facing (2026-10-16)

The user reports that the Weaver builds with its back toward the planet.
Source and lawfully external asset review identify the original Mantis
`Fabricator`, its authored mouth/crane on model -Z and its exhaust on +Z.
Production-math execution confirms the existing native hull correction maps
that mouth to -J, matching the original Mantis construction heading. Adding a
model-specific 180-degree flip would reverse the correct original facing.

The concrete defect is in `ObjectTransform::loadTransform`: `saveTransform`
stores `TRANSFORM::get_yaw`, which measures -J, but loading rotated an identity
by that heading as though it measured +J. A stationary building unit can keep
the reversed orientation after loading. Native loading now reconstructs the
saved -J heading from an identity orientation; position and the original
yaw-only save policy remain intact. The original Windows expression and all
stored fields/bytes remain unchanged. Existing checkpoints that already saved
an incorrect heading are not rewritten or inferred from a construction target.

The existing production material/math probe checks nine angles including zero,
both half-pi and both pi boundaries, three actual save-heading/load rounds,
stale animated pitch/roll removal, unchanged translation and the corrected
Weaver mouth direction. An isolated GCC16.2.1 execution passes; the same probe
with the original reconstruction formula fails at the heading assertion.
Linux x86_64 GCC16.2.1 and Clang22.1.8 full-media Debug rebuild the native shell
and material probe. The focused layout, app/script01/script02/script03/Sol_T
schema, material and font-enabled shell CTests pass. The corresponding fatal
Clang ASan/UBSan checks, plus parser/data-root/network-buffer checks, pass with
leak detection outside the sandbox. Visible Weaver construction/reload
confirmation remains pending. These checks prove the load defect, not every
animation or the user's entire observed construction sequence. Native Windows
Debug/Final/module ABI and hosted CI remain unexecuted.

The final full-media TM3 checkpoint run performs two real new F7/F8 rounds and
normal close. Targeted training in its accepted scope and full-media TM1 each
also perform ordinary F7/F8 and close successfully. Those runtime paths execute
the new load owner; only the mathematical regression measures exact restored
headings. The user's visible Weaver build-pose confirmation remains required.

### Large selection and sampler churn (2026-10-16)

**Renewed user feedback:** selected units still cause frame drops after the
bar and sampler changes below. A later actual TM3 checkpoint run selects ten
Missile Cruisers by normal hover and double-click, confirmed by the group HUD
and selection markers. Five complete active-selection intervals record
18.41 to 35.07 FPS, 204 to 246 draws/frame and 12.87 to 29.47 ms/frame in the
backend's presentation queue-submit call. The preceding unselected interval
records 58.95 FPS and 133 draws/frame. Combat continues between intervals, so
this establishes the reported slowdown, not a static same-frame comparison.
Pause hides the selection display and cannot serve as its rendering baseline.

The concrete upload-pool defect is in frame retirement. Old unused entries and
newly retired active entries shared one vector. Trimming its back after each
insertion repeatedly discarded the active entries while preserving unsuitable
old buffers. Actual selected frames reused only about 180 of 300 draws.
Retirement now reserves room for the complete current frame by trimming only
the old free entries first. The count bound grows from 256 to 512 because the
actual 18-unit group requires 294 to 322 draws; the 64 MiB byte budget remains.
Draw order, resource lifetime, GPU synchronization and all content remain intact.

An experiment batching copy command buffers and making texture uploads
asynchronous passed GPU image/lifecycle fixtures but did not resolve the real
selection slowdown. Its regular close also failed to complete within the
controller timeout. Both experimental behavior changes were removed before
the candidate; their fixtures alone were not performance or lifecycle acceptance.

The existing `--frame-stats` diagnostic additionally reports texture upload
CPU time and counts. The genuine checkpoint run records about two updates per
frame, costing about 1.5 to 2.5 ms/frame before this change. Those timings
include driver waits and are not GPU timestamps.

**Experimental GPU execution:** GCC16.2.1 and Clang22.1.8 full-media Debug builds execute
`portability.sdl3_gpu` and `portability.game_material_gpu` through the existing
explicit Hyprland/X11 host preparation. The existing map replacement regression
now queues 23 alternating uploads before readback. It requires the current
captured frame to remain byte-identical and the next frame to equal the original
reference for the final queued color. Existing captured draw, mip, sampler,
resize and repeated lifecycle assertions also pass. The pool regression now
additionally fills stale indexed entries, switches to 400 direct draws and
requires all 400 to be reused in the following frame with equal readback.
The final pool fix passes both GCC and Clang prepared GPU test pairs, including
all 400 active buffers reused and equal images. The pressure regression initially
left its 400-overlapping-draw scene active, correctly failing the existing later
retained-scene assertion. Running pressure before the original two-draw
growth/shrink checks restores their intended scene; no assertion is weakened.
**Final bounded retail-input execution:** Clang22.1.8 Debug, full-media Vulkan,
strict external font mapping and the original `--no-frame-limit` option load
an isolated byte-identical genuine TM3 checkpoint with ordinary F8. Normal
empty-area clicks and the saved group-1 key alternate twice; independent
screenshots confirm the real 18-unit group, markers, range circles and group
HUD. Five complete five-second intervals per selected phase average 57.24 and
55.97 FPS, versus 56.69 and 57.58 FPS in the corresponding unselected phases.
Selected phases average 270.89/274.67 draws and 270.87/274.66 reused uploads
per frame, with about 1.53 ms/frame in presentation queue submission. The
previous buffer churn is absent. Simulation, AI and combat continue normally;
these are reproducible input phases, not fixed-frame before/after determinism
or a universal FPS guarantee. No rendering or media is excluded.

The same run executes two ordinary F7/F8 rounds. Distinct new quicksave hashes
and modification times, subsequent load-completion markers, rendered
continuation and native exit 0 with audio streams zero/hints idle establish
fresh saves and normal close. The original user checkpoint remains unchanged.
Separate accepted-scope training and full-media TM1 runs each execute ordinary
F7/F8 and normal close with exit 0, zero audio streams and idle hints. Training
retains its previously accepted scope; its unsupported full-media dynamic-light
path is not newly accepted. These narrow regressions do not establish a full
mission run, every active mechanic or visible Weaver construction. User
confirmation, whole-process leak freedom, Windows/module ABI and hosted CI
remain pending. Raw checkpoints, hashes, profiles and captures stay external.

The user reports frame drops with many selected units and heavy weapons fire.
Source review identifies two repeated production costs. Native
`ObjectSelection::DrawHighlighted` submitted each four-pixel health/supply segment
through `DA::RectangleFill`, repeating its state setup. Native sampler changes
created immutable SDL samplers and waited for the GPU even when captured draws
retained earlier samplers.

The native 2D owner now submits each complete segmented bar together. Segment
spacing, partial final segments, inclusive rectangle edges, colors, vertex order,
pane offsets and toolbar hiding remain unchanged. Windows keeps its original
loop. The existing material probe compares actual production submissions:
one full-health/full-supply unit uses two rather than 40 draws; 40 units use
80 rather than 1600, with exactly the same 9600 vertices, colors and UV values.
Empty and partial lengths 0/1/5/6/99 retain the reference geometry. This is a
20-fold reduction in bar submissions, not a claim of 20-fold game frame rate.

The SDL backend now owns at most 32 immutable samplers, keyed by the five
supported address/filter properties and shared between texture stages. A state
switch borrows its sampler; shutdown releases each cached sampler once after
the existing GPU idle boundary. There is no per-switch GPU wait or sampler
release. Unsupported states still reject transactionally. The existing GPU
fixture exercises all 32 states twice across both stages, invalid-field rollback,
and captured draws retaining their original sampler bindings and readback.
No effect, texture stage, lighting, animation, simulation tick or RNG call is
removed or simplified.

**Local synthetic execution:** GCC16.2.1 and Clang22.1.8 full-media Debug material
and shell builds/checks pass, as do the focused fatal Clang ASan/UBSan checks
with leak detection outside the sandbox. Linked GPU readback and matched genuine
checkpoint measurements are recorded below. Synthetic bar counts
are not a substitute for a large-selection gameplay measurement. Native Windows,
hosted CI, complete audiovisual parity and whole-process leak freedom remain
unverified.


**GPU execution:** both GCC16.2.1 and Clang22.1.8 execute
`portability.game_material_gpu` and `portability.sdl3_gpu` without skips through
the existing explicit `gpu_host_setup.run` Hyprland/X11 host preparation,
`FRONTIERX_M3_GPU_SETUP=hyprland-x11`. The latter observes exactly 32 sampler
creations after repeated two-stage state sweeps, preserves invalid-request
rollback and captured-output readback, and completes resize and repeated
scene shutdown/reinitialization. No resize assertion is disabled. Ordinary
unprepared host runs pass the sampler/readback assertions but fail the known
tiled-window resize check with unchanged swapchain size; those runs remain
failures, not GPU passes. The explicit preparation floats only descendant test
windows and changes no persistent desktop configuration.


**Bounded retail-input profiling:** the existing `--frame-stats` counters run
without per-draw debugger stops on Linux x86_64 Clang22.1.8 full-media Debug
Vulkan, using the original `--no-frame-limit` option at normal game speed.
In seven complete five-second fresh TM3 intervals before F8, frame-weighted
sampler CPU time changes from 2.164 to 0.081 ms/frame. Corresponding average
submission counts are 139.710 and 139.802 draws/frame, and sampler-switch counts
are 38.797 and 39.108/frame. Both runs render all configured content. These are
bounded comparable-source-scene measurements of this owner, not a heavy-fire
or large-selection overall-FPS claim; presentation remains near its display
limit. A true checkpoint copy also loads and continues normally with the repair,
then closes with zero mission audio streams and idle hints.

Automated attempts to produce a large-selection scene instead move the camera
outside the fleet and leave an empty selection panel. They are explicitly
rejected as large-selection/performance evidence. The synthetic 40-unit geometry
and draw-count regression remains valid. Actual large-selection and heavy-fire
perception/FPS need the short user-operated continuation from the genuine
checkpoint; no full mission replay or inferred acceptance is required. Raw
profiles, unsuccessful attempts, captures and checkpoints remain private.


**Training baseline boundary:** a full-media fresh training attempt rejects the
existing unsupported dynamic-light pipeline path before play. The preserved
pre-change Clang binary rejects the same path. This is not a new sampler/bar
regression or a full-training pass. A separate 1.5-second fixture-mode attempt
in the documented bounded training scope reaches rendering/save/reload but
correctly rejects its unmet moved-Forger/objective precondition; no normal
selection/movement had been supplied. That attempt is not a pass either. The
ordinary user-mode start/render/normal-close regressions below use the accepted
bounded training scope and full-media TM1 respectively, retaining all assertions
and without manufacturing objective completion.


**Ordinary-path targeted regressions:** current Clang22.1.8 full-media Debug
executes accepted-scope training start/render/normal-close for 102 source frames
and full-media TM1 start/render/normal-close for 100 frames. Both return native
0 with zero mission audio streams and idle hints. These short fresh starts do
not exercise selection, combat, animated-portrait playback, mission victory or
full training/campaign acceptance. Training retains only its already documented
media/render exclusions; the Terran handoff has none.


### Weaver root animation model basis (2026-10-16)

The user's screenshot disproves the earlier inference from a negative-Z root
sample to correct construction facing. The native ship renderer's model-axis
correction also has to compose with authored Object-root motion. A per-script
root basis now converts Weaver `Sc_build` at the Animation owner, leaving
Joint motion and unconfigured callers unchanged. The existing material probe
checks the complete rendered matrix, planet-facing anchor, child pose,
translation and repeated script ownership. GCC/Clang and fatal sanitizers
execute it; a genuine same-camera F8 before/after run shows the corrected tilt.
Exact owner, target and remaining acceptance limits are recorded in
[the media follow-up](NATIVE_TERRAN_MEDIA.md#weaver-screenshot-follow-up-2026-10-16).
