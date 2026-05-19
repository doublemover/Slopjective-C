$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$hardeningGuardModuleRoot = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_hardening_guards"
$hardeningGuardOrchestrationModule = Join-Path $hardeningGuardModuleRoot "orchestration.psm1"
if (!(Test-Path -LiteralPath $hardeningGuardOrchestrationModule -PathType Leaf)) {
  Write-Error "native compile frontend hardening guard orchestration module missing at $hardeningGuardOrchestrationModule"
  exit 2
}
$hardeningGuardOrchestrationRootLiteral = (Split-Path -Parent $hardeningGuardOrchestrationModule).Replace("'", "''")
$hardeningGuardOrchestrationScript = [scriptblock]::Create(
  "`$PSScriptRoot = '$hardeningGuardOrchestrationRootLiteral'`n" +
  (Get-Content -LiteralPath $hardeningGuardOrchestrationModule -Raw)
)
. $hardeningGuardOrchestrationScript

function Assert-FrontendEdgeRobustness {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendEdgeRobustnessGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult
}

function Assert-FrontendDiagnosticsHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendDiagnosticsHardeningGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult
}

function Assert-FrontendRecoveryDeterminismHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendRecoveryDeterminismHardeningGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult
}

Export-ModuleMember -Function @("Assert-FrontendEdgeRobustness", "Assert-FrontendDiagnosticsHardening", "Assert-FrontendRecoveryDeterminismHardening")
