# M3 simulation, save and compatibility comparison

Status: **2026-10-13, NOT ACCEPTED** (work started2026-10-12). CI has not executed this work. Local
Linux and SSH native Windows results below are supplementary evidence only.
The restored Windows executable is an implementation under test, not an
unqualified retail oracle. The user's mission-conversation failure remains
open; no Linux behavior is changed to imitate it.

## Baseline audit and source provenance

The initial Linux worktree was clean at
`516b842820776b0699e270fcb37e8a77c4dc5c77`. This work is an uncommitted
working-tree overlay; no commit, branch, push or publication was performed.
The supplied SSH machine has a clean older checkout at
`7c5aa450ba531b1abaee31d0a2639561df29deba`; it was preserved. Builds use a
separate private source export of the Linux revision plus this overlay.
Source updates were transferred without retail Data/audio, executables,
DLLs, PDFs or dumps. Retail inputs remain outside both source checkouts.

M2 evidence was checked before implementation, including the external
`m2-regular-close-final` build/test summary and GCC/Clang/ASan logs, not just
the acceptance prose. GCC and Clang each return original victory2, then
restarted-mission quit4, with audio-stream count0 and original hint owners
idle on both closes; exit0. ASan executes those closes but fails whole LSan
with3156 bytes/7 external allocations. The full fresh GCC and compiler-own
Clang continuation/user completion ledgers are historical gameplay evidence,
not newly replayed identical inputs. Their differing end times and input
journals do **not** establish determinism. M2 remains accepted in its agreed
Linux scope; Windows/retail, comparative determinism and host leaks were not
retrospectively counted as passes.

## Bounded original-owner protocol

The current **diagnostic** protocol is a fresh `Sol_Train.dmission` load,
source game-active transition, observation of every original ObjectList
update count0..1024, and these commands:

| Boundary | Original operation |
|---|---|
| Before update count32 | `MScript::SelectUnit(GetPartByName("Forger"))` → original highlight/selection owners |
| Before update count64 | `MScript::OrderMoveTo(Forger, TriggerPoint, false)` → original USRMOVE/NetPacket/OpAgent owners |
| After count1024 and scheduler update | Post original `CQE_PLAYER_QUIT`; no mission success or objective setter |

The commands are issued once at the specified boundaries in the unpaused
source loop. Original script evaluation runs on its existing eight-count
cadence; command submission and processing are different boundaries.
Rendering, the frame clock, live audio and original event delivery are
retained. Linux uses the supported M2 media/render exclusions, with training
speech, subtitles and unit voices enabled. It loads directly through
`Mission::Load`, omitting the selector/briefing as a bounded common start.
Windows `FRONTIERX_M3_SOLTRAIN=1` selects the same original load owner.
`FRONTIERX_M3_TRACE` selects a **private external output file** and enables
this command protocol. Without that variable, the observer does not issue
commands or request quit. `FRONTIERX_M3_JOURNAL` independently enables private
owner journals. These are test access points, not ordinary launcher defaults.

`M3Trace.h` records mission-backed objects through original `GetMissionData`
and `GetTransform` contracts: stable mission ID, class, owner, system, XYZ,
hull, supplies, selection, readiness, group and pending/completed operation
IDs. Objectives are read from original MGlobals list/completion/failure/
secondary accessors. Agent count and player gas/metal/crew are also read.
Five existing scalar accessors gain Windows export visibility; no field,
vtable slot or saved layout is added. Full native rebuild/ABI checks cover
that additive symbol change.

Coverage is deliberately marked **partial**. Queued order payloads,
script-program state, non-mission objects, full player state and relevant
CRT/custom RNG state are not yet exported semantically. A source tick
snapshot is taken after `SetUpdateCount`/`SCHEDULER->UpdateQueue`, before the
outer frame's physical update: that boundary is not a complete physical
simulation state boundary. The comparator refuses acceptance even if all
observed fields match.

Normalization: only JSON object-key order is ignored. Source object/list
order is retained. Each SINGLE is exported as exactly eight hexadecimal
IEEE754 bit digits via `memcpy`; XYZ, signed zero and all bits are compared
exactly. No pointer, structure padding or raw runtime-memory range is read.
The private observer writes can perturb the measured wall-clock frame duration;
this is another reason these traces are diagnostic rather than proof of a
controlled deterministic initial state/event schedule. No rounding, tolerance, coordinate projection, ID renumbering or omitted
Z comparison is used. Missing ticks, duplicate keys/IDs, malformed fields,
nonfinite JSON numbers and unsupported coverage fail explicitly.

