$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$contractModulePath = Join-Path $PSScriptRoot "objc3c_sema_pass_manager_diagnostics_bus_contract/contract.psm1"
Import-Module $contractModulePath -Force -DisableNameChecking

$summary = Invoke-SemaPassManagerDiagnosticsBusContract `
  -ScriptRoot $PSScriptRoot `
  -ConfiguredRunId $env:OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID `
  -ConfiguredNativeExe $env:OBJC3C_NATIVE_EXECUTABLE

if (-not [string]::IsNullOrEmpty($summary.fatal_error)) {
  Write-Output ("error: {0}" -f $summary.fatal_error)
}

Write-Output ("summary_path: {0}" -f $summary.summary_path)
Write-Output ("status: {0}" -f $summary.status)

if ($summary.status -ne "PASS") {
  exit 1
}
