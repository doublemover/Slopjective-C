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

function Invoke-FrontendEdgeRobustnessGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $rules = Get-FrontendEdgeRobustnessGuardRules
  $robustnessPath = Resolve-FrontendEdgeRobustnessPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $payload = Read-FrontendHardeningGuardJsonArtifact -Path $robustnessPath -ArtifactName $rules.artifact_name

  Assert-FrontendEdgeRobustnessPayload `
    -Payload $payload `
    -Rules $rules `
    -ArtifactPath $robustnessPath

  return New-FrontendHardeningGuardReport -PropertyName $rules.report_key -ArtifactPath $robustnessPath
}

function Invoke-FrontendDiagnosticsHardeningGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $rules = Get-FrontendDiagnosticsHardeningGuardRules
  $diagnosticsPath = Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $payload = Read-FrontendHardeningGuardJsonArtifact -Path $diagnosticsPath -ArtifactName $rules.artifact_name

  Assert-FrontendDiagnosticsHardeningPayload `
    -Payload $payload `
    -Rules $rules `
    -ArtifactPath $diagnosticsPath

  return New-FrontendHardeningGuardReport -PropertyName $rules.report_key -ArtifactPath $diagnosticsPath
}

function Invoke-FrontendRecoveryDeterminismHardeningGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $rules = Get-FrontendRecoveryDeterminismHardeningGuardRules
  $recoveryPath = Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $payload = Read-FrontendHardeningGuardJsonArtifact -Path $recoveryPath -ArtifactName $rules.artifact_name

  Assert-FrontendRecoveryDeterminismHardeningPayload `
    -Payload $payload `
    -Rules $rules `
    -ArtifactPath $recoveryPath

  return New-FrontendHardeningGuardReport -PropertyName $rules.report_key -ArtifactPath $recoveryPath
}

Export-ModuleMember -Function @(
  "Invoke-FrontendEdgeRobustnessGuard",
  "Invoke-FrontendDiagnosticsHardeningGuard",
  "Invoke-FrontendRecoveryDeterminismHardeningGuard"
)
