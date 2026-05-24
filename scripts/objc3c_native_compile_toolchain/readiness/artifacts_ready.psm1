$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-NativeCompilerRequiredFrontendArtifactPaths {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot,
    [object]$ExistingBuildResult
  )

  return @(
    (Resolve-FrontendScaffoldPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendInvocationLockPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendEdgeCompatibilityPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendEdgeRobustnessPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendConformanceMatrixPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendConformanceCorpusPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendIntegrationCloseoutPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
  )
}

function Test-NativeCompilerBuildArtifactsReady {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot,
    [object]$ExistingBuildResult
  )

  $exe = Resolve-NativeCompilerExecutablePath -RepoRoot $CompilerRepoRoot
  $runtimeLibrary = Resolve-NativeCompilerRuntimeLibraryPath -RepoRoot $CompilerRepoRoot
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    return $false
  }
  if (!(Test-Path -LiteralPath $runtimeLibrary -PathType Leaf)) {
    return $false
  }

  foreach ($artifactPath in Get-NativeCompilerRequiredFrontendArtifactPaths -CompilerRepoRoot $CompilerRepoRoot -ExistingBuildResult $ExistingBuildResult) {
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      return $false
    }
  }
  if (-not (Test-FrontendInvocationLockCurrent -CompilerRepoRoot $CompilerRepoRoot -ExistingBuildResult $ExistingBuildResult)) {
    return $false
  }

  $buildInputPaths = Get-NativeCompilerBuildInputPaths -RepoRoot $CompilerRepoRoot
  if (Test-AnyPathNewerThanTarget -TargetPath $exe -InputPaths $buildInputPaths) {
    return $false
  }
  if (Test-AnyPathNewerThanTarget -TargetPath $runtimeLibrary -InputPaths $buildInputPaths) {
    return $false
  }

  return $true
}
