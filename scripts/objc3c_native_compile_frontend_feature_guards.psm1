$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendCoreFeatureExpansion {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs
  )

  $featurePath = Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $featurePath -PathType Leaf)) {
    Write-Error "frontend core feature expansion artifact missing at $featurePath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $featurePath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend core feature expansion artifact is not valid JSON at $featurePath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend core feature expansion contract id mismatch in $featurePath"
    exit 2
  }

  $expectedDependencyContracts = @(
    "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1",
    "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
  )
  $presentDependencyContracts = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (![string]::IsNullOrWhiteSpace($contractIdText)) {
      $presentDependencyContracts[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencyContracts) {
    if (-not $presentDependencyContracts.ContainsKey($requiredContractId)) {
      Write-Error "frontend core feature expansion missing dependency contract '$requiredContractId' in $featurePath"
      exit 2
    }
  }

  $requiredModules = @("driver", "diagnostics-io", "ir", "lex-parse", "frontend-api", "lowering", "pipeline", "sema")
  $presentModules = @{}
  foreach ($moduleName in @($payload.module_names)) {
    $moduleText = [string]$moduleName
    if (![string]::IsNullOrWhiteSpace($moduleText)) {
      $presentModules[$moduleText] = $true
    }
  }
  foreach ($requiredModule in $requiredModules) {
    if (-not $presentModules.ContainsKey($requiredModule)) {
      Write-Error "frontend core feature expansion missing required module '$requiredModule' in $featurePath"
      exit 2
    }
  }

  $invocation = $payload.invocation
  if ($null -eq $invocation) {
    Write-Error "frontend core feature expansion invocation metadata missing in $featurePath"
    exit 2
  }
  if ([string]$invocation.default_out_dir -ne "tmp/artifacts/compilation/objc3c-native") {
    Write-Error "frontend core feature expansion default_out_dir mismatch in $featurePath"
    exit 2
  }
  if ([string]$invocation.cache_root -ne "tmp/artifacts/objc3c-native/cache") {
    Write-Error "frontend core feature expansion cache_root mismatch in $featurePath"
    exit 2
  }
  if (-not [bool]$invocation.supports_cache) {
    Write-Error "frontend core feature expansion supports_cache must be true in $featurePath"
    exit 2
  }

  $backendRouting = $payload.backend_routing
  if ($null -eq $backendRouting) {
    Write-Error "frontend core feature expansion backend_routing metadata missing in $featurePath"
    exit 2
  }
  if (-not [bool]$backendRouting.supports_capability_routing) {
    Write-Error "frontend core feature expansion supports_capability_routing must be true in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend core feature expansion capability_summary_flag mismatch in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend core feature expansion route_flag mismatch in $featurePath"
    exit 2
  }

  $allowedBackends = @{}
  foreach ($backend in @($backendRouting.allowed_ir_object_backends)) {
    $backendText = ([string]$backend).Trim()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends[$backendText.ToLowerInvariant()] = $backendText
    }
  }
  foreach ($requiredBackend in @("clang", "llvm-direct")) {
    if (-not $allowedBackends.ContainsKey($requiredBackend)) {
      Write-Error "frontend core feature expansion missing backend '$requiredBackend' in $featurePath"
      exit 2
    }
  }

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $requestedBackend = $null
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]
    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $requestedBackend = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      continue
    }
    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $requestedBackend = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
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
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }
    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($requestedBackend)) {
    $normalizedRequestedBackend = $requestedBackend.Trim().ToLowerInvariant().Replace("_", "-")
    if (-not $allowedBackends.ContainsKey($normalizedRequestedBackend)) {
      Write-Error "requested --objc3-ir-object-backend '$requestedBackend' is not allowed by frontend core feature expansion in $featurePath"
      exit 2
    }
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    feature_path = $featurePath
    allowed_ir_object_backends = @($allowedBackends.Keys)
  }
}

