Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "budget_result_parsing.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "command_construction.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "process_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

$cacheProofInvocationModuleRoot = Join-Path $PSScriptRoot "cache_proof_invocation"
. (Join-Path $cacheProofInvocationModuleRoot "exports.psm1")

foreach ($cacheProofInvocationModule in Get-Objc3cNativePerfCacheProofInvocationModuleNames) {
  . (Join-Path $cacheProofInvocationModuleRoot $cacheProofInvocationModule)
}

Export-ModuleMember -Function (Get-Objc3cNativePerfCacheProofInvocationExportedFunctionNames)
