$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendDiagnosticsHardeningPayload {
  param(
    [object]$Payload,
    [object]$Rules,
    [string]$ArtifactPath
  )

  Assert-FrontendHardeningContractAndDependencies -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath

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
