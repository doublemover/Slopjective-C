$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
foreach ($modulePath in @($featureGuardConfigModule, $featureGuardNormalizationModule, $featureGuardReportingModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard helper module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Assert-FrontendFeatureDependencyContracts {
  param(
    [object[]]$PresentContracts,
    [string[]]$RequiredContracts,
    [string]$ArtifactName,
    [string]$ArtifactPath
  )

  $presentDependencyContracts = New-FrontendFeatureStringSet -Values @($PresentContracts)
  foreach ($requiredContractId in $RequiredContracts) {
    if (-not $presentDependencyContracts.ContainsKey($requiredContractId)) {
      Stop-FrontendFeatureGuard "$ArtifactName missing dependency contract '$requiredContractId' in $ArtifactPath"
    }
  }
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

  $invocation = $Payload.invocation
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

  $backendRouting = $Payload.backend_routing
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

  $requestedBackend = $null
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false
  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]
    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendFeatureGuard "missing value for --objc3-ir-object-backend"
      }
      $i++
      $requestedBackend = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Stop-FrontendFeatureGuard "empty value for --objc3-ir-object-backend"
      }
      continue
    }
    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $requestedBackend = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Stop-FrontendFeatureGuard "empty value for --objc3-ir-object-backend"
      }
      continue
    }
    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $usesCapabilityRouting = $true
      continue
    }
    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        $usesCapabilityRouting = $false
        continue
      }
      Stop-FrontendFeatureGuard "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
    }
    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendFeatureGuard "missing value for --llvm-capabilities-summary"
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Stop-FrontendFeatureGuard "empty value for --llvm-capabilities-summary"
      }
      $hasCapabilitySummary = $true
      continue
    }
    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Stop-FrontendFeatureGuard "empty value for --llvm-capabilities-summary"
      }
      $hasCapabilitySummary = $true
      continue
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($requestedBackend)) {
    $normalizedRequestedBackend = Normalize-FrontendFeatureBackendKey -Value $requestedBackend
    if (-not $AllowedBackends.ContainsKey($normalizedRequestedBackend)) {
      Stop-FrontendFeatureGuard "requested --objc3-ir-object-backend '$requestedBackend' is not allowed by frontend core feature expansion in $ArtifactPath"
    }
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Stop-FrontendFeatureGuard "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
  }
}

function Invoke-FrontendCoreFeatureExpansionEvaluation {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [object]$ParsedArgs
  )

  $allowedBackends = Assert-FrontendCoreFeaturePayload -Payload $Payload -ArtifactPath $ArtifactPath
  Assert-FrontendCoreCompileArgs -ParsedArgs $ParsedArgs -AllowedBackends $allowedBackends -ArtifactPath $ArtifactPath

  return [pscustomobject]@{
    feature_path = $ArtifactPath
    allowed_ir_object_backends = @($allowedBackends.Keys)
  }
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

  $edgeCompat = $Payload.invocation_edge_compat
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

  $backendCompat = $Payload.backend_compat
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

  return [pscustomobject]@{
    alias_map = $aliasMap
    single_value_flags = $singleValueFlags
  }
}

