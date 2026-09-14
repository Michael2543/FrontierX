# Linux Port Readiness Plan

Status: implementation in progress, 2026-09-12

This document turns the direction in [PROJECT.md](PROJECT.md) into an executable sequence of
small, falsifiable milestones. Native layout, TinyXML, platform timing, and data-root inspection
targets now exist, but no Linux game or engine-module target exists. Current restoration state
remains in
[HANDOFF.md](../HANDOFF.md), and runtime acceptance evidence remains in
[RETAIL_TEST_MATRIX.md](../RETAIL_TEST_MATRIX.md).

## Evidence Labels

Every completed item in this plan must identify its evidence:

- **Source review**: established by reading the repository only.
- **Compile check**: established by a named compiler and exact command.
- **Automated check**: established by a named script or test and exact command.
- **Runtime check**: established by running named binaries against a lawful retail installation.

An architectural choice, generated build graph, or successful compile is not runtime evidence.

## Current Baseline

Verified by existing project documentation:

- VS2022 Win32 Debug and Final full rebuilds passed on 2026-08-30.
- All 18 retail campaign modules build with the Windows baseline.
- The retail acceptance matrix still contains open restoration and soak tests.
- The CMake build graph contains a dependency-free Linux x86_64 layout probe, an isolated TinyXML
  static library and behavior test, and a standard-C++ platform library with timing and data-root
  behavior tests; no game library, native launcher, or Linux package exists.
- No production graphics, audio, or video dependency has been approved for Linux.
- Runtime media must come from a lawfully obtained retail installation and must remain outside
  source history and release artifacts.

Source review still needs to turn the broad platform inventory into measured call sites and target
dependencies. Until that inventory is complete, component boundaries below are proposals.

## Scope and Gates

The first supported port target is Linux x86_64 ELF. The initial path is single-player only.
macOS, ARM64, multiplayer transport, editor support, packaging, and unrelated gameplay fixes are
outside the bring-up scope.

Work may overlap when it does not weaken evidence, but milestone acceptance is ordered. In
particular:

1. The Windows baseline remains buildable while portable targets are added.
2. Persistent layouts and cross-module ABIs are measured before pointer-width changes.
3. Startup and data loading are measurable before renderer adoption.
4. A renderer is selected only after a maintained candidate passes a focused prototype and legal
   review.
5. Packaging starts only after runtime acceptance and retail-data separation are automated.

## Milestone 0: Freeze the Regression Baseline

**Goal:** make the current Windows behavior a usable comparison point for port work.

Planned work:

- Complete or explicitly defer each immediate item in the retail acceptance matrix.
- Record exact Debug and Final rebuild commands, compiler version, required SDK, output set, and
  hashes for a known baseline revision.
- Re-run campaign symbol and layout checks after affected ABI changes.
- Separate known retail-restoration defects from regressions introduced by the port.

Exit criteria:

- A named baseline revision has reproducible Debug and Final build evidence on Windows.
- Required smoke tests have dated results or a documented reason for deferral.
- The remaining known defects are listed and do not become implicit Linux acceptance criteria.

### Windows ABI Tool Follow-up

Source review on 2026-09-11 found machine-specific paths in `Tools/abicheck/symdiff.ps1` and
`Tools/abicheck/allmods.ps1`. The scripts now take explicit paths, share strict `dumpbin` parsing,
and return failures for missing inputs, parser failures, native-tool failures, and unsatisfied
symbols. The 2026-08-20 audit recorded 153 required `Mission.dll` symbols and two required
`Trim.dll` symbols with zero unsatisfied imports; that historical result is not a current execution
result.

The implemented parameterized interface is:

| Script | Required parameters |
|---|---|
| `symdiff.ps1` | `DumpbinPath`, `RetailMissionDll`, `BuiltMissionDll`, `ScriptDirectory` |
| `allmods.ps1` | `DumpbinPath`, `BuiltGameDirectory`, `ScriptDirectory` |

Implemented behavior:

- reject missing inputs before invoking `dumpbin`;
- support paths containing spaces without embedded machine defaults;
- return a nonzero process status for parse failures, missing built modules, or unsatisfied symbols;
- report the complete required and unsatisfied symbol counts; and
- reject non-Windows hosts before path validation or process execution.

