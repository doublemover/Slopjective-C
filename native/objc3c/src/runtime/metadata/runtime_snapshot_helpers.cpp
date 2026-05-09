#include "runtime/metadata/runtime_capability_contracts.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int
objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing(
    objc3_runtime_metaprogramming_expansion_host_boundary_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->property_runtime_ready = 1;
  snapshot->macro_host_execution_ready = 0;
  snapshot->macro_host_process_launch_ready = 0;
  snapshot->runtime_package_loader_ready = 0;
  snapshot->deterministic = 1;
  snapshot->runtime_support_library_archive_relative_path =
      "artifacts/lib/objc3_runtime.lib";
  snapshot->property_behavior_runtime_model =
      "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks";
  snapshot->macro_expansion_host_model =
      "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed";
  snapshot->packaging_model =
      "native-driver-packaging-still-hands-off-metaprogramming-runtime-support-through-artifacts-lib-objc3_runtime-lib-and-runtime-registration-manifests";
  snapshot->fail_closed_model =
      "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int
objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing(
    objc3_runtime_metaprogramming_macro_host_process_cache_integration_snapshot
        *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->property_runtime_ready = 1;
  snapshot->macro_host_execution_ready = 1;
  snapshot->macro_host_process_launch_ready = 1;
  snapshot->runtime_package_loader_ready = 0;
  snapshot->deterministic = 1;
  snapshot->host_executable_relative_path =
      "artifacts/bin/objc3c-frontend-c-api-runner.exe";
  snapshot->cache_root_relative_path =
      objc3c::runtime::MetaprogrammingHostCacheRootForTesting();
  snapshot->host_model =
      "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization";
  snapshot->toolchain_model =
      "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization";
  snapshot->cache_model =
      "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs";
  snapshot->fail_closed_model =
      "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int
objc3_runtime_copy_interop_bridge_packaging_toolchain_snapshot_for_testing(
    objc3_runtime_interop_bridge_packaging_toolchain_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->packaging_topology_ready = 1;
  snapshot->operator_visible_evidence_ready = 1;
  snapshot->header_generation_ready = 0;
  snapshot->module_generation_ready = 0;
  snapshot->bridge_generation_ready = 0;
  snapshot->deterministic = 1;
  snapshot->runtime_support_library_archive_relative_path =
      "artifacts/lib/objc3_runtime.lib";
  snapshot->registration_manifest_model =
      "runtime-registration-manifest-publishes-runtime-archive-path-owned-payloads-and-driver-link-wiring";
  snapshot->cross_module_link_plan_model =
      "runtime-import-surfaces-plus-registration-manifest-peer-artifacts-feed-one-fail-closed-cross-module-link-plan-and-linker-response-sidecar";
  snapshot->operator_visible_evidence_model =
      "operator-visible-interop-evidence-is-the-packaged-runtime-archive-registration-manifest-cross-module-link-plan-and-ir-summary";
  snapshot->fail_closed_model =
      "header-module-and-bridge-generation-remain-unclaimed-until-next-runtime-phase";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_release_candidate_claim_snapshot_for_testing(
    objc3_runtime_release_candidate_claim_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->claim_bundle_ready = 1;
  snapshot->deterministic = 1;
  snapshot->selected_profile = "core";
  snapshot->claimed_profile_ids_csv =
      "core,strict,strict-concurrency,strict-system";
  snapshot->targeted_profile_ids_csv =
      "strict,strict-concurrency,strict-system";
  snapshot->conformance_publication_contract_id =
      objc3c::runtime::kObjc3ConformancePublicationContractId;
  snapshot->conformance_claim_operations_contract_id =
      objc3c::runtime::kObjc3ConformanceClaimOperationsContractId;
  snapshot->selection_model =
      objc3c::runtime::kObjc3ConformanceClaimSelectionModel;
  snapshot->failure_model =
      objc3c::runtime::kObjc3ConformanceClaimFailureModel;
  snapshot->advanced_feature_ops_contract_id =
      objc3c::runtime::kObjc3AdvancedFeatureOpsContractId;
  snapshot->advanced_feature_reporting_contract_id =
      objc3c::runtime::kObjc3AdvancedFeatureReportingContractId;
  snapshot->advanced_feature_release_evidence_contract_id =
      objc3c::runtime::kObjc3AdvancedFeatureReleaseEvidenceContractId;
  snapshot->release_evidence_operation_contract_id =
      objc3c::runtime::kObjc3AdvancedFeatureReleaseEvidenceOperationContractId;
  snapshot->dashboard_status_publication_contract_id =
      objc3c::runtime::kObjc3AdvancedFeatureDashboardStatusPublicationContractId;
  snapshot->release_candidate_matrix_contract_id =
      "objc3c.tooling.release.candidate.execution.matrix.v1";
  snapshot->dashboard_schema_path =
      objc3c::runtime::kObjc3AdvancedFeatureDashboardSchemaPath;
  snapshot->gate_script_path =
      objc3c::runtime::kObjc3AdvancedFeatureEvidenceGateScriptPath;
  snapshot->runbook_reference_path =
      objc3c::runtime::kObjc3AdvancedFeatureEvidenceRunbookPath;
  snapshot->release_bundle_model =
      "final-claim-publication-bundle-is-the-conformance-publication-validation-release-evidence-dashboard-advanced-feature-gate-and-release-candidate-matrix-artifact-set";
  snapshot->deprecated_path_shutdown_model =
      "deprecated-claim-matrix-dashboard-ready-summary-and-toolchain-runtime-ga-scaffold-paths-remain-retired-and-fail-closed";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int
objc3_runtime_copy_release_candidate_evidence_state_for_testing(
    objc3_runtime_release_candidate_evidence_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->validation_artifact_ready = 1;
  snapshot->release_evidence_operation_ready = 1;
  snapshot->dashboard_status_ready = 1;
  snapshot->advanced_feature_gate_ready = 1;
  snapshot->release_candidate_matrix_ready = 1;
  snapshot->deprecated_paths_shutdown = 1;
  snapshot->deterministic = 1;
  snapshot->validation_artifact_name = "module.objc3-conformance-validation.json";
  snapshot->release_evidence_operation_artifact_name =
      "module.objc3-release-evidence-operation.json";
  snapshot->dashboard_status_artifact_name = "module.objc3-dashboard-status.json";
  snapshot->advanced_feature_gate_artifact_name =
      "module.objc3-advanced-feature-gate.json";
  snapshot->release_candidate_matrix_artifact_name =
      "module.objc3-release-candidate-matrix.json";
  snapshot->validation_model =
      "driver-validates-versioned-conformance-report-and-publication-sidecars-before-toolchain-consumption";
  snapshot->release_bundle_model =
      "validation-completes-the-final-claim-publication-bundle-over-the-live-validation-release-evidence-dashboard-gate-and-release-candidate-artifacts";
  snapshot->deprecated_path_shutdown_model =
      "deprecated-claim-matrix-dashboard-ready-summary-and-toolchain-runtime-ga-scaffold-paths-remain-retired-and-fail-closed";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_interop_bridge_generation_snapshot_for_testing(
    objc3_runtime_interop_bridge_generation_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->runtime_generation_ready = 1;
  snapshot->cross_module_packaging_ready = 1;
  snapshot->header_generation_ready = 1;
  snapshot->module_generation_ready = 1;
  snapshot->bridge_generation_ready = 1;
  snapshot->deterministic = 1;
  snapshot->header_artifact_relative_path = "module.interop-bridge.h";
  snapshot->module_artifact_relative_path = "module.interop-bridge.modulemap";
  snapshot->bridge_artifact_relative_path = "module.interop-bridge.json";
  snapshot->generation_model =
      "compiler-emits-deterministic-interop-bridge-header-modulemap-and-bridge-json-artifacts-for-supported-foreign-callable-surfaces";
  snapshot->packaging_model =
      "runtime-import-surfaces-and-cross-module-link-plans-preserve-interop-bridge-artifact-paths-for-mixed-module-consumption";
  snapshot->fail_closed_model =
      "missing-generated-artifacts-or-drifted-import-surface-paths-disable-live-interop-bridge-generation-claims";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
