$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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

    if (Read-FrontendEdgeBackendCompileArgToken `
        -Token $token `
        -CompileArgs $compileArgs `
        -Index ([ref]$i) `
        -AliasMap $AliasMap `
        -SingleValueFlags $SingleValueFlags `
        -State $state) {
      continue
    }
    if (Read-FrontendEdgeCapabilitySummaryCompileArgToken `
        -Token $token `
        -CompileArgs $compileArgs `
        -Index ([ref]$i) `
        -SingleValueFlags $SingleValueFlags `
        -State $state) {
      continue
    }
    if (Read-FrontendEdgePlainValueCompileArgToken `
        -Token $token `
        -CompileArgs $compileArgs `
        -Index ([ref]$i) `
        -State $state) {
      continue
    }
    if (Read-FrontendEdgeRouteCompileArgToken -Token $token -State $state) {
      continue
    }

    $state.normalized_args.Add($token)
  }

  Assert-FrontendFeatureSingleUseFlagCounts -SingleValueFlags $SingleValueFlags
  Assert-FrontendEdgeRouteCompileArgState -State $state
  return $state.normalized_args.ToArray()
}

function Read-FrontendEdgeBackendCompileArgToken {
  param(
    [string]$Token,
    [object[]]$CompileArgs,
    [ref]$Index,
    [hashtable]$AliasMap,
    [hashtable]$SingleValueFlags,
    [object]$State
  )

  if ($Token -eq "--objc3-ir-object-backend") {
    Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $CompileArgs -Index $Index.Value -FlagName "--objc3-ir-object-backend"
    $Index.Value++
    Add-FrontendEdgeBackendCompileArg `
      -BackendValue ([string]$CompileArgs[$Index.Value]) `
      -AliasMap $AliasMap `
      -SingleValueFlags $SingleValueFlags `
      -NormalizedArgs $State.normalized_args
    return $true
  }

  if ($Token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
    Add-FrontendEdgeBackendCompileArg `
      -BackendValue ($Token.Substring("--objc3-ir-object-backend=".Length)) `
      -AliasMap $AliasMap `
      -SingleValueFlags $SingleValueFlags `
      -NormalizedArgs $State.normalized_args
    return $true
  }

  return $false
}

function Read-FrontendEdgeCapabilitySummaryCompileArgToken {
  param(
    [string]$Token,
    [object[]]$CompileArgs,
    [ref]$Index,
    [hashtable]$SingleValueFlags,
    [object]$State
  )

  if ($Token -eq "--llvm-capabilities-summary") {
    Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $CompileArgs -Index $Index.Value -FlagName "--llvm-capabilities-summary"
    $Index.Value++
    Add-FrontendEdgeCapabilitySummaryCompileArg `
      -SummaryPath ([string]$CompileArgs[$Index.Value]) `
      -SingleValueFlags $SingleValueFlags `
      -State $State
    return $true
  }

  if ($Token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
    Add-FrontendEdgeCapabilitySummaryCompileArg `
      -SummaryPath ($Token.Substring("--llvm-capabilities-summary=".Length)) `
      -SingleValueFlags $SingleValueFlags `
      -State $State
    return $true
  }

  return $false
}

function Read-FrontendEdgePlainValueCompileArgToken {
  param(
    [string]$Token,
    [object[]]$CompileArgs,
    [ref]$Index,
    [object]$State
  )

  foreach ($flagName in @("--emit-prefix", "--clang")) {
    if ($Token.StartsWith("$flagName=", [System.StringComparison]::Ordinal)) {
      Add-FrontendEdgeInlineValueCompileArg `
        -FlagName $flagName `
        -Token $Token `
        -Value ($Token.Substring(("$flagName=").Length)) `
        -NormalizedArgs $State.normalized_args
      return $true
    }
    if ($Token -eq $flagName) {
      Assert-FrontendFeatureCompileArgHasNextValue -CompileArgs $CompileArgs -Index $Index.Value -FlagName $flagName
      $Index.Value++
      Add-FrontendEdgeSplitValueCompileArg `
        -FlagName $flagName `
        -Value ([string]$CompileArgs[$Index.Value]) `
        -NormalizedArgs $State.normalized_args
      return $true
    }
  }

  return $false
}

function Read-FrontendEdgeRouteCompileArgToken {
  param(
    [string]$Token,
    [object]$State
  )

  if ($Token -eq "--objc3-route-backend-from-capabilities") {
    Add-FrontendEdgeRouteCompileArg -ImplicitTrue $true -State $State
    return $true
  }

  if ($Token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
    Add-FrontendEdgeRouteCompileArg `
      -RouteValue ($Token.Substring("--objc3-route-backend-from-capabilities=".Length)) `
      -ImplicitTrue $false `
      -State $State
    return $true
  }

  return $false
}
