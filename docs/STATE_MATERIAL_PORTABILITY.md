# Production StateMaterial Native Slice

Status: 2026-09-26, synthetic Linux CPU and visible Vulkan runtime integration. This is a production
`StateMaterial` owner executing through the existing `IMaterial` and
`IStateMaterial` interfaces. `MaterialLibrary` now discovers two classes in a
synthetic UTF archive, creates and indexes the owners and runs an indexed CPU
draw. `SimpleMaterial` also reads diffuse color, opacity and a named texture
from that archive in the focused CPU check. The same check links the production
`TextureLibrary` aggregate, uploads a generated RGB565 image and resolves its
native frame through `SimpleMaterial::verify` and `update`. The existing GPU
probe now also loads `SimpleMaterial` from the production `MaterialLibrary`,
binds its resolved frame through `StateMaterial` and observes visible Vulkan
readback. `SimpleMaterial::apply` and native game rendering remain
open. Focused texture evidence is in
[TEXTURE_LIBRARY_PORTABILITY.md](TEXTURE_LIBRARY_PORTABILITY.md).
The supported game uses a distinct `IMaterialManager` mesh-material interface;
see [GAME_MATERIAL_PATH.md](GAME_MATERIAL_PATH.md) before treating this
synthetic `IMaterialLibrary` path as a game-scene gate.

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

### MaterialLibrary creation path, 2026-09-26

The next production-owner blocker was that `MaterialLibrary.cpp` could not
compile on Linux: its lock used `CRITICAL_SECTION`, and its filesystem loader
required a concrete Win32 find-data record. The real caller is
`MaterialLibrary::create_material`, which creates and initializes `IMaterial`
instances through DACOM. The existing `StateMaterial` probe previously created
the class directly, so it could not detect a broken class map, aggregate
initialization or library reference ownership. The falsifiable check is that
the production library can create and find `StateMaterial`, apply its states,
submit its indexed draw, then release the indexed instance. Failure would keep
material-library composition out of the native scene; success allows the
filesystem loader to be connected next.

The owner retains Win32 critical sections on Windows and uses a recursive C++
mutex on Linux. It treats profile-parser class-map overrides as optional, as
its existing `initialize_materials_maps` reader already did. The native manager
does not expose `IProfileParser` through its global manager query. Class-ID
copies now check the caller's buffer length, and direct class-name fallback
checks its fixed local buffer. The probe registers the production
`MaterialLibrary` aggregate, supplies the same render services used by the
material, verifies undersized mapping output is rejected, resolves a class
alias, creates and case-insensitively finds the material, then retains the
earlier state, indexed draw and release checks. This changes process-local
component creation only; no persistent layout, module interface or draw-order
contract changed.

At this check's first execution, the native UTF adapter lacked the directory
contract, so `load_library` returned `GR_NOT_IMPLEMENTED`. The subsequent UTF
directory integration below supersedes that temporary Linux return path.

Executed on Linux x86_64 using repository-owned synthetic geometry, state
values and material names:

```text
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R '^portability.state_material$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_state_material_probe -j2
ctest --preset linux-clang-debug -R '^portability.state_material$' --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe
```

The GCC and Clang focused CTests passed and printed `production state material
draw passed`; the Clang ASan/UBSan executable printed the same result without
a sanitizer finding. LeakSanitizer was disabled for this host's ptrace
environment. The checks exercise a synthetic CPU draw through the production
library, material, pipeline and primitive. No GPU runtime, retail data, native
game scene or filesystem loading was exercised in this first check. Native Windows
Debug/Final material-module rebuild and ABI/layout checks were not performed;
the Windows material path requires a separate validation run.

### Native UTF directory to MaterialLibrary, 2026-09-26

The production `load_library` caller enters `Material library`, enumerates its
child directories, reads each `Type` child and calls `create_material`. The
existing native UTF adapter could read individual entries but offered none of
the three `IFileSystem` directory operations used by that caller. Extending
the earlier CPU probe is justified by this specific integration blocker: a
direct `create_material` check cannot prove that the archive's directory tree
reaches the production material owner. A failure would keep material discovery
outside the native composition; a pass permits the real material field and
texture owners to be connected next.

