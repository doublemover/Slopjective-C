$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_execution.psm1") -Force -DisableNameChecking

Invoke-Objc3cDiagnosticsRegressionSuite -ScriptRoot $PSScriptRoot