The full protocol still needs original-owner saves at agreed complete
boundaries256 and512, immediate source reload and continuation to1024
against an uninterrupted run. These points are **specified, not implemented
or executed**. A save taken inside an ObjectList iteration would be unsafe;
forcing one frame per tick or silently shifting to the next frame would
change the experiment. Resolve the frame/event boundary first.

## First divergences: measured, not normalized away

1. Two focused executions of the **same optional Clang Debug binary** first
   differ at count5: mission ID57281, class103 (`M_TAOS`),
   `position_f32[2]`. One value is0, the other approximately-0.50983524.
   Count0 and the preceding captured states match. Each reaches1024 and
   original objectives1/2 through the commands, but the existing native-shell
   diagnostic later exits7 on its two-unit-voice requirement: the scripted
   movement path produced only one unit-voice start. That assertion is retained;
   these are failed diagnostic processes, not passed M3 runs.
2. A diagnostic GCC/Clang comparison also first differs at count5 in that
   field. Because same-build repeatability already fails, this is **not**
   isolated evidence of a compiler defect.
3. Native Win32 MSVC and Linux CRT characterization with `srand(1)` differs
   on the first actual `rand()` draw: Windows41, Linux1804289383; `RAND_MAX`
   is32767 versus2147483647. GCC/Clang CRT traces match. The probe characterizes
   the real library boundary used by MGlobals/OpAgent/RandomNumGenerator;
   it is not a replacement mission simulation or proof those draws alone
   caused a mission divergence. `RandomGen::rand()` also has `RAND_MAX` as
   its default modulus; matching its internal recurrence would not suffice.

Responsible source paths for the first physical difference:
`ObjectList::BeginFrame` measures wall-clock elapsed time and groups AI
substeps; `ObjectList::engine_update(dt)` calls original `PhysicalUpdate(dt)`;
`ObjectFrame::FRAME_physicalUpdate`, `ObjectControl::physUpdateControl` and
`ObjectPhysics::physUpdatePhysics` update velocity and the returned transform,
including Z. The latter performs `x += velocity * dt`. Physical integration
runs outside the captured AI substep and is visibility/default dependent.
This is a shared source/frame-boundary repeatability blocker. Its gameplay
impact and any port-specific contribution remain unresolved. It is not
classified as harmless presentation and has not been “fixed” by changing
physics, game speed, assertions or a comparison tolerance.

The CRT difference is a confirmed platform-library contract difference.
Audit ownership/seed and consumption before choosing a compatibility policy:
Windows modules use static CRTs, `Trim::DllMain` calls `srand(time(NULL))` and
`rand()` on DLL notifications, while the native closure shares a different
process composition. No seed override, RNG replacement or save-RNG repair
is introduced here. Initial state is therefore **not yet proven identical**.

## Speech and restored Windows defects

Original `Sol_T` repeatedly gates transitions on `MScript::IsStreamPlaying`,
including briefing, multiple selection, scouting, construction and success.
Disabling speech or returning false immediately changes the script schedule.
No such shortcut is used for the comparison.

Private journals separate these observations:

| Stage | Instrumented owner / current evidence |
|---|---|
| Script request/result | MScript animated-message request and returned sound ID; source `IsStreamPlaying` result logged without replacing it |
| Resource resolution | NativeStreamer actual UTF/file resolution; Windows SoundMan animation instance/interface and lip-script resolution/parse |
| Decode | NativeStreamer actual optional IAC decoder result and PCM byte count; Windows decoder result remains unobserved separately from stream initialization |
| Playback | Native SoundMan successful original streamer Restart; Windows initialization notification is observable, explicit decoder/playback characterization still open |
| Completion | Native streamer notification and SoundMan completed-status removal; Windows `handleStreamerNotify` status and original script polling |

The bounded Clang journal actually resolves/decodes/starts STMG03, STMG04,
STMG05 plus a Forger unit voice. STMG05 reaches original completed status4
and script polling changes to false. Earlier training streams are interrupted
through the original objective transition. This is source/audio-owner runtime
evidence, **not** a new audible/retail comparison. Portraits are excluded on
Linux, so Windows lip/portrait dependency behavior must not become Linux's
expected failure. Journals contain bounded resource basenames and scalar
status only; raw media, pointers and private paths are absent. Journal
streams are separate per owner; their IDs/statuses are not a complete
cross-owner event-order replay contract.

