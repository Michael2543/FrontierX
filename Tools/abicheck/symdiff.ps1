[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$DumpbinPath,

    [Parameter(Mandatory = $true)]
    [string]$RetailMissionDll,

    [Parameter(Mandatory = $true)]
    [string]$BuiltMissionDll,

    [Parameter(Mandatory = $true)]
    [string]$ScriptDirectory
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'AbiCheck.Common.ps1')

Assert-AbiCheckWindowsHost
Assert-AbiCheckFile -Path $DumpbinPath -Description 'dumpbin executable'
Assert-AbiCheckFile -Path $RetailMissionDll -Description 'Retail Mission.dll'
Assert-AbiCheckFile -Path $BuiltMissionDll -Description 'Built Mission.dll'
Assert-AbiCheckDirectory -Path $ScriptDirectory -Description 'Campaign script directory'

$scriptFiles = @(Get-ChildItem -LiteralPath $ScriptDirectory -Filter '*.dll' -File | Sort-Object Name)
if ($scriptFiles.Count -eq 0) {
    throw "Campaign script directory contains no DLLs: $ScriptDirectory"
}

$retail = Get-AbiExports -DumpbinPath $DumpbinPath -Path $RetailMissionDll
$ours = Get-AbiExports -DumpbinPath $DumpbinPath -Path $BuiltMissionDll

Write-Output "=== EXPORT COUNTS ==="
Write-Output ("retail mission.dll : {0}" -f $retail.Count)
Write-Output ("our    Mission.dll : {0}" -f $ours.Count)

$missing = @($retail | Where-Object { -not $ours.Contains($_) })
$added   = @($ours   | Where-Object { -not $retail.Contains($_) })

Write-Output ""
Write-Output ("=== IN RETAIL BUT MISSING FROM OURS: {0} ===" -f $missing.Count)
$missing | Sort-Object | ForEach-Object { Write-Output "  $_" }

Write-Output ""
Write-Output ("=== IN OURS BUT NOT RETAIL (additions): {0} ===" -f $added.Count)
$added | Sort-Object | ForEach-Object { Write-Output "  $_" }

# --- what the retail scripts actually demand ---
Write-Output ""
Write-Output "=== SCRIPT IMPORT SATISFACTION ==="
$allUnsat = @{}
foreach ($f in $scriptFiles) {
    $need = Get-AbiImports -DumpbinPath $DumpbinPath -Path $f.FullName -ModulePattern 'mission|conquest'
    if ($need.Count -eq 0) {
        Write-Output ("{0,-16} (no mission/conquest imports)" -f $f.Name)
        continue
    }
    $unsat = @($need | Where-Object { -not $ours.Contains($_) })
    Write-Output ("{0,-16} needs {1,4}   UNSATISFIED {2}" -f $f.Name, $need.Count, $unsat.Count)
    foreach ($u in $unsat) { $allUnsat[$u] = $true }
}

Write-Output ""
Write-Output ("=== DISTINCT UNSATISFIED SYMBOLS: {0} ===" -f $allUnsat.Keys.Count)
$allUnsat.Keys | Sort-Object | ForEach-Object { Write-Output "  $_" }

if ($allUnsat.Keys.Count -ne 0) {
    throw "$($allUnsat.Keys.Count) distinct campaign imports are not satisfied by '$BuiltMissionDll'."
}
