$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
