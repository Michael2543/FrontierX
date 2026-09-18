# FrontierX: Instructions for AI Coding Agents

This is the canonical project-wide instruction file for Codex and GitHub Copilot. Keep global
rules here and path-specific Copilot guidance in `.github/instructions/`; do not duplicate these
rules in `.github/copilot-instructions.md`.

## What This Project Is

FrontierX is a non-commercial native Linux x86_64 port of *Conquest: Frontier Wars*. It preserves
the original C and C++ engine, shipped gameplay, retail data compatibility, and deterministic
simulation behavior while replacing legacy Windows platform dependencies incrementally.

This repository starts from Revan67's `ConquestFWR` Windows 11 retail-restoration work. The source
release is a CQ2 development snapshot, not the exact retail source, so the upstream restoration's
layout, object-ID, campaign-ABI, renderer, and gameplay repairs are part of the required baseline.

Read [docs/PROJECT.md](docs/PROJECT.md) for the project charter and architecture status. Read
[LEGAL.md](LEGAL.md) before adding dependencies, assets, binaries, reverse-engineering findings,
or release tooling. Use [HANDOFF.md](HANDOFF.md) and [RETAIL_TEST_MATRIX.md](RETAIL_TEST_MATRIX.md)
as the current restoration evidence base.

## Current Reality

- The supported build is VS2022 Win32 on Windows 10/11.
- There is no working Linux game build or Linux package. The CMake graph builds isolated probes,
  platform libraries, a headless data checker, and a native startup/window shell; none builds or
  runs the game.
- Linux x86_64 ELF is the first port target. macOS, ARM64, and multiplayer portability are later.
- CMake with Ninja and SDL3 are intended directions, not implemented facts.
- The native D3D9-to-Vulkan approach is unresolved. The archived `misyltoad/dxvk-native` project
  is a reference, not an approved dependency.
- Missing retail media must come from a lawfully obtained installation and must not enter this
  repository.

Never report a planned capability as implemented. State whether evidence came from source review,
compilation, an automated check, or a runtime test.

## Ordered Priorities

1. Preserve and measure the restored retail/Win32 baseline.
2. Inventory platform, compiler, pointer-width, ABI, D3DX9, and build-graph blockers.
3. Introduce a portable build graph alongside the existing Visual Studio build.
4. Establish narrow platform interfaces before replacing platform implementations.
5. Bring up startup, data loading, an SDL3 window/event loop, and one rendering vertical slice.
6. Port audio and video after basic rendering is measurable; port networking last.
7. Package only after dependency provenance and retail-data separation are automated.

Do not combine several priorities into a repository-wide rewrite. Prefer the smallest change that
produces a falsifiable build, layout, ABI, or runtime result.

## Non-Negotiable Invariants

1. **Retail behavior is authoritative.** CQ2-only behavior is not an improvement when it conflicts
   with shipped data or observed retail behavior.
2. **Keep the Windows baseline viable.** Portable work must coexist with the supported VS2022
   Win32 path until an explicitly documented decision changes that policy.
3. **Isolate platform code.** Do not add Win32, Linux, X11, Wayland, Cocoa, or Vulkan calls to game
   logic. Put platform implementations behind focused interfaces in a dedicated platform/backend
   area consistent with the existing library organization.
4. **Separate storage from runtime.** Persistent file, save, map, schema, campaign, and network
   layouts retain fixed widths and exact packing. Native pointers and handles use pointer-sized
   representations or handle tables, never serialized 32-bit fields.
5. **Protect ABI-linked modules.** Changes shared by `Conquest`, `Mission`, `Trim`, `Globals`,
   `ZBatcher`, and campaign modules require all affected modules and ABI/layout checks.
6. **Preserve determinism.** Platform work must not change simulation order, RNG consumption,
   floating-point behavior, container iteration order, timing semantics, or serialized bytes.
7. **No symptom patches.** Find the owning code path and fix the root cause. Keep unrelated retail
   restoration work in separate changes.
8. **Reference, do not transplant.** Study GeneralsX and other ports for patterns. Do not copy code
   unless provenance and license compatibility are documented and the implementation fits this
   engine.

## Architecture Direction

