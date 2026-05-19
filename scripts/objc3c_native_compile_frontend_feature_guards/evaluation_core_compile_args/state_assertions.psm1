$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendCoreCompileArgState {
  param(
    [Parameter(Mandatory = $true)]$State,
    [Parameter(Mandatory = $true)][hashtable]$AllowedBackends,
    [Parameter(Mandatory = $true)][string]$ArtifactPath
  )

  if (-not [string]::IsNullOrWhiteSpace($State.requested_backend)) {
    $normalizedRequestedBackend = Normalize-FrontendFeatureBackendKey -Value $State.requested_backend
    if (-not $AllowedBackends.ContainsKey($normalizedRequestedBackend)) {
      Stop-FrontendFeatureGuard "requested --objc3-ir-object-backend '$($State.requested_backend)' is not allowed by frontend core feature expansion in $ArtifactPath"
    }
  }
  Assert-FrontendFeatureCapabilityRoutingHasSummary `
    -UsesCapabilityRouting $State.uses_capability_routing `
    -HasCapabilitySummary $State.has_capability_summary
}
