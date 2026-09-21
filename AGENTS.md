# FrontierX: Instructions for AI Coding Agents

This is the canonical project-wide instruction file for Codex and GitHub Copilot. Keep durable
project rules here and path-specific guidance in `.github/instructions/`. Detailed implementation
status and validation history belong in the linked project documents, not in this file.

## Project and authoritative references

FrontierX is a non-commercial native Linux x86_64 port of *Conquest: Frontier Wars*. It preserves
the original C and C++ engine, shipped gameplay, retail-data compatibility and deterministic
simulation while incrementally replacing legacy Windows dependencies.

The source starts from Revan67's `ConquestFWR` Windows retail-restoration work. The released source
is a CQ2 development snapshot rather than the exact retail source, so the upstream layout,
object-ID, campaign-ABI, renderer and gameplay repairs are part of the required baseline.

Read the smallest relevant authoritative document before changing code:

- [docs/PROJECT.md](docs/PROJECT.md): project charter, architecture and concise current status.
- [docs/PORT_READINESS.md](docs/PORT_READINESS.md): milestones, blockers and ordered work queue.
- [docs/BUILD_GRAPH.md](docs/BUILD_GRAPH.md): measured build graph and native target boundaries.
- [HANDOFF.md](HANDOFF.md) and [RETAIL_TEST_MATRIX.md](RETAIL_TEST_MATRIX.md): Windows restoration
  state and retail-runtime evidence.
- [docs/DEPENDENCIES.md](docs/DEPENDENCIES.md): dependency provenance and release gates.
- Focused documents under `docs/`: component contracts, commands, evidence and unresolved checks.
- [LEGAL.md](LEGAL.md) and `Conquest Source License.txt`: controlling legal and distribution rules.

## Current reality

- VS2022 Win32 on Windows 10/11 remains the supported game build and regression baseline.
- The Linux CMake/Ninja graph is implemented for isolated libraries and checks, platform services,
  a headless data checker and a native startup/window shell. It does not yet build or run the game.
- SDL3 is implemented at the native platform boundary: an SDL-backed `IWindowManager` owns the
  shell window/event lifecycle, and an isolated SDL GPU/Vulkan prototype exercises indexed drawing,
  texture sampling, depth, resize, presentation and readback. Neither result is a production game
  renderer or proof of retail compatibility.
- Selected production owners including profile parsing, the event system, native user defaults and
  hotkey processing compile and execute in native checks. Their exact status and remaining gates are
  recorded in the focused documents linked from `docs/PROJECT.md` and `docs/PORT_READINESS.md`.
- A production `PrimitiveBuilder` draw reaches Vulkan presentation and bounded readback through a
  registered native `IRenderPrimitive` and the portable submission contract. The native shell can
  explicitly select that lifecycle and render the repository smoke scene. State/material mapping,
  Windows comparison, backend selection and a native game scene remain open.
- Linux x86_64 ELF is the first port target. macOS, ARM64 and multiplayer portability are later.
- Missing retail media must come from a lawfully obtained installation and must never enter this
  repository.

Never report a plan, source inspection or synthetic check as an implemented game capability. Label
evidence as source review, compilation, automated execution, synthetic runtime, Windows runtime or
retail runtime.

## Ordered priorities

1. Preserve and measure the restored retail/Win32 baseline.
2. Inventory platform, compiler, pointer-width, ABI, D3DX9 and build-graph blockers.
3. Extend the portable build graph alongside the Visual Studio build.
4. Establish focused platform interfaces before replacing implementations.
5. Connect startup, data loading, SDL3 window/input and one production rendering vertical slice.
6. Reach a deterministic playable single-player slice.
7. Port audio and video; port networking last.
8. Package only after dependency provenance and retail-data separation are automated.

Do not combine several priorities into a repository-wide rewrite. Prefer the smallest change that
advances a production owner and produces a falsifiable build, layout, ABI or runtime result.

## Current rendering milestone

The active milestone is the **first native scene through a production render boundary**. Its Linux
geometry/presentation slice now runs in the native shell; complete it incrementally through the
existing `IRenderPipeline`/`IRenderPrimitive` ownership model rather than creating a parallel API.

The milestone is accepted only when all of the following are evidenced:

1. At least one real production draw caller reaches the native backend through an adapter at the
   existing renderer boundary. Direct and U16-indexed RPVertex-shaped submissions are covered;
   unsupported FVF/topology/state combinations fail explicitly.
2. A representative scene uses a repository-owned fixture or lawfully supplied external data to
   draw inspectable geometry, a texture and material state in the SDL3 window. No retail asset is
   copied into the repository.
3. Depth, alpha blending, texture sampling, resize-dependent targets, presentation and teardown
   have bounded automated or capture evidence. Resource ownership survives repeated scene runs.
