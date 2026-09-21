# Portable Render-Submission Contract

Status snapshot: **2026-09-20**. This is a narrow runtime transfer contract derived from existing
renderer consumers. It is not a replacement `IRenderPipeline`, material system, vertex-buffer
manager, or persistent mesh format.

## Source-owned boundary

Source review identified two concrete consumers before the contract was introduced:

- `RPUL/PrimitiveBuilder` accumulates the established `RPVertex` shape and calls
  `IRenderPipeline`/`IRenderPrimitive::draw_primitive` directly.
- `RenderBatcher` copies direct or 16-bit-indexed vertex submissions, preserves topology and state,
  and later forwards them through `draw_primitive` or `draw_indexed_primitive`.

TriMesh and the material implementations additionally use the same primitive/topology semantics
through vertex-buffer draw calls. The common first slice is therefore deliberately limited to:

- position as three 32-bit floats;
- diffuse color as the legacy packed BGRA8 word;
- one two-float texture coordinate;
- point list, line list/strip, and triangle list/strip topology; and
- optional 16-bit indices.

`FrontierX/RenderSubmission.h` owns `RenderVertexP3DiffuseT2`, `PrimitiveTopology`, and the borrowed
`RenderSubmissionView`. It contains no SDL, Vulkan, Win32, Direct3D, DACOM or allocation type.

## Layout contract

The vertex is a 24-byte standard-layout runtime value:

| Offset | Field | Width |
|---:|---|---:|
| 0 | position X/Y/Z | 12 bytes |
| 12 | packed diffuse BGRA8 | 4 bytes |
| 16 | texture U/V | 8 bytes |

Compile-time checks cover size, both offsets, standard layout and the end of the final field.
`PackDiffuseBgra` makes the channel-word mapping explicit rather than relying on a compiler-specific
anonymous union. The SDL GPU probe binds the color as normalized four-byte BGRA and performs the
BGRA-to-RGB swizzle in its vertex shader.

This type is not serialized and does not alter `RPVertex`, its FVF constant, any module ABI, or the
supported Windows renderer. The existing probe now compiles the real `RPVertex` declaration and
asserts its standard layout, 24-byte size and position/color/UV offsets against the portable type.
Native Windows compile/layout comparison remains an acceptance gate; Linux assertions are not a
substitute for the supported MSVC Win32 ABI check.

## Legacy draw-call adapter

`FrontierX/LegacyRenderSubmission.h` records the stable D3D9 primitive values 1 through 6 and the
exact `D3DFVF_RPVERTEX` combination `0x142` (`XYZ | DIFFUSE | TEX1`) without pulling Direct3D into
the portable target. Those values were checked against the Microsoft D3D9 definitions:
[D3DPRIMITIVETYPE](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dprimitivetype) and
[D3DFVF](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dfvf).

`AdaptLegacyRpVertexSubmission` accepts an untyped legacy source and caller-owned portable vertex
storage. It validates the primitive, exact FVF, counts, pointers, U16 bounds and destination capacity
before using `memmove` to create typed portable values. This avoids reading a live `RPVertex` object
through an unrelated C++ struct type. The output remains empty on failure. Point, line and triangle
lists plus line and triangle strips map directly; triangle fans, other FVF layouts and indexed point
lists return explicit unsupported statuses rather than being approximated.

## Production caller and native owner closure

The named integration blocker was that including `RPUL/PrimitiveBuilder.h` on Linux pulled the
entire historical `rendpipeline.h` implementation surface through `IRenderPrimitive.h`. That made a
single real draw caller depend at compile time on unrelated DirectDraw, D3DX, Win32 window/DC and
legacy math declarations. The production owner is `PrimitiveBuilder`; its real consumer boundary is
`IRenderPrimitive::draw_primitive`. The falsifiable hypothesis was that the primitive-only route can
compile and preserve its existing reference, FVF, topology, vertex and quad-expansion behavior
without first porting the full `IRenderPipeline` interface.

The public `IRenderPrimitive` declaration is now self-contained around DACOM, stable D3D9 value
types and forward-declared matrix types. `RPVertex1.h` declares its own prerequisites and preserves
the existing anonymous color overlay with compiler-local diagnostic scoping. On non-Windows builds,
`PrimitiveBuilder` exposes its existing `IRenderPrimitive` constructor while the still-unportable
full-pipeline constructor remains Windows-only. The `_WIN32` route retains both constructors and the
pipeline fallback.

`frontierx_native_render_primitive` now supplies a registered native implementation of the real
`IRenderPrimitive` interface. Creation uses `NativeComponentManager` and an explicit native-only
descriptor containing a synchronous submission callback and context. Direct and U16-indexed draws
are checked by `AdaptLegacyRpVertexSubmission`, copied into owner-held typed vertex storage and
borrowed by the callback only for its duration. Invalid input is rejected, unsupported primitive or
FVF combinations return `GR_NOT_IMPLEMENTED`, allocation failure remains distinct, and callback
failures propagate to the legacy caller. State, scene, material, texture, vertex-buffer and flush
methods fail explicitly until their owning backend semantics exist.

