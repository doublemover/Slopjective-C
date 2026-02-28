param(
  [Nullable[int]]$MaxElapsedMs,
  [string]$ExtraPositiveFixtureDirs
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
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
$defaultMaxElapsedMs = 4000
$defaultPerFixtureBudgetMs = 150

function Get-RepoRelativePath {
  param(
    [string]$Path,
    [string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-ShortHash {
  param([string]$Value)

  $sha1 = [System.Security.Cryptography.SHA1]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Value)
    $hashBytes = $sha1.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant().Substring(0, 10)
  } finally {
    $sha1.Dispose()
  }
}

function Get-Fixtures {
  param(
    [string]$Directory,
    [string]$FixtureKind,
    [string[]]$Extensions = @(".objc3", ".m")
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "perf-budget FAIL: missing $FixtureKind fixture directory at $Directory"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $Directory -Recurse -File |
      Where-Object { $_.Extension -in $Extensions } |
      Sort-Object -Property FullName
  )

  if ($fixtures.Count -eq 0) {
    throw "perf-budget FAIL: no $FixtureKind fixtures found in $Directory"
  }

  return $fixtures
}

function Get-PerfFixtureDirectories {
  param(
    [string]$RepoRoot,
    [string]$BaselineDirectory,
    [string]$RequiredDispatchDirectory,
    [string[]]$DispatchCandidateDirectories,
    [string]$ExtraDirectoriesRaw
  )

  $seen = New-Object "System.Collections.Generic.HashSet[string]" ([System.StringComparer]::OrdinalIgnoreCase)
  $directories = New-Object "System.Collections.Generic.List[object]"

  function Add-DirectoryEntry {
    param(
      [string]$RawPath,
      [string]$FixtureKind,
      [string]$SourceLabel,
      [bool]$Required,
      [string[]]$Extensions = @(".objc3", ".m")
    )

    if ([string]::IsNullOrWhiteSpace($RawPath)) {
      return
    }

    $candidate = $RawPath
    if (-not [System.IO.Path]::IsPathRooted($candidate)) {
      $candidate = Join-Path $RepoRoot $candidate
    }
    $fullPath = [System.IO.Path]::GetFullPath($candidate)
    if (!(Test-Path -LiteralPath $fullPath -PathType Container)) {
      if ($Required) {
        throw "perf-budget FAIL: missing $SourceLabel fixture directory at $fullPath"
      }
      return
    }

    if ($seen.Add($fullPath)) {
      $directories.Add([pscustomobject]@{
          directory = $fullPath
          fixture_kind = $FixtureKind
          source = $SourceLabel
          extensions = $Extensions
        }) | Out-Null
    }
  }

  Add-DirectoryEntry -RawPath $BaselineDirectory -FixtureKind "recovery-positive" -SourceLabel "baseline positive recovery" -Required $true -Extensions @(".objc3")
  Add-DirectoryEntry -RawPath $RequiredDispatchDirectory -FixtureKind "dispatch-positive" -SourceLabel "dispatch lowering suite" -Required $true -Extensions @(".m", ".objc3")
  foreach ($candidate in $DispatchCandidateDirectories) {
    Add-DirectoryEntry -RawPath $candidate -FixtureKind "dispatch-positive" -SourceLabel "dispatch positive" -Required $false -Extensions @(".m", ".objc3")
  }

  if (-not [string]::IsNullOrWhiteSpace($ExtraDirectoriesRaw)) {
    $extraDirectories = @(
      $ExtraDirectoriesRaw.Split(";") |
        ForEach-Object { $_.Trim() } |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_) }
    )
    foreach ($extraDirectory in $extraDirectories) {
      Add-DirectoryEntry -RawPath $extraDirectory -FixtureKind "dispatch-positive" -SourceLabel "extra positive" -Required $true -Extensions @(".m", ".objc3")
    }
  }

  if ($directories.Count -eq 0) {
    throw "perf-budget FAIL: no positive fixture directories resolved"
  }

  return $directories.ToArray()
}

