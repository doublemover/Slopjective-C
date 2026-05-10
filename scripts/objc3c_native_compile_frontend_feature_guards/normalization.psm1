$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
if (!(Test-Path -LiteralPath $featureGuardReportingModule -PathType Leaf)) {
  Write-Error "native compile frontend feature guard reporting module missing at $featureGuardReportingModule"
  exit 2
}
Import-Module $featureGuardReportingModule -Force -DisableNameChecking

function Read-FrontendFeatureGuardJsonArtifact {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$ArtifactName
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    Stop-FrontendFeatureGuard "$ArtifactName artifact missing at $Path"
  }

  try {
    return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
  } catch {
    Stop-FrontendFeatureGuard "$ArtifactName artifact is not valid JSON at $Path"
  }
}

function New-FrontendFeatureStringSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $text = [string]$value
    if (-not [string]::IsNullOrWhiteSpace($text)) {
      $set[$text] = $true
    }
  }
  return $set
}

function Normalize-FrontendFeatureBackendKey {
  param(
    [string]$Value
  )

  if ([string]::IsNullOrWhiteSpace($Value)) {
    return ""
  }
  return $Value.Trim().ToLowerInvariant().Replace("_", "-")
}

function New-FrontendFeatureBackendSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $backendText = ([string]$value).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $set[$backendText] = [string]$value
    }
  }
  return $set
}

function New-FrontendFeatureLowercaseStringSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $text = ([string]$value).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($text)) {
      $set[$text] = $true
    }
  }
  return $set
}

function New-FrontendFeatureCountedFlagSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $text = [string]$value
    if (-not [string]::IsNullOrWhiteSpace($text)) {
      $set[$text] = 0
    }
  }
  return $set
}

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

function Test-FrontendFeatureRelativePathHasParentSegment {
  param(
    [string]$Path
  )

  return (-not [System.IO.Path]::IsPathRooted($Path) -and $Path.Replace('\', '/').Split('/') -contains "..")
}

Export-ModuleMember -Function @(
  "Read-FrontendFeatureGuardJsonArtifact",
  "New-FrontendFeatureStringSet",
  "Normalize-FrontendFeatureBackendKey",
  "New-FrontendFeatureBackendSet",
  "New-FrontendFeatureLowercaseStringSet",
  "New-FrontendFeatureCountedFlagSet",
  "New-FrontendEdgeBackendAliasMap",
  "Test-FrontendFeatureRelativePathHasParentSegment"
)
