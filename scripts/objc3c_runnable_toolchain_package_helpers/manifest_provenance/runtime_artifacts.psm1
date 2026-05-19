Set-StrictMode -Version Latest

function Get-ManifestProvenanceRuntimeArtifactFiles {
  return @(
    "scripts/check_objc3c_native_perf_budget.ps1",
    "scripts/benchmark_objc3c_runtime_performance.py",
    "scripts/check_objc3c_runtime_acceptance.py",
    "tmp/artifacts/objc3c-native/frontend_source_graph.json",
    "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
    "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
    "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
    "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
    "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
    "tmp/artifacts/objc3c-native/frontend_integration_closeout.json",
    "tmp/build-objc3c-native/repo_superclean_source_of_truth.json",
    "native/objc3c/src/runtime/public/objc3_runtime_api.h",
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceRuntimeArtifactFiles")
