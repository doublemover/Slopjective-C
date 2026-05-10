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

function Assert-FrontendHardeningContractId {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  if ([string]$Payload.contract_id -ne [string]$Rules.contract_id) {
    Write-Error "$($Rules.artifact_name) contract id mismatch in $ArtifactPath"
    exit 2
  }
}

function Assert-FrontendHardeningDependencies {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

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

function Assert-FrontendEdgeRobustnessPayload {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  Assert-FrontendHardeningContractId -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath
  Assert-FrontendHardeningDependencies -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath

  $guardrails = $Payload.wrapper_guardrails
  Assert-FrontendHardeningMetadataPresent `
    -Metadata $guardrails `
    -ArtifactName $Rules.artifact_name `
    -FieldName "wrapper_guardrails" `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningRequiredValues `
    -ActualValues $guardrails.wrapper_single_value_flags `
    -RequiredValues $Rules.wrapper_single_value_flags `
    -MessageTemplate "$($Rules.artifact_name) missing wrapper_single_value flag '{0}' in {1}" `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningRequiredValues `
    -ActualValues $guardrails.compile_single_value_flags `
    -RequiredValues $Rules.compile_single_value_flags `
    -MessageTemplate "$($Rules.artifact_name) missing compile_single_value flag '{0}' in {1}" `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningRequiredValues `
    -ActualValues $guardrails.reject_empty_equals_value_flags `
    -RequiredValues $Rules.reject_empty_equals_value_flags `
    -MessageTemplate "$($Rules.artifact_name) missing reject_empty_equals_value flag '{0}' in {1}" `
    -ArtifactPath $ArtifactPath
}

function Assert-FrontendDiagnosticsHardeningPayload {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  Assert-FrontendHardeningContractId -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath
  Assert-FrontendHardeningDependencies -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath

  $wrapperDiagnostics = $Payload.wrapper_diagnostics
  Assert-FrontendHardeningMetadataPresent `
    -Metadata $wrapperDiagnostics `
    -ArtifactName $Rules.artifact_name `
    -FieldName "wrapper_diagnostics" `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningFailClosedExitCode `
    -Metadata $wrapperDiagnostics `
    -ExpectedExitCode $Rules.fail_closed_exit_code `
    -ArtifactName $Rules.artifact_name `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningRequiredValues `
    -ActualValues $wrapperDiagnostics.required_error_messages `
    -RequiredValues $Rules.required_error_messages `
    -MessageTemplate "$($Rules.artifact_name) missing required_error_messages entry '{0}' in {1}" `
    -ArtifactPath $ArtifactPath
}

function Assert-FrontendRecoveryDeterminismHardeningPayload {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  Assert-FrontendHardeningContractId -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath
  Assert-FrontendHardeningDependencies -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath

  $cacheDeterminism = $Payload.cache_determinism
  Assert-FrontendHardeningMetadataPresent `
    -Metadata $cacheDeterminism `
    -ArtifactName $Rules.artifact_name `
    -FieldName "cache_determinism" `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningFailClosedExitCode `
    -Metadata $cacheDeterminism `
    -ExpectedExitCode $Rules.fail_closed_exit_code `
    -ArtifactName $Rules.artifact_name `
    -ArtifactPath $ArtifactPath

  if ([string]$cacheDeterminism.entry_contract_id -ne [string]$Rules.entry_contract_id) {
    Write-Error "$($Rules.artifact_name) entry_contract_id mismatch in $ArtifactPath"
    exit 2
  }

  Assert-FrontendHardeningRequiredValues `
    -ActualValues $cacheDeterminism.cache_status_tokens `
    -RequiredValues $Rules.cache_status_tokens `
    -MessageTemplate "$($Rules.artifact_name) missing cache_status_tokens entry '{0}' in {1}" `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningRequiredValues `
    -ActualValues $cacheDeterminism.required_entry_files `
    -RequiredValues $Rules.required_entry_files `
    -MessageTemplate "$($Rules.artifact_name) missing required_entry_files entry '{0}' in {1}" `
    -ArtifactPath $ArtifactPath

  Assert-FrontendHardeningRequiredValues `
    -ActualValues $cacheDeterminism.recovery_signals `
    -RequiredValues $Rules.recovery_signals `
    -MessageTemplate "$($Rules.artifact_name) missing recovery_signals entry '{0}' in {1}" `
    -ArtifactPath $ArtifactPath
}

Export-ModuleMember -Function @(
  "Assert-FrontendEdgeRobustnessPayload",
  "Assert-FrontendDiagnosticsHardeningPayload",
  "Assert-FrontendRecoveryDeterminismHardeningPayload"
)
