# M3 simulation, save and compatibility comparison

Latest M3 evidence (2026-10-13): **NOT ACCEPTED**. Completed protected CI
[run38426508626](https://github.com/Michael2543/frontierX/actions/runs/38426508626)
at `b24115686c0f733f3d22a7ee852f638dd26cd205` fails overall. Its six Linux
CPU scopes and both explicit GPU jobs pass; native observer export, mission,
sanitizer and comparison gates fail. Follow-up
[run38439947717](https://github.com/Michael2543/frontierX/actions/runs/38439947717)
at `b6ca3dd2051703764cbf0144516bce22b6442f16` is still running. Both GPU jobs
pass; native Debug/Final rebuilds and ABI execute successfully, but the job
fails afterward in the launch-helper regression. Windows mission is skipped.
GCC measured mission sanitizers expose an unaligned Planetoid sync store after
the preceding OpAgent correction. Working-tree follow-up is supplementary,
not executed CI proof. The user confirms the separate Steam executable is
unmodified retail; its runtime has not yet been compared. Retail behavior is
the preferred corroborating reference, rather than the restored conversation
failure. Full state/RNG, repeatability and Save/Reload continuation remain open.
See the current follow-up section below.

Earlier updates below are dated historical snapshots.

## Baseline audit and source provenance

The initial Linux worktree was clean at
`516b842820776b0699e270fcb37e8a77c4dc5c77`. The user subsequently committed
and pushed the prepared work as `09d3c7d86a39bc900afc746207d7c2e6bffdf288`,
verified on the private repository default branch on2026-10-13. The user subsequently explicitly authorized committing, pushing and running CI.
Related changes are now pushed through118efae; the initial worktree and
the older Windows checkout were preserved.
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
satisfy GPU acceptance. Only synthetic CRT and fixed-seed production RandomGen draws/contract metadata may
be uploaded. Build/test logs, journals, captures, saves and external runtime
files stay in private runner temporary directories, without caches/uploads.
The Linux mission driver retains real presentation/audio. Its current
coverage and process failures are deliberately fatal; the Windows driver
rejects session0 rather than reporting a skip/pass.

**CI snapshot before runner setup:** no execution. The user-pushed revision
contains the workflow. The latest actual run is recorded in the follow-up below.
`gh` has no authenticated session. The installed GitHub connector does read
this private repository: default branch `main`, user-owned repository,
remote `main` is revision `09d3c7d`, and Actions still reports0 workflow
runs on the follow-up inspection. The exposed connector has no runner-administration endpoint. Actual
registered-runner inventory and access policy therefore remain unverified;
admin-capable local API access is still needed for registration/policy checks. On the supplied
Windows machine no Actions runner service or conventional runner installation
was found. No registration token was obtained and no runner registered.
The workflow was pushed by the user, but no executed or queued run is reported. The first controlled interactive
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
failure was resolved locally in the follow-up described below. The later
hotkey crash is not yet the root cause, nor a reproduced dialogue defect.
The historical restored-runtime/module-composition failure did not establish parity;
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
3. The user has committed/pushed `09d3c7d`. Dispatch the protected workflow
   once runner access and inputs are verified; publish any further reviewed
   overlay only with explicit authorization. Record workflow run/job IDs and
   results. Until then, local results cannot
   fulfill the user's mandatory CI criterion.

The implementation blockers remain independently visible: frame/physical
state boundary repeatability, CRT/RNG ownership and state, complete semantic
script/order capture, exact save/reload continuation, remaining input/state identity, native
Windows speech-stage characterization and correct retail reference behavior.
No M3 success is claimed; visible image/input and audible comparisons remain
additional required evidence after automated core gates.

## Follow-up at user-pushed revision09d3c7d (2026-10-13)

Evidence: source review and supplementary native Windows execution, not CI.
The user confirmed that no runners have been registered. The supplied SSH
host also reports no Actions runner service. Runner registration, desktop
sessions and the repository-wide runner access policy remain infrastructure blockers.
Local `gh` remains unauthenticated and the installed connector cannot dispatch
workflows or administer runners. Commit/push is complete; runner administration
and actual workflow execution remain separate prerequisites.

The original DACOM owner journal now records bounded interface names, descriptor
sizes, registration counts and factory results, with no pointer/path/media
capture. It establishes the first startup failure: `LoadAllFromPath` scans the
application directory twice when `DACOM` is unset or resolves to that directory.
The second `AddLibrary` unregisters the first factory. `DACOM_GetDllVersion`
leaks its `LoadLibrary` reference when version metadata is absent, so the
second load does not invoke DLL_PROCESS_ATTACH and re-register it. Observed
`IRenderPipeline`, `IViewConstructor` and `IMaterialManager` provider counts
fall to0. This is a Windows-specific source-owner defect, separated from
Linux port changes and the unresolved reported conversation defect.

The owner fix avoids scanning the application directory twice and releases
module references and version buffers on all version-query error paths.
Explicit `AddLibrary` replacement behavior, interface layouts, registration
priority, gameplay, assertions and saves are unchanged. MSBuild17.14 /
MSVC14.44 native Win32 DACOM Debug and Release rebuilds pass. The new
`dacom_registry_probe.cpp` exercises actual built DACOM and D3DRenderPipe
providers without retail input or a graphics device: unset, relative-dot
and absolute application-directory searches retain exactly one production
IRenderPipeline factory and release its module after shutdown. All3 cases
fail with the original09d3c7d Release DACOM and pass with the owner correction
under both Debug and Release (6 successful actual-provider executions).
`windows-native` now compiles and executes this regression; that job has not
run. Script syntax checks pass for both changed Windows CI drivers.

The interactive corrected startup now creates all14 configured system
components successfully. It still produces no mission tick trace: private
window inspection identifies the original missing `Scripts` directory
dialog, not a reproduced conversation fault. The automated driver previously
flattened campaign DLLs into the runtime root; it now stages them in the
original `Scripts` directory as `Build-Conquest.ps1` does. Failure details are
retained in a private runner file, with only a bounded stage printed publicly.
The next actual run reaches all1025 records (counts0..1024), with original
script requests at0/32/72 for STMG03/04/05. All3 lip resources resolve/parse
and streams open;4 original completion notifications are observed. Separate
decoding, playback and audible behavior remain unverified. On original quit,
the process exits-1073741819; Application Error again identifies the installed
retail `dahotkey.dll`, exceptionc0000005 at offset0x18be. The driver correctly
fails the first process and does not claim a repetition or cross-build pass.
A controlled follow-up rebuilds the source DAHotkey owner, preserving the
installation and all assertions; the wrapper now includes that required
provider in both native configurations (15 explicit provider projects). Both
source DAHotkey configurations compile successfully with NoDeploy. With that
matching source provider, both actual Windows Sol_Train processes reach all
1025 records and close with exit0. The strict comparator fails at
`$[3].units[40].position_f32[2]`: count3, missionID57313, class98, owner1.
No position normalization or tolerance is introduced. This same-platform
repeatability failure must be resolved before cross-platform conclusions;
the shared wall-clock/physical-frame boundary remains the first owner to
control. Both runs resolve/parse/open3 training speech resources and emit4
completion notifications, but decode/playback/audible parity is still open.
Replacing the installed provider with the matching built source removes the
observed close crash in these2 runs; the exact installed-module defect is
not inferred, and the reported Windows conversation issue remains open.
Temporary tasks and validation game processes are confirmed absent afterward. The first Release attempt omitted
NoDeploy and failed at its legacy nonexistent build-drive copy command;
repeating with explicit NoDeploy passes. This was a tooling failure, not a
passed full wrapper rebuild. Provider registry regressions do not prove
mission, speech, save/reload or retail compatibility.

## Runner registration handoff

Follow-up2026-10-13: the user installed/configured the Linux runner outside
the repository. Its local non-secret configuration identifies agent
`froniterX-linux`, group `Default`, and the intended private repository.
No Runner.Listener/Runner.Worker process is running. No credentials were read
or transferred and the agent did not start/register a runner. Server-side
registration status, labels and access restrictions still require an
authenticated administrative check; local `gh` remains unauthenticated.
Actions still reports0 workflow runs. Windows runner registration remains
missing. Installation/configuration does not establish CI or GPU evidence.

The GitHub registration screen is repository **Settings → Actions → Runners
→ New self-hosted runner**, selecting Linux/x64 or Windows/x64. Execute its
download/configuration commands on the corresponding host, not in the source
checkout. The displayed registration token is temporary and must not be
pasted into chat or committed. See [GitHub registration instructions](https://docs.github.com/en/actions/how-tos/manage-runners/self-hosted-runners/add-runners).

Required custom labels, in addition to automatically assigned OS/architecture:

| Controlled machine | Labels | Required execution environment |
|---|---|---|
| Linux build and desktop runner | `frontierx-m3-linux`, `frontierx-m3-linux-gpu` | GCC/Clang, CMake/Ninja, SDL3/Vulkan, reviewed optional shared decoder/font; actual desktop/GPU/audio for host jobs |
| Native Windows desktop runner | `frontierx-m3-windows`, `frontierx-m3-windows-gpu` | VS2022 Win32, external DXSDK, Python, lawful external installation; active console session/GPU/audio |

One runner per machine can carry both labels and serialize that machine's
jobs. For the Windows GPU label, use the runner from the logged-in desktop
(`run.cmd`); a Windows service/SSH session0 fails the explicit mission check.
The Linux desktop runner likewise needs the actual display/audio environment.

**Resolve runner access policy before exposing either retail-capable host.**
A label and protected environment in this workflow cannot prevent another
workflow from requesting the same runner. Use an organization runner group
restricted to the selected repository and exact reviewed default-branch
workflow where that policy is available; otherwise use a separate controlled
private CI repository with no untrusted PR jobs and access restricted to its
trusted operators. The current personal repository has no established such
policy. No organization/repository transfer, CI repository creation or token
registration is authorized/performed by this handoff. See [GitHub runner
access policies](https://docs.github.com/en/enterprise-cloud@latest/actions/how-tos/manage-runners/self-hosted-runners/manage-access).

After the policy and local-only input configuration are verified, dispatch
**M3 controlled compatibility gates** from the **Actions** tab on the reviewed
revision. A queued job, skipped host test or failure of the current explicit
acceptance blocker is not M3 acceptance. The user-pushed09d3c7d workflow is
available, but this follow-up owner/tooling overlay still needs separately
authorized publication before CI can exercise it.

Focused follow-up CTest execution: existing `portability.rng_contract`,
`portability.m3_compare` and `portability.m3_ci_gates` all pass under the
existing GCC and Clang default graphs, with0 skips. These are contract/tool
regressions, not deterministic mission acceptance. The new15-provider full
Debug/Final wrapper and complete core comparisons still require real CI.

## First actual CI run and access-policy verification (2026-10-13)

GitHub administrator access is now authenticated locally. API inspection
confirms repository privacy, only the owner's collaborator account, disabled
fork-PR workflow execution, and the Linux runner's2 required custom labels.
This establishes a current trusted-operator boundary, not an organization
runner-group workflow allowlist. Recheck it before granting any additional
repository access, adding PR workflows or exposing another runner.

The `frontierx-m3-controlled` environment now exists and its only deployment
branch rule permits branch `main`. Repository Actions settings require full
commit-SHA pinning, permit GitHub-owned actions (no verified-marketplace
allowance or extra patterns), give the default job token read permissions
and disallow approving PR reviews. Fork workflow execution remains disabled;
no write-token or secret/variable forwarding is enabled. These settings were
applied and read back through the official API. They complement the reviewed
manual-only workflow; an environment alone does not constrain other workflows.

The user-installed Linux runner2.337.0 (`froniterX-linux`) is now online and
executing a job. Local decoder/font/retail/display/audio configuration is
stored only in its outside-repository environment file with owner-only
permissions. No credentials, paths or retail files were published or uploaded.
The source worktree and its uncommitted changes remain separate from the
runner's clean checkout.

[Actions run38360609114](https://github.com/Michael2543/frontierX/actions/runs/38360609114)
was dispatched for workflow `.github/workflows/m3.yml`, revision
`09d3c7d86a39bc900afc746207d7c2e6bffdf288`. Initial job
`linux-host-mission (gcc)` /115173651342 executes on that Linux runner;
setup and checkout complete successfully, and actual GCC optional source
build execution is in progress. Results must be updated after job completion.
Queued jobs are not executed evidence, and this initial run cannot exercise
the uncommitted follow-up DACOM/DAHotkey/tooling changes. Its explicit
incomplete semantic/save/continuation acceptance failure remains intact.

The Windows host is reachable again. The user's runner is now registered
and online; its required Windows build and GPU labels have been applied and
read back. A misleading registration-deleted startup error was traced to a
three-hour UTC clock difference: the runner diagnostic recorded an OAuth
expiry already in the server's past. The user synchronized the clock;
read-only checks now show successful session creation and Listening for Jobs.
No credentials were copied into the repository. Attempt1 Windows CI job115173651357 remained queued despite matching labels.
After an idle listener-session refresh, the stale attempt was cancelled and
only the Windows job requested again. Attempt2 job115196293924 now executes
on frontierX-Windows at the same09d3c7d revision. Online/session status alone
is infrastructure evidence; native build results remain pending. Linux results
above belong to completed attempt1 jobs and remain preserved.

The independent CI and source preparation continues without committing or
pushing this overlay. Its additional owner/tooling fixes need explicit
commit/push authorization before a later CI revision can validate them.

## Executed first-run Linux results and follow-up overlay (2026-10-13)

Revision `09d3c7d86a39bc900afc746207d7c2e6bffdf288`, workflow `m3.yml`,
run38360609114 executes the published baseline, not this working overlay.

| Actual Actions job | Job ID | Result |
|---|---:|---|
| linux (gcc, default) | 115173651453 | passed |
| linux (gcc, optional) | 115173651578 | passed |
| linux (clang, default) | 115173651656 | passed |
| linux (clang, optional) | 115173651596 | passed |
| linux (clang, sanitizers) | 115173651597 | passed |
| linux (gcc, sanitizers) | 115173651511 | failed linking the NetBuffer/MovieCamera closure: missing IBaseObject RTTI |
| linux-host-gpu (gcc / clang) | 115173651227 / 115173651443 | failed; not accepted |
| linux-host-mission (gcc / clang) | 115173651342 / 115173651514 | failed; not accepted |

Toolchains are GCC15.2 and Clang21.1. Published drivers generated input manifests
and exercised source/synthetic checks; the GCC mission job built the optional
closure and passed its non-GPU checks before the real mission failed. The
initial GPU/mission jobs' detailed private diagnostics were erased by automatic
RUNNER_TEMP cleanup before investigation. Their exact failure owners cannot be
claimed from local reproductions. Late-job diagnostics are retained privately;
no retail traces or private logs were uploaded.

The overlay requires `FRONTIERX_M3_PRIVATE_ROOT`, an absolute outside-checkout,
outside-RUNNER_TEMP, outside-retail directory. The helper resolves symlinks and
rejects protected/disposable roots, uses POSIX owner-only directory permissions
and retains evidence between jobs. Windows uses the configured private host
folder's inherited access policy. Existing CTest execution-gate tests now check
retention across temp deletion and reject absent/relative/unsafe roots. Those
checks pass locally; native Windows helper creation also passes. Linux listener
configuration must be reloaded before an overlay CI run.

The original IBaseObject destructor now resides in BaseObject.cpp with its
unchanged destruction sequence. The original ARCHNODE definition moved unchanged
to a private header shared by BaseObject and ObjList; serialized/public fields
and offsets did not change. A bounded BaseObject owner target supplies the real
RTTI, vtable and implementation to NetBuffer instead of disabling UBSan vptr.
Local GCC ASan/UBSan NetBuffer checks pass. All five existing GCC/Clang optional and
Clang-sanitized non-GPU sequences passed after this owner relocation.
Native Windows Debug/Final full15-provider rebuilds and immediate same-build
campaign ABI checks also pass on the private source overlay.

A further full GCC sanitizer link identified ICloakEffect::Init as an undefined
non-pure key function. Source review finds only the concrete CloakEffect::Init
implementation; the real ObjectCloak caller invokes that implementation through
the interface. The overlay declares this interface slot pure virtual without
adding substitute behavior or changing the slot signature/order. Full GCC
sanitizer linking now passes. Its first non-GPU execution fails at
MaterialLibrary.cpp:526, material->AddRef(), with a UBSan dynamic-type report for
DAComponent<StateMaterial>. GDB identifies the actual material vtable and
original caller. Source review identifies an ELF ODR collision: legacy
Shading/IMaterial.h defines an IDAComponent-derived IMaterial, while
IMaterialManager.h defines a different CQ2 IMaterial without that base.
The native probe links both previously separate DLL closures, including the
original CQ2 PolyMesh owner. The RTTI symbol selected for the common name
therefore described the wrong interface. The two legacy material targets now
use a private build definition IMaterial=NativeLegacyMaterial, consistent across
their actual owners and callers. Interface slots, registered DACOM names, data
formats and Windows projects are unchanged. No vptr check is disabled.
After rebuilding the complete GCC ASan/UBSan graph, all52 non-GPU CTests pass
with detect_leaks=1 and halt_on_error=1. This covers synthetic/default closure
checks, not the original live mission or whole GPU/host process. All five other affected optional GCC/Clang configurations now also rebuild
and pass their non-GPU checks after the final build-definition change, including
the optional Clang ASan/UBSan closure. These remain local checks of the overlay. Native Windows
revalidation of the final interface correction remains pending.

The workflow additionally prepares explicit GCC/Clang original-mission
ASan/UBSan jobs (`linux-host-mission-sanitizers`), with leak detection and fatal
reports. They use the optional font/shared-decoder closure and live graphics/audio;
no headless replacement or leak suppression is used. These jobs have not yet
executed in CI. Independent CRT comparison can run when native Windows passes,
even if another Linux matrix scope fails; missing artifacts still fail.
Full-state, RNG/script/order capture, controlled timing, exact-tick save/reload
continuation, Windows conversations and retail corroboration remain mandatory
open acceptance blockers. No skip, partial trace or local check fulfills them.

## Next original-owner gates (source review, 2026-10-13)

The next determinism intervention must preserve the original frame/event
contract rather than force a matching snapshot. ObjectList::BeginFrame derives
elapsedTime from performance-counter deltas, clamps to0.25, scales game speed,
and accumulates ELAPSED_TIME/8 logic steps. ObjectList::Update may interleave
engine_update(ELAPSED_TIME) with batches, then engine_update(elapsedTime) after
the observed logic ticks. A logic-tick snapshot is therefore not a complete
physical frame boundary. Frame timing/event observation is required before
choosing a controlled clock entry; the existing positional first difference
alone does not establish the exact causal counter delta. No Z normalization,
float tolerance, gameplay change or assertion weakening is authorized by this
incomplete evidence.

| Original owner / caller | Source finding | Remaining required evidence |
|---|---|---|
| MGlobals::AssignPlayers | srand(randomSeed) seeds its CRT domain | actual training entry seed and complete draw-domain provenance |
| MGlobals::IsUpdateFrame | CRT rand controls the source synchronization decision | real mission draw sequence/state, not only standalone CRT probe |
| OpAgent synchronization / ID selection | CRT draws participate in decisions and selection | relevant queues, masks and CRT-domain state at each comparison tick |
| Trim DLL initialization | srand(time(NULL)), then rand, in its Windows CRT domain | distinguish Windows DLL/MT domains from merged Linux ownership |
| cloud.cpp temporary RandomGen | seed derives from mission ID; default modulus is RAND_MAX | actual training relevance and Windows/native modulus contract |
| ObjectList frame owner | wall-clock remainder and physical batching are separate from updateCount | controlled original event/frame boundary and identical audio completion ordering |
| Mission::Save / original ISaveLoad owners | original file-backed owner exists | tick256/512 safe-boundary saves, semantic records, reload and continued tick1024 comparison |

These are source findings, not executed deterministic or save acceptance.
Save/reload must not occur during an ObjectList iteration. The current M2
moved-state reload checks do not establish full RNG/queued-order/script/physical
continuation equivalence. The known restored-Windows conversation defect
remains open across script trigger, lookup, decode, playback and completion;
matching compiler traces cannot serve as retail behavior evidence.

## Completed CI follow-up and Linux-only continuation (2026-10-13)

The user explicitly authorized commit, push and CI execution. Commits824aa2e
(owner/CI repairs) and62662a1 (extracted-header whitespace) were pushed to main.
[Run38368454837](https://github.com/Michael2543/frontierX/actions/runs/38368454837)
at `62662a112c6d3fd51490eb40b1232a30caf0b6fc` is completed with failure.

| Executed Actions job | Job ID | Result |
|---|---:|---|
| Linux GCC default / optional / sanitizers | 115208559560 / 115208559430 / 115208559499 | all passed |
| Linux Clang default / optional / sanitizers | 115208559603 / 115208559638 / 115208559562 | all passed |
| Native Windows Debug/Final and same-build ABI | 115208559574 | passed, including synthetic validation helpers and all6 DACOM provider/release cases |
| Linux host GPU GCC / Clang | 115208559258 / 115208559478 | failed |
| Linux original mission GCC / Clang | 115208559498 / 115208559453 | failed |
| Linux original mission ASan/UBSan GCC / Clang | 115208559345 / 115208559657 | failed; CPU checks passed before real mission checks |
| Native Windows original mission | 115210863409 | failed exact same-build comparison at count3/unit40/Z; mission runs executed |
| CRT cross-build | 115218979919 | GCC/Clang characterization equal; Windows differs at first draw |
| Acceptance | 115219147229 | failed explicitly; not M3 acceptance |

Private evidence survives runner cleanup at the configured retained root.
Public logs contain bounded revision/toolchain/stage summaries; only synthetic
CRT metadata is uploaded. Both Linux ASan/UBSan compiler paths now actually
execute in CI; the earlier GCC link failure is fixed at the original owner.
This does not turn failed live mission or GPU sanitizer processes into passes.
Native Windows uses MSVC14.44.35207, native Win32, installed external DXSDK.
The final local isolated15-provider Final rebuild/ABI after ICloakEffect's
interface correction also passes. An earlier local overlapping rebuild lost
DACOM.lib while another project rebuilt it: a test-setup conflict, not a source
link defect; it was rerun serially. Those local checks supplement CI only.

The user clarifies that **every restored-Windows mission, training or campaign,
immediately skips conversations when they should start**. Cause/stage remains
unknown. The user also explicitly limits further repair work to Linux. No
additional Windows Streamer/provider repair is pursued. Existing native Windows
comparison/ABI jobs remain observations of a constrained reference, not the
Linux behavior specification. An x86 ACM format-availability query returns0
for PCM and IAC tags1025/1026; this does not prove actual resource decoding,
playback or completion. Experimental uncommitted Windows Streamer work was
removed; no retail installation was overwritten.

## Original Linux frame/checkpoint access (118efae CI in progress)

Evidence below is local source/build/runtime work on base62662a1, not CI
execution of this overlay. The original Modal and ObjectList owners accept an
opt-in `FRONTIERX_M3_FIXED_FRAME=1` input equal to source `RENDER_PERIOD` for
each frame. Modal retains the original truncated1/1024-millisecond control
units; ObjectList retains original scaling, tick remainder, logic, scheduler,
physical updates and event processing. Normal measured-time paths are unchanged.
The private trace is required. Audio completion and other live clocks remain
live; this is not proof of complete deterministic input isolation. A first
120-second fixed-period attempt captured only519 records and timed out; fixed
frame cases therefore have a documented600-second bound. Timeouts fail.

`FRONTIERX_M3_CHECKPOINT` selects only256-save/256-reload/512-save/512-reload.
After original physical updating, outside ObjectList iteration and before the
GameSystem event pump, the test access calls original Mission::Save and optional
Mission::Load through SAVEDIR. It requires the exact updateCount and rejects an
overshot point rather than moving it. Independent private states are captured
before/after save and after reload. No mission objective setter, replacement
simulation or sidecar restoration of omitted state is used.

The local optional Clang256-reload path executes Save/Load, retains a263765-byte
original UTF save, and continues through all1025 tick records to1024. Its
process still fails the existing two-unit-voice diagnostic (the original script
move is silent); that assertion remains intact. Observed fields before/after
Save are equal. Reload changes master-list order and clears the Forger selection;
identity sets remain equal. Global speed/limit/pause/active controls match, and
all1024 observed frames have exactly one original logic step with active,
unpaused flags. Another earlier trial timed out after257; its exact cause was
not measured and is not attributed to the new control-field observations.

Schema2 adds original global speed/frame-limit/constant-rate/active/pause flags.
Boolean flags use their documented truth value0/1; no float tolerance or ID/list
normalization is applied. The paused-world original tick branch now receives
the same observer/command access, preventing silent gaps in captured updateCount.
The comparator retains schema1 compatibility for old diagnostics; mismatched
schemas do not compare equal. Partial checkpoint equality explicitly cannot
satisfy complete save semantics; existing CTest comparator checks enforce this.
Complete RNG/script/queued-payload capture remains open.

The workflow prepares measured and fixed-source-period Linux mission matrices
under GCC/Clang, also under ASan/UBSan. Fixed cases execute two uninterrupted
references and Save-only/Save+Reload at both points, followed by continuation to
1024 and exact comparisons. Each process result is retained, including timeouts,
and independent cases still run after a failure. Original save bytes are read
and compared separately; no blanket byte-equality requirement or unknown-metadata
normalization is inferred. All traces/saves stay private. A protected
`linux-cross-build` job reads only matching same-run/attempt/revision input
manifests from retained host storage; stale/dirty/missing inputs fail. It checks
same-build repeatability first and labels later comparisons as characterization
when repeatability fails. This matrix is now executing in run38384161068 at118efae. Its final
results and downstream cross-build evaluation are still pending.

## Native UTF sibling-order owner correction (34159ed, included in118efae)

Original `DOSFile/UTFDMan.cpp`, DirectoryManager::addChildEntry, appends children.
Original UTF::FindNextFile follows the stored sibling chain. NativeUtfWriter
instead used lexical map traversal, and NativeWritableUtfArchive enumerated and
flushed a lexical map. Original ObjectList::Load consumes that enumeration and
AddObject prepends to the master list; engine_update calls PhysicalUpdate in
master-list order. Lexical canonicalization therefore changes an observable
simulation input and must not be treated as harmless save normalization.

The native writer now retains node creation order for sibling links. The writable
archive retains a private creation ordinal for enumeration/flush and preserves it
when an existing file is truncated. No serialized field/public interface changes.
The existing data_root CTest now checks the **original append contract**: identical
ordered inputs encode identically, changed creation order remains visible,
nonlexical live child enumeration survives Flush/production-reader reopening,
and invalid duplicates/traversal leave the last valid output unchanged. This
replaces the old lexical-canonicalization expectation with the source format
contract, not a weakened gameplay assertion. The extended regression fails
against the pre-fix native libraries and passes with the corrected owners.
All six affected local GCC/Clang default/optional and optional ASan/UBSan build/
non-GPU sequences pass. Actual checkpoint continuation after this writer change
and its CI execution remain pending. Original reload selection and further
save/continuation differences remain visible open investigations; no list
sorting or selection restoration is added to make observations match.

Follow-up local native-writer execution: the Clang256-reload trial reaches the
checkpoint with the corrected append owner. The first immediate reload
difference moves from unit0/class to unit0/ID: lexical class-group reordering is
removed, but the original AddObject prepend reverses instance traversal. All
per-identity observed unit fields match except the selected Forger bit; identity
sets and global controls match. This identifies a remaining shared-loader
ordering behavior by source review and native execution, not a proved Windows
Save runtime result. No comparator sort, selection restoration or gameplay
relinking is used. The latest trial also captures all1025 records through1024 and exits7 at the
unchanged voice diagnostic. Its Save/Load file and continued path are real
execution, while reload differences and full-coverage gaps remain failures.


## Linux startup sanitizer failure and correction (2026-10-13)

Run38384161068 at118efae, job115251675803 (`linux-host-mission-sanitizers
(gcc, measured)`), actually fails both original mission attempts before tick0.
With fatal UBSan, Camera::SetWorldRotation reads an uninitialized RECT and
reports signed overflow at Camera.cpp:541. The original global startup caller
creates Camera before Mission::Load. Sector's current system is0 and
GetSystemRect returns FALSE without writing its output when no system exists.
This is a source return-contract misuse exposed in the native startup closure,
not a measured simulation/RNG difference or an external GPU sanitizer leak.

Hypothesis: honoring the failed rectangle lookup removes this startup UB while
preserving the successful loaded-system branch. Linux SetWorldRotation and
SetWorldRotationPitchRoll now use their existing zero-center/no-sector branch
when GetSystemRect fails. Windows code, loaded-system arithmetic, camera layout,
saves, simulation updates and assertions are unchanged. The smallest falsifying
check is the same original mission startup under fatal UBSan; build/CPU tests
alone do not establish that it passes. Local GCC/Clang sanitizer shell builds
and their existing non-GPU CTest suites pass. Actual corrected mission execution
and CI remain pending; the current run still tests118efae before this correction.

In that same run the GCC/Clang explicit GPU jobs115251675425/115251675507
actually fail `portability.sdl3_gpu`: requested resizing leaves the mapped
swapchain size unchanged. This remains a desktop/resize environment limitation,
not a skipped or passed GPU gate. GCC measured mission job115251675670 records
both complete1025-tick traces but retains process exit7. Native Windows job
115251675441 passes Debug/Final/ABI; Windows mission115253813234 fails. No new
Windows repair is undertaken. Fixed-period/save cases, remaining Linux scopes
and private cross-build jobs are still pending; do not infer their results.


## Prepared explicit GPU host setup (2026-10-13)

The ordinary Wayland resize failure remains reproducible. M2's supplementary
mapped-floating X11 runs used an external preload; that is not CI evidence.
The existing two GPU CTests now have opt-in pre-render host preparation. Each
real probe creates its real SDL window, writes a private per-process request,
and pumps SDL events while waiting at most10 seconds. Normal probe execution
is unchanged when no setup directory is supplied. Setup timeout fails rather
than skips; renderer initialization, original resize request and all readback,
retirement/depth/material assertions remain unchanged. No mission frame/input
or audio owner is involved in this GPU-only preparation.

Manual workflow input `gpu_host_setup=hyprland-x11` selects this explicitly
conditioned job. Tools/ci/gpu_host_setup.py requires mapped Xwayland windows
with one of the exact two probe titles **and a PID descended from its own CTest
process**. Only those windows are floated through the current Hyprland Lua
dispatcher; readiness requires an actually floating mapped window. There is no
persistent desktop rule, inventory log, preload, media transfer or assertion
override. Other windows are excluded. The existing deterministic CTest
`portability.m3_ci_gates` exercises real child-process ownership and rejects
unrelated processes. Missing compositor/access, unsupported host setup, timeouts,
skips and failed renderer assertions remain failures. The ordinary input remains
the workflow default. Host-conditioned success would not prove ordinary Wayland
resize success or audible/visual retail parity.

The current118efae run does not contain this preparation; corrected GPU runtime
and CI are pending. Its GCC fixed-period references each reach1024 and have
no difference in the **partial observed fields**. Their source process still
exits7 at the retained voice assertion, and full RNG/script state is absent.
The256-save case instead returns source modal result0 after mouse messages
with only ticks0..143; it never reaches its checkpoint and fails. The reason
for that early return/input provenance has not yet been established. Do not
count the completed reference subset as full repeatability or that truncated
case as executed save evidence.


Publication/execution ledger: the user-authorized Linux follow-up is committed
and pushed as `d089a1537e01446a12f7c1b11873bb470a571b0b`. Manual
[Actions run38386971218](https://github.com/Michael2543/frontierX/actions/runs/38386971218)
was dispatched with `gpu_host_setup=hyprland-x11`; it is queued behind the
retained118efae run, not an executed corrected CI result. All six affected
local Linux default/optional/sanitizer build and non-GPU sequences pass with
this code, using the existing lawfully external optional dependencies; fatal
ASan/UBSan and leak detection remain enabled for the instrumented scopes.
The retained Linux/Windows comparison jobs do not authorize further Windows
repair. Corrected camera startup, GPU host execution and final CI comparisons
remain pending. Complete semantic/RNG/script capture and equivalent continued
Save/Reload remain open implementation/acceptance blockers.


## Completed d089a15 CI and next owner corrections (2026-10-13)

All21 jobs of run38386971218 completed. The six `linux` GCC/Clang
Default/Optional/Sanitizer scopes pass; `windows-native` passes actual
Win32 Debug/Final rebuilds, campaign/module ABI and DACOM regression.
Both `linux-host-gpu` jobs fail: the opt-in preparation waits for a mapped
window before its first presentation, but SDL's X11 window is not yet mapped.
Preparation now runs after the SDL probe's first successful original
presentation and before the unchanged resize request. The material probe
needs no preparation hook; that unnecessary hook is removed. The helper
still only floats its own mapped descendant-PID test window. Existing GPU
assertions and fatal timeout/skip policy remain intact. Supplementary GCC
optional execution now passes both original GPU CTests in this explicit
X11 configuration; corrected CI and Clang execution remain pending.

All four `linux-host-mission-sanitizers` jobs fail before tick0 at the
original FieldManager::CreateInstance (`cloud.cpp:4621`): an AsteroidArchetype
is accessed as the unrelated FieldArchetype<BASIC_DATA> specialization.
Camera startup gets past the previous RECT failure. The Linux Field owner
now registers its original data/archetype metadata and exact typed destructor
at successful factory creation, and uses that metadata for instance creation
and destruction. This private lookup is not simulation iteration; no field,
vtable, save layout or Windows product implementation changes. The native
AntiMatter factory receives the original archetype from that metadata.
Supplementary Clang optional ASan/UBSan executes the original mission through
1024 after this correction, with no fatal access/UB report. Whole LeakSanitizer
still fails3644B/9 allocations including DBus and unresolved module frames;
these are not suppressed or all assumed external. Corrected CI remains pending.

The four ordinary host-mission jobs fail. GCC fixed-period cases record exits
0/0/0/0/timeout/7, but uncontrolled desktop input contaminates their protocol;
those zero exits are not controlled comparison passes. The narrow Linux
WindowManager fixture now requires an explicit private trace plus
FRONTIERX_M3_CONTROLLED_INPUT=1. It holds the source active state and centers
the pointer while excluding host focus, key/text and mouse events. Original
close/quit/resize and actual Streamer notifications remain delivered. Its
existing WindowManager regression tests real generated PCM initialization,
EOF/completion alongside excluded input and a fatal original close message.
Ordinary game input is unchanged. Modifier-state access remains in the
existing native Tab input owner, whose key event is excluded by this fixture.

Protocol version2 replaces the silent script move at64 with the original
player CQE_MOVE_SELECTED_UNITS event. Original ObjComm constructs the actual
user-move packet, updates the original operation owner and starts the movement
voice. The32 selection and1024 quit boundaries remain. No objective setter,
voice assertion change or fabricated playback completion is introduced.
A supplementary optional Clang original mission run reaches all1025 records,
starts two unit voices and exits0 with the retained native-shell diagnostic.
This is a source-process result, not an accepted comparator/CI result.

Schema3 now observes original MissionData and registered MScript program
identity/order/flags/save-record fields, plus original OpAgent queues and
deferred completion/termination/dependency state. DataParser enumerates declared
fields, using its original symbol schema to distinguish anonymous enums from
pointers. Scalars retain exact little-endian bits; nested fields retain order.
MPartRef retains only documented mission identity, excluding its cache index
and debug pSave/pInit pointers. Declared padding and the compiler's empty-record
size byte are excluded; generic pointers or unknown nonempty operation payloads
fail. MOVETO exports actual grid-quarter coordinates/system and ordered object
IDs, not runtime packet padding. Parser tests show changed identity/state matters,
while cache/debug/padding does not. No Save or script/operation executor is
called to observe state. The synchronous borrowed output callback keeps FILE
and allocation ownership within the caller's CRT. Its additive MScript export
requires actual new native rebuild/ABI execution, still pending.

Coverage stays explicitly partial: full physics/non-mission object state,
transport-buffer payloads and relevant RNG state are absent; snapshots remain
at the existing pre-physical logic boundary. Live speech completion can affect
script scheduling and is retained. No comparator tolerance, list sorting,
script-field omission or forced speech completion is used. Save-only versus
Save/Reload continuation is still required at256 and512. Existing reload
order and selection differences remain visible and uncorrected.

A separate concrete Linux RNG contract correction fixes RandomGen's implicit
modulus to the original Win32 value32767. The original class recurrence,
seed/state widths and explicit-modulus callers remain unchanged. Native libc
RAND_MAX2147483647 was incorrectly substituted into this non-libc owner;
Field's original ephemeral seeded generators use that default for placement.
The existing RNG CTest now links the production RandomGen owner and verifies
32 default-versus-explicit32767 draws for four seeds. It fails before and
passes after the Linux correction. Windows CI links its real rebuilt Final
RandomGen.obj for the same contract check; no new Windows product repair or
replacement CRT generator is introduced. The16-draw libc characterization
still differs across Windows/Linux and is not concealed by this correction.

`windows-host-mission`, `linux-cross-build`, `crt-cross-build` and `acceptance`
also fail in d089a15. Matching builds still cannot establish retail parity.
The user-reported instantly skipped Windows conversations remain an open
Windows defect/reference limitation; further Windows repairs are out of scope.
New working-tree evidence must be committed and tested by the protected
workflow before any corrected CI pass is claimed.


Local working-tree verification before the next publication: all six existing
Linux Debug trees rebuild and pass their complete non-GPU CTest scopes under
GCC16.2.1/Clang22.1.8, including default, optional and optional ASan/UBSan with
fatal errors/leak detection enabled. The optional font and shared decoder use
the existing reviewed external installation. This covers the production-owner
RNG and parser, input/Streamer lifecycle and comparator regressions. A real
mission trial exposes a pending PT_USRMOVE before sendHostCommand assigns
MOVETO; the observer now captures its original packet type and typed destination
in both states, rather than dropping the pending payload. This is an observer
correction, not a command executor or gameplay change. Full corrected mission
and CI comparisons remain pending.


## Post-physical state and sound lifetime follow-up (working tree, 2026-10-13)

User-authorized publicationb241156 starts protected Actions
[38426508626](https://github.com/Michael2543/frontierX/actions/runs/38426508626).
Its GCC and Clang `linux-host-gpu` jobs actually pass both existing GPU CTests
with explicit owned-window X11 setup; ordinary Wayland success is not inferred.
The native build fails on the introduced operation-observer export: OpAgent is
in Mission.dll, so its borrowed callback entry needs the same producer/import
boundary as the other Mission exports. That declaration is corrected; a
supplementary native Debug rebuild passes afterward. A new complete CI native
Debug/Final/ABI execution remains required. Windows mission is skipped by its
failed prerequisite inb241156 and provides no runtime proof.

GCC measured mission sanitizer actually reaches the first source tick after the
Field correction, then finds a misaligned U32 append at OpAgent::sendSyncMessages
(line3678 inb241156). Original GeneralSync records have variable byte lengths;
the Linux append now uses memcpy for the exact same four-byte parent operation
value. Windows product code and packet byte layout are unchanged. Supplementary
Clang measured ASan/UBSan now reaches1024/all1025 records with no fatal access/UB
report, but whole LSan still fails3836B/9 allocations with external/unresolved
frames. This is not a leak-free acceptance or completed corrected CI result.

Protocol version3 and schema4 move **fixed-input** snapshots after physical
updating/deferred destruction, before the original GameSystem pump. Tick0 is
the explicit initial boundary; measured diagnostics remain after-logic. The
observer rejects skipped physical ticks rather than inventing intermediate
states or changing original step grouping. On Windows Trim's Modal calls the
existing original ObjectList IEventCallback with a private opt-in observation
event; the callback executes outside iteration in the Conquest owner. FILEs and
allocators remain in their original modules, and no vtable slot is added.
Checkpoint capture and continued tick capture use the same complete boundary.
The parser rejects mixed boundaries/schemas and incomplete checkpoint boundaries.

Six supplementary optional Clang schema4 processes (two references, Save-only
and Save/Reload at256/512) all reach1024 and exit0. Before/after Save observations
match at both points. Save/Reload does not: raw processed-packet transport type,
original master-list order and selection are visibly different. Whole save bytes
also differ across independent save/reload cases; no whole-container byte-equality
contract is inferred. The first same-build **post-physical** difference is tick185,
MissionData.displayObjectiveHandlerParams_teletypeHandle and the original
TM1_DisplayObjectiveHandler program list. Its source Update waits on
IsStreamPlaying before starting TeletypeObjective. This confirms actual audio
completion affects script scheduling, not merely presentation. The field and
program remain in the comparator; no forced completion, tolerance or omitted
script state is introduced.

A separate confirmed Linux sound lifetime error: NativeMissionSound::IsPlaying
returned false at EOFREACHED, while NativeStreamer still waits for the audible
device tail and source SoundMan retains its registered stream until completion/
error handling or explicit stop. Native IsPlaying now retains that same owner
lifetime. Existing native_shell CTest plays its generated PCM through the real
NativeStreamer/SDL device and actual mission sound owner, checks paused/EOF live
status, real completion/owner cleanup and explicit stop. The new EOF regression
fails before this owner correction and passes afterward under Clang ASan/UBSan;
no fabricated status or mission is used. This does not make asynchronous live
speech deterministic or prove audible retail parity. Private native completion,
stream notifications and script polls now include original tick context without
changing notification order or delivering replacement events.

Schema5 includes every original world-list object, including objects without
mission data: existing part/class/owner/system accessors, original transform
basis/translation, linear velocity, visibility masks, update bin and eight
original object flags. Order and exact f32 bits remain; zero part IDs are retained,
not replaced with invented identities. Angular velocity, internal control goals
and procedural field state are still missing. Schema6 additionally reads all
35 top-level MT_GlobalData field trees through the original DataParser. Linux
uses the existing production UTF16/Win32-layout encoder on a temporary record;
Windows reads its original3236-byte record. Only declared fields are exported;
no runtime pointers, record padding, save mutation or objective setter is used.
A supplementary optional Clang schema6 run validates all1025 records,77 original
world objects and the full declared global field tree and exits0. It is a dirty
working-tree supplement, not a CI or whole-state acceptance result.

Narrow normalization added for processed MOVETO only: its obsolete user transport
packet type is exported as0. Original save RWPACKET_HEADER stores host command,
operands and ordered object set, omitting this prior user type; loaded MOVETO has
type0. Source sendHostCommand consumes the user type before the operation reaches
the host queue; MOVETO dispatch/operand access uses hostCmd. Pending user commands
retain the actual type, and other commands do not inherit this normalization
(the docking path, for example, still uses type). This removes an irrelevant
transport-stage distinction; destination, operation IDs/flags/order and actual
reload list/selection differences remain exact and visible.

Native Windows comparison access is expanded to measured/fixed references and
original Save/Reload points with continuation. Fixed source frame input is shared
with Linux, normal frame timing unchanged. The opt-in original System wndProc
excludes manual key/text/mouse messages while retaining resize, close, streamer
and activation handling. The original WindowManager must actually be active;
held modifiers or failed exact client-center cursor placement fail the case.
Original SAVEDIR is redirected only in the explicit private Sol_Train fixture;
retail installation data is not overwritten. Each case continues independently
on failure and records its actual process result. Manifests bind revision,
working state, input hashes, protocol and actual binary SHA256; changing the
binary within a case set fails. The known Windows conversation fault is not
repaired, suppressed or used as Linux's specification.

The protected `native-cross-build` job is prepared to fetch only same-run/
attempt/revision private Windows semantic traces over confirmed SSH. Connection
configuration/key paths and copied traces remain in the retained private Linux
root with restricted permissions, outside checkout/logs/caches/artifacts. It
checks same-build repeatability and input/protocol provenance before describing
cross-platform fields; missing/dirty/stale evidence fails. No raw media or dumps
are transferred. Actual execution remains pending. The separate
`randomgen-cross-build` job compares four fixed-seed32-draw vectors from the real
production owner under GCC/Clang/native MSVC. Only those synthetic vectors and
the existing16-draw libc characterization are allowed in uploaded artifacts.
Matching that narrow owner does not satisfy mission/RNG or retail acceptance;
the incompatible libc characterization and incomplete coverage remain failures.

All six local GCC16.2.1/Clang22.1.8 default/optional and optional ASan/UBSan
build/non-GPU scopes pass for the global/sound observer follow-up before the
latest Windows frame-access changes. The final combined source, native ABI,
controlled Windows cases and new comparison jobs still require rebuilding and
actual CI execution. Remaining implementation/acceptance blockers include full
relevant RNG/physics/queued-transport state, reproducible genuine speech events,
equivalent original Save/Reload continuation, whole-process leaks and additional
retail/visible/audible corroboration. No partial equality or skip counts as M3.


Final combined local preparation: all six Linux default/optional/optional-
sanitizer builds and non-GPU CTest scopes pass after native frame fixture and
original global/sound corrections. The subsequent cross-DLL post-frame routing
uses the existing ObjectList callback instead of a new Conquest import; a final
focused rebuild/replay of that routing and native Final/ABI are in progress.
The private SSH host key/connection configuration is retained outside source
with restricted permissions; no runner re-registration or credential transfer
is required for this comparison integration. This prepares execution, not proof.

## Retail reference and next sync boundary (2026-10-13)

The user confirms the external Steam `Conquest.exe` is unchanged from Steam.
Read-only Windows metadata identifies version `01.07.10.1802`, SHA256
`ed8741c0cb727a43b7de7be0e73566b04fbc1cbd58b54a62d538e3f39dcb3f52`.
This establishes user-reported input provenance, not a runtime result or proof
that the source snapshot matches retail. No original installation is modified.
Retail speech/visible/audible execution remains open. Further restored Windows
product repairs remain outside the requested Linux scope.

Completed b241156 CI has all six CPU scopes and both GPU jobs passing. Native
build failure is the introduced Mission observer export, corrected in b6ca3dd;
Windows mission/CRT jobs are skipped and acceptance fails. b6ca3dd CI remains
in progress: native Debug/Final rebuild and ABI pass within their job, but its
launch-helper stage fails before DACOM/RNG stages. An independent SSH recheck
of that exact helper passes twice, including strict mode; this does not erase
the CI failure or establish its cause. The next driver retains the exception
privately and publishes only a fixed stage name. No exception paths or private
runtime outputs are uploaded.

GCC measured mission sanitizers in b6ca3dd stop at
`Planetoid::getSyncData`, line2342. Original
`ObjectFrame::FRAME_getGeneralSyncData` deliberately calls each owner at
`buffer+1`, after its one-byte node header. The typed PlanetSync store is thus
not guaranteed aligned. The Linux follow-up copies the same sizeof(PlanetSync)
bytes for both get/put; all bitfields, resource deltas, thresholds and update
order remain unchanged. Windows product code remains unchanged. The actual
mission sanitizer execution, rather than a parallel packet model, checks it.

Observer schema7 adds exact engine angular-velocity bits through the original
`ENGINE::get_angular_velocity` for each nonnegative original object index.
Objects without an engine index report null, not invented zero. This observes
engine-instance state (e.g. a FighterWing index identifies its bay door), and
does not claim complete object angular/control/physics state. Comparator CTest
checks null versus zero, signed-zero bits and malformed vectors; old schemas
retain their own exact contracts. Full RNG/physics coverage is still partial.

The completed supplemental b6ca3dd-source fixed Clang six-case run exits0 in
all six game processes and records1025 ticks each. The driver correctly fails.
Same-build first divergence remains tick185, script field
`displayObjectiveHandlerParams_teletypeHandle`. New full global observation
also identifies `lastStreamID` and `lastTeletypeID` changing from0 to5/1 during
the original Save, in both256 cases. `MScript::Save` refreshes these caches from
SOUNDMANAGER/TELETYPE after writing program data; this is source-owner cache
behavior, not yet evidence of a port defect. No field is dropped or normalized
in this follow-up. Existing reload object order/selection differences remain
open separately; whole-save byte equality remains unestablished.

Validation: Linux Clang22.1.8 optional ASan/UBSan build and51 non-GPU/non-mission
CTests pass after the Planetoid/schema7 edits; generated comparator regressions
pass. All six affected GCC16.2.1/Clang22.1.8 default/optional/sanitizer local
builds and non-GPU suites pass. Actual Clang optional sanitizer mission reaches
all1025 schema7 records without an ASan access or UBSan report, then exits1
on whole LSan; the partial-coverage driver fails as required. GCC actual
mission sanitizer follow-up is still running. These are dirty local supplements; new native
rebuild/ABI and corrected CI execution remain pending. No M3 acceptance claim.

The prepared Windows host job requires an explicit output emitted only after
both native rebuilds and ABI checks pass. It may then execute independently
of a later helper failure; that failure still blocks mandatory overall
acceptance. The host job rebuilds Final itself and retains private source/run
provenance. No failed rebuild, skipped ABI check or missing output admits it.
