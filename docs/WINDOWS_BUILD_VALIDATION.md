## M3 supplementary validation addendum (2026-10-13)

The supplied SSH Windows11/VS2022 Community machine executes current native
Debug/Final full no-deploy builds and immediate same-build campaign ABI checks
for the uncommitted M3 overlay; CI has not run. The former nine-engine wrapper
now also rebuilds the source VertexBufferManager, MaterialManager,
ParticleEffect, StringTable and TextureManager required by the isolated
runtime. Shared PCH platform-guard and Win32 min/max fixes are tracked
separately from gameplay comparisons. The existing outside-repository
validation helper gains bounded launch arguments, timeout/nonzero failure
status and tested registry restoration. Both the first staging and corrected interactive composition have no mission
trace. The latter exits with an access violation in installed dahotkey.dll,
after required provider creation failures beginning at IRenderPipeline.
This is an open restored-runtime/module gate, not mission/audio evidence. Current results and limitations:
[M3_SIMULATION_COMPARISON.md](M3_SIMULATION_COMPARISON.md). The older entries
below remain historical, not proof for the current overlay.

# VS2022 Win32 Build Validation

Status snapshot: **2026-09-25**. This record covers native Windows compilation and linking of the
supported Debug and Final wrappers, followed by a separate isolated Final game launch on a
physical Windows host. Build results and Windows runtime observations are identified separately;
neither is a retail image-parity result or a native Linux game build.

## Host and inputs

- Host: a Windows 11 Pro x86_64 VM, native VS2022 Professional MSBuild
  `17.14.60.43110`, v143 toolset `14.44.35207`, Win32 target.
- Python `3.13.14`; Windows SDK `10.0.22621.0`; a locally installed June 2010 DirectX SDK
  located by `DXSDK_DIR`. The SDK header `d3dx9.h` and x86 library `d3dx9.lib` were present.
- Source: the project checkout with this worktree's changes. No retail media was copied into the
  checkout or used as build input. The campaign inputs were the repository's source and resource
  scripts. `-NoDeploy` kept the build from copying binaries into a game installation.
- The VM did not have MFC's `afxres.h`. An attempted installation of the official VS2022
  `Microsoft.VisualStudio.Component.VC.ATLMFC` component exited with code 1 and did not install
  that header. The resource scripts only needed the standard SDK resource declarations; their
  includes and editor `TEXTINCLUDE` records now use the installed Windows SDK `winres.h`.

## Build blockers and owning fixes

The first full Debug attempts exposed these failures in sequence. Each change was retested at the
nearest target before the successful full builds:

| Failure observed on native Windows | Owning change and bounded result |
|---|---|
| `d3dx9.h` unavailable despite the installed SDK | `Directory.Build.props` keeps the existing sibling SDK location and falls back to `DXSDK_DIR` when that location lacks the required headers/libraries. `D3DRenderPipe` and `MeshManager` then rebuilt. |
| Win32 `max` macro expanded C++ `numeric_limits::max()` and `std::max()` | Protected the affected calls in DACOM runtime/handle/profile code and the App batcher. The batcher also converts its `U32` delay operand to the existing 32-bit `unsigned int` `maxDelay` type; the global symbol and width are unchanged. |
| `GlobalFlags` collided with the Win32 `GlobalFlags` function | Renamed only the C++ type to `CQGlobalFlags`, retaining the exported `CQFLAGS` variable and bitfields. Win32 compilation now checks the 8-byte flag layout and 4-byte render-flag layout. No persistent record or public vtable changed. |
| `-NoDeploy` still ran `MeshManager`'s post-build copy | Passed `ConquestEnableDeploy=false` to engine and campaign builds and conditioned the Debug MeshManager copy on it. Its destination now uses the existing deploy property when deployment is enabled. |
| `Globals` generation called unavailable `py` | Its custom step now calls the installed `python`; `sfxdata.dat` was generated before resource compilation. |
| `Globals.dll` lacked `__MATH_ENGINE` and then `DACOM_Acquire` | Declared the correct Debug/Release `MathLib.lib` and `DACOM.lib` link inputs in `Globals.vcxproj`; the targeted `Globals` Debug rebuild linked. |
| `Trim` could not instantiate `_modalfactory` | Implemented its required no-op `GlobalComponent::Initialize`; the existing `Startup` DLL lookup stays in place. |
| `Trim` mixed `/MT` with Debug STL directives from three objects | The supported Debug engine inputs now use `/MT`, matching the established static CRT policy. `PlatformTime`, `LineManager` and `Teletype` compile without the conflicting manually defined `_DEBUG`; the latter two avoid the Debug precompiled header. `dumpbin /directives` identified those exact objects; targeted `Trim` Debug then linked. |
| Campaign resources required MFC and `Mantis_T` lacked `__MATH_ENGINE` | Switched the 21 campaign `.rc` sources from `afxres.h` to SDK `winres.h`; appended `MathLib.lib` to the existing campaign `Directory.Build.targets` configuration-aware dependencies. Targeted `Mantis_T` Debug linked, then the full campaign solution built. |

The numerical-limit guards in `StateMaterial.cpp` and `RenderSubmission.h` also remain valid under
the Win32 `max` macro by source review and Linux compilation. Those translation units are outside
the supported Windows wrapper's measured closure; the full Win32 builds do not establish their
separate Windows execution or link status.

## Commands and observed output

The following commands ran in PowerShell in the VM, with `$root` set to `C:\Code\frontierX`:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
$log = Join-Path $root 'out\windows-debug-build-11.log'
& (Join-Path $root 'Build-Conquest.ps1') -Configuration Debug -NoDeploy *> $log

$log = Join-Path $root 'out\windows-final-build.log'
& (Join-Path $root 'Build-Conquest.ps1') -Configuration Final -NoDeploy *> $log
```

Both commands completed successfully. The wrapper rebuilt the engine projects, the application
solution and the campaign solution, then verified its required outputs. Independent file checks
found, for **each** configuration, `Conquest.exe`, `Globals.dll`, `Mission.dll`, `Trim.dll` and
`ZBatcher.dll` under `Code\App\Src\Debug` or `Final`, plus `D3DRenderPipe.dll` and
`MeshManager.dll` under `Code\Libs\ExplicitDLL\debug` or `release`. `dumpbin /headers` reported
`14C machine (x86)` for both `Conquest.exe` files. The campaign output directory
`Code\App\Src\Scripts\Build` contained **18 DLLs**: `Mantis_T`, `SCRIPT01` through `SCRIPT16`,
and `Sol_T`. The Final campaign rebuild writes to the same campaign output directory as Debug,
so the retained campaign files there are from the last, Final build.

The same worktree's production batcher and related Linux targets compiled under GCC 16.2.1 and
Clang 22.1.8. The focused `portability.render_batcher_header` and
`portability.render_batcher_runtime` checks passed under both compilers after the final delay-type
change. Earlier full Linux presets each executed 37 successful CTests and skipped the GPU CTest in
the command sandbox. The separate visible Vulkan and sanitizer evidence for the rendering slice
is recorded in the renderer and material documents; these Windows builds add no Windows image
evidence.

A read-only same-build import/export check also ran on native Windows after Final:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
& (Join-Path $root 'Tools\abicheck\allmods.ps1') `
  -DumpbinPath 'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\dumpbin.exe' `
  -BuiltGameDirectory (Join-Path $root 'Code\App\Src\Final') `
  -ScriptDirectory (Join-Path $root 'Code\App\Src\Scripts\Build')
