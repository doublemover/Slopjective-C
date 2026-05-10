$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$toolchainScriptRoot = Split-Path -Parent $PSScriptRoot
$compileIoModule = Join-Path $toolchainScriptRoot "objc3c_native_compile_io.psm1"
if (!(Test-Path -LiteralPath $compileIoModule -PathType Leaf)) {
  Write-Error "native compile IO helper missing at $compileIoModule"
  exit 2
}
Import-Module $compileIoModule -Force -DisableNameChecking

function Get-FrontendArtifactDefinitions {
  return @(
    [pscustomobject]@{
      key = "frontend_source_graph"
      build_result_property = "frontend_source_graph_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_source_graph.json"
      label = "frontend source graph"
    }
    [pscustomobject]@{
      key = "frontend_invocation_lock"
      build_result_property = "frontend_invocation_lock_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
      label = "frontend invocation lock"
    }
    [pscustomobject]@{
      key = "frontend_core_feature_expansion"
      build_result_property = "frontend_core_feature_expansion_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
      label = "frontend core feature expansion"
    }
    [pscustomobject]@{
      key = "frontend_edge_compat"
      build_result_property = "frontend_edge_compat_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
      label = "frontend edge compatibility"
    }
    [pscustomobject]@{
      key = "frontend_edge_robustness"
      build_result_property = "frontend_edge_robustness_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
      label = "frontend edge robustness"
    }
    [pscustomobject]@{
      key = "frontend_diagnostics_hardening"
      build_result_property = "frontend_diagnostics_hardening_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"
      label = "frontend diagnostics hardening"
    }
    [pscustomobject]@{
      key = "frontend_recovery_determinism_hardening"
      build_result_property = "frontend_recovery_determinism_hardening_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"
      label = "frontend recovery determinism hardening"
    }
    [pscustomobject]@{
      key = "frontend_conformance_matrix"
      build_result_property = "frontend_conformance_matrix_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"
      label = "frontend conformance matrix"
    }
    [pscustomobject]@{
      key = "frontend_conformance_corpus"
      build_result_property = "frontend_conformance_corpus_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json"
      label = "frontend conformance corpus"
    }
    [pscustomobject]@{
      key = "frontend_integration_closeout"
      build_result_property = "frontend_integration_closeout_relative_path"
      default_relative_path = "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
      label = "frontend integration closeout"
    }
  )
}

function Get-FrontendArtifactDefinition {
  param([Parameter(Mandatory = $true)][string]$ArtifactKey)

  foreach ($definition in Get-FrontendArtifactDefinitions) {
    if ([string]$definition.key -eq $ArtifactKey) {
      return $definition
    }
  }

  Write-Error "unknown frontend artifact key: $ArtifactKey"
  exit 2
}

function Get-BuildResultArtifactRelativePath {
  param(
    [object]$BuildResult,
    [string]$PropertyName
  )

  if ($null -eq $BuildResult) {
    return $null
  }

  $property = $BuildResult.PSObject.Properties[$PropertyName]
  if ($null -eq $property) {
    return $null
  }

  return [string]$property.Value
}

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

function Resolve-NativeCompilerExecutablePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $configuredNativeExe = [string]$env:OBJC3C_NATIVE_EXECUTABLE
  if (-not [string]::IsNullOrWhiteSpace($configuredNativeExe)) {
    return [System.IO.Path]::GetFullPath($configuredNativeExe)
  }
  return (Join-Path $RepoRoot "artifacts/bin/objc3c-native.exe")
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
  "Resolve-NativeCompilerExecutablePath"
)
