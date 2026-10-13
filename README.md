# FrontierX

Selected original gameplay hints, M1 (2026-10-11): normal SDL mouse hover in
Sol_Train now visibly shows original Cygnus/asteroid hints under GCC and Clang,
including resource switch/removal and renewed hover after source save/reload.
The original edge-scroll owner provides camera access. Optional
`--mission-hint-check` adds a six-second normal-hover observation after reload
and fails without source Cygnus draws; it requires `--mission-start` and an
external `--ui-font`, and injects no hint text or input. Builds, focused
checks and Clang ASan/UBSan passed. Native Windows Debug/Final and ABI remain
open. See [commands, provenance and limits](docs/NATIVE_SINGLEPLAYER_SLICE.md#original-gameplay-hintbox-checkpoint-m1-2026-10-11).
Older dated summaries below are historical.

Selected native alert smoke (2026-10-11): font-enabled missions can now run
original UnitComm alert text without `--skip-unit-alerts`. Explicit
`--mission-alert-smoke` invokes the original MScript caller once on the
selected Forger for a controlled rendering check; it requires unit voice,
Teletype and the external font. GCC/Clang visible checks passed movement and
source save/reload. Gameplay hints, map pings and natural combat alerts remain
open. See [commands, provenance and limits](docs/NATIVE_SINGLEPLAYER_SLICE.md#original-unitcomm-alert-rendering-checkpoint-2026-10-11).

Selected native mission text (2026-10-10): original Teletype now draws
training objectives in the running Linux scene, alongside normal movement
and unit voices. Configure with `-DFRONTIERX_BUILD_NATIVE_FONTS=ON` and pass
`--ui-font <external-install>/FONTS/bauhmib.ttf` instead of
`--skip-teletype-visuals`. This optional path requires FreeType >=2.14;
default builds do not. The external font and retail data remain outside the
repository. `-DFRONTIERX_TEST_FONT_FILE=<external-font>` enables original
text lifecycle validation in the existing shell CTest. See the
[bounded mission record](docs/NATIVE_SINGLEPLAYER_SLICE.md#original-in-window-mission-teletype-checkpoint-2026-10-10)
for the complete command and remaining exclusions. Full game and Windows
font parity remain open.

Native audio status (2026-09-27): the existing window-manager check now also
executes `IStreamer` PCM playback and notifications. Retail training speech
executes with the optional external shared-LGPL FFmpeg IAC decoder; see [the audio contract](docs/NATIVE_AUDIO_STREAMER.md).
This is not mission execution. For IAC, configure either Linux preset with
`-DFRONTIERX_BUILD_FFMPEG_AUDIO=ON -DCMAKE_PREFIX_PATH=/external/lgpl-prefix`
after building the reviewed dependency as documented there. The default stays
SDL-only and rejects IAC; GPL-enabled FFmpeg libraries are rejected at configure.
Existing build/test commands are unchanged.
The explicit `--skip-movies` option excludes full-screen cinematics for the
training checkpoint through the existing App flag; it preserves audio and
animation requirements. Startup then stops at `IParticleManager`.

FrontierX is a non-commercial effort to port *Conquest: Frontier Wars* to native 64-bit Linux
while preserving retail gameplay, data compatibility, and deterministic simulation behavior. It
starts from Revan67's Windows 11 retail-restoration work rather than from the uncorrected 2013
source snapshot.

The Linux port is in its bootstrap phase: native layout, TinyXML, platform timing, and data-root
inspection targets build, but no game or engine-module target exists yet. The working VS2022 Win32
build remains the behavioral and regression baseline while portable build, platform, and rendering
boundaries are introduced incrementally. See [docs/PROJECT.md](docs/PROJECT.md) for the project
charter and current architecture decisions.

FrontierX does not include or grant rights to missing retail media. Running the game requires a
lawfully obtained retail installation. This entire Program Derivative is non-commercial and is
subject to the exact, unmodified Fever Pitch Studios source license; see
[Conquest Source License.txt](Conquest%20Source%20License.txt), [LEGAL.md](LEGAL.md), and
[MODIFICATIONS.md](MODIFICATIONS.md).

## Upstream restoration baseline

This project restores the officially released **Conquest: Frontier Wars** development source so
the 2001 game can build and run on modern Windows with Visual Studio 2022.

The released code is not the exact retail source tree. It is a later development snapshot for the
unshipped sequel, commonly identified in the code as **CQ2** or **Conquest: Vyrium Uprising**.
Retail data and media are therefore being matched against a codebase containing changed binary
layouts, object IDs, renderer paths, and unfinished sequel features.

The project has two ordered goals:

1. Restore the game as closely as practical to shipped retail behavior.
2. Modernize the renderer, platform code, and supporting systems only after the retail baseline is
   stable and measurable.

Modification notice: this is a modified version of the Program. The modern build conversion,
retail-restoration work, diagnostics, and documentation are community modifications by Revan67,
principally dated 2022-12-25 and 2026-03-25 through 2026-08-30. The entire Program Derivative is
released solely under the exact terms of the Fever Pitch Studios source license. The FrontierX
Linux-port bootstrap and project instructions are community modifications by Michael2543 dated
2026-09-11. See
[MODIFICATIONS.md](MODIFICATIONS.md) for the prominent modification record and
[LEGAL.md](LEGAL.md) for the repository's compliance and interoperability policy.

## Current status

Status snapshot: **2026-09-19**. Windows runtime evidence was last verified on **2026-08-30**.

| Area | Status |
|---|---|
| Linux x86_64 layout probe | **Working** with CMake/Ninja, GCC, Clang, and CTest |
| Linux TinyXML support library | **Working** as an isolated static library with a linked behavior test |
| Portable timing library | **Working** with monotonic milliseconds, sleep, yield, and a linked behavior test |
| Linux data-root inspector | **Working** with exact-case diagnostics; retail installation validation pending |
| Native preference store | **Working in headless and shell startup** with XDG path, fixed-width schema, real `IUserDefaults`, immediate value/MRU writes and shutdown persistence; game/UI integration pending |
| Native production event-system owner | **Working statically and as a configured ELF component** with factory, connection, queue, liveness-gated unload and SDL shell/pump tests; game-loop integration remains pending |
| Native production profile-parser owner | **Working as a statically linked DACOM service target** with real factories, buffer/file parsing and callback tests; native manager reuses the owning raw-line parser for bootstrap |
| Native DACOM manager | **Working through process-global `DACOM_Acquire`** with a native `IHeap`, production profile factories, static/adapted ELF factories, priority, transactional `[Libraries]` configuration and liveness-gated unload; EventSystem cross-ELF heap ownership proven, broader modules pending |
| Native ELF loader boundary | **Working against synthetic and production EventSystem shared objects** with local eager lookup, explicit DACOM registration and safe-unload contract; broader component/campaign ABI integration pending |
| Production hotkey event owner | **Working in an isolated target and optional shell pipeline** with a read-only UTF adapter, both checked-in source tables, real EventSystem round trip, keyboard/mouse/joystick state tests and shared-heap ownership; packaging, SDL joystick and Windows parity pending |
| Native startup shell | **Working for synthetic lifecycle checks** with data-root validation, configured component bootstrap, real settings/event/window services and SDL pumping; game/renderer startup pending |
| Native rendering backend probe | **Working as an isolated SDL GPU/Vulkan experiment** with generated SPIR-V, vertex/texture upload, sampling, depth, resize-safe present and bounded pixel readback under GCC and Clang; real game data, Windows comparison, renderer integration and backend selection pending |
| VS2022 Win32 build | **Working** — Debug and Final rebuild successfully |
| Retail campaign DLLs | **Working** — all 18 build, verify, and deploy together |
| Startup and mission loading | **Working**, with repeated-launch soak still required |
| Ship movement and path completion | **Fixed and runtime-confirmed** |
| Ship model orientation and combat facing | **Fixed and runtime-confirmed** |
| Opening Mission 1 combat progression | **Fixed and runtime-confirmed** |
| HQ/platform construction crash | **Fixed and runtime-confirmed** |
| Combat renderer lighting crash | **Fixed and runtime-confirmed** |
| Completed platform textures | **Fixed and runtime-confirmed** |
| Explosion mesh splitting | **Stabilized**, broader combat soak required |
| Mission 1 final-wave progression | **Fix deployed**, runtime retest pending |
| Projectile and engine-light transforms | **Open** — effects are offset or fire along the wrong axis |
| Planet rendering | **Partial** — geometry works; diffuse/depth behavior remains wrong |
| Refinery/shipyard transient materials | **Open** — some active construction meshes sample wrong textures |
| Remaining retail schema differences | **Approximately 11**, down from 37 |
| UI parity | **Partial** — several layout and information-panel differences remain |
| Modern renderer/codebase | **Prototype only** — an SDL-free vertex/topology/index submission contract and isolated SDL GPU/Vulkan path work; production `IRenderPipeline` mapping, Windows comparison and backend selection remain open |

Mission 1 currently reaches and survives combat, advances to the Fabricator, constructs the HQ and
other available orbital structures, builds ships, and survives later combat waves. The immediate
test target is the final Tau Ceti wave: after its last Mantis ship dies, the mission should advance
to the beacon-recovery objective.

For a detailed working-state handoff, see [HANDOFF.md](HANDOFF.md). For acceptance testing, see
[RETAIL_TEST_MATRIX.md](RETAIL_TEST_MATRIX.md).

## What the source release does and does not provide

The source release contains enough material to rebuild the game executable, application DLLs,
engine modules, data-schema generator, and all 18 retail campaign modules. The project is not
blocked merely because the source came from a development branch.

What it does **not** provide is the complete set of retail media assets. A legitimate retail
installation is still required to run the rebuilt code because music, video, textures, meshes,
missions, and other shipped content are not duplicated in this repository.

The practical restoration problem is compatibility:

- Retail `.db` files must be parsed with retail-compatible structure layouts and enum values.
- Campaign DLLs and `Mission.dll` share a decorated C++ ABI and must be built/deployed together.
- CQ2 rendering paths sometimes interpret retail meshes and materials differently.
- Modern wrappers and worker threads expose races that were rarely encountered in the original
  single-threaded runtime environment.

Retail binaries, resources, and observed behavior are treated as the authority whenever the
development source disagrees with shipped data.

## Requirements

The Linux targets require Linux x86_64, CMake 3.25 or newer, Ninja, and either GCC or Clang. The
checked-in presets also require Python 3, `llvm-rc`, `pkg-config`, SDL3 3.4.16 or a compatible SDL3 package, and `glslc`
because they enable the SDL3 platform and isolated GPU probe. A core/headless configure can leave
both SDL3 options off; a window-only build can leave `FRONTIERX_BUILD_SDL3_GPU=OFF`. None of these
targets requires retail media or builds/runs the game.

The supported game build requires:

- Windows 10 or Windows 11 with 32-bit application support.
- Visual Studio 2022 with the **Desktop development with C++** workload.
- Python 3 with the Windows `py` launcher. It replaces the original 16-bit XMIFF build utility.
- DirectX SDK (June 2010), extracted beside the repository as `DXSDK`.
- DirectX 9 runtime components.
- A retail installation of Conquest: Frontier Wars for runtime media.
- MFC for Visual Studio 2022 only if building the separate map editor.

### DirectX SDK setup

Installing `DXSDK_Jun10.exe` often fails with error `S1023` on modern systems containing newer
Visual C++ redistributables. Do not remove system redistributables. Extract the SDK instead:

```text
7z x DXSDK_Jun10.exe -o"<repository-parent>" "DXSDK/Include/*" "DXSDK/Lib/*"
```

`Directory.Build.props` resolves the SDK at `..\DXSDK` relative to this repository. The important
files are `Include\d3dx9.h`, `Lib\x86\d3dx9.lib`, `d3dx9d.lib`, and `DxGuid.lib`.

## Build and run

### Linux layout probe

The current Linux targets check fundamental storage widths, the LP64 runtime model, retail mission
enum values, the production `MISSION_DATA_BIN` layout, the existing TinyXML parser as an isolated
static library, the first standard-C++ platform timing contract, and exact-case data-root paths.
Build and test them with GCC:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug
```

Run the independent Clang build with:

```text
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug
```

The authoritative dated matrix result is recorded in [docs/BUILD_GRAPH.md](docs/BUILD_GRAPH.md).
These sequences do not build `Conquest.exe`,
the complete engine-module graph, campaign modules, or a native Linux game executable. The modal UI frame clock
and progress-animation clock and sleep now use the portable contract, but the owning `Trim` module
still requires Windows validation. The vendored TinyXML snapshot has unresolved exact-version
provenance and is not approved for distribution; see [docs/DEPENDENCIES.md](docs/DEPENDENCIES.md).
The lexical [platform touchpoint inventory](docs/PLATFORM_INVENTORY.md) records the measured
Windows, compiler, pointer-width, renderer, audio, and video audit surface and the ordered
follow-up; its counts are source-review evidence, not build failures. Its pointer audit now reports
zero measured legacy-width DOSFile handle-index casts and zero high-confidence true object/address
truncations. IFF, UTF, and SharedUTF use the tested pointer-width token contract and explicit
serialized-call contexts, but native Windows validation remains pending; see
[docs/IFF_PORTABILITY.md](docs/IFF_PORTABILITY.md) and
[docs/UTF_RUNTIME_PORTABILITY.md](docs/UTF_RUNTIME_PORTABILITY.md).
The public DACOM templates now use pointer-width runtime offsets and deterministic non-pointer
connection tokens while retaining their `U32` public handle ABI. A native manager hosts the
production `EventSystem` owner and the synthetic-tested ELF module contract; explicit
configuration-driven loading, process-global acquisition and the native `IHeap` are wired to the native shell. The production EventSystem ELF allocates and frees through that host heap with hidden C++ symbols; broader module ownership and native Windows
ABI/runtime checks remain open. The real public `DACOM.H`/`TComponent.h` interface-map and
connection-point template layer also executes in a dedicated GCC/Clang probe. The public
`IWindowManager` contract now compiles with pointer-width native window/message tokens and
fixed-width legacy scalars without changing its Windows type path. A separate SDL-backed DACOM
factory now owns the native shell's window, callback pump, focus/cursor state and close-veto
lifecycle; game-loop placement and Windows behavioral parity remain open. See
[docs/WINDOW_MANAGER_PORTABILITY.md](docs/WINDOW_MANAGER_PORTABILITY.md). The x86 automation
dispatcher is limited to four legacy Viewer headers and is not part of component creation.
See [docs/DACOM_RUNTIME_PORTABILITY.md](docs/DACOM_RUNTIME_PORTABILITY.md) and
[docs/DACOM_MANAGER_PORTABILITY.md](docs/DACOM_MANAGER_PORTABILITY.md).
Six x86 `esp`-based debug guards in mission/object wrappers now use a pointer-width,
stack-direction-independent proximity helper without changing layouts or assertion limits; see
[docs/STACK_ADDRESS_PORTABILITY.md](docs/STACK_ADDRESS_PORTABILITY.md).
The remaining particle, RichEdit, DirectShow, and hotkey callback contexts now use pointer-width
runtime types while fixed-width gameplay and persistent IDs remain unchanged; see
[docs/RUNTIME_CONTEXT_PORTABILITY.md](docs/RUNTIME_CONTEXT_PORTABILITY.md).
The [native settings format and integration gates](docs/NATIVE_SETTINGS.md) document the XDG path,
fixed-width schema, value/MRU adapter, shared player/global lifecycle owner and shell-integrated real
`IUserDefaults` component. The legacy `USER_DEFAULTS` representation exists only as a live mapping;
no raw struct disk layout or complete game/UI integration is implied.

The SDL dependency is target-scoped. This separate check configured and built the headless target
without looking for SDL3:

```text
cmake -S . -B /tmp/frontierx-core-config -G Ninja \
  -DBUILD_TESTING=OFF -DFRONTIERX_BUILD_SDL3=OFF
cmake --build /tmp/frontierx-core-config --target frontierx_headless
```

Inspect a lawfully obtained external data directory without reading or copying file contents:

```text
./out/build/linux-gcc-debug/frontierx_data_root_probe /path/to/retail/Data
```

When no argument is supplied, the probe uses `FRONTIERX_DATA_ROOT` and then falls back to `Data` in
the current directory. The resulting Linux precedence is command-line path, environment variable,
then `Data`. This does not change the Windows game's existing registry-backed `DataPath` behavior.

The command returns zero when all seven startup-required directory names have exact case. Missing
or mismatched optional movie, music, SFX, speech, and mission-speech directories are reported but
do not block startup. The repository's development `Data` tree currently reports case mismatches
for four required and all five optional paths, plus missing `Profiles`, `Maps/Campaign`, and
`Maps/Multiplayer`. This is a source-tree check, not evidence about a retail installation.

### Windows game build

Use the supported root-level entry points:

```bat
build-debug.cmd
build-final.cmd
```

Both commands perform a full Win32 rebuild of:

- engine renderer modules and their dependencies;
- `Conquest.exe`, `Globals.dll`, `Mission.dll`, `Trim.dll`, and `ZBatcher.dll`;
- all 18 retail campaign modules: `SCRIPT01`–`SCRIPT16`, `Mantis_T`, and `Sol_T`.

The wrapper verifies required outputs, deploys them to the selected retail installation, and
compares SHA-256 hashes so a successful message cannot conceal a stale deployed DLL.

The default local destinations are:

- Debug: `D:\Games\GOG\Conquest Frontier Wars`
- Final: `D:\Games\GOG-Final\Conquest Frontier Wars`

Override the destination or disable deployment:

```powershell
.\Build-Conquest.ps1 -Configuration Debug -NoDeploy
.\Build-Conquest.ps1 -Configuration Final -DeployDirectory 'D:\Games\Another CFW Test'
```

Launch through the wrappers so the process starts in the retail installation directory:

```bat
run-debug.cmd
run-final.cmd
```

The working directory matters. Launching from an unrelated command prompt can make the game fail
to locate `Conquest.ini` even when the executable itself is valid.

### Solutions and configurations

- `Code/App/Src/Conquest.sln` builds the main application using `Debug | Win32` or
  `Final | Win32`.
- `Code/App/Src/Scripts/ConquestCampaign.sln` builds exactly the retail campaign set using
  `Debug | Win32` or `Release | Win32`.
- The root build wrapper maps campaign Debug to application Debug and campaign Release to
  application Final, preventing mixed import libraries.

Always use **Rebuild**, not an incremental Build. Legacy tracked intermediates, resource steps,
and precompiled-header behavior can otherwise make MSBuild reuse stale output. Building only
`Conquest.vcxproj` is also unsafe because it consumes sibling import libraries without rebuilding
the corresponding DLL projects.

Debug keeps `CQASSERT` and `CQBOMB` active; they intentionally execute `int 3`. Final defines
`FINAL_RELEASE` and is the closer retail-parity configuration. They are configurations, not Git
branches.

The application projects must remain on the static `/MT` runtime while DACOM/COMHeap owns the
game heap. Switching them to `/MDd` creates incompatible allocation paths.

## Restoration principles

### Retail first, modernization second

CQ2 additions are not automatically improvements. If an unshipped feature changes retail data
layout or behavior, the retail path is restored first. Modernization should later occur on a
separate line of work with parity tests available to expose regressions.

### Measure layout, not only size

A `sizeof` assertion cannot prove a structure is correct. Two opposite layout errors can cancel
and leave the final size unchanged. That exact failure broke ship movement: a 32-byte-short
mission-data block was masked by 32 bytes of unrelated CQ2 fields elsewhere in the structure.

Important layouts therefore use field-offset and end-of-structure invariants in addition to size
checks. The parser schema embedded in the shipped retail `Globals.dll` is the primary source for
archetype layout.

### Keep ABI-linked modules together

Campaign scripts import decorated C++ symbols from `Mission.dll` and `Trim.dll`. A symbol can keep
the same name while a referenced structure changes size, so successful linking alone is not proof
of ABI safety. Critical cross-module types have compile-time size guards, and the complete retail
campaign is rebuilt and deployed with the engine.

The current ABI audit accounts for 153 imported `Mission.dll` symbols and two imported `Trim.dll`
symbols. Re-runnable checks are under `Tools/abicheck`.

## Major fixes completed

### Retail object IDs and Mission 1 progression

The sequel source inserted new values throughout `M_OBJCLASS`, shifting every later retail object
ID. Retail Mantis frigates arrived as class 73 while the source compared them with class 90, so
destruction events could not update the intended campaign state. Retail values `0..110` are now
preserved exactly, sequel-only values follow the retail sentinel, and critical IDs have compile-
time guards.

Mission 1's later combat counter could also become stale or underflow when construction waves
overlapped the final attack. The final Tau Ceti step now derives the counter from live local Mantis
frigates/scout carriers and guards zero before decrementing it.

### Ship movement and model orientation

Retail stores the complete 72-byte `MISSION_DATA` inside ship archetypes. The development source
substituted a 40-byte `MISSION_DATA_BIN` and later masked the size deficit with fabricated rocking
fields. `dynamicsData` was consequently read from the wrong offset and could produce a negative
`maxAngVelocity`, forcing a nonzero turn every frame and preventing path completion.

The retail layout and offset guards are restored. Ships now move, complete paths, stop, face their
targets, and render level with the authored bow pointing forward. The visual model-axis correction
is applied through the renderer so the gameplay/physics transform remains planar.

### Explosion and debris stability

Explosion splitting previously reached mesh nodes without valid vertex bindings, wrote past
fixed-size split buffers on unsuitable meshes, and could destroy child engine instances twice when
debris expired. MeshInfo bindings are restored, incomplete/unsafe meshes are rejected before
splitting, ownership is normalized before destruction, and extreme angular debris velocity is
bounded.

The fixes have survived representative combat and multiple destroyed ships. Broader soak testing
is still required before treating the entire explosion system as closed.

### HQ/platform construction crash

A placement shadow could initialize a shared renderer array with fewer children than the completed
platform. Constructing the Mission 1 HQ then indexed beyond that array. Completed objects now grow
undersized shared arrays and bind every actual mesh child safely. HQ construction, all other
available structures, and ship production have completed successfully in runtime testing.

### Combat lighting crash

The D3D renderer stored four selected lights locally but searched for a key light using an enabled
count of up to eight. An out-of-range selection copied a full `D3DLIGHT9` over the stack frame. All
selection and key-light loops are now bounded to local/shader capacity, zero capacity cannot
underflow, and the enabled-light list is bounded in Final builds as well as Debug.

### Startup allocator race

Modern Direct3D wrappers may create worker threads while engine DLLs are attaching. Their CRT
bookkeeping can allocate through the shared DACOM heap concurrently. The main game heap now enables
DACOM's existing multithreaded path. A complete mission run has passed since the fix; repeated rapid
launch/exit testing remains on the acceptance list.

### Retail rendering path restoration

Completed orbital platforms now render through the retained retail engine-instance path, fixing
their black/white and unrelated striped materials. Planets use the same path and are visible again,
but their diffuse texture and depth interaction with the far side of the orbital ring remain open.

## Current priority queue

1. Retest the Mission 1 final-wave live-count fix through the beacon-recovery transition.
2. Correct the shared hardpoint/effect axis used by projectile direction and engine lights.
3. Restore planet diffuse material and depth/occlusion behavior.
4. Fix refinery and shipyard active-construction material binding.
5. Perform repeated startup testing and broader explosion/debris combat soak.
6. Resolve the remaining retail schema differences, led by `BT_FLAGSHIP_DATA`.
7. Restore retail cinematic paths and finish the smaller UI-parity issues.
8. Begin renderer/code modernization only after Priority 1 has a stable test baseline.

## Debugging and evidence

### Crash capture

`run-debug-capture.cmd` launches the Debug build with crash capture enabled. Local dumps are kept
under `CrashDumps` and intentionally ignored by Git because a single dump can be several gigabytes.
`Tools/analyze_minidump.py` provides a repeatable first-pass analysis.

When recording a crash, retain:

- build configuration and binary timestamps;
- mission/save and reproduction steps;
- call stack or minidump;
- the deployed binary hashes;
- a Time Travel Debugging trace when the reproduction is short enough.

### Time Travel Debugging

TTD is particularly useful for old stateful code where the visible failure occurs long after the
corrupting write. Useful lessons from this restoration:

- `L<n>` debugger counts are hexadecimal; use `L0n12` for twelve decimal elements.
- Prefer named fields (`dt Type address field`) over copied offsets.
- Query a full trace rather than logging only the first N calls, which usually samples mission
  loading instead of steady-state gameplay.
- Archive matching PDBs with traces; rebuilding makes old source-level symbol results unreliable.
- Do not interrupt large trace indexing. A partial index is normally unusable.

### Retail schema

The shipped retail `Globals.dll` contains a `PARSER` resource holding the authoritative `data.i`
schema. Compare it with generated `Code/App/Src/data.i`, but edit the corresponding headers under
`Code/App/DInclude`; `data.i` is generated output.

## Repository layout

```text
Code/
  App/
    DB/                 Development-source database files
    DInclude/           Shared data/ABI declarations and parser source of truth
    Src/                Main executable and application DLL sources
      Scripts/          All campaign sources and ConquestCampaign.sln
  Editor/               Separate MFC map editor
  Libs/
    Include/            Shared engine headers and compatibility headers
    Src/                DACOM, renderer, MeshManager, and other engine sources
    Static/             Engine import/static libraries
    ExplicitDLL/        Explicitly loaded runtime engine modules
    ImplicitDLL/        DACOM runtime

Tools/
  abicheck/             Campaign/Mission decorated-symbol checks
  analyze_minidump.py   Repeatable minidump summary tool
  read_gendata.py       Database/archetype inspection utility
  retail_gametypes.h    Retail structure-layout reference
  xmiff2/               Python replacement for the legacy XMIFF data compiler

Build-Conquest.ps1      Supported full build, verification, and deployment wrapper
HANDOFF.md              Detailed current engineering state
RETAIL_TEST_MATRIX.md   Retail-parity acceptance ledger
```

## Legal and compliance

The controlling license is the unmodified
[Public License for Conquest: Frontier Wars Program and Source Code](Conquest%20Source%20License.txt),
copyright Fever Pitch Studios, December 2013. This repository is a **Program Derivative**, is
non-commercial, and is conveyed as a whole solely under that license's exact terms. The license is
revocable and prohibits payment, donations, expenses, gratuities, usage fees, or any other
consideration in exchange for the Program, a Program Derivative, warranty, or support.

Every source or binary distribution must include, conspicuously and intact:

- `Conquest Source License.txt`, including its warranty and liability notices;
- [MODIFICATIONS.md](MODIFICATIONS.md), identifying the date and author of modifications; and
- a prominent statement that the entire Program Derivative and every recipient are subject to the
  exact same license terms.

[LEGAL.md](LEGAL.md) contains the full release checklist, third-party-material policy, and the
narrow rules for compatibility research. In short, retail comparison is limited to functional
facts needed for interoperability with a lawfully obtained copy. Do not commit or distribute retail
executables, extracted retail media, disassembly/decompiler output, access-control keys, or
circumvention tools. The project neither authorizes nor requires bypassing copy protection.

`LEGAL.md`, this summary, and the modification record are notices and project procedures only. They
do not amend, replace, or add terms to the Fever Pitch Studios license. If any summary conflicts
with the license, the license controls. This is a good-faith compliance policy, not legal advice or
a guarantee that every historical file has sufficient third-party redistribution rights.

## Contributing

Contributions are welcome when they follow the project order of operations:

- establish retail evidence before changing behavior;
- explain binary-layout or ABI reasoning explicitly;
- add or update a reproducible test where practical;
- rebuild Debug and Final plus the complete retail campaign set;
- do not add retail media, crash dumps, local tool installations, or other large generated files;
- identify the author and date of modifications in [MODIFICATIONS.md](MODIFICATIONS.md);
- document the provenance and license of every new third-party dependency or binary; and
- keep all work non-commercial and follow [LEGAL.md](LEGAL.md) and the controlling source license.

## Acknowledgements

- Fever Pitch Studios for releasing the development source.
- The original Conquest: Frontier Wars development and publishing teams.
- The Conquest modding community for preserving documentation, tools, and knowledge of the game.

## Port readiness follow-up

See [Linux port readiness](docs/PORT_READINESS.md) for the first proposed layout-probe milestone,
initial blocker inventory, ABI tool parameters and pending validation. The layout probe and isolated
TinyXML library are implemented; no native game build exists. The ABI scripts are parameterized and
must be run only on native Windows with VS2022 `dumpbin`; container, Wine, and Linux PowerShell
execution is intentionally blocked.

## Build declaration inventory

Run `python3 Tools/portability/build_inventory.py` from the repository root to inspect the
supported Windows entry points without invoking Windows tools. See [build graph evidence](docs/BUILD_GRAPH.md)
for scope, test commands, measured dependencies and production profile-parser evidence. This
read-only inventory does not evaluate MSBuild or replace native Windows ABI validation.

## Profile buffer core (2026-09-13)

The Linux presets build `frontierx_profile_buffer_probe`; its focused GCC and Clang checks pass.
`FrontierX/ProfileBuffer.h` owns section marking, line/section navigation,
key lookup/copying, and section/key callback traversal shared with the production DACOM parser.
Runtime section offsets now use the pointer-width encoded-handle contract instead of `U32`
decoding. This remains an internal borrowed-buffer contract with sentinel padding, not a complete
portable DACOM implementation or game target. Native Windows Debug/Final rebuilds, callback
comparison and ABI checks remain required; see
[docs/PROFILE_CALLBACK_PORTABILITY.md](docs/PROFILE_CALLBACK_PORTABILITY.md).

### Owning native profile reader (2026-09-13)

The existing GCC and Clang presets build and execute `frontierx_profile_document_probe`.
`FrontierX/ProfileDocument.h` provides owning text/file loading, ordered section names
and key lookup through the shared profile-buffer core. Native startup configuration currently
accepts ASCII without NUL bytes, up to 1 MiB, with locale-independent ASCII case folding.
This policy is limited to the new reader; Windows DACOM comparison behavior is unchanged.
No native game startup or retail-configuration compatibility is claimed by the synthetic tests.

The production `ProfileParser.cpp` now also builds as `frontierx_profile_parser`. A native probe
registers both real factories through `NativeComponentManager`, creates the aggregate, queries
`IProfileParser` and `IProfileParser2`, and covers buffer/file loading, callbacks, duplicate
sections, case-insensitive lookup and transactional failures. Windows retains its existing file
branch. Native `SetINIConfig` now uses shared raw-line parsing for an explicit `[Libraries]` list;
broader cross-ELF heap ownership and native Windows parity remain open; the native heap is
documented in [docs/NATIVE_HEAP.md](docs/NATIVE_HEAP.md);
see [the production parser evidence](docs/PROFILE_CALLBACK_PORTABILITY.md).

## Native headless startup check (2026-09-13)

The GCC and Clang presets build `frontierx_headless`; its focused process test contains ten
scenarios. Python 3 is required when BUILD_TESTING is enabled.
The following help command was executed:

```text
out/build/linux-gcc-debug/frontierx_headless --help
```

The executable accepts `--data-root DIRECTORY`, optional `--config FILE`, and optional
`--preferences FILE`. The config is new native configuration, not retail Conquest.ini: use
`[FrontierX]` and `DataRoot=path` with a final LF newline. Relative config paths resolve against
the config file's directory. Data-root selection order is CLI, `FRONTIERX_DATA_ROOT`, explicit
config, then `Data` relative to the working directory. An explicitly supplied invalid
configuration always fails, even when another path overrides it. ASCII/control-character
restrictions of the native reader apply; paths are not silently trimmed.

Preference selection is an explicit CLI path, then an absolute `XDG_CONFIG_HOME`, then
`$HOME/.config/frontierx/preferences.dat`. Relative XDG and HOME values are ignored. A missing
default file represents first-run empty preferences; a missing explicit file, invalid format, I/O
failure, or unavailable base directory returns exit code 6. Diagnostics report source, status and
entry count without printing the resolved path.

Exit codes: 0 means checks passed (or help), 1 invalid arguments, 2 invalid configuration,
3 unusable required data directories, 4 unexpected startup-check failure, 5 invalid UTF metadata
or entry, and 6 unusable preferences. Optional media directories and a missing default preference
file do not fail. Reports are deterministic and do not print the user's absolute installation or
settings path. No media contents, campaign, renderer or game startup are validated. Synthetic
process tests cover data/config precedence, relative configuration paths, XDG/default/explicit
preference selection, valid and invalid preference files, malformed options/configuration, missing
roots, case mismatches, optional media, and UTF metadata/entry validation.

### UTF metadata header check (2026-09-13)

`frontierx_headless` additionally accepts `--metadata FILE`, an explicit path resolved from the
working directory. After directory checks, it reads only the 56-byte UTF 1.01 header and validates
signature, version, 44-byte directory stride, aligned directory size, metadata ranges, overlap and
free-entry offset. Fields are decoded as little-endian fixed-width words; native structs are never
mapped onto disk bytes. Invalid metadata returns exit code 5. Omitting the option leaves this check
unperformed. Success does not validate directory entries, names, payloads or GenData archetypes.

The focused headless suite covers synthetic UTF header mutations and missing/truncated metadata.
No retail files were read.
The format evidence is the source declarations in DOSFile/BaseUTF.h, header validation in
UTFDMan.cpp and the GenData.cpp loadTypesData caller. The UTF inspector is not yet the DOSFile
backend replacement. Its summary is diagnostic only and does not certify a compatible installation.

### UTF directory validation (2026-09-13)

The metadata check now reads directory and used-name tables (combined limit 64 MiB), walks the
reachable tree iteratively, rejects repeated/cyclic/misaligned links, validates NUL-terminated
name references and bounds file allocation/used extents against the container size. The free-entry
chain is checked separately. Source review of UTFDMan.cpp corrected the earlier assumption:
the header's first free-entry offset is absolute; its subsequent dwNext links are directory-relative.
Synthetic tests include a valid free chain and a cyclic one. Unreachable slots and free payload-space
lists are not certified; payload contents/decompression and archetype schemas remain unchecked.

GCC and Clang passed the focused headless scenarios with synthetic UTF trees, invalid links, names
and payload ranges. No retail installation was inspected. This
extends the read-only inspector; it does not yet replace the Windows DOSFile implementation.

The headless checker now accepts `--metadata FILE --entry VIRTUAL_PATH` to read a bounded UTF
entry into memory and report its byte count and diagnostic checksum (2026-09-13). Both preset
builds passed the focused check. No schema or retail compatibility is implied; see the UTF entry-reading
section of the port-readiness plan for limits and outstanding work.

The Linux presets also build the isolated `frontierx_sdl3_platform` library and
`frontierx_sdl3_window_probe` against the locally installed SDL3 3.4.16 system package. The
backend keeps SDL types out of its public event values and translates resize, focus, close,
keyboard, owned UTF-8 text, mouse motion/button/wheel and quit events. GCC and Clang pass the
focused hidden-window and injected-event checks under
`SDL_VIDEODRIVER=dummy`.

A real-display smoke test was also executed on 2026-09-16:

```text
out/build/linux-gcc-debug/frontierx_sdl3_window_probe --visible-smoke
```

It opened a visible window through SDL's Wayland backend, pumped and translated events for 250 ms,
then closed cleanly. This does not start the game, prove Windows-message input parity, render a
frame, read retail data or approve SDL3 for distribution. A separate synthetic probe now connects
compatibility records to the production `IEventSystem`; the visible smoke does not.

The presets build a reusable SDL GPU owner and `frontierx_sdl3_gpu_probe`. Original GLSL is compiled
offline to out-of-tree SPIR-V with `glslc`; the probe and native shell can feed the same repository
scene from production `PrimitiveBuilder` through registered native `IRenderPrimitive`. Visible GCC,
Clang and focused sanitizer shell runs used Wayland/Vulkan, presented the nine-vertex textured/depth
fixture and produced the same bounded 64-by-64 structural readback. This is synthetic backend and
integration evidence, not a selected game renderer, D3D9 state adapter or Windows comparison. See
[the renderer-backend prototype record](docs/RENDERER_BACKEND_PROTOTYPE.md).

`SdlLegacyInputAdapter` now converts the measured input surface into owning compatibility records:
numeric message IDs, a pointer-width window token, `wParam` and `lParam`. Tests cover virtual keys,
set-1 scan values used by the hotkey path, right-modifier/Alt/repeat/release bits, mouse masks and
coordinates, button move/down/move and double-click sequences, wheel delta, resize, and UTF-8 to
UTF-16 character delivery. A synchronous dispatch helper keeps the temporary window-message record
alive for the complete callback and prevents it from being mistaken for an asynchronously owned
queue item. This is an adapter around observed consumers, not a binary reimplementation of `MSG`.

`frontierx_sdl3_event_pump` now combines the window source, persistent adapter state and production
`IEventSystem` in a separate integration target. Its injected dummy-driver test drains one complete
batch, filters a foreign-window resize, and preserves owned resize, Unicode and quit delivery order.
It reports close/quit observations but does not own application shutdown or frame cadence. The exact
contract and remaining lifecycle gates are documented in
[the native SDL event-pump note](docs/SDL_EVENT_PUMP.md).

The portable platform library also contains an `EventQueue` core derived from source review of
the existing `IEventSystem` implementation. Its probe verifies FIFO posting, callback-time posts
within the same update, last-completed-batch `Peek` behavior, empty updates and non-owning native
pointer identity. The production `EventSystem` now builds in a separate native target and its real
DACOM aggregate factory, connection point, queue and direct-send behavior pass GCC, Clang and
focused sanitizers. The native SDL pump now dispatches complete synthetic batches through it,
including immediate Unicode values, but no application/game loop is connected; see
[the event-system evidence and remaining gates](docs/EVENT_SYSTEM_PORTABILITY.md).
The probe now runs through the platform-neutral
[native component manager](docs/DACOM_MANAGER_PORTABILITY.md), not a test-only manager fake.
The separate [dynamic-library boundary](docs/DYNAMIC_LIBRARY_PORTABILITY.md) now supports explicit
native DACOM module registration, registration attribution, balanced repeated loads and
liveness-gated reverse unload through the native manager. Its evidence is a synthetic ELF fixture;
it is not yet a production component or campaign loader.

The production DAHOTKEY hotkey-event processor now builds as `frontierx_hotkey_event`. Its real
factory loads synthetic and both checked-in source hotkey tables through the public `IFileSystem`
contract and a bounded read-only native UTF adapter. The probe executes keyboard, mouse and
joystick matching plus a complete EventSystem input/queued-symbol round trip. The native shell can
compose the same path with explicit `--hotkeys FILE` and now advances the production EventSystem
owner after every window batch. The loader bounds fixed-width counts, record strides and text slots
and returns every aggregate allocation to the shared heap on success and failure. Installed
resource selection, an SDL joystick source and native Windows parity remain open; see
[the hotkey-event portability record](docs/HOTKEY_EVENT_PORTABILITY.md).

`frontierx_native_shell` is the first executable to compose shared native configuration/data-root
selection, the native component
manager, production App system container loading EventSystem and SDL WindowManager
from synthetic `[System]` entries by default or a bounded external
`--system-profile FILE`, and, in the GPU build, the production
`GameSystem::startup` caller with an empty synthetic `[Engine]` section,
window/text input, platform timing and complete event
pump in one lifecycle. Its normal mode opens a visible window until close/quit; bounded
`--smoke-ms ... --hidden` mode supports deterministic process tests. When the GPU option is built,
explicit `--render-smoke VERTEX_SPV FRAGMENT_SPV` owns the tested Vulkan lifecycle and renders the
shared fixture. An explicit `--game-scene MATERIAL_UTF MESH_UTF VERTEX_SPV FRAGMENT_SPV
EXTENDED_SPV GAME_SPV` keeps a visible production GameSystem-owned Engine/RenderMgr/PolyMesh scene alive from supplied
UTF files until close; `--smoke-ms` bounds a repeated-frame run. The CQ2 branch also calls the App
`MeshRender::Render` path with the supported `VertexColor.fx` material. This scene does not start gameplay.
The production App system container now owns the scene's `ITManager`,
`IMaterialManager` and `TextureLibrary`. A supplied profile can create its
`IRenderPrimitive` aggregate, which binds to the GPU after startup and
unbinds after scene teardown. A supplied profile can also create the `IRenderPipeline` aggregate, which
binds after GPU startup. The checked-in `Conquest.ini` next stops at its
`IViewConstructor` entry. Empty synthetic
directories are not retail compatibility evidence. See
[the native startup-shell contract](docs/NATIVE_SHELL.md).
The container and GameSystem startup boundaries are documented in
[the App system-container record](docs/APP_SYSTEM_CONTAINER.md) and
[the GameSystem startup record](docs/GAME_SYSTEM_STARTUP.md).

### Portable scalar math core

The native CMake graph now includes a standard-C++ scalar math core for the operations behind the
existing `I3DMathEngine` boundary. It covers matrix and affine transforms, quaternion operations,
list transforms and the historical lookup-table inverse square root, with FMA contraction disabled
for the target. Source review separated this path from the optional K62/3DNow provider that owns
most measured inline assembly. The core passes synthetic GCC and Clang tests, but is not yet wired
to DACOM or the game and has not been compared against a native Windows numerical oracle. See
[native math-backend readiness](docs/MATH_BACKEND.md).

### Portable mesh ray geometry

The production MeshManager hit-test path now uses a small header-only ray/AABB and ray/triangle
implementation rather than D3DX9 calls and object-layout casts. Its native probe covers scaled and
parallel directions, inside/away box rays, both triangle windings, barycentric results, edges and
behind-origin rejection. The remaining D3DX9 surface is classified by feature in
[D3DX9_PORT.md](docs/D3DX9_PORT.md). A native Windows comparison is still required for exact D3DX
edge tolerances before parity is accepted.

Native training entry checkpoint (2026-10-12): optional --mission-campaign
uses the original Solar training selector and briefing with --mission-start
and Sol_Train.dmission. Visible Clang initial entry executes; other campaigns,
full training and winning return are unaccepted. Commands, requirements and
limits: [native singleplayer evidence](docs/NATIVE_SINGLEPLAYER_SLICE.md).

M3 automation is under development, not accepted. The manual
[controlled CI workflow](.github/workflows/m3.yml) requires protected Linux/GCC,
Linux/Clang and native Windows runners with lawfully external inputs. Explicit
host/GPU jobs and native rebuild/ABI checks remain separate from simulation
acceptance. Real sound-lifetime, declared-state and original Save/Reload fixtures
are being extended, alongside private SSH semantic comparison and synthetic
production-RandomGen vectors. Missing evidence, required skips, live-event
repeatability and incomplete state coverage remain failures. The retained
`FRONTIERX_M3_PRIVATE_ROOT` must be outside checkout, runner temp and retail
roots. Detailed evidence and remaining blockers are in
[the focused M3 record](docs/M3_SIMULATION_COMPARISON.md).
