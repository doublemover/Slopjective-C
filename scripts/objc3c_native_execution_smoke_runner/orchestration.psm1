Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "case_execution.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "config.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "fixtures.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "summary.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "timings.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativeExecutionSmokeCore {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  $context = Resolve-Objc3cNativeExecutionSmokeConfig -ScriptRoot $ScriptRoot
  New-Item -ItemType Directory -Force -Path $context.run_dir | Out-Null
  Push-Location $context.repo_root
  try {
    $suiteStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    Initialize-ExecutionSmokeStageTimings | Out-Null

    Ensure-NativeCompilerExecutable `
      -NativeExePath $context.native_exe `
      -NativeExeExplicit $context.native_exe_explicit `
      -BuildScriptPath $context.build_script

    $clangCheckExit = Invoke-LoggedCommand -Command $context.clang_command -Arguments @("--version") -LogPath (Join-Path $context.run_dir "clang-version.log")
    if ($clangCheckExit -ne 0) {
      throw "execution smoke FAIL: clang command is unavailable: $($context.clang_command)"
    }

    $positiveFixtures = Get-Fixtures -Directory $context.positive_fixture_dir -FixtureKind "positive execution"
    $negativeFixtures = Get-Fixtures -Directory $context.negative_fixture_dir -FixtureKind "negative execution"
    $executionFixtureEntries = New-ExecutionFixtureEntries `
      -PositiveFixtures $positiveFixtures `
      -NegativeFixtures $negativeFixtures `
      -RepoRoot $context.repo_root
    $selectedExecutionEntries = Select-ExecutionFixtureEntries `
      -Entries $executionFixtureEntries `
      -FixtureListPath $FixtureList `
      -FixtureGlobPattern $FixtureGlob `
      -ShardIndexValue $ShardIndex `
      -ShardCountValue $ShardCount `
      -LimitValue $Limit `
      -RepoRoot $context.repo_root
    $selectedPositiveFixtures = @($selectedExecutionEntries | Where-Object { $_.kind -eq "positive" } | ForEach-Object { $_.file })
    $selectedNegativeFixtures = @($selectedExecutionEntries | Where-Object { $_.kind -eq "negative" } | ForEach-Object { $_.file })
    Write-Output ("selection: positive={0} negative={1}" -f $selectedPositiveFixtures.Count, $selectedNegativeFixtures.Count)

    $results = [System.Collections.Generic.List[object]]::new()
    $caseTimings = [System.Collections.Generic.List[object]]::new()
    $totalSelectedFixtures = $selectedPositiveFixtures.Count + $selectedNegativeFixtures.Count
    $fixtureIndex = 0
    $lastCompletedFixture = "none"
    $currentFixtureKind = "unknown"
    $currentFixtureRel = "unknown"

    try {
      foreach ($fixture in $selectedPositiveFixtures) {
        $fixtureIndex += 1
        $currentFixtureKind = "positive"
        $currentFixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $context.repo_root
        Invoke-PositiveExecutionSmokeFixture `
          -Fixture $fixture `
          -Context $context `
          -FixtureIndex $fixtureIndex `
          -TotalSelectedFixtures $totalSelectedFixtures `
          -SuiteStopwatch $suiteStopwatch `
          -Results $results `
          -CaseTimings $caseTimings `
          -LastCompletedFixture ([ref]$lastCompletedFixture)
      }

      foreach ($fixture in $selectedNegativeFixtures) {
        $fixtureIndex += 1
        $currentFixtureKind = "negative"
        $currentFixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $context.repo_root
        Invoke-NegativeExecutionSmokeFixture `
          -Fixture $fixture `
          -Context $context `
          -FixtureIndex $fixtureIndex `
          -TotalSelectedFixtures $totalSelectedFixtures `
          -SuiteStopwatch $suiteStopwatch `
          -Results $results `
          -CaseTimings $caseTimings `
          -LastCompletedFixture ([ref]$lastCompletedFixture)
      }
    }
    catch {
      Write-FailedExecutionSmokeSummary `
        -Context $context `
        -SuiteStopwatch $suiteStopwatch `
        -Results $results `
        -CaseTimings $caseTimings `
        -FixtureList $FixtureList `
        -FixtureGlob $FixtureGlob `
        -ShardIndex $ShardIndex `
        -ShardCount $ShardCount `
        -Limit $Limit `
        -SelectedPositiveCount $selectedPositiveFixtures.Count `
        -SelectedNegativeCount $selectedNegativeFixtures.Count `
        -FailedFixtureKind $currentFixtureKind `
        -FailedFixtureRel $currentFixtureRel `
        -FailedFixtureIndex $fixtureIndex `
        -TotalSelectedFixtures $totalSelectedFixtures `
        -LastCompletedFixture $lastCompletedFixture `
        -ErrorRecord $_
      throw
    }

    Write-ExecutionSmokeSummary `
      -Context $context `
      -SuiteStopwatch $suiteStopwatch `
      -Results $results `
      -CaseTimings $caseTimings `
      -FixtureList $FixtureList `
      -FixtureGlob $FixtureGlob `
      -ShardIndex $ShardIndex `
      -ShardCount $ShardCount `
      -Limit $Limit `
      -SelectedPositiveCount $selectedPositiveFixtures.Count `
      -SelectedNegativeCount $selectedNegativeFixtures.Count
  }
  finally {
    Pop-Location
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativeExecutionSmokeCore"
)