function Invoke-TimedNativeCommand {
  param(
    [string]$Command,
    [string[]]$Arguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    $exitCode = $LASTEXITCODE
  } finally {
    $stopwatch.Stop()
    $ErrorActionPreference = $previousErrorAction
  }

  return [pscustomobject]@{
    exit_code = $exitCode
    elapsed_ms = [Math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
  }
}

function Invoke-TimedWrapperCommand {
  param(
    [string]$ScriptPath,
    [string[]]$ScriptArguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  $outputLines = @()
  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  try {
    $ErrorActionPreference = "Continue"
    $outputLines = & powershell -NoProfile -ExecutionPolicy Bypass -File $ScriptPath @ScriptArguments 2>&1
    $exitCode = $LASTEXITCODE
  } finally {
    $stopwatch.Stop()
    $ErrorActionPreference = $previousErrorAction
  }

  $outputText = ""
  if ($null -ne $outputLines) {
    $outputText = (($outputLines | ForEach-Object { $_.ToString() }) -join [Environment]::NewLine)
  }
  Set-Content -LiteralPath $LogPath -Value $outputText -Encoding utf8

  return [pscustomobject]@{
    exit_code = $exitCode
    elapsed_ms = [Math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
    output_text = $outputText
  }
}

function Parse-CacheHitFlag {
  param(
    [string]$OutputText,
    [string]$RunLabel
  )

  if ([string]::IsNullOrWhiteSpace($OutputText)) {
    throw "perf-budget FAIL: $RunLabel produced no output (missing cache_hit marker)"
  }

  $matches = [regex]::Matches($OutputText, "(?m)^cache_hit=(true|false)\s*$")
  if ($matches.Count -ne 1) {
    throw "perf-budget FAIL: $RunLabel expected exactly one cache_hit marker, observed $($matches.Count)"
  }
  return ($matches[0].Groups[1].Value.ToLowerInvariant() -eq "true")
}

function Get-ArtifactHashSet {
  param(
    [string]$Directory,
    [string[]]$ArtifactNames
  )

  $hashes = [ordered]@{}
  foreach ($name in $ArtifactNames) {
    $path = Join-Path $Directory $name
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
      throw "perf-budget FAIL: cache-proof missing artifact $path"
    }
    $sha256 = [System.Security.Cryptography.SHA256]::Create()
    $stream = [System.IO.File]::OpenRead($path)
    try {
      $hashBytes = $sha256.ComputeHash($stream)
      $hashes[$name] = ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
    } finally {
      $stream.Dispose()
      $sha256.Dispose()
    }
  }
  return $hashes
}

$resolvedMaxElapsedMs = $defaultMaxElapsedMs
$explicitMaxElapsedMs = $false
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

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$compileScript = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"
$hadFatalError = $false
$fatalErrorMessage = ""
$buildExecuted = $false
$buildElapsedMs = 0.0
$fixtureSets = @()
$dispatchFixturePathSet = New-Object "System.Collections.Generic.HashSet[string]" ([System.StringComparer]::OrdinalIgnoreCase)
$dispatchFixtureCount = 0
$results = @()
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

Push-Location $repoRoot
try {
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    $buildExecuted = $true
    $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
    $buildLog = Join-Path $runDir "build.log"
    $buildRun = Invoke-TimedNativeCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript) `
      -LogPath $buildLog
    $buildElapsedMs = $buildRun.elapsed_ms
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

  $fixtureDirectories = Get-PerfFixtureDirectories `
    -RepoRoot $repoRoot `
    -BaselineDirectory $positiveDir `
    -RequiredDispatchDirectory $dispatchRequiredDir `
    -DispatchCandidateDirectories $dispatchPositiveCandidateDirs `
    -ExtraDirectoriesRaw $resolvedExtraPositiveFixtureDirs

  $fixtures = @()
  foreach ($fixtureDirectory in $fixtureDirectories) {
    $dirFixtures = Get-Fixtures -Directory $fixtureDirectory.directory -FixtureKind $fixtureDirectory.source -Extensions $fixtureDirectory.extensions
    $fixtureSets += [pscustomobject]@{
      fixture_root = Get-RepoRelativePath -Path $fixtureDirectory.directory -Root $repoRoot
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
  if (-not $explicitMaxElapsedMs) {
    $scaledBudget = [int][Math]::Ceiling($fixtures.Count * $defaultPerFixtureBudgetMs)
    if ($scaledBudget -gt $resolvedMaxElapsedMs) {
      $resolvedMaxElapsedMs = $scaledBudget
    }
  }
  $dispatchFixtureCount = @($fixtures | Where-Object { $dispatchFixturePathSet.Contains($_.FullName) }).Count
  if ($dispatchFixtureCount -le 0) {
    throw "perf-budget FAIL: dispatch fixture suite resolved zero fixtures"
  }

  foreach ($fixtureSet in $fixtureSets) {
    Write-Output ("fixture-set: kind={0} root={1} count={2}" -f $fixtureSet.fixture_kind, $fixtureSet.fixture_root, $fixtureSet.fixture_count)
  }

  foreach ($fixture in $fixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot
    $hash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $runDir ("fixture_{0}_{1}" -f $hash, $fixture.BaseName)
    $compileLog = Join-Path $caseDir "compile.log"
    New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

    $compileArgs = @($fixture.FullName, "--out-dir", $caseDir, "--emit-prefix", "module")
    if ($fixture.Extension -eq ".objc3") {
      # Perf-budget is a throughput regression gate, not a backend-availability gate.
      # Force .objc3 perf samples onto clang object backend to avoid llc availability skew.
      $compileArgs += @("--objc3-ir-object-backend", "clang")
    }
    $run = Invoke-TimedNativeCommand `
      -Command $exe `
      -Arguments $compileArgs `
      -LogPath $compileLog

    $objPath = Join-Path $caseDir "module.obj"
    $objExists = Test-Path -LiteralPath $objPath -PathType Leaf
    $objSize = if ($objExists) { (Get-Item -LiteralPath $objPath).Length } else { 0 }

    $passed = ($run.exit_code -eq 0) -and $objExists -and ($objSize -gt 0)
    $detail = if ($passed) {
      "exit=0 obj_bytes=$objSize"
    } elseif ($run.exit_code -ne 0) {
      "expected exit=0 got exit=$($run.exit_code)"
    } elseif (!$objExists) {
      "missing module.obj"
    } else {
      "empty module.obj"
    }

    $results += [pscustomobject]@{
      fixture = $fixtureRel
      elapsed_ms = $run.elapsed_ms
      exit_code = $run.exit_code
      passed = $passed
      detail = $detail
      out_dir = (Get-RepoRelativePath -Path $caseDir -Root $repoRoot)
    }

    $statusToken = if ($passed) { "PASS" } else { "FAIL" }
    Write-Output ("[{0}] {1} elapsed_ms={2} ({3})" -f $statusToken, $fixtureRel, $run.elapsed_ms, $detail)
  }

  $cacheFixture = $null
  if ($dispatchFixtureCount -gt 0) {
    $cacheFixture = @($fixtures | Where-Object { $dispatchFixturePathSet.Contains($_.FullName) } | Select-Object -First 1)[0]
  }
  if ($null -eq $cacheFixture) {
    $cacheFixture = $fixtures[0]
  }
  $cacheFixtureRel = Get-RepoRelativePath -Path $cacheFixture.FullName -Root $repoRoot
  $cacheFixtureKind = if ($dispatchFixturePathSet.Contains($cacheFixture.FullName)) { "dispatch-positive" } else { "recovery-positive" }
  $emitPrefix = "cacheproof_{0}" -f $runId.Replace("_", "")
  $cacheDir = Join-Path $runDir "cache-proof"
  $missDir = Join-Path $cacheDir "miss"
  $hitDir = Join-Path $cacheDir "hit"
  New-Item -ItemType Directory -Force -Path $cacheDir | Out-Null

  $run1Log = Join-Path $cacheDir "run1.log"
  $run2Log = Join-Path $cacheDir "run2.log"
  $cacheScriptArgs = @($cacheFixture.FullName, "--use-cache", "--emit-prefix", $emitPrefix, "--out-dir", $missDir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $cacheScriptArgs += @("--objc3-ir-object-backend", "clang")
  }
  $run1 = Invoke-TimedWrapperCommand `
    -ScriptPath $compileScript `
    -ScriptArguments $cacheScriptArgs `
    -LogPath $run1Log
  if ($run1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-proof run1 failed with exit code $($run1.exit_code)"
  }
  $run1Hit = Parse-CacheHitFlag -OutputText $run1.output_text -RunLabel "cache-proof run1"
  if ($run1Hit) {
    throw "perf-budget FAIL: cache-proof run1 expected cache_hit=false, observed true"
  }

  $cacheScriptArgsHit = @($cacheFixture.FullName, "--use-cache", "--emit-prefix", $emitPrefix, "--out-dir", $hitDir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $cacheScriptArgsHit += @("--objc3-ir-object-backend", "clang")
  }
  $run2 = Invoke-TimedWrapperCommand `
    -ScriptPath $compileScript `
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

  $cacheProof.executed = $true
  $cacheProof.status = "PASS"
  $cacheProof.detail = "run1_hit=false run2_hit=true artifact_hashes_match=true"
  $cacheProof.fixture = $cacheFixtureRel
  $cacheProof.fixture_kind = $cacheFixtureKind
  $cacheProof.emit_prefix = $emitPrefix
  $cacheProof.run1 = [ordered]@{
    elapsed_ms = $run1.elapsed_ms
    exit_code = $run1.exit_code
    cache_hit = $run1Hit
    log = (Get-RepoRelativePath -Path $run1Log -Root $repoRoot)
    out_dir = (Get-RepoRelativePath -Path $missDir -Root $repoRoot)
  }
  $cacheProof.run2 = [ordered]@{
    elapsed_ms = $run2.elapsed_ms
    exit_code = $run2.exit_code
    cache_hit = $run2Hit
    log = (Get-RepoRelativePath -Path $run2Log -Root $repoRoot)
    out_dir = (Get-RepoRelativePath -Path $hitDir -Root $repoRoot)
  }
  $cacheProof.artifacts = [ordered]@{
    miss_sha256 = $missHashes
    hit_sha256 = $hitHashes
  }
  Write-Output ("cache-proof PASS fixture={0} fixture_kind={1} run1_hit={2} run2_hit={3}" -f $cacheFixtureRel, $cacheFixtureKind, $run1Hit, $run2Hit)
} catch {
  $hadFatalError = $true
  $fatalErrorMessage = $_.Exception.Message
  Write-Output ("error: {0}" -f $fatalErrorMessage)
} finally {
  Pop-Location
}

$total = $results.Count
$passedCount = @($results | Where-Object { $_.passed }).Count
$failedCount = $total - $passedCount
$elapsedRows = @($results | ForEach-Object { [double]$_.elapsed_ms })
$totalElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Sum).Sum), 3) } else { 0.0 }
$minFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Minimum).Minimum), 3) } else { 0.0 }
$maxFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Maximum).Maximum), 3) } else { 0.0 }
$avgFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round(($totalElapsedMs / $elapsedRows.Count), 3) } else { 0.0 }
$budgetBreached = $totalElapsedMs -gt $resolvedMaxElapsedMs
$budgetMarginMs = [Math]::Round(($resolvedMaxElapsedMs - $totalElapsedMs), 3)
$cacheProofPassed = $cacheProof.executed -and ($cacheProof.status -eq "PASS")
$status = if (!$hadFatalError -and $total -gt 0 -and $failedCount -eq 0 -and !$budgetBreached -and $cacheProofPassed) { "PASS" } else { "FAIL" }