| Layer | Direction | Rule |
|---|---|---|
| Build | CMake presets and Ninja | Add incrementally; preserve MSBuild until parity is measured |
| Platform | SDL3 plus standard C/C++ | No direct OS calls in simulation or game-object code |
| Graphics | Existing D3D9-facing renderer, then a proven native Vulkan path | Prototype before standardizing; replace D3DX9 explicitly |
| Audio | Portable backend to be selected | Match behavior before improving features |
| Video | FFmpeg-based replacement, later | Never distribute legacy Bink binaries |
| Network | Portable sockets, last | Preserve packet layout and deterministic semantics |

Do not invent a generic abstraction before identifying at least two concrete consumers or an
existing engine boundary. Prefer adapters around existing interfaces such as `IRenderPipeline`
over invasive game-logic changes.

## Compatibility and 64-Bit Rules

- Treat `long`, `DWORD`, pointer casts, pointer-sized callbacks, varargs, bitfields, and packed
  structures as audit points; Windows and LP64 Linux differ.
- Do not store pointers or COM-style interface addresses in `U32` handles. Use `uintptr_t`, native
  pointers, or an explicit typed handle table according to ownership and ABI needs.
- Do not globally widen `U32`, `S32`, IDs, offsets, or serialized handles.
- Surround intentional packed layouts with scoped packing and compile-time size, offset, and
  end-of-structure checks.
- Keep disk/wire schema types separate from convenient 64-bit runtime types.
- Preserve exact filename and include-path case. Linux case sensitivity is part of correctness.
- Do not enable fast-math or FMA contraction without cross-build determinism evidence.
- Do not import GeneralsX-specific `WWMath` names; FrontierX needs evidence-based math policy for
  this engine.

## Working Method

Before editing:

1. Start from the failing behavior, target, symbol, or nearest owner.
2. Read only the relevant surrounding implementation and one nearby test or caller.
3. State a falsifiable local hypothesis and the cheapest check that could disprove it.
4. Check whether the touched type crosses a persistent-format or module ABI boundary.

After editing:

1. Run the narrowest executable validation immediately.
2. If no Linux target exists for the touched code, run available static/layout/tool checks and say
   what could not be run.
3. Keep generated files and logs out of source control.
4. Update documentation when architecture, commands, current status, or compatibility contracts
   change. Update `MODIFICATIONS.md` for material changes before distribution.

Never hide a failed check by weakening assertions, removing validation, or catching and ignoring
the error.

## Build and Test

Existing Windows baseline entry points:

```text
build-debug.cmd
build-final.cmd
Build-Conquest.ps1 -Configuration Debug -NoDeploy
Build-Conquest.ps1 -Configuration Final -NoDeploy
```

These require Windows, VS2022, Python 3, and the June 2010 DirectX SDK. Do not attempt to present
them as native Linux commands. Full rebuilds are required; incremental legacy builds can reuse
stale intermediates.

No native Linux game build exists yet. The CMake graph contains isolated layout, TinyXML, platform
timing, profile, data-root, UTF/headless and optional SDL3 window/input targets plus a native startup
shell that composes data-root, production event-system and SDL lifecycle owners. The checked-in
Linux presets enable SDL3 and therefore require its `pkg-config` module. Its verified GCC commands
are:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug
```

Use `linux-clang-debug` in all three commands for the Clang check. Both presets configured, built,
and passed twenty-four CTests on Linux x86_64 on 2026-09-18. A separate SDL-disabled configure built the
headless target without probing SDL3. These commands do not build or run the game and do not replace
the required Windows regression path.

Use `Tools/abicheck`, schema readers, compile-time layout assertions, and
`RETAIL_TEST_MATRIX.md` where relevant. Runtime claims require an actual run against a legitimate
retail installation.

The ABI scripts require native Windows, VS2022 `dumpbin`, and PE binaries. They reject non-Windows
hosts by design. Do not run them through Docker, Podman, Wine, or Linux PowerShell, and do not add
them to the Linux CMake/CTest graph.

## Legal and Distribution

The controlling file is `Conquest Source License.txt`; never edit or replace it. The entire
derivative remains under its exact non-commercial, revocable terms. Do not add a second project
license or imply OSI-open-source status.

Before conveying source or binaries:

- preserve the full controlling license and required notices;
- keep `MODIFICATIONS.md` dated and attributed for material changes;
- verify every dependency's source, license, and redistribution compatibility;
- exclude retail media, SDK redistributables, reverse-engineering dumps, and unapproved binaries;
- exclude `binkw32.dll` and `Conquest_Frontier_Wars_Manual.pdf` unless documented permission is
  obtained; and
- do not request or accept payments, donations, expenses, or consideration for the Program,
  distribution, warranty, or support.

Keep raw retail binaries, disassembly databases, decompiler output, traces, and dumps outside the
repository. Commit only independently written interoperability facts and implementations permitted
by `LEGAL.md`.

## Source Control and Attribution

- `upstream` is `https://github.com/Revan67/ConquestFWR.git`.
- The local `main` history is intended to remain free of unresolved third-party files. Do not merge
  or publish upstream history blindly; review provenance and use a filtered import or reviewed
  patches when necessary.
