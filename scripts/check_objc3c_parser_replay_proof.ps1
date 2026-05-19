param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_parser_replay_proof_runner.psm1") -Force -DisableNameChecking

Invoke-Objc3cParserReplayProof -ScriptRoot $PSScriptRoot
