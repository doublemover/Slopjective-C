$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
