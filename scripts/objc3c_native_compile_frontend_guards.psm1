$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$objc3c_native_compile_frontend_artifact_guards_psm1 = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_artifact_guards.psm1"
if (!(Test-Path -LiteralPath $objc3c_native_compile_frontend_artifact_guards_psm1 -PathType Leaf)) {
  Write-Error "native compile frontend guard module missing at $objc3c_native_compile_frontend_artifact_guards_psm1"
  exit 2
}
Import-Module $objc3c_native_compile_frontend_artifact_guards_psm1 -Force -DisableNameChecking

$objc3c_native_compile_frontend_feature_guards_psm1 = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_feature_guards.psm1"
if (!(Test-Path -LiteralPath $objc3c_native_compile_frontend_feature_guards_psm1 -PathType Leaf)) {
  Write-Error "native compile frontend guard module missing at $objc3c_native_compile_frontend_feature_guards_psm1"
  exit 2
}
Import-Module $objc3c_native_compile_frontend_feature_guards_psm1 -Force -DisableNameChecking

$objc3c_native_compile_frontend_hardening_guards_psm1 = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_hardening_guards.psm1"
if (!(Test-Path -LiteralPath $objc3c_native_compile_frontend_hardening_guards_psm1 -PathType Leaf)) {
  Write-Error "native compile frontend guard module missing at $objc3c_native_compile_frontend_hardening_guards_psm1"
  exit 2
}
Import-Module $objc3c_native_compile_frontend_hardening_guards_psm1 -Force -DisableNameChecking

$objc3c_native_compile_frontend_conformance_guards_psm1 = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_conformance_guards.psm1"
if (!(Test-Path -LiteralPath $objc3c_native_compile_frontend_conformance_guards_psm1 -PathType Leaf)) {
  Write-Error "native compile frontend guard module missing at $objc3c_native_compile_frontend_conformance_guards_psm1"
  exit 2
}
Import-Module $objc3c_native_compile_frontend_conformance_guards_psm1 -Force -DisableNameChecking

function Invoke-Objc3cNativeCompileFrontendGuards {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult,
    [Parameter(Mandatory = $true)]$ParsedArgs
  )

  Assert-FrontendModuleScaffold -RepoRoot $RepoRoot -BuildResult $BuildResult
  Assert-FrontendInvocationLock -RepoRoot $RepoRoot -BuildResult $BuildResult
  $coreFeatureGuard = Assert-FrontendCoreFeatureExpansion -RepoRoot $RepoRoot -BuildResult $BuildResult -ParsedArgs $ParsedArgs
  $edgeCompatGuard = Assert-FrontendEdgeCompatibility `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs `
    -CoreFeatureGuard $coreFeatureGuard
  Assert-FrontendEdgeRobustness -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null
  Assert-FrontendDiagnosticsHardening -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null
  Assert-FrontendRecoveryDeterminismHardening -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null
  $effectiveCompileArgs = @($ParsedArgs.compile_args)
  if ($null -ne $edgeCompatGuard -and $null -ne $edgeCompatGuard.normalized_compile_args) {
    $effectiveCompileArgs = @($edgeCompatGuard.normalized_compile_args)
  }
  $matrixGuard = Assert-FrontendConformanceMatrix `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs `
    -EffectiveCompileArgs $effectiveCompileArgs
  Assert-FrontendConformanceCorpus `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -InvocationProfileKey ([string]$matrixGuard.profile_key) | Out-Null
  Assert-FrontendIntegrationCloseout -RepoRoot $RepoRoot -BuildResult $BuildResult | Out-Null

  return [pscustomobject]@{
    effective_compile_args = $effectiveCompileArgs
  }
}

Export-ModuleMember -Function "Invoke-Objc3cNativeCompileFrontendGuards"
