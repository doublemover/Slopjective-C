$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_reporting.psm1") -Force -DisableNameChecking

$script:Objc3cDiagnosticsExecutionModuleRoot = Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_execution"
Import-Module (Join-Path $script:Objc3cDiagnosticsExecutionModuleRoot "command_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:Objc3cDiagnosticsExecutionModuleRoot "case_execution.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:Objc3cDiagnosticsExecutionModuleRoot "report_shaping.psm1") -Force -DisableNameChecking

function Invoke-Objc3cDiagnosticsRegressionSuite {
  param([string]$ScriptRoot)

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/diagnostics-regression"
  $runId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
  $runDir = Join-Path $suiteRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
  $exePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $fatalErrorMessage = ""
  $hadFatalError = $false

  New-Item -ItemType Directory -Force -Path $runDir | Out-Null

  $results = @()

  Push-Location $repoRoot
  try {
    if (-not (Test-Path -LiteralPath $buildScript -PathType Leaf)) {
      throw "suite FAIL: missing build script at $buildScript"
    }

    if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
      $buildLogPath = Join-Path $runDir "build.log"
      $buildExit = Invoke-Objc3cDiagnosticsLoggedNativeCommand `
        -Command "powershell" `
        -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript) `
        -LogPath $buildLogPath
      if ($buildExit -ne 0) {
        throw "suite FAIL: native compiler build failed with exit code $buildExit"
      }
    }

    if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
      throw "suite FAIL: native compiler executable missing at $exePath"
    }

    $fixtures = @(Get-Objc3cDiagnosticsFixtures -FixtureDir $fixtureDir)

    foreach ($fixture in $fixtures) {
      $caseResult = Invoke-Objc3cDiagnosticsRegressionCase `
        -Fixture $fixture `
        -RepoRoot $repoRoot `
        -RunDir $runDir `
        -ExePath $exePath
      Write-Objc3cDiagnosticsCaseResult `
        -Passed $caseResult.passed `
        -FixtureRelativePath $caseResult.fixture `
        -Errors @($caseResult.errors)
      $results += $caseResult
    }
  } catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output "error: $fatalErrorMessage"
  } finally {
    Pop-Location
  }

  $report = New-Objc3cDiagnosticsRegressionSuiteReport `
    -RunId $runId `
    -RunDir $runDir `
    -RepoRoot $repoRoot `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage `
    -Results $results

  Write-Objc3cDiagnosticsRegressionSummary -Summary $report.summary -SummaryPath $summaryPath -RepoRoot $repoRoot

  if ($report.status -ne "PASS") {
    exit 1
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cDiagnosticsRegressionSuite"
)
