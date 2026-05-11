$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
if (!(Test-Path -LiteralPath $featureGuardReportingModule -PathType Leaf)) {
  Write-Error "native compile frontend feature guard reporting module missing at $featureGuardReportingModule"
  exit 2
}
Import-Module $featureGuardReportingModule -Force -DisableNameChecking

$normalizationRoot = Join-Path $PSScriptRoot "normalization"
$normalizationModules = @(
  "json_artifacts.psm1",
  "sets.psm1",
  "backend_aliases.psm1",
  "paths.psm1"
)

foreach ($normalizationModule in $normalizationModules) {
  $normalizationModulePath = Join-Path $normalizationRoot $normalizationModule
  if (!(Test-Path -LiteralPath $normalizationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard normalization helper missing at $normalizationModulePath"
    exit 2
  }
  . $normalizationModulePath
}

Export-ModuleMember -Function @(
  "Read-FrontendFeatureGuardJsonArtifact",
  "New-FrontendFeatureStringSet",
  "Normalize-FrontendFeatureBackendKey",
  "New-FrontendFeatureBackendSet",
  "New-FrontendFeatureLowercaseStringSet",
  "New-FrontendFeatureCountedFlagSet",
  "New-FrontendEdgeBackendAliasMap",
  "Test-FrontendFeatureRelativePathHasParentSegment"
)