4. GCC and Clang build and execute the slice on Linux x86_64; focused sanitizers cover CPU-side
   ownership and bounds. A skip caused by an unavailable GPU is not runtime evidence.
5. The same representative input is captured on the supported Windows path and compared under a
   documented tolerance. Native Windows Debug/Final rebuilds and relevant ABI/layout checks pass.
6. The focused renderer document records commands, observed output, data provenance, known gaps and
   whether the evidence is compilation, synthetic runtime, Windows runtime or retail runtime.

Do not select a production backend or claim the rendering milestone from the isolated GPU probe
alone. Continue replacing D3DX9 facilities explicitly and keep the supported D3D9 path viable.

## Non-negotiable invariants

1. **Retail behavior is authoritative.** CQ2-only behavior is not an improvement when it conflicts
   with shipped data or observed retail behavior.
2. **Keep the Windows baseline viable.** Portable work must coexist with the VS2022 Win32 path
   until an explicitly documented decision changes that policy.
3. **Isolate platform code.** Do not add Win32, Linux, X11, Wayland, Cocoa or Vulkan calls to game
   logic. Keep implementations behind focused interfaces in the owning platform/backend area.
4. **Separate storage from runtime.** Disk, save, map, schema, campaign and network layouts retain
   fixed widths and exact packing. Native pointers and handles use pointer-sized representations or
   typed handle tables and never leak into serialized 32-bit fields.
5. **Protect ABI-linked modules.** Changes shared by `Conquest`, `Mission`, `Trim`, `Globals`,
   `ZBatcher` or campaign modules require all affected builds and ABI/layout checks.
6. **Preserve determinism.** Platform work must not change simulation order, RNG consumption,
   floating-point behavior, container iteration order, timing semantics or serialized bytes.
7. **Fix owners, not symptoms.** Identify the owning path and keep unrelated restoration work in a
   separate change.
8. **Reference, do not transplant.** Do not copy code from GeneralsX or another project unless
   provenance and license compatibility are documented and the implementation fits this engine.

## Architecture direction

| Layer | Direction | Rule |
|---|---|---|
| Build | CMake presets and Ninja | Extend incrementally; preserve MSBuild until parity is measured |
| Platform | Existing SDL3 boundary plus standard C/C++ | No direct OS calls in simulation/game objects |
| Graphics | Existing D3D9-facing contracts adapted to a proven native path | Connect a production slice before selecting the backend; replace D3DX9 explicitly |
| Audio | Portable backend to be selected | Match behavior before improving features |
| Video | FFmpeg-based replacement, later | Never distribute legacy Bink binaries |
| Network | Portable sockets, last | Preserve packet layout and deterministic semantics |

Do not invent a generic abstraction before identifying at least two concrete consumers or an
existing engine boundary. Prefer adapters around established interfaces over invasive game-logic
changes.

## Compatibility and 64-bit rules

- Audit `long`, `DWORD`, pointer casts, callbacks, varargs, bitfields and packed structures; Win32
  and LP64 Linux differ.
- Never store pointers or COM-style addresses in `U32` handles. Use native pointers,
  `std::uintptr_t` or an explicit typed handle table according to ownership and ABI needs.
- Do not globally widen `U32`, `S32`, IDs, offsets or serialized handles.
- Scope intentional packing and assert size, offsets and final-field extent.
- Keep disk/wire schema values separate from convenient runtime types.
- Preserve exact filename and include-path case.
- Do not enable fast-math or FMA contraction without cross-build determinism evidence.
- Do not import GeneralsX-specific `WWMath` names or policy.

## Working method

Before editing:

1. Start from the failing behavior, target, symbol or nearest production owner.
2. Read the relevant implementation and at least one real caller or adjacent test.
3. State a falsifiable local hypothesis and the cheapest check that could disprove it.
4. Determine whether each touched type crosses persistent-format or module-ABI boundaries.

After editing:

1. Run the narrowest executable validation immediately, then the affected preset/configuration.
2. If the affected production code has no Linux target, run the strongest available compile,
   layout or source check and state what remains unexecuted.
3. Keep generated files and logs out of source control.
4. Update architecture/status documentation only when its contract or current state changed.
5. Update `MODIFICATIONS.md` for material changes before distribution.

Never hide a failure by weakening assertions, removing validation or swallowing the error.

## Rules for probes and regression checks

A new probe is justified only when it does at least one of these:

- disproves a named integration blocker on the path to a production owner;
- characterizes a compatibility/ABI contract that cannot yet execute in its production closure; or
- permanently protects a relevant regression in code that is already integrated or being
  integrated.

Before adding one, document in its focused status section or nearby test description:

- the concrete blocker or regression;
- the production owner and real caller it represents;
- the falsifiable behavior being checked;
- why an existing test cannot cover it; and
- which integration decision changes when it passes or fails.

