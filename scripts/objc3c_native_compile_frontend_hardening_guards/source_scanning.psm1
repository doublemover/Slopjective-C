$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Read-FrontendHardeningGuardJsonArtifact {
  param(
    [string]$Path,
    [string]$ArtifactName
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    Write-Error "$ArtifactName artifact missing at $Path"
    exit 2
  }

  try {
    return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
  } catch {
    Write-Error "$ArtifactName artifact is not valid JSON at $Path"
    exit 2
  }
}

Export-ModuleMember -Function "Read-FrontendHardeningGuardJsonArtifact"
