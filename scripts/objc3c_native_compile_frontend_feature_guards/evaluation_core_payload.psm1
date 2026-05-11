$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$featureGuardArtifactsModule = Join-Path $PSScriptRoot "evaluation_artifacts.psm1"
$featureGuardDiagnosticsModule = Join-Path $PSScriptRoot "evaluation_diagnostics.psm1"
foreach ($modulePath in @(
    $featureGuardConfigModule,
    $featureGuardNormalizationModule,
    $featureGuardReportingModule,
    $featureGuardArtifactsModule,
    $featureGuardDiagnosticsModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard core payload dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

$corePayloadRoot = Join-Path $PSScriptRoot "evaluation_core_payload"
$corePayloadModules = @(
  "metadata_assertions.psm1",
  "payload_assertions.psm1"
)

foreach ($corePayloadModule in $corePayloadModules) {
  $corePayloadModulePath = Join-Path $corePayloadRoot $corePayloadModule
  if (!(Test-Path -LiteralPath $corePayloadModulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard core payload helper missing at $corePayloadModulePath"
    exit 2
  }
  . $corePayloadModulePath
}

Export-ModuleMember -Function "Assert-FrontendCoreFeaturePayloadImpl"
