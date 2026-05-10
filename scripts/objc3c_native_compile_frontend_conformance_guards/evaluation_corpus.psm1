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

function Assert-FrontendConformanceCorpusCounts {
  param(
    [object[]]$AcceptanceRows,
    [object[]]$RejectionRows,
    [object]$Payload,
    [string]$ArtifactPath
  )

  if ([int]$Payload.acceptance_corpus_count -ne $AcceptanceRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance corpus acceptance_corpus_count mismatch in $ArtifactPath"
  }
  if ([int]$Payload.rejection_corpus_count -ne $RejectionRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance corpus rejection_corpus_count mismatch in $ArtifactPath"
  }
  if ([int]$Payload.corpus_case_count -ne ($AcceptanceRows.Count + $RejectionRows.Count)) {
    Stop-FrontendConformanceGuard "frontend conformance corpus corpus_case_count mismatch in $ArtifactPath"
  }
  if ($AcceptanceRows.Count -le 0 -or $RejectionRows.Count -le 0) {
    Stop-FrontendConformanceGuard "frontend conformance corpus requires non-empty acceptance and rejection corpus in $ArtifactPath"
  }
}

function Assert-FrontendConformanceCorpusAcceptanceRows {
  param(
    [object[]]$Rows,
    [string]$ArtifactPath
  )

  $acceptanceByProfile = @{}
  foreach ($row in $Rows) {
    $caseId = [string]$row.corpus_case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Stop-FrontendConformanceGuard "frontend conformance corpus acceptance rows must define corpus_case_id and profile_key in $ArtifactPath"
    }
    if ($expectedResult -ne "accept") {
      Stop-FrontendConformanceGuard "frontend conformance corpus acceptance row '$caseId' must declare expected_result='accept' in $ArtifactPath"
    }
    if ($expectedExitCode -ne 0) {
      Stop-FrontendConformanceGuard "frontend conformance corpus acceptance row '$caseId' must declare expected_exit_code=0 in $ArtifactPath"
    }
    if ($compileArgs.Count -le 0) {
      Stop-FrontendConformanceGuard "frontend conformance corpus acceptance row '$caseId' must provide compile_args in $ArtifactPath"
    }
    if (-not $acceptanceByProfile.ContainsKey($profileKey)) {
      $acceptanceByProfile[$profileKey] = New-Object System.Collections.Generic.List[string]
    }
    $acceptanceByProfile[$profileKey].Add($caseId)
  }

  return $acceptanceByProfile
}

function Assert-FrontendConformanceCorpusRejectionRows {
  param(
    [object[]]$Rows,
    [string]$ArtifactPath
  )

  $rejectDiagnosticSet = @{}
  foreach ($row in $Rows) {
    $caseId = [string]$row.corpus_case_id
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $expectedDiagnostic = [string]$row.expected_diagnostic
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($expectedDiagnostic)) {
      Stop-FrontendConformanceGuard "frontend conformance corpus rejection rows must define corpus_case_id and expected_diagnostic in $ArtifactPath"
    }
    if ($expectedResult -ne "reject") {
      Stop-FrontendConformanceGuard "frontend conformance corpus rejection row '$caseId' must declare expected_result='reject' in $ArtifactPath"
    }
    if ($expectedExitCode -ne 2) {
      Stop-FrontendConformanceGuard "frontend conformance corpus rejection row '$caseId' must declare expected_exit_code=2 in $ArtifactPath"
    }
    if ($compileArgs.Count -le 0) {
      Stop-FrontendConformanceGuard "frontend conformance corpus rejection row '$caseId' must provide compile_args in $ArtifactPath"
    }
    $rejectDiagnosticSet[$expectedDiagnostic] = $true
  }

  return $rejectDiagnosticSet
}

Export-ModuleMember -Function "Invoke-FrontendConformanceCorpusEvaluationImpl"
