Set-StrictMode -Version Latest

function Resolve-Objc3cNativePerfBudgetConfig {
  param(
    [string]$ScriptRoot,
    [Nullable[int]]$MaxElapsedMs,
    [string]$ExtraPositiveFixtureDirs,
    [switch]$EnforceTimingGate
  )

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $recoveryRoot = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery"
  $positiveDir = Join-Path $recoveryRoot "positive"
  $dispatchRequiredDir = "tests/tooling/fixtures/native/recovery/positive/lowering_dispatch"
  $dispatchPositiveCandidateDirs = @(
    "tests/tooling/fixtures/native/message_dispatch/positive",
    "tests/tooling/fixtures/native/dispatch/positive",
    "tests/tooling/fixtures/native/recovery/message_dispatch/positive",
    "tests/tooling/fixtures/native/recovery/dispatch/positive"
  )
  $runId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
  $perfRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/perf-budget"
  $runDir = Join-Path $perfRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $reportRoot = Join-Path $repoRoot "tmp/reports/compiler-throughput"
  $reportPath = Join-Path $reportRoot "benchmark-summary.json"
  $defaultMaxElapsedMs = 4000
  $defaultPerFixtureBudgetMs = 150
  $defaultWindowsLaunchOverheadPerFixtureMs = 450
  $defaultNonWindowsLaunchOverheadPerFixtureMs = 0
  $macroHostFixture = Join-Path $repoRoot "tests/tooling/fixtures/native/macro_host_process_provider.objc3"
  $nativeDocsScript = Join-Path $repoRoot "scripts/build_objc3c_native_docs.py"
  $commandSurfaceScript = Join-Path $repoRoot "scripts/render_objc3c_public_command_surface.py"
  $pythonCommand = if (Get-Command python -ErrorAction SilentlyContinue) { "python" } else { "python" }

  $resolvedMaxElapsedMs = $defaultMaxElapsedMs
  $explicitMaxElapsedMs = $false
  $timingGateEnforced = $false
  if ($PSVersionTable.PSVersion.Major -ge 6) {
    $isWindowsHost = [bool]$IsWindows
  } else {
    $isWindowsHost = ($env:OS -eq "Windows_NT")
  }
  $resolvedLaunchOverheadPerFixtureMs = if ($isWindowsHost) { $defaultWindowsLaunchOverheadPerFixtureMs } else { $defaultNonWindowsLaunchOverheadPerFixtureMs }
  $resolvedPerFixtureBudgetMs = $defaultPerFixtureBudgetMs + $resolvedLaunchOverheadPerFixtureMs
  $resolvedBudgetProfile = if ($isWindowsHost) { "windows-process-launch-calibrated" } else { "baseline" }
  if ($null -ne $MaxElapsedMs) {
    $resolvedMaxElapsedMs = [int]$MaxElapsedMs
    $explicitMaxElapsedMs = $true
  } elseif (-not [string]::IsNullOrWhiteSpace($env:OBJC3C_NATIVE_PERF_MAX_MS)) {
    $parsedBudget = 0
    if (-not [int]::TryParse($env:OBJC3C_NATIVE_PERF_MAX_MS, [ref]$parsedBudget)) {
      throw "perf-budget FAIL: OBJC3C_NATIVE_PERF_MAX_MS must be an integer"
    }
    $resolvedMaxElapsedMs = $parsedBudget
    $explicitMaxElapsedMs = $true
  }

  if (-not [string]::IsNullOrWhiteSpace($env:OBJC3C_NATIVE_PERF_PER_FIXTURE_MS)) {
    $parsedPerFixtureBudget = 0
    if (-not [int]::TryParse($env:OBJC3C_NATIVE_PERF_PER_FIXTURE_MS, [ref]$parsedPerFixtureBudget)) {
      throw "perf-budget FAIL: OBJC3C_NATIVE_PERF_PER_FIXTURE_MS must be an integer"
    }
    if ($parsedPerFixtureBudget -le 0) {
      throw "perf-budget FAIL: OBJC3C_NATIVE_PERF_PER_FIXTURE_MS must be > 0, got $parsedPerFixtureBudget"
    }
    $resolvedPerFixtureBudgetMs = $parsedPerFixtureBudget
    $resolvedLaunchOverheadPerFixtureMs = 0
    $resolvedBudgetProfile = "per-fixture-override"
  } elseif (-not [string]::IsNullOrWhiteSpace($env:OBJC3C_NATIVE_PERF_LAUNCH_OVERHEAD_PER_FIXTURE_MS)) {
    $parsedLaunchOverhead = 0
    if (-not [int]::TryParse($env:OBJC3C_NATIVE_PERF_LAUNCH_OVERHEAD_PER_FIXTURE_MS, [ref]$parsedLaunchOverhead)) {
      throw "perf-budget FAIL: OBJC3C_NATIVE_PERF_LAUNCH_OVERHEAD_PER_FIXTURE_MS must be an integer"
    }
    if ($parsedLaunchOverhead -lt 0) {
      throw "perf-budget FAIL: OBJC3C_NATIVE_PERF_LAUNCH_OVERHEAD_PER_FIXTURE_MS must be >= 0, got $parsedLaunchOverhead"
    }
    $resolvedLaunchOverheadPerFixtureMs = $parsedLaunchOverhead
    $resolvedPerFixtureBudgetMs = $defaultPerFixtureBudgetMs + $resolvedLaunchOverheadPerFixtureMs
    $resolvedBudgetProfile = "launch-overhead-override"
  }

  if ($EnforceTimingGate.IsPresent) {
    $timingGateEnforced = $true
  } elseif (-not [string]::IsNullOrWhiteSpace($env:OBJC3C_NATIVE_PERF_ENFORCE_TIMING_GATE)) {
    $rawTimingGateValue = $env:OBJC3C_NATIVE_PERF_ENFORCE_TIMING_GATE.Trim().ToLowerInvariant()
    if ($rawTimingGateValue -in @("1", "true", "yes", "on")) {
      $timingGateEnforced = $true
    } elseif ($rawTimingGateValue -in @("0", "false", "no", "off")) {
      $timingGateEnforced = $false
    } else {
      throw "perf-budget FAIL: OBJC3C_NATIVE_PERF_ENFORCE_TIMING_GATE must be one of [1,true,yes,on,0,false,no,off]"
    }
  }

  if ($resolvedMaxElapsedMs -le 0) {
    throw "perf-budget FAIL: max elapsed budget must be > 0 (ms), got $resolvedMaxElapsedMs"
  }

  $resolvedExtraPositiveFixtureDirs = $ExtraPositiveFixtureDirs
  if ([string]::IsNullOrWhiteSpace($resolvedExtraPositiveFixtureDirs) -and -not [string]::IsNullOrWhiteSpace($env:OBJC3C_NATIVE_PERF_EXTRA_POSITIVE_FIXTURE_DIRS)) {
    $resolvedExtraPositiveFixtureDirs = $env:OBJC3C_NATIVE_PERF_EXTRA_POSITIVE_FIXTURE_DIRS
  }
  $extraPositiveFixtureDirList = @()
  if (-not [string]::IsNullOrWhiteSpace($resolvedExtraPositiveFixtureDirs)) {
    $extraPositiveFixtureDirList = @(
      $resolvedExtraPositiveFixtureDirs.Split(";") |
        ForEach-Object { $_.Trim() } |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_) }
    )
  }

  return [pscustomobject]@{
    repo_root = $repoRoot
    positive_dir = $positiveDir
    dispatch_required_dir = $dispatchRequiredDir
    dispatch_positive_candidate_dirs = $dispatchPositiveCandidateDirs
    run_id = $runId
    run_dir = $runDir
    summary_path = $summaryPath
    report_root = $reportRoot
    report_path = $reportPath
    default_max_elapsed_ms = $defaultMaxElapsedMs
    default_per_fixture_budget_ms = $defaultPerFixtureBudgetMs
    default_windows_launch_overhead_per_fixture_ms = $defaultWindowsLaunchOverheadPerFixtureMs
    default_non_windows_launch_overhead_per_fixture_ms = $defaultNonWindowsLaunchOverheadPerFixtureMs
    macro_host_fixture = $macroHostFixture
    native_docs_script = $nativeDocsScript
    command_surface_script = $commandSurfaceScript
    python_command = $pythonCommand
    resolved_max_elapsed_ms = $resolvedMaxElapsedMs
    explicit_max_elapsed_ms = $explicitMaxElapsedMs
    timing_gate_enforced = $timingGateEnforced
    resolved_launch_overhead_per_fixture_ms = $resolvedLaunchOverheadPerFixtureMs
    resolved_per_fixture_budget_ms = $resolvedPerFixtureBudgetMs
    resolved_budget_profile = $resolvedBudgetProfile
    resolved_extra_positive_fixture_dirs = $resolvedExtraPositiveFixtureDirs
    extra_positive_fixture_dir_list = $extraPositiveFixtureDirList
  }
}

Export-ModuleMember -Function @(
  "Resolve-Objc3cNativePerfBudgetConfig"
)
