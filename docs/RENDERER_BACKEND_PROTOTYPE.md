# SDL3 GPU Renderer-Backend Prototype

Status snapshot: **2026-09-27**. This document records a focused backend experiment. It does not
select the production renderer, connect the game renderer, or claim a native Linux game build.

## Question and falsifiable check

The immediate Milestone 5 question was whether a maintained dependency already present at the
platform boundary can create a native Vulkan device and exercise the minimum resource/presentation
lifecycle needed by a future renderer adapter. The cheapest useful check was an isolated target
that must:

1. compile original GLSL into generated SPIR-V outside the source tree;
2. create an SDL GPU device using the Vulkan driver and claim an SDL window;
3. create vertex and fragment shaders plus a graphics pipeline;
4. accept the fixture through native `IRenderPrimitive`, validate both its seven-vertex/nine-U16
   representation and the nine-vertex direct `PrimitiveBuilder` expansion, upload the builder
   output plus a 2-by-2 RGBA texture, then sample it in the fragment shader;
5. map the shared depth and alpha-blend snapshot into the graphics pipeline, acquire real swapchain
   images, draw and present twelve frames, request a window resize, and recreate the size-dependent
   depth target; and
6. render the same pipeline into a bounded 64-by-64 offscreen target, wait on a GPU fence, download
   the pixels, validate clear/background/foreground structure, and release resources in dependency
   order.

`frontierx_sdl3_gpu_probe` performs that check. The GPU lifecycle now lives in the reusable
`SdlGpuRenderer` platform owner, and the native startup shell can explicitly select it; both paths
submit the same repository-owned scene through a native implementation of the existing
`IRenderPipeline` vtable and the registered `IRenderPrimitive` boundary. They remain separate from
`Direct3D_pipe.cpp`, game objects, simulation state and retail files. The checked-in shader sources
are independent test inputs. Their generated SPIR-V files remain build outputs and are not stored
in the repository.

The production `PrimitiveBuilder` pipeline constructor expands a repository-owned triangle plus
quad into nine direct legacy `RPVertex` values. Those values cross `IRenderPipeline`, the native
`IRenderPrimitive`, and the SDL-free checked/copying 24-byte position/BGRA8/UV contract before the
probe uploads them. The owner also validates the equivalent seven-vertex/nine-U16-index submission
before the direct builder result is rendered. Layout, topology and bounds are tested by the existing
focused target; see
[RENDER_SUBMISSION_PORTABILITY.md](RENDER_SUBMISSION_PORTABILITY.md).

## Candidate assessment

The source and maintenance review used these upstream records:

