$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$featureGuardModelsModule = Join-Path $PSScriptRoot "evaluation_models.psm1"
$featureGuardArtifactsModule = Join-Path $PSScriptRoot "evaluation_artifacts.psm1"
$featureGuardDiagnosticsModule = Join-Path $PSScriptRoot "evaluation_diagnostics.psm1"
foreach ($modulePath in @(
    $featureGuardNormalizationModule,
    $featureGuardReportingModule,
    $featureGuardModelsModule,
    $featureGuardArtifactsModule,
    $featureGuardDiagnosticsModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard core compile dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

$coreCompileArgRoot = Join-Path $PSScriptRoot "evaluation_core_compile_args"
$coreCompileArgModules = @(
  "token_readers.psm1",
  "state_assertions.psm1",
  "assertion.psm1"
)

foreach ($coreCompileArgModule in $coreCompileArgModules) {
  $coreCompileArgModulePath = Join-Path $coreCompileArgRoot $coreCompileArgModule
  if (!(Test-Path -LiteralPath $coreCompileArgModulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard core compile helper missing at $coreCompileArgModulePath"
    exit 2
  }
  . $coreCompileArgModulePath
}

Export-ModuleMember -Function "Assert-FrontendCoreCompileArgsImpl"
