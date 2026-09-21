# Native Platform Touchpoint Inventory

Status: lexical source inventory measured on 2026-09-19

This document measures where the current production-oriented C and C++ source tree depends on
Windows APIs, compiler extensions, legacy-width assumptions, and legacy media APIs. It is a
source-review aid for selecting small porting slices. It is not a compiler result, a complete
dependency graph, or evidence that any category can be replaced independently.

## Reproduce

Run from the repository root with Python 3. The tool uses only the standard library and emits
deterministic JSON:

```text
PYTHONDONTWRITEBYTECODE=1 python3 Tools/portability/platform_inventory.py \
  > /tmp/frontierx-platform-inventory.json

PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover \
  -s Tools/portability -p 'test_platform_inventory.py' -v
```

The current 2026-09-19 run scanned 1,266 C and C++ source/header files beneath `Code/App/Src`,
`Code/Libs/Src`, and `Code/Libs/Include`. It excludes paths named `Tools`, `Viewer`, `Obsolete`,
`Samples`, `Example`, or `Examples`. Comments are removed before matching, while strings and
preprocessor includes are retained. The synthetic test covers category detection, comment
removal, owner assignment, excluded paths, missing roots, and deterministic output.

Counts are lexical occurrences. A count is an audit candidate, not necessarily a defect or one
unit of implementation work. In particular, the legacy-integer category deliberately includes
ordinary numeric casts that still require classification. `pointer_integer_casts` is the narrower
object/address subset; `encoded_handle_casts` separately records integer tokens deliberately
stored in opaque `HANDLE` values. The Win32 API expression is representative rather than an
exhaustive list of every Windows symbol.

## Measured categories

| Category | Occurrences | Files | Largest measured owners |
|---|---:|---:|---|
| D3D9 surface | 4,568 | 161 | App 2,245/88; RenderPipeline 568/11; Shading 556/17 |
| Win32 API | 2,073 | 257 | App 1,195/160; DOSFile 270/8; MaterialManager 188/5 |
| Legacy integer casts | 1,318 | 230 | App 848/146; ParticleEffect 136/23; public library includes 64/7 |
| MSVC extensions | 1,342 | 292 | App 946/182; public library includes 127/32; DOSFile 39/7 |
| Inline assembly | 441 | 18 | x86Math 402/2; App 27/7 |
| Native `long` | 394 | 94 | EngComps 95/5; public library includes 90/31; App 59/24 |
| D3DX9 helpers | 155 | 8 | MeshManager 60/1; App 65/1; ParticleEffect 17/3 |
| DirectSound/WinMM | 162 | 16 | SoundManager 70/2; Voice 40/4; App 31/6 |
| Direct Windows headers | 150 | 139 | App 26/24; EngComps 24/24; RenderPipeline 14/14 |
| Legacy video | 143 | 7 | public Bink header 101/1; App 16/3; VideoSystem 11/1 |
| Encoded opaque-handle casts | 0 | 0 | Completed for the measured production pattern |
| Object/address-to-legacy-integer casts | 0 | 0 | Completed for the measured production pattern |
| Dynamic loading | 109 | 13 | App 89/8; RenderPipeline 11/2; DACOM 6/1 |
| Varargs | 81 | 15 | RenderPipeline 24/3; App 23/3; public library includes 12/4 |
| Registry | 67 | 1 | App 67/1 (`UserDefaults.cpp`) |
| Explicit packing | 50 | 24 | App 34/17; public library includes 8/3 |

Owner cells use `occurrences/files`. The machine-readable JSON contains every matching file and
the full owner breakdown; the table intentionally shows only the largest owners.

## Concentrations that change the port plan

The renderer is not one replaceable library. D3D9-facing names occur in 161 files, including 88
application files, and are also spread across RenderPipeline, Shading, MeshManager, RendComp, and
other engine libraries. The first renderer milestone therefore remains a narrow proven vertical
slice behind the existing render boundary. A repository-wide type or API rewrite would combine
unrelated ownership and determinism risks.

D3DX9 is much narrower than D3D9: the remaining measured uses are in eight files. The two
`MeshRender.cpp` implementations account for 125 of 155 occurrences. The MeshManager hit-test path
now uses portable ray geometry instead of two D3DX helpers and unsafe layout casts. The remaining
surface is classified in [D3DX9_PORT.md](D3DX9_PORT.md); removing it alone would not make the
renderer portable.

