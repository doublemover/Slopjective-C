Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "bucket_minima.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "family_coverage.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "replay_smoke.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "state.psm1") -Force -DisableNameChecking

function Invoke-ConformanceSuite {
  param([string]$RepoRoot)

  $state = New-ConformanceSuiteState

  Invoke-ConformanceBucketMinimaCheck -RepoRoot $RepoRoot -State $state
  Invoke-ConformanceFamilyCoverageCheck -State $state
  Invoke-ConformanceReplaySmokeCheck -RepoRoot $RepoRoot -State $state

  $failures = $state["Failures"]
  if ($failures.Count -gt 0) {
    Write-Output ("Conformance suite check failed with {0} issue(s):" -f $failures.Count)
    foreach ($failure in $failures) {
      Write-Output ("- " + $failure)
    }
    exit 1
  }

  Write-Output ("metadata_only_fixtures_excluded_from_minima: {0}" -f $state["MetadataOnlyFixtureCount"])
  Write-Output "Conformance suite check passed."
  $global:LASTEXITCODE = 0
  exit 0
}

Export-ModuleMember -Function "Invoke-ConformanceSuite"
