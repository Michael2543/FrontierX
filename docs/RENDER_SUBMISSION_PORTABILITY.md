# Portable Render-Submission Contract

Status snapshot: **2026-09-23**. This is a narrow runtime transfer contract derived from existing
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

The public `IRenderPrimitive` declaration is self-contained around DACOM, stable D3D9 value types
and forward-declared matrix types. `RPVertex1.h` declares its own prerequisites and preserves the
existing anonymous color overlay with compiler-local diagnostic scoping. The later pipeline slice
made `rendpipeline.h` includable on Linux as well: non-Windows declarations use fixed-width
`DDPIXELFORMAT`, `RECT` and opaque Direct3D resources while the Windows branch retains the SDK
types. `PrimitiveBuilder` therefore exposes both of its existing production constructors on Linux:
`IRenderPrimitive *` and `IRenderPipeline *`.

`frontierx_native_render_primitive` now supplies a registered native implementation of the real
`IRenderPrimitive` interface. Creation uses `NativeComponentManager` and an explicit native-only
descriptor containing a synchronous submission callback and context. Direct and U16-indexed draws
are checked by `AdaptLegacyRpVertexSubmission`, copied into owner-held typed vertex storage and
borrowed by the callback only for its duration. Invalid input is rejected, unsupported primitive or
FVF combinations return `GR_NOT_IMPLEMENTED`, allocation failure remains distinct, and callback
failures propagate to the legacy caller. Six render-state methods and the checked vertex-buffer
route described below have native semantics; scene, material, texture-stage, sampler, texture
creation and other unported pipeline methods still fail explicitly until their owning backend
semantics exist.

The existing render-submission probe, rather than a new target, instantiates the production builder
with both a local contract capture and the registered native owner. `End()` crosses the real virtual
draw boundary. The check covers retain/release balance, exact `D3DFVF_RPVERTEX`, a three-vertex
triangle, color/UV preservation, and production quad expansion from four vertices into triangle-list
order `0,1,2,0,2,3`. It also submits bounded U16 indices through the registered owner, checks factory
and manager shutdown, verifies explicit triangle-fan rejection, and proves callback error
propagation. The pipeline regression additionally instantiates the production builder with
`IRenderPipeline *` and crosses the real pipeline vtable into that owner. The SDL GPU probe uses the
same pipeline-to-primitive route as the source of its upload data.

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
generated shaders. It verified the owner-to-readback lifecycle with the earlier opaque fixture. That
run predates the depth/alpha state slice and is not the source of the current checksum below.

`frontierx_sdl3_gpu_probe` is the first backend consumer. The production `PrimitiveBuilder` expands
the repository-owned triangle plus quad fixture into nine direct legacy `RPVertex` values, calls the
real `IRenderPipeline` vtable, and reaches the registered `IRenderPrimitive`; the validated/copied
portable submission is uploaded to SDL GPU in the same process. The fixture preserves the earlier
indexed order and image; the probe separately submits the seven unique vertices and nine U16
indices through the same owner before restoring the pipeline-builder output for rendering. Visible
GCC 16.2.1 and Clang 22.1.8 executions each presented twelve
frames and produced 2,496 changed pixels with the same local diagnostic checksum,
`7951318278861886233`. The value changed when the fixture began preserving non-opaque diffuse alpha
and enabled `SRCALPHA`/`INVSRCALPHA` blending. The native shell uses the same shared fixture and
reproduced that result under the current GCC and Clang builds. The earlier focused Clang ASan/UBSan
GPU run predates the alpha-state change; current sanitizer evidence for the state change is the
CPU owner test described below. Leak detection was disabled because LeakSanitizer is unavailable in
the host's `ptrace` environment.

## Remaining adapter work

- Complete the actual production `RenderBatcher` closure beyond the direct and U16-indexed
  branches now executed through `IRenderPipeline`; queued pools, texture/material state and
  native production global services remain open.
- Define triangle-fan/polygon policy. Quad expansion is now characterized, but the portable
  adapter deliberately rejects direct triangle-fan submissions.
