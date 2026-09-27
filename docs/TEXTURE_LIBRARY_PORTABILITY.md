# Production TextureLibrary Native Bring-Up

Status: 2026-09-26. The production `TextureLibrary.cpp` owner compiles with GCC
and Clang on Linux x86_64. A synthetic CPU check now loads a generated UTF
RGB565 image through that owner, uploads its RGBA8 conversion through the
native `IRenderPipeline`, exposes the frame to production `SimpleMaterial`,
and verifies destruction and failed-upload rollback. The existing SDL/Vulkan
probe now loads `SimpleMaterial` and the same generated fixture through the
production material and texture owners, then observes a changed, bounded image
readback under GCC, Clang and focused
ASan/UBSan. This is a synthetic scene, not a native game scene.

## Owner, caller and integration decision

The production `SimpleMaterial::initialize` caller requires `ITextureLibrary`
from its system services. Its later `verify` and `update` methods resolve a
named texture, retain a texture reference and retrieve the active render-pipe
frame. On Windows, `TextureLibrary::load_texture` opens each UTF texture
directory through `IFileSystem::CreateInstance`, then chooses a normal,
animated or external-reference loader. The native UTF adapter's child-view
contract was established in the preceding material check. The next blocker
was that the 3,802-line production texture owner itself included Win32,
DirectDraw and Windows-only renderer headers and was absent from the Linux
build graph.

The falsifiable first integration check was whether the same production
owner, rather than a test-only `ITextureLibrary` implementation, could be
registered as a DACOM aggregate, initialize with the native pipeline and
provide a balanced texture-ID lifecycle to a library-created material. The
previous material check used a stub that could not answer aggregate, format-map
or reference ownership questions. Passing this check permits an actual UTF
image decode/upload integration; failing it would keep the owner outside the
native material composition. The existing material CTest was extended, with
no separate product-like texture implementation.

## Implementation and boundaries

`TextureLibrary.cpp` now retains its Windows SDK, DirectDraw and DLL entry
path on Windows. On Linux it uses the existing native legacy file declarations,
case-correct engine headers, an exact basename conversion in place of the
Windows CRT path split functions, and a native DACOM registration entry.
The unused `RPUL.h` include was removed. The legacy video-control header now
uses exact include names and the native rectangle declaration. A native
external-file reference still fails explicitly because its Windows
`SearchPath` resolution has no replacement yet; this check does not use that
format. The owner was added as an object target and linked into the existing
material probe. Its process-local runtime texture IDs remain `U32`; no native
pointer was stored in a serialized field or interface handle.

Without a profile parser, the legacy constructor had left `library_state`
uninitialized. It now installs the same default LOD, dimension and load-mode
values requested by the optional profile-parser branch, including the exact
float bits for a 1.0 LOD scale. The probe queries LOD loading and load mode,
creates one named synthetic texture ID through the production interface,
checks presence/count, releases it, and observes absence/count zero. The
production aggregate then supplies `ITextureLibrary` to the material service
container; both previously loaded material classes and their CPU draws still
execute. No texture frame or GPU resource is created by this ID test.

