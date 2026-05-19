$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardModuleRoot = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_feature_guards"
$featureGuardOrchestrationModule = Join-Path $featureGuardModuleRoot "orchestration.psm1"
if (!(Test-Path -LiteralPath $featureGuardOrchestrationModule -PathType Leaf)) {
  Write-Error "native compile frontend feature guard orchestration module missing at $featureGuardOrchestrationModule"
  exit 2
}
$featureGuardOrchestrationRootLiteral = (Split-Path -Parent $featureGuardOrchestrationModule).Replace("'", "''")
$featureGuardOrchestrationScript = [scriptblock]::Create(
  "`$PSScriptRoot = '$featureGuardOrchestrationRootLiteral'`n" +
  (Get-Content -LiteralPath $featureGuardOrchestrationModule -Raw)
)
. $featureGuardOrchestrationScript

function Assert-FrontendCoreFeatureExpansion {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs
  )

  Invoke-FrontendCoreFeatureExpansionGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs
}

function Assert-FrontendEdgeCompatibility {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  Invoke-FrontendEdgeCompatibilityGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs `
    -CoreFeatureGuard $CoreFeatureGuard
}

Export-ModuleMember -Function @("Assert-FrontendCoreFeatureExpansion", "Assert-FrontendEdgeCompatibility")
