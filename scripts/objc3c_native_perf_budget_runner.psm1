Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$objc3cNativePerfBudgetModules = @(
  "objc3c_native_perf_budget_helpers.psm1",
  "objc3c_native_perf_budget_policy.psm1",
  "objc3c_native_perf_budget_catalog.psm1",
  "objc3c_native_perf_budget_measurements.psm1",
  "objc3c_native_perf_budget_invocation.psm1",
  "objc3c_native_perf_budget_report.psm1"
)
foreach ($moduleName in $objc3cNativePerfBudgetModules) {
  Import-Module (Join-Path $PSScriptRoot $moduleName) -Force -DisableNameChecking
}

function Invoke-Objc3cNativePerfBudget {
  param(
    [string]$ScriptRoot,
    [Nullable[int]]$MaxElapsedMs,
    [string]$ExtraPositiveFixtureDirs,
    [switch]$EnforceTimingGate,
    [ref]$ExitCode
  )

  $Config = Resolve-Objc3cNativePerfBudgetConfig `
    -ScriptRoot $ScriptRoot `
    -MaxElapsedMs $MaxElapsedMs `
    -ExtraPositiveFixtureDirs $ExtraPositiveFixtureDirs `
    -EnforceTimingGate:$EnforceTimingGate.IsPresent

  New-Item -ItemType Directory -Force -Path $Config.run_dir | Out-Null

  $hadFatalError = $false
  $fatalErrorMessage = ""
  $buildExecuted = $false
  $buildElapsedMs = 0.0
  $fixtureSets = @()
  $dispatchFixtureCount = 0
  $results = @()
  $proofState = New-Objc3cNativePerfProofState
  $cacheProof = $proofState.cache_proof
  $cacheInvalidationProof = $proofState.cache_invalidation_proof
  $macroHostProof = $proofState.macro_host_proof
  $docsGenerationProof = $proofState.docs_generation_proof
  $resolvedMaxElapsedMs = $Config.resolved_max_elapsed_ms
  $directFixtures = @()
  $directDispatchFixturePathSet = $null
  $cacheFixture = $null
  $cacheFixtureRel = ""

  Push-Location $Config.repo_root
  try {
    $compilerPaths = Ensure-Objc3cNativePerfCompilerAvailable `
      -Config $Config `
      -BuildExecuted ([ref]$buildExecuted) `
      -BuildElapsedMs ([ref]$buildElapsedMs)

    Invoke-Objc3cNativePerfDirectCompiles `
      -Config $Config `
      -CompilerExe $compilerPaths.exe `
      -ResolvedMaxElapsedMs ([ref]$resolvedMaxElapsedMs) `
      -FixtureSets ([ref]$fixtureSets) `
      -DispatchFixtureCount ([ref]$dispatchFixtureCount) `
      -Results ([ref]$results) `
      -Fixtures ([ref]$directFixtures) `
      -DispatchFixturePathSet ([ref]$directDispatchFixturePathSet)

    Invoke-Objc3cNativePerfCacheProof `
      -Config $Config `
      -Fixtures $directFixtures `
      -DispatchFixturePathSet $directDispatchFixturePathSet `
      -CompileScript $compilerPaths.compile_script `
      -CacheProof ([ref]$cacheProof) `
      -CacheFixture ([ref]$cacheFixture) `
      -CacheFixtureRel ([ref]$cacheFixtureRel)

    Invoke-Objc3cNativePerfCacheInvalidationProof `
      -Config $Config `
      -CacheFixture $cacheFixture `
      -CacheFixtureRel $cacheFixtureRel `
      -CompileScript $compilerPaths.compile_script `
      -CacheInvalidationProof ([ref]$cacheInvalidationProof)

    Invoke-Objc3cNativePerfMacroHostProof `
      -Config $Config `
      -CompileScript $compilerPaths.compile_script `
      -MacroHostProof ([ref]$macroHostProof)

    Invoke-Objc3cNativePerfDocsGenerationProof -Config $Config -DocsGenerationProof ([ref]$docsGenerationProof)
  } catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalErrorMessage)
  } finally {
    Pop-Location
  }

  $status = "FAIL"
  Write-Objc3cNativePerfSummary `
    -Config $Config `
    -FixtureSets $fixtureSets `
    -DispatchFixtureCount $dispatchFixtureCount `
    -Results $results `
    -ResolvedMaxElapsedMs $resolvedMaxElapsedMs `
    -BuildExecuted $buildExecuted `
    -BuildElapsedMs $buildElapsedMs `
    -CacheProof $cacheProof `
    -CacheInvalidationProof $cacheInvalidationProof `
    -MacroHostProof $macroHostProof `
    -DocsGenerationProof $docsGenerationProof `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage `
    -Status ([ref]$status)

  if ($status -ne "PASS") {
    $ExitCode.Value = 1
  } else {
    $ExitCode.Value = 0
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativePerfBudget"
)
