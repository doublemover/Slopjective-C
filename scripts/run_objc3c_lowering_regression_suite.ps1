$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_lowering_regression_suite_helpers.psm1") -Force -DisableNameChecking

Invoke-Objc3cLoweringRegressionSuite -ScriptRoot $PSScriptRoot
