# Windows build declarations and next portable slice

Evidence: source review and automated inventory, 2026-09-13. No MSBuild execution or retail run.

This build-declaration view is complemented by the 2026-09-16 lexical
[platform touchpoint inventory](PLATFORM_INVENTORY.md), which groups Windows, compiler,
pointer-width, graphics, audio, and video candidates by source owner. Neither inventory evaluates
MSBuild imports or establishes a native game target.

Native matrix status, 2026-09-24: the checked-in GCC 16.2.1 and Clang 22.1.8 debug build presets
completed. Their CTest presets each discovered 38 tests, executed 37 successfully and skipped only
`portability.sdl3_gpu` because this sandbox exposed no graphical/GPU runtime. The focused render
submission and pipeline tests executed in both matrices; the skipped GPU test is not runtime
evidence. No Windows, retail or native game execution was performed in this matrix. Separate
required visible GCC and Clang probe and shell runs exercised the current shared scene through
Wayland/Vulkan. The 2026-09-23 visible shell runs additionally traversed the production App-side
`RenderBatcher` direct and U16-indexed branches under GCC, Clang and focused Clang ASan/UBSan; their separate GPU
evidence is recorded in the focused batcher, renderer and shell documents. The separate indexed
RPVERTEX2 GPU probe was also executed visibly under GCC, Clang and focused
Clang ASan/UBSan, with distinct readbacks for its two second-UV inputs. The current visible probe
also preserved the last scene after a rejected oversize upload and reproduced its readback across
two renderer initialization cycles. The latest visible probe and shell additionally bind two
repository RGBA8 textures by runtime `U32` handle through the existing renderer boundary and the
production batcher, observe different images and recover the original readback. The preset CTest
GPU entry still skips in the command sandbox.
The new `frontierx_state_material_probe` compiles and executes production
`StateMaterial` through its DACOM factory and a cached indexed native draw in
both presets. Its focused CPU and sanitizer evidence is in
[STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md). Separate
visible GCC, Clang and focused sanitizer Vulkan runs checked its material-driven
indexed draw and the native texture-copy slot with equal alternate/copy readback
checksums; see [STATE_MATERIAL_PORTABILITY.md](STATE_MATERIAL_PORTABILITY.md) and
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

## Reproduce

From the repository root, using Python 3 (standard library only):

```text
python3 Tools/portability/build_inventory.py
PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s Tools/portability -p 'test_build_inventory.py' -v
```

Both commands passed on Linux. The first writes JSON to stdout; redirect diagnostic output outside
source control if needed. Five synthetic tests cover solution edges, conditional declarations,
reference cycles, missing references, unresolved project paths and references outside the root.

The scanner starts with Conquest.sln, ConquestCampaign.sln and the literal engine project list in
Build-Conquest.ps1. It follows ProjectReference declarations recursively. It preserves conditions,
source items, include/library directories, additional link inputs, imports, custom commands and
outputs. Cycles do not cause infinite traversal; each project appears once.

This is a union across configurations, not an evaluated MSBuild graph. Excluded source items remain
counted. Imported props/targets, macro expansion, command execution and implicit library edges
are not evaluated. Directory.Build.props and environment-specific SDK imports still need explicit
review when translating a target. A successful scan does not prove that a source path exists with
exact case, that a project builds, or that a linked DLL was rebuilt.

## Measured closure

The supported entry points reach **29 projects**, **12 distinct ProjectReference edges** and
**8 solution dependency edges**. The closure contains five application projects, eighteen campaign
projects and six library projects. StringTable is not in this explicit closure; its existing TinyXML
ownership does not prove that the supported wrapper rebuilds it.

| Library in explicit closure | Declared source items | Boundary found by source review |
|---|---:|---|
| COMHeap | 1 | Allocation boundary; preserve existing CRT/DACOM ownership |
| DACOM | 5 | Component loading, heaps and profile parser; separate services before porting |
| MathLib | 1 | MathLib.cpp acquires I3DMathEngine through DACOM; the provider is a separate runtime component |
| RPUL | 3 | Win32 device enumeration, render interfaces and text utilities |
| MeshManager | 4 | Link declarations include DACOM, MathLib, COMHeap and D3DX9 |
| D3DRenderPipe | 5 | Renderer and its dependent engine libraries; unsuitable as first isolated service |

