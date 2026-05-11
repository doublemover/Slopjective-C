$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$hardeningGuardRoot = $PSScriptRoot
$scriptRoot = Split-Path $hardeningGuardRoot -Parent
$compileToolchainModule = Join-Path $scriptRoot "objc3c_native_compile_toolchain.psm1"
$hardeningGuardRuleModule = Join-Path $hardeningGuardRoot "rule_definitions.psm1"
$hardeningGuardSourceScanningModule = Join-Path $hardeningGuardRoot "source_scanning.psm1"
$hardeningGuardAssertionsModule = Join-Path $hardeningGuardRoot "assertions.psm1"
$hardeningGuardReportModule = Join-Path $hardeningGuardRoot "report_shaping.psm1"

foreach ($modulePath in @(
  $compileToolchainModule,
  $hardeningGuardRuleModule,
  $hardeningGuardSourceScanningModule,
  $hardeningGuardAssertionsModule,
  $hardeningGuardReportModule
)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend hardening guard dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

$hardeningOrchestrationRoot = Join-Path $PSScriptRoot "orchestration"
$hardeningOrchestrationModules = @(
  "artifact_definitions.psm1",
  "guard_runner.psm1"
)

foreach ($hardeningOrchestrationModule in $hardeningOrchestrationModules) {
  $hardeningOrchestrationModulePath = Join-Path $hardeningOrchestrationRoot $hardeningOrchestrationModule
  if (!(Test-Path -LiteralPath $hardeningOrchestrationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend hardening orchestration helper missing at $hardeningOrchestrationModulePath"
    exit 2
  }
  . $hardeningOrchestrationModulePath
}

function Invoke-FrontendEdgeRobustnessGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendHardeningArtifactGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -Definition (Get-FrontendHardeningEdgeRobustnessDefinition)
}

function Invoke-FrontendDiagnosticsHardeningGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendHardeningArtifactGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -Definition (Get-FrontendHardeningDiagnosticsDefinition)
}

function Invoke-FrontendRecoveryDeterminismHardeningGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendHardeningArtifactGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -Definition (Get-FrontendHardeningRecoveryDeterminismDefinition)
}

Export-ModuleMember -Function @(
  "Invoke-FrontendEdgeRobustnessGuard",
  "Invoke-FrontendDiagnosticsHardeningGuard",
  "Invoke-FrontendRecoveryDeterminismHardeningGuard"
)
