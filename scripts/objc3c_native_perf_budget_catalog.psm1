Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking

function New-Objc3cNativePerfProofState {
  $cacheProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    fixture = ""
    fixture_kind = ""
    emit_prefix = ""
    run1 = $null
    run2 = $null
    artifacts = [ordered]@{}
  }
  $cacheInvalidationProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    fixture = ""
    run1 = $null
    run2 = $null
  }
  $macroHostProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    fixture = ""
    run1 = $null
    run2 = $null
    cache_artifact = ""
  }
  $docsGenerationProof = [ordered]@{
    executed = $false
    status = "FAIL"
    detail = "not_executed"
    native_docs = $null
    public_command_surface = $null
  }

  return [pscustomobject]@{
    cache_proof = $cacheProof
    cache_invalidation_proof = $cacheInvalidationProof
    macro_host_proof = $macroHostProof
    docs_generation_proof = $docsGenerationProof
  }
}

function Resolve-Objc3cNativePerfBenchmarkCatalog {
  param(
    [object]$Config,
    [ref]$ResolvedMaxElapsedMs
  )

  $fixtureDirectories = Get-PerfFixtureDirectories `
    -RepoRoot $Config.repo_root `
    -BaselineDirectory $Config.positive_dir `
    -RequiredDispatchDirectory $Config.dispatch_required_dir `
    -DispatchCandidateDirectories $Config.dispatch_positive_candidate_dirs `
    -ExtraDirectoriesRaw $Config.resolved_extra_positive_fixture_dirs

  $fixtureSets = @()
  $fixtures = @()
  $dispatchFixturePathSet = New-Object "System.Collections.Generic.HashSet[string]" ([System.StringComparer]::OrdinalIgnoreCase)
  foreach ($fixtureDirectory in $fixtureDirectories) {
    $dirFixtures = Get-Fixtures -Directory $fixtureDirectory.directory -FixtureKind $fixtureDirectory.source -Extensions $fixtureDirectory.extensions
    $fixtureSets += [pscustomobject]@{
      fixture_root = Get-RepoRelativePath -Path $fixtureDirectory.directory -Root $Config.repo_root
      fixture_kind = $fixtureDirectory.fixture_kind
      fixture_count = $dirFixtures.Count
    }

    foreach ($fixture in $dirFixtures) {
      $fixtures += $fixture
      if ($fixtureDirectory.fixture_kind -eq "dispatch-positive") {
        $null = $dispatchFixturePathSet.Add($fixture.FullName)
      }
    }
  }

  $fixtures = @($fixtures | Sort-Object -Property FullName -Unique)
  if ($fixtures.Count -eq 0) {
    throw "perf-budget FAIL: no positive fixtures resolved from configured roots"
  }
  if (-not $Config.explicit_max_elapsed_ms) {
    $scaledBudget = [int][Math]::Ceiling($fixtures.Count * $Config.resolved_per_fixture_budget_ms)
    if ($scaledBudget -gt $ResolvedMaxElapsedMs.Value) {
      $ResolvedMaxElapsedMs.Value = $scaledBudget
    }
  }
  $dispatchFixtureCount = @($fixtures | Where-Object { $dispatchFixturePathSet.Contains($_.FullName) }).Count
  if ($dispatchFixtureCount -le 0) {
    throw "perf-budget FAIL: dispatch fixture suite resolved zero fixtures"
  }

  return [pscustomobject]@{
    fixture_sets = @($fixtureSets)
    fixtures = @($fixtures)
    dispatch_fixture_count = $dispatchFixtureCount
    dispatch_fixture_path_set = $dispatchFixturePathSet
  }
}

Export-ModuleMember -Function @(
  "New-Objc3cNativePerfProofState",
  "Resolve-Objc3cNativePerfBenchmarkCatalog"
)
