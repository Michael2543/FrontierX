# FrontierX Project Charter

Status: bootstrap, 2026-09-11

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
4. **Bring up one rendering path.** Preserve the current D3D9-facing renderer initially and prove a
   maintained native D3D9-to-Vulkan route with a small vertical slice before adopting it. Replace
   unsupported D3DX9 facilities explicitly.
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
| Build | CMake presets with Ninja; preserve existing MSBuild path | Planned |
| Window/input/time | SDL3 behind a FrontierX platform boundary | Planned |
| Graphics | Keep D3D9-facing code for bring-up; evaluate a maintained native Vulkan translation path | Prototype required |
| D3DX9 | Inventory and replace effects, texture helpers, and math dependencies | Required |
| Audio | Portable backend, selected after dependency and behavior evaluation | Undecided |
| Video | FFmpeg-based replacement for legacy movie playback | Planned, later phase |
| Network | Portable sockets with deterministic protocol behavior | Deferred |

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