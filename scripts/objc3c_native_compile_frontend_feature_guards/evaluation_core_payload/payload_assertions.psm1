$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendCoreFeaturePayloadImpl {
  param(
    [object]$Payload,
    [string]$ArtifactPath
  )

  $config = Get-FrontendCoreFeatureGuardConfig
  Assert-FrontendCoreFeaturePayloadContract `
    -Payload $Payload `
    -Config $config `
    -ArtifactPath $ArtifactPath
  Assert-FrontendCoreFeaturePayloadRequiredModules `
    -Payload $Payload `
    -Config $config `
    -ArtifactPath $ArtifactPath

  Assert-FrontendCoreInvocationMetadata `
    -Invocation (Read-FrontendCoreFeatureInvocationMetadata -Payload $Payload) `
    -Config $config `
    -ArtifactPath $ArtifactPath
  $backendRouting = Read-FrontendCoreFeatureBackendRoutingMetadata -Payload $Payload
  Assert-FrontendCoreBackendRoutingMetadata `
    -BackendRouting $backendRouting `
    -Config $config `
    -ArtifactPath $ArtifactPath

  return Assert-FrontendCoreFeaturePayloadAllowedBackends `
    -BackendRouting $backendRouting `
    -Config $config `
    -ArtifactPath $ArtifactPath
}

function Assert-FrontendCoreFeaturePayloadContract {
  param(
    [object]$Payload,
    [object]$Config,
    [string]$ArtifactPath
  )

  if ([string]$Payload.contract_id -ne $Config.contract_id) {
    Stop-FrontendFeatureGuard "frontend core feature expansion contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendFeatureDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($Config.dependency_contract_ids) `
    -ArtifactName $Config.artifact_name `
    -ArtifactPath $ArtifactPath
}

function Assert-FrontendCoreFeaturePayloadRequiredModules {
  param(
    [object]$Payload,
    [object]$Config,
    [string]$ArtifactPath
  )

  $presentModules = New-FrontendFeatureStringSet -Values @($Payload.module_names)
  foreach ($requiredModule in @($Config.required_modules)) {
    if (-not $presentModules.ContainsKey($requiredModule)) {
      Stop-FrontendFeatureGuard "frontend core feature expansion missing required module '$requiredModule' in $ArtifactPath"
    }
  }
}

function Assert-FrontendCoreFeaturePayloadAllowedBackends {
  param(
    [object]$BackendRouting,
    [object]$Config,
    [string]$ArtifactPath
  )

  $allowedBackends = New-FrontendFeatureBackendSet -Values @($BackendRouting.allowed_ir_object_backends)
  foreach ($requiredBackend in @($Config.allowed_ir_object_backends)) {
    if (-not $allowedBackends.ContainsKey($requiredBackend)) {
      Stop-FrontendFeatureGuard "frontend core feature expansion missing backend '$requiredBackend' in $ArtifactPath"
    }
  }

  return $allowedBackends
}
