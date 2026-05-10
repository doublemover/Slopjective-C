$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$featureGuardModelsModule = Join-Path $PSScriptRoot "evaluation_models.psm1"
$featureGuardArtifactsModule = Join-Path $PSScriptRoot "evaluation_artifacts.psm1"
$featureGuardDiagnosticsModule = Join-Path $PSScriptRoot "evaluation_diagnostics.psm1"
$featureGuardConditionsModule = Join-Path $PSScriptRoot "evaluation_conditions.psm1"
$featureGuardResultsModule = Join-Path $PSScriptRoot "evaluation_results.psm1"
foreach ($modulePath in @(
    $featureGuardConfigModule,
    $featureGuardNormalizationModule,
    $featureGuardReportingModule,
    $featureGuardModelsModule,
    $featureGuardArtifactsModule,
    $featureGuardDiagnosticsModule,
    $featureGuardConditionsModule,
    $featureGuardResultsModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard helper module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Invoke-FrontendCoreFeatureExpansionEvaluation {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [object]$ParsedArgs
  )

  $allowedBackends = Assert-FrontendCoreFeaturePayload -Payload $Payload -ArtifactPath $ArtifactPath
  Assert-FrontendCoreCompileArgs -ParsedArgs $ParsedArgs -AllowedBackends $allowedBackends -ArtifactPath $ArtifactPath

  return New-FrontendCoreFeatureExpansionEvaluationResult `
    -ArtifactPath $ArtifactPath `
    -AllowedBackends $allowedBackends
}

function Invoke-FrontendEdgeCompatibilityEvaluation {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  $payloadEvaluation = Assert-FrontendEdgeCompatibilityPayload `
    -Payload $Payload `
    -ArtifactPath $ArtifactPath `
    -CoreFeatureGuard $CoreFeatureGuard
  $normalizedCompileArgs = ConvertTo-FrontendEdgeCompatibleCompileArgs `
    -ParsedArgs $ParsedArgs `
    -AliasMap $payloadEvaluation.alias_map `
    -SingleValueFlags $payloadEvaluation.single_value_flags

  return New-FrontendEdgeCompatibilityEvaluationResult `
    -ArtifactPath $ArtifactPath `
    -NormalizedCompileArgs $normalizedCompileArgs
}

Export-ModuleMember -Function @(
  "Invoke-FrontendCoreFeatureExpansionEvaluation",
  "Invoke-FrontendEdgeCompatibilityEvaluation"
)
