param(
  [Nullable[int]]$MaxElapsedMs,
  [string]$ExtraPositiveFixtureDirs,
  [switch]$EnforceTimingGate
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
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

function Read-JsonObject {
  param([string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "perf-budget FAIL: missing JSON artifact at $Path"
  }

  try {
    if ($PSVersionTable.PSVersion.Major -ge 6) {
      return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json -AsHashtable)
    }
    return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
  } catch {
    throw "perf-budget FAIL: invalid JSON artifact at $Path"
  }
}

function Get-FileSha256Hex {
  param([string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "perf-budget FAIL: missing file for hashing at $Path"
  }

  return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-CompileArtifactSurface {
  param(
    [string]$OutputDirectory,
    [string]$RepoRoot
  )

  $manifestPath = Join-Path $OutputDirectory "module.manifest.json"
  if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    return [ordered]@{
      manifest_present = $false
    }
  }

  $manifest = Read-JsonObject -Path $manifestPath
  $frontend = $manifest["frontend"]
  $pipeline = if ($null -ne $frontend) { $frontend["pipeline"] } else { $null }
  $stages = if ($null -ne $pipeline) { $pipeline["stages"] } else { $null }
  $parserStage = if ($null -ne $stages) { $stages["parser"] } else { $null }
  $semanticStage = if ($null -ne $stages) { $stages["semantic"] } else { $null }
  $semanticSurface = if ($null -ne $pipeline) { $pipeline["semantic_surface"] } else { $null }
  $loweringSurface = $manifest["lowering_incremental_module_cache_invalidation"]

  return [ordered]@{
    manifest_present = $true
    manifest_path = (Get-RepoRelativePath -Path $manifestPath -Root $RepoRoot)
    manifest_sha256 = (Get-FileSha256Hex -Path $manifestPath)
    parser_diagnostics = if ($null -ne $parserStage -and $null -ne $parserStage["diagnostics"]) { [int]$parserStage["diagnostics"] } else { 0 }
    semantic_diagnostics = if ($null -ne $semanticStage -and $null -ne $semanticStage["diagnostics"]) { [int]$semanticStage["diagnostics"] } else { 0 }
    semantic_skipped = if ($null -ne $pipeline -and $null -ne $pipeline["semantic_skipped"]) { [bool]$pipeline["semantic_skipped"] } else { $false }
    declared_globals = if ($null -ne $semanticSurface -and $null -ne $semanticSurface["declared_globals"]) { [int]$semanticSurface["declared_globals"] } else { 0 }
    declared_functions = if ($null -ne $semanticSurface -and $null -ne $semanticSurface["declared_functions"]) { [int]$semanticSurface["declared_functions"] } else { 0 }
    lowering_replay_key = if ($null -ne $loweringSurface -and $null -ne $loweringSurface["replay_key"]) { [string]$loweringSurface["replay_key"] } else { "" }
  }
}

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
    $outputLines = & $ScriptPath @ScriptArguments 2>&1
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
    $scaledBudget = [int][Math]::Ceiling($fixtures.Count * $resolvedPerFixtureBudgetMs)
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
    $artifactSurface = if ($passed) {
      Get-CompileArtifactSurface -OutputDirectory $caseDir -RepoRoot $repoRoot
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

    $results += [pscustomobject]@{
      fixture = $fixtureRel
      elapsed_ms = $run.elapsed_ms
      exit_code = $run.exit_code
      passed = $passed
      detail = $detail
      out_dir = (Get-RepoRelativePath -Path $caseDir -Root $repoRoot)
      compile_artifact_surface = $artifactSurface
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

  $invalidationDir = Join-Path $runDir "cache-invalidation"
  $invalidationSource = Join-Path $invalidationDir $cacheFixture.Name
  New-Item -ItemType Directory -Force -Path $invalidationDir | Out-Null
  Copy-Item -LiteralPath $cacheFixture.FullName -Destination $invalidationSource -Force

  $invalidationRun1Dir = Join-Path $invalidationDir "run1"
  $invalidationRun2Dir = Join-Path $invalidationDir "run2"
  $invalidationRun1Log = Join-Path $invalidationDir "run1.log"
  $invalidationRun2Log = Join-Path $invalidationDir "run2.log"

  $invalidationArgs = @($invalidationSource, "--use-cache", "--emit-prefix", $emitPrefix, "--out-dir", $invalidationRun1Dir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $invalidationArgs += @("--objc3-ir-object-backend", "clang")
  }
  $invalidationRun1 = Invoke-TimedWrapperCommand `
    -ScriptPath $compileScript `
    -ScriptArguments $invalidationArgs `
    -LogPath $invalidationRun1Log
  if ($invalidationRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run1 failed with exit code $($invalidationRun1.exit_code)"
  }
  $invalidationRun1Hit = Parse-CacheHitFlag -OutputText $invalidationRun1.output_text -RunLabel "cache-invalidation run1"

  Add-Content -LiteralPath $invalidationSource -Value "// cache invalidation probe mutation"

  $invalidationArgs2 = @($invalidationSource, "--use-cache", "--emit-prefix", $emitPrefix, "--out-dir", $invalidationRun2Dir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $invalidationArgs2 += @("--objc3-ir-object-backend", "clang")
  }
  $invalidationRun2 = Invoke-TimedWrapperCommand `
    -ScriptPath $compileScript `
    -ScriptArguments $invalidationArgs2 `
    -LogPath $invalidationRun2Log
  if ($invalidationRun2.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run2 failed with exit code $($invalidationRun2.exit_code)"
  }
  $invalidationRun2Hit = Parse-CacheHitFlag -OutputText $invalidationRun2.output_text -RunLabel "cache-invalidation run2"
  if ($invalidationRun2Hit) {
    throw "perf-budget FAIL: cache-invalidation run2 expected cache_hit=false after source mutation"
  }

  $cacheInvalidationProof.executed = $true
  $cacheInvalidationProof.status = "PASS"
  $cacheInvalidationProof.detail = "source mutation invalidated wrapper cache key"
  $cacheInvalidationProof.fixture = (Get-RepoRelativePath -Path $cacheFixture.FullName -Root $repoRoot)
  $cacheInvalidationProof.run1 = [ordered]@{
    elapsed_ms = $invalidationRun1.elapsed_ms
    exit_code = $invalidationRun1.exit_code
    cache_hit = $invalidationRun1Hit
    log = (Get-RepoRelativePath -Path $invalidationRun1Log -Root $repoRoot)
    out_dir = (Get-RepoRelativePath -Path $invalidationRun1Dir -Root $repoRoot)
  }
  $cacheInvalidationProof.run2 = [ordered]@{
    elapsed_ms = $invalidationRun2.elapsed_ms
    exit_code = $invalidationRun2.exit_code
    cache_hit = $invalidationRun2Hit
    log = (Get-RepoRelativePath -Path $invalidationRun2Log -Root $repoRoot)
    out_dir = (Get-RepoRelativePath -Path $invalidationRun2Dir -Root $repoRoot)
  }
  Write-Output ("cache-invalidation PASS fixture={0} run1_hit={1} run2_hit={2}" -f $cacheFixtureRel, $invalidationRun1Hit, $invalidationRun2Hit)

  if (!(Test-Path -LiteralPath $macroHostFixture -PathType Leaf)) {
    throw "perf-budget FAIL: missing macro-host fixture at $macroHostFixture"
  }
  $macroHostDir = Join-Path $runDir "macro-host-cache"
  $macroHostRun1Dir = Join-Path $macroHostDir "run1"
  $macroHostRun2Dir = Join-Path $macroHostDir "run2"
  $macroHostRun1Log = Join-Path $macroHostDir "run1.log"
  $macroHostRun2Log = Join-Path $macroHostDir "run2.log"
  New-Item -ItemType Directory -Force -Path $macroHostDir | Out-Null
  $macroEmitPrefix = "macrohost_{0}" -f $runId.Replace("_", "")

  $macroHostArgs = @($macroHostFixture, "--use-cache", "--emit-prefix", $macroEmitPrefix, "--out-dir", $macroHostRun1Dir, "--objc3-ir-object-backend", "clang")
  $macroHostRun1 = Invoke-TimedWrapperCommand `
    -ScriptPath $compileScript `
    -ScriptArguments $macroHostArgs `
    -LogPath $macroHostRun1Log
  if ($macroHostRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: macro-host cache run1 failed with exit code $($macroHostRun1.exit_code)"
  }
  $macroHostRun1Hit = Parse-CacheHitFlag -OutputText $macroHostRun1.output_text -RunLabel "macro-host cache run1"

  $macroHostArgs2 = @($macroHostFixture, "--use-cache", "--emit-prefix", $macroEmitPrefix, "--out-dir", $macroHostRun2Dir, "--objc3-ir-object-backend", "clang")
  $macroHostRun2 = Invoke-TimedWrapperCommand `
    -ScriptPath $compileScript `
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

  $macroHostProof.executed = $true
  $macroHostProof.status = "PASS"
  $macroHostProof.detail = "macro-host cache artifact published through the live wrapper path"
  $macroHostProof.fixture = (Get-RepoRelativePath -Path $macroHostFixture -Root $repoRoot)
  $macroHostProof.run1 = [ordered]@{
    elapsed_ms = $macroHostRun1.elapsed_ms
    exit_code = $macroHostRun1.exit_code
    cache_hit = $macroHostRun1Hit
    log = (Get-RepoRelativePath -Path $macroHostRun1Log -Root $repoRoot)
    out_dir = (Get-RepoRelativePath -Path $macroHostRun1Dir -Root $repoRoot)
  }
  $macroHostProof.run2 = [ordered]@{
    elapsed_ms = $macroHostRun2.elapsed_ms
    exit_code = $macroHostRun2.exit_code
    cache_hit = $macroHostRun2Hit
    log = (Get-RepoRelativePath -Path $macroHostRun2Log -Root $repoRoot)
    out_dir = (Get-RepoRelativePath -Path $macroHostRun2Dir -Root $repoRoot)
  }
  $macroHostProof.cache_artifact = (Get-RepoRelativePath -Path $macroHostArtifact -Root $repoRoot)
  Write-Output ("macro-host-cache PASS fixture={0} run1_hit={1} run2_hit={2}" -f $macroHostProof.fixture, $macroHostRun1Hit, $macroHostRun2Hit)

  if (!(Test-Path -LiteralPath $nativeDocsScript -PathType Leaf)) {
    throw "perf-budget FAIL: missing native docs generator at $nativeDocsScript"
  }
  if (!(Test-Path -LiteralPath $commandSurfaceScript -PathType Leaf)) {
    throw "perf-budget FAIL: missing public command surface generator at $commandSurfaceScript"
  }
  $docsDir = Join-Path $runDir "docs-generation"
  New-Item -ItemType Directory -Force -Path $docsDir | Out-Null
  $nativeDocsLog = Join-Path $docsDir "native-docs.log"
  $commandSurfaceLog = Join-Path $docsDir "public-command-surface.log"
  $nativeDocsRun = Invoke-TimedNativeCommand `
    -Command $pythonCommand `
    -Arguments @($nativeDocsScript) `
    -LogPath $nativeDocsLog
  if ($nativeDocsRun.exit_code -ne 0) {
    throw "perf-budget FAIL: native docs generation failed with exit code $($nativeDocsRun.exit_code)"
  }
  $commandSurfaceRun = Invoke-TimedNativeCommand `
    -Command $pythonCommand `
    -Arguments @($commandSurfaceScript) `
    -LogPath $commandSurfaceLog
  if ($commandSurfaceRun.exit_code -ne 0) {
    throw "perf-budget FAIL: public command surface generation failed with exit code $($commandSurfaceRun.exit_code)"
  }

  $docsGenerationProof.executed = $true
  $docsGenerationProof.status = "PASS"
  $docsGenerationProof.detail = "checked-in docs generators executed on the live repo surface"
  $docsGenerationProof.native_docs = [ordered]@{
    elapsed_ms = $nativeDocsRun.elapsed_ms
    exit_code = $nativeDocsRun.exit_code
    log = (Get-RepoRelativePath -Path $nativeDocsLog -Root $repoRoot)
  }
  $docsGenerationProof.public_command_surface = [ordered]@{
    elapsed_ms = $commandSurfaceRun.elapsed_ms
    exit_code = $commandSurfaceRun.exit_code
    log = (Get-RepoRelativePath -Path $commandSurfaceLog -Root $repoRoot)
  }
  Write-Output ("docs-generation PASS native_docs_ms={0} command_surface_ms={1}" -f $nativeDocsRun.elapsed_ms, $commandSurfaceRun.elapsed_ms)
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
$timingGateViolated = $timingGateEnforced -and $budgetBreached
$budgetMarginMs = [Math]::Round(($resolvedMaxElapsedMs - $totalElapsedMs), 3)
$cacheProofPassed = $cacheProof.executed -and ($cacheProof.status -eq "PASS")
$status = if (!$hadFatalError -and $total -gt 0 -and $failedCount -eq 0 -and !$timingGateViolated -and $cacheProofPassed) { "PASS" } else { "FAIL" }

$summary = [ordered]@{
  contract_id = "objc3c.compiler.throughput.summary.v1"
  benchmark_kind = "native-direct-compile-throughput"
  run_id = $runId
  run_dir = (Get-RepoRelativePath -Path $runDir -Root $repoRoot)
  generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
  fixture_set = "tests/tooling/fixtures/native/recovery/positive"
  fixture_sets = $fixtureSets
  dispatch_fixture_count = $dispatchFixtureCount
  extra_positive_fixture_dirs = $extraPositiveFixtureDirList
  budget_profile = $resolvedBudgetProfile
  base_per_fixture_budget_ms = $defaultPerFixtureBudgetMs
  launch_overhead_per_fixture_ms = $resolvedLaunchOverheadPerFixtureMs
  per_fixture_budget_ms = $resolvedPerFixtureBudgetMs
  explicit_max_elapsed_ms = $explicitMaxElapsedMs
  max_elapsed_ms = $resolvedMaxElapsedMs
  total_elapsed_ms = $totalElapsedMs
  avg_fixture_elapsed_ms = $avgFixtureElapsedMs
  min_fixture_elapsed_ms = $minFixtureElapsedMs
  max_fixture_elapsed_ms = $maxFixtureElapsedMs
  budget_breached = $budgetBreached
  timing_gate_enforced = $timingGateEnforced
  timing_gate_violated = $timingGateViolated
  budget_margin_ms = $budgetMarginMs
  build_executed = $buildExecuted
  build_elapsed_ms = $buildElapsedMs
  cache_proof = $cacheProof
  cache_invalidation_proof = $cacheInvalidationProof
  macro_host_cache_proof = $macroHostProof
  docs_generation_proof = $docsGenerationProof
  workload_summary = [ordered]@{
    direct_compile_fixture_count = $total
    direct_compile_total_elapsed_ms = $totalElapsedMs
    cache_proof_elapsed_ms = if ($cacheProof.executed -and $null -ne $cacheProof.run1 -and $null -ne $cacheProof.run2) {
      [Math]::Round(([double]$cacheProof.run1.elapsed_ms + [double]$cacheProof.run2.elapsed_ms), 3)
    } else {
      0.0
    }
    cache_invalidation_elapsed_ms = if ($cacheInvalidationProof.executed -and $null -ne $cacheInvalidationProof.run1 -and $null -ne $cacheInvalidationProof.run2) {
      [Math]::Round(([double]$cacheInvalidationProof.run1.elapsed_ms + [double]$cacheInvalidationProof.run2.elapsed_ms), 3)
    } else {
      0.0
    }
    macro_host_cache_elapsed_ms = if ($macroHostProof.executed -and $null -ne $macroHostProof.run1 -and $null -ne $macroHostProof.run2) {
      [Math]::Round(([double]$macroHostProof.run1.elapsed_ms + [double]$macroHostProof.run2.elapsed_ms), 3)
    } else {
      0.0
    }
    docs_generation_elapsed_ms = if ($docsGenerationProof.executed -and $null -ne $docsGenerationProof.native_docs -and $null -ne $docsGenerationProof.public_command_surface) {
      [Math]::Round(([double]$docsGenerationProof.native_docs.elapsed_ms + [double]$docsGenerationProof.public_command_surface.elapsed_ms), 3)
    } else {
      0.0
    }
  }
  total = $total
  passed = $passedCount
  failed = $failedCount
  status = $status
  fatal_error = $fatalErrorMessage
  results = $results
}
$summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $summaryPath -Encoding utf8
New-Item -ItemType Directory -Force -Path $reportRoot | Out-Null
$summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $reportPath -Encoding utf8

$run1HitValue = if ($null -ne $cacheProof.run1) { [bool]$cacheProof.run1.cache_hit } else { $false }
$run2HitValue = if ($null -ne $cacheProof.run2) { [bool]$cacheProof.run2.cache_hit } else { $false }
Write-Output ("budget_ms: max={0} total={1} margin={2}" -f $resolvedMaxElapsedMs, $totalElapsedMs, $budgetMarginMs)
Write-Output ("budget_profile: profile={0} base_per_fixture_ms={1} launch_overhead_per_fixture_ms={2} per_fixture_ms={3} explicit_max={4}" -f $resolvedBudgetProfile, $defaultPerFixtureBudgetMs, $resolvedLaunchOverheadPerFixtureMs, $resolvedPerFixtureBudgetMs, $explicitMaxElapsedMs)
Write-Output ("timing_gate: enforced={0} violated={1}" -f $timingGateEnforced, $timingGateViolated)
Write-Output ("cache_proof: status={0} run1_hit={1} run2_hit={2}" -f $cacheProof.status, $run1HitValue, $run2HitValue)
Write-Output ("summary: total={0} passed={1} failed={2}" -f $total, $passedCount, $failedCount)
Write-Output ("summary_path: {0}" -f (Get-RepoRelativePath -Path $summaryPath -Root $repoRoot))
Write-Output ("report_path: {0}" -f (Get-RepoRelativePath -Path $reportPath -Root $repoRoot))
if ($budgetBreached -and -not $timingGateEnforced) {
  Write-Output ("warning: timing budget breached but enforcement disabled (set OBJC3C_NATIVE_PERF_ENFORCE_TIMING_GATE=1 to fail-closed)")
}
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