The ABI scripts are intentionally excluded from CMake and CTest. Do not run them through Docker,
Podman, Wine, or Linux PowerShell: attempts to emulate their Windows environment destabilized the
development environment and cannot validate the native PE toolchain. Before accepting Milestone 1,
run both scripts against freshly rebuilt Debug and Final outputs on native Windows with the VS2022
`dumpbin`. These checks protect the Win32 campaign ABI baseline; they do not prove that PE campaign
DLLs can be loaded by the future Linux ELF runtime.

## Milestone 1: Portable Layout Probe

**Goal:** prove the first native Linux compile without pretending that the game itself builds.

This is the first implementation milestone. Its initial CMake/Ninja target contains no windowing,
rendering, audio, retail media, or Windows SDK dependency. Its job is to expose compiler,
data-model, packing, and header-dependency assumptions as early failures.

Implemented slice, with compile and automated-check evidence from 2026-09-11:

- Root CMake configuration and separate GCC and Clang Ninja presets were added for Linux x86_64.
- `frontierx_layout_probe` verifies the LP64 process model, `U8` through `U64`, `S8` through `S64`,
  `BOOL32`, and native pointer size and alignment in `HYBRID`.
- The three legacy `TYPEDEFS.H` copies use fixed-width integer types outside Windows and SGI while
  preserving the existing Windows underlying types and decorated C++ ABI.
- `ResourceCost` and `MISSION_DATA_BIN` now have one portable production owner header. The probe
  checks their sizes and alignments, every addressable field offset, the final-field extent, retail
  mission enum values, and representative x86_64 bitfield encodings.
- Warnings are target-scoped and treated as errors for GCC and Clang.
- The probe is registered with CTest and builds outside the source tree.

