function Get-Objc3cNativeFrontendPacketDefinitions {
  param(
    [Parameter(Mandatory = $true)]$ArtifactPaths
  )

  return @(
    [pscustomobject]@{
      Name = "frontend_source_graph"
      Family = "source-derived"
      OutputPath = $ArtifactPaths.SourceGraph
      Dependencies = @()
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_invocation_lock"
      Family = "binary-derived"
      OutputPath = $ArtifactPaths.InvocationLock
      Dependencies = @("frontend_source_graph")
      RequiresNativeBinaries = $true
    }
    [pscustomobject]@{
      Name = "frontend_core_feature_expansion"
      Family = "binary-derived"
      OutputPath = $ArtifactPaths.CoreFeatureExpansion
      Dependencies = @("frontend_source_graph", "frontend_invocation_lock")
      RequiresNativeBinaries = $true
    }
    [pscustomobject]@{
      Name = "frontend_edge_compat"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.EdgeCompatibility
      Dependencies = @("frontend_core_feature_expansion")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_edge_robustness"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.EdgeRobustness
      Dependencies = @("frontend_edge_compat")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_diagnostics_hardening"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.DiagnosticsHardening
      Dependencies = @("frontend_edge_robustness")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_recovery_determinism_hardening"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.RecoveryDeterminismHardening
      Dependencies = @("frontend_diagnostics_hardening")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_conformance_matrix"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.ConformanceMatrix
      Dependencies = @("frontend_recovery_determinism_hardening")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_conformance_corpus"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.ConformanceCorpus
      Dependencies = @("frontend_conformance_matrix")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_integration_closeout"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.IntegrationCloseout
      Dependencies = @("frontend_conformance_corpus")
      RequiresNativeBinaries = $false
    }
  )
}
