Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_perf_budget_measurements.psm1") -Force -DisableNameChecking

function Ensure-Objc3cNativePerfCompilerAvailable {
  param(
    [object]$Config,
    [ref]$BuildExecuted,
    [ref]$BuildElapsedMs
  )

  $exe = Join-Path $Config.repo_root "artifacts/bin/objc3c-native.exe"
  $compileScript = Join-Path $Config.repo_root "scripts/objc3c_native_compile.ps1"
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    $BuildExecuted.Value = $true
    $buildScript = Join-Path $Config.repo_root "scripts/build_objc3c_native.ps1"
    $buildLog = Join-Path $Config.run_dir "build.log"
    $buildRun = Invoke-TimedNativeCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript) `
      -LogPath $buildLog
    $BuildElapsedMs.Value = $buildRun.elapsed_ms
    if ($buildRun.exit_code -ne 0) {
      throw "perf-budget FAIL: native compiler build failed with exit code $($buildRun.exit_code)"
    }
  }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "perf-budget FAIL: native compiler executable missing at $exe"
  }
  if (!(Test-Path -LiteralPath $compileScript -PathType Leaf)) {
    throw "perf-budget FAIL: missing compile wrapper at $compileScript"
  }

  return [pscustomobject]@{
    exe = $exe
    compile_script = $compileScript
  }
}

function Invoke-Objc3cNativePerfDirectCompiles {
  param(
    [object]$Config,
    [string]$CompilerExe,
    [ref]$ResolvedMaxElapsedMs,
    [ref]$FixtureSets,
    [ref]$DispatchFixtureCount,
    [ref]$Results,
    [ref]$Fixtures,
    [ref]$DispatchFixturePathSet
  )

  $benchmarkCatalog = Resolve-Objc3cNativePerfBenchmarkCatalog -Config $Config -ResolvedMaxElapsedMs $ResolvedMaxElapsedMs
  $fixtures = @($benchmarkCatalog.fixtures)
  $dispatchFixturePathSet = $benchmarkCatalog.dispatch_fixture_path_set
  $FixtureSets.Value = @($benchmarkCatalog.fixture_sets)
  $DispatchFixtureCount.Value = [int]$benchmarkCatalog.dispatch_fixture_count

  foreach ($fixtureSet in $FixtureSets.Value) {
    Write-Output ("fixture-set: kind={0} root={1} count={2}" -f $fixtureSet.fixture_kind, $fixtureSet.fixture_root, $fixtureSet.fixture_count)
  }

  foreach ($fixture in $fixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $Config.repo_root
    $hash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $Config.run_dir ("fixture_{0}" -f $hash)
    $compileLog = Join-Path $caseDir "compile.log"
    New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

    $compileArgs = @($fixture.FullName, "--out-dir", $caseDir, "--emit-prefix", "module")
    if ($fixture.Extension -eq ".objc3") {
      # Perf-budget is a throughput regression gate, not a backend-availability gate.
      # Force .objc3 perf samples onto clang object backend to avoid llc availability skew.
      $compileArgs += @("--objc3-ir-object-backend", "clang")
    }
    $run = Invoke-TimedNativeCommand `
      -Command $CompilerExe `
      -Arguments $compileArgs `
      -LogPath $compileLog

    $compileResult = New-Objc3cNativePerfCompileResult -Config $Config -Fixture $fixture -CaseDir $caseDir -Run $run
    $Results.Value += $compileResult.result
    Write-Output ("[{0}] {1} elapsed_ms={2} ({3})" -f $compileResult.status_token, $compileResult.fixture_rel, $run.elapsed_ms, $compileResult.detail)
  }

  $Fixtures.Value = $fixtures
  $DispatchFixturePathSet.Value = $dispatchFixturePathSet
}