The user-reported Windows mission-conversation failure is **open and not yet
reproduced on an interactive Windows runtime in this work**. Cause/stage,
other Windows functional failures, shared source errors and port deviations
must remain separate. Observed matching builds would still not prove retail
parity. Reference correct behavior with an original retail executable on
lawful data and documented format contracts, not a broken restored result.
No expected-failure suppression is installed.

## Shared toolbar error-close leak (separate repair)

The new instrumented diagnostic actually reloads, then exits through its
retained voice assertion. LeakSanitizer reports103412 bytes/10 allocations,
including a direct99840-byte App `Menu_tb::loadInterface` map. This is **not**
classified as an external driver leak. `AppGlobalCleanup` sends the original
WM_CLOSE before Mission::Close; the toolbar destructor flushed controls but
never freed its owned map, whereas normal unload did. The destructor now
releases/nulls that same array as a fallback; an earlier ordinary unload has
already nulled it. No interface, saved field, gameplay or assertion changes.
This is a shared UI ownership defect exposed by an error-close path, separate
from port comparison and the existing M2 host leaks. The same bounded ASan diagnostic is repeated after the owner fix: the direct
99840-byte App root disappears, while the retained voice assertion still fails
and whole LSan still reports3812 bytes/12 allocations with external/unresolved
frames. No invalid-memory/UBSan error is observed in that bounded path. The
remaining roots are not automatically relabeled as third-party or suppressed;
no leak-free/full-lifecycle/M3 success is inferred.

## Saves and local execution

Focused live Linux commands used existing optional Debug trees and lawful
Steam data. Direct original load + the command schedule captures0..1024.
The native shell then calls original `Mission::Save("native_slice.mission")`
and `Mission::Load` and observes the same moved Forger owner/XYZ and original
completed1/2. This existing bounded save check executes in the new diagnostic
runs, but is **not** an exact256/512 save, semantic whole-save comparison,
byte-equality test or reload-versus-uninterrupted continuation. Overall
process exit7 remains a failure. No immediate-load check is substituted for
continuation.

Semantic save equality will compare the original owners' persisted field
contracts, ordered operations and script state after load. Actual byte
identity is a separate requirement for fixed-format records and any selected
stable archive contract. There is no blanket demand for whole-container byte
identity and no undocumented timestamp/padding suppression. Establish and
record a save-member inventory before implementing normalization. No save
format was changed by this work.

Initial concurrent/focus-lost attempts time out and are unusable whole-run
proof. Focused repeats are sequential and use the real desktop/event loop.
The CI driver requires a controlled active GPU host; it does not silently
force focus, skip missing data/GPU or replace timing with a headless model.

## Native Windows build repairs and evidence

These build repairs are separate from comparison/gameplay work:

- Application `/Yu` skipped opening platform guards before `pch.h`, causing
  unexpected `#else`. The shared MSBuild target force-includes the project's
  same PCH on `Use` entries before source parsing, including the default pch.h name; creation/non-PCH entries
  and compile flags remain intact.
- A stray leading `f` in Listbox became visible; removed it.
- Win32 `min`/`max` macros expanded NetBuffer/ParticleEffect qualified calls and TextureManager numeric_limits; parenthesized the same operations.
- S32 is Windows `long`, so UnitComm's four `std::max` calls with int constants
  could not deduce one type; cast the same positive constants to S32.
- Manual `_DEBUG` plus supported `/MT` pulled debug STL directives into
  release-CRT application objects. The App PCH primes STL under that release
  CRT contract, then restores `_DEBUG` for all game assertions. No assertion
  is removed or gameplay condition altered. Layouts/allocator policy remain
  unchanged; affected native modules are rebuilt together.

Native SSH host: Windows11 Pro10.0.26200, VS2022 Community MSBuild
17.14.51.32402, v143 MSVC14.44.35207, Python3.14.3, installed June2010 DirectX
SDK (`d3dx9.h` present). **Debug and Final full `Build-Conquest.ps1
-Configuration ... -NoDeploy` wrappers executed successfully**, with zero
build errors after repairs. `Tools/abicheck/allmods.ps1` executed immediately
for each configuration before shared campaign outputs were overwritten:
153 Mission and2 Trim demanded symbols, zero unsatisfied imports. Existing
Win32 compilation layout assertions also compile. These are same-source
build/module results, not retail ABI or gameplay parity.

