$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "paths.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "fixture_discovery.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "native_command_execution.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "result_classification.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_writing.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativeFixtureMatrix {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $fixtureMatrixRoot = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/dispatch"
  $runId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
  $matrixRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/fixture-matrix"
  $runDir = Join-Path $matrixRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $hadFatalError = $false
  $fatalErrorMessage = ""
  $selectedPositiveCount = 0

  New-Item -ItemType Directory -Force -Path $runDir | Out-Null

  $results = @()

  Push-Location $repoRoot
  try {
    $exe = Resolve-Objc3cNativeFixtureMatrixCompiler `
      -RepoRoot $repoRoot `
      -RunDir $runDir

    $positiveFixtures = Get-Objc3cNativeFixtureMatrixFixtures `
      -Directory $fixtureMatrixRoot `
      -FixtureKind "fixture-matrix positive"
    $positiveFixtures = Select-Objc3cNativeFixtureMatrixFixtures `
      -Fixtures $positiveFixtures `
      -FixtureListPath $FixtureList `
      -FixtureGlobPattern $FixtureGlob `
      -ShardIndexValue $ShardIndex `
      -ShardCountValue $ShardCount `
      -LimitValue $Limit `
      -RepoRoot $repoRoot
    $selectedPositiveCount = $positiveFixtures.Count
    Write-Output ("selection: positive={0}" -f $positiveFixtures.Count)

    foreach ($fixture in $positiveFixtures) {
      $fixtureRel = Get-Objc3cNativeFixtureMatrixRepoRelativePath -Path $fixture.FullName -Root $repoRoot
      $hash = Get-Objc3cNativeFixtureMatrixShortHash -Value $fixtureRel
      $caseDir = Join-Path $runDir ("positive_{0}_{1}" -f $hash, $fixture.BaseName)
      $compileLog = Join-Path $caseDir "compile.log"
      New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

      $fixtureNativeCompileArgs = @(Get-Objc3cNativeFixtureMatrixCompileArgs -Fixture $fixture)
      $compilerArgs = @($fixture.FullName, "--out-dir", $caseDir, "--emit-prefix", "module") + $fixtureNativeCompileArgs

      $exitCode = Invoke-Objc3cNativeFixtureMatrixLoggedCommand `
        -Command $exe `
        -Arguments $compilerArgs `
        -LogPath $compileLog

      $result = New-Objc3cNativeFixtureMatrixPositiveResult `
        -FixtureRel $fixtureRel `
        -ExitCode $exitCode `
        -CaseDir $caseDir `
        -RepoRoot $repoRoot
      $results += $result

      $statusToken = if ($result.passed) { "PASS" } else { "FAIL" }
      Write-Output ("[{0}] positive {1} ({2})" -f $statusToken, $fixtureRel, $result.detail)
    }
  } catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalErrorMessage)
  } finally {
    Pop-Location
  }

  $report = Write-Objc3cNativeFixtureMatrixReport `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -SummaryPath $summaryPath `
    -Results $results `
    -FixtureList $FixtureList `
    -FixtureGlob $FixtureGlob `
    -ShardIndex $ShardIndex `
    -ShardCount $ShardCount `
    -Limit $Limit `
    -SelectedPositiveCount $selectedPositiveCount `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage

  Write-Output ("summary: total={0} passed={1} failed={2}" -f $report.total, $report.passed, $report.failed)
  Write-Output ("summary_path: {0}" -f $report.summary_path)
  Write-Output ("status: {0}" -f $report.status)

  if ($report.status -ne "PASS") {
    exit 1
  }
}

Export-ModuleMember -Function "Invoke-Objc3cNativeFixtureMatrix"
