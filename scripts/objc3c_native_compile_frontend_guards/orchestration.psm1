$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-Objc3cNativeCompileFrontendGuards {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult,
    [Parameter(Mandatory = $true)]$ParsedArgs
  )

  Assert-Objc3cNativeCompileFrontendFoundationGuards `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult
  $coreFeatureGuard = Assert-FrontendCoreFeatureExpansion `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs
  $edgeCompatGuard = Assert-FrontendEdgeCompatibility `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs `
    -CoreFeatureGuard $coreFeatureGuard
  Assert-Objc3cNativeCompileFrontendHardeningGuards `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult

  $effectiveCompileArgs = Get-Objc3cNativeCompileFrontendEffectiveArgs `
    -ParsedArgs $ParsedArgs `
    -EdgeCompatGuard $edgeCompatGuard
  Assert-Objc3cNativeCompileFrontendConformanceGuards `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs `
    -EffectiveCompileArgs $effectiveCompileArgs

  return [pscustomobject]@{
    effective_compile_args = $effectiveCompileArgs
  }
}

function Assert-Objc3cNativeCompileFrontendFoundationGuards {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult
  )

  Assert-FrontendModuleScaffold -RepoRoot $RepoRoot -BuildResult $BuildResult
  Assert-FrontendInvocationLock -RepoRoot $RepoRoot -BuildResult $BuildResult
}

function Assert-Objc3cNativeCompileFrontendHardeningGuards {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult
  )

  Assert-FrontendEdgeRobustness -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null
  Assert-FrontendDiagnosticsHardening -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null
  Assert-FrontendRecoveryDeterminismHardening -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null
}

function Get-Objc3cNativeCompileFrontendEffectiveArgs {
  param(
    [Parameter(Mandatory = $true)]$ParsedArgs,
    $EdgeCompatGuard
  )

  if ($null -ne $EdgeCompatGuard -and $null -ne $EdgeCompatGuard.normalized_compile_args) {
    return @($EdgeCompatGuard.normalized_compile_args)
  }

  return @($ParsedArgs.compile_args)
}

function Assert-Objc3cNativeCompileFrontendConformanceGuards {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult,
    [Parameter(Mandatory = $true)]$ParsedArgs,
    [Parameter(Mandatory = $true)][string[]]$EffectiveCompileArgs
  )

  $matrixGuard = Assert-FrontendConformanceMatrix `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs `
    -EffectiveCompileArgs $EffectiveCompileArgs
  Assert-FrontendConformanceCorpus `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -InvocationProfileKey ([string]$matrixGuard.profile_key) | Out-Null
  Assert-FrontendIntegrationCloseout -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null
}

Export-ModuleMember -Function "Invoke-Objc3cNativeCompileFrontendGuards"
