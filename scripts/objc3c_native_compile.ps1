$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function Assert-Objc3cNativeCompileDependencyFile {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Path,
    [Parameter(Mandatory = $true)]
    [string]$Description
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    Write-Error "$Description missing at $Path"
    exit 2
  }
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$compileArgumentsScript = Join-Path $repoRoot "scripts/objc3c_native_compile_arguments.ps1"
$compileIoModule = Join-Path $repoRoot "scripts/objc3c_native_compile_io.psm1"
$compileToolchainModule = Join-Path $repoRoot "scripts/objc3c_native_compile_toolchain.psm1"
$compileFrontendGuardsModule = Join-Path $repoRoot "scripts/objc3c_native_compile_frontend_guards.psm1"
$compileCommandModule = Join-Path $repoRoot "scripts/objc3c_native_compile_command.psm1"
$runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
$compileWrapperModule = Join-Path $repoRoot "scripts/objc3c_native_compile_wrapper.psm1"

Assert-Objc3cNativeCompileDependencyFile -Path $compileArgumentsScript -Description "native compile argument helper"
Assert-Objc3cNativeCompileDependencyFile -Path $compileIoModule -Description "native compile IO helper"
Assert-Objc3cNativeCompileDependencyFile -Path $compileToolchainModule -Description "native compile toolchain helper"
Assert-Objc3cNativeCompileDependencyFile -Path $compileFrontendGuardsModule -Description "native compile frontend guard helper"
Assert-Objc3cNativeCompileDependencyFile -Path $compileCommandModule -Description "native compile command helper"
Assert-Objc3cNativeCompileDependencyFile -Path $runtimeLaunchContractScript -Description "runtime launch contract helper"
Assert-Objc3cNativeCompileDependencyFile -Path $compileWrapperModule -Description "native compile wrapper helper"

. $compileArgumentsScript
Import-Module $compileFrontendGuardsModule -Force -DisableNameChecking
Import-Module $compileIoModule -Force -DisableNameChecking
Import-Module $compileToolchainModule -Force -DisableNameChecking
Import-Module $compileCommandModule -Force -DisableNameChecking
. $runtimeLaunchContractScript
Import-Module $compileWrapperModule -Force -DisableNameChecking

Invoke-Objc3cNativeCompileWrapper `
  -RepoRoot $repoRoot `
  -DefaultOutDir (Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native") `
  -RawArgs $args `
  -WrapperScriptPath $PSCommandPath
