Set-StrictMode -Version Latest

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
$script:RuntimeLaunchContractScript = Join-Path $script:ScriptsRoot "objc3c_runtime_launch_contract.ps1"
if (!(Test-Path -LiteralPath $script:RuntimeLaunchContractScript -PathType Leaf)) {
  throw "execution smoke FAIL: runtime launch contract helper missing at $script:RuntimeLaunchContractScript"
}
. $script:RuntimeLaunchContractScript

function Get-RuntimeLaunchLinkContract {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EmitPrefix = "module"
  )

  return Get-Objc3cRuntimeLaunchContract `
    -CompileDir $CompileDir `
    -RepoRoot $RepoRoot `
    -EmitPrefix $EmitPrefix `
    -DefaultRuntimeLibraryRelativePath "artifacts/lib/objc3_runtime.lib"
}

Export-ModuleMember -Function @(
  "Get-RuntimeLaunchLinkContract"
)
