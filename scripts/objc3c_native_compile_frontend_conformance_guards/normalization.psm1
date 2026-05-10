$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
foreach ($modulePath in @($conformanceGuardConfigModule, $conformanceGuardReportingModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard helper module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Read-FrontendConformanceJsonArtifact {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$ArtifactName
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    Stop-FrontendConformanceGuard "$ArtifactName artifact missing at $Path"
  }

  try {
    return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
  } catch {
    Stop-FrontendConformanceGuard "$ArtifactName artifact is not valid JSON at $Path"
  }
}

function New-FrontendConformanceStringSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $text = [string]$value
    if (-not [string]::IsNullOrWhiteSpace($text)) {
      $set[$text] = $true
    }
  }
  return $set
}

function Normalize-FrontendConformanceBackendKey {
  param(
    [string]$Value
  )

  if ([string]::IsNullOrWhiteSpace($Value)) {
    return ""
  }
  return $Value.Trim().ToLowerInvariant().Replace("_", "-")
}

function New-FrontendConformanceExpectedProfileSet {
  $config = Get-FrontendConformanceInvocationConfig
  $expectedProfileSet = @{}
  foreach ($cacheMode in @($config.cache_modes)) {
    foreach ($backendMode in @($config.backend_modes)) {
      foreach ($summaryMode in @($config.summary_modes)) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $expectedProfileSet[$profileKey] = $true
      }
      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $expectedProfileSet[$profileKey] = $true
    }
  }
  return $expectedProfileSet
}

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
      $backendKey = Normalize-FrontendConformanceBackendKey -Value $backendValue
      if (@($config.allowed_ir_object_backends) -notcontains $backendKey) {
        Stop-FrontendConformanceGuard "unsupported value '$backendValue' for --objc3-ir-object-backend"
      }
      $backendMode = $backendKey
      continue
    }

    if ($token.StartsWith("$($config.backend_flag)=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("$($config.backend_flag)=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Stop-FrontendConformanceGuard "empty value for --objc3-ir-object-backend"
      }
      $backendKey = Normalize-FrontendConformanceBackendKey -Value $backendValue
      if (@($config.allowed_ir_object_backends) -notcontains $backendKey) {
        Stop-FrontendConformanceGuard "unsupported value '$backendValue' for --objc3-ir-object-backend"
      }
      $backendMode = $backendKey
      continue
    }

    if ($token -eq $config.capability_summary_flag) {
      if (($i + 1) -ge $compileArgs.Count) {
        Stop-FrontendConformanceGuard "missing value for --llvm-capabilities-summary"
      }
      $i++
      $summaryValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Stop-FrontendConformanceGuard "empty value for --llvm-capabilities-summary"
      }
      $summaryMode = "present"
      continue
    }

    if ($token.StartsWith("$($config.capability_summary_flag)=", [System.StringComparison]::Ordinal)) {
      $summaryValue = $token.Substring("$($config.capability_summary_flag)=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Stop-FrontendConformanceGuard "empty value for --llvm-capabilities-summary"
      }
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

Export-ModuleMember -Function @(
  "Get-FrontendConformanceInvocationProfileKey",
  "New-FrontendConformanceExpectedProfileSet",
  "New-FrontendConformanceStringSet",
  "Normalize-FrontendConformanceBackendKey",
  "Read-FrontendConformanceJsonArtifact"
)