The SSH process runs in Windows session0. A separate inventory confirms an
active user console session1 and NVIDIA GeForce RTX3050Ti Laptop GPU,
driver32.0.15.9647. The host-runtime job rejects direct session0 execution.
A temporary controlled interactive scheduled task can use the existing
console session without credentials or changing the original checkout/data;
its failed native startup outcome is recorded below. No service/session0 build
result is substituted for GPU/audio execution.

## External input provenance

The user specified an external Steam installation on Windows; Linux uses
its existing lawful Steam installation. No installation path is committed.
These files were checked on both machines and are byte-identical:

| Logical input | SHA256 |
|---|---|
| stringpack.db | `0f35826dfc62592c621d0efc9ea384d700679b7ca2c6d53f84b1e328b68e3df5` |
| gendata.db | `adfa34d58f22163665ec0e9973a7d8e69a82166f265b22416ea2ff74e89a01c3` |
| gametypes.db | `20a454b8269140d824b3a42ceac0412fbd31085550275ab4ab8a2c64bf173741` |
| bauhmib.ttf | `c13cf63ab70ccacbb6e967ae448d960b3606029043995701578d48ece2ebb9a1` |

The packed maps, objects, mission speech, unit speech and textures archives
were also hashed on both machines: all five are byte-identical. Logical
archive lookup matches only the supplied basename case-insensitively and
rejects ambiguity; file bytes are hashed without normalization. Private root
paths are excluded. Identical maps archives contain identical mission bytes;
actual selected-member loading still needs Windows runtime evidence. CI
mission drivers generate the same bounded private manifest with
`Tools/ci/input_manifest.py`. Other resources/profile/configuration and full
initial RNG/state identity are not yet established. Original Windows executable hash was read privately for future
retail-reference identification; no copied executable or dump is committed.

Linux toolchains: GCC16.2.1, Clang22.1.8, CMake4.4.3, Python3.14.7,
SDL3.4.16, optional external reviewed shared LGPL decoder and FreeType2.14.3.
Both default and optional GCC/Clang graphs and optional Clang ASan/UBSan are
built/tested locally; central counts belong in [BUILD_GRAPH.md](BUILD_GRAPH.md).
ASan/UBSan flags: `-fsanitize=address,undefined -fno-omit-frame-pointer`;
CPU CTest runs have leak detection and halt-on-error enabled. SDL/GPU/driver,
FreeType and external decoder internals are not instrumented. This does not
turn whole-process host leak failures into passes.

## Prepared CI and infrastructure blockers

