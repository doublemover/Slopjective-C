#include "runtime/metadata/runtime_capability_contracts.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_copy_release_candidate_claim_snapshot_for_testing(
    objc3_runtime_release_candidate_claim_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->claim_bundle_ready = 1;
  snapshot->deterministic = 1;
  snapshot->selected_profile = "core";
  snapshot->claimed_profile_ids_csv = "core,strict,strict-concurrency";
  snapshot->targeted_profile_ids_csv =
      "strict-system";
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