```

It found **zero unsatisfied imports** among the 153 distinct `Mission.dll` and two distinct
`Trim.dll` symbols demanded by the 18 freshly built campaign DLLs. The script's generic output
calls its inputs "retail scripts", but this invocation used only **our freshly built** scripts.
It is a synthetic same-build cross-module check, not retail ABI parity. The tool reported
`DACOM.dll` and `KERNEL32.dll` as outside its checked built-module set.

## Limits of the initial build-only check

- At this build-only stage, no Windows executable was launched and no retail installation, game
  scene, save, network session or campaign mission was exercised. Compilation and linking alone
  do not establish retail behavior; the later retail startup attempt is recorded below.
- No representative Windows image was captured and no Linux/Windows pixel comparison or tolerance
  was evaluated. The current rendering milestone remains open.
- At this stage, no native-Windows `Tools/abicheck` comparison against **original retail** script
  DLLs had run. The later installed-retail follow-up below records that check separately.
- The supported Windows wrapper does not build the production `StateMaterial` target separately.
  Its Linux CPU and Vulkan tests remain synthetic evidence; material-library loading is open.
- Rebuilding this legacy solution modifies tracked binary outputs and generated `data.i` in the VM
  checkout. These generated changes are local build products and must not be committed as source.
  Diagnostic logs are under the ignored `out` directory.

The VM build can be repeated from the checkout with `Build-Conquest.ps1 -Configuration Debug
-NoDeploy` and `Build-Conquest.ps1 -Configuration Final -NoDeploy`. Before rendering milestone
acceptance, capture the same repository fixture on the supported D3D9 path and compare it with
the native output under a documented tolerance. The original campaign import check is recorded
in the later follow-up below; gameplay and renderer runtime checks remain open.

## Queued-pool follow-up (2026-09-25)

The later native queued-pool source change in `RenderBatcher.cpp` was copied alone to the same
Windows 11 VM checkout. A line-by-line comparison before transfer found that its VM copy differed
from the local file only by the new texture-stage replay predicate and its nine call sites. The
VM's SHA-256 of the transferred source matched the local file:
`032e5ad20cf1019fb3f4fdf6aafdeee4bf8a3e6593bb4eccc133ca631cc3e23d`. The Windows
predicate accepts all legacy stage slots, preserving the D3D9 loop coverage by source review.

From the VM checkout, both supported wrappers were rerun in PowerShell:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
& '.\Build-Conquest.ps1' -Configuration Debug -NoDeploy *> 'out\windows-debug-queued.log'
& '.\Build-Conquest.ps1' -Configuration Final -NoDeploy *> 'out\windows-final-queued.log'
```

Both commands exited successfully. Their logs show native MSVC compilation of
`RenderBatcher.cpp` and linking of `Debug\ZBatcher.dll` or `Final\ZBatcher.dll`. Each wrapper
reported the required game and engine outputs plus 18 built campaign modules. `dumpbin /headers`
reported `14C machine (x86)` for both `Conquest.exe` files. The Final same-build ABI check was
rerun with the command above: all 153 distinct `Mission.dll` and two `Trim.dll` imports requested
by the freshly built campaign DLLs were satisfied. This remains a synthetic same-build check; the
tool's generic "retail scripts" heading does not describe these inputs. No original retail DLLs,
Windows game runtime, D3D9 fixture image or Linux/Windows image tolerance comparison were used.
Generated binaries and logs remain in the VM checkout or its ignored `out` directory; none was
copied into the Linux repository. The additional Linux GCC/Clang and sanitizer evidence for this
slice is in [RENDER_BATCHER_PORTABILITY.md](RENDER_BATCHER_PORTABILITY.md).

## Installed retail campaign ABI follow-up (2026-09-25)

