$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileIoModule = Join-Path $PSScriptRoot "objc3c_native_compile_io.psm1"
$compileToolchainModule = Join-Path $PSScriptRoot "objc3c_native_compile_toolchain.psm1"
foreach ($dependencyModule in @($compileIoModule, $compileToolchainModule)) {
  if (!(Test-Path -LiteralPath $dependencyModule -PathType Leaf)) {
    Write-Error "native compile frontend artifact guard dependency missing at $dependencyModule"
    exit 2
  }
  Import-Module $dependencyModule -Force -DisableNameChecking
}

$artifactGuardRoot = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_artifact_guards"
$artifactGuardModules = @(
  "json_loading.psm1",
  "scaffold.psm1",
  "invocation_lock.psm1"
)

foreach ($artifactGuardModule in $artifactGuardModules) {
  $artifactGuardModulePath = Join-Path $artifactGuardRoot $artifactGuardModule
  if (!(Test-Path -LiteralPath $artifactGuardModulePath -PathType Leaf)) {
    Write-Error "native compile frontend artifact guard helper missing at $artifactGuardModulePath"
    exit 2
  }
  . $artifactGuardModulePath
}

Export-ModuleMember -Function @("Assert-FrontendModuleScaffold", "Assert-FrontendInvocationLock")
