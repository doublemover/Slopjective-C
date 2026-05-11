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

$closeoutEvaluationRoot = Join-Path $PSScriptRoot "evaluation_closeout"
$closeoutEvaluationModules = @(
  "closeout_gate.psm1"
)

foreach ($closeoutEvaluationModule in $closeoutEvaluationModules) {
  $closeoutEvaluationModulePath = Join-Path $closeoutEvaluationRoot $closeoutEvaluationModule
  if (!(Test-Path -LiteralPath $closeoutEvaluationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance closeout evaluation helper missing at $closeoutEvaluationModulePath"
    exit 2
  }
  . $closeoutEvaluationModulePath
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

Export-ModuleMember -Function "Invoke-FrontendIntegrationCloseoutEvaluationImpl"
