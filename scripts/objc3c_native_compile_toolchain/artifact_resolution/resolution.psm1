$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Resolve-FrontendArtifactPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [Parameter(Mandatory = $true)][string]$ArtifactKey
  )

  $definition = Get-FrontendArtifactDefinition -ArtifactKey $ArtifactKey
  $relativePath = [string]$definition.default_relative_path
  $candidatePath = Get-BuildResultArtifactRelativePath `
    -BuildResult $BuildResult `
    -PropertyName ([string]$definition.build_result_property)
  if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
    $relativePath = $candidatePath
  }

  return Resolve-RepoBoundPath `
    -RepoRoot $RepoRoot `
    -RelativeOrAbsolutePath $relativePath `
    -Label ([string]$definition.label)
}

function Resolve-FrontendScaffoldPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_source_graph"
}

function Resolve-FrontendInvocationLockPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_invocation_lock"
}

function Resolve-FrontendCoreFeatureExpansionPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_core_feature_expansion"
}

function Resolve-FrontendEdgeCompatibilityPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_edge_compat"
}

function Resolve-FrontendEdgeRobustnessPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_edge_robustness"
}

function Resolve-FrontendDiagnosticsHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_diagnostics_hardening"
}

function Resolve-FrontendRecoveryDeterminismHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_recovery_determinism_hardening"
}

function Resolve-FrontendConformanceMatrixPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_conformance_matrix"
}

function Resolve-FrontendConformanceCorpusPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_conformance_corpus"
}

function Resolve-FrontendIntegrationCloseoutPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  return Resolve-FrontendArtifactPath -RepoRoot $RepoRoot -BuildResult $BuildResult -ArtifactKey "frontend_integration_closeout"
}
