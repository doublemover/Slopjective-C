Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking

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

function New-Objc3cNativePerfProofState {
  $cacheProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    fixture = ""
    fixture_kind = ""
    emit_prefix = ""
    run1 = $null
    run2 = $null
    artifacts = [ordered]@{}
  }
  $cacheInvalidationProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    fixture = ""
    run1 = $null
    run2 = $null
  }
  $macroHostProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    fixture = ""
    run1 = $null
    run2 = $null
    cache_artifact = ""
  }
  $docsGenerationProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    native_docs = $null
    public_command_surface = $null
  }

  return [pscustomobject]@{
    cache_proof = $cacheProof
    cache_invalidation_proof = $cacheInvalidationProof
    macro_host_proof = $macroHostProof
    docs_generation_proof = $docsGenerationProof
  }
}

function Ensure-Objc3cNativePerfCompilerAvailable {
  param(
    [object]$Config,
    [ref]$BuildExecuted,
    [ref]$BuildElapsedMs
  )

  $exe = Join-Path $Config.repo_root "artifacts/bin/objc3c-native.exe"
  $compileScript = Join-Path $Config.repo_root "scripts/objc3c_native_compile.ps1"
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    $BuildExecuted.Value = $true
    $buildScript = Join-Path $Config.repo_root "scripts/build_objc3c_native.ps1"
    $buildLog = Join-Path $Config.run_dir "build.log"
    $buildRun = Invoke-TimedNativeCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript) `
      -LogPath $buildLog
    $BuildElapsedMs.Value = $buildRun.elapsed_ms
    if ($buildRun.exit_code -ne 0) {
      throw "perf-budget FAIL: native compiler build failed with exit code $($buildRun.exit_code)"
    }
  }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "perf-budget FAIL: native compiler executable missing at $exe"
  }
  if (!(Test-Path -LiteralPath $compileScript -PathType Leaf)) {
    throw "perf-budget FAIL: missing compile wrapper at $compileScript"
  }

  return [pscustomobject]@{
    exe = $exe
    compile_script = $compileScript
  }
}

function Invoke-Objc3cNativePerfDirectCompiles {
  param(
    [object]$Config,
    [string]$CompilerExe,
    [ref]$ResolvedMaxElapsedMs,
    [ref]$FixtureSets,
    [ref]$DispatchFixtureCount,
    [ref]$Results,
    [ref]$Fixtures,
    [ref]$DispatchFixturePathSet
  )

  $fixtureDirectories = Get-PerfFixtureDirectories `
    -RepoRoot $Config.repo_root `
    -BaselineDirectory $Config.positive_dir `
    -RequiredDispatchDirectory $Config.dispatch_required_dir `
    -DispatchCandidateDirectories $Config.dispatch_positive_candidate_dirs `
    -ExtraDirectoriesRaw $Config.resolved_extra_positive_fixture_dirs

  $fixtures = @()
  $dispatchFixturePathSet = New-Object "System.Collections.Generic.HashSet[string]" ([System.StringComparer]::OrdinalIgnoreCase)
  foreach ($fixtureDirectory in $fixtureDirectories) {
    $dirFixtures = Get-Fixtures -Directory $fixtureDirectory.directory -FixtureKind $fixtureDirectory.source -Extensions $fixtureDirectory.extensions
    $FixtureSets.Value += [pscustomobject]@{
      fixture_root = Get-RepoRelativePath -Path $fixtureDirectory.directory -Root $Config.repo_root
      fixture_kind = $fixtureDirectory.fixture_kind
      fixture_count = $dirFixtures.Count
    }

    foreach ($fixture in $dirFixtures) {
      $fixtures += $fixture
      if ($fixtureDirectory.fixture_kind -eq "dispatch-positive") {
        $null = $dispatchFixturePathSet.Add($fixture.FullName)
      }
    }
  }

  $fixtures = @($fixtures | Sort-Object -Property FullName -Unique)
  if ($fixtures.Count -eq 0) {
    throw "perf-budget FAIL: no positive fixtures resolved from configured roots"
  }
  if (-not $Config.explicit_max_elapsed_ms) {
    $scaledBudget = [int][Math]::Ceiling($fixtures.Count * $Config.resolved_per_fixture_budget_ms)
    if ($scaledBudget -gt $ResolvedMaxElapsedMs.Value) {
      $ResolvedMaxElapsedMs.Value = $scaledBudget
    }
  }
  $DispatchFixtureCount.Value = @($fixtures | Where-Object { $dispatchFixturePathSet.Contains($_.FullName) }).Count
  if ($DispatchFixtureCount.Value -le 0) {
    throw "perf-budget FAIL: dispatch fixture suite resolved zero fixtures"
  }

  foreach ($fixtureSet in $FixtureSets.Value) {
    Write-Output ("fixture-set: kind={0} root={1} count={2}" -f $fixtureSet.fixture_kind, $fixtureSet.fixture_root, $fixtureSet.fixture_count)
  }

  foreach ($fixture in $fixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $Config.repo_root
    $hash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $Config.run_dir ("fixture_{0}" -f $hash)
    $compileLog = Join-Path $caseDir "compile.log"
    New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

    $compileArgs = @($fixture.FullName, "--out-dir", $caseDir, "--emit-prefix", "module")
    if ($fixture.Extension -eq ".objc3") {
      # Perf-budget is a throughput regression gate, not a backend-availability gate.
      # Force .objc3 perf samples onto clang object backend to avoid llc availability skew.
      $compileArgs += @("--objc3-ir-object-backend", "clang")
    }
    $run = Invoke-TimedNativeCommand `
      -Command $CompilerExe `
      -Arguments $compileArgs `
      -LogPath $compileLog

    $objPath = Join-Path $caseDir "module.obj"
    $objExists = Test-Path -LiteralPath $objPath -PathType Leaf
    $objSize = if ($objExists) { (Get-Item -LiteralPath $objPath).Length } else { 0 }
    $passed = ($run.exit_code -eq 0) -and $objExists -and ($objSize -gt 0)
    $artifactSurface = if ($passed) {
      Get-CompileArtifactSurface -OutputDirectory $caseDir -RepoRoot $Config.repo_root
    } else {
      [ordered]@{
        manifest_present = $false
      }
    }
    $detail = if ($passed) {
      "exit=0 obj_bytes=$objSize"
    } elseif ($run.exit_code -ne 0) {
      "expected exit=0 got exit=$($run.exit_code)"
    } elseif (!$objExists) {
      "missing module.obj"
    } else {
      "empty module.obj"
    }

    $Results.Value += [pscustomobject]@{
      fixture = $fixtureRel
      elapsed_ms = $run.elapsed_ms
      exit_code = $run.exit_code
      passed = $passed
      detail = $detail
      out_dir = (Get-RepoRelativePath -Path $caseDir -Root $Config.repo_root)
      compile_artifact_surface = $artifactSurface
    }

    $statusToken = if ($passed) { "PASS" } else { "FAIL" }
    Write-Output ("[{0}] {1} elapsed_ms={2} ({3})" -f $statusToken, $fixtureRel, $run.elapsed_ms, $detail)
  }

  $Fixtures.Value = $fixtures
  $DispatchFixturePathSet.Value = $dispatchFixturePathSet
}

function Invoke-Objc3cNativePerfCacheProof {
  param(
    [object]$Config,
    [object[]]$Fixtures,
    [object]$DispatchFixturePathSet,
    [string]$CompileScript,
    [ref]$CacheProof,
    [ref]$CacheFixture,
    [ref]$CacheFixtureRel
  )

  $cacheFixture = @($Fixtures | Where-Object { $_.Extension -eq ".objc3" } | Select-Object -First 1)[0]
  if ($null -eq $cacheFixture) {
    throw "perf-budget FAIL: cache-proof requires at least one .objc3 fixture so the live compile-wrapper contract can publish the runtime registration manifest"
  }
  $cacheFixtureRel = Get-RepoRelativePath -Path $cacheFixture.FullName -Root $Config.repo_root
  $cacheFixtureKind = if ($DispatchFixturePathSet.Contains($cacheFixture.FullName)) { "dispatch-positive" } else { "recovery-positive" }
  $emitPrefix = "module"
  $cacheDir = Join-Path $Config.run_dir "cache-proof"
  $missDir = Join-Path $cacheDir "miss"
  $hitDir = Join-Path $cacheDir "hit"
  New-Item -ItemType Directory -Force -Path $cacheDir | Out-Null
  $cacheFixtureSource = Join-Path $cacheDir $cacheFixture.Name
  Copy-Item -LiteralPath $cacheFixture.FullName -Destination $cacheFixtureSource -Force

  $run1Log = Join-Path $cacheDir "run1.log"
  $run2Log = Join-Path $cacheDir "run2.log"
  $cacheScriptArgs = @($cacheFixtureSource, "--use-cache", "--out-dir", $missDir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $cacheScriptArgs += @("--objc3-ir-object-backend", "clang")
  }
  $run1 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $cacheScriptArgs `
    -LogPath $run1Log
  if ($run1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-proof run1 failed with exit code $($run1.exit_code)"
  }
  $run1Hit = Parse-CacheHitFlag -OutputText $run1.output_text -RunLabel "cache-proof run1"
  if ($run1Hit) {
    throw "perf-budget FAIL: cache-proof run1 expected cache_hit=false, observed true"
  }

  $cacheScriptArgsHit = @($cacheFixtureSource, "--use-cache", "--out-dir", $hitDir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $cacheScriptArgsHit += @("--objc3-ir-object-backend", "clang")
  }
  $run2 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $cacheScriptArgsHit `
    -LogPath $run2Log
  if ($run2.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-proof run2 failed with exit code $($run2.exit_code)"
  }
  $run2Hit = Parse-CacheHitFlag -OutputText $run2.output_text -RunLabel "cache-proof run2"
  if (!$run2Hit) {
    throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"
  }

  $artifactNames = @(
    "$emitPrefix.obj",
    "$emitPrefix.manifest.json",
    "$emitPrefix.diagnostics.txt"
  )
  if ($cacheFixture.Extension -eq ".objc3") {
    $artifactNames += "$emitPrefix.ll"
  }

  $missHashes = Get-ArtifactHashSet -Directory $missDir -ArtifactNames $artifactNames
  $hitHashes = Get-ArtifactHashSet -Directory $hitDir -ArtifactNames $artifactNames
  foreach ($name in $artifactNames) {
    if ($missHashes[$name] -ne $hitHashes[$name]) {
      throw "perf-budget FAIL: cache-proof artifact hash drift for $name"
    }
  }

  $objPath = Join-Path $missDir "$emitPrefix.obj"
  $objSize = (Get-Item -LiteralPath $objPath).Length
  if ($objSize -le 0) {
    throw "perf-budget FAIL: cache-proof produced empty object artifact"
  }

  $proof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "run1_hit=false run2_hit=true artifact_hashes_match=true"
    fixture = $cacheFixtureRel
    fixture_kind = $cacheFixtureKind
    emit_prefix = $emitPrefix
    run1 = [ordered]@{
      elapsed_ms = $run1.elapsed_ms
      exit_code = $run1.exit_code
      cache_hit = $run1Hit
      log = (Get-RepoRelativePath -Path $run1Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $missDir -Root $Config.repo_root)
    }
    run2 = [ordered]@{
      elapsed_ms = $run2.elapsed_ms
      exit_code = $run2.exit_code
      cache_hit = $run2Hit
      log = (Get-RepoRelativePath -Path $run2Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $hitDir -Root $Config.repo_root)
    }
    artifacts = [ordered]@{
      miss_sha256 = $missHashes
      hit_sha256 = $hitHashes
    }
  }
  Write-Output ("cache-proof PASS fixture={0} fixture_kind={1} run1_hit={2} run2_hit={3}" -f $cacheFixtureRel, $cacheFixtureKind, $run1Hit, $run2Hit)

  $CacheProof.Value = $proof
  $CacheFixture.Value = $cacheFixture
  $CacheFixtureRel.Value = $cacheFixtureRel
}

function Invoke-Objc3cNativePerfCacheInvalidationProof {
  param(
    [object]$Config,
    [object]$CacheFixture,
    [string]$CacheFixtureRel,
    [string]$CompileScript,
    [ref]$CacheInvalidationProof
  )

  $invalidationDir = Join-Path $Config.run_dir "cache-invalidation"
  $invalidationSource = Join-Path $invalidationDir $CacheFixture.Name
  New-Item -ItemType Directory -Force -Path $invalidationDir | Out-Null
  Copy-Item -LiteralPath $CacheFixture.FullName -Destination $invalidationSource -Force

  $invalidationRun1Dir = Join-Path $invalidationDir "run1"
  $invalidationRun2Dir = Join-Path $invalidationDir "run2"
  $invalidationRun1Log = Join-Path $invalidationDir "run1.log"
  $invalidationRun2Log = Join-Path $invalidationDir "run2.log"

  $invalidationArgs = @($invalidationSource, "--use-cache", "--out-dir", $invalidationRun1Dir)
  if ($CacheFixture.Extension -eq ".objc3") {
    $invalidationArgs += @("--objc3-ir-object-backend", "clang")
  }
  $invalidationRun1 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $invalidationArgs `
    -LogPath $invalidationRun1Log
  if ($invalidationRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run1 failed with exit code $($invalidationRun1.exit_code)"
  }
  $invalidationRun1Hit = Parse-CacheHitFlag -OutputText $invalidationRun1.output_text -RunLabel "cache-invalidation run1"

  Add-Content -LiteralPath $invalidationSource -Value "// cache invalidation probe mutation"

  $invalidationArgs2 = @($invalidationSource, "--use-cache", "--out-dir", $invalidationRun2Dir)
  if ($CacheFixture.Extension -eq ".objc3") {
    $invalidationArgs2 += @("--objc3-ir-object-backend", "clang")
  }
  $invalidationRun2 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $invalidationArgs2 `
    -LogPath $invalidationRun2Log
  if ($invalidationRun2.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run2 failed with exit code $($invalidationRun2.exit_code)"
  }
  $invalidationRun2Hit = Parse-CacheHitFlag -OutputText $invalidationRun2.output_text -RunLabel "cache-invalidation run2"
  if ($invalidationRun2Hit) {
    throw "perf-budget FAIL: cache-invalidation run2 expected cache_hit=false after source mutation"
  }

  $cacheInvalidationProof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "source mutation invalidated wrapper cache key"
    fixture = (Get-RepoRelativePath -Path $CacheFixture.FullName -Root $Config.repo_root)
    run1 = [ordered]@{
      elapsed_ms = $invalidationRun1.elapsed_ms
      exit_code = $invalidationRun1.exit_code
      cache_hit = $invalidationRun1Hit
      log = (Get-RepoRelativePath -Path $invalidationRun1Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $invalidationRun1Dir -Root $Config.repo_root)
    }
    run2 = [ordered]@{
      elapsed_ms = $invalidationRun2.elapsed_ms
      exit_code = $invalidationRun2.exit_code
      cache_hit = $invalidationRun2Hit
      log = (Get-RepoRelativePath -Path $invalidationRun2Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $invalidationRun2Dir -Root $Config.repo_root)
    }
  }
  Write-Output ("cache-invalidation PASS fixture={0} run1_hit={1} run2_hit={2}" -f $CacheFixtureRel, $invalidationRun1Hit, $invalidationRun2Hit)

  $CacheInvalidationProof.Value = $cacheInvalidationProof
}

function Invoke-Objc3cNativePerfMacroHostProof {
  param(
    [object]$Config,
    [string]$CompileScript,
    [ref]$MacroHostProof
  )

  if (!(Test-Path -LiteralPath $Config.macro_host_fixture -PathType Leaf)) {
    throw "perf-budget FAIL: missing macro-host fixture at $($Config.macro_host_fixture)"
  }
  $macroHostDir = Join-Path $Config.run_dir "macro-host-cache"
  $macroHostRun1Dir = Join-Path $macroHostDir "run1"
  $macroHostRun2Dir = Join-Path $macroHostDir "run2"
  $macroHostRun1Log = Join-Path $macroHostDir "run1.log"
  $macroHostRun2Log = Join-Path $macroHostDir "run2.log"
  New-Item -ItemType Directory -Force -Path $macroHostDir | Out-Null
  $macroHostFixtureSource = Join-Path $macroHostDir (Split-Path -Leaf $Config.macro_host_fixture)
  Copy-Item -LiteralPath $Config.macro_host_fixture -Destination $macroHostFixtureSource -Force
  $macroEmitPrefix = "module"

  $macroHostArgs = @($macroHostFixtureSource, "--use-cache", "--out-dir", $macroHostRun1Dir, "--objc3-ir-object-backend", "clang")
  $macroHostRun1 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $macroHostArgs `
    -LogPath $macroHostRun1Log
  if ($macroHostRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: macro-host cache run1 failed with exit code $($macroHostRun1.exit_code)"
  }
  $macroHostRun1Hit = Parse-CacheHitFlag -OutputText $macroHostRun1.output_text -RunLabel "macro-host cache run1"

  $macroHostArgs2 = @($macroHostFixtureSource, "--use-cache", "--out-dir", $macroHostRun2Dir, "--objc3-ir-object-backend", "clang")
  $macroHostRun2 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $macroHostArgs2 `
    -LogPath $macroHostRun2Log
  if ($macroHostRun2.exit_code -ne 0) {
    throw "perf-budget FAIL: macro-host cache run2 failed with exit code $($macroHostRun2.exit_code)"
  }
  $macroHostRun2Hit = Parse-CacheHitFlag -OutputText $macroHostRun2.output_text -RunLabel "macro-host cache run2"
  if (!$macroHostRun2Hit) {
    throw "perf-budget FAIL: macro-host cache run2 expected cache_hit=true"
  }

  $macroHostArtifact = Join-Path $macroHostRun2Dir "$macroEmitPrefix.metaprogramming-macro-host-cache.json"
  if (!(Test-Path -LiteralPath $macroHostArtifact -PathType Leaf)) {
    $macroHostArtifact = Join-Path $macroHostRun2Dir "module.metaprogramming-macro-host-cache.json"
  }
  if (!(Test-Path -LiteralPath $macroHostArtifact -PathType Leaf)) {
    throw "perf-budget FAIL: macro-host cache compile did not publish a metaprogramming host-cache artifact"
  }

  $macroHostProof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "macro-host cache artifact published through the live wrapper path"
    fixture = (Get-RepoRelativePath -Path $Config.macro_host_fixture -Root $Config.repo_root)
    run1 = [ordered]@{
      elapsed_ms = $macroHostRun1.elapsed_ms
      exit_code = $macroHostRun1.exit_code
      cache_hit = $macroHostRun1Hit
      log = (Get-RepoRelativePath -Path $macroHostRun1Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $macroHostRun1Dir -Root $Config.repo_root)
    }
    run2 = [ordered]@{
      elapsed_ms = $macroHostRun2.elapsed_ms
      exit_code = $macroHostRun2.exit_code
      cache_hit = $macroHostRun2Hit
      log = (Get-RepoRelativePath -Path $macroHostRun2Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $macroHostRun2Dir -Root $Config.repo_root)
    }
    cache_artifact = (Get-RepoRelativePath -Path $macroHostArtifact -Root $Config.repo_root)
  }
  Write-Output ("macro-host-cache PASS fixture={0} run1_hit={1} run2_hit={2}" -f $macroHostProof.fixture, $macroHostRun1Hit, $macroHostRun2Hit)

  $MacroHostProof.Value = $macroHostProof
}

function Invoke-Objc3cNativePerfDocsGenerationProof {
  param(
    [object]$Config,
    [ref]$DocsGenerationProof
  )

  if (!(Test-Path -LiteralPath $Config.native_docs_script -PathType Leaf)) {
    throw "perf-budget FAIL: missing native docs generator at $($Config.native_docs_script)"
  }
  if (!(Test-Path -LiteralPath $Config.command_surface_script -PathType Leaf)) {
    throw "perf-budget FAIL: missing public command surface generator at $($Config.command_surface_script)"
  }
  $docsDir = Join-Path $Config.run_dir "docs-generation"
  New-Item -ItemType Directory -Force -Path $docsDir | Out-Null
  $nativeDocsLog = Join-Path $docsDir "native-docs.log"
  $commandSurfaceLog = Join-Path $docsDir "public-command-surface.log"
  $nativeDocsRun = Invoke-TimedNativeCommand `
    -Command $Config.python_command `
    -Arguments @($Config.native_docs_script) `
    -LogPath $nativeDocsLog
  if ($nativeDocsRun.exit_code -ne 0) {
    throw "perf-budget FAIL: native docs generation failed with exit code $($nativeDocsRun.exit_code)"
  }
  $commandSurfaceRun = Invoke-TimedNativeCommand `
    -Command $Config.python_command `
    -Arguments @($Config.command_surface_script) `
    -LogPath $commandSurfaceLog
  if ($commandSurfaceRun.exit_code -ne 0) {
    throw "perf-budget FAIL: public command surface generation failed with exit code $($commandSurfaceRun.exit_code)"
  }

  $docsGenerationProof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "checked-in docs generators executed on the live repo surface"
    native_docs = [ordered]@{
      elapsed_ms = $nativeDocsRun.elapsed_ms
      exit_code = $nativeDocsRun.exit_code
      log = (Get-RepoRelativePath -Path $nativeDocsLog -Root $Config.repo_root)
    }
    public_command_surface = [ordered]@{
      elapsed_ms = $commandSurfaceRun.elapsed_ms
      exit_code = $commandSurfaceRun.exit_code
      log = (Get-RepoRelativePath -Path $commandSurfaceLog -Root $Config.repo_root)
    }
  }
  Write-Output ("docs-generation PASS native_docs_ms={0} command_surface_ms={1}" -f $nativeDocsRun.elapsed_ms, $commandSurfaceRun.elapsed_ms)

  $DocsGenerationProof.Value = $docsGenerationProof
}

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

  $total = $Results.Count
  $passedCount = @($Results | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $elapsedRows = @($Results | ForEach-Object { [double]$_.elapsed_ms })
  $totalElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Sum).Sum), 3) } else { 0.0 }
  $minFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Minimum).Minimum), 3) } else { 0.0 }
  $maxFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Maximum).Maximum), 3) } else { 0.0 }
  $avgFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round(($totalElapsedMs / $elapsedRows.Count), 3) } else { 0.0 }
  $budgetBreached = $totalElapsedMs -gt $ResolvedMaxElapsedMs
  $timingGateViolated = $Config.timing_gate_enforced -and $budgetBreached
  $budgetMarginMs = [Math]::Round(($ResolvedMaxElapsedMs - $totalElapsedMs), 3)
  $cacheProofPassed = $CacheProof.executed -and ($CacheProof.status -eq "PASS")
  $statusValue = if (!$HadFatalError -and $total -gt 0 -and $failedCount -eq 0 -and !$timingGateViolated -and $cacheProofPassed) { "PASS" } else { "FAIL" }
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
      cache_proof_elapsed_ms = if ($CacheProof.executed -and $null -ne $CacheProof.run1 -and $null -ne $CacheProof.run2) {
        [Math]::Round(([double]$CacheProof.run1.elapsed_ms + [double]$CacheProof.run2.elapsed_ms), 3)
      } else {
        0.0
      }
      cache_invalidation_elapsed_ms = if ($CacheInvalidationProof.executed -and $null -ne $CacheInvalidationProof.run1 -and $null -ne $CacheInvalidationProof.run2) {
        [Math]::Round(([double]$CacheInvalidationProof.run1.elapsed_ms + [double]$CacheInvalidationProof.run2.elapsed_ms), 3)
      } else {
        0.0
      }
      macro_host_cache_elapsed_ms = if ($MacroHostProof.executed -and $null -ne $MacroHostProof.run1 -and $null -ne $MacroHostProof.run2) {
        [Math]::Round(([double]$MacroHostProof.run1.elapsed_ms + [double]$MacroHostProof.run2.elapsed_ms), 3)
      } else {
        0.0
      }
      docs_generation_elapsed_ms = if ($DocsGenerationProof.executed -and $null -ne $DocsGenerationProof.native_docs -and $null -ne $DocsGenerationProof.public_command_surface) {
        [Math]::Round(([double]$DocsGenerationProof.native_docs.elapsed_ms + [double]$DocsGenerationProof.public_command_surface.elapsed_ms), 3)
      } else {
        0.0
      }
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

function Invoke-Objc3cNativePerfBudget {
  param(
    [string]$ScriptRoot,
    [Nullable[int]]$MaxElapsedMs,
    [string]$ExtraPositiveFixtureDirs,
    [switch]$EnforceTimingGate,
    [ref]$ExitCode
  )

  $Config = Resolve-Objc3cNativePerfBudgetConfig `
    -ScriptRoot $ScriptRoot `
    -MaxElapsedMs $MaxElapsedMs `
    -ExtraPositiveFixtureDirs $ExtraPositiveFixtureDirs `
    -EnforceTimingGate:$EnforceTimingGate.IsPresent

  New-Item -ItemType Directory -Force -Path $Config.run_dir | Out-Null

  $hadFatalError = $false
  $fatalErrorMessage = ""
  $buildExecuted = $false
  $buildElapsedMs = 0.0
  $fixtureSets = @()
  $dispatchFixtureCount = 0
  $results = @()
  $proofState = New-Objc3cNativePerfProofState
  $cacheProof = $proofState.cache_proof
  $cacheInvalidationProof = $proofState.cache_invalidation_proof
  $macroHostProof = $proofState.macro_host_proof
  $docsGenerationProof = $proofState.docs_generation_proof
  $resolvedMaxElapsedMs = $Config.resolved_max_elapsed_ms
  $directFixtures = @()
  $directDispatchFixturePathSet = $null
  $cacheFixture = $null
  $cacheFixtureRel = ""

  Push-Location $Config.repo_root
  try {
    $compilerPaths = Ensure-Objc3cNativePerfCompilerAvailable `
      -Config $Config `
      -BuildExecuted ([ref]$buildExecuted) `
      -BuildElapsedMs ([ref]$buildElapsedMs)

    Invoke-Objc3cNativePerfDirectCompiles `
      -Config $Config `
      -CompilerExe $compilerPaths.exe `
      -ResolvedMaxElapsedMs ([ref]$resolvedMaxElapsedMs) `
      -FixtureSets ([ref]$fixtureSets) `
      -DispatchFixtureCount ([ref]$dispatchFixtureCount) `
      -Results ([ref]$results) `
      -Fixtures ([ref]$directFixtures) `
      -DispatchFixturePathSet ([ref]$directDispatchFixturePathSet)

    Invoke-Objc3cNativePerfCacheProof `
      -Config $Config `
      -Fixtures $directFixtures `
      -DispatchFixturePathSet $directDispatchFixturePathSet `
      -CompileScript $compilerPaths.compile_script `
      -CacheProof ([ref]$cacheProof) `
      -CacheFixture ([ref]$cacheFixture) `
      -CacheFixtureRel ([ref]$cacheFixtureRel)

    Invoke-Objc3cNativePerfCacheInvalidationProof `
      -Config $Config `
      -CacheFixture $cacheFixture `
      -CacheFixtureRel $cacheFixtureRel `
      -CompileScript $compilerPaths.compile_script `
      -CacheInvalidationProof ([ref]$cacheInvalidationProof)

    Invoke-Objc3cNativePerfMacroHostProof `
      -Config $Config `
      -CompileScript $compilerPaths.compile_script `
      -MacroHostProof ([ref]$macroHostProof)

    Invoke-Objc3cNativePerfDocsGenerationProof -Config $Config -DocsGenerationProof ([ref]$docsGenerationProof)
  } catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalErrorMessage)
  } finally {
    Pop-Location
  }

  $status = "FAIL"
  Write-Objc3cNativePerfSummary `
    -Config $Config `
    -FixtureSets $fixtureSets `
    -DispatchFixtureCount $dispatchFixtureCount `
    -Results $results `
    -ResolvedMaxElapsedMs $resolvedMaxElapsedMs `
    -BuildExecuted $buildExecuted `
    -BuildElapsedMs $buildElapsedMs `
    -CacheProof $cacheProof `
    -CacheInvalidationProof $cacheInvalidationProof `
    -MacroHostProof $macroHostProof `
    -DocsGenerationProof $docsGenerationProof `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage `
    -Status ([ref]$status)

  if ($status -ne "PASS") {
    $ExitCode.Value = 1
  } else {
    $ExitCode.Value = 0
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativePerfBudget"
)
