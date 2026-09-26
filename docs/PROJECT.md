# FrontierX Project Charter

Status: incremental native bring-up, 2026-09-25. The supported VS2022 Win32 Debug and Final
no-deploy builds were rebuilt on physical Windows 11, including the 18 campaign modules.
Fresh modules passed the original campaign import check. On the physical Windows host, the
restored Final D3D9 path reached the main menu, first Terran campaign briefing and a visible,
scrollable game scene with external installed data and repository-built TextureLibrary. The first
Terran mission currently shows the background and HUD but no running mission or units; the user
separately reported two visible ships in the Celareon campaign. The repository-owned rendering
fixture now also executes and captures through the supported Windows D3D9 path. Its 64-by-64 P6
image passes the documented one-pixel-tolerant comparison with the visible Linux SDL/Vulkan
capture; this is synthetic cross-backend runtime evidence, not retail gameplay parity. See
[WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md).

## Mission

FrontierX will make *Conquest: Frontier Wars* run as a native 64-bit Linux application while
preserving the shipped game's behavior. The intended result is an x86_64 ELF build that does not
require Wine and consumes media from a lawfully obtained retail installation.

This is a port of the original C and C++ engine, not a gameplay rewrite or a new engine inspired by
Conquest. Modernization exists to remove platform dependencies and keep the game maintainable; it
must not silently redesign simulation, content formats, missions, or balance.

## Source Baseline

- Upstream restoration: `https://github.com/Revan67/ConquestFWR`
- Reference branch: `master`
- Bootstrap reference revision: `8051dd2fd54e141000694a4c24e027a086e6a52f`
- Upstream purpose: restore retail behavior on modern Windows before broad modernization

The 2013 source release is a partially modified CQ2 development snapshot, not the exact retail
source. Retail-compatible layouts, object IDs, campaign ABI repairs, and verified runtime fixes in
the restoration repository are part of the baseline and must not be discarded during the port.

## Definition of Success

FrontierX is successful when:

1. The game configures and builds reproducibly on a supported Linux x86_64 toolchain.
2. The produced executables and libraries are native ELF binaries without a Wine runtime.
3. A legitimate retail installation can supply all required media without copying that media into
   this repository or its release artifacts.
4. Campaign loading, gameplay, saves, maps, and other supported retail data remain compatible.
5. Simulation behavior remains deterministic across supported builds.
6. The retail acceptance matrix has Linux evidence comparable to the Windows baseline.

Multiplayer, additional architectures, and other operating systems are later milestones. They must
not broaden the first Linux bring-up.

## Ordered Strategy

1. **Protect the baseline.** Record source provenance, preserve the working Win32 build, retain ABI
   and layout checks, and establish small executable tests where possible.
2. **Build portably.** Introduce CMake and Ninja incrementally alongside the supported Visual Studio
   build. A generated project file is not evidence that a target runs.
3. **Create a platform boundary.** Move windowing, input, timing, configuration, filesystem, dynamic
   loading, threading, and diagnostics behind portable interfaces. Do not spread Linux, X11,
   Wayland, or Win32 calls through game logic.
4. **Bring up one rendering path.** Preserve the current D3D9-facing contracts and connect one real
   production draw caller to the proven native submission surface before selecting a backend.
   Compare a representative scene with Windows and replace unsupported D3DX9 facilities explicitly.
5. **Port content services.** Address audio and video after startup, data loading, input, and basic
   rendering are measurable. Network transport comes after deterministic single-player behavior.
6. **Package last.** Release packaging starts only after license, dependency provenance, retail-data
   separation, and runtime acceptance checks are automated.

Each phase must leave a testable result. Avoid broad mechanical rewrites that mix build-system,
platform, renderer, ABI, and gameplay changes in one step.

## Architecture Direction

