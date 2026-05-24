Set-StrictMode -Version Latest

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "timings.psm1") -Force -DisableNameChecking

function Write-ExecutionSmokeSummary {
  param(
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][System.Collections.Generic.List[object]]$Results,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][System.Collections.Generic.List[object]]$CaseTimings,
    [string]$FixtureList,
    [string]$FixtureGlob,
    [Parameter(Mandatory = $true)][int]$ShardIndex,
    [Parameter(Mandatory = $true)][int]$ShardCount,
    [Parameter(Mandatory = $true)][int]$Limit,
    [Parameter(Mandatory = $true)][int]$SelectedPositiveCount,
    [Parameter(Mandatory = $true)][int]$SelectedNegativeCount
  )

  $resultItems = @($Results.ToArray())
  $caseTimingItems = @($CaseTimings.ToArray())
  $total = $resultItems.Count
  $passedCount = @($resultItems | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $reportStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $slowestFixtures = @($caseTimingItems | Sort-Object -Property duration_seconds -Descending | Select-Object -First 10)
  $summary = [ordered]@{
    run_dir = Get-RepoRelativePath -Path $Context.run_dir -Root $Context.repo_root
    compile_command = if (Test-Path -LiteralPath $Context.native_exe -PathType Leaf) { Get-RepoRelativePath -Path $Context.native_exe -Root $Context.repo_root } else { $Context.native_exe }
    runtime_launch_contract_script = Get-RepoRelativePath -Path $Context.runtime_launch_contract_script -Root $Context.repo_root
    native_exe = if (Test-Path -LiteralPath $Context.native_exe -PathType Leaf) { Get-RepoRelativePath -Path $Context.native_exe -Root $Context.repo_root } else { $Context.native_exe }
    runtime_library = if (Test-Path -LiteralPath $Context.default_runtime_library -PathType Leaf) { Get-RepoRelativePath -Path $Context.default_runtime_library -Root $Context.repo_root } else { "" }
    live_runtime_dispatch_default_symbol = "objc3_runtime_dispatch_i32"
    sanitizer_variant = $Context.sanitizer_variant
    sanitizer_runtime_dir = if (-not [string]::IsNullOrWhiteSpace($Context.sanitizer_runtime_dir)) { Get-RepoRelativePath -Path $Context.sanitizer_runtime_dir -Root $Context.repo_root } else { "" }
    sanitizer_environment = $Context.sanitizer_environment
    clang = $Context.clang_command
    llc = $Context.llc_command
    llc_source = $Context.llc_source_path
    selection = [ordered]@{
      fixture_list = $FixtureList
      fixture_glob = $FixtureGlob
      shard_index = $ShardIndex
      shard_count = $ShardCount
      limit = $Limit
      selected_positive = $SelectedPositiveCount
      selected_negative = $SelectedNegativeCount
    }
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = if ($failedCount -eq 0) { "PASS" } else { "FAIL" }
    timing = [ordered]@{
      elapsed_seconds = [math]::Round($SuiteStopwatch.Elapsed.TotalSeconds, 6)
      stage_totals = Get-ExecutionSmokeStageTimings
      slowest_fixtures = @($slowestFixtures)
      fixture_timings = @($caseTimingItems)
    }
    results = @($resultItems)
  }
  $reportStopwatch.Stop()
  Add-StageDuration -StageKey "output_report_seconds" -DurationSeconds ([math]::Round($reportStopwatch.Elapsed.TotalSeconds, 6))
  $summary.timing.stage_totals = Get-ExecutionSmokeStageTimings
  $summary.timing.elapsed_seconds = [math]::Round($SuiteStopwatch.Elapsed.TotalSeconds, 6)
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $Context.summary_path -Encoding utf8
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $Context.summary_path -Root $Context.repo_root)"
  Write-Output "status: PASS"
  $global:LASTEXITCODE = 0
}

Export-ModuleMember -Function @(
  "Write-ExecutionSmokeSummary"
)
