Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_expectations.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_cases.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_runner.psm1") -Force -DisableNameChecking

Export-ModuleMember -Function @(
  "Invoke-Objc3cLoweringRegressionSuite"
)