The exact successful commands were:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug
```

CMake 4.3.0 with Ninja 1.13.2 identified GCC 16.2.1 and Clang 22.1.8. Both builds produced a native
64-bit x86-64 ELF executable and passed the one registered test.

Remaining work before Milestone 1 acceptance:

- Extend coverage only when another persistent or cross-module type is selected from measured risk;
  do not duplicate production declarations in the probe.
- Continue isolating shared runtime headers from the Win32-only DACOM boundary exposed through
  `DBaseData.h` -> `SuperTrans.h` -> `3dmath.h`.
- Run the unchanged Windows Debug and Final rebuilds and the parameterized ABI checks on Windows.

Exit criteria:

- A clean out-of-tree CMake configure, Ninja build, and CTest run pass on Linux x86_64.
- The probe is a native 64-bit ELF executable.
- Every tested persistent field remains fixed width; native pointers are not forced into 32-bit
  storage.
- The exact commands and toolchain versions are documented from executed checks.
- The supported Windows rebuild path remains unchanged and has follow-up validation evidence.

The fundamental-type and first production-storage checks passed. Direct source review also proved
that the full `DBaseData.h` runtime include graph reaches Win32-only `DACOM.H`; the portable storage
definition was isolated instead of weakening that platform guard. Milestone 1 remains open pending
Windows regression and ABI evidence.

## Milestone 2: Portable Build Graph Skeleton

**Goal:** model a small, real dependency slice before attempting the application.

Initial implemented slice, with compile and automated-check evidence from 2026-09-11:

- `frontierx_tinyxml` builds the four existing TinyXML/TinyString implementation files as a static
  library with no Windows SDK or external link dependency.
- `frontierx_tinyxml_probe` is a separately linked consumer that tests parsing, entity decoding,
  attribute handling, malformed-input errors, and LP64 `unsigned long` conversion.
- GCC 16.2.1 and Clang 22.1.8 build the library and pass its registered CTest with target-scoped
  warnings treated as errors.
- The existing Windows owner is `StringTable.vcxproj`; a full Windows regression build remains
  pending.
- The source snapshot's exact upstream version is unknown and blocks distribution, as recorded in
  [DEPENDENCIES.md](DEPENDENCIES.md).

Remaining work:

- Select the next engine-owned library slice from measured solution dependencies.
- Keep compiler compatibility changes isolated from game and simulation logic.
- Validate the unchanged `StringTable` owner through a full Windows rebuild.
- Resolve TinyXML's exact upstream-version provenance before any distribution.

Exit criteria:

- The selected library and its test build on Linux with no Windows SDK dependency.
- Its Windows owner and validation path are documented.
- No global include-path, warning suppression, packing, or type-width workaround hides defects in
  unrelated targets.

## Milestone 3: Platform Core and Headless Startup

**Goal:** reach deterministic initialization and retail-data discovery without graphics or audio.

Initial implemented slice, with compile and automated-check evidence from 2026-09-12:

- `frontierx_platform` provides monotonic microseconds and 32-bit milliseconds, clamped elapsed
  time, millisecond sleep, and thread yield through portable C++17 facilities without direct Win32
  or POSIX calls.
- `frontierx_platform_time_probe` verifies deterministic elapsed-time clamping and that the
  monotonic clock advances across a timed wait.
- GCC 16.2.1 and Clang 22.1.8 build and link the library and pass the behavior test with
  target-scoped warnings treated as errors.
- The modal UI frame clock and progress-animation clock and sleep in `Trim` are the first production
  consumers. The portable implementation is included in the VS2022 `Trim` project without changing
  its runtime-library policy.
- `frontierx_data_root_probe` checks the seven required and five optional directory paths identified
  in `MenuResource::updateLoadPath`. It reports exact matches, case mismatches, missing paths,
  ambiguous case-folded matches, and non-directories without reading file contents. Only required
  path failures make the root unusable; synthetic tests cover the optional-warning behavior.
- Its Linux data-root selection order is command-line path, `FRONTIERX_DATA_ROOT`, then the existing
  `Data` relative-path default. Synthetic process checks verified environment selection and CLI
  precedence without changing the Windows registry-backed `DataPath` behavior.
- Its self-test uses temporary empty directories. A separate source-tree check reported four
  required and five optional case mismatches, plus missing `Profiles`, `Maps/Campaign`, and
  `Maps/Multiplayer`; this is not retail-installation evidence.
- Networking, simulation pacing, random seeds, audio timing, and `Modal.cpp`'s legacy profiler
  counters are not migrated by this slice.

Remaining work:

- Define focused boundaries for process startup, diagnostics, filesystem paths, configuration,
  dynamic loading, and additional threading operations as concrete consumers require them.
- Validate the migrated `Trim` consumers in a native Windows Debug and Final rebuild before
  selecting another timing consumer.
- Run the data-root inspector against a lawfully obtained retail installation and record only the
  resulting directory-status summary, never retail files or private installation paths.
- Keep Win32 behavior behind a Windows implementation while adding Linux implementations.
- Replace registry-only configuration with a portable policy without changing established Windows
  defaults silently.
- Resolve case-sensitive includes and runtime asset paths using exact on-disk spelling.
- Load and validate a minimal set of retail metadata from a user-supplied installation path without
  copying media into the repository or build tree.

Exit criteria:

- A native Linux executable initializes core services, locates lawful external data, reports a
  deterministic diagnostic summary, and shuts down cleanly.
- Missing or incompatible data produces an actionable error and a nonzero exit status.
- No window, renderer, audio device, or network service is required for this check.

## Milestone 4: SDL3 Window, Events, and Input

**Goal:** establish the first interactive native process while keeping platform calls isolated.

Planned work:

- Complete dependency provenance, license, version, and redistribution review for SDL3 before
  integration.
- Add SDL3 window creation, event pumping, keyboard/mouse translation, focus, and shutdown behind
  the platform boundary.
- Define timing behavior explicitly so event-loop changes do not alter simulation stepping or RNG
  consumption.
- Preserve a Windows implementation until parity is measured and a deliberate consolidation
  decision is recorded.

Exit criteria:

- Linux opens and closes a responsive window on supported display environments.
- Input translation and focus transitions have automated checks where practical.
- A runtime smoke test records the exact display backend and environment.

## Milestone 5: Rendering Vertical Slice

**Goal:** render one representative, inspectable game scene before broad renderer migration.

Decision work before adoption:

- Inventory D3D9 and D3DX9 usage by feature: device/state management, effects, shader compilation,
  texture helpers, mesh/math helpers, and diagnostics.
- Evaluate maintained native D3D9-to-Vulkan candidates with a pinned prototype, including license,
  maintenance health, required patches, and redistribution constraints.
- Reject the archived `misyltoad/dxvk-native` project as a production dependency unless a new,
  documented decision supersedes current policy.
- Record a decision if an adapter around the existing D3D9-facing renderer is not viable before
  proposing a direct renderer rewrite.

Exit criteria:

- The selected path creates a device, uploads representative geometry and textures, presents
  frames, resizes, and shuts down on Linux.
- The vertical slice uses actual supported game data supplied externally, with no retail media in
  repository or build outputs.
- Captures compare geometry orientation, material binding, depth, blending, and resource lifetime
  against the Windows baseline.
- Unsupported D3DX9 facilities have explicit replacement owners and tests.

## Milestone 6: First Playable Single-Player Slice

**Goal:** start a known mission and complete a narrow gameplay path natively.

Planned work:

- Integrate campaign/module loading without changing fixed-width or decorated-ABI contracts
  silently.
- Bring up game input, simulation stepping, UI, save-path policy, and required rendering features.
- Compare simulation checkpoints, RNG consumption where observable, and serialized bytes with the
  Windows baseline.
- Expand the retail acceptance matrix with separate Linux columns and evidence links.

Exit criteria:

- A named retail mission reaches a defined checkpoint through normal input on Linux.
- Save and reload preserve the same tested state.
- Known visual differences and behavioral differences are separately recorded.
- No Wine runtime is involved.

## Milestone 7: Audio and Video

**Goal:** restore required media services after gameplay and rendering are measurable.

Planned work:

- Inventory sound semantics before choosing a portable audio backend.
- Evaluate audio candidates for behavior, maintenance, license, and distribution compatibility.
- Replace Bink playback with an independently implemented FFmpeg-based path only after the same
  dependency review and a focused playback prototype.
- Keep proprietary codecs, binaries, and extracted media out of the repository and packages.

Exit criteria:

- Music, effects, speech, and representative cinematics play from external retail media with
  documented parity results.
- Missing optional media fails gracefully without corrupting gameplay state.
- All shipped dependencies have recorded provenance and redistribution approval.

## Milestone 8: Acceptance, Hardening, and Packaging

**Goal:** turn the bring-up into a reproducible, distributable non-commercial build.

Planned work:

- Complete the Linux retail acceptance matrix, including startup soak, representative campaign
  missions, save/load, rendering, audio/video, and clean shutdown.
- Add clean configure/build/test documentation based only on commands actually executed.
- Automate exclusion of retail media, SDK redistributables, unapproved binaries, traces, dumps,
  and local paths.
- Include the complete controlling license, legal policy, and current modification record.
- Defer portable multiplayer transport until deterministic single-player acceptance is stable.

Exit criteria:

- A clean machine can reproduce the native Linux binaries from documented prerequisites.
- Package inspection proves that no retail media or unapproved binary is present.
- The package runs against a user-selected lawful retail installation.
- Required notices and dependency records are complete.

## Initial Blocker Register

| Blocker | Evidence now | Next falsifiable check | Target milestone |
|---|---|---|---|
| No portable game or engine-module build graph | Layout and TinyXML targets pass | Map the first engine-owned library slice | 2-3 |
| Parameterized ABI checks lack native Windows execution evidence | Static source validation only | Run both checks against fresh Debug and Final builds on native Windows | 0-1 |
| Win32/MSVC assumptions in shared headers | Source-review requirement | Compile selected headers with GCC and Clang | 1-2 |
| 32-bit runtime assumptions versus LP64 | Known compatibility invariant | Turn pointer casts and layout assumptions into probe failures | 1-3 |
| Legacy solution dependency graph is unmapped | Source-review requirement | Map one selected library and link a real consumer | 2 |
| Startup/configuration are Windows-owned | Project source map | Run a headless Linux initialization path | 3 |
| Linux path case and asset lookup are unmeasured | Known platform difference | Scan, then exercise external data discovery on a case-sensitive filesystem | 3 |
| SDL3 integration is planned, not approved | Architecture direction | Complete provenance review and focused window prototype | 4 |
| Native D3D9-to-Vulkan path is undecided | Architecture direction | Run and document one maintained-candidate prototype | 5 |
| D3DX9 replacement surface is unmeasured | Build dependency evidence | Produce feature-level inventory and compile focused replacements | 5 |
| Campaign DLL model may not map directly to ELF | Windows ABI evidence only | Design and test one module-loading/ABI strategy | 6 |
| Audio backend is undecided | Architecture direction | Inventory semantics and prototype selected candidate | 7 |
| Bink cannot be distributed as the Linux solution | Legal policy | Prototype FFmpeg playback without proprietary binaries | 7 |
| Release dependency provenance is incomplete | Legal policy | Complete dependency records before packaging | 8 |

## Immediate Work Queue

1. Characterize the in-memory profile-parser core selected by the measured
   [build declaration inventory](BUILD_GRAPH.md) before extracting the next engine-owned slice.
   The 2026-09-13 scan covers 29 projects; imported MSBuild logic remains unevaluated.
2. Record Win32, MSVC-extension, pointer-cast, packing, inline-assembly, D3D9, D3DX9, audio, video,
   dynamic-loading, and registry touchpoints by owning subsystem.
3. Validate the unchanged Windows rebuild and parameterized ABI checks on native Windows before
  accepting Milestone 1; do not emulate this check on Linux.
4. Update this plan with measured counts, selected target ownership, exact commands, and dated
   evidence.

## Decision Records Required

The following choices need explicit evidence before they become architecture:

- first production library target and dependency boundary;
- compiler support matrix and warning policy;
- runtime module-loading strategy for campaign and engine libraries;
- SDL3 acquisition and redistribution method;
- native graphics translation or replacement path;
- D3DX9 replacements by feature;
- portable audio backend;
- FFmpeg acquisition, codec scope, and redistribution method; and
- installation-data discovery and user configuration policy.

All dependency decisions must satisfy [LEGAL.md](../LEGAL.md) and the unmodified
[Conquest source license](../Conquest%20Source%20License.txt). Technical success alone does not
approve a dependency or release artifact.
## Profile parser extraction evidence (2026-09-13)

The first engine-owned parser slice is implemented in `FrontierX/ProfileBuffer.h` and reused
by DACOM's existing ProfileParser.cpp. It contains the original section marker and navigation
algorithms; local strlen lengths use size_t. Public interfaces, object fields, handle encodings,
file access and allocator ownership are unchanged. No disk or module ABI type was extracted.

The source-reviewed buffer contract is text at offset one in a zero-filled allocation of
text length plus eight. Section markers mutate that storage; returned pointers borrow it.
The core must not receive arbitrary unpadded buffers or unvalidated external pointers.

Compile and automated evidence: both documented GCC/Clang preset configure/build/test sequences
passed five CTests. Synthetic cases cover empty text, absent/final/malformed/duplicate sections,
CRLF and LF, spaces versus tabs, comments preserved for callers, blank lines, end-of-section
boundaries and a final line without newline. Blank CRLF handling is deliberately preserved,
including the boundary case immediately before another section.

A separate Clang AddressSanitizer/UndefinedBehaviorSanitizer run completed the probe body without
reported access/UB errors, but exited unsuccessfully because LeakSanitizer cannot run under the
host's ptrace environment. This is not a clean sanitizer pass; repeat outside that environment.

Remaining parser work: characterize key lookup and output truncation, then extract those operations;
characterize case matching before replacing the MSVC locale-sensitive comparison functions.
Enumeration contains a source-observed null-pointer strlen path for a value without a newline;
do not silently fold an unrelated behavior repair into the navigation extraction. Full DACOM
compilation, native Windows Debug/Final regression and ABI checks remain pending. Milestones 1-3
are not marked accepted by these Linux-only results.

### Profile lookup and output follow-up (2026-09-13)

The shared core now also provides FindValue and CopyLine, used by the production ReadKeyValue
and ReadProfileLine adapters. FindValue receives the existing Windows comparison and symbol
classification functions from its caller; no replacement locale policy is imposed on Windows.
Pointer differences use size_t internally, while the public U32/HANDLE signatures remain unchanged.
Section handles remain runtime offsets; this does not establish a portable DACOM handle ABI.

Synthetic tests verify first-match duplicate keys, underscore key boundaries, section isolation,
missing and oversized keys, caller-selected case matching, literal inline comments, empty values,
CRLF versus bare CR, truncation, null-output length queries, capacities zero/one, and an output
canary. Zero capacity now returns zero without writing, replacing the legacy unsigned underflow.
Valid nonzero-capacity empty values retain the original single-space sentinel.

Both GCC and Clang passed all five CTests after extraction. A separate Clang UBSan-only build
(`-fsanitize=undefined -fno-sanitize-recover=all`) passed the profile probe; this does not supersede
the earlier unsuccessful LeakSanitizer run or prove arbitrary-input memory safety. Remaining work
includes an owning portable configuration reader, explicit Linux character policy, enumeration,
and native Windows rebuild/ABI validation before accepting the DACOM integration.

### Owning native profile reader (2026-09-13)

ProfileDocument is implemented in the portable library. It owns padded parser storage, returns
copied section names and optional values, and exposes no pointer handles. File loading uses
standard C++ binary streams with a 1 MiB cap, rejects nonregular files and embedded NUL/non-ASCII
input, and preserves the previous document on validation/read failure. Case-insensitive ASCII
matching is an explicit native-startup policy, not a claim of parity for localized retail text.
Empty documents are accepted; this reader intentionally does not turn permissive legacy parsing
into an INI schema validator.

Both GCC and Clang configured, compiled and passed six CTests. Synthetic tests cover real temporary
file loading, missing paths, directory rejection, transactional failure, copied-document ownership,
case folding, duplicate section precedence, missing/empty values and invalid/oversized text.
Tests retain the legacy CR at a section boundary where the marker replaces the preceding LF.
No user installation or retail data was read. Remaining work is connecting configuration and data
root inspection into a headless startup entry point, defining required metadata, and validating
against a lawful installation. Key enumeration and the full Windows DACOM regression remain open.

## Native headless startup check (2026-09-13)

The GCC and Clang presets build `frontierx_headless` and pass seven CTests; the headless CTest
contains six process-level scenarios. Python 3 is required when BUILD_TESTING is enabled.
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

Milestone 3 remains open pending metadata loading and lawful-installation evidence. SDL3,
rendering, campaign loading and playable runtime milestones remain unimplemented.

### UTF metadata header check (2026-09-13)

`frontierx_headless` additionally accepts `--metadata FILE`, an explicit path resolved from the
working directory. After directory checks, it reads only the 56-byte UTF 1.01 header and validates
signature, version, 44-byte directory stride, aligned directory size, metadata ranges, overlap and
free-entry offset. Fields are decoded as little-endian fixed-width words; native structs are never
mapped onto disk bytes. Invalid metadata returns exit code 5. Omitting the option leaves this check
unperformed. Success does not validate directory entries, names, payloads or GenData archetypes.

Both preset builds passed seven CTests; the headless suite now has seven process scenarios,
including synthetic UTF header mutations and missing/truncated metadata. No retail files were read.
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

GCC and Clang passed all seven CTests, including eight headless process scenarios with synthetic
UTF trees, invalid links, names and payload ranges. No retail installation was inspected. This
extends the read-only inspector; it does not yet replace the Windows DOSFile implementation.

### Native UTF entry reading (2026-09-13)

ReadUtfEntry validates metadata and reads a selected entry on the same open stream, returning
owned bytes only after success. Exact case-sensitive virtual paths use '/' without a leading
separator. No path is written to the host filesystem. Entry reads are capped at 64 MiB and virtual
paths at 4096 bytes. Unequal stored/logical lengths are rejected until decoding semantics are
implemented; this is not a claim that every such entry uses compression. DOSFile/UTF.cpp reads
from data-start plus entry-offset using the logical size, which remains the source evidence base.

The headless executable accepts `--metadata FILE --entry VIRTUAL_PATH` and reports byte count plus
a diagnostic FNV-1a checksum, never content. This checksum is not the gameplay checksum. Both
compiler presets passed seven CTests; synthetic entry reads verified the exact `abc` byte checksum,
missing case-sensitive names and unsupported size relationships. Nested paths and broader real
container coverage still require additional tests. Archetype schema interpretation and the engine
IFileSystem adapter remain outstanding. No retail files were accessed or copied.
