Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "budget_result_parsing.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "command_construction.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "process_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

function Copy-Objc3cNativePerfProofFixture {
  param(
    [object]$Fixture,
    [string]$Directory
  )

  New-Item -ItemType Directory -Force -Path $Directory | Out-Null
  $fixtureSource = Join-Path $Directory $Fixture.Name
  Copy-Item -LiteralPath $Fixture.FullName -Destination $fixtureSource -Force
  return $fixtureSource
}

function Invoke-Objc3cNativePerfWrapperCacheRun {
  param(
    [string]$CompileScript,
    [string]$SourcePath,
    [string]$OutputDirectory,
    [string]$Extension,
    [string]$LogPath,
    [switch]$ForceClangObjectBackend
  )

  $scriptArgs = New-Objc3cNativePerfWrapperCompileArguments `
    -SourcePath $SourcePath `
    -UseCache `
    -OutputDirectory $OutputDirectory `
    -Extension $Extension `
    -ForceClangObjectBackend:$ForceClangObjectBackend.IsPresent

  return (Invoke-Objc3cNativePerfWrapperProcess `
      -ScriptPath $CompileScript `
      -ScriptArguments $scriptArgs `
      -LogPath $LogPath)
}

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

function Invoke-Objc3cNativePerfWrapperMacroHostProof {
  param(
    [object]$Config,
    [string]$CompileScript,
    [ref]$MacroHostProof
  )

  if (!(Test-Path -LiteralPath $Config.macro_host_fixture -PathType Leaf)) {
    throw "perf-budget FAIL: missing macro-host fixture at $($Config.macro_host_fixture)"
  }
  $macroHostDir = Join-Path $Config.run_dir "macro-host-cache"
  $macroHostRun1Dir = Join-Path $macroHostDir "run1"
  $macroHostRun2Dir = Join-Path $macroHostDir "run2"
  $macroHostRun1Log = Join-Path $macroHostDir "run1.log"
  $macroHostRun2Log = Join-Path $macroHostDir "run2.log"
  New-Item -ItemType Directory -Force -Path $macroHostDir | Out-Null
  $macroHostFixtureSource = Join-Path $macroHostDir (Split-Path -Leaf $Config.macro_host_fixture)
  Copy-Item -LiteralPath $Config.macro_host_fixture -Destination $macroHostFixtureSource -Force

  $macroHostRun1 = Invoke-Objc3cNativePerfWrapperCacheRun `
    -CompileScript $CompileScript `
    -SourcePath $macroHostFixtureSource `
    -OutputDirectory $macroHostRun1Dir `
    -Extension ".objc3" `
    -LogPath $macroHostRun1Log `
    -ForceClangObjectBackend
  if ($macroHostRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: macro-host cache run1 failed with exit code $($macroHostRun1.exit_code)"
  }
  $macroHostRun1Hit = Read-Objc3cNativePerfInvocationCacheHitFlag -OutputText $macroHostRun1.output_text -RunLabel "macro-host cache run1"

  $macroHostRun2 = Invoke-Objc3cNativePerfWrapperCacheRun `
    -CompileScript $CompileScript `
    -SourcePath $macroHostFixtureSource `
    -OutputDirectory $macroHostRun2Dir `
    -Extension ".objc3" `
    -LogPath $macroHostRun2Log `
    -ForceClangObjectBackend
  if ($macroHostRun2.exit_code -ne 0) {
    throw "perf-budget FAIL: macro-host cache run2 failed with exit code $($macroHostRun2.exit_code)"
  }
  $macroHostRun2Hit = Read-Objc3cNativePerfInvocationCacheHitFlag -OutputText $macroHostRun2.output_text -RunLabel "macro-host cache run2"
  if (!$macroHostRun2Hit) {
    throw "perf-budget FAIL: macro-host cache run2 expected cache_hit=true"
  }

  $macroHostArtifact = Join-Path $macroHostRun2Dir "module.metaprogramming-macro-host-cache.json"
  if (!(Test-Path -LiteralPath $macroHostArtifact -PathType Leaf)) {
    throw "perf-budget FAIL: macro-host cache compile did not publish a metaprogramming host-cache artifact"
  }

  $macroHostProof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "macro-host cache artifact published through the live wrapper path"
    fixture = (Get-RepoRelativePath -Path $Config.macro_host_fixture -Root $Config.repo_root)
    run1 = [ordered]@{
      elapsed_ms = $macroHostRun1.elapsed_ms
      exit_code = $macroHostRun1.exit_code
      cache_hit = $macroHostRun1Hit
      log = (Get-RepoRelativePath -Path $macroHostRun1Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $macroHostRun1Dir -Root $Config.repo_root)
    }
    run2 = [ordered]@{
      elapsed_ms = $macroHostRun2.elapsed_ms
      exit_code = $macroHostRun2.exit_code
      cache_hit = $macroHostRun2Hit
      log = (Get-RepoRelativePath -Path $macroHostRun2Log -Root $Config.repo_root)
      out_dir = (Get-RepoRelativePath -Path $macroHostRun2Dir -Root $Config.repo_root)
    }
    cache_artifact = (Get-RepoRelativePath -Path $macroHostArtifact -Root $Config.repo_root)
  }
  Write-Objc3cNativePerfMacroHostLine -FixtureRel $macroHostProof.fixture -Run1Hit $macroHostRun1Hit -Run2Hit $macroHostRun2Hit

  $MacroHostProof.Value = $macroHostProof
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativePerfWrapperCacheInvalidationProof",
  "Invoke-Objc3cNativePerfWrapperCacheProof",
  "Invoke-Objc3cNativePerfWrapperMacroHostProof"
)
