# ZBatcher RenderBatcher Portability

Status snapshot: **2026-09-25**. This document records the Linux declarative closure of the
production ZBatcher interface, execution of its `BMPOOLJR` arena owner, the
production vertex-buffer manager lifecycle, a neighboring vertex-descriptor copier, linked
synthetic direct and U16-indexed draws and visible SDL GPU direct and indexed draws through the actual App-side
`RenderBatcher.cpp`, plus a native VS2022 Win32 build follow-up. It does not claim pool batching,
a native game scene, or completion of Milestone 5.

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

The actual App-side ZBatcher `.cpp` now compiles in a native object target,
executes a direct draw in a linked synthetic check and reaches visible Vulkan output through the
native shell. Its Windows PCH is skipped
on Linux and the `ICQBatch` and `TManager` declarations compile. The native
translation unit uses the extracted production flag types and only its needed
global declarations, rather than importing the broad Windows-bound `Globals.h`.
The linked check and shell supply temporary diagnostic/global values; there is no
native production `ICQImage` diagnostic service or native owner for the
renderer globals yet. The `DEffectOpts.h` include resolves with the App
`DInclude` path. The
`IVertexBufferManager` implementation now executes in an isolated native
target, but is not linked into ZBatcher. The `BMPOOLJR`
implementation also executes in an isolated native target, but is not linked
into a native ZBatcher module outside the focused executable.
Closing the remaining production dependency chain is the next Milestone 5 step and must preserve
the supported Windows ZBatcher build.

### State index validation (2026-09-23)

Source review of the now-linked production `RenderBatcher::set_state` and `get_state` found that
both indexed the runtime `state` array directly from an `RPRSTATE` argument. The public enum ends
at `RPR_MAX_STATE`, while the App-owned `RPR_DELAY` extension occupies the one extra slot. An
out-of-range value could therefore read or write outside the aggregate. Both methods now reject
indices at or beyond `RPR_MAX_STATE + EXTRA_STATES` with `GR_INVALID_PARMS`; `get_state` also
rejects a null result pointer. The existing linked batcher runtime check passes an out-of-range
positive enum value and a null output pointer, then confirms the production batch state remains
zero and the fixture draw still succeeds. No persistent layout or public vtable changed. The
affected type is a runtime renderer interface shared with the Windows ZBatcher module.

The GCC 16.2.1 and Clang 22.1.8 `frontierx_render_batcher_runtime_probe` targets built and their
`portability.render_batcher_runtime` CTests passed on Linux x86_64. The focused Clang ASan/UBSan
build in `/tmp/frontierx-render-batcher-asan` executed the same repository-owned synthetic inputs
without a sanitizer finding. Leak detection
was disabled on this host because its ptrace environment prevents reliable LeakSanitizer
execution. This is synthetic CPU runtime evidence; native Windows Debug/Final rebuilds,
cross-module ABI checks, queued batching, retail runtime and Windows image comparison remain
unperformed. The focused commands were:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_runtime_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_batcher_runtime$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_runtime_probe -j2
ctest --preset linux-clang-debug -R '^portability.render_batcher_runtime$' --output-on-failure
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_render_batcher_runtime_probe -j2
ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-render-batcher-asan/frontierx_render_batcher_runtime_probe
```

### Indexed production direct draw (2026-09-23)

The linked batcher check previously reached only the non-indexed production draw method.
The native pipeline's isolated indexed checks could not prove that the App-side batcher forwards
U16 indices through its real `IRenderPrimitive` interface. The initial extension submitted a
repository-owned three-vertex RPVertex triangle with indices `[0, 1, 2]` after each existing
PrimitiveBuilder scene. Its native submission callback validated triangle-list topology, vertex
and index counts, index values and primitive count. Across two create/initialize/release cycles,
the process reported four draws and no production assertion. GCC 16.2.1, Clang 22.1.8 and focused
Clang ASan/UBSan builds and executions passed using the same focused commands above. This was
synthetic CPU runtime evidence for the batcher's direct indexed branch. The later indexed
fixture and visible GPU execution are recorded immediately below. Queued batches,
texture/material mapping, native Windows Debug/Final and cross-module ABI checks, retail runtime
and Windows image comparison remain open.
Both full Linux debug preset builds and CTest presets were rerun after the code and check changes;
their current matrix result is recorded in [BUILD_GRAPH.md](BUILD_GRAPH.md). The GPU CTest skipped
in this command sandbox and does not add GPU runtime evidence for this change.

### Indexed batcher draw to visible GPU (2026-09-23)

The CPU check above established the batcher's U16 forwarding but did not show whether the shell's
production-batcher path could present an indexed draw. `RenderSmokeScene` now supplies an indexed
form of its existing repository-owned triangle-and-quad fixture: seven unique `RPVertex` values
and nine U16 indices reproduce the same nine-vertex triangle list, including diffuse alpha and
UVs. The shell first calls the existing production `PrimitiveBuilder` direct path and takes a
bounded 64-by-64 readback, then submits
this indexed form through `RenderBatcher -> NativeRenderPipeline -> NativeRenderPrimitive ->
SdlGpuRenderer`. It validates the final GPU owner's seven vertices and nine indices before
presentation. It takes a second bounded readback after indexed presentation and requires equal
changed-pixel counts and exact local checksums for the two forms in one process. The final window
image is from the indexed submission; the direct image has GPU readback evidence from the same
run. Earlier visible shell runs separately presented the direct fixture.
The indexed fixture changes no retail data, persisted layout, Windows renderer or engine ABI.

The existing linked `frontierx_render_batcher_runtime_probe` now uses that same indexed fixture
instead of its separate one-triangle input. It validates triangle-list topology, seven vertices,
nine indices, index values and three primitives, and observes four callback draws across two
aggregate lifecycles. GCC 16.2.1 and Clang 22.1.8 focused builds and CTests passed. A focused
Clang ASan/UBSan build and process execution reported `production batcher submitted 4 draws`
without a finding; LeakSanitizer was disabled because of this host's ptrace environment.

Visible Wayland/Vulkan shell runs of the GCC and Clang preset builds, followed by the focused
Clang ASan/UBSan shell, used only the repository fixture geometry/texture, generated shaders and
synthetic empty data-root directories. Each returned successfully with matching direct/indexed
structural readbacks and 17 presented frames,
`vertices: 7`, 2,496 changed pixels and local checksum `7951318278861886233`. The shell checked
nine indices internally. The direct readback matched those final indexed values in each process.
The checksum also matches the earlier direct fixture on this driver, but is
diagnostic rather than a cross-driver image tolerance. The command sandbox could not open an SDL
window; the GPU results came from separate visible-host executions. Reproduction commands from
the repository root were:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_runtime_probe frontierx_native_shell -j2
ctest --preset linux-gcc-debug -R '^portability.(render_batcher_runtime|native_shell)$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_runtime_probe frontierx_native_shell -j2
ctest --preset linux-clang-debug -R '^portability.(render_batcher_runtime|native_shell)$' --output-on-failure
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_render_batcher_runtime_probe frontierx_native_shell -j2
ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-render-batcher-asan/frontierx_render_batcher_runtime_probe
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-batcher-shell-data --smoke-ms 250 --render-smoke out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings out/build/linux-clang-debug/frontierx_native_shell --data-root /tmp/frontierx-batcher-shell-data --smoke-ms 250 --render-smoke out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv
env ASAN_OPTIONS=detect_leaks=0 XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings /tmp/frontierx-render-batcher-asan/frontierx_native_shell --data-root /tmp/frontierx-batcher-shell-data --smoke-ms 250 --render-smoke out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv
```

