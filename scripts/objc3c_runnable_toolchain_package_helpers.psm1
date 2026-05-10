Set-StrictMode -Version Latest

$helperModuleRoot = Join-Path $PSScriptRoot "objc3c_runnable_toolchain_package_helpers"
$helperModules = @(
  "path_normalization.psm1",
  "file_inventory.psm1",
  "manifest_provenance.psm1",
  "package_validation.psm1",
  "exports.psm1"
)

foreach ($moduleName in $helperModules) {
  Import-Module (Join-Path $helperModuleRoot $moduleName) -Force -DisableNameChecking
}

Export-ModuleMember -Function (Get-RunnableToolchainPackageHelperExportedFunctionNames)
