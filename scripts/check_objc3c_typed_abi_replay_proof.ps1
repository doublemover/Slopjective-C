$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "check_objc3c_typed_abi_replay_proof/runner.psm1") -Force -DisableNameChecking

Invoke-Objc3cTypedAbiReplayProof -ScriptRoot $PSScriptRoot
