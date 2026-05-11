$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
