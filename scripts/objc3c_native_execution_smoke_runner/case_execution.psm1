Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$positiveExecutionModule = Join-Path $PSScriptRoot "case_execution_positive.psm1"
$negativeExecutionModule = Join-Path $PSScriptRoot "case_execution_negative.psm1"
foreach ($modulePath in @($positiveExecutionModule, $negativeExecutionModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native execution smoke case execution dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Invoke-PositiveExecutionSmokeFixture {
  param(
    [Parameter(Mandatory = $true)][object]$Fixture,
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][int]$FixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$Results,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$CaseTimings,
    [Parameter(Mandatory = $true)][ref]$LastCompletedFixture
  )

  Invoke-PositiveExecutionSmokeFixtureImpl `
    -Fixture $Fixture `
    -Context $Context `
    -FixtureIndex $FixtureIndex `
    -TotalSelectedFixtures $TotalSelectedFixtures `
    -SuiteStopwatch $SuiteStopwatch `
    -Results $Results `
    -CaseTimings $CaseTimings `
    -LastCompletedFixture $LastCompletedFixture
}

function Invoke-NegativeExecutionSmokeFixture {
  param(
    [Parameter(Mandatory = $true)][object]$Fixture,
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][int]$FixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$Results,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$CaseTimings,
    [Parameter(Mandatory = $true)][ref]$LastCompletedFixture
  )

  Invoke-NegativeExecutionSmokeFixtureImpl `
    -Fixture $Fixture `
    -Context $Context `
    -FixtureIndex $FixtureIndex `
    -TotalSelectedFixtures $TotalSelectedFixtures `
    -SuiteStopwatch $SuiteStopwatch `
    -Results $Results `
    -CaseTimings $CaseTimings `
    -LastCompletedFixture $LastCompletedFixture
}

Export-ModuleMember -Function @(
  "Invoke-NegativeExecutionSmokeFixture",
  "Invoke-PositiveExecutionSmokeFixture"
)
