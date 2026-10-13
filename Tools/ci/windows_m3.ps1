[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$private = $null
$stage = 'private-workspace'
try {
    $private = & python (Join-Path $PSScriptRoot 'private_workspace.py') --repository $root --prefix 'frontierx-m3-'
    if ($LASTEXITCODE -or !$private) { throw 'retained private evidence root unavailable' }
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $vs = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -property installationPath
    if (!$vs -or !(Test-Path (Join-Path $env:DXSDK_DIR 'Include\d3dx9.h'))) { throw 'native toolchain missing' }
    $revision = & git -C $root rev-parse HEAD
    Write-Output "Revision $revision; native Windows source/synthetic scope"
    $tools = Get-Content (Join-Path $vs 'VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt')
    Write-Output "MSVC $tools; MSBuild native Win32 Debug/Final; external installed DXSDK"
    $dumpbin = Join-Path $vs "VC\Tools\MSVC\$tools\bin\Hostx64\x86\dumpbin.exe"
    foreach ($config in @('Debug', 'Final')) {
        $stage = "$config-rebuild-abi"
        # Campaign outputs are shared: check ABI immediately before next rebuild.
        & (Join-Path $root 'Build-Conquest.ps1') -Configuration $config -NoDeploy *> (Join-Path $private "$config-build.log")
        & (Join-Path $root 'Tools\abicheck\allmods.ps1') -DumpbinPath $dumpbin -BuiltGameDirectory (Join-Path $root "Code\App\Src\$config") -ScriptDirectory (Join-Path $root 'Code\App\Src\Scripts\Build') *> (Join-Path $private "$config-abi.log")
        Write-Output "Native Windows $config rebuild and same-build campaign ABI executed"
    }
    if ($env:GITHUB_OUTPUT) {
        'rebuild_abi=true' | Add-Content -LiteralPath $env:GITHUB_OUTPUT
    }
    $stage = 'launch-helper-regression'
    & (Join-Path $root 'Tools\windows\Test-RestoredValidation.ps1') -PrivateDirectory $private *> (Join-Path $private 'helper-tests.log')
    Write-Output 'Synthetic Windows launch/error/timeout and registry-restore checks executed'
    $vcvars = Join-Path $vs 'VC\Auxiliary\Build\vcvars32.bat'
    $stage = 'dacom-regression'
    $registryExe = Join-Path $private 'dacom-registry.exe'
    $registrySource = Join-Path $root 'Tools\portability\dacom_registry_probe.cpp'
    $registryInclude = Join-Path $root 'Code\Libs\Include'
    $registryLib = Join-Path $root 'Code\Libs\Static\Release\DACOM.lib'
    $registryCmd = Join-Path $private 'dacom-registry-build.cmd'
    @("@echo off", "call `"$vcvars`"", "if errorlevel 1 exit /b 1", "cl /nologo /EHsc /MT /W4 /I`"$registryInclude`" `"$registrySource`" /Fe:`"$registryExe`" /Fo:`"$private\dacom-registry.obj`" /link `"$registryLib`"", "exit /b %errorlevel%") | Set-Content $registryCmd
    & cmd /c $registryCmd *> (Join-Path $private 'dacom-registry-build.log')
    if ($LASTEXITCODE) { throw 'native DACOM regression build failed' }
    foreach ($engineConfig in @('Debug', 'Release')) {
        Copy-Item (Join-Path $root "Code\Libs\ImplicitDLL\$engineConfig\DACOM.dll") $private -Force
        Copy-Item (Join-Path $root "Code\Libs\ExplicitDLL\$engineConfig\D3DRenderPipe.dll") $private -Force
        foreach ($searchPath in @('unset', 'dot', 'absolute')) {
            & $registryExe $searchPath *> (Join-Path $private "dacom-registry-$engineConfig-$searchPath.log")
            if ($LASTEXITCODE) { throw 'native DACOM registry or module release regression failed' }
        }
    }
    Write-Output 'Native production DACOM registry and provider-release regressions executed'
    $source = Join-Path $root 'Tools\portability\rng_contract_probe.cpp'
    $stage = 'random-contract'
    $exe = Join-Path $private 'rng-contract.exe'
    $cmd = Join-Path $private 'rng-build.cmd'
    $randomOwner = Join-Path $root 'Code\App\Src\Final\RandomGen.obj'
    if (!(Test-Path -LiteralPath $randomOwner)) { throw 'rebuilt original RandomGen owner missing' }
    $randomIncludes = Join-Path $root 'Code\App\Src'
    $typeIncludes = Join-Path $root 'Code\Libs\Include'
    @("@echo off", "call `"$vcvars`"", "if errorlevel 1 exit /b 1", "cl /nologo /EHsc /MT /W4 /I`"$randomIncludes`" /I`"$typeIncludes`" `"$source`" /Fe:`"$exe`" /Fo:`"$private\rng-contract.obj`" /link `"$randomOwner`"", "exit /b %errorlevel%") | Set-Content $cmd
    & cmd /c $cmd *> (Join-Path $private 'rng-build.log')
    if ($LASTEXITCODE) { throw 'CRT characterization build failed' }
    & $exe
    if ($LASTEXITCODE) { throw 'CRT repeat failed' }
    $out = Join-Path $root 'out\m3-synthetic'
    New-Item $out -ItemType Directory -Force | Out-Null
    & $exe --trace | Set-Content -Encoding ascii (Join-Path $out 'windows.json')
    if ($LASTEXITCODE) { throw 'CRT trace failed' }
    & $exe --randomgen-trace | Set-Content -Encoding ascii (Join-Path $out 'windows-randomgen.json')
    if ($LASTEXITCODE) { throw 'original RandomGen trace failed' }
} catch {
    if ($private -and (Test-Path -LiteralPath $private)) {
        $_ | Out-String | Set-Content -LiteralPath (Join-Path $private 'failure-private.txt')
    }
    Write-Output "FAIL: native Windows CI scope at $stage; private runner diagnostics retained"
    exit 1
}
