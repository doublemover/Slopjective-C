$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$featureGuardModelsModule = Join-Path $PSScriptRoot "evaluation_models.psm1"
$featureGuardArtifactsModule = Join-Path $PSScriptRoot "evaluation_artifacts.psm1"
$featureGuardDiagnosticsModule = Join-Path $PSScriptRoot "evaluation_diagnostics.psm1"
$featureGuardCorePayloadModule = Join-Path $PSScriptRoot "evaluation_core_payload.psm1"
$featureGuardCoreCompileArgsModule = Join-Path $PSScriptRoot "evaluation_core_compile_args.psm1"
$featureGuardEdgePayloadModule = Join-Path $PSScriptRoot "evaluation_edge_payload.psm1"
$featureGuardEdgeCompileArgsModule = Join-Path $PSScriptRoot "evaluation_edge_compile_args.psm1"
foreach ($modulePath in @(
    $featureGuardConfigModule,
    $featureGuardNormalizationModule,
    $featureGuardReportingModule,
    $featureGuardModelsModule,
    $featureGuardArtifactsModule,
    $featureGuardDiagnosticsModule,
    $featureGuardCorePayloadModule,
    $featureGuardCoreCompileArgsModule,
    $featureGuardEdgePayloadModule,
    $featureGuardEdgeCompileArgsModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard condition dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Assert-FrontendCoreFeaturePayload {
  param(
    [object]$Payload,
    [string]$ArtifactPath
  )

  Assert-FrontendCoreFeaturePayloadImpl `
    -Payload $Payload `
    -ArtifactPath $ArtifactPath
}

function Assert-FrontendCoreCompileArgs {
  param(
    [object]$ParsedArgs,
    [hashtable]$AllowedBackends,
    [string]$ArtifactPath
  )

  Assert-FrontendCoreCompileArgsImpl `
    -ParsedArgs $ParsedArgs `
    -AllowedBackends $AllowedBackends `
    -ArtifactPath $ArtifactPath
}

function Assert-FrontendEdgeCompatibilityPayload {
  param(
    [object]$Payload,
    [string]$ArtifactPath,
    [object]$CoreFeatureGuard
  )

  Assert-FrontendEdgeCompatibilityPayloadImpl `
    -Payload $Payload `
    -ArtifactPath $ArtifactPath `
    -CoreFeatureGuard $CoreFeatureGuard
}

function ConvertTo-FrontendEdgeCompatibleCompileArgs {
  param(
    [object]$ParsedArgs,
    [hashtable]$AliasMap,
    [hashtable]$SingleValueFlags
  )

  ConvertTo-FrontendEdgeCompatibleCompileArgsImpl `
    -ParsedArgs $ParsedArgs `
    -AliasMap $AliasMap `
    -SingleValueFlags $SingleValueFlags
}

Export-ModuleMember -Function @(
  "Assert-FrontendCoreFeaturePayload",
  "Assert-FrontendCoreCompileArgs",
  "Assert-FrontendEdgeCompatibilityPayload",
  "ConvertTo-FrontendEdgeCompatibleCompileArgs"
)
