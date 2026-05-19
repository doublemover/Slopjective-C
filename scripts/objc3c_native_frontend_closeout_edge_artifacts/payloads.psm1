$ErrorActionPreference = "Stop"

$edgePayloadModules = @(
  "payload_edge_compat.psm1",
  "payload_edge_robustness.psm1",
  "payload_diagnostics.psm1",
  "payload_recovery.psm1"
)

foreach ($edgePayloadModule in $edgePayloadModules) {
  $edgePayloadModulePath = Join-Path $PSScriptRoot $edgePayloadModule
  if (!(Test-Path -LiteralPath $edgePayloadModulePath -PathType Leaf)) {
    throw "frontend closeout edge payload module missing: $edgePayloadModulePath"
  }

  . $edgePayloadModulePath
}
