$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-FrontendCoreFeatureExpansionEvaluationResult {
  param(
    [string]$ArtifactPath,
    [hashtable]$AllowedBackends
  )

  [pscustomobject]@{
    feature_path = $ArtifactPath
    allowed_ir_object_backends = @($AllowedBackends.Keys)
  }
}

function New-FrontendEdgeCompatibilityEvaluationResult {
  param(
    [string]$ArtifactPath,
    [object]$NormalizedCompileArgs
  )

  [pscustomobject]@{
    edge_compat_path = $ArtifactPath
    normalized_compile_args = $NormalizedCompileArgs
  }
}

Export-ModuleMember -Function @(
  "New-FrontendCoreFeatureExpansionEvaluationResult",
  "New-FrontendEdgeCompatibilityEvaluationResult"
)
