$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$featureGuardModelsModule = Join-Path $PSScriptRoot "evaluation_models.psm1"
$featureGuardArtifactsModule = Join-Path $PSScriptRoot "evaluation_artifacts.psm1"
$featureGuardDiagnosticsModule = Join-Path $PSScriptRoot "evaluation_diagnostics.psm1"
foreach ($modulePath in @(
    $featureGuardConfigModule,
    $featureGuardNormalizationModule,
    $featureGuardReportingModule,
    $featureGuardModelsModule,
    $featureGuardArtifactsModule,
    $featureGuardDiagnosticsModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard edge payload dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Assert-FrontendEdgeCompatibilityPayloadImpl {
  param(
    [object]$Payload,
    [string]$ArtifactPath,
    [object]$CoreFeatureGuard
  )

  $config = Get-FrontendEdgeCompatibilityGuardConfig
  if ([string]$Payload.contract_id -ne $config.contract_id) {
    Stop-FrontendFeatureGuard "frontend edge compatibility contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendFeatureDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($config.dependency_contract_ids) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  Assert-FrontendEdgeInvocationMetadata `
    -EdgeCompat (Read-FrontendEdgeCompatibilityInvocationMetadata -Payload $Payload) `
    -Config $config `
    -ArtifactPath $ArtifactPath

  $backendCompat = Read-FrontendEdgeCompatibilityBackendMetadata -Payload $Payload
  if ($null -eq $backendCompat) {
    Stop-FrontendFeatureGuard "frontend edge compatibility backend_compat metadata missing in $ArtifactPath"
  }

  $canonicalBackends = New-FrontendFeatureLowercaseStringSet -Values @($backendCompat.canonical_allowed_backends)
  if ($canonicalBackends.Count -eq 0) {
    Stop-FrontendFeatureGuard "frontend edge compatibility canonical_allowed_backends must be non-empty in $ArtifactPath"
  }
  Assert-FrontendEdgeCoreBackendsCovered `
    -CoreFeatureGuard $CoreFeatureGuard `
    -CanonicalBackends $canonicalBackends

  $aliasMap = New-FrontendEdgeBackendAliasMap `
    -AliasPayload $backendCompat.alias_to_canonical `
    -CanonicalBackends $canonicalBackends `
    -ArtifactPath $ArtifactPath
  $singleValueFlags = New-FrontendFeatureCountedFlagSet -Values @($backendCompat.single_value_flags)
  foreach ($requiredSingleValueFlag in @($config.required_single_value_flags)) {
    if (-not $singleValueFlags.ContainsKey($requiredSingleValueFlag)) {
      Stop-FrontendFeatureGuard "frontend edge compatibility missing single-value flag '$requiredSingleValueFlag' in $ArtifactPath"
    }
  }

  return New-FrontendEdgeCompatibilityPayloadEvaluation `
    -AliasMap $aliasMap `
    -SingleValueFlags $singleValueFlags
}

function Assert-FrontendEdgeInvocationMetadata {
  param(
    [object]$EdgeCompat,
    [object]$Config,
    [string]$ArtifactPath
  )

  if ($null -eq $EdgeCompat) {
    Stop-FrontendFeatureGuard "frontend edge compatibility invocation_edge_compat metadata missing in $ArtifactPath"
  }
  if ([int]$EdgeCompat.fail_closed_exit_code -ne $Config.fail_closed_exit_code) {
    Stop-FrontendFeatureGuard "frontend edge compatibility fail_closed_exit_code must be 2 in $ArtifactPath"
  }
  if (-not [bool]$EdgeCompat.disallow_relative_parent_segments) {
    Stop-FrontendFeatureGuard "frontend edge compatibility disallow_relative_parent_segments must be true in $ArtifactPath"
  }
  if ([string]$EdgeCompat.route_flag -ne $Config.route_flag) {
    Stop-FrontendFeatureGuard "frontend edge compatibility route_flag mismatch in $ArtifactPath"
  }
  if ([string]$EdgeCompat.capability_summary_flag -ne $Config.capability_summary_flag) {
    Stop-FrontendFeatureGuard "frontend edge compatibility capability_summary_flag mismatch in $ArtifactPath"
  }
}

function Assert-FrontendEdgeCoreBackendsCovered {
  param(
    [object]$CoreFeatureGuard,
    [hashtable]$CanonicalBackends
  )

  if ($null -eq $CoreFeatureGuard) {
    return
  }
  foreach ($coreBackend in @($CoreFeatureGuard.allowed_ir_object_backends)) {
    $coreBackendText = ([string]$coreBackend).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($coreBackendText) -and
        -not $CanonicalBackends.ContainsKey($coreBackendText)) {
      Stop-FrontendFeatureGuard "frontend edge compatibility missing backend '$coreBackendText' declared by frontend core feature expansion"
    }
  }
}

Export-ModuleMember -Function "Assert-FrontendEdgeCompatibilityPayloadImpl"
