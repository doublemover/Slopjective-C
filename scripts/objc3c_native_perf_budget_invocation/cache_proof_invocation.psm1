Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "budget_result_parsing.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "command_construction.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "process_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

$cacheProofInvocationModuleRoot = Join-Path $PSScriptRoot "cache_proof_invocation"
$cacheProofInvocationModules = @(
  "fixture_copy.psm1",
  "cache_run.psm1",
  "cache_proof.psm1",
  "cache_invalidation_proof.psm1",
  "macro_host_proof.psm1"
)
$cacheProofInvocationExports = @(
  "Invoke-Objc3cNativePerfWrapperCacheInvalidationProof",
  "Invoke-Objc3cNativePerfWrapperCacheProof",
  "Invoke-Objc3cNativePerfWrapperMacroHostProof"
)

foreach ($cacheProofInvocationModule in $cacheProofInvocationModules) {
  $cacheProofInvocationModulePath = Join-Path $cacheProofInvocationModuleRoot $cacheProofInvocationModule
  # Dot-sourcing a .psm1 path can keep the functions in that module's scope. Load
  # the checked-in leaf module text into this aggregate module scope instead.
  . ([scriptblock]::Create((Get-Content -Raw -LiteralPath $cacheProofInvocationModulePath)))
}

Export-ModuleMember -Function $cacheProofInvocationExports
