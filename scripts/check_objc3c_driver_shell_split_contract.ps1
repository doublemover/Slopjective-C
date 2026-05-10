$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_driver_shell_split_contract_runner.psm1") -Force -DisableNameChecking

Invoke-Objc3cDriverShellSplitContract -ScriptRoot $PSScriptRoot