Globals declares only one ClCompile source item but also custom generation steps for Data.cpp and
sfxdata.xmf. A source-count ranking would therefore hide both generation and runtime dependencies.

## Selected next candidate: profile parsing core

The next engine-owned slice to investigate is the in-memory parsing core of
Code/Libs/Src/DACOM/ProfileParser.cpp. It has concrete startup consumers in DACOM.CPP and
Code/App/Src/Conquest.cpp and an existing IProfileParser boundary. Its section-marking and
navigation core now has a shared header and Linux probe (2026-09-13); key lookup, output copying
and an owning portable reader are also shared. Section/key callback traversal is now shared by the
native reader and production DACOM adapter. Runtime section offsets use pointer-width encoded
handles, and the public profile interface header compiles on the portable path. Windows regression
and the Win32 I/O/heap owner closure remain unported.

Before extracting it:

1. Characterize section/key matching, line endings, comments, duplicate entries, missing keys and
   output-buffer truncation from the existing implementation with synthetic input tests. Section
   and key enumeration, early callback stop, final unterminated values, and the production callback
   adapter now have Linux synthetic/source-regression coverage.
2. Separate byte parsing from CreateFile/ReadFile, DACOM interface registration and heap ownership.
3. Section handles are classified as runtime byte offsets in a pointer-width carrier and use the
   shared encoded-handle helper. They are not native object pointers or persistent fields.
4. Link the actual extracted implementation into a Linux test consumer. Preserve the Windows
   adapter and require native Windows regression/ABI validation before accepting that integration.

No replacement parser, generic configuration framework, math-policy change or new dependency was
introduced. Focused profile buffer, document, and public callback paths pass AddressSanitizer and
UndefinedBehaviorSanitizer. Detailed contracts and remaining gates are in
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

## IFF build-edge evidence

The 2026-09-17 pointer-width review found that `DOSFile.vcxproj` lists `IFF.cpp` but marks it
`ExcludedFromBuild` in every one of its six current Win32 configurations. Therefore neither the
documented Windows DOSFile build nor the Linux CMake graph currently compiles that implementation.
Its pointer-width source migration has isolated contract tests, but production acceptance requires
first proving a retail consumer and then adding an explicit compile/behavior target. This excluded
edge must not be described as validated by an ordinary DOSFile rebuild. See
[IFF_PORTABILITY.md](IFF_PORTABILITY.md).

## Math provider follow-up

The 2026-09-16 follow-up established that the 402 concentrated inline-assembly candidates belong
only to the optional legacy K62/3DNow provider. The standard `x86math.vcxproj` compiles scalar
`x86math.cpp` alone. It is not reached by the current `Build-Conquest.ps1` roots even though the
reachable `MathLib` accessor requires a registered provider at runtime. An isolated portable scalar
core and CTest now cover that operation surface without changing the Windows ABI. See
[native math-backend readiness](MATH_BACKEND.md) for the evidence, limitations and adapter gates.

## Parallel platform slice: SDL3 events

Source review on 2026-09-16 identified two concrete consumers for the native window boundary:
`IWindowManager::ServeMessageQueue` owns window state and queue service, while
`System.cpp::wndProc` forwards focus, keyboard, text and mouse messages to `IEventSystem`. The
isolated `frontierx_sdl3_platform` target now owns SDL video/window lifetime and produces SDL-free,
owning event values for those categories. It deliberately does not expose Windows messages or link
the DACOM/event-system graph, so the adapter can be characterized without changing the supported
Win32 path. The legacy-input probe links the otherwise separate targets and synchronously dispatches
compatibility records through the production `EventSystem` to a real DACOM subscriber; the
`frontierx_sdl3_event_pump` integration target now owns complete queue draining and persistent
adapter state, while keeping SDL out of the event-system target. Its injected batch test covers
foreign-window filtering plus ordered resize, Unicode and quit delivery. See
[SDL_EVENT_PUMP.md](SDL_EVENT_PUMP.md) and
[PORT_READINESS.md](PORT_READINESS.md) for evidence.

## Parallel renderer-candidate slice: SDL GPU

