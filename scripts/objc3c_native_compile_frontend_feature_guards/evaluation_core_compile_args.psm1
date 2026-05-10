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

function Assert-FrontendCoreCompileArgsImpl {
  param(
    [object]$ParsedArgs,
    [hashtable]$AllowedBackends,
    [string]$ArtifactPath
  )

  $state = New-FrontendCoreCompileArgGuardState
  $compileArgs = @(Read-FrontendFeatureCompileArgs -ParsedArgs $ParsedArgs)

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]
    if ($token -eq "--objc3-ir-object-backend") {
      Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $compileArgs -Index $i -FlagName "--objc3-ir-object-backend"
      $i++
      $state.requested_backend = [string]$compileArgs[$i]
      Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--objc3-ir-object-backend" -Value $state.requested_backend
      continue
    }
    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $state.requested_backend = $token.Substring("--objc3-ir-object-backend=".Length)
      Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--objc3-ir-object-backend" -Value $state.requested_backend
      continue
    }
    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $state.uses_capability_routing = $true
      continue
    }
    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length)
      $state.uses_capability_routing = ConvertTo-FrontendFeatureBooleanFlagValue `
        -FlagName "--objc3-route-backend-from-capabilities" `
        -Value $routeBoolean
      continue
    }
    if ($token -eq "--llvm-capabilities-summary") {
      Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $compileArgs -Index $i -FlagName "--llvm-capabilities-summary"
      $i++
      $summaryPath = [string]$compileArgs[$i]
      Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--llvm-capabilities-summary" -Value $summaryPath
      $state.has_capability_summary = $true
      continue
    }
    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--llvm-capabilities-summary" -Value $summaryPath
      $state.has_capability_summary = $true
      continue
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($state.requested_backend)) {
    $normalizedRequestedBackend = Normalize-FrontendFeatureBackendKey -Value $state.requested_backend
    if (-not $AllowedBackends.ContainsKey($normalizedRequestedBackend)) {
      Stop-FrontendFeatureGuard "requested --objc3-ir-object-backend '$($state.requested_backend)' is not allowed by frontend core feature expansion in $ArtifactPath"
    }
  }
  Assert-FrontendFeatureCapabilityRoutingHasSummary `
    -UsesCapabilityRouting $state.uses_capability_routing `
    -HasCapabilitySummary $state.has_capability_summary
}

Export-ModuleMember -Function "Assert-FrontendCoreCompileArgsImpl"
