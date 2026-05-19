$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendConformanceInvocationProfileKey {
  param(
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  $config = Get-FrontendConformanceInvocationConfig
  $compileArgs = @($EffectiveCompileArgs)
  $cacheMode = if ($null -ne $ParsedArgs -and [bool]$ParsedArgs.use_cache) { "cache-aware" } else { "no-cache" }
  $backendMode = "default"
  $routingMode = "manual"
  $summaryMode = "none"
  $routeEnabled = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq $config.backend_flag) {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendConformanceGuard "missing value for --objc3-ir-object-backend"
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Stop-FrontendConformanceGuard "empty value for --objc3-ir-object-backend"
      }
      $backendMode = Resolve-FrontendConformanceBackendMode -BackendValue $backendValue -Config $config
      continue
    }

    if ($token.StartsWith("$($config.backend_flag)=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("$($config.backend_flag)=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Stop-FrontendConformanceGuard "empty value for --objc3-ir-object-backend"
      }
      $backendMode = Resolve-FrontendConformanceBackendMode -BackendValue $backendValue -Config $config
      continue
    }

    if ($token -eq $config.capability_summary_flag) {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendConformanceGuard "missing value for --llvm-capabilities-summary"
      }
      $i++
      Assert-FrontendConformanceNonEmptyFlagValue `
        -FlagName $config.capability_summary_flag `
        -FlagValue ([string]$compileArgs[$i])
      $summaryMode = "present"
      continue
    }

    if ($token.StartsWith("$($config.capability_summary_flag)=", [System.StringComparison]::Ordinal)) {
      Assert-FrontendConformanceNonEmptyFlagValue `
        -FlagName $config.capability_summary_flag `
        -FlagValue ($token.Substring("$($config.capability_summary_flag)=".Length))
      $summaryMode = "present"
      continue
    }

    if ($token -eq $config.route_flag) {
      $routeEnabled = $true
      continue
    }

    if ($token.StartsWith("$($config.route_flag)=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("$($config.route_flag)=".Length).Trim().ToLowerInvariant()
      if (@($config.allowed_boolean_true_values) -contains $routeBoolean) {
        $routeEnabled = $true
        continue
      }
      if (@($config.allowed_boolean_false_values) -contains $routeBoolean) {
        continue
      }
      Stop-FrontendConformanceGuard "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
    }
  }

  if ($routeEnabled) {
    $routingMode = "capability-route"
  }
  if ($routingMode -eq "capability-route" -and $summaryMode -ne "present") {
    Stop-FrontendConformanceGuard "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
  }

  return ("{0}|{1}|{2}|{3}" -f $cacheMode, $backendMode, $routingMode, $summaryMode)
}

function Resolve-FrontendConformanceBackendMode {
  param(
    [string]$BackendValue,
    [object]$Config
  )

  $backendKey = Normalize-FrontendConformanceBackendKey -Value $BackendValue
  if (@($Config.allowed_ir_object_backends) -notcontains $backendKey) {
    Stop-FrontendConformanceGuard "unsupported value '$BackendValue' for --objc3-ir-object-backend"
  }
  return $backendKey
}

function Assert-FrontendConformanceNonEmptyFlagValue {
  param(
    [string]$FlagName,
    [string]$FlagValue
  )

  if ([string]::IsNullOrWhiteSpace($FlagValue)) {
    Stop-FrontendConformanceGuard "empty value for $FlagName"
  }
}
