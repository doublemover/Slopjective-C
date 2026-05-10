Set-StrictMode -Version Latest

function Get-ExecutionReplayProofHelperModuleNames {
  return @(
    "path_normalization.psm1",
    "artifact_loading.psm1",
    "command_invocation.psm1",
    "catalog.psm1",
    "context.psm1",
    "digest_provenance.psm1",
    "replay_comparison.psm1",
    "compilation.psm1",
    "report_rendering.psm1",
    "invocation.psm1"
  )
}

function Get-ExecutionReplayProofExportedFunctionNames {
  return @(
    "Invoke-Objc3cExecutionReplayProof"
  )
}
