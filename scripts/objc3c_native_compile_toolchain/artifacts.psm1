$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$toolchainScriptRoot = Split-Path -Parent $PSScriptRoot
$compileIoModule = Join-Path $toolchainScriptRoot "objc3c_native_compile_io.psm1"
if (!(Test-Path -LiteralPath $compileIoModule -PathType Leaf)) {
  Write-Error "native compile IO helper missing at $compileIoModule"
  exit 2
}
$compileIoRootLiteral = (Split-Path -Parent $compileIoModule).Replace("'", "''")
$compileIoScript = [scriptblock]::Create(
  "`$PSScriptRoot = '$compileIoRootLiteral'`n" +
  (Get-Content -LiteralPath $compileIoModule -Raw)
)
. $compileIoScript

$artifactModuleRoot = Join-Path $PSScriptRoot "artifact_resolution"
$artifactModules = @(
  "definitions.psm1",
  "build_results.psm1",
  "resolution.psm1",
  "native_executable.psm1"
)

foreach ($artifactModule in $artifactModules) {
  $artifactModulePath = Join-Path $artifactModuleRoot $artifactModule
  if (!(Test-Path -LiteralPath $artifactModulePath -PathType Leaf)) {
    Write-Error "native compile toolchain artifact helper missing at $artifactModulePath"
    exit 2
  }
  $artifactModuleRootLiteral = (Split-Path -Parent $artifactModulePath).Replace("'", "''")
  $artifactScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$artifactModuleRootLiteral'`n" +
    (Get-Content -LiteralPath $artifactModulePath -Raw)
  )
  . $artifactScript
}

Export-ModuleMember -Function @(
  "Resolve-FrontendArtifactPath",
  "Resolve-FrontendConformanceCorpusPath",
  "Resolve-FrontendConformanceMatrixPath",
  "Resolve-FrontendCoreFeatureExpansionPath",
  "Resolve-FrontendDiagnosticsHardeningPath",
  "Resolve-FrontendEdgeCompatibilityPath",
  "Resolve-FrontendEdgeRobustnessPath",
  "Resolve-FrontendIntegrationCloseoutPath",
  "Resolve-FrontendInvocationLockPath",
  "Resolve-FrontendRecoveryDeterminismHardeningPath",
  "Resolve-FrontendScaffoldPath",
  "Resolve-NativeCompilerExecutablePath",
  "Resolve-NativeCompilerRuntimeLibraryPath"
)
