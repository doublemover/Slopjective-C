$ErrorActionPreference = "Stop"

function Assert-Objc3cNativeFrontendCloseoutConformanceContractId {
  param(
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

function Assert-Objc3cNativeFrontendCloseoutConformanceCorpusCoverage {
  param(
    [int]$AcceptanceCount,
    [int]$RejectionCount
  )

  if ($AcceptanceCount -le 0 -or $RejectionCount -le 0) {
    throw "frontend conformance corpus must provide non-empty acceptance and rejection coverage for integration closeout"
  }
}
