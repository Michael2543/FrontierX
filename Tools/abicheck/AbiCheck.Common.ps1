Set-StrictMode -Version Latest

function Assert-AbiCheckWindowsHost {
    if ([System.Environment]::OSVersion.Platform -ne [System.PlatformID]::Win32NT) {
        throw 'FrontierX ABI checks require native Windows, Visual Studio dumpbin, and PE binaries. Do not run them through Docker, Podman, Wine, or Linux PowerShell.'
    }
}

function Assert-AbiCheckFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "$Description does not exist: $Path"
    }
}

function Assert-AbiCheckDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Container)) {
        throw "$Description does not exist: $Path"
    }
}

function Invoke-AbiDumpbin {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DumpbinPath,

        [Parameter(Mandatory = $true)]
        [ValidateSet('/exports', '/imports')]
        [string]$Mode,

        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    $output = @(& $DumpbinPath $Mode $Path 2>&1)
    if ($LASTEXITCODE -ne 0) {
        $details = ($output | Select-Object -Last 5) -join [Environment]::NewLine
        throw "dumpbin $Mode failed for '$Path' with exit code $LASTEXITCODE.$([Environment]::NewLine)$details"
    }
    if ($output.Count -eq 0) {
        throw "dumpbin $Mode returned no output for '$Path'."
    }

    return $output
}

function Get-AbiExports {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DumpbinPath,

        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    $output = Invoke-AbiDumpbin -DumpbinPath $DumpbinPath -Mode '/exports' -Path $Path
    $names = New-Object System.Collections.Generic.HashSet[string]
    $sawHeader = $false
    foreach ($line in $output) {
        if ($line -match '^\s+ordinal\s+hint\s+RVA\s+name') {
            $sawHeader = $true
            continue
        }
        if (-not $sawHeader) {
            continue
        }
        if ($line -match '^\s*\d+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]{8}\s+(\S+)') {
            [void]$names.Add($Matches[1])
        }
        elseif ($line -match '^\s*\d+\s+[0-9A-Fa-f]+\s+(\S+)$') {
            [void]$names.Add($Matches[1])
        }
    }

    if (-not $sawHeader) {
        throw "Could not parse the export table header from '$Path'."
    }

    return ,$names
}

function Get-AbiImports {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DumpbinPath,

        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [string]$ModulePattern
    )

    $output = Invoke-AbiDumpbin -DumpbinPath $DumpbinPath -Mode '/imports' -Path $Path
    $names = New-Object System.Collections.Generic.HashSet[string]
    $sawModule = $false
    $inTarget = $false
    foreach ($line in $output) {
        if ($line -match '^\s{2,}(\S+\.(dll|exe))\s*$') {
            $sawModule = $true
            $inTarget = ($Matches[1] -match $ModulePattern)
            continue
        }
        if ($inTarget -and $line -match '^\s+[0-9A-Fa-f]+\s+(\S+)\s*$') {
            [void]$names.Add($Matches[1])
        }
    }

    if (-not $sawModule) {
        throw "Could not parse any import modules from '$Path'."
    }

    return ,$names
}

function Get-AbiImportsByModule {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DumpbinPath,

        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    $output = Invoke-AbiDumpbin -DumpbinPath $DumpbinPath -Mode '/imports' -Path $Path
    $importsByModule = @{}
    $currentModule = $null
    foreach ($line in $output) {
        if ($line -match '^\s{2,}(\S+\.(dll|exe))\s*$') {
            $currentModule = $Matches[1]
            if (-not $importsByModule.ContainsKey($currentModule)) {
                $importsByModule[$currentModule] = New-Object System.Collections.Generic.HashSet[string]
            }
            continue
        }
        if ($null -ne $currentModule -and $line -match '^\s+[0-9A-Fa-f]+\s+(\S+)\s*$') {
            [void]$importsByModule[$currentModule].Add($Matches[1])
        }
    }

    if ($importsByModule.Count -eq 0) {
        throw "Could not parse any import modules from '$Path'."
    }

    return ,$importsByModule
}