- Continue render/material/sampler capture beyond the six depth/blend states now connected through
  the existing owner. Do not fold the entire stateful `IRenderPipeline` interface into this view.
- Build the affected headers and run ABI/layout plus representative draw comparison on native
  Windows Debug and Final. Run the eventual scene against lawful retail data and record image,
  depth, blend, resize and lifetime evidence. None of those checks was performed here.

## Native vertex-buffer pipeline adapter (2026-09-21)

Source review identifies `Code/App/Src/RenderBatcher.cpp` as the production consumer: it fills
`D3DFVF_RPVERTEX` and `D3DFVF_RPVERTEX2` buffers and flushes them through the existing
`IRenderPipeline` vertex-buffer methods. The supported D3D9 owner in
`Code/Libs/Src/RenderPipeline/DirectDraw/D3D/Direct3D_pipe.cpp` passes `start_vert` as
`BaseVertexIndex`, with unchanged slice-relative U16 indices. The adapter must preserve that
contract for nonzero starts; treating indices as absolute buffer offsets rejects valid batches.

`NativeRenderPipeline` is a native implementation of the existing DACOM `IRenderPipeline`
interface. `QueryInterface`, `AddRef` and `Release` expose the unchanged vtable contract; the
pipeline borrows its `IRenderPrimitive` draw target, so the enclosing owner must keep that target
alive. `RenderPipelineAdapter` implements every as-yet-unported legacy slot as an explicit
`GR_NOT_IMPLEMENTED` result (or null for the historical effect-pointer return). Implemented
primitive methods synchronously forward to the draw target. The pipeline supports either a
context-aware callback that returns a borrowed read-only buffer view or an internally owned native
buffer store. Both views carry the actual FVF and byte size and remain valid through the synchronous
draw. The adapter exposes the production direct and indexed vertex-buffer methods;
both inspect the buffer FVF and dispatch RPVertex or RPVERTEX2 without format-specific parallel
entry points. It checks the format and byte extent before accessing a slice, copies its bytes into
typed portable storage, validates relative indices and forwards them unchanged through
`IRenderPrimitive`. The draw target is borrowed and must outlive the call. Allocation failures
return `GR_OUT_OF_MEMORY`; invalid inputs, unsupported formats/topologies and owner failures remain
explicit. These are runtime-only types and do not change persistent formats or existing module
interfaces.

The owned path implements the existing `create_vertex_buffer`, `lock_vertex_buffer`,
`unlock_vertex_buffer` and `destroy_vertex_buffer` call shapes for exact RPVertex and RPVERTEX2
formats. Handles are monotonically allocated nonzero `U32` tokens, never encoded pointers. Creation
validates count, format, the three established `IRP_VBF_*` flag bits and allocation extent. Lock
returns writable CPU bytes plus vertex capacity, rejects double locks and keeps locked buffers out
of draw lookup. Unlock rejects unmatched calls; destruction rejects a live lock, erases storage and
leaves the stale token invalid. Lock flags remain optimization hints, matching the current D3D9
implementation, which does not branch on them. `OwnedVertexBufferLookup` exposes only the borrowed
read-only synchronous view needed by a registered primitive owner.

The registered native `IRenderPrimitive` owner now closes this adapter route at its existing
vertex-buffer vtable. `NativeRenderPrimitiveDesc` can supply an optional buffer lookup plus a
separate lookup context; when present, the owner constructs and owns `NativeRenderPipeline` and
uses itself as the borrowed direct-draw target. Consequently both `draw_primitive_vb` and
`draw_indexed_primitive_vb` follow the same validation and callback path as direct submissions.
The descriptor also separates RPVertex and RPVERTEX2 synchronous callbacks so a backend cannot
accidentally interpret a 32-byte vertex as the 24-byte layout. A missing lookup keeps the
vertex-buffer methods explicitly unsupported, and a missing extended callback rejects RPVERTEX2.
The lookup view, draw target and callback data remain borrowed only for their documented
synchronous calls; the owner copies all vertex bytes into its own typed storage before invoking a
backend callback.