This is synthetic Linux GPU runtime evidence for one indexed production-batcher draw, not pool
batching or material/texture-stage integration. The SDL owner still uploads its fixture texture
directly. Repeated in-process scene ownership, native Windows Debug/Final rebuilds and module ABI
checks, Windows image capture/comparison, retail data and native game execution remain open.

### Linked production direct draw (2026-09-23)

The supported App-side batcher now has a non-Windows registration function
using the same aggregate factory type and priority as its Windows DLL entry.
The focused `frontierx_render_batcher_runtime_probe` links that production
object with the production `BMPOOLJR` source, the real `MathLib` accessor,
the existing DACOM bootstrap, native `IRenderPipeline` and registered native
`IRenderPrimitive`, and the repository-owned `RenderSmokeScene` caller. It
provides only the absent game-global values and `ICQImage`/`FDUMP` diagnostic
handlers inside the test executable. Those test values are not a native game
service. A new check was needed because the header, manager and isolated
allocator probes could not prove that the supported App-side owner links,
creates through DACOM or forwards a real draw. The result determines whether
the next work can focus on pool/state integration or must still close the
basic component and link boundary.

The process creates the native submission owner first, then registers and
creates the production batcher aggregate through DACOM, queries its aggregate
interface and calls its real `Initialize`. With the batch state disabled by
the production default, the `PrimitiveBuilder` smoke scene traverses
`PrimitiveBuilder -> RenderBatcher -> IRenderPipeline -> IRenderPrimitive`.
At that first linked check, the callback observed exactly one valid triangle-list submission with nine
RPVertex-shaped vertices, no indices and three primitives. No production
assertion was invoked. This exercises the direct forwarding branch; it does
not exercise the material pools, queued flush or texture-stage state capture.

Executed on Linux x86_64:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_runtime_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_batcher_runtime$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_runtime_probe -j2
ctest --preset linux-clang-debug -R '^portability.render_batcher_runtime$' --output-on-failure
cmake --preset linux-clang-debug -B /tmp/frontierx-render-batcher-asan -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_render_batcher_runtime_probe -j2
ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-render-batcher-asan/frontierx_render_batcher_runtime_probe
```

GCC 16.2.1, Clang 22.1.8 and the focused Clang ASan/UBSan process each
reported `production batcher submitted 1 draws` and passed the shape and
assertion checks. Leak detection was disabled because this host's ptrace
environment prevents reliable LeakSanitizer execution. Input data came only
from the repository smoke fixture. This is synthetic CPU runtime evidence,
not a Vulkan capture or retail scene. At that point, native Windows Debug/Final rebuilds,
module ABI/layout checks, sanitizer execution of the queued pool path,
production global/diagnostic ownership, registered math provider, texture and
material mapping, GPU readback through this batcher and Windows image comparison
remained open. The later shell evidence is recorded above.

### Transactional batch activation (2026-09-23)

Source review of the production `RenderBatcher::set_state(RPR_BATCH, 1)` path found that it
captures every texture-stage state from `PIPE` before accepting queued batching. Previously it
ignored each `get_texture_stage_state` result. The native pipeline returns
`GR_NOT_IMPLEMENTED` for this slot, so the batcher could mark batching active with an
uninitialized state snapshot. The owner now captures into a zero-initialized temporary array,
returns the first failing result, and copies the complete snapshot only after all reads succeed.
The existing Windows success path retains the same state values and order. This changes a shared
production method, so native Win32 Debug/Final rebuilds and renderer behavior checks are still
required.

The existing `frontierx_render_batcher_runtime_probe` now checks that the production default is
`RPR_BATCH=0`, that native activation returns `GR_NOT_IMPLEMENTED` and leaves that state at zero,
and that the repository smoke geometry still reaches the direct draw callback afterward. During
this check, `opt_get_u32` also exposed an uninitialized default when no `IProfileParser` is
available through manager `QueryInterface`. All four shared option helpers now initialize their
output defaults before looking up the parser; the same probe checks unsigned integer, float,
string and vector defaults. These are synthetic CPU runtime results, not evidence that queued
pool batching or a texture/material state path executes.

On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 rebuilt the focused target and passed
`portability.render_batcher_runtime`; the focused Clang ASan/UBSan executable also passed with
`ASAN_OPTIONS=detect_leaks=0` because host ptrace prevents reliable LeakSanitizer execution.
Both full debug presets built and each CTest run executed 36 checks successfully; the SDL GPU
check was skipped in this sandbox and provides no runtime evidence here. The focused commands are
the ones above; the full matrix uses `cmake --build --preset linux-gcc-debug -j2` and
`ctest --preset linux-gcc-debug --output-on-failure`, with `linux-clang-debug` substituted for
Clang. A native texture-stage implementation, pool/flush execution, sanitizer coverage for the
queued path, Windows ABI and behavior checks, retail data, and Windows image comparison remain
open.

### Texture-stage setter ownership (2026-09-23)

The same production owner stores four texture-stage snapshots and four current/committed texture
IDs. Source review found both setters indexed these arrays before checking `stage`; the state
setter also indexed a state value outside its `[1, D3DTSS_MAX_STATE)` range. In direct mode it
updated the cached state before asking `PIPE` to apply the value, and the texture setter advanced
its committed ID even when `PIPE` failed. A later successful submission could therefore observe
cached values that the backend had rejected.

`set_texture_stage_state` and `set_texture_stage_texture` now reject invalid stage/state indices
with `GR_INVALID_PARMS`. In direct mode they update cached values only after the pipeline call
succeeds; queued-mode storage remains available once that mode has a supported backend. The
existing linked runtime check now exercises invalid stage/state inputs and confirms that a valid
state returns the native backend's `GR_NOT_IMPLEMENTED`, followed by the same successful direct
smoke draw. This is a bounds and failure-propagation check, not an executed texture/material
rendering path. GCC 16.2.1 and Clang 22.1.8 focused builds and CTests, and the focused Clang
ASan/UBSan executable, passed on Linux x86_64 with repository-owned fixture geometry. Windows
Debug/Final rebuilds, module ABI checks, queued texture behavior, GPU output and retail/Windows
comparison remain unperformed.

### Stage-zero state forwarding in the visible shell (2026-09-23)

The shared fixture now sets the six color/alpha stage-zero fields used by production
`SimpleMaterial`/`DebugMaterial`, plus stage-one color/alpha disable, through the actual
`RenderBatcher` in the native shell. Direct-mode production setters forward to `PIPE` and retain
their cache only after the native primitive/backend succeeds. A visible GCC Wayland/Vulkan shell
run submitted direct and U16-indexed forms, presented 17 frames, and read back 2,496 changed
pixels with checksum `7951318278861886233`. The synthetic 2-by-2 fixture texture still enters
the SDL renderer directly; production texture handles, material objects and queued pool state
capture remain open. The focused GPU probe exercises stage-zero image change and rollback; see
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md). GCC and Clang full builds and 36
executed CTests per preset passed; their GPU CTests skipped under the command sandbox. Windows
Debug/Final, module ABI, retail runtime and Windows image comparison were not performed.

The shell now also registers a second repository-owned RGBA8 texture with the SDL owner, binds
its runtime `U32` ID through `RenderBatcher::set_texture_stage_texture`, observes a different
bounded image, restores the fixture handle and exact original checksum, then destroys the extra
texture before the indexed submission. The native pipeline's `is_texture` checks live backend
handles so the production batcher's assertion and forwarding use the existing renderer contract.
GCC, Clang and focused Clang ASan/UBSan visible shell runs passed with 17 presentations and final
checksum `7951318278861886233`; the focused GPU probe also checks invalid and stale IDs. See
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md) for commands and ownership limits.
The production `TextureManager`/`ITextureLibrary` still do not create these handles, and queued
pool capture remains unsupported.

### Integrated shell-to-GPU direct draw (2026-09-23)

The remaining named integration blocker was that the linked production batcher had only reached a
CPU callback; the native shell's visible GPU scene still bypassed it. The existing shell
`--render-smoke` path now creates the native `IRenderPrimitive` first, registers the production
App-side `RenderBatcher` factory, creates and initializes its DACOM aggregate, and submits the
existing repository-owned `PrimitiveBuilder` fixture through
`PrimitiveBuilder -> RenderBatcher -> NativeRenderPipeline -> NativeRenderPrimitive ->
SdlGpuRenderer`. The native pipeline is the batcher's `PIPE` owner during submission. The shell
composition root supplies temporary App flag/effect globals and fatal diagnostics because the
production Globals and `ICQImage` services are still Windows-bound. It releases the batcher before
the pipeline leaves scope. The direct branch is selected by the production `RPR_BATCH=0` default;
the previous explicit refusal of unsupported pool activation remains in force.

The production `BMPOOLJR` translation unit is shared as a native CMake object between its focused
probe, the linked batcher probe and the shell. This keeps strict warnings on the shell itself while
scoping the known legacy math-header warning exceptions to that one owner. It does not replace the
Windows ZBatcher project or change persistent layouts. The SDL renderer still owns the fixture's
2-by-2 texture and graphics pipeline directly. The later state-forwarding check above covers six
stage-zero combine fields, while material binding, queued pool flushing and retail asset
compatibility remain open.

Executed on Linux x86_64 with repository fixture geometry/texture and generated GLSL SPIR-V:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell -j2
ctest --preset linux-gcc-debug -R '^portability.(native_shell|render_batcher_runtime|bmpooljr)$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_native_shell -j2
cmake --build /tmp/frontierx-render-batcher-asan --target frontierx_native_shell -j2
cmake --preset linux-gcc-debug -B /tmp/frontierx-batcher-clean
cmake --build /tmp/frontierx-batcher-clean --target frontierx_native_shell -j2
ctest --test-dir /tmp/frontierx-batcher-clean -R '^portability.native_shell$' --output-on-failure
```

