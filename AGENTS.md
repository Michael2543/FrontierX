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

No FrontierX Linux build command exists yet. When one is introduced, document the exact configure,
build, test, and run commands in `README.md` and `docs/PROJECT.md`, and validate them before adding
them here.

Use `Tools/abicheck`, schema readers, compile-time layout assertions, and
`RETAIL_TEST_MATRIX.md` where relevant. Runtime claims require an actual run against a legitimate
retail installation.

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
- `Tools/read_gendata.py`: retail schema inspection
- `HANDOFF.md`: detailed restoration state and unresolved defects
- `RETAIL_TEST_MATRIX.md`: runtime acceptance evidence
- `LEGAL.md`: compliance and interoperability policy
- `MODIFICATIONS.md`: required prominent modification notice

## Scoped Copilot Instructions

GitHub Copilot must also apply these files when their paths match:

| File | Scope |
|---|---|
| `.github/instructions/cpp-conventions.instructions.md` | C and C++ source/header changes |
| `.github/instructions/build.instructions.md` | CMake, project, and build tooling changes |
| `.github/instructions/docs.instructions.md` | Markdown documentation changes |
| `.github/instructions/legal-release.instructions.md` | Dependencies, binaries, assets, reverse engineering, packaging, and releases |

Keep this table synchronized when scoped instruction files are added, removed, or renamed.