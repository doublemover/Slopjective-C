$ErrorActionPreference = "Stop"

function Read-Objc3cNativeFrontendCloseoutEdgeJsonArtifact {
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

function Assert-Objc3cNativeFrontendCloseoutEdgeContractId {
  param(
    [Parameter(Mandatory = $true)]
    $Payload,
    [Parameter(Mandatory = $true)]
    [string]$ExpectedContractId,
    [Parameter(Mandatory = $true)]
    [string]$MismatchMessage
  )

  if ([string]$Payload.contract_id -ne $ExpectedContractId) {
    throw $MismatchMessage
  }
}

function Get-Objc3cNativeFrontendCloseoutEdgeAllowedBackends {
  param(
    [Parameter(Mandatory = $true)]
    $CoreFeaturePayload,
    [Parameter(Mandatory = $true)]
    [string]$MissingMessage
  )

  $allowedBackends = @()
  foreach ($backend in @($CoreFeaturePayload.backend_routing.allowed_ir_object_backends)) {
    $backendText = [string]$backend
    if (![string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends += $backendText
    }
  }

  if ($allowedBackends.Count -eq 0) {
    throw $MissingMessage
  }

  return $allowedBackends
}
