$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileWrapperArgumentsScript = Join-Path $PSScriptRoot "objc3c_native_compile_arguments.ps1"
$compileWrapperIoModule = Join-Path $PSScriptRoot "objc3c_native_compile_io.psm1"
$compileWrapperToolchainModule = Join-Path $PSScriptRoot "objc3c_native_compile_toolchain.psm1"
$compileWrapperFrontendGuardsModule = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_guards.psm1"
$compileWrapperCommandModule = Join-Path $PSScriptRoot "objc3c_native_compile_command.psm1"
$compileWrapperRuntimeLaunchContractScript = Join-Path $PSScriptRoot "objc3c_runtime_launch_contract.ps1"

$compileWrapperDependencyFiles = @(
  [pscustomobject]@{ Path = $compileWrapperArgumentsScript; Description = "native compile wrapper argument dependency" },
  [pscustomobject]@{ Path = $compileWrapperIoModule; Description = "native compile wrapper IO dependency" },
  [pscustomobject]@{ Path = $compileWrapperToolchainModule; Description = "native compile wrapper toolchain dependency" },
  [pscustomobject]@{ Path = $compileWrapperFrontendGuardsModule; Description = "native compile wrapper frontend guard dependency" },
  [pscustomobject]@{ Path = $compileWrapperCommandModule; Description = "native compile wrapper command dependency" },
  [pscustomobject]@{ Path = $compileWrapperRuntimeLaunchContractScript; Description = "native compile wrapper runtime launch contract dependency" }
)
foreach ($compileWrapperDependencyFile in $compileWrapperDependencyFiles) {
  if (!(Test-Path -LiteralPath $compileWrapperDependencyFile.Path -PathType Leaf)) {
    Write-Error "$($compileWrapperDependencyFile.Description) missing at $($compileWrapperDependencyFile.Path)"
    exit 2
  }
}

. $compileWrapperArgumentsScript
Import-Module $compileWrapperIoModule -Force -DisableNameChecking
Import-Module $compileWrapperToolchainModule -Force -DisableNameChecking
Import-Module $compileWrapperFrontendGuardsModule -Force -DisableNameChecking
Import-Module $compileWrapperCommandModule -Force -DisableNameChecking
. $compileWrapperRuntimeLaunchContractScript

$compileWrapperRoot = Join-Path $PSScriptRoot "objc3c_native_compile_wrapper"
$compileWrapperModules = @(
  "success_provenance.psm1",
  "cache_flow.psm1",
  "orchestration.psm1"
)

foreach ($compileWrapperModule in $compileWrapperModules) {
  $compileWrapperModulePath = Join-Path $compileWrapperRoot $compileWrapperModule
  if (!(Test-Path -LiteralPath $compileWrapperModulePath -PathType Leaf)) {
    Write-Error "native compile wrapper support module missing at $compileWrapperModulePath"
    exit 2
  }
  $compileWrapperModuleRootLiteral = (Split-Path -Parent $compileWrapperModulePath).Replace("'", "''")
  $compileWrapperScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$compileWrapperModuleRootLiteral'`n" +
    (Get-Content -LiteralPath $compileWrapperModulePath -Raw)
  )
  . $compileWrapperScript
}

Export-ModuleMember -Function "Invoke-Objc3cNativeCompileWrapper"
