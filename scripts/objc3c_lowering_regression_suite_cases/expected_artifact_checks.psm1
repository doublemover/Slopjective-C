Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:ExpectedArtifactChecksModuleRoot = Join-Path $PSScriptRoot "expected_artifact_checks"
Import-Module (Join-Path $script:ExpectedArtifactChecksModuleRoot "artifacts.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:ExpectedArtifactChecksModuleRoot "ir_expectations.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:ExpectedArtifactChecksModuleRoot "ir_artifact_assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:ExpectedArtifactChecksModuleRoot "artifact_failure_checks.psm1") -Force -DisableNameChecking

Export-ModuleMember -Function @(
  "Add-LoweringExpectedArtifactFailures",
  "Get-LoweringCaseArtifacts",
  "Get-LoweringIrExpectations",
  "Invoke-DispatchIrArtifactExpectations",
  "Test-Objc3IrArtifactExpectations"
)
