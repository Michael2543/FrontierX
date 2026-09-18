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

The 2026-09-16 lexical [platform touchpoint inventory](PLATFORM_INVENTORY.md) measures call-site
candidates by category, file, and owning subsystem. It narrows the next audits, but does not replace
compile-derived target dependencies or prove that any proposed component boundary is sufficient.

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
- A portable `EventQueue` reproduces the source-reviewed `IEventSystem` `Post`/`Update`/`Peek`
  batch behavior without DACOM, Windows headers or serialized pointer surrogates. Its GCC/Clang
  probe covers callback-time posting and last-completed-batch visibility.
- The production DAHOTKEY `EventSystem` owner now compiles and executes as a native DACOM aggregate
  closure. Its factory, interface maps, connection point, `Post`/`Update`/`Peek` and direct `Send`
  paths pass GCC, Clang and focused sanitizer checks. A separate SDL pump drains synthetic batches
  synchronously into this owner; application/game-loop lifecycle and cadence remain open.
- `NativeComponentManager` implements the existing `ICOManager` registration, priority, creation,
  enumeration, reference and shutdown rules, hosts that production event owner, and loads native
  ELF factories through explicit registration/liveness entry points. Its bounded read-only
  `SetINIConfig` applies exact config-relative `[Libraries]` entries transactionally. The production
  EventSystem is the first configured ELF component with instance-counted unload gating. The
  process-global `DACOM_Acquire` bootstrap registers a process-local native `IHeap` before both
  production profile factories. Cross-ELF allocation ownership and legacy COMHeap behavior remain
  unimplemented.
- A portable `PreferenceStore` provides bounded string and binary values beneath the isolated
  native settings adapters. Its fixed-width `FXSET001` file rejects invalid keys, types, sizes,
  duplicate/truncated/trailing data and preserves the previous in-memory state after a failed load.
  Serialization is deterministic and uses same-directory temporary-file replacement. The exact
  format, source-reviewed registry behavior and remaining gates are in
  [NATIVE_SETTINGS.md](NATIVE_SETTINGS.md).
- `LegacyPreferenceValues` reproduces the value-oriented `IUserDefaults` type, byte-count,
  short-buffer, deletion and four-entry MRU contracts over that store. The real interface header
  compiles under GCC and Clang through the DACOM probe while retaining its MSVC decoration.
- `NativeUserSettings` maps the raw default fields into 25 required versioned values and 31
  explicitly assigned flag bits. The conversion rejects partial, wrong-type, non-finite,
  unknown-flag and unsupported-version input transactionally; headless process tests cover absent,
  complete and incomplete schemas.
- `StartupPreferences` now drives `NativeUserSettingsOwner` in both headless startup and the native
  shell with shared path selection, player/global/default selection, hex-encoded player namespaces,
  missing-player reset, hardware-cursor preservation, invalid-player cleanup/global fallback and
  transactional global/player stores.
- `frontierx_native_user_defaults` implements the unchanged real interface as a DACOM service,
  maps all live fields/flags without serializing the legacy struct, persists named values and MRU
  mutations transactionally, and performs the shell's global/player shutdown save. Its UI,
  installation-metadata and EULA methods fail explicitly until portable owners exist.

Remaining work:

- Define focused boundaries for process startup, diagnostics, filesystem paths, configuration,
  dynamic loading, and additional threading operations as concrete consumers require them.
- Validate the migrated `Trim` consumers in a native Windows Debug and Final rebuild before
  selecting another timing consumer.
- Run the data-root inspector against a lawfully obtained retail installation and record only the
  resulting directory-status summary, never retail files or private installation paths.
- Keep Win32 behavior behind a Windows implementation while adding Linux implementations.
- Select the tested live native `IUserDefaults` component from a real game-startup owner; port or
  isolate its file-dialog, document/viewer, installation-metadata and EULA collaborators; and finish
  fsync/concurrent-write durability policy without changing established Windows registry defaults
  or persisting the raw native `USER_DEFAULTS` layout.
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

Implemented integration slice, with compile and automated-check evidence from 2026-09-18:

- `frontierx_sdl3_event_pump` drains every translated event currently available from an owned
  `SdlWindow`, retains modifier/button translation state across calls, and uses synchronous
  `IEventSystem::Send` delivery so temporary compatibility-message pointers never enter a queue.
- The returned batch result distinguishes an owned-window close request from SDL's global quit
  request without taking shutdown ownership.
- The production-event subscriber test injects a foreign-window resize, owned resize, UTF-8 text
  and global quit, then proves filtering and ordered resize/Unicode/character/close delivery with
  both GCC and Clang under the SDL dummy driver.
- `frontierx_native_shell` composes the headless checker's shared native configuration/data-root
  and preference owners, required-directory validation, static component registration,
  real `IUserDefaults`, production event-system initialization, SDL window/text-input lifetime,
  bounded pumping, settings persistence and dependency-ordered teardown in one native process.
  Synthetic command-line, environment and
  failure scenarios pass under GCC, Clang and focused sanitizers.
- The exact lifetime contract and remaining owner gates are in
  [SDL_EVENT_PUMP.md](SDL_EVENT_PUMP.md); the process lifecycle and explicit non-game scope are in
  [NATIVE_SHELL.md](NATIVE_SHELL.md).

Exit criteria:

- Linux opens and closes a responsive window on supported display environments.
- Input translation and focus transitions have automated checks where practical.
- A runtime smoke test records the exact display backend and environment.

The first and third criteria have process evidence from the earlier Wayland smoke and the native
shell's dummy-driver suite. Input translation has synthetic coverage. Milestone 4 remains open for
application update/render cadence, Windows behavior comparison, lawful retail-path execution and
distribution approval; the shell is not the game.

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
| No portable game or engine-module build graph | Platform services plus an integrated native lifecycle shell build; no game owner compiles | Compile the first game/renderer-owned closure without weakening Windows | 2-5 |
| Parameterized ABI checks lack native Windows execution evidence | Static source validation only | Run both checks against fresh Debug and Final builds on native Windows | 0-1 |
| Win32/MSVC assumptions in shared headers | 150 direct-header matches and 1,379 extension matches measured lexically | Compile selected owner closures with GCC and Clang | 1-2 |
| 32-bit runtime assumptions versus LP64 | Both measured object/address truncations and encoded DOSFile-handle casts are eliminated in source | Validate all affected modules and interfaces on native Windows, then continue the wider implicit-conversion audit | 1-3 |
| Legacy solution dependency graph is unmapped | Source-review requirement | Map one selected library and link a real consumer | 2 |
| Production game startup owner remains Windows-bound | Headless and shell initialization, shared profile callbacks, native preferences and data-root checks pass | Compile and connect the first real game-startup owner without changing Windows registry behavior | 3 |
| Linux path case and asset lookup are unmeasured | Known platform difference | Scan, then exercise external data discovery on a case-sensitive filesystem | 3 |
| SDL3 integration is locally prototyped but not approved for distribution | System package, neutral backend, production-event batch pump, native DACOM `IWindowManager`, shell integration and Wayland smoke | Connect real application cadence, compare Windows behavior, and review provenance/notice packaging | 4 |
| Native D3D9-to-Vulkan path is undecided | Architecture direction | Run and document one maintained-candidate prototype | 5 |
| D3DX9 replacement remains incomplete | 155 lexical matches in eight files after production ray-geometry replacement | Characterize and replace shared renderer matrices/tangents, effects and cubemap decoding | 5 |
| Campaign DLL model may not map directly to ELF | Native DACOM ELF registration/unload passes a synthetic module test; campaign ABI remains Windows-only and separate | Define and test one campaign C export/ABI strategy | 6 |
| Audio backend is undecided | Architecture direction | Inventory semantics and prototype selected candidate | 7 |
| Bink cannot be distributed as the Linux solution | Legal policy | Prototype FFmpeg playback without proprietary binaries | 7 |
| Release dependency provenance is incomplete | Legal policy | Complete dependency records before packaging | 8 |

## Immediate Work Queue

1. Select the tested live native `IUserDefaults` component from the first real game-startup owner;
   add its portable UI/install/EULA collaborators and durability policy without changing supported
   Windows defaults; the shared DACOM callback traversal still requires native Windows comparison.