| Area | Direction | Current status |
|---|---|---|
| Primary target | Linux x86_64, native ELF | Accepted goal |
| Windows | VS2022 Win32 restoration build | Regression baseline |
| Build | CMake presets with Ninja; preserve existing MSBuild path | Isolated probes, platform services, a configured production EventSystem ELF module, production event/profile owners and native lifecycle shell build; game graph planned |
| Hotkey input | Existing DAHOTKEY event processor behind legacy-message and `IFileSystem` contracts | Production factory, read-only UTF adapter, checked-in source tables and optional shell/EventSystem composition work natively; packaging, SDL joystick, recorder and Windows parity remain open |
| Window/input/time | SDL3 plus standard C++ behind a FrontierX platform boundary | A native SDL-backed DACOM `IWindowManager` owns the shell window, callback pump, focus/cursor state and close-veto lifecycle; application cadence, Windows parity, renderer and game integration remain |
| Graphics | Keep D3D9-facing code for bring-up; evaluate maintained native Vulkan paths | The native shell can explicitly send the repository scene from production `PrimitiveBuilder` through the actual App-side `RenderBatcher`, native `IRenderPipeline` vtable and registered `IRenderPrimitive` to Vulkan presentation/readback. The adapter owns checked RPVertex/RPVERTEX2 CPU buffers through create/lock/unlock/destroy, routes direct/indexed draws into the registered owner and supports six transactional depth/blend states; unsupported pipeline slots fail explicitly. The production `BMPOOLJR` arena owner, vertex-buffer manager CPU lifecycle and neighboring vertex-descriptor copier execute in focused native targets. The batcher direct and U16-indexed branches plus opaque and alpha-depth queued-pool CPU paths have linked synthetic checks; the alpha-depth pool has visible GCC/Clang and focused sanitizer GPU evidence using repository geometry. A separate GPU probe draws indexed RPVERTEX2 geometry, observes the second UV pair through readback and verifies two GPU scene lifecycles plus preservation after an invalid upload. Six stage-zero color/alpha combine fields traverse the batcher and primitive to the fragment shader, with bounded image-change and rollback evidence. Production `StateMaterial` submits a cached indexed draw in visible GCC/Clang and focused sanitizer Vulkan probes. Production texture loading/material-library ownership, other queued pool modes, production global/diagnostic services, Windows oracle, backend selection and game rendering remain open |

| D3DX9 | Inventory and replace effects, texture helpers, and math dependencies | Required |
| Audio | Portable backend, selected after dependency and behavior evaluation | Undecided |
| Video | FFmpeg-based replacement for legacy movie playback | Planned, later phase |
| Network | Portable sockets with deterministic protocol behavior | Deferred |

The SDL owner now registers two repository RGBA8 textures under monotonic runtime `U32` handles;
the production `RenderBatcher` binds both through the existing texture-stage slot, with bounded
image-change, exact restoration and stale-handle checks. The production texture library and
material loading remain unported; see
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).
The production `StateMaterial` now runs through DACOM initialization, native
state/texture callbacks and a cached indexed draw in a synthetic CPU target
and visible Vulkan probe; its ownership checks and remaining material-library
and game-composition gates are in
[STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md).
The native `IRenderPipeline` also creates and uploads one RGBA8 texture level for the probe and
shell. A 2026-09-24 synthetic regression rejects duplicate live backend handles while retaining
the first texture's metadata. Its one-level CPU lock/unlock path now preserves a writable
shadow and retries a failed backend upload. Visible GCC, Clang and focused ASan/UBSan
Wayland/Vulkan probe runs observed the resulting alternate texture. Production material
ownership and further formats remain open.

### Verified Linux Layout Probe

