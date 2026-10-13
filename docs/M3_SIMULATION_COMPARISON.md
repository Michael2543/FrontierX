# M3 simulation, save and compatibility comparison

Status: **2026-10-13, NOT ACCEPTED** (work started2026-10-12). The first
actual GitHub Actions run is in progress at the published09d3c7d revision;
no complete CI acceptance is established. Local Linux and SSH native Windows
results below remain supplementary evidence only.
The restored Windows executable is an implementation under test, not an
unqualified retail oracle. The user's mission-conversation failure remains
open; no Linux behavior is changed to imitate it.

## Baseline audit and source provenance

The initial Linux worktree was clean at
`516b842820776b0699e270fcb37e8a77c4dc5c77`. The user subsequently committed
and pushed the prepared work as `09d3c7d86a39bc900afc746207d7c2e6bffdf288`,
verified on the private repository default branch on2026-10-13. This follow-up
starts from that clean revision; its additional changes remain uncommitted.
The agent has performed no commit, branch, push or publication.
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