2. Validate the migrated DACOM runtime-offset, connection-token, native-manager and production
   `EventSystem` contracts on native Windows; adapt one real ELF component to the tested explicit
   registration/liveness contract after defining profile ordering and heap ownership.
3. Validate the completed particle, RichEdit, DirectShow, hotkey-message, and stack-guard
   migrations across their affected native Windows module and callback boundaries.
4. Validate the completed IFF/UTF/SharedUTF pointer-width token and explicit serialized-context
   migrations with native Windows compile coverage and DOSFile behavioral tests.
5. Adapt the isolated scalar math core to `I3DMathEngine` after capturing Windows provider-loading
   and numerical-oracle evidence. The files containing 402 of 441 measured assembly matches belong
   to the separate historical K62/3DNow provider and are not first-target requirements.
6. Continue the classified D3DX9 replacement with shared renderer matrix/tangent ownership before
   choosing an effects or translation strategy.
7. Validate the unchanged Windows rebuild and parameterized ABI checks on native Windows before
   accepting Milestone 1; do not emulate this check on Linux.

The completed owner-grouped inventory, exact reproduction commands, limitations, counts, and
ordered interpretation are recorded in [PLATFORM_INVENTORY.md](PLATFORM_INVENTORY.md). Imported
MSBuild logic remains unevaluated by the separate 29-project build-declaration scan.

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
and Windows file access are unchanged. The native branch now supplies transactional standard-library
file reads, while disabled-DA-heap allocation remains paired through the same class operators. No
disk or module ABI type was extracted.

The source-reviewed buffer contract is text at offset one in a zero-filled allocation of
text length plus eight. Section markers mutate that storage; returned pointers borrow it.
The core must not receive arbitrary unpadded buffers or unvalidated external pointers.

Current compile and automated evidence: both documented GCC/Clang preset build/test sequences
pass twenty-eight CTests. Synthetic cases cover empty text, absent/final/malformed/duplicate sections,
CRLF and LF, spaces versus tabs, comments preserved for callers, blank lines, end-of-section
boundaries and a final line without newline. Blank CRLF handling is deliberately preserved,
including the boundary case immediately before another section.

A focused AddressSanitizer/UndefinedBehaviorSanitizer run passes with leak detection disabled
because LeakSanitizer cannot operate under the host's ptrace environment.

Key lookup, output truncation, section/key callback traversal, and the source-observed null-pointer
`strlen` path for a final value without a newline are now extracted and characterized. The
production adapter retains its MSVC comparison path and uses explicit ASCII folding natively. The
real factories, aggregate, `IProfileParser` and `IProfileParser2` now execute through
`NativeComponentManager`, including native file loading and transactional failure retention.
`Initialize2` no longer passes a buffer size through an invalid enum value. The global native
bootstrap registers both factories once. The next production ELF component, writable INI behavior, native Windows Debug/Final
regression and ABI checks remain pending.
Milestones 1-3 are not marked accepted by these Linux-only results.

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

Both GCC and Clang pass all twenty-eight CTests after extraction. Focused AddressSanitizer and
UndefinedBehaviorSanitizer runs pass with leak detection disabled. The owning portable reader,
explicit Linux character policy, and enumeration are now implemented; native Windows rebuild,
behavioral comparison and ABI validation remain required before accepting the DACOM integration.

### Owning native profile reader (2026-09-13)

ProfileDocument is implemented in the portable library. It owns padded parser storage, returns
copied section names and optional values, and exposes no pointer handles. File loading uses
standard C++ binary streams with a 1 MiB cap, rejects nonregular files and embedded NUL/non-ASCII
input, and preserves the previous document on validation/read failure. Case-insensitive ASCII
matching is an explicit native-startup policy, not a claim of parity for localized retail text.
Empty documents are accepted; this reader intentionally does not turn permissive legacy parsing
into an INI schema validator.

Both GCC and Clang configured, compiled and pass twenty-eight CTests. Synthetic tests cover real temporary
file loading, missing paths, directory rejection, transactional failure, copied-document ownership,
case folding, duplicate section precedence, missing/empty values and invalid/oversized text.
Tests retain the legacy CR at a section boundary where the marker replaces the preceding LF.
No user installation or retail data was read. Configuration and data-root inspection are connected
to the headless startup checker, and key enumeration is shared with the DACOM callback core.
Validation against a lawful installation and the full Windows DACOM regression remain open.

