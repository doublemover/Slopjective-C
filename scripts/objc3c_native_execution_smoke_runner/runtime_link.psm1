Set-StrictMode -Version Latest

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
$script:RuntimeLaunchContractScript = Join-Path $script:ScriptsRoot "objc3c_runtime_launch_contract.ps1"
if (!(Test-Path -LiteralPath $script:RuntimeLaunchContractScript -PathType Leaf)) {
  throw "execution smoke FAIL: runtime launch contract helper missing at $script:RuntimeLaunchContractScript"
}
. $script:RuntimeLaunchContractScript
Import-Module (Join-Path $PSScriptRoot "config.psm1") -Force -DisableNameChecking

function Get-RuntimeLaunchLinkContract {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EmitPrefix = "module"
  )

  $defaultRuntimeLibraryRelativePath = Get-Objc3cNativeExecutionSmokeDefaultRuntimeLibraryRelativePath
  return Get-Objc3cRuntimeLaunchContract `
    -CompileDir $CompileDir `
    -RepoRoot $RepoRoot `
    -EmitPrefix $EmitPrefix `
    -DefaultRuntimeLibraryRelativePath $defaultRuntimeLibraryRelativePath
}

Export-ModuleMember -Function @(
  "Get-RuntimeLaunchLinkContract"
)
