Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_measurements.psm1") -Force -DisableNameChecking

function Write-Objc3cNativePerfSummary {
  param(
    [object]$Config,
    [object[]]$FixtureSets,
    [int]$DispatchFixtureCount,
    [object[]]$Results,
    [int]$ResolvedMaxElapsedMs,
    [bool]$BuildExecuted,
    [double]$BuildElapsedMs,
    [object]$CacheProof,
    [object]$CacheInvalidationProof,
    [object]$MacroHostProof,
    [object]$DocsGenerationProof,
    [bool]$HadFatalError,
    [string]$FatalErrorMessage,
    [ref]$Status
  )

  $measurements = Get-Objc3cNativePerfSummaryMeasurements `
    -Config $Config `
    -Results $Results `
    -ResolvedMaxElapsedMs $ResolvedMaxElapsedMs `
    -CacheProof $CacheProof `
    -HadFatalError $HadFatalError

  $total = $measurements.total
  $passedCount = $measurements.passed_count
  $failedCount = $measurements.failed_count
  $totalElapsedMs = $measurements.total_elapsed_ms
  $minFixtureElapsedMs = $measurements.min_fixture_elapsed_ms
  $maxFixtureElapsedMs = $measurements.max_fixture_elapsed_ms
  $avgFixtureElapsedMs = $measurements.avg_fixture_elapsed_ms
  $budgetBreached = $measurements.budget_breached
  $timingGateViolated = $measurements.timing_gate_violated
  $budgetMarginMs = $measurements.budget_margin_ms
  $statusValue = $measurements.status
  $Status.Value = $statusValue

  $summary = [ordered]@{
    contract_id = "objc3c.compiler.throughput.summary.v1"
    benchmark_kind = "native-direct-compile-throughput"
    run_id = $Config.run_id
    run_dir = (Get-RepoRelativePath -Path $Config.run_dir -Root $Config.repo_root)
    generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    fixture_set = "tests/tooling/fixtures/native/recovery/positive"
    fixture_sets = $FixtureSets
    dispatch_fixture_count = $DispatchFixtureCount
    extra_positive_fixture_dirs = $Config.extra_positive_fixture_dir_list
    budget_profile = $Config.resolved_budget_profile
    base_per_fixture_budget_ms = $Config.default_per_fixture_budget_ms
    launch_overhead_per_fixture_ms = $Config.resolved_launch_overhead_per_fixture_ms
    per_fixture_budget_ms = $Config.resolved_per_fixture_budget_ms
    explicit_max_elapsed_ms = $Config.explicit_max_elapsed_ms
    max_elapsed_ms = $ResolvedMaxElapsedMs
    total_elapsed_ms = $totalElapsedMs
    avg_fixture_elapsed_ms = $avgFixtureElapsedMs
    min_fixture_elapsed_ms = $minFixtureElapsedMs
    max_fixture_elapsed_ms = $maxFixtureElapsedMs
    budget_breached = $budgetBreached
    timing_gate_enforced = $Config.timing_gate_enforced
    timing_gate_violated = $timingGateViolated
    budget_margin_ms = $budgetMarginMs
    build_executed = $BuildExecuted
    build_elapsed_ms = $BuildElapsedMs
    cache_proof = $CacheProof
    cache_invalidation_proof = $CacheInvalidationProof
    macro_host_cache_proof = $MacroHostProof
    docs_generation_proof = $DocsGenerationProof
    workload_summary = [ordered]@{
      direct_compile_fixture_count = $total
      direct_compile_total_elapsed_ms = $totalElapsedMs
      cache_proof_elapsed_ms = (Get-Objc3cNativePerfPairElapsedMs -Proof $CacheProof)
      cache_invalidation_elapsed_ms = (Get-Objc3cNativePerfPairElapsedMs -Proof $CacheInvalidationProof)
      macro_host_cache_elapsed_ms = (Get-Objc3cNativePerfPairElapsedMs -Proof $MacroHostProof)
      docs_generation_elapsed_ms = (Get-Objc3cNativePerfDocsElapsedMs -DocsGenerationProof $DocsGenerationProof)
    }
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = $statusValue
    fatal_error = $FatalErrorMessage
    results = $Results
  }
  $summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $Config.summary_path -Encoding utf8
  New-Item -ItemType Directory -Force -Path $Config.report_root | Out-Null
  $summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $Config.report_path -Encoding utf8

  $run1HitValue = if ($null -ne $CacheProof.run1) { [bool]$CacheProof.run1.cache_hit } else { $false }
  $run2HitValue = if ($null -ne $CacheProof.run2) { [bool]$CacheProof.run2.cache_hit } else { $false }
  Write-Output ("budget_ms: max={0} total={1} margin={2}" -f $ResolvedMaxElapsedMs, $totalElapsedMs, $budgetMarginMs)
  Write-Output ("budget_profile: profile={0} base_per_fixture_ms={1} launch_overhead_per_fixture_ms={2} per_fixture_ms={3} explicit_max={4}" -f $Config.resolved_budget_profile, $Config.default_per_fixture_budget_ms, $Config.resolved_launch_overhead_per_fixture_ms, $Config.resolved_per_fixture_budget_ms, $Config.explicit_max_elapsed_ms)
  Write-Output ("timing_gate: enforced={0} violated={1}" -f $Config.timing_gate_enforced, $timingGateViolated)
  Write-Output ("cache_proof: status={0} run1_hit={1} run2_hit={2}" -f $CacheProof.status, $run1HitValue, $run2HitValue)
  Write-Output ("summary: total={0} passed={1} failed={2}" -f $total, $passedCount, $failedCount)
  Write-Output ("summary_path: {0}" -f (Get-RepoRelativePath -Path $Config.summary_path -Root $Config.repo_root))
  Write-Output ("report_path: {0}" -f (Get-RepoRelativePath -Path $Config.report_path -Root $Config.repo_root))
  if ($budgetBreached -and -not $Config.timing_gate_enforced) {
    Write-Output ("warning: timing budget breached but enforcement disabled (set OBJC3C_NATIVE_PERF_ENFORCE_TIMING_GATE=1 to fail-closed)")
  }
  Write-Output ("status: {0}" -f $statusValue)
}

Export-ModuleMember -Function @(
  "Write-Objc3cNativePerfSummary"
)
