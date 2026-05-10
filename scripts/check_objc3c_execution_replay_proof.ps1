param(
  [string]$CaseId = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_execution_replay_proof_helpers.psm1") -Force -DisableNameChecking

Invoke-Objc3cExecutionReplayProof `
  -ScriptRoot $PSScriptRoot `
  -CaseId $CaseId `
  -ShardIndex $ShardIndex `
  -ShardCount $ShardCount `
  -Limit $Limit
