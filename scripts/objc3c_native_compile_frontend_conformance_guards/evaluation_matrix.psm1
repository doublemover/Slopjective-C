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

function Assert-FrontendConformanceMatrixAcceptanceRows {
  param(
    [object[]]$Rows,
    [int]$AcceptanceProfileCount,
    [string]$ArtifactPath
  )

  $acceptanceProfileSet = @{}
  foreach ($row in $Rows) {
    $caseId = [string]$row.case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Stop-FrontendConformanceGuard "frontend conformance matrix acceptance rows must define case_id and profile_key in $ArtifactPath"
    }
    if ($expectedResult -ne "accept") {
      Stop-FrontendConformanceGuard "frontend conformance matrix acceptance row '$caseId' must declare expected_result='accept' in $ArtifactPath"
    }
    if ($acceptanceProfileSet.ContainsKey($profileKey)) {
      Stop-FrontendConformanceGuard "frontend conformance matrix duplicate acceptance profile '$profileKey' in $ArtifactPath"
    }
    $acceptanceProfileSet[$profileKey] = $caseId
  }
  if ($AcceptanceProfileCount -ne $Rows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance matrix acceptance_profile_count mismatch in $ArtifactPath"
  }

  $expectedProfileSet = New-FrontendConformanceExpectedProfileSet
  foreach ($expectedProfile in $expectedProfileSet.Keys) {
    if (-not $acceptanceProfileSet.ContainsKey($expectedProfile)) {
      Stop-FrontendConformanceGuard "frontend conformance matrix missing acceptance profile '$expectedProfile' in $ArtifactPath"
    }
  }

  return $acceptanceProfileSet
}

function Assert-FrontendConformanceMatrixRejectionRows {
  param(
    [object[]]$Rows,
    [int]$RejectionProfileCount,
    [string]$ArtifactPath
  )

  if ($RejectionProfileCount -ne $Rows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance matrix rejection_profile_count mismatch in $ArtifactPath"
  }

  $rejectDiagnosticSet = @{}
  foreach ($row in $Rows) {
    $caseId = [string]$row.case_id
    $expectedResult = [string]$row.expected_result
    $requiredDiagnostic = [string]$row.required_diagnostic
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($requiredDiagnostic)) {
      Stop-FrontendConformanceGuard "frontend conformance matrix rejection rows must define case_id and required_diagnostic in $ArtifactPath"
    }
    if ($expectedResult -ne "reject") {
      Stop-FrontendConformanceGuard "frontend conformance matrix rejection row '$caseId' must declare expected_result='reject' in $ArtifactPath"
    }
    $rejectDiagnosticSet[$requiredDiagnostic] = $true
  }

  return $rejectDiagnosticSet
}

Export-ModuleMember -Function "Invoke-FrontendConformanceMatrixEvaluationImpl"
