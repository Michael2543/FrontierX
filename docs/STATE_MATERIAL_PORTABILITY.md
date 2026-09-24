# Production StateMaterial Native Slice

Status: 2026-09-24, synthetic Linux CPU and visible Vulkan runtime integration. This is a production
`StateMaterial` owner executing through the existing `IMaterial` and
`IStateMaterial` interfaces. Production material-library loading and native game
rendering remain open.

## Owner, caller and falsifiable blocker

`Code/Libs/Src/Shading/Materials/StateMaterial.cpp` is registered by the
supported Windows `Materials.cpp` module. `MaterialLibrary::create_material`
uses the registered class mapping to create and initialize an `IMaterial`;
the material applies stored render and texture states and can submit cached
indexed geometry through `IRenderPrimitive`. The current native renderer
already supports the selected depth, texture-combine and texture-binding
states, but no production material owner had been linked into a Linux target.

The integration question was whether the unchanged DACOM factory and material
interfaces could initialize this owner with the native primitive, pipeline and
production vertex-buffer manager, then send one cached indexed draw through
the real material method. A syntax-only compile could not answer lifetime,
factory, state forwarding or draw routing. The focused CTest target
`frontierx_state_material_probe` is therefore registered for this named
integration blocker. A failure means that the production material boundary
cannot yet be used for the representative native scene; success permits the
next step of replacing the synthetic services and callback with game and GPU
owners.

## Implementation and ownership

The production `StateMaterial` stored three `U32` arrays grown with `realloc`
but released them with `delete[]`. Its three setters also overwrote the only
array pointer when `realloc` failed. The owner now calls `free` for those
arrays, checks count and byte-size overflow before growth, and commits a new
pointer only after allocation succeeds. On allocation failure the setter
returns `GR_OUT_OF_MEMORY` and retains the old values. This is process-local
material state, with no disk/wire schema or public vtable change. The
`IMaterial.h` header now declares its `IFileSystem` pointer prerequisite
directly so consumers need not rely on an unrelated include order.

The probe registers the real `StateMaterial`, native primitive and production
vertex-buffer manager factories. A small synthetic service container supplies
those existing interfaces to `StateMaterial::initialize`. The material stores
one depth state, a stage-zero combine operation and a runtime `U32` texture
token, then `apply` forwards them to the native primitive callbacks. The probe
also saves a rejected texture token and verifies that `apply` returns the
backend failure without changing the committed binding. The owner now
propagates `apply`, `verify` and cached/noncached `render` errors rather than
returning success for failed backend calls; a rejected cached index and null
context are checked explicitly. Successful calls preserve the existing state
and draw order. `apply` is sequential and is not an atomic multi-state
transaction. The probe fills a pipeline-owned RPVertex-shaped buffer, invokes
`StateMaterial::render` with a cached-buffer `MaterialContext` and three U16
indices, observes one checked triangle submission, and destroys the buffer
and DACOM references. The token is a synthetic callback value; no texture
library, media file or GPU texture is loaded by this probe.

The existing SDL GPU probe separately creates that same production owner and
the production vertex-buffer manager through DACOM. A synthetic service
container supplies the native pipeline and primitive. With repository-owned
geometry, a pipeline-owned indexed vertex buffer and a registered 2x2 RGBA8
fixture texture, `StateMaterial::apply` forwards depth, stage-zero color combine
and the runtime texture handle, and `StateMaterial::render` submits the cached
U16-indexed triangle through the native primitive to Vulkan. Bounded 64x64
readback reports 2496 changed pixels and checksum `8289071695263040328`.
The probe then binds an alternate texture and its pipeline-copied equivalent
through the material, observes equal alternate/copy checksums
`2956733785601487666`, restores the original binding and repeats the full
scene lifecycle. The material draw and the direct `PrimitiveBuilder` draw use
different vertex/color inputs; their image checksums are intentionally not
treated as a pixel-parity comparison.

## Executed validation

Host: Linux x86_64. GCC 16.2.1 and Clang 22.1.8. Input: repository-owned
synthetic geometry and state values. Commands executed from the repository
root:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R '^portability.state_material$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_state_material_probe -j2
ctest --preset linux-clang-debug -R '^portability.state_material$' --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure

cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe

cmake --build --preset linux-gcc-debug --target frontierx_sdl3_gpu_probe -j2
out/build/linux-gcc-debug/frontierx_sdl3_gpu_probe out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
cmake --build --preset linux-clang-debug --target frontierx_sdl3_gpu_probe -j2
out/build/linux-clang-debug/frontierx_sdl3_gpu_probe out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_sdl3_gpu_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_sdl3_gpu_probe /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv --require-runtime --visible
```

Both focused CTests printed `production state material draw passed` and
returned success. The Clang ASan/UBSan process printed the same result without
a sanitizer finding; LeakSanitizer was disabled for this host's ptrace
environment. Both full Linux debug presets built and executed 37 passing
CTests; the SDL GPU CTest skipped in the command sandbox. The material probe
is synthetic CPU runtime evidence. Separate visible Wayland/Vulkan runs of the
material-integrated GPU probe passed with GCC, Clang and focused Clang
ASan/UBSan; all three reported the same material, alternate and copied texture
checksums, 12 presented frames and two scene runs. No sanitizer finding was
reported; LeakSanitizer was disabled for this host's ptrace environment. The
texture-copy result is also recorded in
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

## Remaining acceptance work

`MaterialLibrary` did not create the material in this probe, and
`load_from_filesystem` in `StateMaterial` is a stub in the source snapshot.
The noncached vertex-copy branch, `clone` and allocation-failure injection
remain unexecuted. The GPU draw uses synthetic service composition rather than
the game startup/material library. `SimpleMaterial`/`DebugMaterial` retain the
compile-derived D3D/Win32 blockers listed in the renderer document. Native
Windows Debug/Final rebuilds, material-module ABI checks, retail runtime and
Windows image comparison were not performed. No native Linux game target is
claimed.
