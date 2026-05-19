Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_helpers/fixture_selection.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_helpers/hash_io.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_helpers/ll_surface.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_helpers/manifest_pipeline.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_helpers/provenance.psm1") -Force -DisableNameChecking

Export-ModuleMember -Function @(
  "Assert-CompileOutputProvenance",
  "Assert-Objc3ManifestPipelineSurface",
  "Assert-RecoveryFixtureClass",
  "Get-EntrypointLlSurface",
  "Get-FixtureCaseName",
  "Get-RecoveryFixtures",
  "Select-RecoveryFixtureEntries"
)
