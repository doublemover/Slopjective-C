$ErrorActionPreference = "Stop"

function Read-Objc3cNativeFrontendJsonArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Path,
    [Parameter(Mandatory = $true)]
    [string]$MissingMessage,
    [Parameter(Mandatory = $true)]
    [string]$InvalidJsonMessage
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw $MissingMessage
  }

  try {
    return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
  } catch {
    throw $InvalidJsonMessage
  }
}
