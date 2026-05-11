$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Read-FrontendConformanceJsonArtifact {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$ArtifactName
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    Stop-FrontendConformanceGuard "$ArtifactName artifact missing at $Path"
  }

  try {
    return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
  } catch {
    Stop-FrontendConformanceGuard "$ArtifactName artifact is not valid JSON at $Path"
  }
}
