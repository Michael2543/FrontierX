# SDL3 GPU Renderer-Backend Prototype

Status snapshot: **2026-09-21**. This document records a focused backend experiment. It does not
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

Both generated shaders and compiled the probe with the project's strict warning policy. Separate
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

Both current shell runs reported the Wayland video driver, Vulkan backend, production
`PrimitiveBuilder via IRenderPipeline`, 17 presented frames, nine direct vertices, 2,496 changed
pixels and checksum `7951318278861886233`. The exact frame count and compositor-selected resize are
diagnostic, not cross-host invariants. The earlier focused Clang ASan/UBSan shell run used the opaque
fixture before this state slice; current sanitizer evidence covers the CPU state and pipeline owner.

The central matrix result is recorded in [BUILD_GRAPH.md](BUILD_GRAPH.md); its sandbox GPU check
skipped because no video device was exposed, so it is not counted as runtime evidence. An earlier
Clang 22.1.8 build with AddressSanitizer and UndefinedBehaviorSanitizer completed the visible
registered-owner-to-readback path before diffuse alpha was enabled. The current state owner and
vertex-buffer adapter passed focused ASan/UBSan CPU checks; no sanitized current-image claim is
made. Leak detection was disabled because LeakSanitizer cannot run in the local graphics-driver/
`ptrace` environment. The shaders were recompiled from the checked-in GLSL for both current presets.

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
  explicit smoke request. There is no actual `RenderBatcher`, complete legacy D3D9 state/material
  adapter or game target.
- The probe does not replace or modify the supported Win32 D3D9 renderer.
- Neither SDL3 nor the shader toolchain is approved for FrontierX binary distribution by this test.

## Next acceptance slice

The next renderer change should compile and execute the actual `RenderBatcher` closure against the
new native `IRenderPipeline` create/lock/unlock/destroy owner, then add RPVERTEX2 GPU consumption and
texture-stage/material mapping.
The repository-owned fixture must also be driven through the supported Windows path and compared
under a documented tolerance.
Only after that evidence should the project decide between a direct adapter and another maintained
D3D9-preserving route.
