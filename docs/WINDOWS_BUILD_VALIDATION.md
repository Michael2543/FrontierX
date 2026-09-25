# VS2022 Win32 Build Validation

Status snapshot: **2026-09-25**. This record covers native Windows compilation and linking of the
supported Debug and Final wrappers after the current FrontierX changes. It is **build evidence**,
not a game launch, retail runtime check, Windows renderer capture or native Linux game build.

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
