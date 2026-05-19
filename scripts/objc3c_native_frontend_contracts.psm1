$ErrorActionPreference = "Stop"

$frontendContractModuleRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_contracts"
$frontendContractExportModule = Join-Path $frontendContractModuleRoot "exports.psm1"
if (!(Test-Path -LiteralPath $frontendContractExportModule -PathType Leaf)) {
  throw "frontend contract export module missing: $frontendContractExportModule"
}

Import-Module $frontendContractExportModule -Force -DisableNameChecking

$frontendContractModules = @(Get-Objc3cNativeFrontendContractModuleNames)
foreach ($frontendContractModule in $frontendContractModules) {
  $frontendContractModulePath = Join-Path $frontendContractModuleRoot $frontendContractModule
  if (!(Test-Path -LiteralPath $frontendContractModulePath -PathType Leaf)) {
    throw "frontend contract support module missing: $frontendContractModulePath"
  }

  Import-Module $frontendContractModulePath -Force -DisableNameChecking
}

$frontendContractExportedFunctions = @(Get-Objc3cNativeFrontendContractExportedFunctionNames)
Export-ModuleMember -Function $frontendContractExportedFunctions
