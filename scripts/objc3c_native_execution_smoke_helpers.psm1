Set-StrictMode -Version Latest

$script:HelperModuleRoot = Join-Path $PSScriptRoot "objc3c_native_execution_smoke_helpers"

Import-Module (Join-Path $script:HelperModuleRoot "compiler.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:HelperModuleRoot "commands.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:HelperModuleRoot "paths.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:HelperModuleRoot "diagnostics.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:HelperModuleRoot "expectations.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:HelperModuleRoot "fixtures.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:HelperModuleRoot "objects.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:HelperModuleRoot "runtime_dispatch.psm1") -Force -DisableNameChecking

Export-ModuleMember -Function @(
  "Assert-RuntimeDispatchParityFromLl",
  "Ensure-NativeCompilerExecutable",
  "Get-CanonicalLinkDiagnosticsText",
  "Get-CaseDirectoryName",
  "Get-Fixtures",
  "Get-MissingTokens",
  "Get-NegativeExpectation",
  "Get-PositiveExpectation",
  "Get-RepoRelativePath",
  "Invoke-LoggedCommand",
  "Resolve-NativeObjectPath",
  "Select-ExecutionFixtureEntries"
)