[Workflow](../.github/workflows/m3.yml) uses `workflow_dispatch` only, pinned
actions, read-only contents permission, reviewed default-branch gating and
the `frontierx-m3-controlled` environment. This workflow has no PR or pull_request_target trigger. Environment
approval alone does not protect a runner from another workflow requesting its
labels. Before registration, enforce a runner-group allowlist for reviewed
workflows/repository access, or use a separately controlled single-job worker
with no untrusted queue exposure. If that policy cannot be enforced on this
repository/account, persistent retail-access runners remain blocked. Configure
environment branch restrictions and required reviewer approval as an
additional control. These server-side protections are not created by YAML. GitHub documents the
workflow allowlist under [runner-group access control](https://docs.github.com/en/actions/how-tos/manage-runners/self-hosted-runners/manage-access).

| Job | Mandatory scope / runner |
|---|---|
| linux | GCC/Clang default, optional and ASan/UBSan configure/full build/CPU CTest; `frontierx-m3-linux` |
| linux-host-gpu | Explicit existing production GPU checks under both compilers; `frontierx-m3-linux-gpu` |
| linux-host-mission | Original Sol_Train sequential repeats and strict comparison under both compilers; same controlled GPU label |
| windows-native | Native Debug/Final full rebuilds, immediate per-build ABI checks and CRT repeat; `frontierx-m3-windows` |
| windows-host-mission | Native original mission entry/repeat, private runtime and strict comparison; interactive `frontierx-m3-windows-gpu` |
| crt-cross-build | GCC/Clang first, then Linux/native Windows synthetic CRT contract; known difference remains a failing gate |
| acceptance | Explicit failure while required semantic capture/save/continuation and reference gates remain incomplete |

`Tools/ci/check_ctest.py` rejects absent testcases, failure, error and skip.
CTest registers the CRT repeat, comparator and execution-gate regression
checks. Existing GPU CTests carry `gpu;host` labels; CPU exclusion does not
satisfy GPU acceptance. Only16 synthetic CRT draws/RAND_MAX/seed/schema may
be uploaded. Build/test logs, journals, captures, saves and external runtime
files stay in private runner temporary directories, without caches/uploads.
The Linux mission driver retains real presentation/audio. Its current
coverage and process failures are deliberately fatal; the Windows driver
rejects session0 rather than reporting a skip/pass.

**Actual CI execution: none.** The checkout has no previous workflows.
`gh` has no authenticated session. The installed GitHub connector does read
this private repository: default branch `main`, user-owned repository,
remote `.github` contains only instructions, and Actions reports0 workflow
runs. The exposed connector has no runner-administration endpoint. Actual
registered-runner inventory and access policy therefore remain unverified;
admin-capable local API access is still needed for registration/policy checks. On the supplied
Windows machine no Actions runner service or conventional runner installation
was found. No registration token was obtained and no runner registered.
No workflow has been pushed or dispatched. The first controlled interactive
Windows runtime-driver attempt rebuilt Final and failed before producing a
tick trace. Its initial staging omitted installed provider DLLs from the
external `DLL` directory and the newly built implicit DACOM owner; it also
used the full checked-in profile with unavailable optional movie providers.
This is a **test-runtime composition failure**, not a reproduced mission
conversation failure or an unavailable GPU. The driver now stages those
external providers privately, replaces DACOM with the matching built owner,
uses a font junction, and omits only the same full-screen-movie providers
excluded by the Linux protocol. The wrapper additionally rebuilds the source
VertexBufferManager, MaterialManager, ParticleEffect, StringTable and
TextureManager providers already required by the documented Windows baseline.
The driver reuses `Invoke-RestoredGameValidation.ps1` for its original
DataOverlay/Mat junction and temporary DataPath restoration, selects the
previously validated D3D9 Rend0 profile, and adds explicit process timeout/
nonzero failure handling. Native parser and synthetic utility checks verify
success, nonzero exit, timeout, outside-checkout rejection and exact DataPath
value/presence restoration. These utility checks are registered in the native
Windows CI driver and are not mission evidence. The corrected repeat ran in interactive session1 against the latest Final
14-provider build. It fails before any tick trace or script journal:
Conquest exits-1073741819; Windows Application Error reports access violation
`c0000005` in the installed retail `dahotkey.dll`, offset`0x18be`. Before that
fault, the original startup log records first `IRenderPipeline [Rend0]`
creation failed, then multiple required provider creations failed. All tested
runtime modules are native x86 PE images. The owning path is
`SystemContainer::LoadSystemComponents` → DACOM/factory creation; the Windows
branch currently continues after required creation failure. That first
failure's registration/descriptor/module cause remains unresolved. The later
hotkey crash is not yet the root cause, nor a reproduced dialogue defect.
This restored-runtime/module-composition failure is an explicit M3 blocker;
no Linux failure is introduced to match it. Temporary interactive tasks were
removed, and the helper restores DataPath on the observed failed process.
No original installation or existing checkout is overwritten. Local repeat
uses the already verified Final build; the CI driver still rebuilds by default.
No CI result is inferred.

Next infrastructure actions, requiring the user's requested authorization:

1. Provide an authenticated local GitHub administrator session (not a secret
   pasted into chat); inspect actual repository runner inventory and create
   the protected environment/default-branch restrictions and runner access policy.
2. Register controlled Linux build and interactive GPU runners and the supplied
   native Windows build runner with the documented labels; registration needs
   the repository's short-lived registration token and appropriate permission.
   The Windows GPU label additionally requires an actual interactive desktop
   runner and working GPU/audio, not the current SSH session0.
3. Authorize commit/push of the reviewed overlay, then dispatch that revision.
   Record workflow run/job IDs and results. Until then, local results cannot
   fulfill the user's mandatory CI criterion.

The implementation blockers remain independently visible: frame/physical
state boundary repeatability, CRT/RNG ownership and state, complete semantic
script/order capture, exact save/reload continuation, remaining input/state identity, native
Windows speech-stage characterization and correct retail reference behavior.
No M3 success is claimed; visible image/input and audible comparisons remain
additional required evidence after automated core gates.
