$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function ConvertTo-FrontendHardeningStringSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $valueText = [string]$value
    if (-not [string]::IsNullOrWhiteSpace($valueText)) {
      $set[$valueText] = $true
    }
  }
  return $set
}

function Assert-FrontendHardeningContractAndDependencies {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  if ([string]$Payload.contract_id -ne [string]$Rules.contract_id) {
    Write-Error "$($Rules.artifact_name) contract id mismatch in $ArtifactPath"
    exit 2
  }

  $dependencySet = ConvertTo-FrontendHardeningStringSet -Values $Payload.depends_on_contract_ids
  foreach ($requiredContractId in @($Rules.dependency_contract_ids)) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "$($Rules.artifact_name) missing dependency contract '$requiredContractId' in $ArtifactPath"
      exit 2
    }
  }
}

function Assert-FrontendHardeningMetadataPresent {
  param(
    [object]$Metadata,
    [string]$ArtifactName,
    [string]$FieldName,
    [string]$ArtifactPath
  )

  if ($null -eq $Metadata) {
    Write-Error "$ArtifactName $FieldName metadata missing in $ArtifactPath"
    exit 2
  }
}

function Assert-FrontendHardeningRequiredValues {
  param(
    [object[]]$ActualValues,
    [string[]]$RequiredValues,
    [string]$MessageTemplate,
    [string]$ArtifactPath
  )

  $valueSet = ConvertTo-FrontendHardeningStringSet -Values $ActualValues
  foreach ($requiredValue in @($RequiredValues)) {
    if (-not $valueSet.ContainsKey($requiredValue)) {
      Write-Error ($MessageTemplate -f $requiredValue, $ArtifactPath)
      exit 2
    }
  }
}

function Assert-FrontendHardeningFailClosedExitCode {
  param(
    [object]$Metadata,
    [int]$ExpectedExitCode,
    [string]$ArtifactName,
    [string]$ArtifactPath
  )

  if ([int]$Metadata.fail_closed_exit_code -ne $ExpectedExitCode) {
    Write-Error "$ArtifactName fail_closed_exit_code must be $ExpectedExitCode in $ArtifactPath"
    exit 2
  }
}
