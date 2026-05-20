function Get-Objc3cNativeFrontendArtifactPaths {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$ArtifactRoot = ""
  )

  $root = if ($ArtifactRoot) { $ArtifactRoot } else { Join-Path $RepoRoot "tmp/artifacts/objc3c-native" }

  return [pscustomobject]@{
    SourceGraph = Join-Path $root "frontend_source_graph.json"
    InvocationLock = Join-Path $root "frontend_invocation_lock.json"
    CoreFeatureExpansion = Join-Path $root "frontend_core_feature_expansion.json"
    EdgeCompatibility = Join-Path $root "frontend_edge_compat.json"
    EdgeRobustness = Join-Path $root "frontend_edge_robustness.json"
    DiagnosticsHardening = Join-Path $root "frontend_diagnostics_hardening.json"
    RecoveryDeterminismHardening = Join-Path $root "frontend_recovery_determinism_hardening.json"
    ConformanceMatrix = Join-Path $root "frontend_conformance_matrix.json"
    ConformanceCorpus = Join-Path $root "frontend_conformance_corpus.json"
    IntegrationCloseout = Join-Path $root "frontend_integration_closeout.json"
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeFrontendArtifactPaths"
)
