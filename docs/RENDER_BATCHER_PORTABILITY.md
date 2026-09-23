# ZBatcher RenderBatcher Header Closure

Status snapshot: **2026-09-22**. This document records the Linux declarative closure of the
production ZBatcher interface. It does not claim behavioral batching, a native game scene, or
completion of the Milestone 5 exit criteria.

## Question and falsifiable check

Milestone 5 requires the actual production `RenderBatcher` closure to compile and execute against
the now-portable public `IRenderPipeline`/`IRenderPrimitive` boundary. The supported Windows
baseline builds `Code/App/Src/RenderBatcher.cpp` through `Code/App/Src/ZBatcher.vcxproj`; the
legacy `Code/Libs/Src/RenderBatcher/RenderBatcher.cpp` copy is not in the supported build graph and
contains an unresolved `DA_DEFINE_NEW_OPERATOR` macro reference. The falsifiable check for this
session was therefore the declarative ZBatcher closure: whether `Code/App/Src/RenderBatcher.h`
and the headers it relies on could compile on a non-Windows toolchain without weakening the
supported Windows build or pretending the App-side `.cpp` translation unit had been executed.

The probe `frontierx_render_batcher_header_probe` includes the production header directly and
compiles it under both Linux compilers. This guards the header closure against regression. It is
a compile-time check only; it is not behavioral evidence of the batcher.

## What the closure required

Linux-includable prerequisites in this closure that were previously unavailable:

- `Code/App/Src/RenderBatcher.h` previously relied on include-order side effects and on
  `STDDAT.H`, `3dmath.h`, `rendpipeline.h`, `IRenderPrimitive.h`, `RPVertex1.h`, `da_d3dtypes.h`,
  `TYPEDEFS.H` and `BMPoolJr.h` already being included by the translation unit. The header now
  includes those prerequisites itself so the closure compiles as a single translation unit.
- `Code/Libs/Include/STDDAT.H` used a Windows-specific include for `<malloc.h>` and referenced the
  on-disk `TYPEDEFS.H` through the lowercase spelling `typedefs.h`. Both are fixed with the Linux
  branch keeping the semantics identical on Windows.
- `Code/Libs/Include/FVF.h` and `Code/Libs/Include/VertexBufferDesc.h` had the same lowercase
  `typedefs.h` mismatch.
- `Code/Libs/Include/TempStr.h` now accepts a non-Windows build, using `vsnprintf` instead of
  `wvsprintf` and including `<cstdarg>`/`<cstdio>` on non-Windows toolchains. The Windows
  `wvsprintf` path is unchanged.
- `Code/Libs/Include/FDump.h` uses `DXDEC` only when `IS_WIN32` is defined, and the error-handler
  function-pointer typedef uses `DACOM_CDECL` instead of the MSVC-only `__cdecl` so the header
  remains portable. Windows definitions are unchanged.
- `Code/Libs/Include/matrix.h` previously declared its free `mul`/`transpose_mul` helpers only
  through friend declarations. GCC requires a namespace-scope declaration for the explicit
  `::mul(...)` calls used by the inline friend operators; the header now declares them before
  the class. The friend declarations and the downstream definitions in `3dmath.h` are unchanged.
- `Code/Libs/Include/quat.h` keeps its existing `ANONYMOUS_ILLEGAL` fallback paths for the
  scalar/vector alias members. Those fallbacks are now selected for GCC and Clang, which reject
  the non-trivial `Vector` member inside the anonymous union. MSVC behavior is unchanged and the
  four-byte `SINGLE` overlay layout is preserved.
- `Code/Libs/Include/3dmath.h` previously relied on MSVC accepting `__stdcall` as a storage-class
  specifier on the `__MATH_ENGINE` declaration and on `MATH_ENGINE_Uninitialize`. The header now
  uses the project-standard `COMAPI` macro instead, which expands to `__stdcall` on MSVC and to
  nothing on non-Windows compilers. The pointer and calling semantics are unchanged.
- `Code/Libs/Include/IProfileParser_Utility.h` previously required `<windows.h>` for `SUCCEEDED`,
  `FAILED` and `stricmp`, and included `Vector.h`/`fdump.h`/`tempstr.h` with wrong-case names.
  The non-Windows branch now uses the on-disk header spellings, defines `SUCCEEDED`/`FAILED`
  with the `HRESULT`-compatible `>= 0` / `< 0` semantics on `GENRESULT`, and aliases `stricmp`
  to `strcasecmp`. Windows include behavior is unchanged.

## What remains open

Behavioral execution of the App-side ZBatcher `.cpp` on Linux is still blocked. The production
translation unit additionally requires `pch.h` (which pulls `vfx.h`, `Engine.h`, `GL/GL.h` and
`MyVertex.h`), `Code/App/Src/Include/Globals.h` (with `__declspec(dllexport/dllimport)` module
decorations), `Code/App/Src/Include/CQTrace.h` (which requires `ICQImage::Error`), `CQBatch.h`
(`ICQBatch` ABI used by other App modules), `DEffectOpts.h`, `IVertexBufferManager.h`, the
`BMPoolJr.h` implementation, and the `__int64` ticks member used by `ICQBatch`. Those dependencies
are listed in dependency order; closing them is the next concrete step on the Milestone 5 path
and must not weaken the supported Windows ZBatcher build.

## Validation record

Target/configuration: `frontierx_render_batcher_header_probe` on Linux x86_64 with GCC 16.2.1
and Clang 22.1.8. The following commands executed on this host:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_header_probe -j2
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_header_probe -j2
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug
```

Both compilers build the probe successfully and `ctest` reports 100% pass out of 33 under both
presets; the host-dependent `portability.sdl3_gpu` test remains skipped as environment
information, not as runtime evidence. Inputs are repository-owned source files only; no retail
data was used. Native Windows Debug/Final rebuilds, cross-module ABI/layout checks, Windows
image comparison, behavioral ZBatcher execution, RPVERTEX2 GPU consumption and texture-stage/
material mapping were not performed for this change.

### FDump include-order regression fix (2026-09-22)

Source review found that `FDump.h` used `DACOM_CDECL` and `IS_WIN32` without including
their owner, `DACOM.H`. Callers such as `def_patch.cpp` include FDump first. The header
now includes DACOM explicitly; no callback signature or persistent layout changes.
The existing batcher header probe includes FDump first so the larger header closure
cannot mask this regression. No additional probe target is needed.

Compile evidence on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8: a translation unit
containing only `#include "FDump.h"` passes each compiler's
`-x c++ -std=c++17 -fsyntax-only -ICode/Libs/Include -` check. Both
`cmake --build --preset linux-gcc-debug` and
`cmake --build --preset linux-clang-debug` succeeded. Automated execution through
`ctest --preset linux-gcc-debug` and `ctest --preset linux-clang-debug` passed the
`portability.render_batcher_header` regression check and all other executed checks;
the host-dependent GPU test was skipped. Inputs were repository-owned headers only.
Native Windows Debug/Final builds, ABI checks, GPU and retail runtime remain unperformed.
