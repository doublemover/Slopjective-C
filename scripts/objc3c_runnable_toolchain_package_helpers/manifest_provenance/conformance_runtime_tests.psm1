Set-StrictMode -Version Latest

function Get-ManifestProvenanceConformanceRuntimeTestFiles {
  return @(
    "scripts/check_conformance_suite.ps1",
    "scripts/check_conformance_corpus_surface.py",
    "scripts/generate_conformance_corpus_index.py",
    "spec/conformance/release_evidence_gate_maintenance.md",
    "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp",
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp",
    "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
    "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
    "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
    "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
    "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
    "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
    "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp",
    "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
    "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
    "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
    "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp",
    "tests/tooling/runtime/header_module_bridge_generation_probe.cpp",
    "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp",
    "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp",
    "tests/tooling/runtime/support/dispatch_expectations.h",
    "tests/tooling/runtime/support/json_probe_writer.h",
    "tests/tooling/runtime/support/output_expectations.h",
    "tests/tooling/runtime/support/runtime_snapshot_json.h",
    "tests/tooling/runtime/support/runtime_snapshot_stabilizers.h",
    "tests/tooling/runtime/support/runtime_snapshot_text.h"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceConformanceRuntimeTestFiles")
