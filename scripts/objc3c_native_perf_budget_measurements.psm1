Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking

function New-Objc3cNativePerfCompileResult {
  param(
    [object]$Config,
    [object]$Fixture,
    [string]$CaseDir,
    [object]$Run
  )

  $fixtureRel = Get-RepoRelativePath -Path $Fixture.FullName -Root $Config.repo_root
  $objPath = Join-Path $CaseDir "module.obj"
  $objExists = Test-Path -LiteralPath $objPath -PathType Leaf
  $objSize = if ($objExists) { (Get-Item -LiteralPath $objPath).Length } else { 0 }
  $passed = ($Run.exit_code -eq 0) -and $objExists -and ($objSize -gt 0)
  $artifactSurface = if ($passed) {
    Get-CompileArtifactSurface -OutputDirectory $CaseDir -RepoRoot $Config.repo_root
  } else {
    [ordered]@{
      manifest_present = $false
    }
  }
  $detail = if ($passed) {
    "exit=0 obj_bytes=$objSize"
  } elseif ($Run.exit_code -ne 0) {
    "expected exit=0 got exit=$($Run.exit_code)"
  } elseif (!$objExists) {
    "missing module.obj"
  } else {
    "empty module.obj"
  }
  $statusToken = if ($passed) { "PASS" } else { "FAIL" }

  return [pscustomobject]@{
    fixture_rel = $fixtureRel
    status_token = $statusToken
    detail = $detail
    result = [pscustomobject]@{
      fixture = $fixtureRel
      elapsed_ms = $Run.elapsed_ms
      exit_code = $Run.exit_code
      passed = $passed
      detail = $detail
      out_dir = (Get-RepoRelativePath -Path $CaseDir -Root $Config.repo_root)
      compile_artifact_surface = $artifactSurface
    }
  }
}

function Read-Objc3cNativePerfCacheHitFlag {
  param(
    [string]$OutputText,
    [string]$RunLabel
  )

  return (Parse-CacheHitFlag -OutputText $OutputText -RunLabel $RunLabel)
}

function Get-Objc3cNativePerfArtifactHashSet {
  param(
    [string]$Directory,
    [string[]]$ArtifactNames
  )

  return (Get-ArtifactHashSet -Directory $Directory -ArtifactNames $ArtifactNames)
}

function Get-Objc3cNativePerfSummaryMeasurements {
  param(
    [object]$Config,
    [object[]]$Results,
    [int]$ResolvedMaxElapsedMs,
    [object]$CacheProof,
    [bool]$HadFatalError
  )

  $total = $Results.Count
  $passedCount = @($Results | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $elapsedRows = @($Results | ForEach-Object { [double]$_.elapsed_ms })
  $totalElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Sum).Sum), 3) } else { 0.0 }
  $minFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Minimum).Minimum), 3) } else { 0.0 }
  $maxFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round((($elapsedRows | Measure-Object -Maximum).Maximum), 3) } else { 0.0 }
  $avgFixtureElapsedMs = if ($elapsedRows.Count -gt 0) { [Math]::Round(($totalElapsedMs / $elapsedRows.Count), 3) } else { 0.0 }
  $budgetBreached = $totalElapsedMs -gt $ResolvedMaxElapsedMs
  $timingGateViolated = $Config.timing_gate_enforced -and $budgetBreached
  $budgetMarginMs = [Math]::Round(($ResolvedMaxElapsedMs - $totalElapsedMs), 3)
  $cacheProofPassed = $CacheProof.executed -and ($CacheProof.status -eq "PASS")
  $statusValue = if (!$HadFatalError -and $total -gt 0 -and $failedCount -eq 0 -and !$timingGateViolated -and $cacheProofPassed) { "PASS" } else { "FAIL" }

  return [pscustomobject]@{
    total = $total
    passed_count = $passedCount
    failed_count = $failedCount
    total_elapsed_ms = $totalElapsedMs
    min_fixture_elapsed_ms = $minFixtureElapsedMs
    max_fixture_elapsed_ms = $maxFixtureElapsedMs
    avg_fixture_elapsed_ms = $avgFixtureElapsedMs
    budget_breached = $budgetBreached
    timing_gate_violated = $timingGateViolated
    budget_margin_ms = $budgetMarginMs
    cache_proof_passed = $cacheProofPassed
    status = $statusValue
  }
}

function Get-Objc3cNativePerfPairElapsedMs {
  param([object]$Proof)

  if ($Proof.executed -and $null -ne $Proof.run1 -and $null -ne $Proof.run2) {
    return [Math]::Round(([double]$Proof.run1.elapsed_ms + [double]$Proof.run2.elapsed_ms), 3)
  }
  return 0.0
}

function Get-Objc3cNativePerfDocsElapsedMs {
  param([object]$DocsGenerationProof)

  if ($DocsGenerationProof.executed -and $null -ne $DocsGenerationProof.native_docs -and $null -ne $DocsGenerationProof.public_command_surface) {
    return [Math]::Round(([double]$DocsGenerationProof.native_docs.elapsed_ms + [double]$DocsGenerationProof.public_command_surface.elapsed_ms), 3)
  }
  return 0.0
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativePerfArtifactHashSet",
  "Get-Objc3cNativePerfDocsElapsedMs",
  "Get-Objc3cNativePerfPairElapsedMs",
  "Get-Objc3cNativePerfSummaryMeasurements",
  "New-Objc3cNativePerfCompileResult",
  "Read-Objc3cNativePerfCacheHitFlag"
)
