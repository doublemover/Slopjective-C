function Get-Objc3cNativeFrontendArtifactPaths {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  return [pscustomobject]@{
    SourceGraph = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_source_graph.json"
    InvocationLock = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
    CoreFeatureExpansion = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
    EdgeCompatibility = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
    EdgeRobustness = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
    DiagnosticsHardening = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"
    RecoveryDeterminismHardening = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"
    ConformanceMatrix = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"
    ConformanceCorpus = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json"
    IntegrationCloseout = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
  }
}
