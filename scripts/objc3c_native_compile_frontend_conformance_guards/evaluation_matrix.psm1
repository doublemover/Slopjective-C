$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$conformanceGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$conformanceGuardCommonEvaluationModule = Join-Path $PSScriptRoot "evaluation_common.psm1"
foreach ($modulePath in @(
    $conformanceGuardConfigModule,
    $conformanceGuardNormalizationModule,
    $conformanceGuardReportingModule,
    $conformanceGuardCommonEvaluationModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard matrix dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

$matrixEvaluationRoot = Join-Path $PSScriptRoot "evaluation_matrix"
$matrixEvaluationModules = @(
  "acceptance_rows.psm1",
  "rejection_rows.psm1"
)

foreach ($matrixEvaluationModule in $matrixEvaluationModules) {
  $matrixEvaluationModulePath = Join-Path $matrixEvaluationRoot $matrixEvaluationModule
  if (!(Test-Path -LiteralPath $matrixEvaluationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance matrix evaluation helper missing at $matrixEvaluationModulePath"
    exit 2
  }
  . $matrixEvaluationModulePath
}

function Invoke-FrontendConformanceMatrixEvaluationImpl {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  $config = Get-FrontendConformanceMatrixGuardConfig
  if ([string]$Payload.contract_id -ne $config.contract_id) {
    Stop-FrontendConformanceGuard "frontend conformance matrix contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendConformanceDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($config.dependency_contract_ids) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  if ([int]$Payload.acceptance_profile_count -le 0) {
    Stop-FrontendConformanceGuard "frontend conformance matrix acceptance_profile_count must be positive in $ArtifactPath"
  }
  if ([int]$Payload.rejection_profile_count -le 0) {
    Stop-FrontendConformanceGuard "frontend conformance matrix rejection_profile_count must be positive in $ArtifactPath"
  }

  $acceptanceProfileSet = Assert-FrontendConformanceMatrixAcceptanceRows `
    -Rows @($Payload.acceptance_matrix) `
    -AcceptanceProfileCount ([int]$Payload.acceptance_profile_count) `
    -ArtifactPath $ArtifactPath
  $rejectDiagnosticSet = Assert-FrontendConformanceMatrixRejectionRows `
    -Rows @($Payload.rejection_matrix) `
    -RejectionProfileCount ([int]$Payload.rejection_profile_count) `
    -ArtifactPath $ArtifactPath

  Assert-FrontendConformanceRequiredDiagnostics `
    -PresentDiagnostics $rejectDiagnosticSet `
    -RequiredDiagnostics @($config.required_rejection_diagnostics) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  $invocationProfileKey = Get-FrontendConformanceInvocationProfileKey `
    -ParsedArgs $ParsedArgs `
    -EffectiveCompileArgs $EffectiveCompileArgs
  if (-not $acceptanceProfileSet.ContainsKey($invocationProfileKey)) {
    Stop-FrontendConformanceGuard "frontend conformance matrix has no acceptance row for invocation profile '$invocationProfileKey' in $ArtifactPath"
  }

  return [pscustomobject]@{
    conformance_matrix_path = $ArtifactPath
    profile_key = $invocationProfileKey
    case_id = [string]$acceptanceProfileSet[$invocationProfileKey]
  }
}

Export-ModuleMember -Function "Invoke-FrontendConformanceMatrixEvaluationImpl"
