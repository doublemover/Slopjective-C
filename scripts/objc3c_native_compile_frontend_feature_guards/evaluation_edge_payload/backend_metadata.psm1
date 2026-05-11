$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-FrontendEdgeCompatibilityBackendPayloadEvaluation {
  param(
    [object]$BackendCompat,
    [object]$Config,
    [object]$CoreFeatureGuard,
    [string]$ArtifactPath
  )

  if ($null -eq $BackendCompat) {
    Stop-FrontendFeatureGuard "frontend edge compatibility backend_compat metadata missing in $ArtifactPath"
  }

  $canonicalBackends = New-FrontendFeatureLowercaseStringSet -Values @($BackendCompat.canonical_allowed_backends)
  if ($canonicalBackends.Count -eq 0) {
    Stop-FrontendFeatureGuard "frontend edge compatibility canonical_allowed_backends must be non-empty in $ArtifactPath"
  }
  Assert-FrontendEdgeCoreBackendsCovered `
    -CoreFeatureGuard $CoreFeatureGuard `
    -CanonicalBackends $canonicalBackends

  $aliasMap = New-FrontendEdgeBackendAliasMap `
    -AliasPayload $BackendCompat.alias_to_canonical `
    -CanonicalBackends $canonicalBackends `
    -ArtifactPath $ArtifactPath
  $singleValueFlags = New-FrontendFeatureCountedFlagSet -Values @($BackendCompat.single_value_flags)
  Assert-FrontendEdgeRequiredSingleValueFlags `
    -SingleValueFlags $singleValueFlags `
    -Config $Config `
    -ArtifactPath $ArtifactPath

  return New-FrontendEdgeCompatibilityPayloadEvaluation `
    -AliasMap $aliasMap `
    -SingleValueFlags $singleValueFlags
}

function Assert-FrontendEdgeRequiredSingleValueFlags {
  param(
    [hashtable]$SingleValueFlags,
    [object]$Config,
    [string]$ArtifactPath
  )

  foreach ($requiredSingleValueFlag in @($Config.required_single_value_flags)) {
    if (-not $SingleValueFlags.ContainsKey($requiredSingleValueFlag)) {
      Stop-FrontendFeatureGuard "frontend edge compatibility missing single-value flag '$requiredSingleValueFlag' in $ArtifactPath"
    }
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
