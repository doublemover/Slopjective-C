$positiveCasesRoot = Join-Path $PSScriptRoot "objc3c_sema_pass_manager_diagnostics_bus_runtime_positive_cases"

. (Join-Path $positiveCasesRoot "catalog.psm1")
. (Join-Path $positiveCasesRoot "path_report.psm1")
. (Join-Path $positiveCasesRoot "invocation.psm1")
. (Join-Path $positiveCasesRoot "assertions.psm1")
. (Join-Path $positiveCasesRoot "orchestration.psm1")

if ($MyInvocation.InvocationName -ne ".") {
  Export-ModuleMember -Function @(
    "Get-SemaPassManagerPositiveRuntimeArtifacts",
    "Invoke-SemaPassManagerPositiveClangDefaultRuntimeCase",
    "Invoke-SemaPassManagerPositiveLlvmDirectDefaultRuntimeCase",
    "Invoke-SemaPassManagerPositiveLlvmDirectForcedMissingLlcRuntimeCase"
  )
}