- SDL documents its GPU API as a cross-platform device, resource, command-buffer, render-pass and
  presentation interface. `SDL_CreateGPUDevice` can select the Vulkan driver and has been available
  since SDL 3.2.0: [SDL GPU API](https://wiki.libsdl.org/SDL3/CategoryGPU) and
  [SDL_CreateGPUDevice](https://wiki.libsdl.org/SDL3/SDL_CreateGPUDevice).
- SDL's own header recommends offline shader compilation and identifies `SDL_shadercross` for
  multi-backend shader production. The prototype uses the locally installed `glslc` only to produce
  SPIR-V for its explicitly Vulkan-scoped test; it does not add runtime shader compilation.
- Current DXVK describes itself as a Direct3D implementation for Linux through Wine:
  [DXVK README](https://github.com/doitsujin/dxvk/blob/master/README.md).
- The earlier native fork explicitly intended for native D3D9/11 ports was archived by its owner on
  2022-11-05 and is read-only: [dxvk-native archive](https://github.com/misyltoad/dxvk-native).

These facts reject archived `dxvk-native` as a production dependency and make SDL GPU a reasonable
prototype surface. They do **not** prove that a direct SDL GPU adapter is lower-risk than preserving
the complete D3D9-facing renderer through another maintained translation route. A production
decision still needs representative game rendering, D3D9 state/effect mapping, dependency review,
and Windows comparison evidence.

## Build boundary

The prototype has its own `FRONTIERX_BUILD_SDL3_GPU` option. It defaults to `OFF`, requires
`FRONTIERX_BUILD_SDL3`, and is enabled by the two checked-in Linux presets. When enabled, configure
requires `glslc`; CMake generates the two SPIR-V files in `out/build/<preset>/generated/shaders/`.
No Vulkan SDK headers or direct Vulkan loader calls are used by FrontierX code.

The locally measured dependencies were:

| Component | Measured version | Role |
|---|---:|---|
| SDL3 | 3.4.16 | Runtime GPU abstraction, Vulkan device, window swapchain and presentation |
| shaderc `glslc` | 2026.3 | Build-time-only GLSL-to-SPIR-V compiler |
| Vulkan loader metadata | 1.4.357 | Host/runtime evidence only; not directly linked by the probe |

SDL3 remains a system-package dependency whose FrontierX redistribution review is open. `glslc` is
not linked or copied into FrontierX outputs. Its package provenance and any reproducible-build or
redistribution implications are still recorded as release gates rather than inferred from local
availability.

## Depth and blend pipeline mapping

The native `IRenderPrimitive` owner retains an SDL-free `RenderStateSnapshot` and sends a complete
candidate to `SdlGpuRenderer` after each supported legacy state change. The backend maps all eight
`D3DCMPFUNC` values to their SDL compare operations and maps `ZERO`, `ONE`, source/destination color,
inverse color, source/destination alpha, inverse alpha and source-alpha-saturate to the corresponding
SDL blend factors. The same source and destination factors currently drive color and alpha with
`ADD`; separate alpha factors and blend operations are outside this slice.

SDL GPU state is part of an immutable graphics pipeline. `SetRenderState` therefore validates the
whole snapshot, creates a candidate pipeline, waits for the current device to become idle, releases
the previous pipeline and only then stores the new snapshot. If validation, pipeline creation or the
idle wait fails, the candidate is released and the active pipeline remains installed. The native
owner commits its matching snapshot only when the callback returns `GR_OK`, so legacy get/set state
and the backend pipeline cannot diverge after a reported failure.

The smoke state enables depth test/write with `LESSEQUAL` and enables
`SRCALPHA`/`INVSRCALPHA`. Fixture alpha is 210 for the foreground triangle and 160 for the background
quad. The vertex shader converts the packed BGRA input to RGBA, and the fragment shader multiplies
the sampled texture by all four diffuse channels. This makes the visible/readback result sensitive
to the newly connected alpha path while retaining the earlier depth-overlap check.

## Verification evidence

The following configure and focused compile commands succeeded on Linux x86_64:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_sdl3_gpu_probe -j2

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_sdl3_gpu_probe -j2
```

Both generated the shaders and compiled the probe with the project's strict warning policy. Separate
visible runtime checks were executed on 2026-09-21 against the local graphical session with
`--require-runtime --visible`. Both binaries reported:

```text
SDL3 GPU probe passed: driver=vulkan presented_frames=12 vertices=9 indices=0
source=primitivebuilder-native-irenderpipeline texture=2x2 resize=2560x1440->2536x1390
depth_recreations=2 changed_pixels=2496 pixel_checksum=7951318278861886233
```

The visible commands used each preset's generated shaders:

```text
out/build/linux-gcc-debug/frontierx_sdl3_gpu_probe \
  out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv \
  out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv \
  --require-runtime --visible
out/build/linux-clang-debug/frontierx_sdl3_gpu_probe \
  out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv \
  out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv \
  --require-runtime --visible
```

The integrated shell was then executed visibly with each preset build, an isolated synthetic data
root, a 250-millisecond deadline and the same generated shaders:

```text
frontierx_native_shell --smoke-ms 250 \
  --render-smoke generated/shaders/sdl3_gpu_probe.vert.spv \
                 generated/shaders/sdl3_gpu_probe.frag.spv
```

Both 2026-09-21 shell runs reported the Wayland video driver, Vulkan backend, production
`PrimitiveBuilder via IRenderPipeline`, 17 presented frames, nine direct vertices, 2,496 changed
pixels and checksum `7951318278861886233`. The exact frame count and compositor-selected resize are
diagnostic, not cross-host invariants. The earlier focused Clang ASan/UBSan shell run used the opaque
fixture before this state slice; current sanitizer evidence covers the CPU state and pipeline owner.

On 2026-09-23 the same shell option was routed through the supported App-side production
`RenderBatcher` direct branch before `IRenderPipeline`. Visible GCC 16.2.1, Clang 22.1.8 and focused
Clang ASan/UBSan Wayland/Vulkan shell runs each reported the same nine vertices, 2,496 changed
pixels and local checksum; see [RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md) for
the exact integrated commands and ownership limits. The texture remains supplied directly to the
SDL renderer. A later stage-zero combine-state path is recorded below; this older run did not
exercise it or production material-resource binding.

### Extended RPVERTEX2 GPU consumption (2026-09-23)

The production-shaped native `IRenderPrimitive` already validates and copies the 32-byte
`D3DFVF_RPVERTEX2` layout, but the SDL owner previously supplied only the 24-byte callback. The
renderer now accepts an optional second vertex shader, builds a separate four-attribute,
32-byte-stride graphics pipeline, and exposes the existing typed extended submission callback.
Its transactional depth/blend rebuild creates both pipelines before replacing either active
pipeline. The indexed transfer still uses U16 indices; unsupported topology and absent extended
shader support continue to fail explicitly. The new repository shader samples the second UV pair
through the existing 2-by-2 fixture texture. This tests GPU consumption of those bytes, not a
second texture stage or production material mapping.

The existing `frontierx_sdl3_gpu_probe` now submits the same seven-position/nine-index geometry
through `IRenderPipeline -> IRenderPrimitive -> SdlGpuRenderer` twice. First-UV values, positions,
diffuse color, indices, texture and render state stay fixed; only the second UV pair changes from
`(0,0)` to `(1,1)`. Each variant passes the bounded 64-by-64 structural readback, and their
checksums differ: `4297535189445205777` versus `688991715298737102`. The probe then restores
the original 24-byte production `PrimitiveBuilder` scene; its checksum remains
`7951318278861886233` with 2,496 changed pixels. This would fail if the extended path ignored
the second UV pair or interpreted the 32-byte stride as 24 bytes.

Commands executed on Linux x86_64 with GCC 16.2.1, Clang 22.1.8 and a focused Clang ASan/UBSan
build were:

```text
cmake --build --preset linux-gcc-debug --target frontierx_sdl3_gpu_probe -j2
cmake --build --preset linux-clang-debug --target frontierx_sdl3_gpu_probe -j2
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_sdl3_gpu_probe -j2
cmake --preset linux-gcc-debug -B /tmp/frontierx-rpvertex2-clean
cmake --build /tmp/frontierx-rpvertex2-clean --target frontierx_sdl3_gpu_probe -j2
./frontierx_sdl3_gpu_probe generated/shaders/sdl3_gpu_probe.vert.spv \
  generated/shaders/sdl3_gpu_probe.frag.spv \
  generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
```

The last command ran once from each preset build directory; the sanitizer run used the same
generated shader names in `/tmp/frontierx-render-batcher-asan` with
`ASAN_OPTIONS=detect_leaks=0`. All three visible Wayland/Vulkan runs passed twelve presentations,
resize-dependent depth-target recreation, both extended UV checks and the restored ordinary
scene; no sanitizer finding was reported. LeakSanitizer was disabled because host ptrace prevents
reliable execution. The clean GCC directory configured, regenerated all three shaders and built
the probe; it was not used for a separate runtime claim. Both complete Linux debug presets built
and executed 36 CTests successfully; their separate GPU CTest skipped inside the command sandbox
and supplies no GPU runtime evidence. Native Windows Debug/Final and ABI checks, a matching Windows capture,
multi-texture stage/material mapping, long-running resource churn, lawful retail data and
native game execution remain open.

The central matrix result is recorded in [BUILD_GRAPH.md](BUILD_GRAPH.md); its sandbox GPU check
skipped because no video device was exposed, so it is not counted as runtime evidence. The separate
2026-09-23 visible Clang ASan/UBSan probe covers the current extended and ordinary image paths,
with LeakSanitizer disabled because it cannot run reliably in the local graphics-driver/`ptrace`
environment. The shaders were recompiled from checked-in GLSL for both presets.

The native shell's later 2026-09-23 run also presented the seven-vertex/nine-U16 indexed form of
the repository smoke fixture through the production App-side `RenderBatcher` and existing
pipeline/primitive boundary. Visible GCC, Clang and focused Clang ASan/UBSan Wayland/Vulkan
executions each required matching direct and indexed bounded readbacks in the same process and
reported 17 presented frames, seven vertices, 2,496 changed pixels and local checksum
`7951318278861886233`, matching the earlier direct fixture on this driver. This is
synthetic GPU evidence for indexed batcher forwarding; the shell's final image comes from the
indexed draw. See [RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md) for exact commands,
fixture construction and remaining ownership gates.

### Transactional scene upload and repeated lifecycle (2026-09-23)

Source review of `SdlGpuRenderer::SubmitBytes` found that a new submission discarded the previous
vertex/index buffers, upload buffer and texture before checking its combined byte count and before
new resource creation or GPU submission succeeded. A failed later scene could therefore remove the
last valid image. The owner now checks the total upload size first, creates candidate GPU resources
under a local release owner, uploads into those candidates and replaces the active handles only
after `SDL_SubmitGPUCommandBuffer` succeeds. Failed candidate creation, mapping, command acquisition
or submission leaves the previous handles and draw counts intact by source review. The already
required idle wait occurs before candidate creation so the old resources are no longer in flight
when a successful replacement releases them. No renderer interface, retail data or Windows D3D9
code changed.

The existing GPU probe now sends a valid triangle-list view with three U16 indices and a declared
vertex count near the 32-bit upload-byte limit after its ordinary scene is installed. The view
does not dereference that many vertices: the combined-size check rejects it with
`GR_INVALID_PARMS` before copying or allocating. The probe then verifies the prior nine-vertex
draw counts, a structurally valid bounded readback and an unchanged checksum/pixel count after
presentation. This check directly covers the former size-failure regression. Candidate SDL
allocation or command failure was not injected; preservation for those paths is source-review
evidence only.

The same probe already tears down and initializes `SdlGpuRenderer` a second time on the same SDL
window. It repeats the extended-UV submissions, presents again and requires matching bounded
readbacks. Visible GCC 16.2.1, Clang 22.1.8 and focused Clang ASan/UBSan runs on Linux x86_64
passed with the repository-owned synthetic geometry/texture and build-generated shaders. Each
reported `scene_runs=2`, `repeated_presented=1`, twelve first-run presentations, 2,496 changed
pixels, checksum `7951318278861886233`, and distinct extended UV checksums
`4297535189445205777` and `688991715298737102`. The local resize changed the depth target from
1920x1080 to 1896x1030. LeakSanitizer was disabled for the focused run because this host's ptrace
environment prevents reliable execution. The visible commands, run from each build directory,
were:

```text
cmake --build --preset linux-gcc-debug --target frontierx_sdl3_gpu_probe -j2
cmake --build --preset linux-clang-debug --target frontierx_sdl3_gpu_probe -j2
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_sdl3_gpu_probe -j2
./frontierx_sdl3_gpu_probe generated/shaders/sdl3_gpu_probe.vert.spv generated/shaders/sdl3_gpu_probe.frag.spv generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
ASAN_OPTIONS=detect_leaks=0 ./frontierx_sdl3_gpu_probe generated/shaders/sdl3_gpu_probe.vert.spv generated/shaders/sdl3_gpu_probe.frag.spv generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
```

The ordinary command was executed from both preset build directories; the ASan/UBSan command was
executed from `/tmp/frontierx-render-batcher-asan`. These are synthetic visible GPU runtime
results on one local Vulkan driver, not a Windows or retail comparison. Long-running churn,
device loss, native Windows Debug/Final and ABI checks, game material mapping, and a game scene
remain open.

After the resource change, the integrated GCC native shell was also run visibly with its explicit
synthetic data root and `--render-smoke` shaders. Its two consecutive production-batcher
submissions still produced matching direct/indexed readbacks, 17 presentations, seven final
vertices, 2,496 changed pixels and checksum `7951318278861886233`. This is an integration
regression check for the new replacement order; the focused GPU probe above supplies the
rejected-upload and second-initialization evidence.

This is runtime evidence of device creation, shader/pipeline creation and replacement, vertex and
texture upload, texture/sampler binding, depth comparison/write and alpha blending, command
submission, swapchain image
acquisition, draw, presentation, compositor-observed resizing, depth-target recreation and ordered
shutdown through SDL GPU. The bounded readback found 2,496 changed pixels out of 4,096 and separately
required the clear corner, textured background geometry and depth-preserved foreground to differ.
GCC and Clang produced the same diagnostic FNV-1a value on the same local Vulkan driver. That full
checksum is reported for comparison but is not an acceptance constant across different hardware or
drivers. The compositor chose the observed final size; the test asserts a real swapchain size
transition rather than assuming its requested logical dimensions are authoritative.
The registered CTest opens a transient non-focusable window and returns skip code 77 only when the
host has no usable video/GPU runtime; once a device and swapchain are available, shader, pipeline,
upload, draw or submit failures are test failures. A required local validation can use
`--require-runtime` to convert environment absence into failure.

## What this does not prove

- The geometry and 2-by-2 texture are synthetic and contain no external retail mesh, material,
  effect or UI.
- The structural pixel readback is not a Windows image comparison and does not set cross-driver
  color/tolerance policy.
- Stencil, separate alpha blend factors/operations, fullscreen behavior, device loss, shader
  variants, and long-running resource churn are untested. Depth, source-alpha blending, sampling and
  resize paths now have API/runtime plus bounded pixel evidence, but no retail or Windows oracle.
- The native `IRenderPipeline`/`IRenderPrimitive` geometry adapters and `PrimitiveBuilder` caller are
  production-boundary code, and the shell selects the reusable SDL GPU lifecycle only for an
  explicit smoke request. The shell now traverses the production App-side `RenderBatcher` direct and indexed
  branch, but there is no complete legacy D3D9 state/material adapter or game target.
- The probe does not replace or modify the supported Win32 D3D9 renderer.
- Neither SDL3 nor the shader toolchain is approved for FrontierX binary distribution by this test.

### Stage-zero texture combine through the production boundary (2026-09-23)

Source review of production `SimpleMaterial` and `DebugMaterial` found stage-zero color and alpha
`MODULATE` with `DIFFUSE` and `TEXTURE` arguments, followed by stage-one disable. The native-only
legacy constants, `TextureStageSnapshot`, registered `IRenderPrimitive` callback and SDL fragment
uniform now carry exactly these six stage-zero fields. Stage zero also accepts `SELECTARG1` and
`SELECTARG2` with `DIFFUSE`, `CURRENT` or `TEXTURE`; `CURRENT` aliases diffuse at stage zero.
The shader evaluates color RGB and alpha separately. The stage-one disable calls are accepted;
other stages, operations, arguments, sampler states and texture-handle binding still return
`GR_NOT_IMPLEMENTED`. The native owner updates its gettable snapshot only after its backend
callback succeeds. The shell configures the smoke state through the production `RenderBatcher`
before direct and indexed fixture submission. This is an executed texture combine state path,
not production material or texture-resource ownership: SDL still receives the repository fixture
texture directly.

The existing GPU probe now reads the direct `PrimitiveBuilder` image in `MODULATE` state, switches
the same live pipeline to `SELECTARG1` diffuse color, checks that its 64-by-64 image checksum
changes, rejects `D3DTOP_DISABLE` for stage zero without changing the gettable operation, restores
`MODULATE`, and checks the original checksum exactly. It then selects texture-only alpha,
observes a separate blended-image checksum change, restores alpha `MODULATE` and recovers the
original checksum again. It repeats these checks on a second renderer
initialization. The framebuffer checks use repository-owned synthetic geometry and 2-by-2 texture.

On Linux x86_64, full `linux-gcc-debug` and `linux-clang-debug` builds passed. Both full CTest
presets passed 36 executed checks; `portability.sdl3_gpu` skipped inside the command sandbox and
is **not** counted as runtime evidence. Explicit visible Wayland/Vulkan GCC 16.2.1, Clang 22.1.8
and focused Clang ASan/UBSan GPU runs each passed with twelve presented frames, two scene runs,
2,496 changed pixels and restored checksum `7951318278861886233`. The visible GCC native shell
run passed with 17 presentations and the same checksum through `RenderBatcher` direct and indexed
draws. Exact commands were:

```text
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_sdl3_gpu_probe -j2
./frontierx_sdl3_gpu_probe generated/shaders/sdl3_gpu_probe.vert.spv generated/shaders/sdl3_gpu_probe.frag.spv generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
ASAN_OPTIONS=detect_leaks=0 ./frontierx_sdl3_gpu_probe generated/shaders/sdl3_gpu_probe.vert.spv generated/shaders/sdl3_gpu_probe.frag.spv generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-batcher-shell-data --smoke-ms 250 --render-smoke out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv
```

The first visible GPU command ran from each preset build directory; the sanitizer command ran
from `/tmp/frontierx-render-batcher-asan`, with LeakSanitizer disabled for this host's ptrace
environment. The shell command ran from the repository root. Native Windows Debug/Final, module
ABI checks, retail runtime, Windows image tolerance comparison, material owner and texture-handle
binding were not performed. The shader's single-texture stage-zero mapping does not establish
multi-stage, effect, lighting or game-scene fidelity.

### Runtime texture handles through the existing boundary (2026-09-23)

The next blocker was that `SimpleMaterial`/`DebugMaterial` pass a `U32` texture ID to the existing
`set_texture_stage_texture` slot while the native primitive rejected that slot and the SDL owner
always sampled its directly supplied fixture texture. `RenderBatcher` also checks nonzero IDs with
`PIPE->is_texture` before forwarding. Source review established that these are renderer runtime
handles, not pointers or serialized texture bytes. The native pipeline now delegates `is_texture`
to an explicit backend lookup, and the registered native primitive forwards stage-zero bindings
through an optional callback. Its getter commits only successful bindings. Stage-one zero remains
an accepted disabled stage; unregistered IDs and other stages fail explicitly.

The SDL owner retains handle 1 for the startup fixture and registers additional RGBA8 textures in
a monotonic `U32` table. Registration validates exact dimensions/byte count, uploads through an SDL
copy pass, waits for completion and publishes the handle only after success. The current handle
selects the sampled GPU texture in both presentation and bounded readback. Destruction rejects the
currently bound handle, waits for idle before releasing another handle and makes that ID stale;
shutdown releases all remaining GPU textures. No native pointer is converted into a `U32` ID.

The existing GPU probe registers a second repository-owned 2-by-2 texture. It binds that ID
through `IRenderPipeline -> IRenderPrimitive`, reads a different image with the same geometry
footprint, rejects an unknown ID without changing the getter, refuses to destroy a bound texture,
restores handle 1 and the original checksum, then destroys the extra texture and rejects its stale
ID. Both scene lifecycles execute this sequence. The native shell also binds the extra ID through
the real production `RenderBatcher`, checks a changed readback, restores handle 1 with exact
checksum recovery, destroys the extra texture, then submits its indexed scene. This gives
synthetic runtime evidence of the **existing binding path**, while the startup fixture and extra
texture still originate from repository arrays. The later native
`IRenderPipeline::create_texture` and `set_texture_level_data` path is described below;
The subsequent one-level `lock_texture`/`unlock_texture` path is described below;
the production `TextureManager` and `ITextureLibrary` remain open.

On Linux x86_64, full GCC 16.2.1 and Clang 22.1.8 debug builds and 36 executed CTests per preset
passed. The GPU CTest skipped inside the command sandbox and is not counted as execution. Explicit
visible Wayland/Vulkan GCC and Clang GPU probes and native shells passed; the probe reported twelve
presentations, two scene runs, 2,496 changed pixels after rebinding and checksum
`7951318278861886233`; the second registered texture produced checksum
`2956733785601487666` on the same host and matched across both scene runs and compilers. The
shell reported 17 presentations and the same final checksum. Focused
Clang ASan/UBSan visible GPU probe and shell also passed with LeakSanitizer disabled for this host's
ptrace environment. The exact commands, from each named build directory for the probe and from
the repository root for the shell, were:

```text
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_sdl3_gpu_probe frontierx_native_shell -j2
./frontierx_sdl3_gpu_probe generated/shaders/sdl3_gpu_probe.vert.spv generated/shaders/sdl3_gpu_probe.frag.spv generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
ASAN_OPTIONS=detect_leaks=0 ./frontierx_sdl3_gpu_probe generated/shaders/sdl3_gpu_probe.vert.spv generated/shaders/sdl3_gpu_probe.frag.spv generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-batcher-shell-data --smoke-ms 250 --render-smoke out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv
```

The probe command was run from both preset build directories; the sanitizer variant and shell
were also run from the sanitizer build directory and repository root respectively. Clang shell
used the corresponding `out/build/linux-clang-debug` paths. These were synthetic Linux runtime
checks, not native Windows Debug/Final, cross-module ABI, retail runtime or Windows image
comparison. Multi-stage materials, mipmaps, material-owned texture loading and long-running churn
remain open.

### Native texture handle collision regression (2026-09-24)

The native pipeline now accepts only one owner for each backend `U32` texture handle.
Source review of `NativeRenderPipeline::create_texture` found that a second backend create
returning an already owned handle was previously reported as success even though map insertion
did not replace the first texture's dimensions. A later destroy could then target a resource
different from the metadata used to validate uploads. The pipeline now returns `GR_GENERIC`
for that collision, leaves the caller's output handle at zero and retains the first record.
It deliberately does not destroy the colliding handle, because that would invalidate the first
resource if the backend reused the same ID. The backend still must supply unique live IDs.

The existing `frontierx_render_pipeline_probe` now makes a synthetic backend return handle 41
twice with different dimensions. It observes success for the first creation, failure and zero
output for the second, the original 2-by-2 metadata and one successful final destruction.
The production owner and real callers are `NativeRenderPipeline::create_texture` and the
native shell/GPU probe texture upload paths; this regression could not be covered by their
normal monotonic SDL handles. Exact Linux x86_64 checks executed on 2026-09-24:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_pipeline_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_pipeline$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_pipeline_probe -j2
ctest --preset linux-clang-debug -R '^portability.render_pipeline$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --preset linux-clang-debug -B /tmp/frontierx-texture-handle-asan -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_render_pipeline_probe -j2
ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_render_pipeline_probe
```

GCC 16.2.1 and Clang 22.1.8 focused probes passed. The focused Clang ASan/UBSan
process printed `render pipeline probe passed` without a sanitizer finding; LeakSanitizer
was disabled because of this host's ptrace environment. Both full debug presets built and
executed 36 of 37 CTests; `portability.sdl3_gpu` skipped without a usable GPU in the
command environment. This is synthetic CPU runtime evidence. No GPU run, native Windows
Debug/Final rebuild, ABI/layout comparison, retail runtime or Windows image comparison was
performed for this change.

### One-level native texture locking (2026-09-24)

`IRenderPipeline::lock_texture` is a real production boundary. Source review found
32-bit texel edits in `SysMap::addPlayerIcon`, while `MeshRender::PremultiplyTexture` and
`SpaceShip::FreakTexture` also lock textures but require 16-bit formats or mip levels that
the current native backend cannot represent. The next falsifiable question was whether
the existing RGBA8 `create_texture` and upload owner could expose a stable writable
`RPLOCKDATA` level-zero view without publishing incomplete GPU data or invalidating an
active lock. The existing pipeline probe covers CPU ownership and upload rollback; its
GPU probe can exercise the same path through the SDL owner, so no new target was added.

Each pipeline-created RGBA8 texture now owns a CPU shadow matching its one supported GPU
level. A successful `set_texture_level_data` replaces that shadow only after the backend
accepts the upload. `lock_texture` returns the shadow pointer, 32-bit pixel format,
dimensions and packed row pitch. A second lock, level other than zero, null output,
replacement upload and destruction while locked are rejected. `unlock_texture` submits
the shadow through the existing backend upload callback and clears the lock only after
success. If upload fails, the backend retains its prior image and the caller can retry
the same unlock. This changes no disk schema, Win32 implementation, public vtable or
cross-module structure; the new shadow is private to the native pipeline.

The existing `frontierx_render_pipeline_probe` now creates a synthetic 2-by-2 texture,
uploads known bytes, rejects a failed full replacement while preserving the shadow,
locks and edits one texel, forces another upload failure, checks the previous committed
backend bytes and retained lock, retries successfully, relocks to observe the edit,
and destroys the resource. The existing SDL GPU probe now fills its
second repository texture through `lock_texture`/`unlock_texture` before its established
bounded image-change, rebind and stale-handle checks. The native shell continues to
exercise `set_texture_level_data`, preserving coverage of both upload entry points.

Exact commands executed on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_pipeline_probe frontierx_sdl3_gpu_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_pipeline$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_pipeline_probe frontierx_sdl3_gpu_probe -j2
ctest --preset linux-clang-debug -R '^portability.render_pipeline$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_render_pipeline_probe -j2
ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_render_pipeline_probe
```

The focused CPU probe and Clang ASan/UBSan execution passed. Both full debug presets
built and executed 36 of 37 CTests; `portability.sdl3_gpu` skipped in the command
environment. A later visible-host execution of the same GPU probe ran the lock path
on Wayland/Vulkan under both compilers and focused Clang ASan/UBSan. The exact commands
from the repository root were:

```text
out/build/linux-gcc-debug/frontierx_sdl3_gpu_probe out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
out/build/linux-clang-debug/frontierx_sdl3_gpu_probe out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_sdl3_gpu_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_sdl3_gpu_probe /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
```

Each returned success with 12 presentations, two scene runs, 2,496 changed pixels,
baseline checksum `7951318278861886233` and alternate-texture checksum
`2956733785601487666`. The alternate texture was filled through the new lock/unlock
path, and rebinding restored the baseline image. The first visible sanitizer run omitted
`ASAN_OPTIONS=detect_leaks=0`; it printed the same successful GPU diagnostics but exited
with LeakSanitizer findings in the system D-Bus/SDL initialization path. The rerun with
LeakSanitizer disabled returned zero and showed no ASan or UBSan finding. Leak freedom of
system D-Bus is therefore not claimed.

This is repository-fixture synthetic Linux GPU runtime evidence, not game rendering.
No native Windows Debug/Final rebuild, ABI/layout check, retail runtime or Windows image
comparison was performed. Multi-level mipmaps, 16-bit and
palette formats, flags other than zero, readback of independently changed GPU textures,
material loading and game execution remain open. At the time of this lock check,
the real `SysMap` caller also used `blit_texture`, which was unsupported natively;
the bounded copy below addresses its equal-size full-texture case.

### Bounded native RGBA8 texture copy (2026-09-24)

`SysMap::addPlayerIcon` creates a texture for each player and calls the existing
`IRenderPipeline::blit_texture` slot to copy the complete source icon. `SysMap` and
`Sector` also use this slot for map textures, including size-changing copies. Source
review of the Windows D3D9 owner found a `StretchBlt` path, while the native adapter
previously returned `GR_NOT_IMPLEMENTED` for every call. The local hypothesis was
that the current one-level RGBA8 pipeline owner could copy equal-size complete
textures through its already checked upload callback while preserving both the CPU
shadow and the committed backend image after a failed upload. The existing pipeline
probe is the narrowest executable check; no separate probe was added.

`NativeRenderPipeline::blit_texture` now accepts level-zero, full-rectangle copies
between equally sized pipeline-owned RGBA8 textures. It rejects unknown or locked
handles and nonzero levels; partial rectangles and scaling return
`GR_NOT_IMPLEMENTED`. It copies the source shadow before upload and swaps that copy
into the destination shadow only after the backend accepts it. A failed upload thus
leaves the destination shadow and backend unchanged. Same-handle full copies succeed
without an upload. This adds no serialized fields, public vtable slots or Windows
implementation changes. The source-reviewed real icon caller has the supported
rectangle shape, but it has not executed in the Linux game.

Exact Linux x86_64 commands executed with GCC 16.2.1 and Clang 22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_pipeline_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_pipeline$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_pipeline_probe -j2
ctest --preset linux-clang-debug -R '^portability.render_pipeline$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_render_pipeline_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_render_pipeline_probe
```

The existing CPU probe created two repository-fixture 2-by-2 textures, rejected a
forced upload failure without changing committed destination bytes, copied all 16
source bytes after retry, read the destination shadow through `lock_texture`, and
rejected a partial rectangle and a copy into a locked destination. Both focused
CTests and the Clang ASan/UBSan process passed. LeakSanitizer was disabled for this
host's ptrace environment. Both full debug presets built and ran 36 passing CTests;
the SDL GPU CTest skipped in the sandbox. The visible GPU extension below adds
separate hardware evidence. No native Windows Debug/Final rebuild, ABI/layout
check, retail runtime or Windows image comparison was performed. Map texture
scaling, partial rectangles, other formats and the `get_texture_dc` drawing
path remain open.

### Visible backend copy and material integration inventory (2026-09-24)

The existing SDL GPU probe now creates a second pipeline-owned texture, calls
`IRenderPipeline::blit_texture` to copy the lock-uploaded alternative texture,
binds the copy through the registered native primitive, and compares bounded
readback against the source. It rebinds the source, destroys the copy, checks
stale handles and repeats the full scene lifecycle. This checks the production
pipeline slot against the actual SDL/Vulkan backend; the source and destination
pixels remain repository-owned synthetic data. The added GPU check tests a
successful full copy, while the CPU check above tests upload failure and
shadow rollback.

The exact commands executed from the repository root were:

```text
cmake --build --preset linux-gcc-debug --target frontierx_sdl3_gpu_probe -j2
out/build/linux-gcc-debug/frontierx_sdl3_gpu_probe out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
cmake --build --preset linux-clang-debug --target frontierx_sdl3_gpu_probe -j2
out/build/linux-clang-debug/frontierx_sdl3_gpu_probe out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_sdl3_gpu_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_sdl3_gpu_probe /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
```

These three visible Wayland/Vulkan runs used direct display access because SDL
could not reliably initialize a video device inside the command sandbox. GCC
16.2.1, Clang 22.1.8 and the Clang ASan/UBSan build returned success. Each
reported 12 presented frames, two scene runs, 2,496 changed pixels, baseline
checksum `7951318278861886233`, and equal source/copy checksums
`2956733785601487666`. The sanitizer run reported no ASan or UBSan finding;
LeakSanitizer was disabled for this host's ptrace environment. A first visible
attempt reached Vulkan and obtained matching checksums but failed an
overstrict structure check that was not part of the existing alternate-texture
contract. The corrected check compares changed-pixel count and checksum; the
three final runs passed. This is synthetic Linux GPU runtime evidence, not a
production game draw or a Windows comparison.

For the next material step, direct Clang C++17 syntax checks of
`SimpleMaterial.cpp`, `DebugMaterial.cpp` and `MaterialLibrary.cpp` with the
native App/Libs include paths failed before execution. The production
commands were:

```text
clang++ -std=c++17 -fsyntax-only -ICode/Libs/Include -ICode/App/Src/Include -ICode/App/Src -ICode/App/DInclude Code/Libs/Src/Shading/Materials/SimpleMaterial.cpp
clang++ -std=c++17 -fsyntax-only -ICode/Libs/Include -ICode/App/Src/Include -ICode/App/Src -ICode/App/DInclude Code/Libs/Src/Shading/Materials/DebugMaterial.cpp
clang++ -std=c++17 -fsyntax-only -ICode/Libs/Include -ICode/App/Src/Include -ICode/App/Src -ICode/App/DInclude Code/Libs/Src/Shading/Materials/MaterialLibrary.cpp
```

The production
`SimpleMaterial` currently depends on `MAX_PATH`, `D3DMATERIAL7`, and
`D3DRS_LIGHTING`/`D3DRS_SPECULARENABLE`; the native D3D declaration layer does
not supply those. `DebugMaterial` reaches the Win32-only `RPUL_Misc.h` include
through `RPUL.h`. `MaterialLibrary` additionally requires `CRITICAL_SECTION`,
`WIN32_FIND_DATA` and Win32 file attributes. Clang also diagnosed DACOM
base-subobject offset instantiation while these material classes were
incomplete. These are compile-derived blockers, not claims that material
loading works. Their interfaces and Windows ownership require a focused
portability design before the production material library can drive a game
scene.

The 2026-09-26 production `MaterialLibrary` and `SimpleMaterial` CPU integration
resolved the listed `MaterialLibrary` and `SimpleMaterial` compile blockers for
the focused Linux target. `SimpleMaterial` now reads synthetic diffuse and
opacity fields through the native UTF adapter. Its `apply` still receives
`GR_NOT_IMPLEMENTED` from native `set_material`; production texture loading,
material-driven GPU output and the native game scene remain open. See
[STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md).

### Production StateMaterial visible submission (2026-09-24)

The same SDL GPU probe now registers the production `StateMaterial` and
`VertexBufferManager` factories through DACOM. A synthetic service container
supplies the existing native `IRenderPipeline` and `IRenderPrimitive`; the
material applies depth, stage-zero combine and a runtime texture handle, then
its cached U16-indexed `render` call submits repository-owned RPVertex geometry
to the SDL/Vulkan backend. The 64x64 bounded readback reports 2,496 changed
pixels and material checksum `8289071695263040328`. The material also binds
the alternate and copied textures for the existing copy check; both readbacks
have checksum `2956733785601487666`. The direct `PrimitiveBuilder` draw has a
different vertex/color encoding, so its checksum is a separate scene check,
not a Windows or material image comparison.

The three visible commands listed above were rebuilt and rerun with GCC
16.2.1, Clang 22.1.8 and focused Clang ASan/UBSan. Each passed with the same
material and texture checksums, 12 presented frames and two scene runs. No
ASan/UBSan issue was reported; LeakSanitizer remained disabled for this host's
ptrace environment. This is synthetic Linux GPU runtime evidence through a
production material owner. The production `MaterialLibrary` was not linked,
and there was no native game, Windows Debug/Final, ABI, retail or Windows image
comparison run. The implementation and CPU ownership check are documented in
[STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md).

### Repository-fixture image export preparation (2026-09-25)

The existing `SdlGpuRenderer::Readback64` already downloads a 64-by-64 color target and checks
its geometry, but previously retained only a pixel checksum and count. The native shell now accepts
`--capture-ppm FILE` alongside `--render-smoke` and writes the final repository-fixture readback as
a binary P6 image after successful presentation and checksum validation. The SDL owner converts
the two supported RGBA8/BGRA8 swapchain channel orders to RGB; an unrecognized format rejects the
requested export instead of silently swapping colors. The raw four-channel checksum used by the
existing regression remains unchanged. This result is a bounded synthetic fixture image, not a
game frame or a Windows comparison. The 64-by-64 image excludes alpha because blending is already
resolved into the color target; the full in-memory checksum still includes the original fourth
channel.

The falsifiable blocker was that checksum-only Linux evidence cannot be spatially aligned with a
Windows D3D9 capture. This extends the existing production-boundary shell and GPU readback rather
than adding an isolated renderer probe. `SdlGpuReadback` is a native backend value, not a disk,
network or campaign ABI record. The P6 file is diagnostic output and must stay outside the source
tree. The intended command is:

```text
env XDG_CONFIG_HOME=/tmp/frontierx-capture-settings \
  out/build/linux-gcc-debug/frontierx_native_shell \
  --data-root /tmp/frontierx-capture-data --smoke-ms 250 \
  --capture-ppm /tmp/frontierx-capture.ppm \
  --render-smoke out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv \
  out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv
```

Validation commands:

```text
mkdir -p /tmp/frontierx-capture-data/{Objects,Interface,Textures,Mat,Profiles,Maps/Campaign,Maps/Multiplayer} /tmp/frontierx-capture-settings
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j2
ctest --preset linux-gcc-debug -R '^portability.native_shell$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_native_shell -j2
ctest --preset linux-clang-debug -R '^portability.native_shell$' --output-on-failure
env XDG_CONFIG_HOME=/tmp/frontierx-capture-settings \
  out/build/linux-clang-debug/frontierx_native_shell \
  --data-root /tmp/frontierx-capture-data --smoke-ms 250 \
  --capture-ppm /tmp/frontierx-capture-clang.ppm \
  --render-smoke out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv \
  out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv
file /tmp/frontierx-capture.ppm /tmp/frontierx-capture-clang.ppm
cmp /tmp/frontierx-capture.ppm /tmp/frontierx-capture-clang.ppm
```

On this Linux x86_64 host, the affected `frontierx_native_shell` target compiled with GCC and
Clang. `ctest --preset linux-gcc-debug -R '^portability.native_shell$' --output-on-failure` and
the corresponding Clang command both passed one process check. A direct invocation with
`--capture-ppm` but no `--render-smoke` returned the expected usage error. The complete GPU
command first exited before SDL window startup in the restricted command environment. The same
command then ran in the visible Wayland/Vulkan session with GCC and Clang, using separate `/tmp`
output paths. Both executions presented 17 frames, observed 2,496 changed pixels and retained
the existing raw checksum `7951318278861886233`. `file` identified both outputs as 64-by-64
binary Netpbm images; each was 12,301 bytes including its P6 header, and `cmp` found the files
byte-identical. This is visible synthetic Linux GPU runtime and image-export evidence using only
repository geometry and texture. The matching Windows fixture capture, spatial tolerance
comparison, Windows Debug/Final rebuild and retail runtime remain unperformed for this change.

### Same-fixture Win32 D3D9 capture and comparison (2026-09-25)

**Blocker and owner.** The Linux P6 export above had no corresponding Windows image. The
supported Win32 game now has an explicit `/renderfixture` validation path in
`Code/App/Src/Conquest.cpp` and `WindowsRenderFixture.cpp`. It uses the **same repository-owned**
`RenderSmokeScene.cpp` geometry, two-by-two RGBA texture and six depth/blend plus eight texture
combine states. `PrimitiveBuilder` submits its direct triangles through the existing production
`IRenderPipeline` vtable and D3D9 `D3DRenderPipe`, then the helper presents and copies the top-left
64-by-64 window client pixels into P6 RGB. No retail image is part of either fixture. The Windows
runtime directory uses lawful external game data only to start the supported process; its capture
content is repository-owned. The GDI copy captures the visible desktop surface after `Present`, so
an occluding window or display transform would invalidate this method; the captured file was
inspected and has the expected fixture structure. This is **Windows runtime** fixture evidence,
not a retail game frame or native Linux game scene.

The first visible Windows diagnostic returned `GR_GENERIC` from `clear_buffers`. A targeted D3D9
trace measured `Clear` HRESULT `0x8876086C` (`D3DERR_INVALIDCALL`) with color and depth flags,
while the device's cooperative status was OK. Source inspection found that `create_buffers`
declared `D3DRP_F_DEPTH_CREATED` but passed `EnableAutoDepthStencil=FALSE` and
`AutoDepthStencilFormat=D3DFMT_UNKNOWN` to `CreateDeviceEx`; no manual depth surface was created
on that live path. The production renderer now requests its already selected compatible `depthFmt`
as an automatic depth/stencil surface. The following Windows run completed color/depth clear,
scene begin, production draw, end and presentation, each returning `GR_OK`.

The D3D9 `clear_buffers` implementation also used a hard-coded black instead of its documented
`RP_CLEAR_COLOR` pipeline state. It now passes that state to `Clear`, and the fixture sets
`0xFF060911`, the RGB value of the native SDL target's background. Both samplers use **nearest**
filtering: the SDL GPU owner sets `SDL_GPU_FILTER_NEAREST`, and the Windows fixture uses
`D3DTEXF_POINT`. A deliberate Windows linear-filter trial made the image visibly blurrier and
was discarded. The production `draw_primitive` owner now returns `GR_GENERIC` on a failed
`DrawPrimitiveUP` instead of falsely returning `GR_OK`.

Reproduction on a physical Windows 11 x86_64 host with NVIDIA graphics, VS2022 Community and
the June 2010 DirectX SDK, from the supported checkout:

```powershell
& .\Build-Conquest.ps1 -Configuration Final -NoDeploy
& .\Build-Conquest.ps1 -Configuration Debug -NoDeploy
# In the separate, hash-verified Final runtime directory on the visible desktop:
.\Conquest.exe /renderfixture /w
# Writes debug\render_fixture_status.txt, debug\render_fixture_steps.txt,
# and, on success, debug\render_fixture_win32.ppm.
```

The isolated runtime directory contained five Final application outputs, nine corresponding
engine DLLs and 18 matching campaign DLLs copied with SHA-256 verification and backup. Its
`DataPath=DataOverlay;Data` linked repository materials and external Steam media without
copying either into the checkout. The final P6 was 12,301 bytes with the exact
`P6\n64 64\n255\n` header. The step trace reported `0x00000000` for texture creation/upload,
viewport, transforms, state and sampler setup, color/depth clear, scene begin, draw, end and
present; capture returned 1. The process then exited by design. The user's observation of the
brief image in the upper-left corner agrees with the 64-by-64 viewport placement.

Comparison command on Linux x86_64, after obtaining the Windows P6 into `/tmp`:

```text
python3 Tools/portability/compare_render_fixture.py \
  /tmp/frontierx-capture.ppm /tmp/frontierx-win32-render-fixture.ppm
```

The tool requires exact P6 dimensions/length, background `(6,9,17)`, a populated foreground
and color diversity on each input, foreground counts within 10 pixels, direct mean RGB channel
error at most 3, and bidirectional one-pixel-neighborhood maximum channel error at most 8. The
one-pixel allowance covers D3D9 versus Vulkan triangle-edge sample ownership while the direct
mean and structure gates reject a shifted or empty image. It reported **PASS**: Linux 2,496
foreground pixels and 582 distinct RGB values; Windows 2,491 and 597; direct mean 2.7085;
Windows-to-Linux neighborhood maximum 4 and Linux-to-Windows maximum 8. Of 4,096 aligned pixels,
169 differed by more than eight in at least one channel at their exact coordinate; these are
covered by the spatial tolerance. A synthetic all-background Windows P6 correctly returned
`FAIL` (zero foreground pixels, mean error 45.3162). Linux GCC and Clang P6 files were byte
identical, as recorded above. The comparator is an explicit diagnostic, not a retail-parity
test; no retail asset or capture was added to the repository.

Native Win32 Debug/Final full-wrapper and same-build campaign ABI rerun status after the final
renderer edits is recorded in [WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md).
The native scene still belongs to the shell and production rendering boundary. The Linux game
composition, production texture/material library loading, additional pool modes, and playable
single-player runtime remain later work; no production backend choice follows from this fixture.

#### First production render-boundary slice: acceptance ledger

| Current slice gate | Evidence and limit |
|---|---|
| Real production draw caller through existing boundary | `PrimitiveBuilder` and the App-side `RenderBatcher` direct and U16-indexed paths traverse the native `IRenderPipeline` and registered `IRenderPrimitive`. Bounded CPU and visible Vulkan checks reject unsupported topology, FVF, state and handle inputs; queued alpha-depth batching is a separately bounded branch. This is not native gameplay. |
| Inspectable representative SDL3 scene | Repository-owned vertices, two-by-two texture and shared material/depth/blend states present visibly in the native shell; no retail asset was copied. |
| Sampling, depth, alpha, resize, presentation and ownership | Existing visible probe/shell and focused CPU checks exercise those states, resized depth targets, repeated scene initialization, rejected upload rollback and ordered teardown. A skipped CTest is excluded from GPU runtime evidence. |
| GCC, Clang and sanitizers | Both Linux x86_64 presets built and executed the affected CPU checks; separate visible GCC/Clang and focused Clang ASan/UBSan Vulkan runs produced matching bounded readbacks. LeakSanitizer was disabled for the host's ptrace environment as documented above. |
| Same input on supported Windows | Physical Windows 11/NVIDIA Final D3D9 captured the same fixture; the P6 comparator passed the explicit spatial/color tolerance. Native VS2022 Win32 Debug and Final full-wrapper rebuilds and immediate same-build 18-campaign ABI checks passed. The comparison is synthetic, not retail. |
| Focused record | This document records commands, pixel counts, image tolerance, data provenance, host/compiler evidence, first failure and owner fix, plus unperformed game, retail and production-backend gates. |

The **first production render-boundary slice** meets these six focused gates. Broader Milestone 5
remains open because a native game scene, production material/texture-library integration and
remaining D3DX9 facility replacements do not yet execute in the game composition. Retain D3D9
as the supported Windows baseline and do not select a Linux production backend from this result.

## Next acceptance slice

The native `IRenderPipeline::create_texture`, `set_texture_level_data`,
`lock_texture`/`unlock_texture` and bounded `blit_texture` slots now cover one
RGBA8 level with visible GPU evidence. `StateMaterial` now has a production-owner
synthetic CPU draw and a visible material-driven indexed Vulkan draw. The production
`RenderBatcher` now executes one alpha-depth U16-indexed queued pool draw through its
existing `flush` path in visible GCC, Clang and focused sanitizer Wayland/Vulkan
shell runs. The pooled and immediate indexed fixture readbacks agree exactly:
2,496 changed pixels and checksum `7951318278861886233`. See
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md) for commands,
synthetic input provenance and remaining pool branches. The separate
`RPVERTEX2` GPU path now consumes and distinguishes
the second UV pair, but has not been exercised through a production game caller.
The repository-owned fixture now also runs through the supported Windows D3D9 path and passes the
bounded comparison above. This closes the same-input image-oracle gate for the first production
render-boundary slice. It does not close the broader native game scene, full material-library,
D3DX9 replacement or retail-parity work. Keep backend selection open until those owners can
execute in the game composition and the dependency decision is documented.

The separate 2026-09-25 VS2022 Win32 Debug and Final builds now compile and
link the supported D3D9 game/engine path and all campaign modules. Native Windows
`dumpbin` checks also found that the 18 installed original retail campaign DLLs'
153 `Mission` and two `Trim` imports resolve in both fresh configurations. The
retail and restored-build `Mission` export sets still differ, as expected for distinct source
states and toolchains. This is build and bounded retail-binary ABI evidence: the Windows fixture
image and tolerance comparison are recorded above; a retail-parity runtime comparison remains
open. See
[WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md).

Source review of the supported D3D9 owner found that its `IRenderPipeline::lock_buffer`
still returns `GR_GENERIC`, `IDDBackDoor::get_dd_provider` returns `GR_OK` before filling its
output, and `save_screen_capture` has only a commented-out legacy call in
`Direct3D_pipe.cpp`. None is a usable image capture contract. The next Windows runtime check
therefore needs the installed game's actual D3D9 scene and an external screen or device capture;
the current Win32 rebuild alone cannot supply comparison pixels. No retail installation was
present in the VM during this source review. A subsequent installation allowed the interactive
retail startup attempt documented in [WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md):
the VM had no sound device, and, after sound was disabled in a temporary INI, the game reported
that its virtual graphics card was incompatible. The D3D9 scene and image comparison remain
unexecuted.

At this earlier physical Windows 11 check, the restored Final executable loaded the freshly built
`D3DRenderPipe.dll` and system `d3d9.dll` and visibly presented the main menu and Quick Battle
setup. That confirms Windows D3D9 startup and UI rendering with lawfully installed external data.
Starting a battle exited before a gameplay scene was confirmed in that check. The repository
fixture had not yet been submitted to D3D9; the later same-fixture capture and comparison are
recorded below and in [WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md).

The shared repository fixture source was checked against the supported Windows compiler before
integration. On the physical Windows 11 host, the VS2022 x86 developer environment compiled
`Code/Libs/Src/Platform/RenderSmokeScene.cpp` with `/std:c++17 /EHsc /c`, the repository
`Code/Libs/Include` directory and the June 2010 DirectX SDK Include directory. `cl.exe` exited
zero and wrote a 19,193-byte object in the user's temporary directory. Its Windows checkout copy
matches the Linux worktree source after normalizing CRLF line endings. The first attempt without
the SDK Include directory failed at `d3dx9.h`, confirming that this remains a real D3DX9 build
dependency. This is **Windows compilation** of the exact source used by the Linux fixture, not
linkage into `Conquest`, D3D9 draw execution, an image capture or a tolerance comparison. A
Windows caller must still submit the fixture through the existing production render boundary.
The equivalent command after `VsDevCmd.bat -arch=x86 -host_arch=x64` is:

```cmd
cl /nologo /std:c++17 /EHsc /c /ICode\Libs\Include /I"%DXSDK_DIR%Include" /Fo:"%TEMP%\frontierx-render-fixture-win32.obj" Code\Libs\Src\Platform\RenderSmokeScene.cpp
```

### Production UTF texture boundary, 2026-09-26

The linked `frontierx_state_material_probe` now uses production
`TextureLibrary::load_library` on a generated 2-by-2 RGB565 UTF texture.
The native pipeline converts its four pixels to RGBA8 through the established
texture callback, and `SimpleMaterial::verify`/`update` resolve that native
frame. The same synthetic CPU check covers texture teardown, rejected-upload
rollback and a missing-`MIP0` archive. GCC and Clang full debug presets passed
their non-GPU CTests while the GPU CTest skipped; focused Clang
ASan/UBSan execution passed. The existing SDL/Vulkan probe now loads the
same generated UTF texture through production `TextureLibrary`, resolves the
frame through a `SimpleMaterial` loaded by production `MaterialLibrary`, then
binds it with `StateMaterial` and observes a changed 64-by-64 readback. Visible
GCC, Clang and focused ASan/UBSan runs each reported UTF image checksum
`9085705336011966884`, the same 2,496-pixel footprint and two scene runs.
`SimpleMaterial::apply`, a game scene and this path on supported Windows
remain unexecuted. Exact input, commands and remaining format gates are in
[TEXTURE_LIBRARY_PORTABILITY.md](TEXTURE_LIBRARY_PORTABILITY.md).

### Model/view and projection shader contract, 2026-09-26

The native `IRenderPrimitive` now accepts optional model/view and projection
callbacks. The SDL renderer registers both in the native shell and isolated
GPU probe, retains finite row-major game matrices, and pushes a combined
column-major vertex uniform. Both checked-in vertex shaders multiply input
positions by it; identity remains the initial state for existing smoke
geometry. The GPU probe adds a changed image centroid, projection checksum
and restored checksum check. GCC/Clang full debug builds compiled both SPIR-V
sources and linked the shell and GPU probe. In this command sandbox the SDL
GPU CTest skipped before video/device initialization, so this change has no
new Vulkan transform or bounded image evidence. The separate synthetic
Engine/RenderMgr/PolyMesh CPU caller reaches a model/view callback; its exact
data and remaining game-format gate are in [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).

The same production `PolyMesh` caller now reaches a rejecting native CPU
callback with its indexed 36-byte `MTVERTEX` format (diffuse, specular and
two UV pairs). That callback failure now stops subsequent `PolyMesh` passes
and face groups. The SDL GPU renderer now compiles a third pipeline for the
same format, with a separate vertex shader, stage-zero UV selection and
optional specular RGB in the fragment shader. The GPU probe registers the
new submission callback and contains bounded image assertions for game-format
geometry, UV-set switching and specular color. GCC and Clang full Linux
debug builds compiled all four SPIR-V shaders and linked the probe; their
CTest runs executed 40 passing checks and skipped only the GPU test before
device creation. The game-format GPU assertions therefore have no execution
or image evidence on this host. The synthetic production `PolyMesh` CPU call
has not been connected to a GPU scene. Native Windows Debug/Final, module
ABI, retail runtime and same-input game image comparison remain open; see
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).

## Transactional stage-zero sampler and visible readback (2026-09-27)

The active App diffuse setup needs linear minification and magnification,
point mip selection, and per-axis wrap or clamp. The native primitive now
validates these values on sampler stage zero and sends a complete candidate
snapshot to the backend. Its cache advances only after callback success. The
SDL GPU owner builds the corresponding sampler with `SDL_CreateGPUSampler`,
waits for GPU idle, and swaps resources only after both operations succeed.
The initial sampler remains point filtered and clamped, preserving the
existing smoke fixture. Other filter/address modes and active later stages
still fail explicitly. The production App helper's CPU call and the
primitive rollback checks are recorded in
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).

The existing GPU probe reads the repository 2-by-2 texture after a switch
from point to linear filtering. It requires a different image checksum,
then switches back and requires the original checksum. The same probe's
previously unexecuted model/view test initially failed because its
`structureValid` check assumes the original center/background pixel layout;
a shifted image legitimately violates that shape. The transform check now
uses RGB readback with changed pixels, horizontal image-moment displacement,
a distinct projection checksum and exact checksum restoration. This retains
the relevant assertions for movement and rollback without applying an
unshifted-image rule to the moved frame.

GCC 16.2.1 and Clang 22.1.8 builds compiled and linked the GPU probe and
shell. In the command sandbox, the CTest skipped at SDL video initialization.
The visible Wayland/Vulkan CTest then passed under GCC and Clang, and a
focused Clang ASan/UBSan build also passed with LeakSanitizer disabled for
this host. Each reported 12 presented frames, two scene runs, 2,496 changed
pixels, final checksum `7951318278861886233`, material checksum
`8289071695263040328`, and the same secondary texture checksums. The
linear/point sampler comparison is asserted in the probe but its intermediate
checksum is not printed. This is synthetic GPU runtime evidence; neither
production App game geometry nor retail media was presented. The explicit
commands are in [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md). Native
Windows Debug/Final, module ABI, retail runtime and same-input game-image
comparison remain open.

## Visible production PolyMesh mesh (2026-09-27)

The linked game-material check now has a visible SDL/Vulkan mode. Its
production `Engine -> RenderMgr -> PolyMesh` path submits the generated
six-index game `MTVERTEX` list through the real `IRenderPrimitive` callback,
with game model/view, bounded projection and material state. The backend
owns a white fallback for the production untextured handle-zero binding.
The real game-format callback produced 496 changed RGB pixels and checksum
`10105697032999492819` in a 64-by-64 readback and presented a frame under
both GCC and Clang on the desktop Wayland/Vulkan session. The existing GPU
fixture continued to pass after the fallback allocation. Exact commands,
input provenance and remaining App/Windows/retail gates are in
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md). This is a generated game-mesh
runtime check, not a game scene or retail compatibility result.

The same generated game mesh now embeds a 2-by-2 RGB565 `Tile` in its
production `Texture library`. Production `XMesh::read` and
`TextureLibrary::load_library` resolve and upload it through the native
pipeline's GPU texture callbacks; a production `PolyMesh` update binds the
resulting live handle for its indexed draw. On visible GCC, Clang and focused
Clang ASan/UBSan Wayland/Vulkan runs, the textured 64-by-64 readback checksum
was `11826028615312451232` with 496 changed pixels. Rebinding handle zero
against the same submitted geometry and state restored the white-fallback
checksum `10105697032999492819`. This establishes sampling in the game
mesh draw, while visible App mesh drawing, a full native game scene, same-input
Windows game-mesh comparison and retail parity remain open. Focused commands
are in [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).

The production App `MeshRender::Render` path now executes in the linked
generated game-material check. Both an older-format face group and a CQ2
face group submit six indexed 68-byte vertices through the native pipeline
to a CPU receiver; the latter invokes its resolved production
`IMaterial::DrawVB`. The App caller's largest-index-as-count error was
corrected at its three affected call sites. The older-format App branch
now also submits through a 68-byte SDL/Vulkan pipeline and presents a
distinct visible readback under GCC and Clang. The first image check
was invalid because App `PIPE` still addressed a CPU-only primitive;
the linked check now binds App `PIPE` and `BATCH` to one GPU primitive,
reads back during the draw callback, and distinguishes the App image
from white texture factor and opposite culling. The
CQ2 material effect is still CPU-only. See
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md) for commands and remaining
Windows/retail gates.

## Fragment-uniform render-state update (2026-09-27)

Source review of `SdlGpuRenderer::SetRenderState` found that every
accepted state change rebuilt all available graphics pipelines and
waited for GPU idle. The production App face-group path sets a texture
factor, but that value and `specularEnabled` are read only from the
fragment uniform; they do not alter depth, blend or rasterizer pipeline
configuration. The renderer now validates the whole state first, then
updates the snapshot directly when those are the only changed fields.
Depth, blend and culling transitions retain the existing transactional
pipeline rebuild. A `PipelineRebuilds` generation metric on the native
SDL owner makes this decision falsifiable in the linked game-material
check: its production App texture-factor setter must succeed without
advancing the generation, while the visible draw still changes when the
factor changes. No public game/Win32 interface or persistent layout was
modified.

The repository-generated older-format App draw retained its visible
64-by-64 checksum `2133805298751023491` and 496 changed pixels under
GCC 16.2.1 and Clang 22.1.8. Its white-factor checksum remained
`12405162864621168803`; opposite culling still produced a distinct
image. The existing visible SDL GPU regression passed under both
compilers, and focused Clang ASan/UBSan visible game-material execution
passed with leak detection disabled for this host. Both complete Linux
debug builds and command-sandbox CTest presets passed; the GPU checks
skipped in those presets and were executed separately on the desktop.
Native Windows Debug/Final, affected ABI checks, same-input D3D9 game
captures, retail runtime and a composed native game scene were not run.

Commands executed:

```text
cmake --build --preset linux-gcc-debug --target frontierx_game_material_manager_probe frontierx_sdl3_gpu_probe
cmake --build --preset linux-clang-debug --target frontierx_game_material_manager_probe frontierx_sdl3_gpu_probe
ctest --test-dir out/build/linux-gcc-debug -R '^portability.(game_material_gpu|sdl3_gpu)$' --output-on-failure
ctest --test-dir out/build/linux-clang-debug -R '^portability.(game_material_gpu|sdl3_gpu)$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_game_material_manager_probe
env ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.game_material_gpu$' --output-on-failure
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
ctest --test-dir out/build/linux-gcc-debug -R '^portability.game_material_gpu$' -V
```

## Material vertex-register bridge (2026-09-27)

`NativeRenderPipeline::set_vs_constants` now forwards only `c30` and `c31`
through an optional native backend callback. The SDL owner stores finite
UV offset/scale and vertex-color modulation values and pushes them alongside
the existing clip matrix. Defaults are identity UV and white color. The
`StandardVertex` shader applies them; other vertex formats preserve their
previous results. Visible GCC and Clang game-material runs changed and
restored bounded App images independently for the UV and color registers;
both existing SDL GPU regressions also passed. This verifies the native
register-to-uniform boundary, not production CQ2 effect execution. The
`Material::DrawVB` effect remains unavailable, and native Windows, retail
and same-input D3D9 checks were not run for this change. Exact commands
and the production-owner distinction are in
[GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md).


## Mission background alpha and second texture stage (2026-10-13)

Named P1b blockers: active `SpaceEnv::Render3db` requires alpha testing at
reference8, while selected ship materials have independently authored emissive
UV2 maps requiring the original legacy two-texture combine path. The former
native owner rejected those states. Existing single-stage image checks cannot
disprove an ignored second texture or discarded alpha state, so the existing
GPU probe is extended. The source SpaceEnv high-background RPVERTEX2/stage-one
branch is commented out in this snapshot; it is a reviewed source contract,
not active mission execution or proof that its extra StarBk pass is required.
Its intended retail setting remains a reference question.

Native RenderPrimitive now retains and transactionally forwards two texture-
combine stages, independent texture handles and sampler states. The SDL owner
binds both textures/samplers, snapshots them for captured frames and protects
both against premature destruction or replacement. The fragment shader handles
SELECTARG1/2, MODULATE, ADD and stage disable with the preceding CURRENT value;
it applies all eight alpha comparison functions to the final texture alpha.
Invalid values remain rejected without changing prior state. This is a native
runtime contract; no serialized format or Windows interface slot is widened.

Automated synthetic GPU execution under optional Debug Clang22.1.8 and
independent GCC16.2.1 on the visible Linux SDL/Vulkan host observed actual
all-fragment rejection, source-threshold output restoration, and differing
stage-one ADD images from the second UV pair. Existing captured-map replacement
and other renderer fixture predicates executed before the unchanged resize
failure (`1261x688` remained `1261x688`). The entire GPU CTest therefore remains
FAILED; these observations do not establish a full host pass or correctly
rendered retail mission background. Clang `portability.render_submission` and
the three `game_material_manager` variants executed successfully after the
supported-stage callback expectations were updated to match the real source
calls and assert disabled stage-one color/alpha state. Fresh complete suites,
sanitizers, native Windows Debug/Final, affected ABI checks and actual mission
background inspection remain open until separately recorded.


Follow-up execution: the existing explicit `hyprland-x11` GPU host fixture
passed the entire current Clang `portability.sdl3_gpu` CTest. Only the test's
mapped descendant Xwayland window was floated before real renderer startup;
all resize and repeat assertions remained intact. Stage-one UV2 at2.0 repeats
the0.0 sample under WRAP and matches the1.0 sample under CLAMP, with distinct
images between those modes. Ordinary Wayland resize remains a separate failed
host mode. The selected mission's visible background is still not accepted.


## Authored mip levels, background precision and opaque primary frames (2026-10-14)

Named P1b blockers: mission ship textures contain authored palette MIP0 through
MIP8, while the prior native owner accepted only one level; SpaceEnvironment
requests `StarBk.tga` as RGB5/A1. Loading only the base texture or silently
returning a zero background texture is insufficient. The existing texture/GPU
fixtures now exercise these actual production interfaces with independently
generated data. No retail textures or extracted inventories are included.

NativeRenderPipeline retains separate checked CPU levels, forwards complete
chains to the SDL texture owner, and supports per-level upload and lock/unlock.
SDL uploads every authored level and retains transactional replacement and
captured-frame texture lifetimes. The generated 256-square palette texture has
nine distinct levels; production TextureLibrary resolves it, a selected mip's
mutation changes actual minified output, and restoration recovers the exact
previous checksum. Invalid levels remain rejected. Legacy native `lodCount=0`
continues to mean one level; higher-level blits and general D3D texture API
parity are not claimed.

