Set-StrictMode -Version Latest

function Invoke-Objc3cNativePerfWrapperCacheInvalidationProof {
  param(
    [object]$Config,
    [object]$CacheFixture,
    [string]$CacheFixtureRel,
    [string]$CompileScript,
    [ref]$CacheInvalidationProof
  )

  $invalidationDir = Join-Path $Config.run_dir "cache-invalidation"
  $invalidationSource = Copy-Objc3cNativePerfProofFixture -Fixture $CacheFixture -Directory $invalidationDir

  $invalidationRun1Dir = Join-Path $invalidationDir "run1"
  $invalidationRun2Dir = Join-Path $invalidationDir "run2"
  $invalidationRun1Log = Join-Path $invalidationDir "run1.log"
  $invalidationRun2Log = Join-Path $invalidationDir "run2.log"

  $invalidationRun1 = Invoke-Objc3cNativePerfWrapperCacheRun `
    -CompileScript $CompileScript `
    -SourcePath $invalidationSource `
    -OutputDirectory $invalidationRun1Dir `
    -Extension $CacheFixture.Extension `
    -LogPath $invalidationRun1Log
  if ($invalidationRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run1 failed with exit code $($invalidationRun1.exit_code)"
  }
  $invalidationRun1Hit = Read-Objc3cNativePerfInvocationCacheHitFlag -OutputText $invalidationRun1.output_text -RunLabel "cache-invalidation run1"

  Add-Content -LiteralPath $invalidationSource -Value "// cache invalidation probe mutation"

  $invalidationRun2 = Invoke-Objc3cNativePerfWrapperCacheRun `
    -CompileScript $CompileScript `
    -SourcePath $invalidationSource `
    -OutputDirectory $invalidationRun2Dir `
    -Extension $CacheFixture.Extension `
    -LogPath $invalidationRun2Log
  if ($invalidationRun2.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run2 failed with exit code $($invalidationRun2.exit_code)"
  }
  $invalidationRun2Hit = Read-Objc3cNativePerfInvocationCacheHitFlag -OutputText $invalidationRun2.output_text -RunLabel "cache-invalidation run2"
  if ($invalidationRun2Hit) {
    throw "perf-budget FAIL: cache-invalidation run2 expected cache_hit=false after source mutation"
  }

  $cacheInvalidationProof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "source mutation invalidated wrapper cache key"
    fixture = (Get-RepoRelativePath -Path $CacheFixture.FullName -Root $Config.repo_root)
    run1 = [ordered]@{
      elapsed_ms = $invalidationRun1.elapsed_ms
      exit_code = $invalidationRun1.exit_code
      cache_hit = $invalidationRun1Hit
      log = (Get-RepoRelativePath -Path $invalidationRun1Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $invalidationRun1Dir -Root $Config.repo_root)
    }
    run2 = [ordered]@{
      elapsed_ms = $invalidationRun2.elapsed_ms
      exit_code = $invalidationRun2.exit_code
      cache_hit = $invalidationRun2Hit
      log = (Get-RepoRelativePath -Path $invalidationRun2Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $invalidationRun2Dir -Root $Config.repo_root)
    }
  }
  Write-Objc3cNativePerfCacheInvalidationLine -FixtureRel $CacheFixtureRel -Run1Hit $invalidationRun1Hit -Run2Hit $invalidationRun2Hit

  $CacheInvalidationProof.Value = $cacheInvalidationProof
}
