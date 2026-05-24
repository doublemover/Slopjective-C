Set-StrictMode -Version Latest

function New-RunnableToolchainPackageOperationsManifestSection {
  return [ordered]@{
    long_horizon_operations_runbook = "docs/runbooks/objc3c_long_horizon_operations.md"
    long_horizon_operations_artifact_contract = "tests/tooling/fixtures/long_horizon_operations/artifact_contract.json"
    long_horizon_operations_schema = "schemas/objc3c-long-horizon-operations-evidence-v1.schema.json"
    long_horizon_operations_surface = [ordered]@{
      evidence_generator = "scripts/build_objc3c_long_horizon_operations_evidence.py"
      integration_validation = "scripts/check_objc3c_long_horizon_operations_integration.py"
      support_window_publication = "scripts/publish_objc3c_long_horizon_operations_metadata.py"
    }
    long_horizon_operations_public_actions = @(
      "validate-long-horizon-operations",
      "publish-long-horizon-operations"
    )
    adoption_legibility_runbook = "docs/runbooks/objc3c_adoption_legibility.md"
    adoption_legibility_artifact_contract = "tests/tooling/fixtures/adoption_legibility/artifact_contract.json"
    adoption_legibility_schema = "schemas/objc3c-adoption-legibility-evidence-v1.schema.json"
    adoption_legibility_surface = [ordered]@{
      boundary_inventory = "tests/tooling/fixtures/adoption_legibility/boundary_inventory.json"
      public_claim_policy = "tests/tooling/fixtures/adoption_legibility/public_claim_policy.json"
      capability_comparison_semantics = "tests/tooling/fixtures/adoption_legibility/capability_comparison_semantics.json"
      adoption_replay_semantics = "tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json"
      evidence_generator = "scripts/build_objc3c_adoption_legibility_evidence.py"
      integration_validation = "scripts/check_objc3c_adoption_legibility_integration.py"
      evaluator_publication = "scripts/publish_objc3c_adoption_legibility_metadata.py"
    }
    adoption_legibility_public_actions = @(
      "validate-adoption-legibility",
      "publish-adoption-legibility"
    )
    governance_sustainability_runbook = "docs/runbooks/objc3c_governance_sustainability.md"
    governance_sustainability_artifact_contract = "tests/tooling/fixtures/governance_sustainability/artifact_contract.json"
    governance_sustainability_schema = "schemas/objc3c-governance-sustainability-evidence-v1.schema.json"
    governance_sustainability_surface = [ordered]@{
      budget_inventory = "tests/tooling/fixtures/governance_sustainability/budget_inventory.json"
      extension_review_policy = "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json"
      stewardship_semantics = "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json"
      evidence_generator = "scripts/build_objc3c_governance_sustainability_evidence.py"
      integration_validation = "scripts/check_objc3c_governance_sustainability_integration.py"
      governance_publication = "scripts/publish_objc3c_governance_sustainability_metadata.py"
    }
    governance_sustainability_public_actions = @(
      "validate-governance-sustainability",
      "publish-governance-sustainability"
    )
    platform_hardening_runbook = "docs/runbooks/objc3c_platform_hardening.md"
    platform_hardening_boundary_inventory = "tests/tooling/fixtures/platform_hardening/boundary_inventory.json"
    platform_support_tier_policy = "tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json"
    platform_unsupported_host_policy = "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json"
    platform_toolchain_archive_policy = "tests/tooling/fixtures/platform_hardening/toolchain_archive_claim_policy.json"
    platform_support_matrix_schema = "schemas/objc3c-platform-support-matrix-v1.schema.json"
    platform_toolchain_support_evidence_schema = "schemas/objc3c-platform-toolchain-support-evidence-v1.schema.json"
    platform_toolchain_support_evidence = "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json"
    platform_support_matrix_contract = "tests/tooling/fixtures/platform_hardening/platform_matrix_artifact_contract.json"
    platform_build_package_validation_contract = "tests/tooling/fixtures/platform_hardening/build_package_validation_contract.json"
    platform_toolchain_range_replay_contract = "tests/tooling/fixtures/platform_hardening/toolchain_range_replay_contract.json"
    platform_install_matrix_integration_contract = "tests/tooling/fixtures/platform_hardening/install_matrix_integration_contract.json"
    platform_packaged_smoke_contract = "tests/tooling/fixtures/platform_hardening/packaged_smoke_integration_contract.json"
    platform_hardening_scripts = [ordered]@{
      support_matrix = "scripts/build_objc3c_platform_support_matrix.py"
      host_evidence_ingestion = "scripts/ingest_objc3c_platform_host_evidence.py"
      boundary_inventory_summary = "scripts/build_platform_hardening_boundary_inventory_summary.py"
      support_tier_policy_summary = "scripts/build_platform_hardening_support_tier_policy_summary.py"
      unsupported_host_policy_summary = "scripts/build_platform_hardening_unsupported_host_policy_summary.py"
      toolchain_archive_policy_summary = "scripts/build_platform_hardening_toolchain_archive_policy_summary.py"
      support_evidence_validation = "scripts/check_platform_hardening_support_evidence.py"
      artifact_contract_summary = "scripts/build_platform_hardening_artifact_contract_summary.py"
      live_integration_validation = "scripts/check_objc3c_platform_hardening_integration.py"
      build_package_validation = "scripts/check_platform_hardening_build_package_validation.py"
      toolchain_range_replay = "scripts/check_platform_hardening_toolchain_range_replay.py"
      install_matrix_integration = "scripts/check_platform_hardening_install_matrix_integration.py"
      runnable_end_to_end_validation = "scripts/check_objc3c_runnable_platform_hardening_end_to_end.py"
    }
    platform_hardening_public_actions = @(
      "build-platform-support-matrix",
      "ingest-platform-host-evidence",
      "validate-platform-hardening",
      "validate-platform-hardening-end-to-end"
    )
    object_model_probe = "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp"
    block_arc_fixture = "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3"
    block_arc_runtime_abi_probe = "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp"
    block_arc_byref_forwarding_probe = "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp"
    storage_reflection_fixture = "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3"
    storage_reflection_probe = "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp"
    error_runtime_fixture = "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3"
    error_runtime_probe = "tests/tooling/runtime/live_error_runtime_integration_probe.cpp"
    continuation_runtime_fixture = "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3"
    continuation_runtime_probe = "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp"
    task_runtime_fixture = "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3"
    task_runtime_probe = "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp"
    actor_runtime_fixture = "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3"
    actor_runtime_probe = "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp"
    metaprogramming_runtime_fixture = "tests/tooling/fixtures/native/macro_host_process_provider.objc3"
    metaprogramming_runtime_consumer_fixture = "tests/tooling/fixtures/native/macro_host_process_consumer.objc3"
    metaprogramming_runtime_probe = "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp"
    interop_runtime_fixture = "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3"
    interop_runtime_consumer_fixture = "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3"
    interop_header_bridge_fixture = "tests/tooling/fixtures/native/header_module_bridge_provider.objc3"
    interop_header_bridge_consumer_fixture = "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
    interop_packaging_probe = "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp"
    interop_bridge_generation_probe = "tests/tooling/runtime/header_module_bridge_generation_probe.cpp"
    execution_fixture_root = "tests/tooling/fixtures/native/execution"
    frontend_contract_artifacts = @(
      "tmp/artifacts/objc3c-native/frontend_source_graph.json",
      "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
      "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
      "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
      "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
      "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
      "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
      "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
      "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
      "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
    )
  }
}

Export-ModuleMember -Function @(
  "New-RunnableToolchainPackageOperationsManifestSection"
)