function Invoke-Objc3cNativePerfCacheProof {
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
  New-Item -ItemType Directory -Force -Path $cacheDir | Out-Null
  $cacheFixtureSource = Join-Path $cacheDir $cacheFixture.Name
  Copy-Item -LiteralPath $cacheFixture.FullName -Destination $cacheFixtureSource -Force

  $run1Log = Join-Path $cacheDir "run1.log"
  $run2Log = Join-Path $cacheDir "run2.log"
  $cacheScriptArgs = @($cacheFixtureSource, "--use-cache", "--out-dir", $missDir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $cacheScriptArgs += @("--objc3-ir-object-backend", "clang")
  }
  $run1 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $cacheScriptArgs `
    -LogPath $run1Log
  if ($run1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-proof run1 failed with exit code $($run1.exit_code)"
  }
  $run1Hit = Read-Objc3cNativePerfCacheHitFlag -OutputText $run1.output_text -RunLabel "cache-proof run1"
  if ($run1Hit) {
    throw "perf-budget FAIL: cache-proof run1 expected cache_hit=false, observed true"
  }

  $cacheScriptArgsHit = @($cacheFixtureSource, "--use-cache", "--out-dir", $hitDir)
  if ($cacheFixture.Extension -eq ".objc3") {
    $cacheScriptArgsHit += @("--objc3-ir-object-backend", "clang")
  }
  $run2 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $cacheScriptArgsHit `
    -LogPath $run2Log
  if ($run2.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-proof run2 failed with exit code $($run2.exit_code)"
  }
  $run2Hit = Read-Objc3cNativePerfCacheHitFlag -OutputText $run2.output_text -RunLabel "cache-proof run2"
  if (!$run2Hit) {
    throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"
  }

  $artifactNames = @(
    "$emitPrefix.obj",
    "$emitPrefix.manifest.json",
    "$emitPrefix.diagnostics.txt"
  )
  if ($cacheFixture.Extension -eq ".objc3") {
    $artifactNames += "$emitPrefix.ll"
  }

  $missHashes = Get-Objc3cNativePerfArtifactHashSet -Directory $missDir -ArtifactNames $artifactNames
  $hitHashes = Get-Objc3cNativePerfArtifactHashSet -Directory $hitDir -ArtifactNames $artifactNames
  foreach ($name in $artifactNames) {
    if ($missHashes[$name] -ne $hitHashes[$name]) {
      throw "perf-budget FAIL: cache-proof artifact hash drift for $name"
    }
  }

  $objPath = Join-Path $missDir "$emitPrefix.obj"
  $objSize = (Get-Item -LiteralPath $objPath).Length
  if ($objSize -le 0) {
    throw "perf-budget FAIL: cache-proof produced empty object artifact"
  }

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
  Write-Output ("cache-proof PASS fixture={0} fixture_kind={1} run1_hit={2} run2_hit={3}" -f $cacheFixtureRel, $cacheFixtureKind, $run1Hit, $run2Hit)

  $CacheProof.Value = $proof
  $CacheFixture.Value = $cacheFixture
  $CacheFixtureRel.Value = $cacheFixtureRel
}

function Invoke-Objc3cNativePerfCacheInvalidationProof {
  param(
    [object]$Config,
    [object]$CacheFixture,
    [string]$CacheFixtureRel,
    [string]$CompileScript,
    [ref]$CacheInvalidationProof
  )

  $invalidationDir = Join-Path $Config.run_dir "cache-invalidation"
  $invalidationSource = Join-Path $invalidationDir $CacheFixture.Name
  New-Item -ItemType Directory -Force -Path $invalidationDir | Out-Null
  Copy-Item -LiteralPath $CacheFixture.FullName -Destination $invalidationSource -Force

  $invalidationRun1Dir = Join-Path $invalidationDir "run1"
  $invalidationRun2Dir = Join-Path $invalidationDir "run2"
  $invalidationRun1Log = Join-Path $invalidationDir "run1.log"
  $invalidationRun2Log = Join-Path $invalidationDir "run2.log"

  $invalidationArgs = @($invalidationSource, "--use-cache", "--out-dir", $invalidationRun1Dir)
  if ($CacheFixture.Extension -eq ".objc3") {
    $invalidationArgs += @("--objc3-ir-object-backend", "clang")
  }
  $invalidationRun1 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $invalidationArgs `
    -LogPath $invalidationRun1Log
  if ($invalidationRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run1 failed with exit code $($invalidationRun1.exit_code)"
  }
  $invalidationRun1Hit = Read-Objc3cNativePerfCacheHitFlag -OutputText $invalidationRun1.output_text -RunLabel "cache-invalidation run1"

  Add-Content -LiteralPath $invalidationSource -Value "// cache invalidation probe mutation"

  $invalidationArgs2 = @($invalidationSource, "--use-cache", "--out-dir", $invalidationRun2Dir)
  if ($CacheFixture.Extension -eq ".objc3") {
    $invalidationArgs2 += @("--objc3-ir-object-backend", "clang")
  }
  $invalidationRun2 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $invalidationArgs2 `
    -LogPath $invalidationRun2Log
  if ($invalidationRun2.exit_code -ne 0) {
    throw "perf-budget FAIL: cache-invalidation run2 failed with exit code $($invalidationRun2.exit_code)"
  }
  $invalidationRun2Hit = Read-Objc3cNativePerfCacheHitFlag -OutputText $invalidationRun2.output_text -RunLabel "cache-invalidation run2"
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
  Write-Output ("cache-invalidation PASS fixture={0} run1_hit={1} run2_hit={2}" -f $CacheFixtureRel, $invalidationRun1Hit, $invalidationRun2Hit)

  $CacheInvalidationProof.Value = $cacheInvalidationProof
}

