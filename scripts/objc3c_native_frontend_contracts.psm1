$ErrorActionPreference = "Stop"

$frontendContractModuleRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_contracts"
$frontendContractExportModule = Join-Path $frontendContractModuleRoot "exports.psm1"
if (!(Test-Path -LiteralPath $frontendContractExportModule -PathType Leaf)) {
  throw "frontend contract export module missing: $frontendContractExportModule"
}

. $frontendContractExportModule

foreach ($frontendContractModule in Get-Objc3cNativeFrontendContractModuleNames) {
  $frontendContractModulePath = Join-Path $frontendContractModuleRoot $frontendContractModule
  if (!(Test-Path -LiteralPath $frontendContractModulePath -PathType Leaf)) {
    throw "frontend contract support module missing: $frontendContractModulePath"
  }

  . $frontendContractModulePath
}

Export-ModuleMember -Function (Get-Objc3cNativeFrontendContractExportedFunctionNames)
