$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
