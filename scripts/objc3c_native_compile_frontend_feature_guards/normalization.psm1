$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
if (!(Test-Path -LiteralPath $featureGuardReportingModule -PathType Leaf)) {
  Write-Error "native compile frontend feature guard reporting module missing at $featureGuardReportingModule"
  exit 2
}
$featureGuardReportingRootLiteral = (Split-Path -Parent $featureGuardReportingModule).Replace("'", "''")
$featureGuardReportingScript = [scriptblock]::Create(
  "`$PSScriptRoot = '$featureGuardReportingRootLiteral'`n" +
  (Get-Content -LiteralPath $featureGuardReportingModule -Raw)
)
. $featureGuardReportingScript

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
  $normalizationRootLiteral = (Split-Path -Parent $normalizationModulePath).Replace("'", "''")
  $normalizationScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$normalizationRootLiteral'`n" +
    (Get-Content -LiteralPath $normalizationModulePath -Raw)
  )
  . $normalizationScript
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
