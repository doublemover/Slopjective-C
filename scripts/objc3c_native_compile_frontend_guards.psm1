$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$frontendGuardRoot = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_guards"
$frontendGuardModules = @(
  "dependencies.psm1",
  "orchestration.psm1"
)

foreach ($frontendGuardModule in $frontendGuardModules) {
  $frontendGuardModulePath = Join-Path $frontendGuardRoot $frontendGuardModule
  if (!(Test-Path -LiteralPath $frontendGuardModulePath -PathType Leaf)) {
    Write-Error "native compile frontend guard helper missing at $frontendGuardModulePath"
    exit 2
  }
  . $frontendGuardModulePath
}

Import-Objc3cNativeCompileFrontendGuardDependencies -ScriptRoot $PSScriptRoot

Export-ModuleMember -Function "Invoke-Objc3cNativeCompileFrontendGuards"
