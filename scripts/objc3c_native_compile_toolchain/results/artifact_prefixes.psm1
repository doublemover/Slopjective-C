$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-NativeCompilerBuildArtifactRelativePathMap {
  return [ordered]@{
    frontend_source_graph_relative_path = $null
    frontend_invocation_lock_relative_path = $null
    frontend_core_feature_expansion_relative_path = $null
    frontend_edge_compat_relative_path = $null
    frontend_edge_robustness_relative_path = $null
    frontend_diagnostics_hardening_relative_path = $null
    frontend_recovery_determinism_hardening_relative_path = $null
    frontend_conformance_matrix_relative_path = $null
    frontend_conformance_corpus_relative_path = $null
    frontend_integration_closeout_relative_path = $null
  }
}

function Get-NativeCompilerBuildOutputPrefixes {
  return [ordered]@{
    "frontend_source_graph=" = "frontend_source_graph_relative_path"
    "frontend_invocation_lock=" = "frontend_invocation_lock_relative_path"
    "frontend_core_feature_expansion=" = "frontend_core_feature_expansion_relative_path"
    "frontend_edge_compat=" = "frontend_edge_compat_relative_path"
    "frontend_edge_robustness=" = "frontend_edge_robustness_relative_path"
    "frontend_diagnostics_hardening=" = "frontend_diagnostics_hardening_relative_path"
    "frontend_recovery_determinism_hardening=" = "frontend_recovery_determinism_hardening_relative_path"
    "frontend_conformance_matrix=" = "frontend_conformance_matrix_relative_path"
    "frontend_conformance_corpus=" = "frontend_conformance_corpus_relative_path"
    "frontend_integration_closeout=" = "frontend_integration_closeout_relative_path"
  }
}