`frontierx_render_submission` is an SDL-free interface target containing the first validated
position/BGRA8/UV submission shape, topology and optional U16-index view. Its standalone probe tests
layout, bounds and primitive counts; a Direct3D-free adapter maps the stable primitive values and
exact `D3DFVF_RPVERTEX` byte shape into caller-owned typed storage. The SDL GPU probe is its first
backend consumer. The production `PrimitiveBuilder` now expands the fixture to a direct triangle
list, crosses the real `IRenderPipeline` vtable and registered
`frontierx_native_render_primitive` adapter, and supplies the exact data uploaded, presented and
read back by SDL GPU in the same process. The same owner validates the
equivalent U16-index submission, while the CPU probe additionally covers `RPVertex` layout, quad
expansion, callback failure propagation and explicit triangle-fan rejection. The GPU lifecycle is
owned by `frontierx_sdl3_gpu_renderer`; the native shell can select it explicitly and submit the
shared `frontierx_render_smoke_scene` through the same boundaries. Six depth/blend states and
checked CPU vertex-buffer draws are connected; the actual `RenderBatcher`, RPVERTEX2 GPU and
texture-stage/material paths remain unconnected. See
[RENDER_SUBMISSION_PORTABILITY.md](RENDER_SUBMISSION_PORTABILITY.md).

The optional `frontierx_sdl3_gpu_probe` target is gated by `FRONTIERX_BUILD_SDL3_GPU`, requires the
existing SDL3 option, and has no dependency on the game, D3D9 renderer or retail data. CMake invokes
the system `glslc` to compile two repository-owned GLSL test shaders into the
out-of-tree generated directory; the backend has no direct Vulkan-loader dependency. It creates
SDL's Vulkan device and window swapchain, builds a graphics pipeline, uploads seven vertices, nine
U16 indices and a 2-by-2 texture, samples with depth test/write and source-alpha blending enabled,
submits/presents twelve frames and recreates the depth target after an observed swapchain resize. It
also renders to a bounded 64-by-64 target, waits on a GPU
fence, downloads the pixels and validates clear/background/foreground structure. GCC and Clang
visible probe and integrated-shell runtime checks passed on 2026-09-21 with 2,496 changed pixels and
diagnostic checksum `7951318278861886233` for the shared scene on the local Vulkan driver.

This edge proves the local device/upload/draw/present lifecycle, representative depth/blend mapping
and its explicit shell composition, not a selected game backend or complete state/material
integration. Its registered test uses a transient non-focusable window and skip return code 77 when
a host has no graphical/GPU runtime; runtime-capable shader, pipeline, upload, or submission failures
remain failures. See
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

The supported App-side `RenderBatcher.cpp` compiles as
`frontierx_render_batcher_object` under both GCC and Clang. A focused linked
process executes its direct and U16-indexed draw branches against the native CPU submission
owner; the shell visibly presents an indexed fixture through that same batcher and observes bounded
GPU readback. Queued pool batching, production global/diagnostic ownership, texture/material
mapping and Windows comparison remain open. See
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

The public `WindowManager.h` declaration now compiles as part of the existing
`frontierx_dacom_interface_probe` under GCC and Clang. The non-Windows branch uses the shared
pointer-width legacy-message types and explicit 32-bit compatibility scalars.
`frontierx_native_window_manager` now supplies the separate SDL-backed DACOM owner, and
`frontierx_native_window_manager_probe` verifies its lifecycle through the real native manager. See
[WINDOW_MANAGER_PORTABILITY.md](WINDOW_MANAGER_PORTABILITY.md).

`frontierx_native_shell` is the first executable consumer of that integration target. It composes
the existing `frontierx_platform`, `frontierx_dacom_core`, production event owner and native window-manager library,
uses the headless checker's shared configuration/data-root owner, validates the required directory
shape, loads the shared transactional preference owner, creates the real `IUserDefaults`, event and
window services, persists the fixed-width settings schema, pumps through `IWindowManager`, and exits
on close, quit or a bounded smoke deadline. This is an application
lifecycle edge, not a game target or renderer edge. See [NATIVE_SHELL.md](NATIVE_SHELL.md).

