$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendRecoveryDeterminismHardeningPayload {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  Assert-FrontendHardeningContractAndDependencies -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath

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