`ListUtfDirectory` reuses the bounded UTF 1.01 metadata validation and walks
only direct children of an exact-case virtual path. It rejects malformed
paths, duplicate child names, invalid links and invalid metadata without
altering the caller's output. The read-only `NativeUtfFileSystem` adapter now
supports `FindFirstFile`, `FindNextFile`, `FindClose`,
`SetCurrentDirectory` and `GetCurrentDirectory` for those paths. It retains
exact-case archive names while matching simple search wildcards without case
sensitivity. The Linux-only `WIN32_FIND_DATA` declaration has a checked
320-byte Win32-shaped layout and does not change the Windows SDK path. File
and search handles are monotonic process-local tokens in separate odd/even
sets; closing a handle and reusing its table slot does not revive that token.
No pointer or handle is stored in the archive format. Because enumeration now
passes archive names to material owners, `MaterialLibrary` skips names that
cannot fit `IM_MAX_NAME_LEN`, and `StateMaterial` rejects oversized names or
undersized getter buffers before copying. A failed `set_name` removes the
newly created library entry. These checks protect runtime buffers; they do not
change on-disk names or public interface layouts.

The extended existing probe writes a six-entry repository-owned UTF fixture
outside the source tree: root, `Material library`, two material directories,
and one `Type` child per material. It verifies exact-case navigation,
directory attributes, two-entry search iteration, search close, stale-token
rejection and bounded material-name getters. `MaterialLibrary::load_library`
then discovers `Loaded` and `Other`, reads `StateMaterial` from each `Type`
child and creates both through DACOM. Repeating the library load retains two
entries and releases lookup references; `find_material` now acquires its
returned reference while its map lock is held.
The previous aliased material's indexed draw still reaches the native
primitive, and `free_library` leaves zero indexed owners. This is synthetic CPU
runtime evidence for material *class discovery and creation*. `StateMaterial`'s
`load_from_filesystem` remains a no-op; no material state fields, texture bytes
or game scene are loaded by this fixture.

Executed on Linux x86_64 with GCC 16.2.1, Clang 22.1.8, repository-owned
synthetic UTF bytes, geometry and state values:

```text
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
cmake --preset linux-gcc-debug -B /tmp/frontierx-material-clean
cmake --build /tmp/frontierx-material-clean --target frontierx_state_material_probe -j2
/tmp/frontierx-material-clean/frontierx_state_material_probe /tmp/frontierx-material-clean/material_library_fixture.utf
```

Both full Linux builds completed and each CTest matrix executed 37 passing
checks; the GPU test skipped in the command sandbox and supplies no GPU
evidence here. The focused Clang ASan/UBSan process passed without a reported
finding. A fresh GCC configure, 35-step focused build and direct fixture
execution also passed. LeakSanitizer was disabled for this host's ptrace
environment. No
native Windows material-module rebuild, retail-data run, native game scene,
GPU execution for this change or same-input Windows comparison was performed.
The next material gate is a real file-backed material owner, paired with the
production texture library and the native game composition.

### SimpleMaterial field loading, 2026-09-26

The real `MaterialLibrary::load_library` caller constructs each class and invokes
its `load_from_filesystem` method within the UTF material directory. The prior
fixture proved class discovery but used `StateMaterial`, whose loader reads no
fields. The integration decision was whether the production `SimpleMaterial`
owner could read its `Type`, `C0` and `C1` fields through that same native
filesystem and expose their values through `IMaterialProperties`. The existing
probe could not answer this without linking that owner. A failure would leave
even CPU material constants outside the native composition; a pass permits
texture lookup and backend material-state work to proceed.

`SimpleMaterial.cpp` is linked into the existing focused target. Its native
declaration dependencies now include a bounded texture-name array, the D3D9
material layout and the lighting/specular state identifiers. The native
`D3DMATERIAL9` declaration has compile-time checks for its 68-byte size,
component offsets and final-field extent; the Windows build still uses the
DirectX SDK definition. The owner rejects a missing or unknown `Type` before
changing its material class. `apply` now returns the first failed pipeline
operation. In the current native pipeline `set_material` returns
`GR_NOT_IMPLEMENTED`, which the probe expects; a successful material draw is
not claimed. The fixture uses a test-only `ITextureLibrary` service stub solely
to satisfy the material initialization contract. It cannot load textures.

The extended repository-owned UTF fixture contains eight entries: the prior
root, material directory and two named material directories, their `Type`
fields, plus `Other/C0` and `Other/C1`. `Other` is `DcDtOc` with diffuse floats
`0.25, 0.5, 0.75` and opacity `0.5`; the production
`IMaterialProperties::get_constant` path returns those exact values. A second
generated archive supplies `Unknown` as its `Type`. The same material rejects
it with `GR_INVALID_PARMS`, and the previous diffuse value remains `0.25`.
These checks use synthetic CPU data; no retail asset or game scene is involved.

