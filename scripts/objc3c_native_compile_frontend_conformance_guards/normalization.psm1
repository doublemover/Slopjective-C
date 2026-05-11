$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
foreach ($modulePath in @($conformanceGuardConfigModule, $conformanceGuardReportingModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard helper module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

$normalizationRoot = Join-Path $PSScriptRoot "normalization"
$normalizationModules = @(
  "artifact_io.psm1",
  "backend_keys.psm1",
  "sets.psm1",
  "profile_keys.psm1"
)

foreach ($normalizationModule in $normalizationModules) {
  $normalizationModulePath = Join-Path $normalizationRoot $normalizationModule
  if (!(Test-Path -LiteralPath $normalizationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance normalization helper missing at $normalizationModulePath"
    exit 2
  }
  . $normalizationModulePath
}

Export-ModuleMember -Function @(
  "Get-FrontendConformanceInvocationProfileKey",
  "New-FrontendConformanceExpectedProfileSet",
  "New-FrontendConformanceStringSet",
  "Normalize-FrontendConformanceBackendKey",
  "Read-FrontendConformanceJsonArtifact"
)
