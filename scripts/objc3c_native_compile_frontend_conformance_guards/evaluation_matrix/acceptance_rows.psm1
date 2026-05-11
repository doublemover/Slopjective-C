$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
