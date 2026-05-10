$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$conformanceGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
foreach ($modulePath in @($conformanceGuardConfigModule, $conformanceGuardNormalizationModule, $conformanceGuardReportingModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard helper module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Assert-FrontendConformanceDependencyContracts {
  param(
    [object[]]$PresentContracts,
    [string[]]$RequiredContracts,
    [string]$ArtifactName,
    [string]$ArtifactPath
  )

  $dependencySet = New-FrontendConformanceStringSet -Values @($PresentContracts)
  foreach ($requiredContractId in $RequiredContracts) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Stop-FrontendConformanceGuard "$ArtifactName missing dependency contract '$requiredContractId' in $ArtifactPath"
    }
  }
}

function Assert-FrontendConformanceRequiredDiagnostics {
  param(
    [hashtable]$PresentDiagnostics,
    [string[]]$RequiredDiagnostics,
    [string]$ArtifactName,
    [string]$ArtifactPath
  )

  foreach ($requiredDiagnostic in $RequiredDiagnostics) {
    if (-not $PresentDiagnostics.ContainsKey($requiredDiagnostic)) {
      Stop-FrontendConformanceGuard "$ArtifactName missing rejection diagnostic '$requiredDiagnostic' in $ArtifactPath"
    }
  }
}

function Invoke-FrontendConformanceMatrixEvaluation {
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

  $acceptanceRows = @($Payload.acceptance_matrix)
  $acceptanceProfileSet = @{}
  foreach ($row in $acceptanceRows) {
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
  if ([int]$Payload.acceptance_profile_count -ne $acceptanceRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance matrix acceptance_profile_count mismatch in $ArtifactPath"
  }

  $expectedProfileSet = New-FrontendConformanceExpectedProfileSet
  foreach ($expectedProfile in $expectedProfileSet.Keys) {
    if (-not $acceptanceProfileSet.ContainsKey($expectedProfile)) {
      Stop-FrontendConformanceGuard "frontend conformance matrix missing acceptance profile '$expectedProfile' in $ArtifactPath"
    }
  }

  $rejectRows = @($Payload.rejection_matrix)
  if ([int]$Payload.rejection_profile_count -ne $rejectRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance matrix rejection_profile_count mismatch in $ArtifactPath"
  }
  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectRows) {
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

function Invoke-FrontendConformanceCorpusEvaluation {
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
  if ([int]$Payload.acceptance_corpus_count -ne $acceptanceRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance corpus acceptance_corpus_count mismatch in $ArtifactPath"
  }
  if ([int]$Payload.rejection_corpus_count -ne $rejectionRows.Count) {
    Stop-FrontendConformanceGuard "frontend conformance corpus rejection_corpus_count mismatch in $ArtifactPath"
  }
  if ([int]$Payload.corpus_case_count -ne ($acceptanceRows.Count + $rejectionRows.Count)) {
    Stop-FrontendConformanceGuard "frontend conformance corpus corpus_case_count mismatch in $ArtifactPath"
  }
  if ($acceptanceRows.Count -le 0 -or $rejectionRows.Count -le 0) {
    Stop-FrontendConformanceGuard "frontend conformance corpus requires non-empty acceptance and rejection corpus in $ArtifactPath"
  }

  $acceptanceByProfile = @{}
  foreach ($row in $acceptanceRows) {
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

  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectionRows) {
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

function Invoke-FrontendIntegrationCloseoutEvaluation {
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

  $closeoutGate = $Payload.closeout_gate
  if ($null -eq $closeoutGate) {
    Stop-FrontendConformanceGuard "frontend integration closeout closeout_gate metadata missing in $ArtifactPath"
  }
  if (-not [bool]$closeoutGate.build_integration_gate_signoff) {
    Stop-FrontendConformanceGuard "frontend integration closeout build_integration_gate_signoff must be true in $ArtifactPath"
  }
  if (-not [bool]$closeoutGate.invocation_profile_gate_signoff) {
    Stop-FrontendConformanceGuard "frontend integration closeout invocation_profile_gate_signoff must be true in $ArtifactPath"
  }
  if (-not [bool]$closeoutGate.corpus_coverage_gate_signoff) {
    Stop-FrontendConformanceGuard "frontend integration closeout corpus_coverage_gate_signoff must be true in $ArtifactPath"
  }
  if ([int]$closeoutGate.deterministic_fail_closed_exit_code -ne $config.deterministic_fail_closed_exit_code) {
    Stop-FrontendConformanceGuard "frontend integration closeout deterministic_fail_closed_exit_code must be 2 in $ArtifactPath"
  }
  if ([int]$closeoutGate.acceptance_corpus_count -le 0 -or [int]$closeoutGate.rejection_corpus_count -le 0) {
    Stop-FrontendConformanceGuard "frontend integration closeout acceptance/rejection corpus counts must be positive in $ArtifactPath"
  }

  return [pscustomobject]@{
    integration_closeout_path = $ArtifactPath
  }
}

Export-ModuleMember -Function @(
  "Invoke-FrontendConformanceCorpusEvaluation",
  "Invoke-FrontendConformanceMatrixEvaluation",
  "Invoke-FrontendIntegrationCloseoutEvaluation"
)
