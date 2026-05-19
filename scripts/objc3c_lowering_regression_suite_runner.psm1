Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_cases.psm1") -Force -DisableNameChecking

function Invoke-Objc3cLoweringRegressionSuite {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $ErrorActionPreference = "Stop"
  Set-StrictMode -Version Latest
  if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $false
  }

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $positiveRecoveryFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/positive"
  $optionalDispatchFixtureDirs = @(
    (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/positive/lowering_dispatch"),
    (Join-Path $repoRoot "tests/tooling/fixtures/native/dispatch/positive")
  )
  $negativeFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-regression"
  $configuredRunId = $env:OBJC3C_NATIVE_LOWERING_RUN_ID
  $defaultRunId = "typed_abi-lane-c-lowering-regression-default"

  $runId = Resolve-ValidatedRunId -ConfiguredRunId $configuredRunId -DefaultRunId $defaultRunId
  $runDir = Join-Path $suiteRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $deterministicSummaryPath = Join-Path $suiteRoot "latest-summary.json"
  $exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
  $configuredClangPath = $env:OBJC3C_NATIVE_LOWERING_CLANG_PATH
  $clangCommand = if ([string]::IsNullOrWhiteSpace($configuredClangPath)) { "clang" } else { $configuredClangPath }
  $suiteContext = [pscustomobject]@{
    RepoRoot = $repoRoot
    RunDir = $runDir
    NativeCompilerExe = $exe
    ClangCommand = $clangCommand
  }

  New-Item -ItemType Directory -Force -Path $runDir | Out-Null

  $startedAtUtc = (Get-Date).ToUniversalTime().ToString("o")
  $results = @()
  $fatalError = ""
  $fixtureRoots = @(
    (Get-RepoRelativePath -Path $positiveRecoveryFixtureDir -Root $repoRoot),
    (Get-RepoRelativePath -Path $negativeFixtureDir -Root $repoRoot)
  )

  Push-Location $repoRoot
  try {
    if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
      $buildLog = Join-Path $runDir "build.log"
      $buildExit = Invoke-LoggedCommand `
        -Command "powershell" `
        -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript) `
        -LogPath $buildLog
      if ($buildExit -ne 0) {
        throw "lowering regression FAIL: build failed with exit code $buildExit"
      }
    }

    if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
      throw "lowering regression FAIL: native compiler executable missing at $exe"
    }
    if ([System.IO.Path]::IsPathRooted($clangCommand) -and !(Test-Path -LiteralPath $clangCommand -PathType Leaf)) {
      throw "lowering regression FAIL: clang executable missing at $clangCommand"
    }

    $positiveFixtures = @(
      Get-Fixtures -Directory $positiveRecoveryFixtureDir -FixtureKind "positive recovery" -Extensions @(".objc3")
    )
    $dispatchFixtures = @()
    foreach ($dispatchFixtureDir in $optionalDispatchFixtureDirs) {
      if (!(Test-Path -LiteralPath $dispatchFixtureDir -PathType Container)) {
        Write-Output ("[INFO] optional dispatch fixture directory not found: {0}" -f $dispatchFixtureDir)
        continue
      }

      $fixtureRoots += (Get-RepoRelativePath -Path $dispatchFixtureDir -Root $repoRoot)
      $discoveredFixtures = @(
        Get-ChildItem -LiteralPath $dispatchFixtureDir -Recurse -File |
          Where-Object { $_.Extension -ieq ".m" } |
          Sort-Object -Property FullName
      )
      if ($discoveredFixtures.Count -eq 0) {
        Write-Output ("[INFO] optional dispatch fixture directory has no .m fixtures: {0}" -f $dispatchFixtureDir)
        continue
      }

      $dispatchFixtures += $discoveredFixtures
    }

    if ($dispatchFixtures.Count -gt 0) {
      $positiveFixtures += $dispatchFixtures
    }

    $positiveFixtures = @($positiveFixtures | Sort-Object -Property FullName -Unique)
    $negativeFixtures = Get-Fixtures -Directory $negativeFixtureDir -FixtureKind "negative recovery" -Extensions @(".objc3")
    $fixtureRoots = @($fixtureRoots | Select-Object -Unique)

    foreach ($fixture in $positiveFixtures) {
      $caseResult = Invoke-LoweringCase -Fixture $fixture -FixtureKind "positive" -SuiteContext $suiteContext
      $results += $caseResult
      $statusToken = if ($caseResult.passed) { "PASS" } else { "FAIL" }
      if ($caseResult.passed) {
        Write-Output ("[{0}] positive {1}" -f $statusToken, $caseResult.fixture)
      }
      else {
        Write-Output ("[{0}] positive {1} ({2})" -f $statusToken, $caseResult.fixture, $caseResult.detail)
      }
    }

    foreach ($fixture in $negativeFixtures) {
      $caseResult = Invoke-LoweringCase -Fixture $fixture -FixtureKind "negative" -SuiteContext $suiteContext
      $results += $caseResult
      $statusToken = if ($caseResult.passed) { "PASS" } else { "FAIL" }
      if ($caseResult.passed) {
        Write-Output ("[{0}] negative {1}" -f $statusToken, $caseResult.fixture)
      }
      else {
        Write-Output ("[{0}] negative {1} ({2})" -f $statusToken, $caseResult.fixture, $caseResult.detail)
      }
    }
  }
  catch {
    $fatalError = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalError)
  }
  finally {
    Pop-Location
  }

  $total = $results.Count
  $passedCount = @($results | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $status = if ([string]::IsNullOrWhiteSpace($fatalError) -and $total -gt 0 -and $failedCount -eq 0) { "PASS" } else { "FAIL" }

  $summary = [ordered]@{
    run_id = $runId
    started_at_utc = $startedAtUtc
    completed_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    run_dir = (Get-RepoRelativePath -Path $runDir -Root $repoRoot)
    fixture_roots = $fixtureRoots
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = $status
    fatal_error = $fatalError
    results = $results
  }
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

  $deterministicResults = @(
    $results |
      Sort-Object -Property kind, fixture |
      ForEach-Object {
        [ordered]@{
          kind = $_.kind
          fixture = $_.fixture
          passed = $_.passed
          detail = $_.detail
          run1_exit_code = $_.run1_exit_code
          run2_exit_code = $_.run2_exit_code
          checks = $_.checks
        }
      }
  )

  $deterministicSummary = [ordered]@{
    fixture_roots = $fixtureRoots
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = $status
    fatal_error = $fatalError
    results = $deterministicResults
  }
  $deterministicSummary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $deterministicSummaryPath -Encoding utf8

  Write-Output ("summary: total={0} passed={1} failed={2}" -f $total, $passedCount, $failedCount)
  Write-Output ("summary_path: {0}" -f (Get-RepoRelativePath -Path $summaryPath -Root $repoRoot))
  Write-Output ("deterministic_summary_path: {0}" -f (Get-RepoRelativePath -Path $deterministicSummaryPath -Root $repoRoot))
  Write-Output ("status: {0}" -f $status)

  if ($status -ne "PASS") {
    exit 1
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cLoweringRegressionSuite"
)
