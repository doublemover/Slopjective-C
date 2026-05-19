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
    Write-Error "native compile frontend feature guard edge compile dependency module missing at $modulePath"
    exit 2
  }
  $moduleRootLiteral = (Split-Path -Parent $modulePath).Replace("'", "''")
  $moduleScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$moduleRootLiteral'`n" +
    (Get-Content -LiteralPath $modulePath -Raw)
  )
  . $moduleScript
}

$edgeCompileArgRoot = Join-Path $PSScriptRoot "evaluation_edge_compile_args"
$edgeCompileArgModules = @(
  "backend_flags.psm1",
  "value_flags.psm1",
  "route_flags.psm1",
  "conversion.psm1"
)

foreach ($edgeCompileArgModule in $edgeCompileArgModules) {
  $edgeCompileArgModulePath = Join-Path $edgeCompileArgRoot $edgeCompileArgModule
  if (!(Test-Path -LiteralPath $edgeCompileArgModulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard edge compile helper missing at $edgeCompileArgModulePath"
    exit 2
  }
  $edgeCompileArgRootLiteral = (Split-Path -Parent $edgeCompileArgModulePath).Replace("'", "''")
  $edgeCompileArgScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$edgeCompileArgRootLiteral'`n" +
    (Get-Content -LiteralPath $edgeCompileArgModulePath -Raw)
  )
  . $edgeCompileArgScript
}

Export-ModuleMember -Function "ConvertTo-FrontendEdgeCompatibleCompileArgsImpl"
