[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$DumpbinPath,

    [Parameter(Mandatory = $true)]
    [string]$BuiltGameDirectory,

    [Parameter(Mandatory = $true)]
    [string]$ScriptDirectory
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'AbiCheck.Common.ps1')

Assert-AbiCheckWindowsHost
Assert-AbiCheckFile -Path $DumpbinPath -Description 'dumpbin executable'
Assert-AbiCheckDirectory -Path $BuiltGameDirectory -Description 'Built game directory'
Assert-AbiCheckDirectory -Path $ScriptDirectory -Description 'Campaign script directory'

$scriptFiles = @(Get-ChildItem -LiteralPath $ScriptDirectory -Filter '*.dll' -File | Sort-Object Name)
if ($scriptFiles.Count -eq 0) {
    throw "Campaign script directory contains no DLLs: $ScriptDirectory"
}

$builtModules = @{
    'Mission.dll'  = Join-Path $BuiltGameDirectory 'Mission.dll'
    'Globals.dll'  = Join-Path $BuiltGameDirectory 'Globals.dll'
    'Trim.dll'     = Join-Path $BuiltGameDirectory 'Trim.dll'
    'ZBatcher.dll' = Join-Path $BuiltGameDirectory 'ZBatcher.dll'
    'Conquest.exe' = Join-Path $BuiltGameDirectory 'Conquest.exe'
}

foreach ($moduleName in $builtModules.Keys) {
    Assert-AbiCheckFile -Path $builtModules[$moduleName] -Description "Built $moduleName"
}

# Every module every retail script imports from, with counts
$modCount = @{}
$perScript = @{}
foreach ($scriptFile in $scriptFiles) {
    $scriptImports = Get-AbiImportsByModule -DumpbinPath $DumpbinPath -Path $scriptFile.FullName
    foreach ($moduleName in $scriptImports.Keys) {
        if (-not $modCount.ContainsKey($moduleName)) {
            $modCount[$moduleName] = 0
        }
        $modCount[$moduleName]++
        if (-not $perScript.ContainsKey($moduleName)) {
            $perScript[$moduleName] = New-Object System.Collections.Generic.HashSet[string]
        }
        foreach ($symbol in $scriptImports[$moduleName]) {
            [void]$perScript[$moduleName].Add($symbol)
        }
    }
}

Write-Output "=== MODULES THE RETAIL SCRIPTS BIND TO ==="
foreach ($k in ($modCount.Keys | Sort-Object)) {
    $syms = 0; if ($perScript.ContainsKey($k)) { $syms = $perScript[$k].Count }
    Write-Output ("  {0,-20} referenced by {1,2} scripts, {2,4} distinct symbols" -f $k, $modCount[$k], $syms)
}

Write-Output ""
Write-Output "=== SATISFACTION AGAINST OUR FRESH BUILDS ==="
$allUnsatisfied = New-Object System.Collections.Generic.HashSet[string]
foreach ($moduleName in ($perScript.Keys | Sort-Object)) {
    $builtModuleName = @($builtModules.Keys | Where-Object { $_ -ieq $moduleName } | Select-Object -First 1)
    if ($builtModuleName.Count -eq 0) {
        Write-Output ("  {0,-20} (retail/third-party, not built by us - skipped)" -f $moduleName)
        continue
    }

    $exports = Get-AbiExports -DumpbinPath $DumpbinPath -Path $builtModules[$builtModuleName[0]]
    $unsatisfied = @($perScript[$moduleName] | Where-Object { -not $exports.Contains($_) })
    Write-Output ("  {0,-20} demanded {1,4}   UNSATISFIED {2}" -f $moduleName, $perScript[$moduleName].Count, $unsatisfied.Count)
    foreach ($symbol in ($unsatisfied | Sort-Object)) {
        Write-Output "      MISSING: $symbol"
        [void]$allUnsatisfied.Add("$moduleName!$symbol")
    }
}

if ($allUnsatisfied.Count -ne 0) {
    throw "$($allUnsatisfied.Count) distinct campaign imports are not satisfied by the built modules."
}
