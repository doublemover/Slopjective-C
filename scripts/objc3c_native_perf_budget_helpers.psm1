Set-StrictMode -Version Latest

$script:Objc3cNativePerfBudgetHelperRoot = Join-Path $PSScriptRoot "objc3c_native_perf_budget_helpers"
$script:Objc3cNativePerfBudgetHelperModules = @(
  "path_config.psm1",
  "report_shaping.psm1",
  "metric_parsing.psm1",
  "command_timing.psm1"
)

foreach ($moduleName in $script:Objc3cNativePerfBudgetHelperModules) {
  Import-Module (Join-Path $script:Objc3cNativePerfBudgetHelperRoot $moduleName) -Force -DisableNameChecking
}

Export-ModuleMember -Function @(
  "Get-ArtifactHashSet",
  "Get-CompileArtifactSurface",
  "Get-Fixtures",
  "Get-PerfFixtureDirectories",
  "Get-RepoRelativePath",
  "Get-ShortHash",
  "Invoke-TimedNativeCommand",
  "Invoke-TimedWrapperCommand",
  "Parse-CacheHitFlag"
)
