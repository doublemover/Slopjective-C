$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Read-Objc3cNativeFrontendArtifactGuardJson {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Path,
    [Parameter(Mandatory = $true)]
    [string]$MissingMessage,
    [Parameter(Mandatory = $true)]
    [string]$InvalidMessage
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    Write-Error $MissingMessage
    exit 2
  }

  try {
    return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
  } catch {
    Write-Error $InvalidMessage
    exit 2
  }
}

function Assert-Objc3cNativeFrontendArtifactGuardContractId {
  param(
    [Parameter(Mandatory = $true)]
    $Payload,
    [Parameter(Mandatory = $true)]
    [string]$ExpectedContractId,
    [Parameter(Mandatory = $true)]
    [string]$ErrorMessage
  )

  if ([string]$Payload.contract_id -ne $ExpectedContractId) {
    Write-Error $ErrorMessage
    exit 2
  }
}
