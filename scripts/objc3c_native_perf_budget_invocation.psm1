Set-StrictMode -Version Latest

$objc3cNativePerfBudgetInvocationModuleRoot = Join-Path $PSScriptRoot "objc3c_native_perf_budget_invocation"
$objc3cNativePerfBudgetInvocationModules = @(
  "process_invocation.psm1",
  "command_construction.psm1",
  "budget_result_parsing.psm1",
  "report_rendering.psm1",
  "configuration_env_resolution.psm1",
  "direct_compile_invocation.psm1",
  "cache_proof_invocation.psm1",
  "docs_generation_invocation.psm1"
)

foreach ($moduleName in $objc3cNativePerfBudgetInvocationModules) {
  Import-Module (Join-Path $objc3cNativePerfBudgetInvocationModuleRoot $moduleName) -Force -DisableNameChecking
}

function Ensure-Objc3cNativePerfCompilerAvailable {
  param(
    [object]$Config,
    [ref]$BuildExecuted,
    [ref]$BuildElapsedMs
  )

  return (Resolve-Objc3cNativePerfCompilerAvailability `
      -Config $Config `
      -BuildExecuted $BuildExecuted `
      -BuildElapsedMs $BuildElapsedMs)
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

  $resolvedMaxElapsedMsValue = [int]$ResolvedMaxElapsedMs.Value
  $fixtureSetsValue = @($FixtureSets.Value)
  $dispatchFixtureCountValue = [int]$DispatchFixtureCount.Value
  $resultsValue = @($Results.Value)
  $fixturesValue = @($Fixtures.Value)
  $dispatchFixturePathSetValue = $DispatchFixturePathSet.Value

  Invoke-Objc3cNativePerfDirectCompileSet `
    -Config $Config `
    -CompilerExe $CompilerExe `
    -ResolvedMaxElapsedMs ([ref]$resolvedMaxElapsedMsValue) `
    -FixtureSets ([ref]$fixtureSetsValue) `
    -DispatchFixtureCount ([ref]$dispatchFixtureCountValue) `
    -Results ([ref]$resultsValue) `
    -Fixtures ([ref]$fixturesValue) `
    -DispatchFixturePathSet ([ref]$dispatchFixturePathSetValue)

  $ResolvedMaxElapsedMs.Value = $resolvedMaxElapsedMsValue
  $FixtureSets.Value = $fixtureSetsValue
  $DispatchFixtureCount.Value = $dispatchFixtureCountValue
  $Results.Value = $resultsValue
  $Fixtures.Value = $fixturesValue
  $DispatchFixturePathSet.Value = $dispatchFixturePathSetValue
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

  $cacheProofValue = $CacheProof.Value
  $cacheFixtureValue = $CacheFixture.Value
  $cacheFixtureRelValue = [string]$CacheFixtureRel.Value

  Invoke-Objc3cNativePerfWrapperCacheProof `
    -Config $Config `
    -Fixtures $Fixtures `
    -DispatchFixturePathSet $DispatchFixturePathSet `
    -CompileScript $CompileScript `
    -CacheProof ([ref]$cacheProofValue) `
    -CacheFixture ([ref]$cacheFixtureValue) `
    -CacheFixtureRel ([ref]$cacheFixtureRelValue)

  $CacheProof.Value = $cacheProofValue
  $CacheFixture.Value = $cacheFixtureValue
  $CacheFixtureRel.Value = $cacheFixtureRelValue
}

function Invoke-Objc3cNativePerfCacheInvalidationProof {
  param(
    [object]$Config,
    [object]$CacheFixture,
    [string]$CacheFixtureRel,
    [string]$CompileScript,
    [ref]$CacheInvalidationProof
  )

  $cacheInvalidationProofValue = $CacheInvalidationProof.Value

  Invoke-Objc3cNativePerfWrapperCacheInvalidationProof `
    -Config $Config `
    -CacheFixture $CacheFixture `
    -CacheFixtureRel $CacheFixtureRel `
    -CompileScript $CompileScript `
    -CacheInvalidationProof ([ref]$cacheInvalidationProofValue)

  $CacheInvalidationProof.Value = $cacheInvalidationProofValue
}

function Invoke-Objc3cNativePerfMacroHostProof {
  param(
    [object]$Config,
    [string]$CompileScript,
    [ref]$MacroHostProof
  )

  $macroHostProofValue = $MacroHostProof.Value

  Invoke-Objc3cNativePerfWrapperMacroHostProof `
    -Config $Config `
    -CompileScript $CompileScript `
    -MacroHostProof ([ref]$macroHostProofValue)

  $MacroHostProof.Value = $macroHostProofValue
}

function Invoke-Objc3cNativePerfDocsGenerationProof {
  param(
    [object]$Config,
    [ref]$DocsGenerationProof
  )

  $docsGenerationProofValue = $DocsGenerationProof.Value

  Invoke-Objc3cNativePerfDocsGeneratorProof `
    -Config $Config `
    -DocsGenerationProof ([ref]$docsGenerationProofValue)

  $DocsGenerationProof.Value = $docsGenerationProofValue
}

Export-ModuleMember -Function @(
  "Ensure-Objc3cNativePerfCompilerAvailable",
  "Invoke-Objc3cNativePerfCacheInvalidationProof",
  "Invoke-Objc3cNativePerfCacheProof",
  "Invoke-Objc3cNativePerfDirectCompiles",
  "Invoke-Objc3cNativePerfDocsGenerationProof",
  "Invoke-Objc3cNativePerfMacroHostProof"
)
