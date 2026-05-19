Set-StrictMode -Version Latest

function Get-ManifestProvenanceNativeExecutionFiles {
  return @(
    "scripts/benchmark_objc3c_runtime_inspector.py",
    "scripts/objc3c_runtime_launch_contract.ps1",
    "scripts/run_objc3c_native_compile_proof.ps1",
    "scripts/check_objc3c_native_execution_smoke.ps1",
    "scripts/objc3c_native_execution_smoke_helpers.psm1",
    "scripts/objc3c_native_execution_smoke_runner.psm1",
    "scripts/check_objc3c_execution_replay_proof.ps1",
    "scripts/objc3c_execution_replay_proof_helpers.psm1",
    "scripts/build_objc3c_native_docs.py"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceNativeExecutionFiles")
