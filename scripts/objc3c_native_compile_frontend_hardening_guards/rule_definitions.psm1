$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$hardeningRuleRoot = Join-Path $PSScriptRoot "rule_definitions"
$hardeningRuleModules = @(
  "edge_robustness.psm1",
  "diagnostics.psm1",
  "recovery_determinism.psm1"
)

foreach ($hardeningRuleModule in $hardeningRuleModules) {
  $hardeningRuleModulePath = Join-Path $hardeningRuleRoot $hardeningRuleModule
  if (!(Test-Path -LiteralPath $hardeningRuleModulePath -PathType Leaf)) {
    Write-Error "native compile frontend hardening rule helper missing at $hardeningRuleModulePath"
    exit 2
  }
  . $hardeningRuleModulePath
}

Export-ModuleMember -Function @(
  "Get-FrontendEdgeRobustnessGuardRules",
  "Get-FrontendDiagnosticsHardeningGuardRules",
  "Get-FrontendRecoveryDeterminismHardeningGuardRules"
)
