param(
  [Nullable[int]]$MaxElapsedMs,
  [string]$ExtraPositiveFixtureDirs,
  [switch]$EnforceTimingGate
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_runner.psm1") -Force -DisableNameChecking

$exitCode = 0
Invoke-Objc3cNativePerfBudget `
  -ScriptRoot $PSScriptRoot `
  -MaxElapsedMs $MaxElapsedMs `
  -ExtraPositiveFixtureDirs $ExtraPositiveFixtureDirs `
  -EnforceTimingGate:$EnforceTimingGate.IsPresent `
  -ExitCode ([ref]$exitCode)

if ($exitCode -ne 0) {
  exit $exitCode
}