- Never commit, push, create branches, rewrite history, or publish releases unless the user asks.
- Do not discard user changes or ignored diagnostic artifacts.
- Use Conventional Commit subjects in imperative form: `<type>(optional-scope): description`.
- Keep code, comments, commit messages, and repository documentation in English.
- Use Git history and `MODIFICATIONS.md` for attribution; do not add mandatory inline author tags.

## Key Paths

- `Code/App/Src/Conquest.cpp`: Win32 entry point and application startup
- `Code/App/Src/System.cpp`: render/system configuration and runtime integration
- `Code/App/Src/UserDefaults.cpp`: Windows registry-backed settings
- `Code/Libs/Src/RenderPipeline/DirectDraw/D3D/Direct3D_pipe.cpp`: active D3D9 renderer
- `Code/App/Src/Scripts/`: retail campaign modules
- `Tools/abicheck/`: cross-module ABI checks
- `Tools/portability/layout_probe.cpp`: first native Linux type-width probe
- `Tools/portability/tinyxml_probe.cpp`: first linked portable-library behavior test
- `Tools/portability/data_root_probe.cpp`: exact-case startup data-directory inspector
- `CMakePresets.json`: Linux x86_64 GCC and Clang probe presets
- `docs/DEPENDENCIES.md`: native-build dependency provenance and release blockers
- `Tools/read_gendata.py`: retail schema inspection
- `HANDOFF.md`: detailed restoration state and unresolved defects
- `RETAIL_TEST_MATRIX.md`: runtime acceptance evidence
- `LEGAL.md`: compliance and interoperability policy
- `MODIFICATIONS.md`: required prominent modification notice

## Scoped Instructions for Codex and GitHub Copilot

Codex and GitHub Copilot must read and apply these files when their paths match.
The legal/release instructions apply repository-wide; their individual requirements remain
conditional on the action being performed.

| File | Scope |
|---|---|
| `.github/instructions/cpp-conventions.instructions.md` | C and C++ source/header changes |
| `.github/instructions/build.instructions.md` | CMake, project, and build tooling changes |
| `.github/instructions/docs.instructions.md` | Markdown documentation changes |
| `.github/instructions/legal-release.instructions.md` | All paths; action-specific dependency, asset, interoperability, and release rules |

Keep this table synchronized when scoped instruction files are added, removed, or renamed.

## Port readiness follow-up

See [Linux port readiness](docs/PORT_READINESS.md) for the first proposed layout-probe milestone,
initial blocker inventory, ABI tool parameters and pending validation. The layout probe and isolated
TinyXML library now build with GCC and Clang; no native game target exists. The ABI scripts are
parameterized and statically checked, but still require native Windows execution with `dumpbin`.

## Build declaration inventory

Run `python3 Tools/portability/build_inventory.py` from the repository root to inspect the
supported Windows entry points without invoking Windows tools. See [build graph evidence](docs/BUILD_GRAPH.md)
for scope, test commands, measured dependencies and production profile-parser evidence. This
read-only inventory does not evaluate MSBuild or replace native Windows ABI validation.

## Profile buffer core (2026-09-13)

The Linux presets build `frontierx_profile_buffer_probe`; both GCC and Clang pass the complete
twenty-four-test matrix. `FrontierX/ProfileBuffer.h` owns section marking, line/section navigation,
key lookup/copying, and section/key callback traversal shared with the production DACOM parser.
Runtime section offsets now use the pointer-width encoded-handle contract instead of `U32`
decoding. This remains an internal borrowed-buffer contract with sentinel padding, not a complete
portable DACOM implementation or game target. Native Windows Debug/Final rebuilds, callback
comparison and ABI checks remain required; see `docs/PROFILE_CALLBACK_PORTABILITY.md`.

