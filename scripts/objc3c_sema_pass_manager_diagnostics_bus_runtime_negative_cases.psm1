$negativeCasesRoot = Join-Path $PSScriptRoot "objc3c_sema_pass_manager_diagnostics_bus_runtime_negative_cases"

. (Join-Path $negativeCasesRoot "helpers.psm1")
. (Join-Path $negativeCasesRoot "backend_matrix.psm1")
. (Join-Path $negativeCasesRoot "clang_cases.psm1")

if ($MyInvocation.InvocationName -ne ".") {
  Export-ModuleMember -Function @(
    "Invoke-SemaPassManagerNegativeBackendMatrixRuntimeCase",
    "Invoke-SemaPassManagerNegativeClangRuntimeCases"
  )
}
