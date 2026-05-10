Set-StrictMode -Version Latest

function Get-Objc3cNativePerfCacheProofInvocationModuleNames {
  return @(
    "fixture_copy.psm1",
    "cache_run.psm1",
    "cache_proof.psm1",
    "cache_invalidation_proof.psm1",
    "macro_host_proof.psm1"
  )
}

function Get-Objc3cNativePerfCacheProofInvocationExportedFunctionNames {
  return @(
    "Invoke-Objc3cNativePerfWrapperCacheInvalidationProof",
    "Invoke-Objc3cNativePerfWrapperCacheProof",
    "Invoke-Objc3cNativePerfWrapperMacroHostProof"
  )
}