### Owning native profile reader (2026-09-13)

The existing GCC and Clang presets build `frontierx_profile_document_probe` and pass twenty-four
CTests. `FrontierX/ProfileDocument.h` provides owning text/file loading, ordered section names
and key lookup through the shared profile-buffer core. Native startup configuration currently
accepts ASCII without NUL bytes, up to 1 MiB, with locale-independent ASCII case folding.
This policy is limited to the new reader; Windows DACOM comparison behavior is unchanged.
No native game startup or retail-configuration compatibility is claimed by the synthetic tests.

### Production native profile parser (2026-09-18)

`frontierx_profile_parser` compiles the real DACOM `ProfileParser.cpp` factories, aggregates and
both parser interfaces. Its non-Windows read path uses bounded fixed-width sizing and transactional
standard-library file I/O; Windows retains the existing Win32 branch. `Initialize2` now uses a
typed buffer initializer instead of encoding a length in an out-of-range enum value. The real
service passes buffer/file, callback, duplicate, handle, failure-retention, GCC/Clang and focused
sanitizer checks through `NativeComponentManager`. `SetINIConfig`, profile-driven library bootstrap,
DA-heap policy and Windows behavioral/ABI validation remain open; see
`docs/PROFILE_CALLBACK_PORTABILITY.md`.

## Native headless startup check (2026-09-13)

The GCC and Clang presets build `frontierx_headless` and pass twenty-four CTests; the headless CTest
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

### UTF metadata header check (2026-09-13)

`frontierx_headless` additionally accepts `--metadata FILE`, an explicit path resolved from the
working directory. After directory checks, it reads only the 56-byte UTF 1.01 header and validates
signature, version, 44-byte directory stride, aligned directory size, metadata ranges, overlap and
free-entry offset. Fields are decoded as little-endian fixed-width words; native structs are never
mapped onto disk bytes. Invalid metadata returns exit code 5. Omitting the option leaves this check
unperformed. Success does not validate directory entries, names, payloads or GenData archetypes.

Both preset builds passed twenty-four CTests; the headless suite now has ten process scenarios,
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

GCC and Clang passed all twenty-four CTests, including ten headless process scenarios with synthetic
UTF trees, invalid links, names and payload ranges. No retail installation was inspected. This
extends the read-only inspector; it does not yet replace the Windows DOSFile implementation.

The headless checker now accepts `--metadata FILE --entry VIRTUAL_PATH` to read a bounded UTF
entry into memory and report its byte count and diagnostic checksum (2026-09-13). Both presets
passed twenty-four CTests. No schema or retail compatibility is implied; see the UTF entry-reading
section of the port-readiness plan for limits and outstanding work.

### SDL3 window and input backend probe (2026-09-16)

The Linux presets now build the isolated `frontierx_sdl3_platform` library and
`frontierx_sdl3_window_probe` through the installed SDL3 3.4.16 `pkg-config` module. The backend
owns window/video lifetime and translates resize, focus, close, keyboard, owned UTF-8 text, mouse
motion/button/wheel and quit events into SDL-free FrontierX values. An isolated compatibility
adapter produces the numeric message, pointer-width window token, `wParam` and `lParam` contract
read by current controls and hotkey code, with synchronous parameter lifetime. GCC and Clang passed
the full twenty-four-test CTest matrix with the SDL dummy video backend. A separate responsive visible-window
runtime smoke opened, pumped and closed through Wayland. The adapter is not connected to the game;
an isolated event-pump owner drains injected SDL batches synchronously through the production
`EventSystem` to a real DACOM subscriber. `IWindowManager`, application-loop ownership/cadence, Windows input parity,
renderer integration and distribution approval remain open.

### Native startup shell (2026-09-18)

`frontierx_native_shell` is the first integrated Linux process owner. It shares native configuration
and data-root precedence with the headless checker, validates the exact-case required directory
structure, creates and initializes the production `EventSystem` through
`NativeComponentManager`, opens SDL video/window and text input, pumps until close/quit or a bounded
smoke deadline, and tears those owners down in dependency order. GCC and Clang pass the synthetic
process suite under the dummy video driver; focused AddressSanitizer/UndefinedBehaviorSanitizer
coverage also passes. This shell does not load the game, retail metadata, a campaign or a renderer,
and empty synthetic directories do not prove retail compatibility. See `docs/NATIVE_SHELL.md`.

