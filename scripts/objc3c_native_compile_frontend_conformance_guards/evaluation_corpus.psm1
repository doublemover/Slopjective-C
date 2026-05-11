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
    Write-Error "native compile frontend conformance guard corpus dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

$corpusEvaluationRoot = Join-Path $PSScriptRoot "evaluation_corpus"
$corpusEvaluationModules = @(
  "corpus_counts.psm1",
  "acceptance_rows.psm1",
  "rejection_rows.psm1"
)

foreach ($corpusEvaluationModule in $corpusEvaluationModules) {
  $corpusEvaluationModulePath = Join-Path $corpusEvaluationRoot $corpusEvaluationModule
  if (!(Test-Path -LiteralPath $corpusEvaluationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance corpus evaluation helper missing at $corpusEvaluationModulePath"
    exit 2
  }
  . $corpusEvaluationModulePath
}

function Invoke-FrontendConformanceCorpusEvaluationImpl {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [string]$InvocationProfileKey
  )

  $config = Get-FrontendConformanceCorpusGuardConfig
  if ([string]$Payload.contract_id -ne $config.contract_id) {
    Stop-FrontendConformanceGuard "frontend conformance corpus contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendConformanceDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($config.dependency_contract_ids) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  $acceptanceRows = @($Payload.acceptance_corpus)
  $rejectionRows = @($Payload.rejection_corpus)
  Assert-FrontendConformanceCorpusCounts `
    -AcceptanceRows $acceptanceRows `
    -RejectionRows $rejectionRows `
    -Payload $Payload `
    -ArtifactPath $ArtifactPath

  $acceptanceByProfile = Assert-FrontendConformanceCorpusAcceptanceRows `
    -Rows $acceptanceRows `
    -ArtifactPath $ArtifactPath
  $rejectDiagnosticSet = Assert-FrontendConformanceCorpusRejectionRows `
    -Rows $rejectionRows `
    -ArtifactPath $ArtifactPath

  Assert-FrontendConformanceRequiredDiagnostics `
    -PresentDiagnostics $rejectDiagnosticSet `
    -RequiredDiagnostics @($config.required_rejection_diagnostics) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  if ([string]::IsNullOrWhiteSpace($InvocationProfileKey)) {
    Stop-FrontendConformanceGuard "frontend conformance corpus invocation profile key is required"
  }
  if (-not $acceptanceByProfile.ContainsKey($InvocationProfileKey)) {
    Stop-FrontendConformanceGuard "frontend conformance corpus has no acceptance case for invocation profile '$InvocationProfileKey' in $ArtifactPath"
  }

  return [pscustomobject]@{
    conformance_corpus_path = $ArtifactPath
    profile_key = $InvocationProfileKey
    acceptance_case_count = [int]$acceptanceByProfile[$InvocationProfileKey].Count
  }
}

Export-ModuleMember -Function "Invoke-FrontendConformanceCorpusEvaluationImpl"
