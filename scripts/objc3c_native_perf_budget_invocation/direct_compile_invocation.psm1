Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_measurements.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "command_construction.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "process_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativePerfDirectCompileSet {
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
    Write-Objc3cNativePerfFixtureSetLine -FixtureSet $fixtureSet
  }

  foreach ($fixture in $fixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $Config.repo_root
    $hash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $Config.run_dir ("fixture_{0}" -f $hash)
    $compileLog = Join-Path $caseDir "compile.log"
    New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

    $compileArgs = New-Objc3cNativePerfDirectCompileArguments -Fixture $fixture -CaseDir $caseDir
    $run = Invoke-Objc3cNativePerfNativeProcess `
      -Command $CompilerExe `
      -Arguments $compileArgs `
      -LogPath $compileLog

    $compileResult = New-Objc3cNativePerfCompileResult -Config $Config -Fixture $fixture -CaseDir $caseDir -Run $run
    $Results.Value += $compileResult.result
    Write-Objc3cNativePerfCompileResultLine -CompileResult $compileResult
  }

  $Fixtures.Value = $fixtures
  $DispatchFixturePathSet.Value = $dispatchFixturePathSet
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativePerfDirectCompileSet"
)
