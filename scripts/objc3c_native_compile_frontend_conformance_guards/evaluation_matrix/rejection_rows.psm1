$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
