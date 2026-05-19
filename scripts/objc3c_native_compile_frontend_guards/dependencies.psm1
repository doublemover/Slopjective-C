$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-Objc3cNativeCompileFrontendGuardDependencyModules {
  return @(
    "objc3c_native_compile_frontend_artifact_guards.psm1",
    "objc3c_native_compile_frontend_feature_guards.psm1",
    "objc3c_native_compile_frontend_hardening_guards.psm1",
    "objc3c_native_compile_frontend_conformance_guards.psm1"
  )
}

Export-ModuleMember -Function "Get-Objc3cNativeCompileFrontendGuardDependencyModules"