`frontierx_native_user_defaults` includes the production application interface header but not the
Win32 implementation. Its probe exercises the unchanged vtable, live-default mapping, immediate
value/MRU transactions, schema restart and explicit unsupported UI behavior. The compiler-owned
legacy struct remains runtime-only; game-startup selection and Windows ABI validation remain open.

The adjacent `EventSys.cpp` source was then reviewed as the next owner. Its portable queue contract
is implemented in `frontierx_platform` and tested independently: callback-time posts remain in the
current FIFO dispatch batch, while `Peek` exposes the last completed batch. The production source
now also builds as `frontierx_event_system`; its real aggregate factory, DACOM maps, connection
point, direct send and queue execute through `frontierx_event_system_probe`. That probe uses the
real `ICOManager` interface provided by `frontierx_dacom_core`, including priority/fallback,
enumeration, references and shutdown. The EventSystem ELF has a measured shared-heap closure;
broader owner-specific heap evidence, the full
Hotkey module and native Windows validation remain open. See
[EVENT_SYSTEM_PORTABILITY.md](EVENT_SYSTEM_PORTABILITY.md) and
[DACOM_MANAGER_PORTABILITY.md](DACOM_MANAGER_PORTABILITY.md).

The measured four-field `MSG` consumer surface now has an isolated SDL compatibility adapter.
It retains pointer-width window/parameter storage, reconstructs only the numeric messages and bit
fields actually read by controls and hotkey code, and offers synchronous dispatch so temporary
message payloads cannot escape. It is linked only into the SDL platform target and probes; the
Windows Hotkey project and application graph remain unchanged.

## DACOM pointer-width follow-up

The 2026-09-17 audit migrated the public component-map, connection-point, resource, and dispatch
templates away from `U32` address arithmetic. A neutral helper target characterizes
multiple-inheritance adjustment, member-owner recovery, and the deterministic tokens used to keep
the established `IDAConnectionPoint` `U32` handle ABI without returning a node address. This
removes all five previously measured public-header pointer truncations.

The Linux graph now contains a native `ICOManager` core, production event/profile service owners,
and an explicit ELF component-module loader. The dynamic path and configuration-relative
`[Libraries]` bootstrap are wired into the native shell and proven with both a controllable fixture
and the production EventSystem ELF module. `frontierx_native_dacom_bootstrap` now supplies the real
process-global `DACOM_Acquire` symbol, registers both production profile factories once and owns the
manager used by the shell. Its registered process-local `IHeap` supplies the public bootstrap and
tracked allocation contract. The EventSystem ELF now uses that exact owner across `dlopen`; broader
module ownership and legacy COMHeap behavior remain open. The explicit DACOM
project still owns the supported Windows DLL loading and profile integration. The separate 32-bit MSVC automation dispatch is referenced only by four
legacy Viewer tool headers, not by component creation. The public `DACOM.H`, `TComponent.h`, and connection-point template layer now has a real GCC/Clang
executable probe, including an instantiated `DAComponent` map and connection container. It executes
point lookup, owner recovery, subscription enumeration, removal, and deterministic token reuse.
The next graph step is another game- or renderer-owned component with the same ELF heap/symbol proof,
native Windows rebuild and ABI/layout evidence. See
[DACOM_RUNTIME_PORTABILITY.md](DACOM_RUNTIME_PORTABILITY.md).

