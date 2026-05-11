$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Import-Objc3cNativeCompileFrontendGuardDependencies {
  param(
    [Parameter(Mandatory = $true)]
    [string]$ScriptRoot
  )

  $guardModules = @(
    "objc3c_native_compile_frontend_artifact_guards.psm1",
    "objc3c_native_compile_frontend_feature_guards.psm1",
    "objc3c_native_compile_frontend_hardening_guards.psm1",
    "objc3c_native_compile_frontend_conformance_guards.psm1"
  )

  foreach ($guardModule in $guardModules) {
    $guardModulePath = Join-Path $ScriptRoot $guardModule
    if (!(Test-Path -LiteralPath $guardModulePath -PathType Leaf)) {
      Write-Error "native compile frontend guard module missing at $guardModulePath"
      exit 2
    }
    Import-Module $guardModulePath -Force -DisableNameChecking
  }
}
