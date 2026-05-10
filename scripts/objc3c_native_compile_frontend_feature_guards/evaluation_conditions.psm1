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

  $config = Get-FrontendCoreFeatureGuardConfig
  if ([string]$Payload.contract_id -ne $config.contract_id) {
    Stop-FrontendFeatureGuard "frontend core feature expansion contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendFeatureDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($config.dependency_contract_ids) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  $presentModules = New-FrontendFeatureStringSet -Values @($Payload.module_names)
  foreach ($requiredModule in @($config.required_modules)) {
    if (-not $presentModules.ContainsKey($requiredModule)) {
      Stop-FrontendFeatureGuard "frontend core feature expansion missing required module '$requiredModule' in $ArtifactPath"
    }
  }

  $invocation = Read-FrontendCoreFeatureInvocationMetadata -Payload $Payload
  if ($null -eq $invocation) {
    Stop-FrontendFeatureGuard "frontend core feature expansion invocation metadata missing in $ArtifactPath"
  }
  if ([string]$invocation.default_out_dir -ne $config.default_out_dir) {
    Stop-FrontendFeatureGuard "frontend core feature expansion default_out_dir mismatch in $ArtifactPath"
  }
  if ([string]$invocation.cache_root -ne $config.cache_root) {
    Stop-FrontendFeatureGuard "frontend core feature expansion cache_root mismatch in $ArtifactPath"
  }
  if (-not [bool]$invocation.supports_cache) {
    Stop-FrontendFeatureGuard "frontend core feature expansion supports_cache must be true in $ArtifactPath"
  }

  $backendRouting = Read-FrontendCoreFeatureBackendRoutingMetadata -Payload $Payload
  if ($null -eq $backendRouting) {
    Stop-FrontendFeatureGuard "frontend core feature expansion backend_routing metadata missing in $ArtifactPath"
  }
  if (-not [bool]$backendRouting.supports_capability_routing) {
    Stop-FrontendFeatureGuard "frontend core feature expansion supports_capability_routing must be true in $ArtifactPath"
  }
  if ([string]$backendRouting.capability_summary_flag -ne $config.capability_summary_flag) {
    Stop-FrontendFeatureGuard "frontend core feature expansion capability_summary_flag mismatch in $ArtifactPath"
  }
  if ([string]$backendRouting.route_flag -ne $config.route_flag) {
    Stop-FrontendFeatureGuard "frontend core feature expansion route_flag mismatch in $ArtifactPath"
  }

  $allowedBackends = New-FrontendFeatureBackendSet -Values @($backendRouting.allowed_ir_object_backends)
  foreach ($requiredBackend in @($config.allowed_ir_object_backends)) {
    if (-not $allowedBackends.ContainsKey($requiredBackend)) {
      Stop-FrontendFeatureGuard "frontend core feature expansion missing backend '$requiredBackend' in $ArtifactPath"
    }
  }

  return $allowedBackends
}

function Assert-FrontendCoreCompileArgs {
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

function Assert-FrontendEdgeCompatibilityPayload {
  param(
    [object]$Payload,
    [string]$ArtifactPath,
    [object]$CoreFeatureGuard
  )

  $config = Get-FrontendEdgeCompatibilityGuardConfig
  if ([string]$Payload.contract_id -ne $config.contract_id) {
    Stop-FrontendFeatureGuard "frontend edge compatibility contract id mismatch in $ArtifactPath"
  }

  Assert-FrontendFeatureDependencyContracts `
    -PresentContracts @($Payload.depends_on_contract_ids) `
    -RequiredContracts @($config.dependency_contract_ids) `
    -ArtifactName $config.artifact_name `
    -ArtifactPath $ArtifactPath

  $edgeCompat = Read-FrontendEdgeCompatibilityInvocationMetadata -Payload $Payload
  if ($null -eq $edgeCompat) {
    Stop-FrontendFeatureGuard "frontend edge compatibility invocation_edge_compat metadata missing in $ArtifactPath"
  }
  if ([int]$edgeCompat.fail_closed_exit_code -ne $config.fail_closed_exit_code) {
    Stop-FrontendFeatureGuard "frontend edge compatibility fail_closed_exit_code must be 2 in $ArtifactPath"
  }
  if (-not [bool]$edgeCompat.disallow_relative_parent_segments) {
    Stop-FrontendFeatureGuard "frontend edge compatibility disallow_relative_parent_segments must be true in $ArtifactPath"
  }
  if ([string]$edgeCompat.route_flag -ne $config.route_flag) {
    Stop-FrontendFeatureGuard "frontend edge compatibility route_flag mismatch in $ArtifactPath"
  }
  if ([string]$edgeCompat.capability_summary_flag -ne $config.capability_summary_flag) {
    Stop-FrontendFeatureGuard "frontend edge compatibility capability_summary_flag mismatch in $ArtifactPath"
  }

  $backendCompat = Read-FrontendEdgeCompatibilityBackendMetadata -Payload $Payload
  if ($null -eq $backendCompat) {
    Stop-FrontendFeatureGuard "frontend edge compatibility backend_compat metadata missing in $ArtifactPath"
  }

  $canonicalBackends = New-FrontendFeatureLowercaseStringSet -Values @($backendCompat.canonical_allowed_backends)
  if ($canonicalBackends.Count -eq 0) {
    Stop-FrontendFeatureGuard "frontend edge compatibility canonical_allowed_backends must be non-empty in $ArtifactPath"
  }
  if ($null -ne $CoreFeatureGuard) {
    foreach ($coreBackend in @($CoreFeatureGuard.allowed_ir_object_backends)) {
      $coreBackendText = ([string]$coreBackend).Trim().ToLowerInvariant()
      if (-not [string]::IsNullOrWhiteSpace($coreBackendText) -and
          -not $canonicalBackends.ContainsKey($coreBackendText)) {
        Stop-FrontendFeatureGuard "frontend edge compatibility missing backend '$coreBackendText' declared by frontend core feature expansion"
      }
    }
  }

  $aliasMap = New-FrontendEdgeBackendAliasMap `
    -AliasPayload $backendCompat.alias_to_canonical `
    -CanonicalBackends $canonicalBackends `
    -ArtifactPath $ArtifactPath

  $singleValueFlags = New-FrontendFeatureCountedFlagSet -Values @($backendCompat.single_value_flags)
  foreach ($requiredSingleValueFlag in @($config.required_single_value_flags)) {
    if (-not $singleValueFlags.ContainsKey($requiredSingleValueFlag)) {
      Stop-FrontendFeatureGuard "frontend edge compatibility missing single-value flag '$requiredSingleValueFlag' in $ArtifactPath"
    }
  }

  return New-FrontendEdgeCompatibilityPayloadEvaluation `
    -AliasMap $aliasMap `
    -SingleValueFlags $singleValueFlags
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

function ConvertTo-FrontendEdgeCompatibleCompileArgs {
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

Export-ModuleMember -Function @(
  "Assert-FrontendCoreFeaturePayload",
  "Assert-FrontendCoreCompileArgs",
  "Assert-FrontendEdgeCompatibilityPayload",
  "ConvertTo-FrontendEdgeCompatibleCompileArgs"
)
