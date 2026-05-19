Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_cases/layout.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_cases/catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_cases/expected_artifact_checks.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_cases/diagnostics_assertions.psm1") -Force -DisableNameChecking

function Invoke-LoweringCase {
  param(
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][ValidateSet("positive", "negative")][string]$FixtureKind,
    [Parameter(Mandatory = $true)][pscustomobject]$SuiteContext
  )

  $case = New-LoweringCaseCatalogEntry -Fixture $Fixture -FixtureKind $FixtureKind -SuiteContext $SuiteContext
  $layout = $case.Layout

  Initialize-LoweringCaseLayout -Layout $layout

  $nativeRuns = [pscustomobject]@{
    Run1Exit = Invoke-LoggedCommand `
      -Command $layout.NativeCompilerExe `
      -Arguments @($Fixture.FullName, "--out-dir", $layout.Run1Dir, "--emit-prefix", "module") `
      -LogPath $layout.Run1Log
    Run2Exit = Invoke-LoggedCommand `
      -Command $layout.NativeCompilerExe `
      -Arguments @($Fixture.FullName, "--out-dir", $layout.Run2Dir, "--emit-prefix", "module") `
      -LogPath $layout.Run2Log
  }

  $artifacts = Get-LoweringCaseArtifacts -Layout $layout
  $expectations = Get-LoweringIrExpectations -Fixture $Fixture -RepoRoot $layout.RepoRoot
  $objc3IrCheck = Test-Objc3IrArtifactExpectations `
    -Layout $layout `
    -Artifacts $artifacts `
    -Expectation $expectations.Objc3
  $dispatchIrCheck = Invoke-DispatchIrArtifactExpectations `
    -Fixture $Fixture `
    -Layout $layout `
    -Expectation $expectations.Dispatch

  $diagnostics = New-LoweringDiagnosticAssertions -NativeRuns $nativeRuns -Artifacts $artifacts
  $checks = New-LoweringCheckCatalog `
    -Diagnostics $diagnostics `
    -Expectations $expectations `
    -DispatchIrCheck $dispatchIrCheck `
    -Objc3IrCheck $objc3IrCheck
  $failedChecks = New-LoweringFailedCheckList

  Add-LoweringExpectedArtifactFailures `
    -FixtureKind $FixtureKind `
    -NativeRuns $nativeRuns `
    -Layout $layout `
    -Artifacts $artifacts `
    -Checks $checks `
    -FailedChecks $failedChecks `
    -DispatchExpectation $expectations.Dispatch `
    -DispatchIrCheck $dispatchIrCheck `
    -Objc3Expectation $expectations.Objc3 `
    -Objc3IrCheck $objc3IrCheck

  Add-LoweringExpectationValidityFailures `
    -FailedChecks $failedChecks `
    -DispatchExpectation $expectations.Dispatch `
    -Objc3Expectation $expectations.Objc3
  Add-LoweringDiagnosticFailures `
    -FailedChecks $failedChecks `
    -Diagnostics $diagnostics

  return New-LoweringCaseResult `
    -Case $case `
    -NativeRuns $nativeRuns `
    -Artifacts $artifacts `
    -Expectations $expectations `
    -DispatchIrCheck $dispatchIrCheck `
    -Objc3IrCheck $objc3IrCheck `
    -Checks $checks `
    -FailedChecks $failedChecks
}

Export-ModuleMember -Function @(
  "Invoke-LoweringCase"
)
