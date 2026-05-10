Set-StrictMode -Version Latest

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