The graph also compiles production `ProfileParser.cpp` as `frontierx_profile_parser`. Its two real
factories and aggregate execute through the native manager, including typed buffer and native
transactional file initialization. The owning `ProfileDocument` is now a focused reusable target;
the manager uses its raw-line traversal for bounded file/string `[Libraries]` configuration rather
than aggregating the service parser. The global bootstrap registers both production factories;
writable INI behavior and cross-module proof for further owners remain open. See
[PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

`frontierx_native_heap` implements the unchanged `IHeap` surface for process-local native startup,
and `frontierx_native_dacom_bootstrap` registers it before the profile factories. Its probe covers
public initialization, allocation/reallocation, metadata, enumeration, foreign-pointer rejection
and repeated initialization. The production EventSystem ELF additionally proves aggregate and
queue-node ownership through the host heap, exact final accounting and unload. Descriptor capacity
is diagnostic, private COMHeap pass-through slots are unsupported, and further owners need their
own proof. See [NATIVE_HEAP.md](NATIVE_HEAP.md).

Both native EventSystem targets compile with `DA_HEAP_ENABLED`. The static library has an explicit
transitive dependency on `frontierx_native_heap`; its production probe and SDL integration acquire
that owner before factory creation and verify final block-count balance. The ELF imports the host's
exported `HEAP` instead of embedding a second allocator.

`frontierx_hotkey_event` now compiles the production `HKEvent.cpp` service with the same native
heap. `frontierx_hotkey_event_probe` registers its real factory, supplies fixed-width data through
the public `IFileSystem` vtable and executes keyboard, mouse, joystick, callback, malformed-input
and exact allocation-balance paths. `NativeUtfFileSystem` provides the bounded read-only adapter
used to parse both checked-in source hotkey tables. The probe also connects raw EventSystem input
to the hotkey service and its translated output back through the production queue. The native shell
offers this composition through explicit `--hotkeys FILE` and advances `ISystemComponent::Update`
after each window batch. This does not add `HkeyRec.cpp`, installed-resource policy, a complete
DOSFile/UTF owner or a game target. See
[HOTKEY_EVENT_PORTABILITY.md](HOTKEY_EVENT_PORTABILITY.md).

`frontierx_dynamic_library` supplies a move-only Linux ELF handle with local eager symbol
resolution. `NativeComponentManager` now layers explicit C-named registration and liveness entry
points over it, attributes registrations, reference-counts repeated exact-path adds and removes
factories before close. Its synthetic module probe covers failure, lookup, live-object refusal,
reload, configuration rollback and shutdown. The production EventSystem uses this path with
instance-counted unload gating and runs from the native shell; campaigns and further components do
not. Heap ownership and broader ABI contracts remain owner-specific. See
[DYNAMIC_LIBRARY_PORTABILITY.md](DYNAMIC_LIBRARY_PORTABILITY.md).

## Stack-local debug guard follow-up

Six guards shared by mission and object-wrapper owners previously depended on x86 `esp` and
32-bit `this` arithmetic. They now use the header-only `FrontierX/StackAddress.h` boundary, which
has concrete consumers in `MGroupRef.h`, `MPart.h`, and four `Objwatch.h` specializations. The
isolated target verifies the pointer-width, stack-direction-independent distance contract without
pulling an application owner into CMake.

This removes six pointer-truncation and inline-assembly candidates but does not make the mission or
Trim closure portable. Those owners still require their surrounding DACOM/Win32 dependencies and
native Windows ABI/runtime regression. See
[STACK_ADDRESS_PORTABILITY.md](STACK_ADDRESS_PORTABILITY.md).

## Runtime callback boundary follow-up

The remaining measured pointer truncations crossed four existing boundaries rather than one new
generic subsystem. ParticleEffect and Conquest now share the pointer-width `ParticleContext` for
the existing callback context. Trim and DAHOTKEY share pointer-width legacy-message aliases with
the SDL adapter. The Windows-owned RichEdit and DirectShow calls now use their documented
`DWORD_PTR` and `LONG_PTR` fields. The helper probes and source regressions remove the measured
object/address-cast category without adding any of these Windows modules to CMake.

The required next graph evidence is therefore native Windows cross-module validation followed by a
real Linux owner closure, not another isolated alias. The affected virtual interfaces, callbacks,
and synchronous payload lifetimes are detailed in
[RUNTIME_CONTEXT_PORTABILITY.md](RUNTIME_CONTEXT_PORTABILITY.md).

## Native render-pipeline vertex-buffer follow-up

The existing render-submission graph now includes `frontierx_native_render_pipeline` and the
deterministic `frontierx_render_pipeline_probe` CTest. The public `rendpipeline.h` declaration is
includable on Linux through fixed-width non-Windows declaration types, and the native owner derives
from the real `IRenderPipeline` interface. A default adapter returns `GR_NOT_IMPLEMENTED` for every
unported slot; implemented direct/state methods forward to the registered `IRenderPrimitive`. The
CPU target also models the
`RenderBatcher` lock/slice/draw pattern: its context-aware lookup obtains a caller-owned
vertex-buffer view, and the same two draw entry points used by production inspect that view's FVF,
validate the requested direct or U16-indexed slice, preserve slice-relative indices and forward
portable RPVertex-shaped vertices through the already tested `IRenderPrimitive` boundary. The
portable contract and adapter also handle direct and U16-indexed 32-byte
`D3DFVF_RPVERTEX2` slices with validated list and line-strip topologies.

`frontierx_native_render_primitive` now links that adapter and owns it when its native-only
descriptor supplies a context-aware buffer lookup. Its existing `IRenderPrimitive`
vertex-buffer methods therefore execute the same checked route instead of returning
`GR_NOT_IMPLEMENTED`. The adapter now also owns CPU buffers through production-shaped
create/lock/unlock/destroy methods. Nonzero monotonic `U32` tokens keep runtime addresses out of the
legacy handle field; lock state gates draw and destruction. The registered-owner regression creates,
fills and destroys one of these resources and routes its indexed draw through separate typed
RPVertex/RPVERTEX2 callbacks. This proves native resource ownership, owner construction, vtable
routing and shutdown. The regression additionally invokes the repository smoke scene through the
production `PrimitiveBuilder(IRenderPipeline *)` constructor and real pipeline vtable. It does not
execute the production game batcher.

On 2026-09-21, `cmake --preset`, `cmake --build --preset ... -j2` and `ctest --preset`
completed for both `linux-gcc-debug` (GCC 16.2.1) and `linux-clang-debug` (Clang 22.1.8)
on Linux x86_64. Each full preset executed 31 passing tests and skipped `portability.sdl3_gpu`;
the skip is not GPU runtime evidence. Both build presets now explicitly include the pipeline
probe. A fresh Clang ASan/UBSan build also passed the two focused renderer CPU checks.
Exact commands and remaining Windows/retail gates are in
[RENDER_SUBMISSION_PORTABILITY.md](RENDER_SUBMISSION_PORTABILITY.md).

The same registered owner now also links an SDL-free `RenderStateSnapshot` for six existing depth
and alpha-blend values. `set_render_state` validates a complete candidate and commits it only after
the backend callback succeeds; the focused submission regression covers set/get, unsupported and
invalid values, callback propagation and rollback. The SDL GPU callback maps the snapshot into a
replacement graphics pipeline, including all eight D3D9 depth comparisons and eleven directly
mappable blend factors. On 2026-09-21 both full presets again executed 31 passing tests and skipped
only the non-required GPU CTest. After the real pipeline vtable was connected, separate required
visible GCC and Clang GPU-probe and native-shell runs used Vulkan and again agreed on 2,496 changed
pixels with checksum `7951318278861886233`. Probe diagnostics identify
`primitivebuilder-native-irenderpipeline`, and both shells report the production builder via
`IRenderPipeline`. Those four runs are synthetic Linux hardware evidence; Windows, retail,
RPVERTEX2 GPU and actual `RenderBatcher` evidence remain open.

On 2026-09-23 the existing `frontierx_render_batcher_header_probe` gained a second
translation unit for the production `MyVertex.h`/`CQTrace.h`/`CQImage.h` include
order and the 32-byte `Vertex2` stride. The original `FDump.h`-first unit remains
separate because combining these orders triggers `CQTrace.h`'s duplicate error-kind
guard. GCC and Clang configured, built and executed this target and their full
presets. Neither translation unit compiles the App-side `RenderBatcher.cpp` object;
the exact remaining errors and validation limits are in
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

The graph now also builds the real App-side `BMPoolJr.cpp` in
`frontierx_bmpooljr_probe`, registered as `portability.bmpooljr`. Its synthetic
arena ownership, growth, bounds and teardown checks execute under both
compiler presets, with a separate Clang ASan/UBSan run. It remains an isolated
production owner; the full ZBatcher translation unit and game graph are not
linked. Focused commands and limitations are in
[RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

The existing `frontierx_render_batcher_header_probe` also compiles the real
`CopyVertexBufferDesc.cpp` owner and executes its equal-format AOS and indexed
PN/color/UV copies on
repository-owned synthetic vertices. GCC and Clang built and ran that path,
and a focused Clang ASan/UBSan execution passed. The broader vertex-buffer
manager and App-side ZBatcher objects remain outside the native graph; the
focused document records the exact commands and open branches.