### Native profile key enumeration (2026-09-16)

`ProfileDocument::Keys` now provides copied key names for a named section without exposing parser
pointers or legacy section handles. It preserves source order and duplicate keys, matches section
names with the reader's explicit ASCII case-folding policy, ignores comment markers in the key
portion, and retains valid keys whose values contain semicolons. It now consumes the same
`ProfileBuffer::EnumerateKeys` walk as production `ProfileParser2::EnumerateKeys`; the Windows
adapter continues to supply its existing symbol policy.

The existing `profile_document_probe` now covers key enumeration, duplicate ordering, comment
filtering and inline semicolons. The focused GCC configure/build/CTest check passed on 2026-09-16:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_profile_document_probe
ctest --preset linux-gcc-debug -R '^portability\.profile_document$'
```

The production callback adapter now calls the shared section/key enumerators, its runtime section
offsets use the pointer-width encoded-handle contract, and `IProfileParser.h` compiles through the
existing portable calling-convention macro. The final unterminated-value null dereference is fixed
and covered by GCC, Clang and sanitizer tests. Native Windows Debug/Final rebuild, callback
comparison, ABI checks and retail configuration compatibility remain open. See
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

## Native headless startup check (2026-09-13)

The GCC and Clang presets build `frontierx_headless` and pass twenty-eight CTests; the headless CTest
contains ten process-level scenarios. Python 3 is required when BUILD_TESTING is enabled.
The following help command was executed:

```text
out/build/linux-gcc-debug/frontierx_headless --help
```

The executable accepts `--data-root DIRECTORY`, optional `--config FILE`, and optional
`--preferences FILE`. The config is new native configuration, not retail Conquest.ini: use
`[FrontierX]` and `DataRoot=path` with a final LF newline. Relative config paths resolve against
the config file's directory. Data-root selection order is CLI, `FRONTIERX_DATA_ROOT`, explicit
config, then `Data` relative to the working directory. An explicitly supplied invalid
configuration always fails, even when another path overrides it. ASCII/control-character
restrictions of the native reader apply; paths are not silently trimmed.

Preference selection is an explicit CLI path, then an absolute `XDG_CONFIG_HOME`, then
`$HOME/.config/frontierx/preferences.dat`. Relative XDG and HOME values are ignored. A missing
default file represents first-run empty preferences; a missing explicit file, invalid format, I/O
failure, or unavailable base directory returns exit code 6. Diagnostics report source, status and
entry count without printing the resolved path.

Exit codes: 0 means checks passed (or help), 1 invalid arguments, 2 invalid configuration,
3 unusable required data directories, 4 unexpected startup-check failure, 5 invalid UTF metadata
or entry, and 6 unusable preferences. Optional media directories and a missing default preference
file do not fail. Reports are deterministic and do not print the user's absolute installation or
settings path. No media contents, campaign, renderer or game startup are validated. Synthetic
process tests cover data/config precedence, relative configuration paths, XDG/default/explicit
preference selection, valid and invalid preference files, malformed options/configuration, missing
roots, case mismatches, optional media, and UTF metadata/entry validation.

Milestone 3 remains open pending the application settings adapter, lawful-installation evidence
and connection of these services to a native game initialization graph. SDL3 is isolated but not
connected; rendering, campaign loading and playable runtime milestones remain unimplemented.

### SDL3 window and input backend probe (2026-09-16)

The CMake graph now builds `frontierx_sdl3_platform` and `frontierx_sdl3_window_probe` through the
system `pkg-config` module `sdl3` (version 3.4.16). `SdlWindow` owns video-subsystem and window
lifetime. Its public `WindowEvent` values contain no SDL types and copy text input immediately.
The backend translates close/quit, focus, logical and physical keyboard values, key modifiers and
repeat, UTF-8 text, mouse position/deltas/buttons, normalized wheel direction, and logical/pixel
resize notifications. It retains SDL nanosecond timestamps for ordering and diagnostics but does
not use them to advance simulation.

SDL3 is guarded by `FRONTIERX_BUILD_SDL3`, which defaults off for an ad-hoc CMake configure and is
explicitly enabled by both checked-in Linux presets. An SDL-disabled, testing-disabled configure
built `frontierx_headless` successfully in a separate `/tmp` build directory, confirming that the
dependency is not imposed on core/headless consumers.

CTest injects representative events while using `SDL_VIDEODRIVER=dummy`; GCC and Clang both
configured, built and pass the complete twenty-eight-test matrix. Separately, the GCC
probe's `--visible-smoke` mode opened a visible 320x200 window on the real desktop, reported SDL's
`wayland` driver, pumped and translated events for 250 ms and shut down cleanly. That is runtime
evidence for the local Wayland window/event loop only, not extended user-input parity or
cross-desktop support.
The focused probe also built and passed in a separate `/tmp` GCC build with
`-fsanitize=undefined -fno-omit-frame-pointer`; it emitted no undefined-behavior diagnostic.

Milestone 4 remains open. The compatibility records now traverse the production `IEventSystem` and
a real DACOM subscriber synchronously in the probe, including window-message and immediate Unicode
payloads. The real public `IWindowManager` declaration compiles under GCC and Clang with
pointer-width message values, and a native SDL-backed factory now owns the shell's window and
callback pump. The production game application loop, live hotkey component and game state remain
unconnected. Focus ordering, UTF-16/character policy and the broader key table still require a
native Windows A/B test.
SDL3 acquisition and license-notice packaging are not approved for distribution, and no renderer
is attached to the window.

### SDL-to-legacy input compatibility adapter (2026-09-16)

Source review found that the productive window/input consumers read four logical fields from
`MSG`: message, window handle, `wParam` and `lParam`. The hotkey implementation additionally reads
the extended-key, Alt-context, previous-state and transition bits. Mouse/game controls consume the
Win32 button/modifier mask, signed 16-bit coordinate halves and signed wheel delta. No consumer
evidence justified reproducing the complete host `MSG` binary layout.

`LegacyWindowMessage` therefore stores only those fields with pointer-width parameter types.
`SdlLegacyInputAdapter` maps SDL keycodes to the observed virtual-key surface, supplies set-1 scan
values for common keys, distinguishes right Ctrl/Alt and extended navigation keys, and preserves
repeat/up/context bits. Mouse translation maintains current button/modifier state, recreates the
existing `System.cpp` move/button/move sequence and its extra double-click notification, normalizes
wheel delta to the legacy 120-unit convention, and packs coordinates like the current consumers.
Logical resize, close, focus and activation records are also produced. UTF-8 text is validated and
converted into UTF-16 code units for paired `CQE_UNICHAR`-compatible immediate values and character
messages, including surrogate pairs.

The adapter returns owning records and also provides synchronous dispatch. For window messages,
the callback parameter points into the owning record and is valid only for that call; it must be
sent synchronously and never posted into `EventQueue`. Immediate Unicode values retain the legacy
integer-as-parameter convention without narrowing on x86_64. The focused GCC and Clang test covers
the field/sequence rules above, and the SDL window probe exercises focus and repeated-key mapping
through the complete SDL-event to compatibility-record path.

`DispatchLegacyInputToEventSystem` supplies the narrow synchronous bridge to the production event
owner. The integration probe creates that owner through `NativeComponentManager`, subscribes a real
`IEventCallback`, and verifies resize plus immediate-Unicode/character payloads across the complete
adapter/connection-point path. `SdlEventPump` now owns complete queue draining and persistent adapter
state in a separate integration target. Its injected batch additionally proves foreign-window
filtering and ordered resize/text/quit delivery, but it does not establish application cadence or
game-input parity. See [SDL_EVENT_PUMP.md](SDL_EVENT_PUMP.md).

Mouse coordinate and wheel conversions reject non-finite values before float-to-integer conversion
and clamp finite values to their signed 16-bit compatibility fields. The probe includes NaN and
infinity coverage. A separate GCC UndefinedBehaviorSanitizer build passed without a diagnostic.

This is compile, automated-check and short Wayland smoke evidence. Exact event ordering across
focus transitions, locale/layout-dependent keys, IME behavior, long-running interaction and
hotkey-database parity remain unverified until the adapter is connected and compared with native
Windows. No simulation update or RNG path consumes these timestamps or events yet.

### Portable event queue core (2026-09-16)

Source review of `Code/Libs/Src/DAHOTKEY/EventSys.cpp` established the queue contract before
implementation. `Post` appends a message and non-owning `void *` parameter to a pending FIFO.
`Update` dispatches that FIFO, including messages appended by callbacks during the same update,
then makes the complete batch visible to `Peek` and retires the previously delivered batch. An
empty update therefore clears the earlier `Peek` view. `Send` bypasses this queue entirely.

`FrontierX::EventQueue` implements only that contract using pointer-width runtime parameters and
fixed-width 32-bit message values. The probe checks pre-update invisibility, FIFO order, a nested
post during dispatch, parameter identity, old-batch visibility until the next update, null-dispatch
publication and empty-update clearing. GCC and Clang compile and pass the registered CTest.
A separate SDL-disabled GCC build ran the probe with `-fsanitize=undefined` and emitted no
undefined-behavior diagnostic.

The production `EventSystem` object is now a separate native target above this characterization
core. Its real factory, DACOM maps, connection point, `Send`, queue and lifetime execute under GCC
and Clang. The owner was made platform-neutral, its allocation pairs were corrected, and focused
sanitizers pass. A Linux application adapter still has to connect neutral window events without
allowing synchronous payload pointers to escape, and native Windows regression remains required.
See [EVENT_SYSTEM_PORTABILITY.md](EVENT_SYSTEM_PORTABILITY.md).

### Pointer-width encoded filesystem handles (updated 2026-09-17)

Detailed review split two previously conflated risks. The DOSFile matches are small runtime table
indices deliberately encoded in opaque `HANDLE` values; they are not object addresses and are not
serialized UTF fields. `FrontierX/EncodedHandle.h` now defines pointer-width encode/decode,
bounded-index, and offset-index contracts. UTF and SharedUTF child lookup use it instead of first
narrowing the opaque value to signed Win32 `LONG`. IFF now uses it for child, find, and mapping
tokens and uses explicit context structures instead of assuming adjacent x86 stack parameters. The
standalone probe covers zero, ordinary and offset indices, the UTF mapping flag, the all-bits-one
invalid-handle sentinel, underflow, rejected bounds, and unchanged outputs on failure.

GCC and Clang compile the probe with strict warnings and pass the full twenty-eight-test matrix. A focused
GCC UndefinedBehaviorSanitizer run also passed. The Linux build still does not compile the legacy
DOSFile project, and its native Windows validation remains pending; `IFF.cpp` is excluded from all
six current `DOSFile.vcxproj` configurations, so a normal rebuild will not compile it. The measured
encoded-handle category is now zero across IFF, UTF, and SharedUTF. All adjacent-parameter
serialized-call assumptions found in those implementations now use explicit contexts. Detailed
design and acceptance gates are in [IFF_PORTABILITY.md](IFF_PORTABILITY.md) and
[UTF_RUNTIME_PORTABILITY.md](UTF_RUNTIME_PORTABILITY.md). Separately, the corrected lexical subset
finds no remaining genuine object/address-to-32-bit cast after the public DACOM, stack-guard, and
runtime-context migrations described below.

### DACOM runtime offsets and connection tokens (2026-09-17)

The public DACOM templates previously converted component, connection-node, and property addresses
through `U32`. `FrontierX/DacomRuntime.h` now provides pointer-width runtime offsets, owner/member
recovery, and deterministic 32-bit connection tokens. Interface/reference-map offsets use
`std::size_t` with its high bit as the reference marker. CRTP owners use compiler-adjusted
`static_cast`; member connection points use byte offsets. `IDAConnectionPoint` retains its public
`U32` handle signature, but the private nodes now carry the lowest available nonzero token instead
of exposing a truncated address. Static dispatch property addresses use `std::uintptr_t`.

The isolated multiple-inheritance/member/token probe passes with strict warnings under GCC and
Clang and with AddressSanitizer plus UndefinedBehaviorSanitizer. Source regressions cover all six
migrated production headers. Both complete presets pass 28 CTests. The DACOM slice reduced the
measured true pointer-cast subset from 18 occurrences in twelve files to 11 in seven App files; the
subsequent stack-guard slice reduced it to five in four files.

The public `DACOM.H`/`TComponent.h` path now also compiles as production headers on Linux. A second
probe instantiates a real `IDAComponent` implementation, a `DAComponent` inbound interface map,
and the production connection-container/point templates. It checks query, interface adjustment,
reference counts, owner recovery, two subscriptions, enumeration, unsubscription, and deterministic
lowest-gap token reuse under GCC, Clang and sanitizers. The non-Windows path uses empty
export/calling-convention decoration while the MSVC path is unchanged; selected direct includes
now use exact on-disk case.

This is not a complete portable DACOM implementation. The production `EventSystem` is the first
native service-owner closure and `NativeComponentManager` now hosts static factories plus a
synthetic module, the production EventSystem ELF and explicit `[Libraries]` configuration. Further
production modules, cross-ELF heap ownership and legacy COMHeap behavior,
cross-module callbacks, application `TCPoint`, and
native Windows ABI behavior remain untested. The x86 automation dispatch is source-referenced only
by four legacy Viewer tool headers and is a separate later boundary. Clean Windows Debug/Final
rebuilds, ABI/layout checks, native Windows runtime connection tests, and further loader/heap closures
remain required. See [DACOM_MANAGER_PORTABILITY.md](DACOM_MANAGER_PORTABILITY.md) and
[DACOM_RUNTIME_PORTABILITY.md](DACOM_RUNTIME_PORTABILITY.md).

### Native process-local heap bootstrap (2026-09-18)

The native global manager now registers one `IHeap` before its production profile factories.
`InitializeDAHeap` resolves that factory through public DACOM, while `HEAP_Acquire`,
`SetDefaultHeapMsg` and `HEAP` expose the same process-lifetime owner. The focused probe validates
tracked allocation/reallocation, zero fill, metadata, enumeration, foreign-pointer rejection,
diagnostic accounting and repeated initialization under GCC, Clang and sanitizers. The public
header uses existing calling-convention macros, preserving MSVC decoration while compiling natively.
Configured capacity is not enforced, private COMHeap pass-through slots are unsupported, and no
cross-ELF ownership or native Windows parity is claimed. See [NATIVE_HEAP.md](NATIVE_HEAP.md).

### Pointer-width stack-local debug guards (2026-09-17)

Six debug-only checks in `MGroupRef.h`, `MPart.h`, and `Objwatch.h` previously read x86 `esp` with
MSVC inline assembly and narrowed `this` to `U32`. They now call the shared, OS-independent
`FrontierX::IsAddressNearCurrentStack` helper. It uses `std::uintptr_t`, an active stack marker and
an overflow-safe absolute distance, while retaining the historical strict 8,196-byte limit and
assertions. No object or persistent layout changed; the 92-byte retail `MGroupRef` assertion remains.

GCC, Clang, AddressSanitizer, and UndefinedBehaviorSanitizer execute the helper probe. Production
regressions reject reintroduced `esp` assembly or legacy-width address casts in all three headers.
The full presets pass 28 CTests. Native Windows builds and representative valid/invalid debug
lifetime checks remain required, and no owning Linux mission module is built yet. See
[STACK_ADDRESS_PORTABILITY.md](STACK_ADDRESS_PORTABILITY.md).

### Pointer-width runtime contexts and callbacks (2026-09-17)

The remaining five measured truncations exposed four distinct owner contracts. ParticleEffect now
uses a `std::uintptr_t` context end-to-end for the `EffectInstance*` passed through
`IParticleInstance` and `IGamePositionCallback`; numeric effect and persistent IDs remain fixed
width. DumpView uses a static RichEdit callback and `DWORD_PTR` cookie instead of x86 assembly and
a truncated member pointer. DirectShow notification data uses the API's `LONG_PTR`. The DAHOTKEY
message/filter boundary now shares pointer-width window, `wParam`, and `lParam` aliases with the SDL
legacy-input adapter, including its documented synchronous `JOYINFOEX*` payload.

The particle and legacy-message helpers execute pointer round trips in native probes. Production
source regressions cover each migrated owner. GCC and Clang pass all 28 CTests, and the measured
high-confidence object/address cast category is zero across the 1,260-file production scan.

This is source and isolated-runtime evidence, not Windows or game-runtime proof. The public virtual
interfaces cross Conquest, Trim, DAHOTKEY, ParticleEffect and tool boundaries. Clean VS2022 Win32
Debug/Final rebuilds, ABI/vtable checks, and representative particle, RichEdit, DirectShow,
keyboard/mouse/joystick and filter tests remain mandatory. See
[RUNTIME_CONTEXT_PORTABILITY.md](RUNTIME_CONTEXT_PORTABILITY.md).

### UTF metadata header check (2026-09-13)

`frontierx_headless` additionally accepts `--metadata FILE`, an explicit path resolved from the
working directory. After directory checks, it reads only the 56-byte UTF 1.01 header and validates
signature, version, 44-byte directory stride, aligned directory size, metadata ranges, overlap and
free-entry offset. Fields are decoded as little-endian fixed-width words; native structs are never
mapped onto disk bytes. Invalid metadata returns exit code 5. Omitting the option leaves this check
unperformed. Success does not validate directory entries, names, payloads or GenData archetypes.

Both preset builds passed twenty-eight CTests; the headless suite now has ten process scenarios,
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

GCC and Clang passed all twenty-eight CTests, including ten headless process scenarios with synthetic
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
compiler presets passed twenty-eight CTests; synthetic entry reads verified the exact `abc` byte checksum,
missing case-sensitive names, unsupported size relationships and bounded nested paths. Broader real
container coverage, archetype schema interpretation and the engine IFileSystem adapter remain
outstanding. No retail files were accessed or copied.

### Nested UTF entry coverage (2026-09-16)

The headless process suite now includes a synthetic three-slot UTF tree with a root directory, a
nested directory and a leaf entry. The exact virtual path `dir/file` is read successfully and a
case-mismatched path is rejected. Malformed virtual paths with leading, trailing or repeated `/`
separators and DOS `\\` separators are rejected before container traversal. This confirms the
existing iterative path construction for a bounded nested tree; it does not validate compressed
entries, payload schemas or retail containers.

The GCC and Clang full preset checks passed twenty-eight CTests after this addition. No retail files were
read or copied.

## Portable scalar math core (2026-09-16)

Source and build-graph review separated the one-source `MathLib` DACOM accessor, the scalar
`x86math.cpp` provider and the historical K62/3DNow provider. The supported wrapper reaches
`MathLib` but does not build or verify `x86Math.dll`; this is a Windows baseline gap as well as a
native-port dependency. The concentrated 3DNow assembly is not part of the scalar provider.

The new `frontierx_math` library implements the scalar provider's matrix, transform, quaternion and
historical inverse-square-root operation surface with ABI-neutral values and fixed-width bit
handling. Its target disables FMA contraction explicitly. The probe covers inverse/singular/alias
behavior, rigid transforms, quaternion conversion/rotation/slerp and representative square roots.
GCC and Clang pass the complete twenty-eight-test matrix. No legacy header, DACOM interface or Windows
project was changed; a production adapter and Windows numerical oracle remain required. Full scope
and limits are in [MATH_BACKEND.md](MATH_BACKEND.md).

## Portable mesh ray geometry (2026-09-16)

The production `MeshManager` hit-test path no longer calls `D3DXBoxBoundProbe` or
`D3DXIntersectTri` and no longer aliases `Vector` or `MeshVertex` storage as `D3DXVECTOR3`.
`FrontierX/RayGeometry.h` provides the narrow renderer-independent operations, while the call site
converts named position fields explicitly. The registered probe covers box slab and triangle
barycentric edge cases with unnormalized directions and both windings. GCC and Clang pass the full
twenty-eight-test matrix. A native Windows D3DX oracle remains required for extreme and near-degenerate
parity before this production change is accepted. The remaining 155 matches in eight files are
classified in [D3DX9_PORT.md](D3DX9_PORT.md).