The apparent assembly problem is also concentrated. `Code/Libs/Src/x86Math/3DNow_VC.h` and
`Code/Libs/Src/x86Math/K62math.cpp` contain 402 of 441 remaining matches. Build-graph review established that
these belong to a separate historical K62 provider, while the scalar `x86math.vcxproj` compiles
only `x86math.cpp`. The scalar operation surface now has an isolated native core; the optional
provider must not drive speculative 3DNow translation. See [MATH_BACKEND.md](MATH_BACKEND.md).

Configuration is the smallest isolated Windows-storage surface: all 67 registry matches occur in
`Code/App/Src/UserDefaults.cpp`. The existing native profile/data-root and headless settings owners
do not yet replace that production component. The value types, MRU behavior and player/global load
lifecycle are characterized; live read/write timing, dialogs and save integration remain, and the
supported Windows registry behavior must stay unchanged.

Startup and content loading remain foundational but are not small. DOSFile owns 270 Win32 API
matches and 48 general legacy-integer casts. Detailed review corrected the initial classification:
its pointer-looking casts encode small runtime table indices in opaque handles rather than object
addresses. The IFF migration removed its 45 measured casts; the subsequent UTF and SharedUTF work
removed the final five and replaced all adjacent-parameter serialized-call assumptions found in
those three implementations with explicit contexts. A pointer-width `EncodedHandle` contract
covers direct and offset indices; its native probe checks round trips, bounds, unchanged failure
output, underflow, the mapping flag, and the invalid-handle sentinel. This preserves runtime token
semantics; it does not widen persistent UTF/IFF fields or change the public `IFileSystem` ABI.
Native Windows validation remains required. See [IFF_PORTABILITY.md](IFF_PORTABILITY.md) and
[UTF_RUNTIME_PORTABILITY.md](UTF_RUNTIME_PORTABILITY.md).

After separating encoded indices, migrating the public DACOM templates, replacing six x86
stack-local debug checks, and correcting the remaining runtime callback contexts, the measured
high-confidence object/address-to-32-bit subset is zero. The DACOM work retained
the public `U32` connection-handle ABI but replaced node-address handles with explicit runtime
tokens; interface/dispatch runtime offsets are pointer-width. It has isolated GCC/Clang and
sanitizer evidence, not a Linux DACOM target or Windows ABI proof. See
[DACOM_RUNTIME_PORTABILITY.md](DACOM_RUNTIME_PORTABILITY.md). The stack checks now use a shared
pointer-width heuristic and retain their assertions; see
[STACK_ADDRESS_PORTABILITY.md](STACK_ADDRESS_PORTABILITY.md). Particle callbacks now use an
explicit pointer-width context; RichEdit and DirectShow use their pointer-width callback fields;
and the hotkey message ABI shares pointer-width window/parameter types with the SDL adapter. See
[RUNTIME_CONTEXT_PORTABILITY.md](RUNTIME_CONTEXT_PORTABILITY.md). The existing UTF inspector validates a read-only subset; it is not evidence that
DOSFile has been ported. DACOM has only six measured dynamic-loader calls, but its component, heap,
profile, and cross-module ABI ownership makes those calls higher risk than their count suggests.

## Ordered follow-up

The measured next steps are:

1. Complete the production profile/configuration boundary by adapting the tested settings owners
   from the tested live native `IUserDefaults` component into a real game-startup owner; the shared
   DACOM callback traversal still requires native Windows comparison and registry behavior must
   stay intact.
2. Validate the completed IFF/UTF/SharedUTF token and serialized-context migrations on native
   Windows while keeping small runtime tokens distinct from native OS handles and serialized data.
3. Validate the migrated DACOM runtime-offset, connection-token, native-manager and native
   `EventSystem` contracts on Windows, then adapt the next production ELF module and add game-global
   acquisition and heap policies. The x86 automation dispatch is a separate Viewer-tool boundary, not a manager
   prerequisite.
4. Validate the completed runtime-context and hotkey-message migrations across the affected native
   Windows modules, then continue classifying general casts and implicit pointer-width boundaries.
5. Integrate and compare the portable scalar math core through the existing `I3DMathEngine`
   boundary before considering the optional 3DNow provider.
6. Replace the classified D3DX9 surface feature by feature, beginning with shared renderer math
   ownership, then prototype one render vertical slice before selecting a native Vulkan strategy.
7. Defer audio, video, and networking replacement until startup, data loading, and the first
   renderer slice have executable evidence.

Windows Debug/Final rebuilds, ABI checks, and retail runtime checks remain required wherever a
portable change reaches the supported game graph. This inventory was generated on Linux without
building or running the game and without inspecting retail media.
