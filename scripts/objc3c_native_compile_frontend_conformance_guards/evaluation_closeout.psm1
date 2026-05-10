$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$conformanceGuardCommonEvaluationModule = Join-Path $PSScriptRoot "evaluation_common.psm1"
foreach ($modulePath in @(
    $conformanceGuardConfigModule,
    $conformanceGuardReportingModule,
    $conformanceGuardCommonEvaluationModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard closeout dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Invoke-FrontendIntegrationCloseoutEvaluationImpl {
  param(
    [string]$ArtifactPath,
    [object]$Payload
  )

  $config = Get-FrontendIntegrationCloseoutGuardConfig
  if ([string]$Payload.contract_id -ne $config.contract_id) {
    Stop-FrontendConformanceGuard "frontend integration closeout contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendConformanceDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($config.dependency_contract_ids) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  Assert-FrontendIntegrationCloseoutGate `
    -CloseoutGate $Payload.closeout_gate `
    -Config $config `
    -ArtifactPath $ArtifactPath

  return [pscustomobject]@{
    integration_closeout_path = $ArtifactPath
  }
}

function Assert-FrontendIntegrationCloseoutGate {
  param(
    [object]$CloseoutGate,
    [object]$Config,
    [string]$ArtifactPath
  )

  if ($null -eq $CloseoutGate) {
    Stop-FrontendConformanceGuard "frontend integration closeout closeout_gate metadata missing in $ArtifactPath"
  }
  if (-not [bool]$CloseoutGate.build_integration_gate_signoff) {
    Stop-FrontendConformanceGuard "frontend integration closeout build_integration_gate_signoff must be true in $ArtifactPath"
  }
  if (-not [bool]$CloseoutGate.invocation_profile_gate_signoff) {
    Stop-FrontendConformanceGuard "frontend integration closeout invocation_profile_gate_signoff must be true in $ArtifactPath"
  }
  if (-not [bool]$CloseoutGate.corpus_coverage_gate_signoff) {
    Stop-FrontendConformanceGuard "frontend integration closeout corpus_coverage_gate_signoff must be true in $ArtifactPath"
  }
  if ([int]$CloseoutGate.deterministic_fail_closed_exit_code -ne $Config.deterministic_fail_closed_exit_code) {
    Stop-FrontendConformanceGuard "frontend integration closeout deterministic_fail_closed_exit_code must be 2 in $ArtifactPath"
  }
  if ([int]$CloseoutGate.acceptance_corpus_count -le 0 -or [int]$CloseoutGate.rejection_corpus_count -le 0) {
    Stop-FrontendConformanceGuard "frontend integration closeout acceptance/rejection corpus counts must be positive in $ArtifactPath"
  }
}

Export-ModuleMember -Function "Invoke-FrontendIntegrationCloseoutEvaluationImpl"