Compile and automated-check evidence from Linux x86_64 on 2026-09-12:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug
```

GCC 16.2.1 and Clang 22.1.8 each produced native 64-bit x86-64 ELF probes and passed the four
registered CTests. The layout probe verifies the LP64 process model, fixed-width legacy integer
aliases, native pointer width in `HYBRID`, retail mission enum values, and the size, alignment,
field offsets, final-field extent, and x86_64 bitfield encoding of the production
`MISSION_DATA_BIN` and `ResourceCost` definitions. The build also compiles the existing TinyXML
sources as the first portable static library and links a behavior test that covers parsing,
entities, errors, and LP64 attribute conversion. A separate standard-C++ platform library now
provides monotonic microseconds and 32-bit milliseconds, clamped elapsed-time calculation,
millisecond sleep, and thread yield; its linked test checks deterministic clamping and that the
clock advances across a timed wait. The modal UI frame clock and progress-animation clock and sleep
are the first production consumers. The remaining QPC calls in `Modal.cpp` feed legacy profiler
counters and are not part of this migration. These targets do not compile game or engine modules,
load retail data, or establish runtime parity. The owning `Trim` module, Windows rebuild, and ABI
validation remain pending on native Windows. TinyXML release provenance remains a distribution
blocker recorded in [DEPENDENCIES.md](DEPENDENCIES.md).

The read-only `frontierx_data_root_probe` checks the seven required and five optional directory
paths used by `MenuResource::updateLoadPath` and distinguishes exact matches, case mismatches,
missing paths, ambiguous case-folded matches, and non-directories. Its self-test proves that
optional media warnings do not make an otherwise valid root unusable. A source-tree run found four
required and five optional case mismatches plus three missing required paths under `Data`; no retail
installation was inspected, and no file contents were read or copied.
The headless Linux selection order is an explicit command-line path, `FRONTIERX_DATA_ROOT`, then
the existing `Data` relative-path default. Synthetic process checks verified environment selection
and command-line precedence. The Windows registry-backed `DataPath` policy remains unchanged.

The platform library also owns a bounded native preference store for the two value classes used by
`IUserDefaults`: strings and opaque binary values. Its new `FXSET001` format uses explicit
little-endian lengths and types rather than dumping native structs. Keys are serialized in stable
byte order, malformed loads are transactional, and saves replace the target through a temporary
file in the same directory. `LegacyPreferenceValues` now reproduces the existing typed
binary/string, deletion, short-buffer and four-entry MRU value semantics without registry or dialog
calls. A native DACOM component implements the real `IUserDefaults` vtable, maps its live
`USER_DEFAULTS` view to the fixed-width schema, performs transactional immediate and shutdown
saves, and runs in the native shell. Native game-startup selection, file dialogs, document/viewer
editing, installation metadata, EULA presentation and Windows comparison remain open.
`StartupPreferences` now executes the isolated player/global lifecycle for both headless startup
and the native shell through `NativeUserSettingsOwner`:
it selects player/global/default settings, scopes player keys by hex-encoded name bytes, removes
only an invalid player namespace, and stores global/player schemas transactionally. The
separate `NativeUserSettings` conversion persists 25 required fixed-width version-1 fields and 31
explicit flag assignments rather than native bitfields. Both startup paths resolve an explicit
preference path, then absolute `XDG_CONFIG_HOME`, then
`$HOME/.config/frontierx/preferences.dat`; it ignores relative base-directory variables and
validate both the container and user-default schema without exposing the resolved path. The
supported Windows registry implementation is unchanged.

The archived `misyltoad/dxvk-native` project is useful historical evidence that native D3D9 and SDL
integration can work, but it is not an approved or maintained dependency. Do not vendor it or make
it the architecture's source of truth without a current maintenance, compatibility, and license
review.

## GeneralsX as a Reference

GeneralsX is a process and architecture reference, not a source or status baseline for FrontierX.
FrontierX adopts its strongest transferable principles: preserve the original engine and retail
behavior, isolate platform code, keep one portable source line, migrate incrementally, protect
determinism, and validate narrow vertical slices before broad rewrites.

FrontierX does not inherit Generals-specific paths, build commands, backport rules, math wrappers,
or completed-platform claims. Its mandatory DXVK, OpenAL/MiniAudio parity, macOS, and ARM64 choices
also do not become FrontierX decisions without Conquest-specific technical, maintenance, license,
and runtime evidence.

## Compatibility Invariants

- Retail file, save, map, schema, and network layouts use fixed-width integer types and explicit
  packing where required. Never widen a persistent field merely because the runtime is 64-bit.
- Runtime pointers and handles must not be stored in `U32`, `DWORD`, or other 32-bit surrogates.
  Separate serialized identifiers from native runtime references.
- Changes to structures shared by `Conquest`, `Mission`, `Trim`, `Globals`, `ZBatcher`, or campaign
  modules require layout and ABI validation across every affected target.
- Platform, graphics, audio, and logging changes must not alter simulation order, random-number
  consumption, floating-point policy, or serialized bytes without an explicit compatibility plan.
- Case-sensitive Linux paths are authoritative for portable code. Include and asset lookups must
  use the exact on-disk spelling.

## Evidence and Scope

The retail game and its data define expected behavior when the CQ2 snapshot disagrees. Use the
existing `HANDOFF.md`, `RETAIL_TEST_MATRIX.md`, and tools under `Tools/` as the initial evidence
base. State whether a conclusion comes from compilation, an automated check, or a runtime test; do
not describe planned Linux behavior as implemented.

Do not opportunistically fix unrelated retail-restoration defects while introducing a platform
boundary. Report them or isolate them in a separate change so regressions remain attributable.

## Legal Boundary

`Conquest Source License.txt` is the controlling license. `LEGAL.md` defines this repository's
compliance process and `MODIFICATIONS.md` is the required prominent change record. No dependency,
copied implementation, binary, retail asset, or release artifact may be added merely because it is
technically convenient.

In particular, `binkw32.dll` and `Conquest_Frontier_Wars_Manual.pdf` have unresolved redistribution
provenance and must remain outside FrontierX history and release packages unless documented
permission is obtained. Raw retail binaries, disassembly projects, decompiler output, dumps, and
extracted media stay outside the repository.
## Port readiness follow-up

See [Linux port readiness](PORT_READINESS.md) for the first proposed layout-probe milestone,
initial blocker inventory, ABI tool parameters and pending validation. The layout probe and isolated
TinyXML library are implemented; there is no native game build. The ABI scripts are parameterized,
reject non-Windows hosts, and still require native Windows execution with `dumpbin`.

## Build declaration inventory

Run `python3 Tools/portability/build_inventory.py` from the repository root to inspect the
supported Windows entry points without invoking Windows tools. See [build graph evidence](BUILD_GRAPH.md)
for scope, test commands, measured dependencies and production profile-parser evidence. This
read-only inventory does not evaluate MSBuild or replace native Windows ABI validation.

## Profile buffer core (2026-09-13)

The Linux presets build `frontierx_profile_buffer_probe`; its registered checks pass with both GCC
and Clang. `FrontierX/ProfileBuffer.h` owns section marking, line/section navigation,
key lookup/copying, and section/key callback traversal shared with the production DACOM parser.
Runtime section offsets now use the pointer-width encoded-handle contract instead of `U32`
decoding. This remains an internal borrowed-buffer contract with sentinel padding, not a complete
portable DACOM implementation or game target. Native Windows Debug/Final rebuilds, callback
comparison and ABI checks remain required; see
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

### Owning native profile reader (2026-09-13)

The existing GCC and Clang presets build and execute `frontierx_profile_document_probe`.
`FrontierX/ProfileDocument.h` provides owning text/file loading, ordered section names
and key lookup through the shared profile-buffer core. Native startup configuration currently
accepts ASCII without NUL bytes, up to 1 MiB, with locale-independent ASCII case folding.
This policy is limited to the new reader; Windows DACOM comparison behavior is unchanged.
No native game startup or retail-configuration compatibility is claimed by the synthetic tests.

The production DACOM profile-parser source now builds as a native static service with both real
factories and interfaces. Standard-library file I/O is isolated to the non-Windows branch, buffer
initialization no longer transports sizes through invalid enum values, and the aggregate passes
GCC/Clang plus sanitizer execution through the native component manager. The owning document is
now a focused target whose raw-line view drives bounded transactional native `[Libraries]`
configuration. A native process-global `DACOM_Acquire` now owns this manager and registers both
production profile factories once. It also registers the native `IHeap`; the EventSystem proves
one cross-ELF owner while further owners, writable INI behavior and Windows parity remain open; see
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

### Native heap and first cross-ELF owner (2026-09-19)

`frontierx_native_heap` implements the unchanged `IHeap` interface with synchronized tracked native
allocations. The global manager registers it before other factories; `InitializeDAHeap`,
`HEAP_Acquire`, `SetDefaultHeapMsg` and `HEAP` execute through that owner. GCC, Clang and focused
sanitizers cover allocation, zero fill, reallocation, metadata, enumeration, foreign-pointer
rejection and repeated initialization. The production EventSystem ELF uses this same host heap for
its aggregate and queue nodes, restores exact accounting before unload, and hides every C++/RTTI
definition behind a two-entry-point version script. Configured capacity is diagnostic, private
COMHeap pass-through slots are unsupported, and further modules require owner-specific proof. See
[NATIVE_HEAP.md](NATIVE_HEAP.md).

The static EventSystem fallback also compiles with DA-heap allocation enabled. Its probe observes
tracked aggregate/queue blocks and exact baseline restoration, so configured and fallback owners
no longer select different allocation families.

## Native headless startup check (2026-09-13)

The GCC and Clang presets build `frontierx_headless`; its registered process check executes ten
scenarios. Python 3 is required when BUILD_TESTING is enabled.
The following help command was executed:

```text
out/build/linux-gcc-debug/frontierx_headless --help
```

The executable accepts `--data-root DIRECTORY` and an optional `--config FILE`. The latter is a
new native configuration, not retail Conquest.ini: use `[FrontierX]` and `DataRoot=path` with a
final LF newline. Relative config paths resolve against the config file's directory. Selection
order is CLI, FRONTIERX_DATA_ROOT, explicit config, then Data relative to the working directory.
An explicitly supplied invalid configuration always fails, even when another path overrides it.
ASCII/control-character restrictions of the native reader apply; paths are not silently trimmed.

Exit codes: 0 means directory checks passed (or help), 1 invalid arguments, 2 invalid configuration,
3 unusable required data directories, 4 unexpected startup-check failure. Optional directories
produce status lines without failing the process. Reports are deterministic and do not print the
user's absolute installation path. No media contents, metadata, campaign or game startup are
validated. Synthetic tests cover selection precedence, relative configuration paths, malformed
options/configuration, missing roots, case mismatches and optional-media behavior.

### UTF metadata header check (2026-09-13)

`frontierx_headless` additionally accepts `--metadata FILE`, an explicit path resolved from the
working directory. After directory checks, it reads only the 56-byte UTF 1.01 header and validates
signature, version, 44-byte directory stride, aligned directory size, metadata ranges, overlap and
free-entry offset. Fields are decoded as little-endian fixed-width words; native structs are never
mapped onto disk bytes. Invalid metadata returns exit code 5. Omitting the option leaves this check
unperformed. Success does not validate directory entries, names, payloads or GenData archetypes.

The headless process check passes under both compiler presets, including synthetic UTF header
mutations and missing/truncated metadata. No retail files were read.
The format evidence is the source declarations in DOSFile/BaseUTF.h, header validation in
UTFDMan.cpp and the GenData.cpp loadTypesData caller. The UTF inspector is not yet the DOSFile
backend replacement. Its summary is diagnostic only and does not certify a compatible installation.

### UTF directory validation (2026-09-13)

The metadata check now reads directory and used-name tables (combined limit 64 MiB), walks the
reachable tree iteratively, rejects repeated/cyclic/misaligned links, validates NUL-terminated
name references and bounds file allocation/used extents against the container size. The free-entry
chain is checked separately. Source review of UTFDMan.cpp corrected the earlier assumption:
the header's first free-entry offset is absolute; its subsequent dwNext links are directory-relative.
Synthetic tests include a valid free chain and a cyclic one. Unreachable slots and free payload-space
lists are not certified; payload contents/decompression and archetype schemas remain unchecked.

The GCC and Clang headless process checks pass with synthetic UTF trees, invalid links, names and
payload ranges. No retail installation was inspected. This
extends the read-only inspector; it does not yet replace the Windows DOSFile implementation.

The headless checker now accepts `--metadata FILE --entry VIRTUAL_PATH` to read a bounded UTF
entry into memory and report its byte count and diagnostic checksum (2026-09-13). The focused
process scenarios pass under both presets and include a nested `dir/file` entry, exact case-sensitive
lookup and rejection of malformed virtual separators. No schema or retail compatibility is
implied; see the UTF entry-reading
section of the port-readiness plan for limits and outstanding work.

### Portable scalar math core

An isolated `frontierx_math` target now implements the scalar operation surface behind
`I3DMathEngine` using ABI-neutral values and fixed-width inverse-square-root bit handling. Its
GCC/Clang targets disable FMA contraction, and the complete sixteen-test matrices cover the new
probe. The historical K62/3DNow sources own the concentrated assembly findings and are separate
from scalar `x86math.cpp`. The supported Windows wrapper currently reaches `MathLib` but does not
build or verify the provider DLL. The native DACOM adapter, Windows numerical oracle and
floating-point-environment policy remain open; see [MATH_BACKEND.md](MATH_BACKEND.md).

### Portable mesh ray geometry

The production MeshManager hit test now converts named engine positions into a header-only
portable ray-geometry boundary instead of aliasing them as D3DX vectors. GCC and Clang probes cover
the documented box and triangle contract; the full matrices contain sixteen tests. This removes
two D3DX helper functions from one real engine path, but Windows boundary-tolerance comparison and
the remaining renderer replacements are open. See [D3DX9_PORT.md](D3DX9_PORT.md).