The extended `RenderVertexP3DiffuseT4` is the 32-byte transfer layout corresponding to `Vertex2`:
position, packed BGRA8 diffuse, and two pairs of texture coordinates. Both submission views share
count, topology and index validation. `AdaptLegacyRpVertex2Submission` copies bytes into the supplied
destination and returns a view of that destination, including when the source is unaligned legacy
storage. Direct and indexed extended-format calls use the same list/strip rules as RPVertex;
triangle fans and indexed point lists remain explicitly unsupported.

This is an `IRenderPipeline` implementation, but not a complete game renderer. The native adapter
owns repository-created CPU buffers and the registered primitive consumes those resources through
its production-shaped vertex-buffer vtable. The shell now executes the App-side `RenderBatcher`
direct and U16-indexed branches to GPU output, and the separate SDL probe consumes RPVERTEX2 through its extended
callback. Queued batcher execution, material texture binding and native Windows/retail comparison
remain open; the later stage-zero combine-state slice is recorded below.

## Public pipeline-vtable closure (2026-09-21)

The named blocker was the Linux-incompatible public `rendpipeline.h`, which prevented the existing
pipeline constructor in `PrimitiveBuilder` from participating in a native build. The header now
keeps its Windows SDK includes unchanged on `_WIN32` and uses declaration-only native equivalents
elsewhere. The portable `DDPIXELFORMAT` remains the eight-`U32`, 32-byte DirectDraw record; `RECT`
remains four signed 32-bit fields; Direct3D resources that occur only by pointer remain opaque. The
Linux branch forward-declares the legacy math classes instead of importing the still-unported math
provider through this interface. These are runtime declarations and do not modify a persistent or
wire layout.

`NativeRenderPipeline` derives from the real interface and implements direct/indexed submissions,
the four CPU vertex-buffer lifetime calls, six render-state forwards and deterministic absence of
depth-bias support. Its target is the registered native `IRenderPrimitive`; unsupported lifecycle,
buffer, transform, lighting, state-block, texture and effect operations remain explicit rather than
pretending the game renderer exists. The shell and GPU probe construct this owner around the
registered primitive and pass it to the production `PrimitiveBuilder(IRenderPipeline *)`
constructor. The focused CPU regression also checks `QueryInterface`, reference balance, public
layout declarations, unsupported-slot behavior, direct and indexed vtable dispatch, and the shared
repository scene through the production builder.

This closes interface inheritance and one real caller, not the `RenderBatcher` closure. It also does
not select SDL GPU as the production backend: `SdlGpuRenderer` remains a callback consumer behind
the existing renderer interfaces.

## Native stage-zero texture combine state (2026-09-23)

Source review of the production material callers identified `MODULATE` of diffuse and texture
for stage-zero color and alpha, with stage one disabled. The native-only `D3DTSS`, `D3DTOP` and
`D3DTA` constants retain the D3D9 numeric values; Windows continues to use its SDK definitions.
The registered `IRenderPrimitive` now accepts six stage-zero fields through an optional complete
snapshot callback, commits after backend success and exposes committed values through the matching
getter. It accepts stage-one disable as a no-op and rejects other stage/state combinations.
The SDL owner validates and pushes the snapshot as a fragment uniform for presentation and bounded
readback. The existing visible GPU probe observes a different image under `SELECTARG1` diffuse,
checks rejection of stage-zero disable without a state change, and recovers the exact original
image on restoring `MODULATE`. A separate texture-only alpha selection changes the blended image
and restoring alpha `MODULATE` recovers it again. GCC, Clang and focused Clang ASan/UBSan visible Vulkan runs passed
on repository-owned synthetic input; exact commands and observations are in
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md). Texture resource binding, a real
material caller on Linux, Windows Debug/Final, module ABI and retail comparison remain open.

The later native binding slice now passes a live `U32` handle through
`IRenderPipeline::set_texture_stage_texture`, `IRenderPrimitive` and the SDL backend. The pipeline
also answers `is_texture` through the backend's live table, allowing the production batcher to
forward an additional synthetic fixture texture. Its GPU image changes and recovers exactly after
rebinding; unknown and stale handles are rejected. The production material owner and texture
creation/lock API remain open; see
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

