$ErrorActionPreference = "Stop"

$payloadModules = @(
  "payload_matrix.psm1",
  "payload_corpus.psm1",
  "payload_closeout.psm1"
)

foreach ($payloadModule in $payloadModules) {
  $payloadModulePath = Join-Path $PSScriptRoot $payloadModule
  if (!(Test-Path -LiteralPath $payloadModulePath -PathType Leaf)) {
    throw "frontend closeout conformance payload module missing: $payloadModulePath"
  }
  . $payloadModulePath
}
