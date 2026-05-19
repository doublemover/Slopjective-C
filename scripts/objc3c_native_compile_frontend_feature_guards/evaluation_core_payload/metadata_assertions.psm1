$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
