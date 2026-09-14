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
- There is no working Linux build, CMake build, native launcher, or Linux package yet.
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

No native Linux game build exists yet. The CMake graph currently contains the dependency-free
x86_64 layout probe, an isolated TinyXML static library and test consumer, and a standard-C++
platform timing library plus timing and data-root test consumers. Its verified GCC commands are:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug
```

Use `linux-clang-debug` in all three commands for the Clang check. Both presets configured, built,
and passed four CTests on Linux x86_64 on 2026-09-12. These commands do not build or run the game
and do not replace the required Windows regression path.

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
for scope, test commands, measured dependencies and the next profile-parser candidate. This
read-only inventory does not evaluate MSBuild or replace native Windows ABI validation.

## Profile buffer core (2026-09-13)

The Linux presets now also build `frontierx_profile_buffer_probe`; both GCC and Clang
passed all five CTests. `FrontierX/ProfileBuffer.h` owns section marking and line/section
navigation shared with the production DACOM parser. This is an internal, borrowed-buffer
contract with sentinel padding, not a complete portable profile parser or game target.
The DACOM wrapper retains its Win32 I/O, allocator and public interfaces. Native Windows
Debug/Final rebuilds and ABI checks remain required before accepting that integration.

### Owning native profile reader (2026-09-13)

The existing GCC and Clang presets now build `frontierx_profile_document_probe` and pass six
CTests. `FrontierX/ProfileDocument.h` provides owning text/file loading, ordered section names
and key lookup through the shared profile-buffer core. Native startup configuration currently
accepts ASCII without NUL bytes, up to 1 MiB, with locale-independent ASCII case folding.
This policy is limited to the new reader; Windows DACOM comparison behavior is unchanged.
No native game startup or retail-configuration compatibility is claimed by the synthetic tests.

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

The headless checker now accepts `--metadata FILE --entry VIRTUAL_PATH` to read a bounded UTF
entry into memory and report its byte count and diagnostic checksum (2026-09-13). Both presets
passed seven CTests. No schema or retail compatibility is implied; see the UTF entry-reading
section of the port-readiness plan for limits and outstanding work.
