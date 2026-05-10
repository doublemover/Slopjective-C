$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$featureGuardArtifactsModule = Join-Path $PSScriptRoot "evaluation_artifacts.psm1"
$featureGuardDiagnosticsModule = Join-Path $PSScriptRoot "evaluation_diagnostics.psm1"
foreach ($modulePath in @(
    $featureGuardConfigModule,
    $featureGuardNormalizationModule,
    $featureGuardReportingModule,
    $featureGuardArtifactsModule,
    $featureGuardDiagnosticsModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard core payload dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Assert-FrontendCoreFeaturePayloadImpl {
  param(
    [object]$Payload,
    [string]$ArtifactPath
  )

  $config = Get-FrontendCoreFeatureGuardConfig
  if ([string]$Payload.contract_id -ne $config.contract_id) {
    Stop-FrontendFeatureGuard "frontend core feature expansion contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendFeatureDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($config.dependency_contract_ids) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  $presentModules = New-FrontendFeatureStringSet -Values @($Payload.module_names)
  foreach ($requiredModule in @($config.required_modules)) {
    if (-not $presentModules.ContainsKey($requiredModule)) {
      Stop-FrontendFeatureGuard "frontend core feature expansion missing required module '$requiredModule' in $ArtifactPath"
    }
  }

  Assert-FrontendCoreInvocationMetadata `
    -Invocation (Read-FrontendCoreFeatureInvocationMetadata -Payload $Payload) `
    -Config $config `
    -ArtifactPath $ArtifactPath
  $backendRouting = Read-FrontendCoreFeatureBackendRoutingMetadata -Payload $Payload
  Assert-FrontendCoreBackendRoutingMetadata `
    -BackendRouting $backendRouting `
    -Config $config `
    -ArtifactPath $ArtifactPath

  $allowedBackends = New-FrontendFeatureBackendSet -Values @($backendRouting.allowed_ir_object_backends)
  foreach ($requiredBackend in @($config.allowed_ir_object_backends)) {
    if (-not $allowedBackends.ContainsKey($requiredBackend)) {
      Stop-FrontendFeatureGuard "frontend core feature expansion missing backend '$requiredBackend' in $ArtifactPath"
    }
  }

  return $allowedBackends
}

function Assert-FrontendCoreInvocationMetadata {
  param(
    [object]$Invocation,
    [object]$Config,
    [string]$ArtifactPath
  )

  if ($null -eq $Invocation) {
    Stop-FrontendFeatureGuard "frontend core feature expansion invocation metadata missing in $ArtifactPath"
  }
  if ([string]$Invocation.default_out_dir -ne $Config.default_out_dir) {
    Stop-FrontendFeatureGuard "frontend core feature expansion default_out_dir mismatch in $ArtifactPath"
  }
  if ([string]$Invocation.cache_root -ne $Config.cache_root) {
    Stop-FrontendFeatureGuard "frontend core feature expansion cache_root mismatch in $ArtifactPath"
  }
  if (-not [bool]$Invocation.supports_cache) {
    Stop-FrontendFeatureGuard "frontend core feature expansion supports_cache must be true in $ArtifactPath"
  }
}

function Assert-FrontendCoreBackendRoutingMetadata {
  param(
    [object]$BackendRouting,
    [object]$Config,
    [string]$ArtifactPath
  )

  if ($null -eq $BackendRouting) {
    Stop-FrontendFeatureGuard "frontend core feature expansion backend_routing metadata missing in $ArtifactPath"
  }
  if (-not [bool]$BackendRouting.supports_capability_routing) {
    Stop-FrontendFeatureGuard "frontend core feature expansion supports_capability_routing must be true in $ArtifactPath"
  }
  if ([string]$BackendRouting.capability_summary_flag -ne $Config.capability_summary_flag) {
    Stop-FrontendFeatureGuard "frontend core feature expansion capability_summary_flag mismatch in $ArtifactPath"
  }
  if ([string]$BackendRouting.route_flag -ne $Config.route_flag) {
    Stop-FrontendFeatureGuard "frontend core feature expansion route_flag mismatch in $ArtifactPath"
  }
}

Export-ModuleMember -Function "Assert-FrontendCoreFeaturePayloadImpl"