Executed from the repository root on Linux x86_64 with GCC 16.2.1 and Clang
22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R portability.state_material --output-on-failure
cmake --preset linux-gcc-debug -B /tmp/frontierx-simple-clean
cmake --build /tmp/frontierx-simple-clean --target frontierx_state_material_probe -j2
/tmp/frontierx-simple-clean/frontierx_state_material_probe /tmp/frontierx-simple-clean/material_library_fixture.utf
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
```

The clean GCC configuration built the focused target in 36 steps and its
direct execution printed `production state material draw passed`. Both full
presets executed 37 passing CTests with only the GPU CTest skipped in this
command sandbox. The focused Clang ASan/UBSan executable passed without a
reported finding; LeakSanitizer was disabled for this host's ptrace
environment. No native Windows Debug/Final material rebuild or SDK layout
comparison, GPU execution for this change, production texture lookup, retail
run, native game scene or same-input Windows comparison was performed. The
next material gate is production `ITextureLibrary` loading and supported
`set_material`/lighting state behavior at the native renderer boundary.

### Library-loaded SimpleMaterial draw and build-preset coverage, 2026-09-26

The production `SimpleMaterial::render` caller submits material geometry through
`IRenderPipeline`. Source review found that its noncached branch returned
`GR_OK` even if vertex-buffer acquisition, copy, unlock, draw or release
failed. It also dereferenced null `MaterialContext` and `VertexBufferDesc`
arguments. The existing `StateMaterial` probe already owned a valid cached
RPVertex buffer and a capture callback, but had never invoked the
library-loaded `SimpleMaterial` draw. The falsifiable check was that this
owner submits one indexed triangle with a valid cached buffer, returns an
invalid-input error for a rejected index or null input, and leaves the draw
count unchanged on failure. A failure would prevent reliable material-driven
scene diagnostics even after texture and lighting support arrive.

The owner now checks the two input pointers and propagates every operation
result in its noncached branch while still releasing an acquired buffer. The
probe keeps the UTF-loaded `SimpleMaterial` alive through the draw and uses
the existing buffer and capture callback. Its first execution failed because
the probe's material service pipeline had no view of the separate buffer owner.
The service pipeline now uses the existing `OwnedVertexBufferLookup` callback
to access that owner; this is a probe-composition correction, not a new
renderer implementation. One `StateMaterial` and one `SimpleMaterial`
indexed draw reach the CPU capture; invalid indices and null inputs return
`GR_INVALID_PARMS` without a third draw. `SimpleMaterial::apply` remains
unsupported by the native material/lighting pipeline, so this check does not
prove a material-driven GPU image.

An audit of `CMakeLists.txt` CTest commands against `CMakePresets.json` found
five existing executable checks omitted from both full build-preset target
lists: vertex-buffer manager, state material, math accessor, render-batcher
runtime and `BMPOOLJR`. A prior full Clang CTest had therefore executed an
older material binary. Both build presets now include all five; the comparison
of directly named CTest executables to each preset target list reports no
missing target. Subsequent GCC and Clang configure/build/CTest runs compiled
the changed material target before executing the suite.

Executed on Linux x86_64 with repository-owned synthetic UTF, geometry and
state inputs:

```text
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R '^portability.state_material$' --output-on-failure
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
```

The focused GCC CTest, both rebuilt full preset matrices and the focused Clang
ASan/UBSan executable passed; the latter printed `production state material
draw passed` without a reported sanitizer finding. LeakSanitizer was disabled
for this host's ptrace environment. Each full matrix executed 37 passing
checks and skipped the GPU CTest; that skip supplies no GPU evidence. The
noncached `SimpleMaterial` draw branch was changed but not directly executed
by this check. No Windows Debug/Final material rebuild, SDK layout/ABI check,
retail run, production texture loading, native game scene or same-input
Windows comparison was performed.

### Native UTF child views for the production texture caller, 2026-09-26

Source review of `TextureLibrary::load_texture` found that its real caller
enters `Texture library`, then calls `IFileSystem::CreateInstance` with a
`DAFILEDESC` for each texture directory. The native UTF adapter could enumerate
the directory but returned `GR_NOT_IMPLEMENTED` from that factory call. Thus
the production texture loader could not reach its image fields through the
native filesystem even after its Win32/DirectDraw code is ported. The existing
material UTF fixture has the same nested directory shape, so the material
probe was extended rather than adding another probe. A failure here would
keep production texture loading outside the native composition; a pass
permits a later direct `TextureLibrary` integration check.

`NativeUtfFileSystem::CreateInstance` now accepts the legacy `IFileSystem`
factory name and its versioned interface ID for a read-only `DAFILEDESC`.
It validates a single exact-case child directory through the bounded UTF
metadata reader and returns an independently owned filesystem view rooted at
that directory. It rejects path traversal, wrong-case names, file nodes and
write access. The parent current directory does not move. Each view keeps its
own file/search handle tables; no native handle enters the UTF archive.
The existing probe opens `Material library` and then `Other` through the
factory, reads `Type` from the nested view and checks the three rejected
cases. This is synthetic CPU execution of the filesystem contract used by
`TextureLibrary`; the production texture owner was not linked or executed.

Executed from the repository root on Linux x86_64 with GCC 16.2.1 and Clang
22.1.8 using the generated repository-owned UTF fixture:

```text
cmake --build --preset linux-gcc-debug --target frontierx_state_material_probe -j2
ctest --preset linux-gcc-debug -R '^portability.state_material$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_state_material_probe -j2
env ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-texture-handle-asan/frontierx_state_material_probe /tmp/frontierx-texture-handle-asan/material_library_fixture.utf
```

The focused GCC check passed after correcting the factory-name comparison:
`DAFILEDESC` defaults to `IFileSystem`, while `IID_IFileSystem` is the
versioned `IFileSystem__1` string. Both full presets executed 37 passing
checks and skipped the GPU CTest in the command sandbox. The focused Clang
ASan/UBSan executable passed without a reported sanitizer finding;
LeakSanitizer was disabled for this host's ptrace environment. No production
texture image was decoded or uploaded, no native game/GPU or retail runtime
was exercised, and no native Windows Debug/Final material or texture rebuild,
SDK ABI/layout check or same-input Windows comparison was performed. The next
texture gate is to compile and connect the production `TextureLibrary` owner
for one supported UTF image format through the existing pipeline upload path.

### Earlier StateMaterial execution, 2026-09-25

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

## MaterialLibrary to visible texture frame, 2026-09-26

The next blocker was that the GPU probe used a `StateMaterial` texture handle
obtained directly from `TextureLibrary`, while the real material owner resolves
`T0_name` through `SimpleMaterial::verify` and `update`. The existing CPU check
had exercised that lookup but could not show the resulting frame in a GPU
scene. The GPU probe now links production `MaterialLibrary` and
`SimpleMaterial`, registers their factories with the process DACOM manager
used by `MaterialLibrary::create_material`, verifies both material entries
and loads `Other` from the same generated UTF archive as the texture. Its
local service container supplies the existing pipeline, texture library and
vertex manager. The material's
retained texture reference supplies the frame bound by `StateMaterial`; the
material library and texture owner are released before a second scene run.

The `frontierx_sdl3_gpu_probe` target built under GCC 16.2.1, Clang 22.1.8
and the focused Clang ASan/UBSan configuration on Linux x86_64. Direct visible
Wayland/Vulkan execution with `--require-runtime` passed in all three: the
UTF-textured image checksum remained `9085705336011966884`, its foreground
mask matched the base fixture, rebinding restored the base checksum, and the
scene ran twice. Both full GCC and Clang debug presets passed their non-GPU
CTest entries; the command-sandbox GPU entry skipped at SDL video startup.
Exact build and direct-run commands, synthetic input provenance and the
unperformed Windows/retail checks are in
[TEXTURE_LIBRARY_PORTABILITY.md](TEXTURE_LIBRARY_PORTABILITY.md). This is
visible GPU execution of the material lookup and texture frame, not
`SimpleMaterial::apply` or a native game scene.

## Remaining acceptance work

`MaterialLibrary` now reads the material class from a synthetic UTF container
and creates two owners in the CPU probe, but
`load_from_filesystem` in `StateMaterial` is a stub in the source snapshot.
The noncached vertex-copy branch, `clone` and allocation-failure injection
remain unexecuted. The GPU draw uses synthetic service composition rather than
the game startup/material library. The production `SimpleMaterial` class and
one RGB565 texture now execute in a CPU composition, but its material and
lighting state application has no native backend. `DebugMaterial` and other
texture formats retain separate portability gates.
The supported native-Windows Debug/Final wrappers now rebuild successfully,
but their measured closure does not compile this `StateMaterial` translation
unit; see [WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md). A separate
material-module Windows build and ABI check, retail runtime and Windows image
comparison remain unperformed. No native Linux game target is claimed.
