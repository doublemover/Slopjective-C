$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendIntegrationCloseoutGate {
  param(
    [object]$CloseoutGate,
    [object]$Config,
    [string]$ArtifactPath
  )

  if ($null -eq $CloseoutGate) {
    Stop-FrontendConformanceGuard "frontend integration closeout closeout_gate metadata missing in $ArtifactPath"
  }
  Assert-FrontendIntegrationCloseoutSignoff `
    -CloseoutGate $CloseoutGate `
    -PropertyName "build_integration_gate_signoff" `
    -ArtifactPath $ArtifactPath
  Assert-FrontendIntegrationCloseoutSignoff `
    -CloseoutGate $CloseoutGate `
    -PropertyName "invocation_profile_gate_signoff" `
    -ArtifactPath $ArtifactPath
  Assert-FrontendIntegrationCloseoutSignoff `
    -CloseoutGate $CloseoutGate `
    -PropertyName "corpus_coverage_gate_signoff" `
    -ArtifactPath $ArtifactPath

  if ([int]$CloseoutGate.deterministic_fail_closed_exit_code -ne $Config.deterministic_fail_closed_exit_code) {
    Stop-FrontendConformanceGuard "frontend integration closeout deterministic_fail_closed_exit_code must be 2 in $ArtifactPath"
  }
  if ([int]$CloseoutGate.acceptance_corpus_count -le 0 -or [int]$CloseoutGate.rejection_corpus_count -le 0) {
    Stop-FrontendConformanceGuard "frontend integration closeout acceptance/rejection corpus counts must be positive in $ArtifactPath"
  }
}

function Assert-FrontendIntegrationCloseoutSignoff {
  param(
    [object]$CloseoutGate,
    [string]$PropertyName,
    [string]$ArtifactPath
  )

  if (-not [bool]$CloseoutGate.$PropertyName) {
    Stop-FrontendConformanceGuard "frontend integration closeout $PropertyName must be true in $ArtifactPath"
  }
}