Prefer extending an existing probe. Do not add a probe merely to raise coverage, demonstrate an
unconnected helper, or make a planned architecture look implemented. A probe must not become a
parallel product implementation. Register deterministic checks with CTest; keep host-dependent
smokes explicit, and never count a skipped runtime as execution evidence.

## Validation records

For every material change, record validation in the relevant focused document. Keep the entry tied
to the change rather than repeating a repository-wide test count. Include:

- exact target/configuration and host/compiler that were built;
- exact automated checks and runtime paths that actually executed;
- whether inputs were synthetic, repository-owned, lawfully external or retail;
- observable result or invariant, not only “tests passed”;
- sanitizer, layout, ABI or image-comparison evidence when relevant; and
- explicitly unperformed Windows, retail, hardware or cross-module checks.

Do not copy rolling CTest totals into every document; they become stale when unrelated tests are
added. Use the full preset result only in the central build/status document, and use focused target
results in component documents.

## Build entry points

Supported Windows baseline commands:

```text
build-debug.cmd
build-final.cmd
Build-Conquest.ps1 -Configuration Debug -NoDeploy
Build-Conquest.ps1 -Configuration Final -NoDeploy
```

They require native Windows, VS2022, Python 3 and the June 2010 DirectX SDK. Full rebuilds are
required; do not present these as Linux commands or emulate the ABI checks through Wine/containers.

Linux GCC commands:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug
```

Use `linux-clang-debug` for the corresponding Clang sequence. The checked-in presets currently
enable SDL3 and the isolated SDL GPU target and therefore require SDL3's `pkg-config` module and
`glslc`. These commands build the documented native graph, not the game. For the latest measured
targets and results, use [docs/BUILD_GRAPH.md](docs/BUILD_GRAPH.md).

Use `Tools/abicheck`, schema readers, compile-time layout assertions and
`RETAIL_TEST_MATRIX.md` where relevant. ABI scripts require native Windows, VS2022 `dumpbin` and PE
binaries and must not be added to Linux CTest.

## Legal and distribution

The controlling file is `Conquest Source License.txt`; never edit, replace or supplement it with a
second project license. The derivative remains under its exact non-commercial, revocable terms.

Before conveying source or binaries:

- preserve the complete controlling license and required notices;
- keep `MODIFICATIONS.md` dated and attributed;
- verify dependency source, version, license and redistribution compatibility;
- exclude retail media, SDK redistributables, reverse-engineering dumps and unapproved binaries;
- exclude `binkw32.dll` and `Conquest_Frontier_Wars_Manual.pdf` without documented permission; and
- never request or accept payment, donations, expenses or consideration for the Program,
  distribution, warranty or support.

Keep raw retail binaries, disassembly databases, decompiler output, traces and dumps outside the
repository. Commit only independently written interoperability facts and implementations permitted
by `LEGAL.md`.

## Source control

- `upstream` is `https://github.com/Revan67/ConquestFWR.git`.
- Do not merge or publish upstream history blindly; review provenance and use a filtered import or
  reviewed patches when needed.
- Never commit, push, create branches, rewrite history or publish releases unless the user asks.
- Preserve unrelated user changes and ignored diagnostic artifacts.
- Use Conventional Commit subjects: `<type>(optional-scope): description`.
- Keep code, comments, commit messages and repository documentation in English.
- Use Git history and `MODIFICATIONS.md` for attribution; do not add mandatory inline author tags.

## Key entry points

- `Code/App/Src/Conquest.cpp`: Win32 application entry and startup ownership.
- `Code/App/Src/System.cpp`: renderer/system configuration and runtime integration.
- `Code/App/Src/UserDefaults.cpp`: supported Windows registry settings owner.
- `Code/Libs/Include/rendpipeline.h` and `IRenderPrimitive.h`: existing renderer boundaries.
- `Code/Libs/Src/RenderPipeline/DirectDraw/D3D/Direct3D_pipe.cpp`: active D3D9 renderer.
- `Code/Launcher/NativeShell.cpp`: current native process composition root.
- `Code/App/Src/Scripts/`: retail campaign modules.
- `CMakeLists.txt` and `CMakePresets.json`: incremental native build graph.
- `Tools/portability/`: focused native checks and inventories.
- `Tools/abicheck/`: native-Windows cross-module ABI checks.

## Scoped instructions

Read and apply these files when their paths match. Legal/release rules are repository-wide but
their individual requirements remain conditional on the action being performed.

| File | Scope |
|---|---|
| `.github/instructions/cpp-conventions.instructions.md` | C and C++ source/header changes |
| `.github/instructions/build.instructions.md` | CMake, project and build tooling changes |
| `.github/instructions/docs.instructions.md` | Markdown documentation changes |
| `.github/instructions/legal-release.instructions.md` | All paths; action-specific legal/release rules |

Keep this table synchronized when scoped instruction files are added, removed or renamed.
