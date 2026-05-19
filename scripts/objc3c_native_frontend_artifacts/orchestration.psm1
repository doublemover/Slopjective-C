$ErrorActionPreference = "Stop"

$objc3cNativeFrontendOrchestrationRoot = Join-Path $PSScriptRoot "orchestration"
$objc3cNativeFrontendOrchestrationModules = @(
  "status.psm1",
  "core_artifacts.psm1",
  "packet_generation.psm1"
)

foreach ($objc3cNativeFrontendOrchestrationModule in $objc3cNativeFrontendOrchestrationModules) {
  $objc3cNativeFrontendOrchestrationModulePath = Join-Path `
    $objc3cNativeFrontendOrchestrationRoot `
    $objc3cNativeFrontendOrchestrationModule
  if (!(Test-Path -LiteralPath $objc3cNativeFrontendOrchestrationModulePath -PathType Leaf)) {
    throw "frontend artifact orchestration module missing: $objc3cNativeFrontendOrchestrationModulePath"
  }

  . $objc3cNativeFrontendOrchestrationModulePath
}
