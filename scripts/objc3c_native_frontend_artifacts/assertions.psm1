$ErrorActionPreference = "Stop"

function Assert-Objc3cNativeFrontendArtifactPath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Path,
    [Parameter(Mandatory = $true)]
    [string]$MissingMessage
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw $MissingMessage
  }
}

function Assert-Objc3cNativeFrontendArtifactContractId {
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

function Assert-Objc3cNativeFrontendSourceGraphModuleMetadata {
  param(
    [Parameter(Mandatory = $true)]
    [AllowEmptyString()]
    [string]$Name,
    [Parameter(Mandatory = $true)]
    [AllowEmptyCollection()]
    [object[]]$Sources
  )

  if ([string]::IsNullOrWhiteSpace($Name) -or $Sources.Count -eq 0) {
    throw "frontend source graph module metadata is invalid"
  }
}

function Assert-Objc3cNativeFrontendCoreFeatureModuleName {
  param(
    [Parameter(Mandatory = $true)]
    [AllowEmptyString()]
    [string]$ModuleName
  )

  if ([string]::IsNullOrWhiteSpace($ModuleName)) {
    throw "frontend module metadata contains empty module name for core feature expansion artifact"
  }
}
