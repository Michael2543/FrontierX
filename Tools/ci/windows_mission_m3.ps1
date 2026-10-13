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
    foreach ($repeat in @(1, 2)) {
        $stage = "original mission run $repeat"
        $env:FRONTIERX_M3_SOLTRAIN = '1'
        $env:FRONTIERX_M3_TRACE = Join-Path $private "$repeat.jsonl"
        $env:FRONTIERX_M3_JOURNAL = Join-Path $private "$repeat-speech"
        & (Join-Path $root 'Tools\windows\Invoke-RestoredGameValidation.ps1') -TestDirectory $runtime -RepositoryRoot $root -RetailDataDirectory (Join-Path $env:FRONTIERX_RETAIL_ROOT 'DATA') -Arguments @('/w', '/skipmovies', '/skipintro', '/noconfirm') -TimeoutSeconds 120 *> (Join-Path $private "$repeat-validation-private.log")
    }
    $stage = 'semantic tick comparison'
    & python (Join-Path $root 'Tools\portability\m3_compare.py') ticks (Join-Path $private '1.jsonl') (Join-Path $private '2.jsonl')
    if ($LASTEXITCODE) { throw 'mission comparison failed or incomplete' }
} catch {
    if ($private -and (Test-Path $private)) {
        $_ | Out-String | Set-Content (Join-Path $private 'failure-private.log')
    }
    Write-Output "FAIL: native Windows mission comparison at $stage; private diagnostics retained"
    exit 1
} finally {
    Remove-Item Env:FRONTIERX_M3_SOLTRAIN, Env:FRONTIERX_M3_TRACE, Env:FRONTIERX_M3_JOURNAL -ErrorAction SilentlyContinue
}
