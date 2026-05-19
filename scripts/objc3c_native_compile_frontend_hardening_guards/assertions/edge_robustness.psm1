$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendEdgeRobustnessPayload {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  Assert-FrontendHardeningContractAndDependencies -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath

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