function Assert-FrontendEdgeCompatibility {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  $compatPath = Resolve-FrontendEdgeCompatibilityPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $compatPath -PathType Leaf)) {
    Write-Error "frontend edge compatibility artifact missing at $compatPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $compatPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge compatibility artifact is not valid JSON at $compatPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge compatibility contract id mismatch in $compatPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1",
    "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge compatibility missing dependency contract '$requiredContractId' in $compatPath"
      exit 2
    }
  }

  $edgeCompat = $payload.invocation_edge_compat
  if ($null -eq $edgeCompat) {
    Write-Error "frontend edge compatibility invocation_edge_compat metadata missing in $compatPath"
    exit 2
  }
  if ([int]$edgeCompat.fail_closed_exit_code -ne 2) {
    Write-Error "frontend edge compatibility fail_closed_exit_code must be 2 in $compatPath"
    exit 2
  }
  if (-not [bool]$edgeCompat.disallow_relative_parent_segments) {
    Write-Error "frontend edge compatibility disallow_relative_parent_segments must be true in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend edge compatibility route_flag mismatch in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend edge compatibility capability_summary_flag mismatch in $compatPath"
    exit 2
  }

  $backendCompat = $payload.backend_compat
  if ($null -eq $backendCompat) {
    Write-Error "frontend edge compatibility backend_compat metadata missing in $compatPath"
    exit 2
  }

  $canonicalBackends = @{}
  foreach ($backend in @($backendCompat.canonical_allowed_backends)) {
    $backendText = ([string]$backend).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $canonicalBackends[$backendText] = $true
    }
  }
  if ($canonicalBackends.Count -eq 0) {
    Write-Error "frontend edge compatibility canonical_allowed_backends must be non-empty in $compatPath"
    exit 2
  }
  if ($null -ne $CoreFeatureGuard) {
    foreach ($coreBackend in @($CoreFeatureGuard.allowed_ir_object_backends)) {
      $coreBackendText = ([string]$coreBackend).Trim().ToLowerInvariant()
      if (-not [string]::IsNullOrWhiteSpace($coreBackendText) -and
          -not $canonicalBackends.ContainsKey($coreBackendText)) {
        Write-Error "frontend edge compatibility missing backend '$coreBackendText' declared by frontend core feature expansion"
        exit 2
      }
    }
  }

  $aliasMap = @{}
  $aliasPayload = $backendCompat.alias_to_canonical
  if ($null -eq $aliasPayload) {
    Write-Error "frontend edge compatibility alias_to_canonical mapping missing in $compatPath"
    exit 2
  }
  foreach ($property in $aliasPayload.PSObject.Properties) {
    $alias = ([string]$property.Name).Trim().ToLowerInvariant().Replace("_", "-")
    $canonical = ([string]$property.Value).Trim().ToLowerInvariant().Replace("_", "-")
    if ([string]::IsNullOrWhiteSpace($alias) -or [string]::IsNullOrWhiteSpace($canonical)) {
      Write-Error "frontend edge compatibility alias_to_canonical entries must be non-empty in $compatPath"
      exit 2
    }
    if (-not $canonicalBackends.ContainsKey($canonical)) {
      Write-Error "frontend edge compatibility alias '$alias' maps to unknown canonical backend '$canonical' in $compatPath"
      exit 2
    }
    $aliasMap[$alias] = $canonical
  }
  foreach ($canonicalBackend in $canonicalBackends.Keys) {
    if (-not $aliasMap.ContainsKey($canonicalBackend)) {
      $aliasMap[$canonicalBackend] = $canonicalBackend
    }
  }

  $singleValueFlags = @{}
  foreach ($flag in @($backendCompat.single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $singleValueFlags[$flagText] = 0
    }
  }
  foreach ($requiredSingleValueFlag in @("--objc3-ir-object-backend", "--llvm-capabilities-summary")) {
    if (-not $singleValueFlags.ContainsKey($requiredSingleValueFlag)) {
      Write-Error "frontend edge compatibility missing single-value flag '$requiredSingleValueFlag' in $compatPath"
      exit 2
    }
  }

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
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      $emitPrefix = $token.Substring("--emit-prefix=".Length)
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--emit-prefix") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --emit-prefix"
        exit 2
      }
      $i++
      $emitPrefix = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $normalizedArgs.Add("--emit-prefix")
      $normalizedArgs.Add($emitPrefix)
      continue
    }

    if ($token.StartsWith("--clang=", [System.StringComparison]::Ordinal)) {
      $clangPath = $token.Substring("--clang=".Length)
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Write-Error "empty value for --clang"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--clang") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --clang"
        exit 2
      }
      $i++
      $clangPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Write-Error "empty value for --clang"
        exit 2
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
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }

    $normalizedArgs.Add($token)
  }

  foreach ($flag in $singleValueFlags.Keys) {
    if ([int]$singleValueFlags[$flag] -gt 1) {
      Write-Error "$flag can be provided at most once"
      exit 2
    }
  }
  if ($routeFlagOccurrences -gt 1) {
    Write-Error "--objc3-route-backend-from-capabilities can be provided at most once"
    exit 2
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    edge_compat_path = $compatPath
    normalized_compile_args = $normalizedArgs.ToArray()
  }
}

Export-ModuleMember -Function @("Assert-FrontendCoreFeatureExpansion", "Assert-FrontendEdgeCompatibility")
