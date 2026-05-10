Set-StrictMode -Version Latest

$executionReplayProofHelperModuleRoot = Join-Path $PSScriptRoot "objc3c_execution_replay_proof_helpers"
. (Join-Path $executionReplayProofHelperModuleRoot "exports.psm1")

foreach ($executionReplayProofHelperModule in Get-ExecutionReplayProofHelperModuleNames) {
  . (Join-Path $executionReplayProofHelperModuleRoot $executionReplayProofHelperModule)
}

Export-ModuleMember -Function (Get-ExecutionReplayProofExportedFunctionNames)