The explicit RGB5/A1 target preserves source packing precision before normalized
RGBA8 backend storage. The existing CPU check asserts exact quantized RGB
bytes and alpha127/128 endpoints for both data upload and locked TGA conversion.
A failed required StarBk load now raises a native mission error at SpaceEnv;
the original Windows branch remains unchanged. This check does not establish
correct appearance of the external background asset.

The production D3D9 primary buffer is X8R8G8B8 (`Direct3D_pipe.cpp`,
`create_buffers`). Native primary frame clears now retain alpha1 and RGB-only
color writes, while source alpha still participates in fragment rejection and
RGB blending. The existing GPU alpha fixture asserts fully opaque readback
after both rejected and restored draws. This policy is limited to current
primary-frame targets; future ARGB render-to-texture support must preserve
its own stored alpha rather than inherit it.

Executed: optional Debug Clang22.1.8 `portability.render_pipeline` passed exact
RGB5/A1 bytes; the complete existing `portability.sdl3_gpu` CTest passed on the
explicit `hyprland-x11` SDL/Vulkan host, including nine-level minification,
alpha/stage-one predicates, unchanged resize and repeated-scene assertions.
Independent refreshed GCC/Clang/sanitizer validation is ongoing. Ordinary
Wayland resize remains a failed host mode; native Windows/ABI, actual CI and
full mission visual/audio user acceptance remain unperformed.
