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

  Invoke-Objc3cNativePerfDirectCompileSet `
    -Config $Config `
    -CompilerExe $CompilerExe `
    -ResolvedMaxElapsedMs $ResolvedMaxElapsedMs `
    -FixtureSets $FixtureSets `
    -DispatchFixtureCount $DispatchFixtureCount `
    -Results $Results `
    -Fixtures $Fixtures `
    -DispatchFixturePathSet $DispatchFixturePathSet
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

  Invoke-Objc3cNativePerfWrapperCacheProof `
    -Config $Config `
    -Fixtures $Fixtures `
    -DispatchFixturePathSet $DispatchFixturePathSet `
    -CompileScript $CompileScript `
    -CacheProof $CacheProof `
    -CacheFixture $CacheFixture `
    -CacheFixtureRel $CacheFixtureRel
}

function Invoke-Objc3cNativePerfCacheInvalidationProof {
  param(
    [object]$Config,
    [object]$CacheFixture,
    [string]$CacheFixtureRel,
    [string]$CompileScript,
    [ref]$CacheInvalidationProof
  )

  Invoke-Objc3cNativePerfWrapperCacheInvalidationProof `
    -Config $Config `
    -CacheFixture $CacheFixture `
    -CacheFixtureRel $CacheFixtureRel `
    -CompileScript $CompileScript `
    -CacheInvalidationProof $CacheInvalidationProof
}

function Invoke-Objc3cNativePerfMacroHostProof {
  param(
    [object]$Config,
    [string]$CompileScript,
    [ref]$MacroHostProof
  )

  Invoke-Objc3cNativePerfWrapperMacroHostProof `
    -Config $Config `
    -CompileScript $CompileScript `
    -MacroHostProof $MacroHostProof
}

function Invoke-Objc3cNativePerfDocsGenerationProof {
  param(
    [object]$Config,
    [ref]$DocsGenerationProof
  )

  Invoke-Objc3cNativePerfDocsGeneratorProof `
    -Config $Config `
    -DocsGenerationProof $DocsGenerationProof
}

Export-ModuleMember -Function @(
  "Ensure-Objc3cNativePerfCompilerAvailable",
  "Invoke-Objc3cNativePerfCacheInvalidationProof",
  "Invoke-Objc3cNativePerfCacheProof",
  "Invoke-Objc3cNativePerfDirectCompiles",
  "Invoke-Objc3cNativePerfDocsGenerationProof",
  "Invoke-Objc3cNativePerfMacroHostProof"
)
