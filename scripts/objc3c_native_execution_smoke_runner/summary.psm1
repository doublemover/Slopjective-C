Set-StrictMode -Version Latest

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:ScriptsRoot "objc3c_platform_host_evidence_producers.psm1") -Force -DisableNameChecking
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
    [Parameter(Mandatory = $true)][int]$SelectedNegativeCount,
    [AllowNull()][object]$FailureResult = $null,
    [string]$StatusOverride = ""
  )

  $resultItems = @($Results.ToArray())
  if ($null -ne $FailureResult) {
    $resultItems = @($resultItems) + @($FailureResult)
  }
  $caseTimingItems = @($CaseTimings.ToArray())
  $total = $resultItems.Count
  $passedCount = @($resultItems | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $status = if (-not [string]::IsNullOrWhiteSpace($StatusOverride)) {
    $StatusOverride
  } elseif ($failedCount -eq 0) {
    "PASS"
  } else {
    "FAIL"
  }
  $reportStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $slowestFixtures = @($caseTimingItems | Sort-Object -Property duration_seconds -Descending | Select-Object -First 10)
  $summary = [ordered]@{
    run_dir = Get-RepoRelativePath -Path $Context.run_dir -Root $Context.repo_root
    compile_command = if (Test-Path -LiteralPath $Context.native_exe -PathType Leaf) { Get-RepoRelativePath -Path $Context.native_exe -Root $Context.repo_root } else { $Context.native_exe }
    runtime_launch_contract_script = Get-RepoRelativePath -Path $Context.runtime_launch_contract_script -Root $Context.repo_root
    native_exe = if (Test-Path -LiteralPath $Context.native_exe -PathType Leaf) { Get-RepoRelativePath -Path $Context.native_exe -Root $Context.repo_root } else { $Context.native_exe }
    runtime_library = if (Test-Path -LiteralPath $Context.default_runtime_library -PathType Leaf) { Get-RepoRelativePath -Path $Context.default_runtime_library -Root $Context.repo_root } else { "" }
    runtime_library_relative_path = $Context.default_runtime_library_relative_path
    runtime_library_kind = $Context.runtime_library_kind
    runtime_library_names = @($Context.runtime_library_names)
    shared_runtime = [bool]$Context.shared_runtime
    live_runtime_dispatch_default_symbol = "objc3_runtime_dispatch_i32"
    target_platform_id = $Context.target_platform_id
    platform_ids = @($Context.supported_platform_ids)
    target_triple = $Context.target_triple
    host_promotion_state = $Context.host_promotion_state
    support_claim_published = $false
    object_artifact = $Context.object_artifact
    object_file_extension = $Context.object_file_extension
    object_format = $Context.object_format
    debug_format = $Context.debug_format
    link_input_model = [ordered]@{
      object_artifact = $Context.object_artifact
      object_file_extension = $Context.object_file_extension
      runtime_library = $Context.default_runtime_library_relative_path
      runtime_library_kind = $Context.runtime_library_kind
      runtime_library_names = @($Context.runtime_library_names)
      shared_runtime = [bool]$Context.shared_runtime
      loader_path_policy = $Context.loader_path_policy
    }
    runtime_load_environment = $Context.runtime_environment
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
    status = $status
    timing = [ordered]@{
      elapsed_seconds = [math]::Round($SuiteStopwatch.Elapsed.TotalSeconds, 6)
      stage_totals = Get-ExecutionSmokeStageTimings
      slowest_fixtures = @($slowestFixtures)
      fixture_timings = @($caseTimingItems)
    }
    results = @($resultItems)
  }
  if ([bool]$Context.shared_runtime -and -not [string]::IsNullOrWhiteSpace($Context.runtime_load_path_relative)) {
    $summary["load_path"] = @($Context.runtime_load_path_relative)
  }
  if ($null -ne $FailureResult) {
    $summary["failed_fixture"] = [ordered]@{
      kind = $FailureResult.kind
      fixture = $FailureResult.fixture
      fixture_index = $FailureResult.fixture_index
      total_selected_fixtures = $FailureResult.total_selected_fixtures
      last_completed_fixture = $FailureResult.last_completed_fixture
    }
    $summary["failure"] = $FailureResult.failure
  }
  $reportStopwatch.Stop()
  Add-StageDuration -StageKey "output_report_seconds" -DurationSeconds ([math]::Round($reportStopwatch.Elapsed.TotalSeconds, 6))
  $summary.timing.stage_totals = Get-ExecutionSmokeStageTimings
  $summary.timing.elapsed_seconds = [math]::Round($SuiteStopwatch.Elapsed.TotalSeconds, 6)
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $Context.summary_path -Encoding utf8
  Write-Objc3cDarwinRuntimeLoadProbeEvidence `
    -RepoRoot $Context.repo_root `
    -PlatformId $Context.target_platform_id `
    -TargetTriple $Context.target_triple `
    -SummaryPath $Context.summary_path `
    -RuntimeLibraryPath $Context.default_runtime_library `
    -RuntimeLibraryRelativePath $Context.default_runtime_library_relative_path `
    -LoaderPathPolicy $Context.loader_path_policy
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $Context.summary_path -Root $Context.repo_root)"
  Write-Output "status: $status"
  $global:LASTEXITCODE = if ($status -eq "PASS") { 0 } else { 1 }
}

function New-ExecutionSmokeFailureResult {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureKind,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][int]$FixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][string]$LastCompletedFixture,
    [Parameter(Mandatory = $true)][System.Management.Automation.ErrorRecord]$ErrorRecord
  )

  $message = $ErrorRecord.Exception.Message
  if ([string]::IsNullOrWhiteSpace($message)) {
    $message = [string]$ErrorRecord
  }
  $failure = [ordered]@{
    message = $message
    fully_qualified_error_id = [string]$ErrorRecord.FullyQualifiedErrorId
    category = [string]$ErrorRecord.CategoryInfo.Category
    script_stack_trace = [string]$ErrorRecord.ScriptStackTrace
  }

  return [pscustomobject][ordered]@{
    kind = $FixtureKind
    fixture = $FixtureRel
    fixture_index = $FixtureIndex
    total_selected_fixtures = $TotalSelectedFixtures
    last_completed_fixture = $LastCompletedFixture
    passed = $false
    failure = $failure
    timing = [ordered]@{
      compile_seconds = 0.0
      link_seconds = 0.0
      run_seconds = 0.0
    }
  }
}

function Write-FailedExecutionSmokeSummary {
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
    [Parameter(Mandatory = $true)][int]$SelectedNegativeCount,
    [Parameter(Mandatory = $true)][string]$FailedFixtureKind,
    [Parameter(Mandatory = $true)][string]$FailedFixtureRel,
    [Parameter(Mandatory = $true)][int]$FailedFixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][string]$LastCompletedFixture,
    [Parameter(Mandatory = $true)][System.Management.Automation.ErrorRecord]$ErrorRecord
  )

  $failureResult = New-ExecutionSmokeFailureResult `
    -FixtureKind $FailedFixtureKind `
    -FixtureRel $FailedFixtureRel `
    -FixtureIndex $FailedFixtureIndex `
    -TotalSelectedFixtures $TotalSelectedFixtures `
    -LastCompletedFixture $LastCompletedFixture `
    -ErrorRecord $ErrorRecord
  Write-ExecutionSmokeSummary `
    -Context $Context `
    -SuiteStopwatch $SuiteStopwatch `
    -Results $Results `
    -CaseTimings $CaseTimings `
    -FixtureList $FixtureList `
    -FixtureGlob $FixtureGlob `
    -ShardIndex $ShardIndex `
    -ShardCount $ShardCount `
    -Limit $Limit `
    -SelectedPositiveCount $SelectedPositiveCount `
    -SelectedNegativeCount $SelectedNegativeCount `
    -FailureResult $failureResult `
    -StatusOverride "FAIL"
}

Export-ModuleMember -Function @(
  "Write-FailedExecutionSmokeSummary",
  "Write-ExecutionSmokeSummary"
)
