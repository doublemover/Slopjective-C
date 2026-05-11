$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendEdgeCompatibilityPayloadImpl {
  param(
    [object]$Payload,
    [string]$ArtifactPath,
    [object]$CoreFeatureGuard
  )

  $config = Get-FrontendEdgeCompatibilityGuardConfig
  Assert-FrontendEdgeCompatibilityPayloadContract `
    -Payload $Payload `
    -Config $config `
    -ArtifactPath $ArtifactPath

  Assert-FrontendEdgeInvocationMetadata `
    -EdgeCompat (Read-FrontendEdgeCompatibilityInvocationMetadata -Payload $Payload) `
    -Config $config `
    -ArtifactPath $ArtifactPath

  return New-FrontendEdgeCompatibilityBackendPayloadEvaluation `
    -BackendCompat (Read-FrontendEdgeCompatibilityBackendMetadata -Payload $Payload) `
    -Config $config `
    -CoreFeatureGuard $CoreFeatureGuard `
    -ArtifactPath $ArtifactPath
}

function Assert-FrontendEdgeCompatibilityPayloadContract {
  param(
    [object]$Payload,
    [object]$Config,
    [string]$ArtifactPath
  )

  if ([string]$Payload.contract_id -ne $Config.contract_id) {
    Stop-FrontendFeatureGuard "frontend edge compatibility contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendFeatureDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($Config.dependency_contract_ids) `
    -ArtifactName $Config.artifact_name `
    -ArtifactPath $ArtifactPath
}
