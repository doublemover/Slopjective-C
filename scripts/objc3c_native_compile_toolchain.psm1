$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$toolchainModuleRoot = Join-Path $PSScriptRoot "objc3c_native_compile_toolchain"
$toolchainHelperModules = @(
  "artifacts.psm1",
  "commands.psm1",
  "results.psm1",
  "processes.psm1",
  "readiness.psm1",
  "availability.psm1"
)

foreach ($helperModuleName in $toolchainHelperModules) {
  $helperModulePath = Join-Path $toolchainModuleRoot $helperModuleName
  if (!(Test-Path -LiteralPath $helperModulePath -PathType Leaf)) {
    Write-Error "native compile toolchain helper missing at $helperModulePath"
    exit 2
  }
  Import-Module $helperModulePath -Force -DisableNameChecking -Prefix NativeToolchain
}

function Ensure-NativeCompilerAvailable {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult
  )

  return Ensure-NativeToolchainNativeCompilerAvailable @PSBoundParameters
}

function Invoke-NativeCompiler {
  param(
    [string]$ExePath,
    [string[]]$Arguments
  )

  return Invoke-NativeToolchainNativeCompiler @PSBoundParameters
}

function Resolve-FrontendConformanceCorpusPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendConformanceCorpusPath @PSBoundParameters
}

function Resolve-FrontendConformanceMatrixPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendConformanceMatrixPath @PSBoundParameters
}

function Resolve-FrontendCoreFeatureExpansionPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendCoreFeatureExpansionPath @PSBoundParameters
}

function Resolve-FrontendDiagnosticsHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendDiagnosticsHardeningPath @PSBoundParameters
}

function Resolve-FrontendEdgeCompatibilityPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendEdgeCompatibilityPath @PSBoundParameters
}

function Resolve-FrontendEdgeRobustnessPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendEdgeRobustnessPath @PSBoundParameters
}

function Resolve-FrontendIntegrationCloseoutPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendIntegrationCloseoutPath @PSBoundParameters
}

function Resolve-FrontendInvocationLockPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendInvocationLockPath @PSBoundParameters
}

function Resolve-FrontendRecoveryDeterminismHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendRecoveryDeterminismHardeningPath @PSBoundParameters
}

function Resolve-FrontendScaffoldPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-NativeToolchainFrontendScaffoldPath @PSBoundParameters
}

function Resolve-NativeCompilerExecutablePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  return Resolve-NativeToolchainNativeCompilerExecutablePath @PSBoundParameters
}

Export-ModuleMember -Function @(
  "Ensure-NativeCompilerAvailable",
  "Invoke-NativeCompiler",
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
  "Resolve-NativeCompilerExecutablePath"
)
