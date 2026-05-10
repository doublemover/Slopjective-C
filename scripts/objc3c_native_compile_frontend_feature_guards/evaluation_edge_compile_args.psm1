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
  Import-Module $modulePath -Force -DisableNameChecking
}

function Add-FrontendEdgeBackendCompileArg {
  param(
    [string]$BackendValue,
    [hashtable]$AliasMap,
    [hashtable]$SingleValueFlags,
    [System.Collections.Generic.List[string]]$NormalizedArgs
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--objc3-ir-object-backend" -Value $BackendValue
  $backendKey = Normalize-FrontendFeatureBackendKey -Value $BackendValue
  if (-not $AliasMap.ContainsKey($backendKey)) {
    Stop-FrontendFeatureGuard "unsupported value '$BackendValue' for --objc3-ir-object-backend"
  }
  $SingleValueFlags["--objc3-ir-object-backend"] = [int]$SingleValueFlags["--objc3-ir-object-backend"] + 1
  $NormalizedArgs.Add("--objc3-ir-object-backend")
  $NormalizedArgs.Add([string]$AliasMap[$backendKey])
}

function Add-FrontendEdgeCapabilitySummaryCompileArg {
  param(
    [string]$SummaryPath,
    [hashtable]$SingleValueFlags,
    [object]$State
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--llvm-capabilities-summary" -Value $SummaryPath
  if (Test-FrontendFeatureArgumentPathHasParentSegment -Path $SummaryPath) {
    Stop-FrontendFeatureGuard "--llvm-capabilities-summary must not contain '..' relative segments"
  }
  $SingleValueFlags["--llvm-capabilities-summary"] = [int]$SingleValueFlags["--llvm-capabilities-summary"] + 1
  $State.has_capability_summary = $true
  $State.normalized_args.Add("--llvm-capabilities-summary")
  $State.normalized_args.Add($SummaryPath)
}

function Add-FrontendEdgeInlineValueCompileArg {
  param(
    [string]$FlagName,
    [string]$Token,
    [string]$Value,
    [System.Collections.Generic.List[string]]$NormalizedArgs
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName $FlagName -Value $Value
  $NormalizedArgs.Add($Token)
}

function Add-FrontendEdgeSplitValueCompileArg {
  param(
    [string]$FlagName,
    [string]$Value,
    [System.Collections.Generic.List[string]]$NormalizedArgs
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName $FlagName -Value $Value
  $NormalizedArgs.Add($FlagName)
  $NormalizedArgs.Add($Value)
}

function Add-FrontendEdgeRouteCompileArg {
  param(
    [string]$RouteValue,
    [bool]$ImplicitTrue,
    [object]$State
  )

  $State.route_flag_occurrences = [int]$State.route_flag_occurrences + 1
  $routeEnabled = $ImplicitTrue
  if (-not $ImplicitTrue) {
    $routeEnabled = ConvertTo-FrontendFeatureBooleanFlagValue `
      -FlagName "--objc3-route-backend-from-capabilities" `
      -Value $RouteValue
  }
  if ($routeEnabled) {
    $State.uses_capability_routing = $true
    $State.normalized_args.Add("--objc3-route-backend-from-capabilities")
  }
}

function ConvertTo-FrontendEdgeCompatibleCompileArgsImpl {
  param(
    [object]$ParsedArgs,
    [hashtable]$AliasMap,
    [hashtable]$SingleValueFlags
  )

  $compileArgs = @(Read-FrontendFeatureCompileArgs -ParsedArgs $ParsedArgs)
  $state = New-FrontendEdgeCompileArgNormalizationState

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $compileArgs -Index $i -FlagName "--objc3-ir-object-backend"
      $i++
      Add-FrontendEdgeBackendCompileArg `
        -BackendValue ([string]$compileArgs[$i]) `
        -AliasMap $AliasMap `
        -SingleValueFlags $SingleValueFlags `
        -NormalizedArgs $state.normalized_args
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      Add-FrontendEdgeBackendCompileArg `
        -BackendValue ($token.Substring("--objc3-ir-object-backend=".Length)) `
        -AliasMap $AliasMap `
        -SingleValueFlags $SingleValueFlags `
        -NormalizedArgs $state.normalized_args
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $compileArgs -Index $i -FlagName "--llvm-capabilities-summary"
      $i++
      Add-FrontendEdgeCapabilitySummaryCompileArg `
        -SummaryPath ([string]$compileArgs[$i]) `
        -SingleValueFlags $SingleValueFlags `
        -State $state
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      Add-FrontendEdgeCapabilitySummaryCompileArg `
        -SummaryPath ($token.Substring("--llvm-capabilities-summary=".Length)) `
        -SingleValueFlags $SingleValueFlags `
        -State $state
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      Add-FrontendEdgeInlineValueCompileArg `
        -FlagName "--emit-prefix" `
        -Token $token `
        -Value ($token.Substring("--emit-prefix=".Length)) `
        -NormalizedArgs $state.normalized_args
      continue
    }

    if ($token -eq "--emit-prefix") {
      Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $compileArgs -Index $i -FlagName "--emit-prefix"
      $i++
      Add-FrontendEdgeSplitValueCompileArg `
        -FlagName "--emit-prefix" `
        -Value ([string]$compileArgs[$i]) `
        -NormalizedArgs $state.normalized_args
      continue
    }

    if ($token.StartsWith("--clang=", [System.StringComparison]::Ordinal)) {
      Add-FrontendEdgeInlineValueCompileArg `
        -FlagName "--clang" `
        -Token $token `
        -Value ($token.Substring("--clang=".Length)) `
        -NormalizedArgs $state.normalized_args
      continue
    }

    if ($token -eq "--clang") {
      Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $compileArgs -Index $i -FlagName "--clang"
      $i++
      Add-FrontendEdgeSplitValueCompileArg `
        -FlagName "--clang" `
        -Value ([string]$compileArgs[$i]) `
        -NormalizedArgs $state.normalized_args
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      Add-FrontendEdgeRouteCompileArg -ImplicitTrue $true -State $state
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      Add-FrontendEdgeRouteCompileArg `
        -RouteValue ($token.Substring("--objc3-route-backend-from-capabilities=".Length)) `
        -ImplicitTrue $false `
        -State $state
      continue
    }

    $state.normalized_args.Add($token)
  }

  Assert-FrontendFeatureSingleUseFlagCounts -SingleValueFlags $SingleValueFlags
  if ([int]$state.route_flag_occurrences -gt 1) {
    Stop-FrontendFeatureGuard "--objc3-route-backend-from-capabilities can be provided at most once"
  }
  Assert-FrontendFeatureCapabilityRoutingHasSummary `
    -UsesCapabilityRouting $state.uses_capability_routing `
    -HasCapabilitySummary $state.has_capability_summary

  return $state.normalized_args.ToArray()
}

Export-ModuleMember -Function "ConvertTo-FrontendEdgeCompatibleCompileArgsImpl"