function Invoke-Objc3cNativePerfMacroHostProof {
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
  $macroEmitPrefix = "module"

  $macroHostArgs = @($macroHostFixtureSource, "--use-cache", "--out-dir", $macroHostRun1Dir, "--objc3-ir-object-backend", "clang")
  $macroHostRun1 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $macroHostArgs `
    -LogPath $macroHostRun1Log
  if ($macroHostRun1.exit_code -ne 0) {
    throw "perf-budget FAIL: macro-host cache run1 failed with exit code $($macroHostRun1.exit_code)"
  }
  $macroHostRun1Hit = Read-Objc3cNativePerfCacheHitFlag -OutputText $macroHostRun1.output_text -RunLabel "macro-host cache run1"

  $macroHostArgs2 = @($macroHostFixtureSource, "--use-cache", "--out-dir", $macroHostRun2Dir, "--objc3-ir-object-backend", "clang")
  $macroHostRun2 = Invoke-TimedWrapperCommand `
    -ScriptPath $CompileScript `
    -ScriptArguments $macroHostArgs2 `
    -LogPath $macroHostRun2Log
  if ($macroHostRun2.exit_code -ne 0) {
    throw "perf-budget FAIL: macro-host cache run2 failed with exit code $($macroHostRun2.exit_code)"
  }
  $macroHostRun2Hit = Read-Objc3cNativePerfCacheHitFlag -OutputText $macroHostRun2.output_text -RunLabel "macro-host cache run2"
  if (!$macroHostRun2Hit) {
    throw "perf-budget FAIL: macro-host cache run2 expected cache_hit=true"
  }

  $macroHostArtifact = Join-Path $macroHostRun2Dir "$macroEmitPrefix.metaprogramming-macro-host-cache.json"
  if (!(Test-Path -LiteralPath $macroHostArtifact -PathType Leaf)) {
    $macroHostArtifact = Join-Path $macroHostRun2Dir "module.metaprogramming-macro-host-cache.json"
  }
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
  Write-Output ("macro-host-cache PASS fixture={0} run1_hit={1} run2_hit={2}" -f $macroHostProof.fixture, $macroHostRun1Hit, $macroHostRun2Hit)

  $MacroHostProof.Value = $macroHostProof
}

function Invoke-Objc3cNativePerfDocsGenerationProof {
  param(
    [object]$Config,
    [ref]$DocsGenerationProof
  )

  if (!(Test-Path -LiteralPath $Config.native_docs_script -PathType Leaf)) {
    throw "perf-budget FAIL: missing native docs generator at $($Config.native_docs_script)"
  }
  if (!(Test-Path -LiteralPath $Config.command_surface_script -PathType Leaf)) {
    throw "perf-budget FAIL: missing public command surface generator at $($Config.command_surface_script)"
  }
  $docsDir = Join-Path $Config.run_dir "docs-generation"
  New-Item -ItemType Directory -Force -Path $docsDir | Out-Null
  $nativeDocsLog = Join-Path $docsDir "native-docs.log"
  $commandSurfaceLog = Join-Path $docsDir "public-command-surface.log"
  $nativeDocsRun = Invoke-TimedNativeCommand `
    -Command $Config.python_command `
    -Arguments @($Config.native_docs_script) `
    -LogPath $nativeDocsLog
  if ($nativeDocsRun.exit_code -ne 0) {
    throw "perf-budget FAIL: native docs generation failed with exit code $($nativeDocsRun.exit_code)"
  }
  $commandSurfaceRun = Invoke-TimedNativeCommand `
    -Command $Config.python_command `
    -Arguments @($Config.command_surface_script) `
    -LogPath $commandSurfaceLog
  if ($commandSurfaceRun.exit_code -ne 0) {
    throw "perf-budget FAIL: public command surface generation failed with exit code $($commandSurfaceRun.exit_code)"
  }

  $docsGenerationProof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "checked-in docs generators executed on the live repo surface"
    native_docs = [ordered]@{
      elapsed_ms = $nativeDocsRun.elapsed_ms
      exit_code = $nativeDocsRun.exit_code
      log = (Get-RepoRelativePath -Path $nativeDocsLog -Root $Config.repo_root)
    }
    public_command_surface = [ordered]@{
      elapsed_ms = $commandSurfaceRun.elapsed_ms
      exit_code = $commandSurfaceRun.exit_code
      log = (Get-RepoRelativePath -Path $commandSurfaceLog -Root $Config.repo_root)
    }
  }
  Write-Output ("docs-generation PASS native_docs_ms={0} command_surface_ms={1}" -f $nativeDocsRun.elapsed_ms, $commandSurfaceRun.elapsed_ms)

  $DocsGenerationProof.Value = $docsGenerationProof
}

Export-ModuleMember -Function @(
  "Ensure-Objc3cNativePerfCompilerAvailable",
  "Invoke-Objc3cNativePerfCacheInvalidationProof",
  "Invoke-Objc3cNativePerfCacheProof",
  "Invoke-Objc3cNativePerfDirectCompiles",
  "Invoke-Objc3cNativePerfDocsGenerationProof",
  "Invoke-Objc3cNativePerfMacroHostProof"
)