Each GCC 16.2.1 and Clang 22.1.8 shell binary then ran visibly with an isolated empty-directory
data root, a 250-ms deadline and its preset's generated shaders. The empty root was created with
`mkdir -p /tmp/frontierx-batcher-shell-data/{Objects,Interface,Textures,Mat,Profiles,Maps/Campaign,Maps/Multiplayer}`:

```text
env XDG_CONFIG_HOME=/tmp/frontierx-batcher-settings ./frontierx_native_shell \
  --data-root /tmp/frontierx-batcher-shell-data --smoke-ms 250 \
  --render-smoke generated/shaders/sdl3_gpu_probe.vert.spv \
                 generated/shaders/sdl3_gpu_probe.frag.spv
```

Both visible runs reported Wayland/Vulkan, 17 presented frames, nine direct vertices, 2,496
changed pixels and checksum `7951318278861886233`, matching the earlier direct-pipeline fixture.
The Clang ASan/UBSan shell ran with the same data root and GCC-generated shaders, with
`ASAN_OPTIONS=detect_leaks=0` because host ptrace prevents reliable LeakSanitizer execution; it
reported the same image result without sanitizer findings. These are synthetic Linux GPU runtime
results. The frame count and checksum are local observations, not portable image tolerances. The
full GCC and Clang debug presets each executed 36 CTests successfully; their separate sandbox GPU
CTest was skipped and is not included in this GPU evidence.

The sanitized run used the same shell arguments with
`/tmp/frontierx-render-batcher-asan/frontierx_native_shell` as executable and absolute shader
paths under `out/build/linux-gcc-debug/generated/shaders`. Its visible host GPU access required
running outside the command sandbox. The clean GCC directory built the shell and passed its dummy
driver process test. The visible host run and the clean dummy-driver test cover different runtime
paths.

Native Windows Debug/Final rebuilds, cross-module ABI/layout checks, a Windows capture of the same
fixture, repeated in-process scene lifetime, queued batching, texture-stage/material state mapping,
an actual game scene and retail-runtime comparison remain unperformed. The shell's temporary
globals and diagnostics must be replaced by production owners before game integration.

### App-side production object compilation (2026-09-23)

The real `Code/App/Src/RenderBatcher.cpp` uses `PIPE`, `VB_MANAGER`, `CQFLAGS`
and `CQRENDERFLAGS` from `Globals.h`. Source review found no other direct
global use from that broad header. Its two existing flag type definitions
were moved verbatim into `GlobalFlagTypes.h`; `Globals.h` continues to declare
the same exported Windows variables of those types. The native batcher branch
includes the shared type header and declares only the four references it
actually uses. The native type check asserts an eight-byte `CQGlobalFlags` and
four-byte `GlobalRenderFlags` under both Linux compilers. These flags are
runtime globals shared across modules, not disk or wire records. Moving the
definitions does not establish Windows ABI parity: native Win32 layout and
module rebuild checks remain mandatory.