$summary = [ordered]@{
  run_id = $runId
  run_dir = (Get-RepoRelativePath -Path $runDir -Root $repoRoot)
  generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
  fixture_set = "tests/tooling/fixtures/native/recovery/positive"
  fixture_sets = $fixtureSets
  dispatch_fixture_count = $dispatchFixtureCount
  extra_positive_fixture_dirs = $extraPositiveFixtureDirList
  max_elapsed_ms = $resolvedMaxElapsedMs
  total_elapsed_ms = $totalElapsedMs
  avg_fixture_elapsed_ms = $avgFixtureElapsedMs
  min_fixture_elapsed_ms = $minFixtureElapsedMs
  max_fixture_elapsed_ms = $maxFixtureElapsedMs
  budget_breached = $budgetBreached
  budget_margin_ms = $budgetMarginMs
  build_executed = $buildExecuted
  build_elapsed_ms = $buildElapsedMs
  cache_proof = $cacheProof
  total = $total
  passed = $passedCount
  failed = $failedCount
  status = $status
  fatal_error = $fatalErrorMessage
  results = $results
}
$summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $summaryPath -Encoding utf8

$run1HitValue = if ($null -ne $cacheProof.run1) { [bool]$cacheProof.run1.cache_hit } else { $false }
$run2HitValue = if ($null -ne $cacheProof.run2) { [bool]$cacheProof.run2.cache_hit } else { $false }
Write-Output ("budget_ms: max={0} total={1} margin={2}" -f $resolvedMaxElapsedMs, $totalElapsedMs, $budgetMarginMs)
Write-Output ("cache_proof: status={0} run1_hit={1} run2_hit={2}" -f $cacheProof.status, $run1HitValue, $run2HitValue)
Write-Output ("summary: total={0} passed={1} failed={2}" -f $total, $passedCount, $failedCount)
Write-Output ("summary_path: {0}" -f (Get-RepoRelativePath -Path $summaryPath -Root $repoRoot))
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
