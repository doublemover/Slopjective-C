Set-StrictMode -Version Latest

$executionReplayProofHelperModuleRoot = Join-Path $PSScriptRoot "objc3c_execution_replay_proof_helpers"

$executionReplayProofExportsModuleText = Get-Content `
  -LiteralPath (Join-Path $executionReplayProofHelperModuleRoot "exports.psm1") `
  -Raw
. ([scriptblock]::Create($executionReplayProofExportsModuleText))

foreach ($executionReplayProofHelperModule in Get-ExecutionReplayProofHelperModuleNames) {
  $executionReplayProofHelperModuleText = Get-Content `
    -LiteralPath (Join-Path $executionReplayProofHelperModuleRoot $executionReplayProofHelperModule) `
    -Raw
  . ([scriptblock]::Create($executionReplayProofHelperModuleText))
}

Export-ModuleMember -Function (Get-ExecutionReplayProofExportedFunctionNames)