## Native depth and alpha-blend state slice (2026-09-21)

Source review selected six values used together by existing renderer consumers rather than adding
an unrelated state API. `SimpleMaterial` enables `SRCALPHA`/`INVSRCALPHA` for translucent material
diffuse alpha. `AreaEffectBolt` combines depth enable, disabled depth writes, alpha blending and
source-alpha blending. The native owner now accepts and returns the following existing
`IRenderPrimitive` states:

| Legacy state | Snapshot field | Accepted values |
|---|---|---|
| `D3DRS_ZENABLE` | `depthEnabled` | Boolean `0` or `1` |
| `D3DRS_ZWRITEENABLE` | `depthWriteEnabled` | Boolean `0` or `1` |
| `D3DRS_ZFUNC` | `depthFunction` | `D3DCMP_NEVER` through `D3DCMP_ALWAYS` |
| `D3DRS_ALPHABLENDENABLE` | `alphaBlendEnabled` | Boolean `0` or `1` |
| `D3DRS_SRCBLEND` | `sourceBlend` | directly mappable factors `ZERO` through `SRCALPHASAT` |
| `D3DRS_DESTBLEND` | `destinationBlend` | directly mappable factors `ZERO` through `SRCALPHASAT` |

`RenderStateSnapshot` is an SDL-free value owned by the native primitive. A `set_render_state`
call copies the committed snapshot, validates and changes one field, and invokes the optional
backend state callback with the complete candidate. The owner commits only after `GR_OK`; a backend
failure is returned unchanged and the prior value remains visible through `get_render_state`.
Unknown state identifiers and D3D9 blend factors without an SDL mapping return
`GR_NOT_IMPLEMENTED`; malformed Boolean, comparison or blend values return `GR_INVALID_PARMS`.
An owner without a state callback keeps both state entry points explicitly unsupported.

The shared smoke scene configures depth test/write, `LESSEQUAL`, alpha blending and
`SRCALPHA`/`INVSRCALPHA` before submitting geometry. Its foreground vertices use alpha 210 and its
background quad uses alpha 160, so the shaders now carry BGRA input through an RGBA varying and the
fragment shader preserves diffuse alpha. `SdlGpuRenderer` maps the snapshot into SDL depth/stencil
and color-target blend fields. It creates the replacement graphics pipeline first, waits for GPU
idle, then releases the previous pipeline and commits the snapshot. Creation or idle-wait failure
releases the candidate and leaves the active pipeline and native-owner state unchanged.

The render-submission regression verifies defaults; successful set/get for depth write, comparison,
blend enable and both factors; invalid and unsupported values; callback failure propagation; and
rollback to the last committed state. On 2026-09-21 it passed in both complete GCC and Clang
presets. A fresh focused Clang ASan/UBSan build passed `portability.render_submission` and
`portability.render_pipeline` with leak detection disabled. Separate required visible executions of
both preset GPU probes and both native shells used the local Wayland/Vulkan session. The probes each
presented 12 frames, and the shells each presented 17; all four found 2,496 changed pixels and
checksum `7951318278861886233` after the production caller moved to the real pipeline vtable. Probe
output identifies `source=primitivebuilder-native-irenderpipeline`, and both shells identify
`production PrimitiveBuilder via IRenderPipeline`. This is synthetic Linux hardware evidence. It is
not a Windows image comparison, retail-data run, RPVERTEX2 GPU path or proof of complete game-state
compatibility. A later 2026-09-23 probe added the extended `RPVERTEX2` GPU path; see
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md) for the separate visible GCC, Clang
and sanitizer image evidence.

### Review regression evidence (2026-09-21)

The existing `frontierx_render_pipeline_probe` was extended because the submission-only probe has
no vertex-buffer handle, format or slice boundary. The named regressions are incorrect nonzero-start
index semantics, null-index dereferences, unsupported-format overreads, divergence from the two
production draw entry points, incomplete extended validation and missing destination copies.
Passing these checks permits further adapter integration; it does not establish a playable or
rendered scene.

