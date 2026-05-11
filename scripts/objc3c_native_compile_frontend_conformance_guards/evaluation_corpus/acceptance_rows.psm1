$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