A lawfully installed Steam copy was available on the same Windows 11 VM after the build checks.
The external installation had the expected ten top-level `DATA` directories and 18 campaign
DLLs. No installed binary or media was copied into the source checkout. The two repository ABI
scripts ran with native VS2022 `dumpbin` against the installed, original campaign DLLs, original
`Mission.dll`, and each freshly built Debug and Final `Mission.dll`/`Trim.dll`. The commands below
use a placeholder for the external installation path:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
$root = 'C:\Code\frontierX'
$retail = '<external-retail-installation>'
$dumpbin = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\dumpbin.exe'
foreach ($config in @('Debug', 'Final')) {
    $built = Join-Path $root "Code\App\Src\$config"
    & (Join-Path $root 'Tools\abicheck\allmods.ps1') `
        -DumpbinPath $dumpbin -BuiltGameDirectory $built `
        -ScriptDirectory (Join-Path $retail 'Scripts')
    & (Join-Path $root 'Tools\abicheck\symdiff.ps1') `
        -DumpbinPath $dumpbin -RetailMissionDll (Join-Path $retail 'mission.dll') `
        -BuiltMissionDll (Join-Path $built 'Mission.dll') `
        -ScriptDirectory (Join-Path $retail 'Scripts')
}
```

For **each** configuration, `allmods.ps1` found that all 153 distinct `Mission.dll` and two
`Trim.dll` imports requested by the 18 **original retail** campaign DLLs were satisfied; it
reported zero missing symbols. `symdiff.ps1` likewise found zero distinct unsatisfied retail
campaign imports. Its complete `Mission.dll` export comparison counted 333 retail exports and 357
build exports: four names occur only in retail and 28 only in the build. The four retail-only names
are not imported by these 18 campaign DLLs. This is native Windows retail-binary ABI evidence for
that campaign import set, not proof that the retail and restored CQ2-derived builds are identical
or that every possible external binary can load. Source review of the four owners
(`MScript::GetCountedString` and three `MGlobals` name/character helpers), combined with the built
export names, attributes their spelling difference to retail's 16-bit unsigned-short string
pointers versus modern MSVC's distinct native `wchar_t` pointers; their scalar `U32` decorations
agree. This is a measured version/toolchain difference, not an observed defect in the supported
build. Changing the project-wide `/Zc:wchar_t` policy or adding aliases solely to equalize the
export lists could disturb other module boundaries, so this validation made no ABI change.

The VM's active desktop is in a separate console session from SSH. An attempted interactive
launch through a scheduled task with elevated privileges and a full-desktop capture was rejected
by automatic approval review because that execution and capture scope was broader than the
authorized runtime comparison. No such task was registered and no image was captured. The D3D9
game run, repository-fixture capture and cross-platform tolerance comparison remain open.

## Installed retail startup on the interactive VM desktop (2026-09-25)

After the user shared an already open NoVNC tab, the installed retail `Conquest.Exe` was started
from its installation directory in the active Windows console session. The user personally
accepted the game's first-run license agreement and reported seeing the menu. The agent's visible
capture showed the Conquest logo followed by a German message that no sound playback device was
available; the process exited after that dialog. A read-only `Win32_SoundDevice` query returned
no device. This was a **retail runtime attempt**, with no agent-observed stable menu or gameplay
smoke.

For a second bounded attempt, an alternate `FXNoSound.ini` beside the installation was created
from the original INI with only its `Sound =` line commented. The game accepts `/ini=FXNoSound.ini`.
It advanced past the sound-device error but stopped with the visible message that Conquest is
incompatible with the graphics card. The VM reports a Red Hat VirtIO GPU DOD controller, driver
`100.103.104.30200`, at 1280 by 800. A third attempt additionally selected the INI's documented
`RendSft` software pipeline and reached the same graphics-card error. The observed error does not
identify the exact failed Direct3D or DirectDraw capability call; attributing it specifically to
the virtual adapter is an inference from the device report and repeatable behavior.

The original `conquest.ini` and installed media were not modified. The temporary alternate INI
and launch command were removed after the game exited; a process check found no remaining
`Conquest` process. No game image was saved or copied into the repository. The restored-build
runtime, D3D9 fixture capture, and image comparison remain unexecuted on this VM because the
retail reference cannot reach a stable rendering scene with its current graphics setup. A
Windows environment that successfully runs the supported D3D9 game path is required for that
comparison; a software-renderer attempt alone would not satisfy the D3D9 acceptance gate.

## Physical Windows host follow-up (2026-09-25)

The comparison effort moved to a physical Windows 11 machine with a GeForce RTX 3050 Ti Laptop
GPU, VS2022 Community MSBuild `17.14.51`, Python `3.14.3`, and the June 2010 DirectX SDK. The
checkout was at `a343f0fac7d5edb9f4700144af0bf4561880e456`. Native PowerShell invocations of
`Build-Conquest.ps1 -Configuration Debug -NoDeploy` and `Build-Conquest.ps1 -Configuration Final
-NoDeploy` both completed with the game, engine and 18 campaign DLL outputs. The same installed
original retail campaign modules were checked with `Tools/abicheck/allmods.ps1` and
`symdiff.ps1` against each freshly built configuration: all 153 distinct `Mission` and two
distinct `Trim` imports were satisfied. The 333 retail versus 357 built `Mission` exports (four
retail-only, 28 build-only) retain the known version/toolchain difference. These are native
Windows build and retail-binary ABI checks, not image or gameplay parity.

The lawfully installed Steam game reached a menu and scene when launched by the user on the
physical desktop, though the user observed flicker. This is user-reported retail runtime
evidence, not a frame capture or a reference for exact restored-build pixels. A separate restored
Final runtime directory was assembled outside the checkout and installation. It contains copies
of the built executable, engine and campaign DLLs, non-overwriting support DLLs from the retail
installation, and directory junctions to the installation's `DATA`, `FONTS` and `goodies` trees.
No installed media entered the repository, and the original installation was not modified.

On the first restored-build launch, the splash briefly appeared before silent exit. Adding the
missing support DLLs allowed the process to advance to a modal error with an icon and OK button,
but both title and message were empty. A source review identified a concrete diagnostic defect:
`Trim`'s `_localLoadStringW` requested only the Windows user's language with `FindResourceEx`,
whereas the loaded `Globals.dll` resource script declares the needed strings only as English
(US). `Streamer` is one real caller that displays a stop-icon error through this lookup after
DirectSound initialization failure. The blank dialog alone does not establish that DirectSound
is the failing subsystem. The owning `PrintHeap.cpp` lookup was changed to retry English (US)
when the selected resource language is absent. Both supported Win32 wrappers were run again after
this source edit. Final's build log shows `PrintHeap.cpp` compiled in `Trim.vcxproj`, and the
rebuilt `Trim.dll` was copied into the separate runtime directory with matching SHA-256
`14D74E82D791C1D6F6B0F0B8C5846894798AE783EA24A13E7C6F53B4172574D5`. The Debug wrapper
also completed with its required 18 campaign modules. A repeated native-Windows check of the 18
original retail campaign DLLs reported zero distinct unsatisfied imports for both rebuilt Debug
and Final modules. The change adds no ABI slots or persistent fields.

The next interactive restored-build attempt displayed a readable stop-icon error:
`Can't find file/directory: "Mat"` followed by legacy CD-ROM advice. The latter is generic resource
text, not evidence of a copy-protection check. `MenuResource::updateLoadPath` actually requests
`Mat` from the virtual search path, and the installed Steam `DATA` tree contains archive files for
objects, interface, maps, profiles and textures but no `Mat` directory or archive. This is a
specific restored-source-versus-installed-data startup mismatch. A temporary, empty `Mat`
directory was created outside the source checkout and installation. The user's `DataPath` value
was saved as `Data`, then temporarily set to `DataOverlay;Data` so the search path could see that
directory and continue to read installed archives. The follow-up runtime result is recorded below
when observed; the registry value must be restored to `Data` after the test.

With that overlay, the restored Final splash stayed visible for roughly ten seconds and the process
then ended without a dialog. Windows Application Error event 1000 identified a `0xC0000005`
access violation in the built `Conquest.exe` at module offset `0x60120`. The crash executable's
`.text` bytes were compared with the newer Final executable whose PDB was available; both sections
had SHA-256 `cb415638fb2d77d67b00ef880fa46fdaf7af39c773d8d2a199c0f6ee5ef034c4`.
Symbolization maps the instruction to `Conquest.cpp`'s `PARTMAN->InitParticles(pInfo)` call.
The instruction dereferences the queried `IParticleManager` pointer without a runtime null guard;
the installed retail INI contains neither `ParticleEffect.dll` nor its `IParticleManager` system
entry. The repository's INI declares both and also names `MaterialManager`, `StringTable` and
`TextureManager`. This is a measured restored-build composition mismatch, rather than evidence
that the installed retail executable and restored source should behave identically.

Targeted native VS2022 Community `Release|Win32` rebuilds of those four production DLL projects
were run with `ConquestEnableDeploy=false`. `ParticleEffect` and `StringTable` linked without
source edits. `MaterialManager` initially failed to find `IMaterialManager.h` because its Release
include path pointed to `..\Libs\Include`, then failed to link `__MATH_ENGINE`. Its project now
uses the same `..\..\Include` path as Debug and links the existing `MathLib.lib` and `DACOM.lib`.
`TextureManager` initially requested absent `opends60.lib`, even though no source references to
that library were found. After removing that stale link input, the linker exposed three VFX shape
functions implemented in its existing `Winvfx16.asm`; only Debug had a MASM custom-build rule.
The project now builds that object in Release and uses the static `/MT` runtime to match the game.
All four target DLLs linked. `ParticleEffect` and `TextureManager` still emit the inherited
`/FORCE:MULTIPLE` warning that the resulting image might not run, so the interactive load remains
a separate required check.

Copies of these four built DLLs and the rebuilt Final executable were placed only in the external
test directory. A local test INI was derived from the repository's `Conquest.ini`, with relative
DLL paths prefixed by `DLL\`, `Rend0` selected for D3D9, and the unavailable optional
`VideoSystem.dll`/`IVideoSystem` entries omitted. The original installed INI and files remain
unchanged. The following launch tested this restored module composition; it was not yet Windows
renderer or retail-parity evidence.

The subsequent launch crossed the earlier null `PARTMAN` call and instead failed repeatedly in
the installed retail `vertexbuffermanager.dll`: Windows Application Error 1000 reported
`0xC0000005` at module offset `0x19ec` on three consecutive launches. The restored INI included
that name, but the isolated runtime still contained the installed retail DLL. The repository has
a production `VertexBufferManager.vcxproj`; its `Release|Win32` build initially exposed four
`std::numeric_limits<...>::max()` uses expanded by the Win32 `max` macro. Parenthesizing those
four calls preserved their numeric bounds and let the existing source compile and link. A freshly
built `VertexBufferManager.dll` replaced the retail copy only in the test directory; its build
output and staged copy have matching SHA-256
`E3B3A2D58FDB5FBA57003B9C1371C425FCF8E529D6F72942758692B91483AE06`. The same
changed translation unit built under Linux GCC and
Clang; each preset's `portability.vertex_buffer_manager` and `portability.native_shell` focused
CTest checks passed.

With the restored manager in place, the rebuilt Final game visibly reached its main menu and
Single Player Quick Battle setup screen. A 32-bit PowerShell module listing confirmed that the
running process loaded the built `D3DRenderPipe.dll`, the four newly built manager DLLs, the built
`VertexBufferManager.dll`, and the system `d3d9.dll`. This is **Windows runtime** evidence for
startup and UI presentation through the supported D3D9 path. It does not establish a gameplay
scene or the Linux/Windows fixture comparison. On selecting Quick Battle `Start`, Windows
Application Error 1000 reported a repeated `0xC0000005` in the installed retail `dahotkey.dll`
at offset `0x18be` (one earlier report used `0x18f9`). The user also observed the game closing.

The repository's `DAHOTKEY/Hotkey.vcxproj` was then built as `Release|Win32`. Its first compile
found missing Win32 `HINSTANCE` declarations in the shared EventSystem translation unit, an
invalid integer-to-`HWND` `static_cast` in the Hotkey owner, and two numeric-limit calls expanded
by Win32 `max`. The Windows platform include, explicit pointer cast on Win32, and parenthesized
limits fixed those compile errors without changing persistent layout or the event interface.
The project linked but its unconditional legacy post-build copy failed because the destination
drive was absent; its Release post-build event now obeys `ConquestEnableDeploy=false`. The
targeted Release rebuild completed and its DLL was copied only into the test directory, with
matching build/staged SHA-256
`5E18A2690CCAE8C58A7E24A98E767A23D251ADAF71D3F6B20D03AF92EC6E985A`.
Linux GCC and Clang rebuilt the affected production EventSystem and Hotkey targets, then each
executed `portability.event_system` and `portability.hotkey_event` successfully. On the
subsequent interactive attempt, the user reported that Quick Battle `Start` still ended the
game. The staged replacement's SHA-256 was verified again after that report. At 11:22 local
Windows time, no `Conquest.exe` process was running and Application Error event 1000 had no
entry newer than 11:13, before the replacement was staged at 11:18. The previous
`dahotkey.dll` access violation therefore cannot be attributed to the new build. No
`debug/startup_log.txt` or `debug/ingame_diag.txt` was present in the isolated run directory.
The new exit requires a fresh observed error message, debugger stop or timestamped trace before
assigning an owner.

The isolated run directory had no `debug` subdirectory, so its existing
`DumpView::STANDARD_DUMP` and startup diagnostics could not open their relative log files. After
creating that directory **only in the external test copy**, the next interactive run produced
`debug/startup_log.txt`. The embedded parser printed ten `PARSER: Undefined symbol` errors
for `static_assert` at `data.i` lines 4844–5530, then
`PARSER TERMINATING! EXCEEDED MAX ERRORS!`. Source review tied that resource to
`Globals.rc`'s `IDR_PARSER1`, generated by `Globals.vcxproj` with `/D_ADB`. The layout
assertions in `DSpaceship.h` and `DPlatform.h` had no `_ADB` guard, unlike neighboring
binary-layout assertions. They are now excluded only from the data-parser preprocessing path;
the C++ module builds retain them. The checked-in generated `data.i` also no longer contains
those ten assertions, while the build still regenerates that resource.

Targeted native VS2022 `Globals.vcxproj /t:Rebuild /p:Configuration=Final
/p:Platform=Win32 /p:ConquestEnableDeploy=false` and the corresponding Debug command
both succeeded. Both generated `data.i` files had zero `static_assert` lines. The Final DLL
was staged in the isolated run directory with matching build/staged SHA-256
`11EB4D496951D177FB9A470F2877DBB55D69EB909132CDA10CD00CC86F53E5D1`.
The next interactive run's startup log reached `Parse Complete` without parser errors; the
user could now enter the Campaign screen, which previously ended the visible session. This is
**Windows runtime** evidence that the parser blocker was removed. Quick Battle `Start` still
removed the visible window. A `Conquest.exe` process with no main window persisted after that
attempt; the current scene-start failure needs a bounded debugger/trace diagnosis. No gameplay
scene was confirmed.

The representative repository fixture has not yet been submitted on the supported D3D9 path,
captured on this physical host or compared against the Linux PPM. The milestone therefore remains
open despite the fresh builds and the installed game's launch.

### Campaign and data-provenance follow-up

The user then opened the restored Terran campaign and attempted its first mission. The visible
window disappeared. A timestamped Application Error 1000 for that attempt reported
`0xC0000005` in the built Final `Trim.dll` at RVA `0x26b21`. A targeted Final
`Trim.vcxproj` rebuild with compiler and linker PDB generation succeeded. The old and new
DLLs' `.text` sections had identical SHA-256
`20bd7315d90f94cd7a932bf12ef343aa3aaf4d8cf2baeb6384daa1841a1b93ce`;
`llvm-symbolizer` mapped the same RVA to `GenData::LoadArchetype`,
`GenData.cpp:244`. The instruction reads the existing `COMPTR<ICQFactory>` at
`GENNODE + 0x10` before assigning the selected factory. This identifies the failing
operation, but not its cause. The separate mission log recorded successful
`T_Mission1.dmission` briefing, parse data, globals and script loading. The startup log also
reported `ACM error #00000006` and `Streamer::init() failed on 'prolog01.wav'`; that audio
warning has not been shown to cause the GenData access violation.

A short temporary GenData trace was added around the real `ICQFactory::CreateArchetype` caller
to distinguish an initially non-null factory from corruption during a callback. The first traced
campaign attempt still used the installed retail databases and ended without a new Application
Error event. Its recorded successful factory returns retained a null prior pointer. The trace
has not yet identified a corrupting callback and must not be treated as a fix.

The isolated test directory's root `GenData.db`, `GameTypes.db` and `StringPack.db` were then
hashed and found byte-identical to the Steam retail copies. They were **not** the three
repository-owned databases in `Code/App/DB`: for example, `GenData.db` was 2,055,472 bytes
in retail versus 3,116,224 in the repository, and `GameTypes.db` was 1,099,780 versus
6,915,484 bytes. The restored source and installed retail binary/data reflect different
development states, so this was an invalid restored-build runtime composition. The three
installed copies were backed up within the external test directory, and the three repository
DBs were copied into that directory with matching source/staged SHA-256 hashes. The user's
11:49:29 attempt preceded this staging at 11:49:44. In the next interactive run at 11:52, the
user again reported the window disappearing before a scene was visible. The three staged DB
hashes matched the repository files. The current mission log recorded successful
`T_Mission1.dmission` briefing/script loading; the startup log had no parser or ACM error and
ended with a `RenderMgr` teardown notice. No new Application Error 1000 was reported. The
existing `briefing_diag.txt` was absent. The user confirmed that the campaign mission-selection
screen was the last visible game screen: the briefing page with its own `Start` button never
appeared. The matched data removed the invalid composition but did not establish a gameplay
scene. The Steam installation and repository media remain unchanged.

The next supported `Build-Conquest.ps1 -Configuration Final -NoDeploy` rebuild completed with
briefing transition diagnostics and all 18 campaign modules. Its five core binaries were copied
to the isolated directory and verified byte-for-byte by SHA-256 against the new build. During the
user's Windows runtime attempt, `DoMenu_Briefing` constructed the menu, loaded the first Terran
mission's briefing and script, created the viewer, and entered modal focus. The window then
disappeared inside the first `MenuBriefing::onUpdate`, before `MISSION->Update()` returned and
before any briefing frame was confirmed visible. No new Windows Application Error 1000 was
reported. This narrows the observed boundary to the real mission update; it does not yet
identify whether the matrix, script, score or AI owner exited or faulted. A further Final build
with four bounded `Mission::Update` checkpoints compiled successfully and is staged for the next
interactive attempt. Neither a campaign scene nor a D3D9 repository-fixture capture exists yet.

That interactive attempt again removed the window. `Mission::Update` completed
`THEMATRIX->Update()` and stopped before `MScript::Update()` returned. A further Final build
with bounded script diagnostics identified the loaded `Scripts\script01.dll`, the
`TM1_Briefing` program and its first callback as the last completed boundary. Source review of
that program shows its first state plays `prolog01.wav`, requests a briefing subtitle and then
briefing animation and teletype resources. The built `SCRIPT01` resource script declares only
English (US) string tables. On this German-locale host,
`MScript::GetCountedString()` requested only the selected German group and could return null;
`GetScriptStringLength()` dereferences that result in the first callback. This is a concrete
falsifiable cause for the observed exit, not yet a confirmed stack trace. The existing
`_localLoadStringW` already retries English (US) for the same missing-group case. The script
lookup now makes that same targeted retry. Its runtime effect and the subsequent briefing and
gameplay paths remain to be checked on Windows.

A read-only native-Windows `LoadLibraryEx(..., LOAD_LIBRARY_AS_DATAFILE)` and `FindResourceExW`
check of the staged, repository-built `SCRIPT01.dll` confirmed the precise group used by
`IDS_TM1_TELETYPE_LOCATION` (`1054 / 16 + 1 = 66`): German `0x0407` was absent and English (US)
`0x0409` was present. This confirms the resource-language mismatch independently of the
source-level declaration. It does not by itself prove which callback instruction caused the
process exit.

The Final no-deploy rebuild with the English (US) fallback and all 18 repository campaign DLLs
completed on the physical Windows 11 host. The isolated binaries were hash-checked against the
fresh build. In the next **Windows runtime** attempt, the first Terran briefing became visibly
usable, displayed its objectives and animated panels, and `TM1_Briefing::Update()` returned for
multiple iterations. This confirms that the lookup fix crosses the formerly failing callback.
The lawful installed media remained outside the repository; the corresponding restored DBs and
campaign DLLs came from the repository build. The user then clicked the briefing's `Start` button.
`IDS_START` reached `ChangeInterfaceRes(IR_IN_GAME_RESOLUTION)`, which completed
`CQE_LEAVING_FRONTENDMODE` but stopped during `CQE_ENTERING_INGAMEMODE` before
`MISSION->Reload()` and before a game scene appeared. The owner of that event callback is the
next Windows restoration blocker. A bounded callback-dispatch trace in the separately rebuilt
Release `DAHotKey.dll` is staged for one final interactive attempt before the requested pause.

The user's final pre-pause **Windows runtime** attempt again returned to the desktop after the
briefing `Start` button. A targeted native VS2022 Win32 `Hotkey.vcxproj` Release rebuild of the
production EventSystem succeeded and its `DAHotKey.dll` was staged only in the isolated test
directory with matching build/staged SHA-256
`DD0BC28756C19E1060914A6649885039D6C139F06A71BB92F2871AFE87E34A3A`. The bounded
`CQE_ENTERING_INGAMEMODE` dispatcher trace completed callbacks 0–3 and logged callback 4
(`client=1350EFBC`, `vtable=006A0694`, inside the fresh `Conquest.exe`) immediately before its
`Notify` call, with no matching `after 4`. Reading the own build's PE `.rdata` at the VTable
address and following its first x86 adjustment thunk through the matching Final PDB resolves it
to `DAComponent<PlanetFactory>::QueryInterface`; source review finds
`PlanetFactory::Notify(CQE_ENTERING_INGAMEMODE)` calling `loadTextures(true)` in
`Code/App/Src/Planetoid.cpp`. This identifies the next owner to inspect, but does **not**
prove the exact failing instruction or whether it is an access violation: no new Windows
Application Error 1000 was reported. No gameplay scene or Windows D3D9 fixture image was
captured. The EventSystem trace is temporary diagnostic code and remains in the uncommitted
worktree for the paused follow-up; its output and any debug dumps remain outside the repository.

After the test, the user's `HKCU\Software\Fever Pitch\Conquest: Frontier Wars\1.00` `DataPath`
was restored from temporary `DataOverlay;Data` to its saved original `Data`, and a subsequent
registry query verified `Data`. The isolated test directory and lawful Steam installation were
otherwise left in place. Work was paused at the user's request after this test.

## Resumed game-mode entry investigation (2026-09-25)

The next isolated Windows runtime launch reported the missing `Mat` directory. Source review of
`menu.cpp::updateLoadPath` and `MaterialManager::Initialize` confirmed that the restored build
requires the repository's `Data/mat` tree, which contains material, shader and texture inputs; an
empty directory does not satisfy that contract. The isolated test directory's `DataOverlay/Mat`
placeholder was replaced by a junction to the checkout's `Data/Mat`, and the temporary per-user
`DataPath` was set to `DataOverlay;Data`. The second search root remains a junction to the lawful
external installation's data. The installed copy was not edited. The next **Windows runtime**
startup completed parsing and reached the Terran briefing, demonstrating that this particular
missing-directory startup blocker was passed. The override is still active for the isolated test;
it is not a supported deployment configuration and must be restored when validation ends.

After the briefing `Start` button, the window still disappeared. A bounded temporary trace in
`PlanetFactory::loadTextures` followed the already identified `CQE_ENTERING_INGAMEMODE` callback.
The **Windows runtime** trace recorded a successful `point.anm` open and archetype creation, then
a successful `slotholder.3db` open, and stopped during its `ENGINE::create_archetype` call. The
matching Windows Application Error query returned no new error event, so the precise exit
mechanism remains unconfirmed. Neither the gameplay scene nor a representative D3D9 image was
observed.

A DLL inventory then revealed that the isolated directory's `Engine.dll` was an 82,000-byte
external copy, while the checkout had an older 151,552-byte build output at a different path.
The supported Final wrapper builds only `D3DRenderPipe` and `MeshManager` from the explicit engine
DLLs; its successful result did **not** mean `Engine.dll` was rebuilt or deployed. A focused
native VS2022 Win32 Release `Engine.vcxproj` rebuild initially failed on a stale include path,
the Win32 `min` macro in `FileSys_Utility.h`, and a missing shared `Src/Include` path. Correcting
those owning build/header defects made that target compile and link. The linker still warned that
its image was produced with `/FORCE` (`LNK4088`); executable runtime compatibility is therefore
not established by compilation. The new `Engine.dll` was copied only to the isolated test
directory after backing up the prior copy; source and staged SHA-256 matched. A full Final
no-deploy wrapper had rebuilt the five main outputs and all 18 campaign modules before the
focused Engine rebuild; the five main outputs were then staged and matched by SHA-256. The
subsequent interactive launch displayed the Windows UAC dialog. A later check found a fresh
14:34:50 **Windows runtime** startup and engine trace, confirming that the new Engine DLL was
executed. The trace allocated archetype index 3 for `slotholder.3db`, rejected a compound
directory (result `0xFFFFFFFA`) and entered the fallback `IRenderer::create_archetype` callback;
it stopped before that callback returned. The separate `PlanetFactory` trace stopped at the same
Engine call. A gameplay scene was not observed. This moved the active boundary from the Engine
entry to its renderer component.
The matching focused VS2022 Win32 Debug `Engine.vcxproj` rebuild also compiled and linked after
adding the shared `Src/Include` path to its configuration. It was not staged or run. Its linker
reported duplicate CRT/COMHeap definitions, an `LNK4098` runtime-library conflict and the same
`LNK4088` `/FORCE` warning; those warnings remain release/ABI risks, not successful runtime proof.
The shared `FileSys_Utility.h` macro guard also compiled into the existing
`frontierx_hotkey_event_probe` target under Linux GCC and Clang; each corresponding
`portability.hotkey_event` CTest executed and passed. These focused Linux checks do not execute
the Windows Engine or renderer components.

```text
cmake --build --preset linux-gcc-debug --target frontierx_hotkey_event_probe -j2
ctest --test-dir out/build/linux-gcc-debug -R '^portability.hotkey_event$' --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_hotkey_event_probe -j2
ctest --test-dir out/build/linux-clang-debug -R '^portability.hotkey_event$' --output-on-failure
```
A separate 32-bit Windows PowerShell process also called `LoadLibraryW` on the staged Release
`Engine.dll` with the isolated test root and DLL directory on its process-local search path. It
returned a non-null module handle and Win32 error 0. This checks loader resolution only; it does
not execute `ENGINE::create_archetype` or establish game compatibility. The interactive test still
needs a confirmed post-UAC launch result.

This targeted Engine rebuild did not rebuild or establish parity for every external DLL in the
isolated runtime directory. The inventory found matching hash values for the already rebuilt
`DAHotKey`, `MaterialManager`, `ParticleEffect`, `StringTable`, `TextureManager` and
`VertexBufferManager` outputs, but `D3DRenderPipe` and `MeshManager` in the isolated DLL directory
still differ from the latest wrapper outputs. Other runtime DLLs do not have a current build
output in the wrapper's Release directory. The Engine trace identifies the renderer component as
the next runtime boundary; remaining loader dependencies must be reconciled before claiming a
consistent restored Windows runtime. Windows fixture capture and Linux/Windows tolerance
comparison remain unperformed.

The isolated directory's `RenderMgr.dll` at the `IRenderer` boundary was also an external copy.
A focused VS2022 Win32 Release `RenderMgr.vcxproj` rebuild compiled and linked the repository
implementation without a source change, and its DLL replaced only that isolated copy after a
backup. Build/staged SHA-256 matched
`E1F65873B2823A5C689E27182AF18D548D70B09B0F7B417630BAE9141759B10B`. A matching
VS2022 Win32 Debug target rebuild also compiled and linked but was not staged. Both configurations
still emitted the historical CRT/COMHeap duplicate-symbol and `/FORCE` image warnings. The next
interactive game-mode test with this RenderMgr DLL is pending. A separate 32-bit `LoadLibraryW`
call returned a non-null module handle and Win32 error 0, but loading alone does not establish
that its render-component dependencies are compatible with the mixed runtime directory.

The first requested post-staging attempt showed a Windows UAC dialog in the shared desktop, but
no fresh game startup or Engine log followed after the dialog closed. No user confirmation of that
prompt's choice was available. To make the next successful run decisive, temporary diagnostics
were added to `RenderMgr::create_archetype` around map insertion, discrete/continuous/simple LOD
selection and the child render-component calls. The focused VS2022 Win32 Release target rebuilt
successfully, and its isolated staged DLL matched SHA-256
`77F90EE4DA0247F347CB97A260F3E0CFA5930BF540E77DB3C2706E312527A96D`. That newer DLL
supersedes the earlier hash above. The interactive runtime behavior of this diagnostic build is
still pending; the code is temporary and must be removed or suitably gated before release.

The next interactive Windows run (2026-09-25 15:35 local host time) opened the first Terran
briefing and then appeared frozen after its **Start** button. The process had exited by the time
its state was queried at 15:37; the user clarified that the visible failure was a freeze, not an
immediate return to the desktop. The fresh repository-built Engine and RenderMgr diagnostics show
`slotholder.3db` opening, failed compound-directory selection (`0xFFFFFFFA`), successful entry
into `IRenderer::create_archetype`, failed discrete and continuous LOD selection
(`0xFFFFFFFF` each), and entry into the first simple render-component call at index zero. No
return from that child call was logged. The configured component order in `Conquest.ini` begins
with `PolyMesh`, followed by `Optics`, so `PolyMesh::create_archetype` is the next source-level
diagnostic boundary. This trace alone does not prove whether the component hung, crashed, or was
terminated externally. The startup log also contains `Streamer::init` ACM errors for several
briefing sounds, but it continued into the render-component call; those audio errors are not
evidence of the observed stop's cause.

Focused VS2022 Win32 Release and Debug builds of repository `PolyMesh.vcxproj` and
`Optics.vcxproj` both compiled and linked on the physical Windows 11 host. Only their Release
DLLs were copied into the isolated runtime directory after backing up the prior copies; staged
SHA-256 values were `8068AD7144374929CA496C96ED8BD977779A2F0555C7A923495374B328B3276F`
for `PolyMesh.dll` and `67FC2112E1D99B37FF1A31A319C0DBC37FCEEC47456B4AAEA8807B89F25B7255`
for `Optics.dll`. A separate native 32-bit PowerShell `LoadLibraryW` call on each staged module
returned a non-null handle and Win32 error zero. These are compilation and loader checks, not
execution of either component's archetype parser or a scene-rendering result. Both projects still
emit their historical `/FORCE`/CRT link warnings. At this stage, full runtime module parity,
game scene, Windows D3D9 fixture capture and Linux/Windows image tolerance were still open; the
later results appear below.

Temporary diagnostics now bracket `POLYMESH::create_archetype` entry, the mesh-directory open,
pipeline information, `XMesh` allocation and `XMesh::read` so the next visible game run can
identify the first call that does not return. The source was copied to the same Windows checkout
and a focused VS2022 Win32 Release `PolyMesh.vcxproj` build returned exit code zero. The latest
`PolyMesh.dll` was hash-checked after staging only in the isolated test directory
(`9965070C3DD06F3528C5F21B251271D4BF4D492FAA9D5370BB37E50EC2CBF50A`). The staging
script also rebuilt/copied `Optics.dll` at hash
`04C319740FFE9D7F4FD1914CA5742060296B2171130020900A5100CC9CF0E771`. The earlier hashes
above identify the binaries used for the observed 15:35 run; these new hashes identify the
subsequently executed diagnostic composition. A new isolated run at 15:59 local time entered
`POLYMESH::create_archetype`, opened its `openFLAME 3D N-mesh` directory with result zero,
returned from pipeline-info discovery and allocated `XMesh`. Its last PolyMesh trace was
`before XMesh read`; no return from `XMesh::read` was logged. The process was gone at the next
state inspection, but there is no observation in this run distinguishing an exception, a hidden
dialog, an external termination or a hang. The trace moves the active owner to the real
`PolyMesh -> XMesh::read` path; it does not establish mesh-data compatibility.

Temporary step diagnostics were therefore added around `XMesh::read` name resolution, format
check, face groups, normals, vertices, LOD, edges, UVs, materials, sphere, bounds and sorting.
A focused native VS2022 Win32 Release `PolyMesh.vcxproj` rebuild returned exit code zero, and the
new DLL was backed up and hash-verified only in the isolated directory at SHA-256
`1BE65687E493D0ABA94693A7C465D2E5C715C88C2B6A3FCDED17569BD8D8FC1D`. The staging
script's matching `Optics.dll` hash was
`6E148E4F314258A7AB714B1DADF1A2A6396302700B6F65E11D7B0BF1E530A5AC`. This newest
composition was subsequently executed as described below. Both temporary trace owners must be removed or gated before a
release build.

The next isolated Windows runtime attempt at 16:05 left a visible frozen image while the
`Conquest.exe` process remained present at the first inspection; it was absent at a later
inspection. The new `XMesh` trace resolved the name, passed the old-format check, loaded face
groups, normals, vertex data, LOD and edges, then recorded `read_uv_channels` result
`0x80004005`. It did not reach the marker after `read_material_library`. The code calls
`ITextureLibrary::load_library` first in that function, then chooses either the CQ2 or legacy
material directory. These observations localize the next owner but do not prove that the UV
failure caused the stop or that the texture-library call itself failed. The staged
`TextureLibrary.dll` was an external copy without the repository's existing texture-load trace.
A focused native VS2022 Win32 Release and Debug `TextureLibrary.vcxproj` rebuild returned exit
code zero for both configurations, with the existing COMHeap/CRT and `/FORCE` warnings. Its
Release DLL was backed up and hash-verified only in the isolated test directory at SHA-256
`8FF70E33962983F7660E2A1E3F662CC6CC5BD6AC411AE314F1F3CD45263BEE81`. The next
interactive run executed this DLL in the game, as recorded below.

### Extended supported Win32 build roots (2026-09-25)

The earlier wrapper built only `D3DRenderPipe` and `MeshManager` from the explicit engine
projects, leaving the observed `ENGINE -> RenderMgr -> PolyMesh/Optics` path dependent on
separately built or external DLLs. `Build-Conquest.ps1` now also rebuilds `Engine`, `Anim`,
`HardPoint`, `RenderMgr`, `TextureLibrary`, `PolyMesh` and `Optics` before the application and
includes their DLLs in its required-output and deployment hash list. The change uses the
already supported Win32 Debug/Final configurations and leaves all lawfully installed game data
outside the checkout.

On the physical Windows 11 host with VS2022 Community, June 2010 DirectX SDK and Python 3, both
complete NoDeploy wrapper commands returned exit code zero:

```powershell
& .\Build-Conquest.ps1 -Configuration Final -NoDeploy
& .\Build-Conquest.ps1 -Configuration Debug -NoDeploy
```

Each command rebuilt the nine explicit engine DLLs (`D3DRenderPipe`, `MeshManager`, `Engine`,
`Anim`, `HardPoint`, `RenderMgr`, `TextureLibrary`, `PolyMesh`, `Optics`), five application outputs
and 18 campaign DLLs. The Final wrapper reported `Conquest.exe` 3,142,656 bytes,
`Globals.dll` 598,016, `Mission.dll` 1,073,664,
`Trim.dll` 1,377,792 and `ZBatcher.dll` 406,016; Debug reported 4,727,296, 967,168,
1,522,688, 1,693,184 and 433,152 bytes respectively. The projects continue to emit their
known legacy CRT/COMHeap and `/FORCE` warnings. The no-deploy runs neither changed the isolated
runtime directory nor executed the game.

After the latest Debug run, native `allmods.ps1` against its matching freshly built 18 Debug
campaign DLLs found zero missing imports among 153 distinct `Mission.dll` and two `Trim.dll` symbols. The
campaign solution was then rebuilt as Win32 Release (exit code zero) to restore a matching Final
script set; `allmods.ps1` against the fresh Final application DLLs likewise found zero missing
imports among 153 `Mission` and two `Trim` symbols. Both checks used native VS2022 `dumpbin` and
are same-build cross-module ABI evidence, not execution of the campaigns or proof that the
external retail versions match. Other INI-listed runtime DLLs remain outside the wrapper's
explicit roots, and the staged interactive diagnostic composition had not yet been refreshed from
the full wrapper outputs. A visible Windows scene was confirmed separately. At this stage,
mission progress, same-fixture Windows capture, tolerance comparison and Linux game target were
still open; the later fixture result appears below.

### First visible restored Final scene and remaining gameplay blocker

At about 16:14 local time on the physical Windows 11/NVIDIA host, the user repeated the first
Terran campaign briefing's **Start** action in the isolated Final test directory after the
repository-built Release `TextureLibrary.dll` above was staged with backup and SHA-256 check.
This directory still uses `DataPath=DataOverlay;Data`: `DataOverlay/Mat` is a junction to the
repository's material tree and `Data` is a junction to a lawfully installed external Steam data
root. No retail media was copied into the repository or altered in the installation. The user
reported **game scene visible**. The shared Windows desktop showed the game window with its HUD
and space background, and `Conquest.exe` remained running at the immediate process inspection.
The user subsequently confirmed that the background can be scrolled, but no units appear and the
mission does not run. This is bounded **Windows runtime** evidence of scene entry and
interactive camera/background movement, not a playable mission or retail comparison.

The corresponding temporary `XMesh::read` trace passed face groups, normals, vertices, LOD,
edges, materials, sphere, bounds and sorting for repeated mesh loads. Its
`read_uv_channels` call returned `0x80004005`, while later phases still completed; this return
value therefore must not be equated with the previous window loss without further evidence.
The repository TextureLibrary trace recorded `echoanim.tga` texture loading, mip uploads through
the D3D9 path and a `GR_OK` return. The sequence strongly implicates the earlier external
TextureLibrary DLL composition in the material-load stop; it was not a controlled binary A/B
comparison, so a sole root cause is not established. The isolated runtime still combines DLLs
from separate build/staging runs, whereas the latest full Debug/Final wrappers were NoDeploy.

The next falsifiable gameplay check is to locate the first mission-load or simulation boundary
that does not complete after entering the D3D9 scene; the user-visible absence of units alone
does not locate that owner. Read-only inspection of the first visible run's diagnostic files at
16:22 found a `Mission::Reload` trace that passed terrain, sector, map, camera, object-list,
matrix, fog, music and lights loading, then stopped after the `MScript::Load` entry marker. That
was an incomplete trace, not proof that the script loader hung. In a later instrumented **Windows
runtime** run, `MScript::Load` logged `LoadScript: complete`, the briefing's `IDS_START` handler
logged `after Reload`, and `DoMenu_Briefing` logged `modal returned`. The user still saw only a
scrollable background and HUD, without units or mission progress. The loader hypothesis is
disproved for that later run. The origin of the early briefing `IDS_START` event and the missing
scene objects are still unresolved. Quick Battle `Start` separately loses the visible window.
The user additionally reported seeing **two ships in the Celareon campaign** on this host earlier
that day. This observation has no matching diagnostic trace or controlled build hash, so it is
bounded user-reported Windows runtime evidence. It rules out describing all restored scenes as
unit-free; the repeatable missing-unit observation above applies to the first Terran mission.

The repository fixture has now separately been submitted and captured on the supported Windows
D3D9 path and compared with the native Vulkan frame; see the next section. The visible campaign
background was never used as that fixture. Temporary `XMesh`, `PolyMesh`, `RenderMgr`, `Engine`
and other restoration traces still need removal or gating before release.

### Physical Windows 11 same-fixture D3D9 comparison

The explicit `/renderfixture` branch in `Conquest.exe` runs after normal production pipeline
startup and before the game loop. It uses the same repository `RenderSmokeScene.cpp` data as the
Linux native shell, creates its two-by-two RGBA texture through `IRenderPipeline`, applies the
shared depth, source-alpha blend and stage-zero texture-combine states, and calls the production
`PrimitiveBuilder` direct draw through D3D9. It presents once and captures the top-left 64-by-64
client area to `debug/render_fixture_win32.ppm` using a top-down GDI DIB. The capture is a
visible-desktop copy, so occlusion, display scaling and focus are explicit limitations; the
inspected final image contained the expected geometry and distinct colors. The branch writes
`debug/render_fixture_steps.txt` with the return code of each stage and
`debug/render_fixture_status.txt` with `CAPTURE_OK` or `CAPTURE_FAILED`. It exits after cleanup;
the short-lived image at the upper-left of the window was expected.

The isolated Final runtime directory was refreshed as one 32-file closure: five application
outputs (`Conquest`, `Globals`, `Mission`, `Trim`, `ZBatcher`), nine engine DLLs and all 18 campaign
DLLs. Each file was copied from the same Final wrapper build with a backup and matching SHA-256
check. Later focused renderer/EXE replacements were also backed up and hash checked. The
test-only `DataPath=DataOverlay;Data` still links repository materials and a lawful external
Steam data root; the installation and repository were not modified with retail media.

The first fixture attempt returned `CAPTURE_FAILED` before drawing. Its step trace passed texture
creation/upload, transforms and state setup, but `clear_buffers` returned `GR_GENERIC`. A targeted
production-renderer trace recorded D3D9 `Clear` HRESULT `0x8876086C` (`D3DERR_INVALIDCALL`),
flags `0x3`, cooperative status `0` and viewport `(0,0,64,64)`. Source inspection identified a
contradiction in `create_buffers`: it set the `D3DRP_F_DEPTH_CREATED` flag but passed
`EnableAutoDepthStencil=FALSE`, `AutoDepthStencilFormat=D3DFMT_UNKNOWN` and made no manual depth
surface on this path. The owner now enables the already selected `depthFmt` automatic surface.
The later step trace returned `GR_OK` for color/depth clear, scene begin, production draw, scene
end and present, then `capture=1` and `CAPTURE_OK`. The renderer now passes the documented
`RP_CLEAR_COLOR` value to `Clear` instead of hard-coded black; the fixture sets `0xFF060911` to
match the native clear. A `DrawPrimitiveUP` failure now propagates as `GR_GENERIC` instead of a
false success. The final fixture retains D3D9 point filtering, matching SDL's nearest sampler.

Exact visible Windows runtime command from the isolated Final directory:

```powershell
.\Conquest.exe /renderfixture /w
```

The resulting P6 had the exact 64-by-64, 12,301-byte format. The shared fixture comparison tool
ran on Linux with the independently exported visible GCC/Vulkan P6 and this Windows P6:

```text
python3 Tools/portability/compare_render_fixture.py \
  /tmp/frontierx-capture.ppm /tmp/frontierx-win32-render-fixture.ppm
```

It reported **PASS** with 2,496 versus 2,491 non-background pixels, 582 versus 597 distinct RGB
values, direct mean channel error 2.7085, Windows-to-Linux one-pixel-neighborhood maximum channel
error 4 and Linux-to-Windows maximum 8. Its documented acceptance limits are a populated,
diverse foreground, foreground count difference at most 10, direct mean at most 3 and
bidirectional neighborhood maximum at most 8. A synthetic blank comparison input returned
`FAIL` (zero foreground, direct mean 45.3162). The Linux GCC and Clang P6 files were previously
byte-identical. This is **synthetic Windows and Linux GPU runtime** evidence on the same
repository input, not a retail-media or gameplay frame comparison. Detailed Linux commands,
provenance, renderer-owner changes and remaining limits are in
[RENDERER_BACKEND_PROTOTYPE.md](RENDERER_BACKEND_PROTOTYPE.md).

After the final renderer and fixture edits, both supported native Windows 11/VS2022 Community
Win32 wrappers ran with `-NoDeploy` and exited zero. The **Final** wrapper built the nine explicit
engine DLLs, five application outputs and 18 campaign DLLs; it reported `Conquest.exe` 3,150,336,
`Globals.dll` 598,016, `Mission.dll` 1,075,712, `Trim.dll` 1,377,792 and `ZBatcher.dll` 406,016
bytes. The matching Final `Tools/abicheck/allmods.ps1` run using VS2022 `dumpbin` found zero
unsatisfied among 153 distinct `Mission` and two `Trim` imports demanded by all 18 freshly built
campaign DLLs. The **Debug** wrapper subsequently built the same targets and reported sizes
4,739,584, 967,168, 1,524,736, 1,693,184 and 433,152 bytes in the same order. Its immediate
same-build campaign ABI run also found zero unsatisfied of 153 `Mission` and two `Trim` imports.
These are native Windows compilation and binary import checks, separate from the Final fixture
runtime above; Debug fixture runtime, original retail parity and playable mission checks were not
performed after this final build. `-NoDeploy` did not alter the isolated runtime directory.

A subsequent **normal Windows Final runtime** launch from that isolated directory used
`.\Conquest.exe` without `/renderfixture`. The visible desktop briefly showed the game HUD,
space background and a wormhole after UAC was accepted; the process later ended. This confirms
that the depth-surface fix did not prevent that D3D9 scene from being presented, but it does not
establish a stable menu, units, mission progress or a playable scene. The diagnostic files were
updated during this run; `startup_log.txt` still includes repeated
`RenderMgr.cpp` `arch_index != -1` assertions, and `mission_diag.txt` includes the first Terran
terrain/object/script load entries. Because these files append across runs, their final lines
alone do not identify the terminating call. The next gameplay check must correlate a fresh,
isolated trace with the selected mission and the process exit. This is separate from the
successful repository-fixture image comparison.

### Repeatable isolated data-path setup

`Tools/windows/Invoke-RestoredGameValidation.ps1` now prepares the two inputs needed by an
existing isolated, restored-build test directory. It checks for the checkout's `Data/mat`
materials, shaders and textures and for an external data root containing `Objects`; it creates
only two junctions **outside the repository** (`Data` to the lawful external data root and
`DataOverlay/Mat` to the checkout material tree). It temporarily sets the per-user `DataPath` to
`DataOverlay;Data`, launches the test directory's `Conquest.exe`, waits for exit and restores the
previous registry value in `finally`. It refuses an existing entry pointing elsewhere and does
not stage binaries, databases, campaign DLLs or retail media. The test directory must already
contain a compatible restored-build runtime composition. Its normal invocation from a Windows
desktop PowerShell is:

```powershell
& .\Tools\windows\Invoke-RestoredGameValidation.ps1 `
  -TestDirectory '<isolated-test-directory>' `
  -RepositoryRoot (Get-Location).Path `
  -RetailDataDirectory '<lawfully-installed-DATA-directory>'
```

The 2026-09-25 helper review also closed an equality edge case in its path guard: a test root
equal to the checkout root previously passed the `StartsWith(root + separator)` test and could
create links inside the repository. The helper now rejects both equal and descendant test roots,
and rejects an external data root equal to or below the checkout root. Native Windows PowerShell
parsed the revised script with zero errors; two synthetic calls confirmed those equal-root cases
were rejected before registry or junction changes. The normal external-directory launch and
registry restoration remain covered by the earlier synthetic run, not by these rejection calls.

Native Windows PowerShell parsed this script with zero syntax errors. A **synthetic runtime**
check used a temporary test directory, a disposable `Objects` directory and a system utility
renamed to `Conquest.exe`; it did not launch the game or use retail data. The first check exposed
that `New-Item -Force` on the existing registry key removed its `DataPath`. The test value was
immediately restored and verified, and the script now creates that key only when absent. The
second check exited with the utility's code 2, created both expected junctions and preserved
`DataPath=DataOverlay;Data` before and after the run. This validates the helper's setup and
cleanup with an existing value; a real gameplay launch through it remains unperformed.
