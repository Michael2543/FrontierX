# Native math-backend readiness

Evidence: source review plus isolated GCC/Clang compilation and synthetic tests, 2026-09-16.
No Windows build, retail execution, or cross-build floating-point comparison was performed.

## Why this boundary matters

`Code/Libs/Include/3dmath.h` is the existing engine boundary for matrix, transform, quaternion,
square-root and vector-transform operations. Inline engine types call `MATH_ENGINE()`. The static
`MathLib.lib` implementation in `Code/Libs/Src/x86Math/MathLib.cpp` does not perform mathematics;
it asks DACOM for an `I3DMathEngine` provider. Successfully linking `MathLib.lib` therefore does
not provide a usable math engine by itself.

The source contains two providers with very different portability properties:

| Source | Declared build owner | Finding |
|---|---|---|
| `x86math.cpp` | `x86math.vcxproj` | Standard scalar C++ implementation; its project compiles only this source and contains no measured inline-assembly match |
| `K62math.cpp` and `3DNow_VC.h` | legacy `k62math.dsp` | Optional AMD 3DNow provider; contains 402 of the inventory's 441 remaining inline-assembly candidates |

`x86math.vcxproj` and `MathLib.vcxproj` are separate projects. `MathLib.vcxproj` is reachable from
the supported application graph, including `ZBatcher.vcxproj`. The supported
`Build-Conquest.ps1` engine list builds only `D3DRenderPipe.vcxproj` and `MeshManager.vcxproj`; it
does not build `x86math.vcxproj`, and its engine-output verification lists only those two DLLs.
Older full-build declarations and tool configuration files name `x86math.dll`; the historical
`build.pl` also names both standard and K62 providers. This proves that the provider is a real
component boundary, but not which provider a current retail run loads. That still requires a
native Windows trace or an independently documented runtime configuration.

## Portable core now available

`FrontierX/MathCore.h` and `MathCore.cpp` provide an isolated standard-C++ implementation of the
operation surface needed by the standard provider:

- determinant, inverse, scale and matrix multiplication;
- affine-transform multiplication, rotation, forward/inverse vector transforms and list forms;
- matrix/quaternion conversion, quaternion multiplication, vector rotation and slerp; and
- the historical lookup-table plus two-iteration inverse-square-root algorithm.

The new types are intentionally ABI-neutral value types. They are not persistent structures and
do not replace `Matrix`, `Transform`, `Quaternion`, `I3DMathEngine`, or any DLL ABI. This permits
Linux compilation and characterization without widening or silently changing the supported Win32
interface. Source and destination aliasing is supported for the single-value operations.

The inverse-square-root port replaces the historical assumption that `unsigned long` is 32 bits
with `uint32_t` and uses `memcpy` for float bit transfer instead of union/pointer type punning. It
retains the 9-bit table, rounding step and two Newton iterations. Its current tested contract is
positive finite IEEE-754 binary32 input; special values and zero need oracle evidence before a
production adapter can rely on them.

Both `frontierx_math` and its probe compile with `-ffp-contract=off` on GCC and Clang. The project
does not enable fast-math. This prevents contraction inside this slice, but it is not evidence of
bit-identical results versus the VS2022 Win32 provider.

## Automated evidence

The `frontierx_math_core_probe` CTest covers determinant and inverse composition, singular and
in-place inversion, general affine inversion and composition, scalar and in-place list transforms,
quaternion conversion, multiplication and vector rotation, midpoint slerp, and representative
inverse-square-root and square-root results. The probe passed under the complete GCC and Clang
preset matrices. These are synthetic numerical checks, not gameplay or determinism certification.

## Remaining integration gates

1. Add a thin `I3DMathEngine` adapter only after native Windows ABI ownership and provider-loading
   behavior are recorded. The adapter should translate existing engine values to this core without
   changing public layouts.
2. Capture Windows reference outputs for normal, singular, nearly singular, quaternion edge and
   inverse-square-root cases. Compare exact bits where retail determinism requires them and bounded
   error only where the original contract permits it.
3. Decide and document floating-point environment policy: rounding mode, denormal handling,
   exception masks, x87-versus-SSE behavior and compiler contraction settings.
4. Keep `K62Math` outside the first Linux target. It is an optional historical optimization, not a
   reason to translate 3DNow assembly before the scalar provider is integrated and measured.
5. Repair the supported Windows build/deploy wrapper separately so it builds or explicitly verifies
   the selected math-provider DLL. That is a Win32 baseline issue and cannot be validated on Linux.

Until those gates pass, the repository has a tested native math core, not a wired native game math
provider and not a determinism-equivalent Linux game.

## Production MathLib accessor closure (2026-09-23)

The supported `RenderBatcher.cpp` object refers to `__MATH_ENGINE()` through
inline methods in `3dmath.h`. The real `Code/Libs/Src/x86Math/MathLib.cpp`
accessor is now a native static target. It retains the existing DACOM
`DA3DMATHDESC` lookup, gates its Windows header to Windows, spells its include
with the on-disk case and uses the existing `COMAPI` calling-convention macro.
On a failed lookup it returns a null pointer instead of an uninitialized local.
This change does not register an `I3DMathEngine` provider or change numerical
operations.

The new `frontierx_math_accessor_probe` checks a named integration blocker:
whether this real accessor can be linked into the native renderer closure and
reports an absent provider deterministically. Its real caller is the
production `RenderBatcher` math-header path. The existing `frontierx_math_core_probe`
cannot check DACOM lookup because it tests only ABI-neutral numerical values.
If the accessor returned a non-null pointer without a registered provider,
the next integration would have an invalid ownership assumption; a null result
confirms that a provider must be registered before math-dependent batching can
run.

Executed on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_math_accessor_probe -j2
ctest --preset linux-gcc-debug -R '^portability.math_accessor$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_math_accessor_probe -j2
ctest --preset linux-clang-debug -R '^portability.math_accessor$' --output-on-failure
```

Both focused checks passed two absent-provider lookups. Inputs were the
repository-owned native component bootstrap with no math provider; this is
synthetic runtime evidence for the accessor's failure path. A provider-backed
lookup, mathematical execution through `I3DMathEngine`, Windows Debug/Final
rebuilds, cross-module ABI and numerical comparison with retail remain open.
