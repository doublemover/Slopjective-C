$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$toolchainScriptRoot = Split-Path -Parent $PSScriptRoot
$compileIoModule = Join-Path $toolchainScriptRoot "objc3c_native_compile_io.psm1"
$artifactsModule = Join-Path $PSScriptRoot "artifacts.psm1"
foreach ($dependencyModule in @($compileIoModule, $artifactsModule)) {
  if (!(Test-Path -LiteralPath $dependencyModule -PathType Leaf)) {
    Write-Error "native compile toolchain dependency missing at $dependencyModule"
    exit 2
  }
  Import-Module $dependencyModule -Force -DisableNameChecking
}

$readinessModuleRoot = Join-Path $PSScriptRoot "readiness"
$readinessModules = @(
  "build_inputs.psm1",
  "freshness.psm1",
  "frontend_lock.psm1",
  "artifacts_ready.psm1"
)

foreach ($readinessModule in $readinessModules) {
  $readinessModulePath = Join-Path $readinessModuleRoot $readinessModule
  if (!(Test-Path -LiteralPath $readinessModulePath -PathType Leaf)) {
    Write-Error "native compile toolchain readiness helper missing at $readinessModulePath"
    exit 2
  }
  . $readinessModulePath
}

Export-ModuleMember -Function @(
  "Get-NativeCompilerBuildInputPaths",
  "Test-AnyPathNewerThanTarget",
  "Test-FrontendInvocationLockCurrent",
  "Test-NativeCompilerBuildArtifactsReady"
)
