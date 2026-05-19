$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force
$frontendContractModuleRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_contracts"
$frontendContractExportModule = Join-Path $frontendContractModuleRoot "exports.psm1"
if (!(Test-Path -LiteralPath $frontendContractExportModule -PathType Leaf)) {
  throw "frontend contract export module missing: $frontendContractExportModule"
}
Import-Module $frontendContractExportModule -Force -DisableNameChecking
foreach ($frontendContractModule in @(Get-Objc3cNativeFrontendContractModuleNames)) {
  $frontendContractModulePath = Join-Path $frontendContractModuleRoot $frontendContractModule
  if (!(Test-Path -LiteralPath $frontendContractModulePath -PathType Leaf)) {
    throw "frontend contract support module missing: $frontendContractModulePath"
  }
  Import-Module $frontendContractModulePath -Force -DisableNameChecking
}
Import-Module (Join-Path $PSScriptRoot "objc3c_native_frontend_closeout_artifacts.psm1") -Force

$frontendArtifactRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_artifacts"
$frontendArtifactModules = @(
  "constants.psm1",
  "loading.psm1",
  "assertions.psm1",
  "payloads.psm1",
  "orchestration.psm1"
)

foreach ($frontendArtifactModule in $frontendArtifactModules) {
  $frontendArtifactModulePath = Join-Path $frontendArtifactRoot $frontendArtifactModule
  if (!(Test-Path -LiteralPath $frontendArtifactModulePath -PathType Leaf)) {
    throw "frontend artifact support module missing: $frontendArtifactModulePath"
  }

  . $frontendArtifactModulePath
}

Export-ModuleMember -Function "Invoke-Objc3cNativeFrontendPacketGeneration"
