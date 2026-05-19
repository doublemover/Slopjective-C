$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$featureGuardModelsModule = Join-Path $PSScriptRoot "evaluation_models.psm1"
$featureGuardArtifactsModule = Join-Path $PSScriptRoot "evaluation_artifacts.psm1"
$featureGuardDiagnosticsModule = Join-Path $PSScriptRoot "evaluation_diagnostics.psm1"
foreach ($modulePath in @(
    $featureGuardConfigModule,
    $featureGuardNormalizationModule,
    $featureGuardReportingModule,
    $featureGuardModelsModule,
    $featureGuardArtifactsModule,
    $featureGuardDiagnosticsModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard edge payload dependency module missing at $modulePath"
    exit 2
  }
  $moduleRootLiteral = (Split-Path -Parent $modulePath).Replace("'", "''")
  $moduleScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$moduleRootLiteral'`n" +
    (Get-Content -LiteralPath $modulePath -Raw)
  )
  . $moduleScript
}

$edgePayloadRoot = Join-Path $PSScriptRoot "evaluation_edge_payload"
$edgePayloadModules = @(
  "invocation_metadata.psm1",
  "backend_metadata.psm1",
  "payload_assertions.psm1"
)

foreach ($edgePayloadModule in $edgePayloadModules) {
  $edgePayloadModulePath = Join-Path $edgePayloadRoot $edgePayloadModule
  if (!(Test-Path -LiteralPath $edgePayloadModulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard edge payload helper missing at $edgePayloadModulePath"
    exit 2
  }
  $edgePayloadRootLiteral = (Split-Path -Parent $edgePayloadModulePath).Replace("'", "''")
  $edgePayloadScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$edgePayloadRootLiteral'`n" +
    (Get-Content -LiteralPath $edgePayloadModulePath -Raw)
  )
  . $edgePayloadScript
}

Export-ModuleMember -Function "Assert-FrontendEdgeCompatibilityPayloadImpl"