The next falsifiable blocker was the mismatch between legacy DA output-format
classes and the native pipeline's RGBA8 texture owner. For the UTF `True RGB
565` source alone, the Linux production owner now requests RGBA8 and the
native pipeline converts each source pixel to RGBA8 at upload. Windows keeps
its existing format selection. The material probe generates a 2-by-2 UTF
texture with red, green, blue and white pixels; it checks all 16 uploaded
bytes, the production texture frame ID, and `SimpleMaterial`'s named texture
reference after `verify` and `update`. The native pipeline's vertex-lookup
constructor now accepts texture callbacks so the same composition can own
cached geometry and texture resources.

The old and new normal loaders now propagate decode, format selection,
dimension and upload failures and destroy a created native texture after a
failed upload. Mip loops stop at the discovered count even when `MIP0` is
absent, and archive dimensions are bounded before size multiplication or
allocation. The probe forces an upload failure after unloading its first
texture; the second attempted load creates and destroys exactly one handle
without publishing the failed named texture. A second generated archive names
its sole mip directory `MIP1`; the loader terminates without creating or
publishing a texture. This regression protects against the prior unbounded
retry when `MIP0` was absent. This tests one valid level only.
Indexed, multilevel, animated, external-reference and TGA 1.6 formats remain
open. The TGA 1.6 loader has separate unchecked upload results and is not
covered by this check. `SimpleMaterial::apply` still encounters unsupported
native material/lighting states. The GPU check binds the production texture
through the existing `StateMaterial` path; it does not execute
`SimpleMaterial::apply` or the game.

The GPU probe reuses the CPU check's generated UTF fixture writer. Its
`TextureLibrary` DACOM aggregate uses the real native pipeline callbacks
backed by `SdlGpuRenderer`. The production `MaterialLibrary` aggregate finds
both material entries in the same UTF archive and loads `Other`;
`SimpleMaterial::verify` and `update` resolve its `T0_name` to a retained
texture reference and current render-pipe frame. The probe binds that frame
through `StateMaterial`, reads a 64-by-64 image and checks that its checksum
differs from the repository fixture while its 2,496 changed
pixels remain spatially equal. Rebinding the original texture restores its
checksum. Material and texture owners release their references before the
same process repeats the entire scene after renderer shutdown and
initialization. This closes the one-format material-library-to-GPU frame
boundary; broader format loading, `SimpleMaterial::apply` and native game
composition remain separate gates.

`MaterialLibrary::create_material` uses the process DACOM manager, so the GPU
probe registers the production material classes once with `DACOM_Acquire()`.
Its per-scene renderer and texture aggregates still use the existing local
manager and service container. This follows the production factory call path
without changing DACOM interfaces or introducing a second material loader.

## Executed validation

Host: Linux x86_64. Compilers: GCC 16.2.1 and Clang 22.1.8. Inputs: generated
repository-owned synthetic UTF bytes, geometry, material names and RGB565
pixels; no retail media. Additional commands executed from the repository root
for this integration:

```text
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R '^portability.state_material$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
cmake --build --preset linux-gcc-debug --target frontierx_sdl3_gpu_probe -j2
cmake --build --preset linux-clang-debug --target frontierx_sdl3_gpu_probe -j2
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_sdl3_gpu_probe -j2
```

The focused GCC material test passed with the decoded four-color pixel check,
material texture reference, destruction, rejected-upload rollback and missing
`MIP0` termination. Both full Linux presets passed their non-GPU CTests and
skipped the GPU CTest in this command sandbox; the skip is not GPU runtime
evidence. The focused Clang ASan/UBSan executable passed without a reported
sanitizer finding.
LeakSanitizer was disabled for this host's ptrace environment.

The three direct visible GPU commands below ran on the local Wayland/Vulkan
session. The command sandbox's `ctest` GPU entry skipped at SDL video
initialization because it had no video device; those skips are not included
in the GPU runtime evidence. Each direct run used `--require-runtime`, which
returns failure instead of skip when video/GPU startup is unavailable:

```text
out/build/linux-gcc-debug/frontierx_sdl3_gpu_probe out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-gcc-debug/sdl_gpu_material_texture_fixture.utf --require-runtime --visible
out/build/linux-clang-debug/frontierx_sdl3_gpu_probe out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe_t4.vert.spv out/build/linux-clang-debug/sdl_gpu_material_texture_fixture.utf --require-runtime --visible
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_sdl3_gpu_probe /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.vert.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe.frag.spv /tmp/frontierx-texture-handle-asan/generated/shaders/sdl3_gpu_probe_t4.vert.spv /tmp/frontierx-texture-handle-asan/sdl_gpu_material_texture_fixture.utf --require-runtime --visible
```

All three returned success, presented 12 frames in the first scene run and
reported two scene runs with a repeated presentation. Their production UTF
texture checksum was identically `9085705336011966884`, versus the restored
fixture checksum `7951318278861886233`; the displayed footprint remained
2,496 changed pixels. The separate original `StateMaterial` indexed draw
checksum remained `8289071695263040328`. The ASan/UBSan run reported no
sanitizer finding; LeakSanitizer was disabled for the host's ptrace
environment.

This is production-owner compilation, synthetic CPU runtime and visible
SDL/Vulkan runtime evidence for one RGB565 UTF texture, material lookup and
backend ownership. No native game scene, retail runtime, Windows Debug/Final
texture-module rebuild, affected SDK/ABI layout check or same-input Windows
comparison was performed.
