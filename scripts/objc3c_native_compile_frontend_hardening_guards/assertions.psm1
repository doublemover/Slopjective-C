$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$hardeningAssertionsRoot = Join-Path $PSScriptRoot "assertions"
$hardeningAssertionModules = @(
  "common.psm1",
  "edge_robustness.psm1",
  "diagnostics.psm1",
  "recovery_determinism.psm1"
)

foreach ($hardeningAssertionModule in $hardeningAssertionModules) {
  $hardeningAssertionModulePath = Join-Path $hardeningAssertionsRoot $hardeningAssertionModule
  if (!(Test-Path -LiteralPath $hardeningAssertionModulePath -PathType Leaf)) {
    Write-Error "native compile frontend hardening assertion helper missing at $hardeningAssertionModulePath"
    exit 2
  }
  $hardeningAssertionRootLiteral = (Split-Path -Parent $hardeningAssertionModulePath).Replace("'", "''")
  $hardeningAssertionScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$hardeningAssertionRootLiteral'`n" +
    (Get-Content -LiteralPath $hardeningAssertionModulePath -Raw)
  )
  . $hardeningAssertionScript
}

Export-ModuleMember -Function @(
  "Assert-FrontendEdgeRobustnessPayload",
  "Assert-FrontendDiagnosticsHardeningPayload",
  "Assert-FrontendRecoveryDeterminismHardeningPayload"
)