The existing render-submission probe, rather than a new target, instantiates the production builder
with both a local contract capture and the registered native owner. `End()` crosses the real virtual
draw boundary. The check covers retain/release balance, exact `D3DFVF_RPVERTEX`, a three-vertex
triangle, color/UV preservation, and production quad expansion from four vertices into triangle-list
order `0,1,2,0,2,3`. It also submits bounded U16 indices through the registered owner, checks factory
and manager shutdown, verifies explicit triangle-fan rejection, and proves callback error
propagation. The SDL GPU probe uses that same registered owner as the source of its upload data.

## Validation behavior

`RenderSubmissionView::IsValid` rejects:

- null or empty vertex input;
- vertex or index counts outside the existing 32-bit renderer count domain;
- a null index pointer with a nonzero count or a non-null pointer with a zero count;
- any 16-bit index outside the supplied vertex range; and
- element counts invalid for the selected list/strip topology.

`PrimitiveCount` returns zero for invalid input and otherwise uses the source-reviewed list/strip
formulas. The contract borrows all pointed-to data for the duration of a synchronous submission;
it neither owns nor serializes pointers.

## Verification evidence

`frontierx_render_submission_probe` passes under GCC and Clang with strict warnings. It covers the
24-byte layout and packed color constant, direct and indexed triangle lists, invalid pointers/counts,
out-of-range indices, oversized vertex and index counts, unknown topology values and primitive
counts for every supported topology. It also covers exact legacy FVF/topology mapping, byte-preserving
conversion, triangle-fan and indexed-point rejection, invalid legacy indices and insufficient
destination capacity, plus the production-caller and registered-owner closure described above.

The following focused checks were actually built and executed on Linux x86_64 on 2026-09-20 with
GCC 16.2.1 and Clang 22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_submission_probe -j2
out/build/linux-gcc-debug/frontierx_render_submission_probe
cmake --build --preset linux-clang-debug --target frontierx_render_submission_probe -j2
out/build/linux-clang-debug/frontierx_render_submission_probe
ctest --test-dir out/build/linux-gcc-debug -R '^portability\.render_submission$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability\.render_submission$' --output-on-failure
```

Both executables reported `render submission probe passed`; each focused CTest selected and passed
one test. A separate Clang 22.1.8 build used `-fsanitize=address,undefined` and executed successfully
with `ASAN_OPTIONS=detect_leaks=0`. LeakSanitizer itself cannot run under this host's `ptrace`
environment, so no leak-check claim is made. Inputs were repository-owned synthetic vertices and
checked-in production headers; no retail files were used. The focused CPU probe itself did not
require renderer hardware.

The sanitizer build used a fresh out-of-tree directory and these material options before building
and executing both renderer targets:

```text
cmake -S . -B /tmp/frontierx-render-sanitize-20260920 -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ \
  -DFRONTIERX_BUILD_SDL3=ON -DFRONTIERX_BUILD_SDL3_GPU=ON \
  '-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined'
cmake --build /tmp/frontierx-render-sanitize-20260920 \
  --target frontierx_render_submission_probe frontierx_sdl3_gpu_probe -j2
ASAN_OPTIONS=detect_leaks=0 \
  /tmp/frontierx-render-sanitize-20260920/frontierx_render_submission_probe
```

The GPU executable from the same build then ran visibly with `--require-runtime --visible` and the
generated shaders. It reported the owner source, twelve presented frames and the pixel result below.

`frontierx_sdl3_gpu_probe` is the first backend consumer. The production `PrimitiveBuilder` expands
the repository-owned triangle plus quad fixture into nine direct legacy `RPVertex` values, calls the
registered `IRenderPrimitive`, and the validated/copied portable submission is uploaded to SDL GPU
in the same process. The fixture preserves the earlier indexed order and image; the probe separately
submits the seven unique vertices and nine U16 indices through the same owner before restoring the
builder output for rendering. Visible GCC 16.2.1 and Clang 22.1.8 executions each presented twelve
frames and produced 2,496 changed pixels with the same local diagnostic checksum,
`3246152458680859569`. The native shell uses the same shared fixture and reproduced that pixel
result under GCC, Clang and a focused Clang ASan/UBSan build. Leak detection was disabled because
LeakSanitizer is unavailable in the host's `ptrace` environment.

## Remaining adapter work

- Promote the shell-selected geometry consumer toward the existing stateful renderer boundary. The
  reusable SDL GPU class now owns lifecycle/presentation and both shell and probe use one shared
  scene, but it is not an `IRenderPipeline` implementation or a game renderer.
- Add the vertex-buffer-handle path. Handles must remain typed runtime tokens, never serialized
  pointer values.
- Define triangle-fan/polygon policy. Quad expansion is now characterized, but the portable
  adapter deliberately rejects direct triangle-fan submissions.
- Separate render/material/sampler/depth/blend state capture from geometry submission using the
  existing owners. Do not fold the entire stateful `IRenderPipeline` interface into this view.
- Build the affected headers and run ABI/layout plus representative draw comparison on native
  Windows Debug and Final. Run the eventual scene against lawful retail data and record image,
  depth, blend, resize and lifetime evidence. None of those checks was performed here.
