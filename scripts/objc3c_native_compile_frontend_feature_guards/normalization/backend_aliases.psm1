$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-FrontendEdgeBackendAliasMap {
  param(
    [object]$AliasPayload,
    [hashtable]$CanonicalBackends,
    [string]$ArtifactPath
  )

  if ($null -eq $AliasPayload) {
    Stop-FrontendFeatureGuard "frontend edge compatibility alias_to_canonical mapping missing in $ArtifactPath"
  }

  $aliasMap = @{}
  foreach ($property in $AliasPayload.PSObject.Properties) {
    $alias = Normalize-FrontendFeatureBackendKey -Value ([string]$property.Name)
    $canonical = Normalize-FrontendFeatureBackendKey -Value ([string]$property.Value)
    if ([string]::IsNullOrWhiteSpace($alias) -or [string]::IsNullOrWhiteSpace($canonical)) {
      Stop-FrontendFeatureGuard "frontend edge compatibility alias_to_canonical entries must be non-empty in $ArtifactPath"
    }
    if (-not $CanonicalBackends.ContainsKey($canonical)) {
      Stop-FrontendFeatureGuard "frontend edge compatibility alias '$alias' maps to unknown canonical backend '$canonical' in $ArtifactPath"
    }
    $aliasMap[$alias] = $canonical
  }

  foreach ($canonicalBackend in $CanonicalBackends.Keys) {
    if (-not $aliasMap.ContainsKey($canonicalBackend)) {
      $aliasMap[$canonicalBackend] = $canonicalBackend
    }
  }
  return $aliasMap
}
