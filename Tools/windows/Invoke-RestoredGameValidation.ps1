[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$TestDirectory,

    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot,

    [Parameter(Mandatory = $true)]
    [string]$RetailDataDirectory,

    [string[]]$Arguments = @(),

    [ValidateRange(0, 3600)]
    [int]$TimeoutSeconds = 0
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Get-ExistingDirectory([string]$Path, [string]$Description) {
    $resolved = Resolve-Path -LiteralPath $Path -ErrorAction Stop
    if (-not (Test-Path -LiteralPath $resolved.Path -PathType Container)) {
        throw "$Description is not a directory: $Path"
    }
    return $resolved.Path
}

function Assert-Junction([string]$Path, [string]$Target) {
    $existing = Get-Item -LiteralPath $Path -Force -ErrorAction SilentlyContinue
    if ($null -eq $existing) {
        New-Item -ItemType Junction -Path $Path -Target $Target | Out-Null
        $existing = Get-Item -LiteralPath $Path -Force
    }
    if ($existing.LinkType -ne 'Junction') {
        throw "Expected a junction to '$Target' at '$Path'; found a different entry."
    }
    $actual = @($existing.Target)[0]
    if ([System.IO.Path]::GetFullPath($actual).TrimEnd('\') -ine
        [System.IO.Path]::GetFullPath($Target).TrimEnd('\')) {
        throw "Expected a junction to '$Target' at '$Path'; found a different entry."
    }
}

$testRoot = Get-ExistingDirectory $TestDirectory 'Test root'
$repoRoot = Get-ExistingDirectory $RepositoryRoot 'Repository root'
$retailRoot = Get-ExistingDirectory $RetailDataDirectory 'External data root'
$repoPrefix = $repoRoot.TrimEnd('\') + '\'
if ($testRoot -ieq $repoRoot -or
    $testRoot.StartsWith($repoPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw 'The test root must be outside the repository.'
}
if ($retailRoot -ieq $repoRoot -or
    $retailRoot.StartsWith($repoPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw 'The external data root must be outside the repository.'
}
$exe = Join-Path $testRoot 'Conquest.exe'
$materialRoot = Join-Path $repoRoot 'Data\mat'
foreach ($path in @($exe, (Join-Path $materialRoot 'materials'),
        (Join-Path $materialRoot 'shaders'), (Join-Path $materialRoot 'textures'),
        (Join-Path $retailRoot 'Objects'))) {
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Required restored-build input is missing: $path"
    }
}

$overlay = Join-Path $testRoot 'DataOverlay'
New-Item -ItemType Directory -Path $overlay -Force | Out-Null
Assert-Junction (Join-Path $testRoot 'Data') $retailRoot
Assert-Junction (Join-Path $overlay 'Mat') $materialRoot

$registryPath = 'HKCU:\Software\Fever Pitch\Conquest: Frontier Wars\1.00'
if (-not (Test-Path -LiteralPath $registryPath)) {
    New-Item -Path $registryPath -Force | Out-Null
}
$oldValue = Get-ItemProperty -Path $registryPath -Name DataPath -ErrorAction SilentlyContinue
$hadValue = $null -ne $oldValue
$previousPath = if ($hadValue) { $oldValue.DataPath } else { $null }

try {
    Set-ItemProperty -Path $registryPath -Name DataPath -Value 'DataOverlay;Data'
    $launch = @{ FilePath = $exe; WorkingDirectory = $testRoot; PassThru = $true }
    if ($Arguments.Count) { $launch.ArgumentList = $Arguments }
    $process = Start-Process @launch
    # Keep a handle before waiting so ExitCode remains available in Windows PS.
    $null = $process.Handle
    if ($TimeoutSeconds -gt 0) {
        if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
            $process.Kill()
            $process.WaitForExit()
            throw 'Restored game validation timed out.'
        }
    } else {
        $process.WaitForExit()
    }
    $process.Refresh()
    Write-Output "Conquest exited with code $($process.ExitCode)."
    if ($process.ExitCode -ne 0) { throw 'Restored game process failed.' }
}
finally {
    if ($hadValue) {
        Set-ItemProperty -Path $registryPath -Name DataPath -Value $previousPath
    }
    else {
        Remove-ItemProperty -Path $registryPath -Name DataPath -ErrorAction SilentlyContinue
    }
}