### Platform touchpoint inventory (2026-09-16)

`Tools/portability/platform_inventory.py` provides a deterministic lexical audit of production
C/C++ sources grouped by category, file, and owning subsystem. Its 1,250-file run and limitations
are documented in `docs/PLATFORM_INVENTORY.md`; the synthetic inventory test is part of the full
CTest matrix. Counts are source-review candidates, not compile errors or runtime evidence. Use the
machine-readable output to select narrow owner closures; do not use aggregate counts to justify a
repository-wide rewrite. The current run scans 1,250 files; both the measured true object/address
truncation and legacy-width DOSFile handle-index categories are now zero.
IFF, UTF, and SharedUTF use the pointer-width token contract and explicit serialized-call contexts.
The public DACOM templates use pointer-width runtime offsets and non-pointer connection tokens.
Both migrations still require native Windows compile, ABI and behavioral validation; the production
`EventSystem` is the first real native DACOM service owner, while the manager/loader remains open.

### Native preference storage (2026-09-16)

`FrontierX/PreferenceStore.h` and the platform library implement the bounded `FXSET001` native
settings container for typed strings and binary values. It has deterministic fixed-width
little-endian serialization, transactional validation and same-directory temporary-file
replacement. The probe covers missing files, exact round trips, stable bytes, invalid inputs,
corruption, truncation and trailing data; GCC and Clang pass the full twenty-four-test matrix. This is
not the raw `USER_DEFAULTS` registry blob. `LegacyPreferenceValues` now reproduces the typed
binary/string, deletion, short-buffer and four-entry MRU value semantics over that store, and the
real `IUserDefaults` header compiles through the public DACOM probe without changing its MSVC ABI.
`NativeUserSettings` defines 25 required fixed-width version-1 entries and 31 explicit flag bits;
`NativeUserSettingsOwner` now applies player/global precedence, hex-encoded player namespaces,
missing-player reset with hardware-cursor preservation, invalid-player cleanup/global fallback and
transactional dual stores. Headless startup executes this owner at the documented XDG path and
reports only the settings source. The live `IUserDefaults` component, immediate/shutdown file
saves, directory creation/durability policy and Windows regression remain open. Do not silently
change the supported Windows registry behavior.

### Portable event queue core (2026-09-16)

`frontierx_platform` includes an independently owned `EventQueue` matching the source-reviewed
`IEventSystem` `Post`/`Update`/`Peek` batch semantics. The production DAHOTKEY `EventSystem` now
also builds as a native target: its real aggregate factory, inbound/outbound DACOM maps, connection
point, direct `Send`, callback-time posts and `Peek` lifecycle pass under GCC, Clang and focused
sanitizers. `NativeComponentManager` hosts it through the real `ICOManager` interface and preserves
static-factory priority, fallback, enumeration, reference and shutdown behavior. Queue
allocation/free pairing and null-owner member-offset UB were corrected. Raw
parameters remain non-owning native pointers and are never serialized. An isolated SDL pump owns
batch draining and synchronous dispatch, but `IWindowManager` and application-loop lifecycle/cadence integration,
the remaining Hotkey module, profile-driven dynamic loading/heap bootstrap and native Windows
regression remain open. The x86 automation dispatch is used by four legacy Viewer headers, not by
static component creation.

### Native dynamic-library boundary (2026-09-17)

`frontierx_dynamic_library` wraps Linux `dlopen`/`dlsym`/`dlclose` in a move-only local, eager-load
owner. `NativeComponentManager` requires explicit C-named registration/liveness entry points,
attributes module registrations, balances repeated exact-path loads, unregisters before close and
refuses unload while the module reports live objects. A CMake-built synthetic ELF fixture verifies
these paths plus missing symbols, ownership transfer and idempotent close. No Win32 loader call is
replaced. A real component adaptation, profile/search ordering, cross-module heap policy and native
Windows comparison remain; campaigns still need a separate C export and ABI policy. See
`docs/DYNAMIC_LIBRARY_PORTABILITY.md`.

### Portable scalar math core (2026-09-16)