Automated execution with repository-owned synthetic buffers verifies:

- the public pipeline declaration compiles with fixed 32-bit vertex handles, 32-byte
  `DDPIXELFORMAT` and 16-byte `RECT`, and `NativeRenderPipeline` derives from `IRenderPipeline`;
- `QueryInterface` and reference balancing preserve owner lifetime, unknown interfaces clear their
  output, unported lifecycle calls fail explicitly, and unsupported queried abilities are
  deterministic;
- the production `PrimitiveBuilder(IRenderPipeline *)` constructor sends the nine-vertex shared
  repository scene through the actual pipeline vtable;
- create/lock/fill/unlock/direct and indexed draw/relock/destroy sequences for owned RPVertex and
  RPVERTEX2 buffers, including capacity reporting and monotonically fresh handles;
- double lock/unlock, destruction while locked, unknown flags, unsupported formats and stale handles
  fail without exposing storage or reaching the draw target;
- the two production-shaped draw methods select both formats from buffer metadata and preserve
  nonzero-start slice bytes and relative indices;
- null indices, out-of-range indices, truncated buffers, unsupported FVF values and missing lookup,
  context or draw owners fail without reaching the draw target;
- incomplete triangles and invalid topology values fail, with valid list/strip count coverage;
- extended conversion copies unaligned source bytes into caller storage, remains independent of
  source changes and leaves the destination unchanged on failed validation; and
- missing owners and downstream failures are returned explicitly.

The render-submission probe additionally creates the registered native owner through
`NativeComponentManager` and verifies that:

- direct RPVertex and RPVERTEX2 calls reach distinct typed callbacks with exact texture-coordinate
  bytes;
- the owner's two existing vertex-buffer methods perform context-aware lookup and route nonzero
  slices through the adapter for both formats;
- an indexed RPVERTEX2 buffer draw preserves slice-relative U16 indices and the second texture
  coordinate pair; and
- an owned RPVertex buffer crosses create/lock/fill/unlock into the registered owner's indexed
  vertex-buffer vtable, then fails lookup after destruction; and
- missing handles and unsupported buffer FVF values do not invoke either backend callback.

The following commands executed on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_pipeline_probe \
  frontierx_render_submission_probe -j2
ctest --test-dir out/build/linux-gcc-debug -R '^portability\.render_(pipeline|submission)$' \
  --output-on-failure
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug
```

Both presets now include the new executable in their explicit build target lists. Both renderer
checks passed under both compilers; the complete preset results are recorded in
[BUILD_GRAPH.md](BUILD_GRAPH.md). The GPU CTest skipped, so the preset runs themselves add no GPU
runtime evidence; the separately required visible state-slice runs are recorded above.

A fresh Clang Debug sanitizer directory configured, built and executed the same two CPU checks after
pipeline inheritance and production-builder dispatch were connected. The generated directory was
`/tmp/frontierx-render-pipeline-sanitize-0hYwSA`:

```text
render_sanitize_dir=$(mktemp -d /tmp/frontierx-render-pipeline-sanitize-XXXXXX)
cmake -S . -B "$render_sanitize_dir" -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ \
  -DFRONTIERX_BUILD_SDL3=OFF -DFRONTIERX_BUILD_SDL3_GPU=OFF \
  '-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined'
cmake --build "$render_sanitize_dir" \
  --target frontierx_render_pipeline_probe frontierx_render_submission_probe -j2
ASAN_OPTIONS=detect_leaks=0 ctest --test-dir "$render_sanitize_dir" \
  -R '^portability\.render_(pipeline|submission)$' --output-on-failure
```

Both checks passed with no ASan/UBSan diagnostics, including real pipeline inheritance and
production-builder dispatch, registered-owner creation, internal adapter ownership, both callback
layouts and manager shutdown. Leak detection was disabled; no leak-check claim is made. No retail
data was used. Native Windows Debug/Final rebuilds,
cross-module ABI/layout checks, Windows image comparison, RPVERTEX2 hardware rendering and actual
`RenderBatcher` compilation/execution were not performed for this correction.
