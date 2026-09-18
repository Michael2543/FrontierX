# Windows build declarations and next portable slice

Evidence: source review and automated inventory, 2026-09-13. No MSBuild execution or retail run.

This build-declaration view is complemented by the 2026-09-16 lexical
[platform touchpoint inventory](PLATFORM_INVENTORY.md), which groups Windows, compiler,
pointer-width, graphics, audio, and video candidates by source owner. Neither inventory evaluates
MSBuild imports or establishes a native game target.

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
introduced. The current GCC and Clang matrices pass twenty-four CTests; focused profile buffer,
document, and public callback paths also pass AddressSanitizer and UndefinedBehaviorSanitizer.
Detailed contracts and remaining gates are in
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
foreign-window filtering plus ordered resize, Unicode and quit delivery. The application cadence
and lifecycle remain unconnected. See [SDL_EVENT_PUMP.md](SDL_EVENT_PUMP.md) and
[PORT_READINESS.md](PORT_READINESS.md) for evidence.

`frontierx_native_shell` is the first executable consumer of that integration target. It composes
the existing `frontierx_platform`, `frontierx_dacom_core`, production event owner and SDL libraries,
uses the headless checker's shared configuration/data-root owner, validates the required directory
shape, initializes the real event aggregate, owns window/text
input lifetime and exits on close, quit or a bounded smoke deadline. This is an application
lifecycle edge, not a game target or renderer edge. See [NATIVE_SHELL.md](NATIVE_SHELL.md).

The adjacent `EventSys.cpp` source was then reviewed as the next owner. Its portable queue contract
is implemented in `frontierx_platform` and tested independently: callback-time posts remain in the
current FIFO dispatch batch, while `Peek` exposes the last completed batch. The production source
now also builds as `frontierx_event_system`; its real aggregate factory, DACOM maps, connection
point, direct send and queue execute through `frontierx_event_system_probe`. That probe uses the
real `ICOManager` interface provided by `frontierx_dacom_core`, including priority/fallback,
enumeration, references and shutdown. Dynamic loading, profile bootstrap, the DA heap, the full
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
and an explicit ELF component-module loader. The dynamic path is proven with a synthetic module;
profile-driven discovery, a production module and heap policy remain open. The explicit DACOM
project still owns the supported Windows DLL loading and profile integration. The separate 32-bit MSVC automation dispatch is referenced only by four
legacy Viewer tool headers, not by component creation. The public `DACOM.H`, `TComponent.h`, and connection-point template layer now has a real GCC/Clang
executable probe, including an instantiated `DAComponent` map and connection container. It executes
point lookup, owner recovery, subscription enumeration, removal, and deterministic token reuse.
The next graph step is a profile-driven loader/heap closure followed by another game- or
renderer-owned component, native Windows rebuild and ABI/layout evidence. See
[DACOM_RUNTIME_PORTABILITY.md](DACOM_RUNTIME_PORTABILITY.md).

The graph also compiles production `ProfileParser.cpp` as `frontierx_profile_parser`. Its two real
factories and aggregate execute through the native manager, including typed buffer and native
transactional file initialization. This adds a second production DACOM service closure; it does
not implement `NativeComponentManager::SetINIConfig`, automatic Libraries-section loading or a
cross-module heap. See [PROFILE_CALLBACK_PORTABILITY.md](PROFILE_CALLBACK_PORTABILITY.md).

`frontierx_dynamic_library` supplies a move-only Linux ELF handle with local eager symbol
resolution. `NativeComponentManager` now layers explicit C-named registration and liveness entry
points over it, attributes registrations, reference-counts repeated exact-path adds and removes
factories before close. Its synthetic module probe covers failure, lookup, live-object refusal,
reload and shutdown. No production component or campaign uses this path; profile/search ordering,
heap ownership and broader ABI contracts remain owner-specific. See
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