The Linux presets build `frontierx_math` and pass twenty-four CTests with FMA contraction disabled for
that target. The core covers the scalar `I3DMathEngine` operation surface with ABI-neutral values
and fixed-width inverse-square-root bit handling. Source review proved that 402 of 441 remaining assembly
candidates belong to the separate historical K62/3DNow provider, not scalar `x86math.cpp`.
`Build-Conquest.ps1` reaches the `MathLib` DACOM accessor but does not build or verify
`x86Math.dll`; do not claim a complete provider path. A DACOM adapter, native Windows numerical
oracle, floating-point-environment policy and Win32 build/deploy repair remain required. See
`docs/MATH_BACKEND.md`.

### Portable mesh ray geometry (2026-09-16)

`MeshInstance::ComputeHitTest` now uses `FrontierX/RayGeometry.h` instead of
`D3DXBoxBoundProbe`, `D3DXIntersectTri`, and layout-casting engine vertices to D3DX types. The
header-only boundary is compiled by both the existing MeshManager project and a native probe; the
Linux presets pass twenty-four CTests. The remaining D3DX9 inventory is 155 lexical matches in eight
production files and is classified in `docs/D3DX9_PORT.md`. Native Windows comparison remains
required for boundary tolerances and near-degenerate inputs before accepting parity.

### DACOM runtime pointer-width helpers (2026-09-17)

`FrontierX/DacomRuntime.h` and `frontierx_dacom_runtime_probe` characterize pointer-width
interface/member offsets, multiple-inheritance adjustment and deterministic 32-bit connection
tokens. The public DACOM templates no longer truncate component, connection-node or static-property
addresses through `U32`; the established `IDAConnectionPoint` handle ABI remains `U32`, but handles
are now non-pointer tokens. The production inventory reports no remaining measured pointer/address
casts and none in the migrated public DACOM headers. GCC and Clang pass the complete twenty-four-test
matrix; focused AddressSanitizer/UndefinedBehaviorSanitizer execution also passes.

`frontierx_dacom_interface_probe` now compiles the real public `DACOM.H` and `TComponent.h` path,
instantiates `IDAComponent` query/reference behavior, a `DAComponent` inbound map, and the production
connection-container/point templates, and passes under GCC, Clang and focused sanitizers. It covers
lookup, owner recovery, subscription enumeration, removal, and deterministic token reuse. Non-MSVC
export/calling-convention decoration is empty; the existing Win32 decoration remains. Selected
direct includes use exact on-disk case.

This is not a complete Linux DACOM target. A static native manager now hosts the production
`EventSystem`, but DLL loading, profile/heap bootstrap, further owner include/platform boundaries
and native Windows validation remain open. The 32-bit MSVC automation dispatcher is referenced by
four legacy Viewer headers, not by static component creation. Debug/Final rebuilds plus ABI/layout
and connection behavior checks remain required. See
`docs/DACOM_RUNTIME_PORTABILITY.md`.

### Pointer-width stack-local debug guards (2026-09-17)

Six `_DEBUG` guards in `MGroupRef.h`, `MPart.h`, and `Objwatch.h` now use
`FrontierX/StackAddress.h` instead of reading x86 `esp` and truncating `this` to `U32`. The helper
uses `std::uintptr_t`, supports either stack-growth direction, and retains the strict 8,196-byte
limit and existing assertions. No object or persistent layout changed. GCC, Clang and focused
sanitizer probes pass; the complete presets pass twenty-four CTests. Native Windows Debug/Final and
owning-module validation remain required. See `docs/STACK_ADDRESS_PORTABILITY.md`.

### Pointer-width runtime contexts and callbacks (2026-09-17)

`FrontierX/ParticleContext.h` carries the process-local `EffectInstance*` through
`IParticleInstance`, `ParticleSystem`, and `IGamePositionCallback` without widening unrelated
numeric effect or persistent IDs. DumpView uses a static RichEdit callback with `DWORD_PTR`;
DirectShow notification data uses `LONG_PTR`. `FrontierX/LegacyMessage.h` supplies pointer-width
window, `wParam`, and `lParam` types shared by DAHOTKEY and the SDL compatibility adapter, including
the synchronous joystick pointer payload. The measured production pointer/address-cast category is
now zero, as is the encoded DOSFile-handle category. GCC and Clang pass twenty-four CTests.

These are source and isolated-probe results. The affected Conquest, Trim, DAHOTKEY and
ParticleEffect interfaces still require native Windows Debug/Final rebuilds, ABI/vtable checks and
behavioral regression. See `docs/RUNTIME_CONTEXT_PORTABILITY.md`.
