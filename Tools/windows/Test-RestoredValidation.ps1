[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$PrivateDirectory)
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$helper=Join-Path $PSScriptRoot 'Invoke-RestoredGameValidation.ps1'
$base=Join-Path $PrivateDirectory ('helper-' + [guid]::NewGuid())
$runtime=Join-Path $base 'runtime'
$data=Join-Path $base 'synthetic-data'
New-Item $runtime,(Join-Path $data 'Objects') -ItemType Directory -Force | Out-Null
$registry='HKCU:\Software\Fever Pitch\Conquest: Frontier Wars\1.00'
$before=Get-ItemProperty -Path $registry -Name DataPath -ErrorAction SilentlyContinue
$had=$null -ne $before
function Assert-Restored {
    $after=Get-ItemProperty -Path $registry -Name DataPath -ErrorAction SilentlyContinue
    if (($null -ne $after) -ne $had) { throw 'registry presence changed' }
    if ($had -and $after.DataPath -cne $before.DataPath) { throw 'registry value changed' }
}
Copy-Item (Join-Path $env:WINDIR 'System32\where.exe') (Join-Path $runtime 'Conquest.exe')
& $helper -TestDirectory $runtime -RepositoryRoot $root -RetailDataDirectory $data -Arguments @('/?') -TimeoutSeconds 10
Assert-Restored
$failed=$false
try { & $helper -TestDirectory $runtime -RepositoryRoot $root -RetailDataDirectory $data -Arguments @('__FRONTIERX_M3_NO_SUCH_EXECUTABLE__') -TimeoutSeconds 10 } catch { $failed=$true }
if (!$failed) { throw 'nonzero process was accepted' }
Assert-Restored
Copy-Item (Join-Path $env:WINDIR 'System32\ping.exe') (Join-Path $runtime 'Conquest.exe') -Force
$failed=$false
try { & $helper -TestDirectory $runtime -RepositoryRoot $root -RetailDataDirectory $data -Arguments @('127.0.0.1','-n','10') -TimeoutSeconds 1 } catch { $failed=$true }
if (!$failed) { throw 'timeout was accepted' }
Assert-Restored
$failed=$false
try { & $helper -TestDirectory $root -RepositoryRoot $root -RetailDataDirectory $data } catch { $failed=$true }
if (!$failed) { throw 'repository runtime was accepted' }
Assert-Restored
Write-Output 'Synthetic Windows helper checks: success/nonzero/timeout/outside-root and registry restoration verified'
