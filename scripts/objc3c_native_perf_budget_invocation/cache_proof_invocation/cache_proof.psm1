Set-StrictMode -Version Latest

function Invoke-Objc3cNativePerfWrapperCacheProof {
  param(
    [object]$Config,
    [object[]]$Fixtures,
    [object]$DispatchFixturePathSet,
    [string]$CompileScript,
    [ref]$CacheProof,
    [ref]$CacheFixture,
    [ref]$CacheFixtureRel
  )

  $cacheFixture = @($Fixtures | Where-Object { $_.Extension -eq ".objc3" } | Select-Object -First 1)[0]
  if ($null -eq $cacheFixture) {
    throw "perf-budget FAIL: cache-proof requires at least one .objc3 fixture so the live compile-wrapper contract can publish the runtime registration manifest"
  }
  $cacheFixtureRel = Get-RepoRelativePath -Path $cacheFixture.FullName -Root $Config.repo_root
  $cacheFixtureKind = if ($DispatchFixturePathSet.Contains($cacheFixture.FullName)) { "dispatch-positive" } else { "recovery-positive" }
  $emitPrefix = "module"
  $cacheDir = Join-Path $Config.run_dir "cache-proof"
  $missDir = Join-Path $cacheDir "miss"
  $hitDir = Join-Path $cacheDir "hit"
  $cacheFixtureSource = Copy-Objc3cNativePerfProofFixture -Fixture $cacheFixture -Directory $cacheDir

  $run1Log = Join-Path $cacheDir "run1.log"
  $run2Log = Join-Path $cacheDir "run2.log"
  $run1 = Invoke-Objc3cNativePerfWrapperCacheRun `
    -CompileScript $CompileScript `
    -SourcePath $cacheFixtureSource `
    -OutputDirectory $missDir `
    -Extension $cacheFixture.Extension `
    -LogPath $run1Log
  if ($run1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-proof run1 failed with exit code $($run1.exit_code)"
  }
  $run1Hit = Read-Objc3cNativePerfInvocationCacheHitFlag -OutputText $run1.output_text -RunLabel "cache-proof run1"
  if ($run1Hit) {
    throw "perf-budget FAIL: cache-proof run1 expected cache_hit=false, observed true"
  }

  $run2 = Invoke-Objc3cNativePerfWrapperCacheRun `
    -CompileScript $CompileScript `
    -SourcePath $cacheFixtureSource `
    -OutputDirectory $hitDir `
    -Extension $cacheFixture.Extension `
    -LogPath $run2Log
  if ($run2.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-proof run2 failed with exit code $($run2.exit_code)"
  }
  $run2Hit = Read-Objc3cNativePerfInvocationCacheHitFlag -OutputText $run2.output_text -RunLabel "cache-proof run2"
  if (!$run2Hit) {
    throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"
  }

  $artifactNames = Get-Objc3cNativePerfCacheArtifactNames -Extension $cacheFixture.Extension
  $missHashes = Get-Objc3cNativePerfInvocationArtifactHashSet -Directory $missDir -ArtifactNames $artifactNames
  $hitHashes = Get-Objc3cNativePerfInvocationArtifactHashSet -Directory $hitDir -ArtifactNames $artifactNames
  Assert-Objc3cNativePerfCacheArtifactsMatch -MissHashes $missHashes -HitHashes $hitHashes -ArtifactNames $artifactNames
  Assert-Objc3cNativePerfObjectArtifactNonEmpty -ObjectPath (Join-Path $missDir "$emitPrefix.obj")

  $proof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "run1_hit=false run2_hit=true artifact_hashes_match=true"
    fixture = $cacheFixtureRel
    fixture_kind = $cacheFixtureKind
    emit_prefix = $emitPrefix
    run1 = [ordered]@{
      elapsed_ms = $run1.elapsed_ms
      exit_code = $run1.exit_code
      cache_hit = $run1Hit
      log = (Get-RepoRelativePath -Path $run1Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $missDir -Root $Config.repo_root)
    }
    run2 = [ordered]@{
      elapsed_ms = $run2.elapsed_ms
      exit_code = $run2.exit_code
      cache_hit = $run2Hit
      log = (Get-RepoRelativePath -Path $run2Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $hitDir -Root $Config.repo_root)
    }
    artifacts = [ordered]@{
      miss_sha256 = $missHashes
      hit_sha256 = $hitHashes
    }
  }
  Write-Objc3cNativePerfCacheProofLine -FixtureRel $cacheFixtureRel -FixtureKind $cacheFixtureKind -Run1Hit $run1Hit -Run2Hit $run2Hit

  $CacheProof.Value = $proof
  $CacheFixture.Value = $cacheFixture
  $CacheFixtureRel.Value = $cacheFixtureRel
}
