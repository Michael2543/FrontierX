[CmdletBinding()]
param([switch]$UseExistingFinalBuild)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$private = $null
$stage = 'runner prerequisites'
try {
    if ([Diagnostics.Process]::GetCurrentProcess().SessionId -eq 0) {
        throw 'interactive Windows GPU runner session required'
    }
    if (!$env:FRONTIERX_RETAIL_ROOT) { throw 'lawful external installation missing' }
    $private = & python (Join-Path $PSScriptRoot 'private_workspace.py') --repository $root --prefix 'frontierx-m3-mission-'
    if ($LASTEXITCODE -or !$private) { throw 'retained private evidence root unavailable' }
    & python (Join-Path $root 'Tools\ci\input_manifest.py') $env:FRONTIERX_RETAIL_ROOT (Join-Path $env:FRONTIERX_RETAIL_ROOT 'FONTS\bauhmib.ttf') *> (Join-Path $private 'input-manifest-private.json')
    if ($LASTEXITCODE) { throw 'external input manifest failed' }
    if (!$UseExistingFinalBuild) {
        $stage = 'Final rebuild'
        & (Join-Path $root 'Build-Conquest.ps1') -Configuration Final -NoDeploy *> (Join-Path $private 'build-private.log')
    }
    $revision = (& git -C $root rev-parse HEAD).Trim()
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $vs = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -property installationPath
    if (!$vs) { throw 'native MSVC provenance unavailable' }
    $toolchain = 'MSVC ' + (Get-Content (Join-Path $vs 'VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt')).Trim()
    $dirty = [bool](& git -C $root status --porcelain)
    if ($dirty) { throw 'native CI source revision is dirty' }
    $runtime = Join-Path $private 'runtime'
    $stage = 'private runtime composition'
    New-Item $runtime -ItemType Directory | Out-Null
    # External runtime only; no original installation is overwritten.
    Get-ChildItem (Join-Path $env:FRONTIERX_RETAIL_ROOT 'DLL') -Filter '*.dll' -File | Copy-Item -Destination $runtime
    Get-ChildItem -LiteralPath $env:FRONTIERX_RETAIL_ROOT -File | Where-Object { $_.Extension -in @('.dll', '.ini', '.db') } | Copy-Item -Destination $runtime
    New-Item (Join-Path $runtime 'DATA') -ItemType Junction -Target (Join-Path $env:FRONTIERX_RETAIL_ROOT 'DATA') | Out-Null
    Get-ChildItem (Join-Path $root 'Code\App\Src\Final') -File | Where-Object { $_.Extension -in @('.dll', '.exe') } | Copy-Item -Destination $runtime -Force
    Get-ChildItem (Join-Path $root 'Code\Libs\ImplicitDLL\Release') -Filter '*.dll' -File | Copy-Item -Destination $runtime -Force
    Get-ChildItem (Join-Path $root 'Code\Libs\ExplicitDLL\release') -Filter '*.dll' -File | Copy-Item -Destination $runtime -Force
    $scripts = Join-Path $runtime 'Scripts'
    New-Item $scripts -ItemType Directory | Out-Null
    Get-ChildItem (Join-Path $root 'Code\App\Src\Scripts\Build') -Filter '*.dll' -File | Copy-Item -Destination $scripts -Force
    $profile = Get-Content (Join-Path $root 'Code\App\Src\Conquest.ini')
    # Match the explicit full-screen-movie exclusion already used on Linux.
    # Keep animated mission speech/portrait providers and completion events.
    $profile | Where-Object { $_.Trim() -notin @('VideoSystem.dll', 'IVideoSystem') } | ForEach-Object { $_ -replace '^IRenderPipeline\s*=\s*RendSft', 'IRenderPipeline = Rend0' } | Set-Content (Join-Path $runtime 'Conquest.ini')
    New-Item (Join-Path $runtime 'FONTS') -ItemType Junction -Target (Join-Path $env:FRONTIERX_RETAIL_ROOT 'FONTS') | Out-Null
    New-Item (Join-Path $runtime 'debug') -ItemType Directory | Out-Null
    $binary = Join-Path $runtime 'Conquest.exe'
    $binaryDigest = @{ bytes = (Get-Item $binary).Length; sha256 = (Get-FileHash $binary -Algorithm SHA256).Hash.ToLowerInvariant() }
    foreach ($mode in @('measured', 'fixed-source-period')) {
        $fixed = $mode -eq 'fixed-source-period'
        $directory = Join-Path $private $mode
        New-Item $directory -ItemType Directory | Out-Null
        $manifest = Get-Content (Join-Path $private 'input-manifest-private.json') -Raw | ConvertFrom-Json
        $protocol = @{ version=3; movement_owner='CQE_MOVE_SELECTED_UNITS/ObjComm'; frame_input=$mode;
            snapshot_boundary=$(if ($fixed) { 'after-physical' } else { 'after-logic' });
            control_time_units='1/1024 millisecond, truncated'; commands_before_ticks=@(32,64); last_tick=1024;
            window_input='active, centered pointer; host input excluded; close fatal';
            speech_completion='live original owner'; coverage='partial' }
        $execution = @{ revision=$revision; working_tree_dirty=$dirty; workflow_run=$env:GITHUB_RUN_ID;
            attempt=$env:GITHUB_RUN_ATTEMPT; job=$env:GITHUB_JOB; compiler='msvc'; scope='mission';
            configuration='Final'; toolchain=$toolchain; binary=$binaryDigest; timeout_seconds=$(if ($fixed) {600} else {120}) }
        $manifest | Add-Member -NotePropertyName protocol -NotePropertyValue $protocol
        $manifest | Add-Member -NotePropertyName execution -NotePropertyValue $execution
        $manifest | ConvertTo-Json -Depth 12 | Set-Content (Join-Path $directory 'input-manifest.json')
        $cases = @('repeat1','repeat2')
        if ($fixed) { $cases += @('256-save','256-reload','512-save','512-reload') }
        $results = @()
        foreach ($case in $cases) {
            $stage = "original mission $mode $case"
            $caseRoot = Join-Path $directory $case
            New-Item $caseRoot -ItemType Directory | Out-Null
            $save = Join-Path $caseRoot 'save'
            New-Item $save -ItemType Directory | Out-Null
            $env:FRONTIERX_M3_SOLTRAIN = '1'
            $env:FRONTIERX_M3_TRACE = Join-Path $caseRoot 'ticks.jsonl'
            $env:FRONTIERX_M3_JOURNAL = Join-Path $caseRoot 'speech'
            $env:FRONTIERX_M3_SAVE_ROOT = $save
            $env:FRONTIERX_M3_CONTROLLED_INPUT = '1'
            Remove-Item Env:FRONTIERX_M3_FIXED_FRAME,Env:FRONTIERX_M3_CHECKPOINT,Env:FRONTIERX_M3_CHECKPOINT_PREFIX -ErrorAction SilentlyContinue
            if ($fixed) { $env:FRONTIERX_M3_FIXED_FRAME = '1' }
            if ($case.Contains('-')) {
                $env:FRONTIERX_M3_CHECKPOINT = $case
                $env:FRONTIERX_M3_CHECKPOINT_PREFIX = Join-Path $caseRoot 'checkpoint'
            }
            $passed = $false
            $processExit = 'failed-to-start-or-run'
            try {
                if ((Get-FileHash $binary -Algorithm SHA256).Hash.ToLowerInvariant() -ne $binaryDigest.sha256) { throw 'native binary changed between cases' }
                & (Join-Path $root 'Tools\windows\Invoke-RestoredGameValidation.ps1') -TestDirectory $runtime -RepositoryRoot $root -RetailDataDirectory (Join-Path $env:FRONTIERX_RETAIL_ROOT 'DATA') -Arguments @('/w', '/skipmovies', '/skipintro', '/noconfirm') -TimeoutSeconds $execution.timeout_seconds *> (Join-Path $caseRoot 'validation-private.log')
                $passed = $true
                $processExit = 0
            } catch {
                $_ | Out-String | Set-Content (Join-Path $caseRoot 'failure-private.log')
                if ($_.Exception.Message -match 'timed out') { $processExit = 'timeout' }
                $validation = Join-Path $caseRoot 'validation-private.log'
                if (Test-Path $validation) {
                    $exitLine = Get-Content $validation | Where-Object { $_ -match '^Conquest exited with code (-?\d+)\.$' } | Select-Object -Last 1
                    if ($exitLine -and $exitLine -match '^Conquest exited with code (-?\d+)\.$') { $processExit = [int]$Matches[1] }
                }
            }
            $record = @{ case=$case; process_success=$passed; process_exit=$processExit; coverage='partial'; binary=$binaryDigest }
            $saved = Join-Path $save 'm3-checkpoint.mission'
            if (Test-Path $saved) { $record.save_bytes = @{ bytes=(Get-Item $saved).Length; sha256=(Get-FileHash $saved -Algorithm SHA256).Hash.ToLowerInvariant() } }
            $record | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $caseRoot 'result.json')
            $results += $passed
        }
        $comparisons = @()
        $compare = Join-Path $root 'Tools\portability\m3_compare.py'
        foreach ($case in $cases | Where-Object { $_ -ne 'repeat1' }) {
            & python $compare ticks (Join-Path $directory 'repeat1\ticks.jsonl') (Join-Path (Join-Path $directory $case) 'ticks.jsonl')
            $comparisons += $LASTEXITCODE
        }
        foreach ($case in $cases | Where-Object { $_.Contains('-') }) {
            $caseRoot = Join-Path $directory $case
            $stages = @('after-save'); if ($case.EndsWith('reload')) { $stages += 'after-reload' }
            foreach ($checkpointStage in $stages) {
                & python $compare checkpoint (Join-Path $caseRoot 'checkpoint-before-save.json') (Join-Path $caseRoot "checkpoint-$checkpointStage.json") --observed-only
                $comparisons += $LASTEXITCODE
            }
        }
        @{frame_input=$mode;process_success=$results;comparison_results=$comparisons;coverage='partial';byte_equality_contract='not established'} | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $directory 'result.json')
        if ($env:GITHUB_RUN_ID) {
            $index = Join-Path $env:FRONTIERX_M3_PRIVATE_ROOT 'mission-index'
            New-Item $index -ItemType Directory -Force | Out-Null
            $name = "$($env:GITHUB_RUN_ID)-$($env:GITHUB_RUN_ATTEMPT)-$revision-mission-$mode-msvc.json"
            @{private_directory=$directory;manifest=$manifest} | ConvertTo-Json -Depth 15 | Set-Content (Join-Path $index $name)
        }
    }
    throw 'partial native observation cannot satisfy M3 acceptance'
} catch {
    if ($private -and (Test-Path $private)) {
        $_ | Out-String | Set-Content (Join-Path $private 'failure-private.log')
    }
    Write-Output "FAIL: native Windows mission comparison at $stage; private diagnostics retained"
    exit 1
} finally {
    Remove-Item Env:FRONTIERX_M3_SOLTRAIN, Env:FRONTIERX_M3_TRACE, Env:FRONTIERX_M3_JOURNAL,Env:FRONTIERX_M3_SAVE_ROOT,Env:FRONTIERX_M3_CONTROLLED_INPUT,Env:FRONTIERX_M3_FIXED_FRAME,Env:FRONTIERX_M3_CHECKPOINT,Env:FRONTIERX_M3_CHECKPOINT_PREFIX -ErrorAction SilentlyContinue
}