`DEffectOpts.h` now includes its fixed-width type owner, keeps its Windows
import/export and `_ADB` branches, and declares its native global as `extern`.
The App-side source includes `<cfloat>` for `FLT_MAX`, uses `std::max` for its
two delay comparisons, and spells the `flush` parameter as `U32`, matching
the production `IRenderPrimitive` vtable (`DWORD` and `U32` are the same
32-bit underlying type on supported Win32). Its DLL entry, heap-message
function and Windows export remain in the Windows branch. This keeps Windows
module setup on Windows without compiling Win32 calls into a Linux object.

The native-only `da_d3dtypes.h` branch now supplies the D3D9 render-state and
texture-operation values used directly by this owner, plus the two DirectDraw
vertex-buffer lock bits. Their values were checked against the published
[Windows SDK D3D9 type header](https://github.com/tpn/winsdk-10/blob/master/Include/10.0.14393.0/shared/d3d9types.h)
and [DirectDraw header](https://chromium.googlesource.com/chromium/deps/perl/+/6f3e5028eb65d0b4c5fdd792106ac4c84eee1eb3/c/i686-w64-mingw32/include/ddraw.h);
the Windows branch still uses the installed DirectX headers. These numeric
interoperability facts do not implement the corresponding native render states.

The new `frontierx_render_batcher_object` CMake target compiles the actual
supported App-side translation unit. It has no synthetic replacement source
and is part of both default Linux preset builds. The focused commands executed
on Linux x86_64 were:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_object -j2
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_object -j2
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_header_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_batcher_header$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_header_probe -j2
ctest --preset linux-clang-debug -R '^portability.render_batcher_header$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
```

GCC 16.2.1 and Clang 22.1.8 each compiled the production object and passed
the focused header check. Each full matrix built, ran 34 passing CTests and
skipped its one host-dependent GPU test. The object build reported existing
`IProfileParser_Utility.h` character/string warnings and, under Clang, a
switch missing eight state enumerators; GCC C++20 also reported the legacy
`STDDAT.H` template-constructor spelling. These warnings remain visible.
Inputs were repository source and synthetic header-check data. No batcher
method was linked or executed by this target, and no retail input was used.

`nm -u` on the GCC object identifies the next link owners: `CQEFFECTS`,
`CQFLAGS`, `CQRENDERFLAGS`, `PIPE`, `VB_MANAGER`, `TheBigPointer`, the
production `BMPOOLJR` methods, `DACOM_Acquire`, `FDUMP`, `ICQImage::Assert`
and `ICQImage::Bomb`, and `__MATH_ENGINE()`. The next integration must resolve
these through their real component owners and exercise a production batch
submission. Merely compiling this object does not prove allocator ownership,
material/state behavior, GPU output or the first-scene milestone. Native
Windows Debug/Final rebuilds, cross-module ABI/layout checks, sanitizer
execution of the linked batcher, Windows image comparison and retail runtime
were not performed.

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

### ICQBatch tick declaration closure (2026-09-23)

The next App-side `RenderBatcher.cpp` compile blocker is its `ICQBatch` contract.
`CQBatch.h` depended on prior includes for `IDAComponent`, `D3DPRIMITIVETYPE`,
`IRP_VERTEXBUFFERHANDLE` and `U16`, and exposed the MSVC-only spelling
`unsigned __int64` in `GetTicks`. The production caller obtains `ICQBatch`
through `QueryInterface` in `Conquest.cpp`; the implementation and interface
must therefore retain the same 64-bit return type. `CQBatch.h` now includes
its own prerequisites, and both its declaration and the App-side implementation
use the existing `U64` alias. On Windows, `TYPEDEFS.H` defines `U64` as
`unsigned __int64`; the vtable return width and calling convention are unchanged
by source inspection. The existing header probe now includes `CQBatch.h` before
`RenderBatcher.h` and asserts the return width.

On Linux x86_64, `cmake --preset linux-gcc-debug` and
`cmake --preset linux-clang-debug` configured, both
`cmake --build --preset ... --target frontierx_render_batcher_header_probe -j2`
commands compiled and linked, and each corresponding
`ctest --preset ... -R portability.render_batcher_header --output-on-failure`
executed one passing check. Inputs were repository-owned source declarations.
Both full Linux preset builds completed, and CTest executed 32 passing checks
plus one GPU check skipped because the runtime was unavailable.
An explicit `g++ -std=c++17 -fsyntax-only` check of the App-side
`Code/App/Src/RenderBatcher.cpp` with its App and library include directories
still stops at `Code/App/Src/Include/pch.h:29` because `<windows.h>` is absent
on Linux. This identifies the next production translation-unit boundary; the
header result does not imply the implementation compiles.
This is compile and automated-execution evidence for the interface closure,
not execution of the App-side batcher. The latter still needs its PCH, globals,
trace, effect and buffer-manager dependencies closed. Native Windows Debug/Final
builds, ABI checks, retail data, GPU execution and image comparison remain open.

### First App-side translation-unit dependencies (2026-09-23)

The next check asked whether the production App-side translation unit could
pass its first include boundary on Linux x86_64. `RenderBatcher.cpp` now uses
its Windows PCH only on Windows and names the on-disk `Globals.h` exactly. The
Windows PCH remains in use on Windows. `Globals.h` includes the production
`TManager.h`, which had wrong-case `DACOM.h`/`Pixel.h` includes and MSVC-only
`__stdcall` declarations. `TManager.h` now uses `DACOM.H`/`pixel.h` and the
existing `COMAPI` macro. Source review of `DACOM.H` shows `COMAPI` expands to
`__stdcall` under MSVC, preserving the declared Windows calling convention.
These declarations cross module ABI boundaries; native Windows rebuilds and
ABI checks remain required.

`TManager.h` compiled alone via `g++` and `clang++` with
`-std=c++17 -fsyntax-only -x c++ -ICode/Libs/Include`. The existing
`frontierx_render_batcher_header_probe` now includes `TManager.h` before the
other production interfaces; both `linux-gcc-debug` and `linux-clang-debug`
target builds and focused `portability.render_batcher_header` CTests passed.
Inputs were repository-owned source declarations only. This is compile and
automated-execution evidence for a dependency, not batcher behavior.

The direct GCC `-fsyntax-only` check of `Code/App/Src/RenderBatcher.cpp`, with
App and Libs include directories, now reaches `CQTrace.h:46` and fails because
its `..\\CQImage.h` include is a Windows path. Independently including
`Globals.h` under GCC produces errors at its Windows `__declspec` declarations.
Both are verified compile blockers. The App-side object, actual batching,
GPU scene, retail runtime and Windows comparison remain unverified.

### Production material bounds review (2026-09-23)

Source review of the real `MeshRender.cpp` caller found that material hints
cross the `ICQBatch::CreateMaterial`/`ActivateMaterial` boundary. The hint
encodes one of three pool indices in its top two bits. `ActivateMaterial`
previously indexed `bm_pools[pool]` without checking that the decoded index
was below three. It now returns false for an invalid pool, preserving the
method's existing failed-activation result. The current `MeshRender.cpp`
caller checks its initial `U32(-1)` hint before activation; this guard protects
the public interface from an invalid returned or caller-supplied hint. No
runtime failure on that caller was observed.

The same owner had a concrete bounds-order defect while scanning a full
material pool: `materials[i].is_free()` was evaluated before
`i < materials_max_cnt`. When every slot is occupied, the next loop test
reads one past the array. The bounds term now evaluates first. The existing
full-pool eviction path remains in place.

These two corrections are source-review evidence only. The App-side
translation unit does not yet compile on Linux, so neither branch was
executed under a native compiler or sanitizer. The Linux header and renderer
probes do not cover these methods. Native Windows Debug/Final rebuilds,
material-pool runtime tests, cross-module ABI validation, retail behavior and
Linux batcher execution remain required.

The source dependency inventory also narrows `RenderBatcher.cpp`'s direct
use of `Globals.h` to `PIPE`, `BATCH`, `VB_MANAGER`, `DACOM` and the
`CQFLAGS.b3DEnabled` assertion; `HEAP` comes from `HeapObj.h`. This is a
source-review aid for the next interface extraction, not evidence that the
global owner or its module ABI is portable.

### MyVertex and diagnostic declaration closure (2026-09-23)

The App-side compile moved past its previous `CQTrace.h` path error after
replacing the Windows separator in its `CQImage.h` include. It then found two
wrong-case includes in `RenderBatcher.cpp` (`TComponent.h`, `STDDAT.H`) and
the Windows separator in `MyVertex.h`'s `RPUL/PrimitiveBuilder.h` include.
The paths now match the files on disk. The Windows include targets are the
same files by source inspection.

The real `MyVertex.h` dependency also imports the `CQImage.h` diagnostic
interface. `CQImage.h` now includes its own `FDump.h` type owner and the
existing native legacy-window declarations on non-Windows, and uses
`COMAPI`/`DACOM_CDECL` for its declarations. Its Windows import/export macros
are preserved; `MyVertex.h` omits Windows import decoration only on Linux.
The `CQTrace.h` callback declaration uses the same portable calling-convention
macro already used by `FDump.h`. None of these changes supplies a native
`ICQImage` implementation or changes assertion behavior.

The existing `frontierx_render_batcher_header_probe` now compiles a second
translation unit that includes production `MyVertex.h` and asserts the
32-byte `Vertex2` stride expected by the native RPVERTEX2 CPU adapter. A
separate translation unit is necessary because the original probe deliberately
includes `FDump.h` before `RenderBatcher.h`, whereas `CQTrace.h` defines its
application error enum before importing `FDump.h`. The first combined build
failed at `CQTrace.h`'s explicit duplicate-enum error; the split preserves
both source include orders without weakening that guard. Under
`linux-gcc-debug` and `linux-clang-debug`, the two translation units compiled,
linked and passed the existing focused CTest. Standalone `CQImage.h` and
`MyVertex.h` syntax checks also passed under both GCC and Clang. Inputs were
repository-owned declarations only; this is compile evidence, not diagnostic
or GPU runtime evidence.

Both full Linux preset builds completed after this change. Each full CTest
preset ran 32 passing checks and skipped `portability.sdl3_gpu` because its
runtime was unavailable. The central build matrix records this total.

The App-side `RenderBatcher.cpp` syntax check now parses past these headers
and reports many remaining errors, led by MSVC-only token-pasting in active
`CQTrace` macros, Windows export/type declarations in `Globals.h`, and the
Windows DLL lifecycle at the end of the translation unit. No `RenderBatcher`
object file was produced. Native Windows Debug/Final builds, cross-module ABI,
sanitizer execution, GPU rendering, retail runtime and image comparison remain
unperformed for this change.

### Active trace format and globals inventory (2026-09-23)

The `CQTRACE10` calls in the real App-side `RenderBatcher.cpp` were blocked by
MSVC-only `##` token pasting between string literals in `CQTrace.h`. The active
trace, bomb and error format macros now use standard adjacent string literals;
the stringized assertion formats use the same syntax. This preserves the
intended prefix, supplied format and newline without changing the
`FINAL_RELEASE` silence branch or removing any assertion. Windows Debug and
Final rebuilds remain the required parity check.

The existing `frontierx_render_batcher_header_probe` now invokes the real
`CQTRACE10` macro through an injected `FDUMP` callback. On Linux x86_64 these
commands compiled, linked and executed the focused check under both compilers:

```text
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_header_probe -j2
ctest --preset linux-gcc-debug -R portability.render_batcher_header --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_header_probe -j2
ctest --preset linux-clang-debug -R portability.render_batcher_header --output-on-failure
```

Both executions passed: the captured call had `ERR_GENERAL`, `SEV_TRACE_1` and
the expected file/line-prefixed diagnostic text. Direct GCC and Clang
`-fsyntax-only` checks of a `CQTRACE10` call also passed. These are
automated and compile results on repository-owned inputs. The callback was
supplied by the test; no native production diagnostic service was exercised.

A separate diagnostic syntax check of `Globals.h` with command-line-only
MSVC decoration overrides and `LegacyWindowTypes.h` confirms that the owner
also needs Windows types including `DWORD`, `HIMC`, `COLORREF`, `GUID` and
exception-filter declarations, plus `WINAPI` functions. Those command-line
overrides are not part of a product build and do not constitute a portable
Globals implementation. The App-side `RenderBatcher.cpp` still cannot
compile to an object on Linux. Native Windows Debug/Final, ABI, sanitizer,
GPU, retail and image-comparison checks remain open.

### Active assertion declaration path (2026-09-23)

The actual `RenderBatcher.cpp` uses `CQASSERT` throughout draw, buffer and
material ownership. Its active macro used MSVC inline assembly directly and
passed the const `__FILE__` string to `ICQImage::Assert`'s existing `void *`
parameter. `CQTrace.h` now routes a requested break through a small inline
diagnostic function: MSVC retains `int 3`, and the non-MSVC path calls
`std::abort()`. The assertion macro explicitly casts the file string without
changing the exported `ICQImage::Assert` signature. The predicate and handler
call remain active in Debug; the existing `FINAL_RELEASE` branch remains
unchanged.

Direct `g++` and `clang++` `-std=c++17 -fsyntax-only` checks of active
`CQASSERT` calls passed. The existing
`frontierx_render_batcher_header_probe` injects a local test implementation of
`ICQImage::Assert` and verifies that one true predicate does not call the
handler, while one false predicate evaluates once and calls the handler once.
The handler returns false, matching the legacy non-break path. Both
`linux-gcc-debug` and `linux-clang-debug` target builds and focused CTests
passed. Repository-owned source declarations and synthetic predicates were
used; this is automated execution of the macro contract, not a native
production diagnostic service. A separate temporary C++ caller that returned
true from `ICQImage::Assert` compiled against the same header with `g++` and
`clang++`; both subprocesses terminated with `SIGABRT` (core dumps disabled).
This verifies the non-MSVC requested-break branch on this host, not the
Windows `int 3` path or production diagnostic wiring.

Source review of `Globals.h` shows the batcher needs a few render globals,
but the header also declares Windows UI, VFX, exception, network and module
entry points. A diagnostic-only GCC syntax check, with temporary command-line
decoration overrides, still found missing Windows declaration types and VFX
types. No narrow replacement or duplicate `CQGlobalFlags` layout was added:
the real `Globals` owner and module ABI must be established before the
App-side `RenderBatcher.cpp` can be compiled and linked natively. Windows
Debug/Final rebuilds, ABI checks, batcher execution, retail runtime, GPU
rendering and image comparison remain open.

### Production BMPOOLJR arena owner (2026-09-23)

`RenderBatcher.cpp` creates `BMPOOLJR` objects for vertex and index payloads,
then uses their returned offsets and `pool` pointers while building queued
draws. The supported Windows `ZBatcher.vcxproj` compiles the real
`Code/App/Src/BMPoolJr.cpp`; the earlier Linux header probe never compiled or
executed that implementation. The falsifiable blocker was whether its arena
allocation, copying, bounds and destruction could run safely on Linux. The
result determines whether this owner can join the eventual native ZBatcher
closure; it does not decide the renderer backend.

Source review found `BMPOOLJR::pool` is assigned only to views into the
`TheBigBuffer` arena (the old `new[]` path is commented out), while its
destructor called `delete[] pool`. The destructor now drops its non-owning
view without freeing arena storage. The owner no longer includes Windows on
Linux; exact-case `STDDAT.H`, `DACOM.H` and `rendpipeline.h` includes and a
header guard close its direct translation-unit dependencies. Allocation
checks occupied bytes, requested bytes, pool size and arena position before
addition or copying. An exact-fit allocation at the arena end succeeds; an
oversized or full-arena request returns the existing `U32(-1)` failure value
without changing the owner. Copies use `memmove` because arena regions can
overlap after the batcher resets its shared arena cursor. The used-count
setter rejects a count beyond the allocated span even if assertions are
disabled. `reallocate` checks its copy bounds before moving bytes.

The new `frontierx_bmpooljr_probe` builds `Code/App/Src/BMPoolJr.cpp` itself,
not a replicated allocator. It exercises initial allocation, growth and
byte preservation, valid and invalid used-count rollback, reallocation,
exact-end allocation, full-arena rejection, overflow rejection and destruction
while the pool still points into the arena. A test-only `ICQImage::Assert`
handler satisfies the production diagnostic interface. Commands executed on
Linux x86_64:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_bmpooljr_probe -j2
ctest --preset linux-gcc-debug -R portability.bmpooljr --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_bmpooljr_probe -j2
ctest --preset linux-clang-debug -R portability.bmpooljr --output-on-failure
```

Both focused GCC 16.2.1 and Clang 22.1.8 checks compiled, linked and passed.
A separate Clang `-fsanitize=address,undefined -fno-omit-frame-pointer`
build of the same production source and test passed. The first sanitizer run
was stopped by LeakSanitizer's ptrace incompatibility on this host; rerunning
with `ASAN_OPTIONS=detect_leaks=0` executed the AddressSanitizer and
UndefinedBehaviorSanitizer checks successfully. Leak detection is therefore
not claimed. Inputs were repository-owned synthetic arena operations; no
retail files were read. This is production-owner compilation and synthetic
execution, not full `RenderBatcher` execution or a game scene.

`BMPOOLJR` is a runtime-only owner inside ZBatcher, with no disk or wire
layout. The Windows DLL still needs native Debug/Final rebuilds, ownership
regression and cross-module ABI checks. The App-side `RenderBatcher.cpp`, its
`Globals.h` dependency, actual queued draws, GPU integration, lawful retail
runtime and Windows image comparison remain open.

### Vertex-buffer descriptor width and manager declaration (2026-09-23)

The actual `RenderBatcher.cpp` owner calls `IVertexBufferManager` to acquire,
release and remove vertex buffers. Source review of that interface and its
`VertexBufferDesc` prerequisite found a 64-bit blocker in a neighboring real
caller: `TriMeshArchetype.cpp` allocates `U32[]` index arrays and passes them
through `VertexBufferItemDesc::indices`, which was declared `unsigned long*`.
On Linux LP64, `unsigned long` is eight bytes while `U32` is four. The
descriptor's size, stride, count, format and vertex-count members likewise
used `unsigned long`. They are now `U32`, and the production
`TriMesh/VertexBufferDescUtil.h` copy helper allocates and copies index arrays
in `U32` units. On supported Win32, `U32` remains an alias for 32-bit
`unsigned long`, so these field declarations and the helper's element width
retain their Windows representation by source review. This descriptor contains
runtime pointers and is shared across renderer DLL boundaries; it is not a
disk or wire schema. Native Windows ABI/layout validation remains necessary.

The existing `frontierx_render_batcher_header_probe` now includes the real
`IVertexBufferManager.h` and checks five scalar widths and the index-pointer
type at compile time. The falsifiable question is whether that declaration
matches the `U32` arrays from the real mesh caller on Linux. It does; no new
probe or parallel manager was introduced. Commands executed on Linux x86_64:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_render_batcher_header_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_batcher_header$' --output-on-failure
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_render_batcher_header_probe -j2
ctest --preset linux-clang-debug -R '^portability.render_batcher_header$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
```

GCC 16.2.1 and Clang 22.1.8 both compiled and ran the focused CTest. Both
incremental full preset builds succeeded. Inputs were
repository-owned declarations and the existing synthetic diagnostic fixture.
This is compile and focused automated-execution evidence for the interface.
The changed `TriMesh/VertexBufferDescUtil.h` copy helper has only source-review
evidence. A direct GCC syntax check of that helper's header failed at the existing
`IMaterial.h` include of `Engine.h`, whose on-disk spelling is `ENGINE.H`.
The Windows `VertexBufferManager.cpp` still requires Win32/DACOM. Neither that
manager implementation nor `RenderBatcher.cpp` was built or executed by this
change. Native Windows Debug/Final builds, cross-module ABI/layout checks,
execution of the TriMesh helper, GPU draws, retail runtime and image comparison
remain open.

The adjacent production `CopyVertexBufferDesc.cpp` had wrong-case `DACOM.h` and
`FDUMP.h` includes and an unused `<d3dx9.h>` include. It uses the local
`D3DVERTEX` in `FVF.h` and the real `Vector` from `3dmath.h`; these owners are
now included directly. Four FVF constants used by these copy paths
are present in the native-only `da_d3dtypes.h` branch with their D3D9 numeric
values (`0x00e`, `0x010`, `0xf00`, `8`); the supported Windows header branch still gets
them from the D3D9 SDK. The decision check was whether the real copier could
compile and copy a matching array-of-structures format without D3DX9. The
existing CTest target now compiles that production source and calls it with
two repository-owned synthetic XYZ/diffuse/UV vertices, comparing all 48
output bytes with the input. A second call checks indexed
position/normal/color/UV structure-of-arrays data with independently ordered
`U32` indices and compares both 36-byte destination vertices. Source review
found that path copied and advanced by three `Vector` objects for each single
position and normal. It now copies and advances by exactly `sizeof(Vector)`,
preventing destination overwrite on that path. Both cases passed under GCC and
Clang after both preset
configure/build sequences above. A direct Clang AddressSanitizer and
UndefinedBehaviorSanitizer build of the same three translation units ran the
same check successfully with `ASAN_OPTIONS=detect_leaks=0`; leak detection was
disabled because this host's ptrace environment prevents LeakSanitizer from
running reliably. The sanitizer command was:

```text
clang++ -std=c++17 -fsanitize=address,undefined -fno-omit-frame-pointer -ICode/App/Src -ICode/App/Src/Include -ICode/Libs/Include Code/Libs/Src/VertexBufferManager/CopyVertexBufferDesc.cpp Tools/portability/render_batcher_header_probe.cpp Tools/portability/render_batcher_myvertex_header.cpp -o /tmp/frontierx-vertex-buffer-copy-sanitized
ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-vertex-buffer-copy-sanitized
```

Both full GCC and Clang preset builds passed; in each full
CTest run, 33 checks executed and passed and the GPU smoke was skipped because
its runtime was unavailable. The central build matrix records the total.

Only the equal-format AOS and indexed PN/color/UV copy branches were executed
by that copier check. Other indexed and non-indexed structure-of-arrays paths,
destination capacity and source-index validity, Windows Debug/Final,
cross-module ABI, GPU drawing, retail runtime and image comparison remain
unverified for the copier.

### Production vertex-buffer manager lifecycle (2026-09-23)

The next production owner is `Code/Libs/Src/VertexBufferManager/VertexBufferManager.cpp`.
Its real caller, `Code/App/Src/RenderBatcher.cpp`, acquires buffers for direct
draws and releases them after unlocking and drawing. The falsifiable blocker
was whether this manager could use the existing native `IRenderPipeline`
buffer owner through its unchanged `IVertexBufferManager` interface. The
header probe could compile the interface, but could not execute allocation,
reuse, locking or teardown. The focused CMake target therefore compiles the
production manager and copier and creates the manager through DACOM against
`NativeRenderPipeline`; it does not provide a parallel manager.

The non-Windows registration and initialization path uses the existing
component factory and deterministic default buffer counts without Win32
profile APIs. The Windows profile parser and DLL registration remain in their
Windows branch. The manager now retains a buffer when pipeline destruction
fails, removes only successfully destroyed buffers from the available list,
rejects unknown or duplicate releases, honors exact-format selection, accepts
an exactly sized reusable buffer, bounds rounded allocation counts and
destroys a new buffer when its initial lock fails. `add_vertex_buffer` rejects
calls before initialization or after cleanup and rejects zero or counts above
the pipeline's signed `int` limit before dereferencing the pipeline. This
runtime manager owns process-local handles and pointers; no persistent or wire
layout changes were made. Its public interface crosses renderer module
boundaries, so native Windows ABI validation is still required.

Executed on Linux x86_64 with GCC 16.2.1 and Clang 22.1.8:

```text
cmake --build --preset linux-gcc-debug --target frontierx_vertex_buffer_manager_probe -j2
ctest --preset linux-gcc-debug -R '^portability.vertex_buffer_manager$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_vertex_buffer_manager_probe -j2
ctest --preset linux-clang-debug -R '^portability.vertex_buffer_manager$' --output-on-failure
cmake --build --preset linux-gcc-debug -j2
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j2
ctest --preset linux-clang-debug --output-on-failure
```

Both focused CTests passed. The process exercised DACOM factory creation,
pre-initialization rejection, native initialization, invalid-count rejection,
no-create failure without output mutation, preallocation and exact-size
acquisition, locked teardown failure without losing ownership, unlock and
release, duplicate/unknown release rejection, rounded dynamic allocation,
successful teardown, stale-handle rejection and post-cleanup rejection. Inputs
were repository-owned synthetic buffer requests; no retail media was read.
Both full Linux matrices built and ran 34 passing CTests each; the one SDL GPU
test was skipped because this host exposed no GPU runtime. That skip is not
GPU execution evidence. The exact tests above establish manager behavior only
against the native CPU buffer owner. At the time of this manager check, the
App-side `RenderBatcher.cpp` did not compile on Linux: with its App `DInclude`
path supplied, the direct syntax check reached Windows `__declspec` globals
and many later Win32 and D3D declarations. The subsequent App-side object
compilation is recorded above. Actual queued draws, texture/material mapping, sanitizer
execution for this manager target, native Windows Debug/Final rebuilds and
cross-module ABI checks, retail runtime and Windows image comparison remain
open.

On 2026-09-27, the same production source was moved into the shared
`frontierx_vertex_buffer_manager_owner` object target and linked into the
GPU shell. Native registration now uses the supported Windows aggregate
factory shape; the existing lifecycle probe requires its populated inner
component and still executes acquisition/release against the real native
pipeline under GCC, Clang and focused Clang ASan/UBSan. A generated
eight-service system profile loads this owner through the App container;
the scene initializes it after pipeline binding and cleans it up before
unbinding. Visible GCC, Clang and focused sanitizer scenes preserve the
legacy PolyMesh checksum `3422507570980372643` on generated inputs.
The full App `RenderBatcher` buffer-acquisition branch has not yet been
executed through this profile owner. Startup composition details are in
[GAME_SYSTEM_STARTUP.md](GAME_SYSTEM_STARTUP.md).
The first full preset rerun exposed an existing StateMaterial fixture that
queried the factory's outer pointer as though it were standalone. That
fixture and the SDL GPU material fixture now supply and query the
aggregate inner pointer. Focused StateMaterial checks pass under GCC,
Clang and Clang ASan/UBSan; visible SDL GPU checks pass under both native
compilers. Both complete Debug presets pass 42 executing checks and skip
the two command-sandbox GPU checks.

## Native queued pool slice (2026-09-25)

The next integration blocker was `RenderBatcher::set_state(RPR_BATCH, 1)`: it snapshots every
legacy D3D texture-stage slot, while the native primitive implements six stage-zero combine
fields and the disabled stage-one color/alpha operations. Before this change, the snapshot
returned `GR_NOT_IMPLEMENTED` and left batching disabled. The production batcher now checks a
single replay predicate when taking, comparing and applying texture-stage snapshots. On Linux it
uses only the implemented native slots; on Windows the predicate accepts every slot, preserving
the original D3D9 loops. Unsupported native setters still fail explicitly in the primitive.
No persistent structure, vtable or serialized layout changed. The real caller is the
`RenderBatcher` pool path selected by `RPR_STATE_ID` and `RPR_BATCH`; the existing linked probe and
shell were extended because they already own its DACOM and renderer boundary.

The linked `frontierx_render_batcher_runtime_probe` now provides the native state and
texture-stage callbacks, enables `RPR_BATCH`, sets a nonzero state ID and identity modelview,
submits the repository U16-indexed fixture, and proves that the submission count stays unchanged
until `flush`. In the second lifecycle it queues the fixture twice; one flush then submits a
14-vertex, 18-index primitive with the second index set rebased by seven vertices. The callback
checks the final index values and separately counts the merged submission. The process reports
six submissions total, two immediate and one pooled per lifecycle. The first pooled draw uses the
opaque pool; the second enables alpha blending, selects the alpha-depth pool and merges two indexed
primitives. This is synthetic CPU runtime evidence. It uses state flags zero; material replay
combinations, unsorted alpha, delay stages and queue vertex-buffer paths remain unexecuted.

The native shell uses the alpha-depth production pool with the same repository geometry and RGBA8
texture. Its source-reviewed `render_depth_sort` currently delegates to `render_no_sort`, so this
does not establish depth ordering. It reads back the immediate indexed draw and the later pooled
draw, requiring exact checksum and changed-pixel equality. Visible Wayland/Vulkan runs on this
host with GCC 16.2.1,
Clang 22.1.8 and a focused Clang ASan/UBSan build each reported 17 presented frames, 2,496 changed
pixels and pooled checksum `7951318278861886233`. The sanitizer CPU probe also submitted six
draws without a finding; LeakSanitizer was disabled for this host's ptrace environment. The
synthetic data root contained empty required directories, not retail media. Commands actually run:

```text
cmake --build --preset linux-gcc-debug --target frontierx_native_shell frontierx_render_batcher_runtime_probe -j2
cmake --build --preset linux-clang-debug --target frontierx_native_shell frontierx_render_batcher_runtime_probe -j2
ctest --preset linux-gcc-debug -R '^portability.render_batcher_runtime$' --output-on-failure
ctest --preset linux-clang-debug -R '^portability.render_batcher_runtime$' --output-on-failure
cmake -S . -B /tmp/frontierx-pooled-asan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined -DFRONTIERX_BUILD_SDL3=ON -DFRONTIERX_BUILD_SDL3_GPU=ON
cmake --build /tmp/frontierx-pooled-asan --target frontierx_render_batcher_runtime_probe frontierx_native_shell -j2
ASAN_OPTIONS=detect_leaks=0 /tmp/frontierx-pooled-asan/frontierx_render_batcher_runtime_probe
env XDG_CONFIG_HOME=/tmp/frontierx-pooled-shell-settings out/build/linux-gcc-debug/frontierx_native_shell --data-root /tmp/frontierx-pooled-shell-data --smoke-ms 250 --render-smoke out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv
env XDG_CONFIG_HOME=/tmp/frontierx-pooled-shell-settings out/build/linux-clang-debug/frontierx_native_shell --data-root /tmp/frontierx-pooled-shell-data --smoke-ms 250 --render-smoke out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-clang-debug/generated/shaders/sdl3_gpu_probe.frag.spv
env ASAN_OPTIONS=detect_leaks=0 XDG_CONFIG_HOME=/tmp/frontierx-pooled-shell-settings /tmp/frontierx-pooled-asan/frontierx_native_shell --data-root /tmp/frontierx-pooled-shell-data --smoke-ms 250 --render-smoke out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.vert.spv out/build/linux-gcc-debug/generated/shaders/sdl3_gpu_probe.frag.spv
```

The shell commands ran outside the command sandbox against the local Wayland display. The first
sanitizer shell attempt used shader paths under the focused build, where no shader target had run;
it exited before rendering with `could not open shader`. The recorded successful sanitizer command
uses the already generated repository shaders from the GCC preset. Both complete Linux preset
builds and CTest runs also passed as recorded in [BUILD_GRAPH.md](BUILD_GRAPH.md); their GPU CTest
entry skipped in the command sandbox and supplies no GPU runtime evidence.

The supported VS2022 Win32 Debug and Final no-deploy builds were rerun after this source change;
both compiled `RenderBatcher.cpp`, linked `ZBatcher.dll` and produced the game and 18 campaign
modules. The fresh Final same-build ABI check satisfied all 153 `Mission.dll` and two `Trim.dll`
imports requested by those campaign modules. See
[WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md) for commands and limits. This slice is
not Milestone 5 acceptance: Windows D3D9 fixture capture and cross-platform image-tolerance
comparison, retail runtime, production global/diagnostic ownership, material-library loading,
and a native game scene remain open.

## Native Windows build follow-up (2026-09-25)

The supported VS2022 Win32 Debug and Final no-deploy wrappers now rebuild
`ZBatcher.dll` as part of the complete game and campaign build. Native Windows
compilation exposed the Win32 `max` macro at two `std::max` calls and a distinct
`U32` (`unsigned long`) versus `maxDelay` (`unsigned int`) template deduction.
The calls now shield `std::max` from the macro and convert the state operand to
the existing 32-bit `maxDelay` type; the global variable, state layout and
submission order are unchanged. The exported `CQFLAGS` variable keeps its name
and bitfields, while its C++ type is `CQGlobalFlags` to avoid the Win32
`GlobalFlags` function name. Its 8-byte layout and the 4-byte render-flag
layout are compile-time checked by the Win32 build.

After the final batcher change, GCC 16.2.1 and Clang 22.1.8 rebuilt
`frontierx_render_batcher_runtime_probe` and
`frontierx_render_batcher_header_probe`; both focused CTests passed under each
compiler. These are synthetic Linux checks. The Windows result is compilation
and linking, with no game execution, cross-module retail ABI comparison or
Windows fixture capture. Commands and outputs are in
[WINDOWS_BUILD_VALIDATION.md](WINDOWS_BUILD_VALIDATION.md).
