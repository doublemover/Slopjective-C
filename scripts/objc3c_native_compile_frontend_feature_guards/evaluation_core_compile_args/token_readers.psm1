$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Read-FrontendCoreBackendCompileArgToken {
  param(
    [Parameter(Mandatory = $true)]$State,
    [Parameter(Mandatory = $true)][string]$Token,
    [Parameter(Mandatory = $true)][object[]]$CompileArgs,
    [Parameter(Mandatory = $true)][ref]$Index
  )

  if ($Token -eq "--objc3-ir-object-backend") {
    Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $CompileArgs -Index $Index.Value -FlagName "--objc3-ir-object-backend"
    $Index.Value++
    $State.requested_backend = [string]$CompileArgs[$Index.Value]
    Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--objc3-ir-object-backend" -Value $State.requested_backend
    return $true
  }
  if ($Token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
    $State.requested_backend = $Token.Substring("--objc3-ir-object-backend=".Length)
    Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--objc3-ir-object-backend" -Value $State.requested_backend
    return $true
  }

  return $false
}

function Read-FrontendCoreCapabilityRouteCompileArgToken {
  param(
    [Parameter(Mandatory = $true)]$State,
    [Parameter(Mandatory = $true)][string]$Token
  )

  if ($Token -eq "--objc3-route-backend-from-capabilities") {
    $State.uses_capability_routing = $true
    return $true
  }
  if ($Token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
    $State.uses_capability_routing = ConvertTo-FrontendFeatureBooleanFlagValue `
      -FlagName "--objc3-route-backend-from-capabilities" `
      -Value ($Token.Substring("--objc3-route-backend-from-capabilities=".Length))
    return $true
  }

  return $false
}

function Read-FrontendCoreCapabilitySummaryCompileArgToken {
  param(
    [Parameter(Mandatory = $true)]$State,
    [Parameter(Mandatory = $true)][string]$Token,
    [Parameter(Mandatory = $true)][object[]]$CompileArgs,
    [Parameter(Mandatory = $true)][ref]$Index
  )

  if ($Token -eq "--llvm-capabilities-summary") {
    Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $CompileArgs -Index $Index.Value -FlagName "--llvm-capabilities-summary"
    $Index.Value++
    $summaryPath = [string]$CompileArgs[$Index.Value]
    Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--llvm-capabilities-summary" -Value $summaryPath
    $State.has_capability_summary = $true
    return $true
  }
  if ($Token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
    $summaryPath = $Token.Substring("--llvm-capabilities-summary=".Length)
    Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--llvm-capabilities-summary" -Value $summaryPath
    $State.has_capability_summary = $true
    return $true
  }

  return $false
}