function ConvertTo-FrontendEdgeCompatibleCompileArgs {
  param(
    [object]$ParsedArgs,
    [hashtable]$AliasMap,
    [hashtable]$SingleValueFlags
  )

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $normalizedArgs = New-Object System.Collections.Generic.List[string]
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false
  $routeFlagOccurrences = 0

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendFeatureGuard "missing value for --objc3-ir-object-backend"
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Stop-FrontendFeatureGuard "empty value for --objc3-ir-object-backend"
      }
      $backendKey = Normalize-FrontendFeatureBackendKey -Value $backendValue
      if (-not $AliasMap.ContainsKey($backendKey)) {
        Stop-FrontendFeatureGuard "unsupported value '$backendValue' for --objc3-ir-object-backend"
      }
      $SingleValueFlags["--objc3-ir-object-backend"] = [int]$SingleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$AliasMap[$backendKey])
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Stop-FrontendFeatureGuard "empty value for --objc3-ir-object-backend"
      }
      $backendKey = Normalize-FrontendFeatureBackendKey -Value $backendValue
      if (-not $AliasMap.ContainsKey($backendKey)) {
        Stop-FrontendFeatureGuard "unsupported value '$backendValue' for --objc3-ir-object-backend"
      }
      $SingleValueFlags["--objc3-ir-object-backend"] = [int]$SingleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$AliasMap[$backendKey])
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendFeatureGuard "missing value for --llvm-capabilities-summary"
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Stop-FrontendFeatureGuard "empty value for --llvm-capabilities-summary"
      }
      if (Test-FrontendFeatureRelativePathHasParentSegment -Path $summaryPath) {
        Stop-FrontendFeatureGuard "--llvm-capabilities-summary must not contain '..' relative segments"
      }
      $SingleValueFlags["--llvm-capabilities-summary"] = [int]$SingleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Stop-FrontendFeatureGuard "empty value for --llvm-capabilities-summary"
      }
      if (Test-FrontendFeatureRelativePathHasParentSegment -Path $summaryPath) {
        Stop-FrontendFeatureGuard "--llvm-capabilities-summary must not contain '..' relative segments"
      }
      $SingleValueFlags["--llvm-capabilities-summary"] = [int]$SingleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      $emitPrefix = $token.Substring("--emit-prefix=".Length)
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Stop-FrontendFeatureGuard "empty value for --emit-prefix"
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--emit-prefix") {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendFeatureGuard "missing value for --emit-prefix"
      }
      $i++
      $emitPrefix = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Stop-FrontendFeatureGuard "empty value for --emit-prefix"
      }
      $normalizedArgs.Add("--emit-prefix")
      $normalizedArgs.Add($emitPrefix)
      continue
    }

    if ($token.StartsWith("--clang=", [System.StringComparison]::Ordinal)) {
      $clangPath = $token.Substring("--clang=".Length)
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Stop-FrontendFeatureGuard "empty value for --clang"
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--clang") {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendFeatureGuard "missing value for --clang"
      }
      $i++
      $clangPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Stop-FrontendFeatureGuard "empty value for --clang"
      }
      $normalizedArgs.Add("--clang")
      $normalizedArgs.Add($clangPath)
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $routeFlagOccurrences++
      $usesCapabilityRouting = $true
      $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeFlagOccurrences++
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        continue
      }
      Stop-FrontendFeatureGuard "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
    }

    $normalizedArgs.Add($token)
  }

  foreach ($flag in $SingleValueFlags.Keys) {
    if ([int]$SingleValueFlags[$flag] -gt 1) {
      Stop-FrontendFeatureGuard "$flag can be provided at most once"
    }
  }
  if ($routeFlagOccurrences -gt 1) {
    Stop-FrontendFeatureGuard "--objc3-route-backend-from-capabilities can be provided at most once"
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Stop-FrontendFeatureGuard "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
  }

  return $normalizedArgs.ToArray()
}

function Invoke-FrontendEdgeCompatibilityEvaluation {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  $payloadEvaluation = Assert-FrontendEdgeCompatibilityPayload `
    -Payload $Payload `
    -ArtifactPath $ArtifactPath `
    -CoreFeatureGuard $CoreFeatureGuard
  $normalizedCompileArgs = ConvertTo-FrontendEdgeCompatibleCompileArgs `
    -ParsedArgs $ParsedArgs `
    -AliasMap $payloadEvaluation.alias_map `
    -SingleValueFlags $payloadEvaluation.single_value_flags

  return [pscustomobject]@{
    edge_compat_path = $ArtifactPath
    normalized_compile_args = $normalizedCompileArgs
  }
}

Export-ModuleMember -Function @(
  "Invoke-FrontendCoreFeatureExpansionEvaluation",
  "Invoke-FrontendEdgeCompatibilityEvaluation"
)
