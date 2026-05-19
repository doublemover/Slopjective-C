#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_legality_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "io/objc3_json.h"
#include "runtime/metadata/selector_metadata_bootstrap_legality_surfaces.h"

namespace objc3::artifacts::frontend {

using objc3::io::EscapeJsonString;

void AppendObjc3FrontendArtifactRuntimeBootstrapLegalityManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan) {
  const Objc3RuntimeBootstrapLegalityFailureContractSummary
      &runtime_bootstrap_legality_failure_contract =
          runtime_registration_plan
              .runtime_bootstrap_legality_failure_contract;
  const Objc3RuntimeBootstrapLegalitySemanticsSummary
      &runtime_bootstrap_legality_semantics =
          runtime_registration_plan.runtime_bootstrap_legality_semantics;

  manifest
      << ",\"runtime_bootstrap_legality_failure_contract_id\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract.contract_id)
      << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract
                 .registration_descriptor_frontend_closure_contract_id)
      << "\",\"runtime_bootstrap_legality_failure_bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .bootstrap_semantics_contract_id)
      << "\",\"runtime_bootstrap_legality_failure_frontend_surface_path\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.frontend_surface_path)
      << "\",\"runtime_bootstrap_legality_failure_duplicate_registration_policy\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .duplicate_registration_policy)
      << "\",\"runtime_bootstrap_legality_failure_image_registration_order_invariant\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .image_registration_order_invariant)
      << "\",\"runtime_bootstrap_legality_failure_failure_mode\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.failure_mode)
      << "\",\"runtime_bootstrap_legality_failure_restart_lifecycle_model\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.restart_lifecycle_model)
      << "\",\"runtime_bootstrap_legality_failure_replay_order_model\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.replay_order_model)
      << "\",\"runtime_bootstrap_legality_failure_image_local_init_reset_model\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .image_local_init_reset_model)
      << "\",\"runtime_bootstrap_legality_failure_catalog_retention_model\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.catalog_retention_model)
      << "\",\"runtime_bootstrap_legality_failure_runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .runtime_state_snapshot_symbol)
      << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_identifier\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .registration_descriptor_identifier)
      << "\",\"runtime_bootstrap_legality_failure_image_root_identifier\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.image_root_identifier)
      << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_identity_source\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .registration_descriptor_identity_source)
      << "\",\"runtime_bootstrap_legality_failure_image_root_identity_source\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .image_root_identity_source)
      << "\",\"runtime_bootstrap_legality_failure_translation_unit_registration_order_ordinal\":"
      << runtime_bootstrap_legality_failure_contract
             .translation_unit_registration_order_ordinal
      << ",\"runtime_bootstrap_legality_failure_fail_closed\":"
      << (runtime_bootstrap_legality_failure_contract.fail_closed ? "true"
                                                                 : "false")
      << ",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_contract_ready\":"
      << (runtime_bootstrap_legality_failure_contract
                  .registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_bootstrap_semantics_contract_ready\":"
      << (runtime_bootstrap_legality_failure_contract
                  .bootstrap_semantics_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_semantic_boundary_ready\":"
      << (runtime_bootstrap_legality_failure_contract.semantic_boundary_ready
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_duplicate_registration_policy_frozen\":"
      << (runtime_bootstrap_legality_failure_contract
                  .duplicate_registration_policy_frozen
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_image_order_invariant_frozen\":"
      << (runtime_bootstrap_legality_failure_contract
                  .image_order_invariant_frozen
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_bootstrap_rejection_frozen\":"
      << (runtime_bootstrap_legality_failure_contract.bootstrap_rejection_frozen
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_restart_boundary_frozen\":"
      << (runtime_bootstrap_legality_failure_contract.restart_boundary_frozen
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_semantic_diagnostics_required\":"
      << (runtime_bootstrap_legality_failure_contract
                  .semantic_diagnostics_required
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_ready_for_lowering_and_runtime\":"
      << (runtime_bootstrap_legality_failure_contract
                  .ready_for_lowering_and_runtime
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                              .registration_descriptor_frontend_closure_replay_key)
      << "\",\"runtime_bootstrap_legality_failure_bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract
                 .bootstrap_semantics_replay_key)
      << "\",\"runtime_bootstrap_legality_failure_semantic_boundary_replay_key\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.semantic_boundary_replay_key)
      << "\",\"runtime_bootstrap_legality_failure_replay_key\":\""
      << EscapeJsonString(runtime_bootstrap_legality_failure_contract.replay_key)
      << "\",\"runtime_bootstrap_legality_failure_failure_reason\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_failure_contract.failure_reason)
      << "\""
      << ",\"runtime_bootstrap_legality_semantics_contract_id\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics.contract_id)
      << "\",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_id\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics
                              .bootstrap_legality_failure_contract_id)
      << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics
                              .registration_descriptor_frontend_closure_contract_id)
      << "\",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.bootstrap_semantics_contract_id)
      << "\",\"runtime_bootstrap_legality_semantics_frontend_surface_path\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.frontend_surface_path)
      << "\",\"runtime_bootstrap_legality_semantics_duplicate_registration_policy\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.duplicate_registration_policy)
      << "\",\"runtime_bootstrap_legality_semantics_image_registration_order_invariant\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics
                              .image_registration_order_invariant)
      << "\",\"runtime_bootstrap_legality_semantics_cross_image_legality_model\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.cross_image_legality_model)
      << "\",\"runtime_bootstrap_legality_semantics_semantic_diagnostic_model\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.semantic_diagnostic_model)
      << "\",\"runtime_bootstrap_legality_semantics_translation_unit_identity_model\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.translation_unit_identity_model)
      << "\",\"runtime_bootstrap_legality_semantics_translation_unit_identity_key\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.translation_unit_identity_key)
      << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_identifier\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics
                              .registration_descriptor_identifier)
      << "\",\"runtime_bootstrap_legality_semantics_image_root_identifier\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.image_root_identifier)
      << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_identity_source\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics
                              .registration_descriptor_identity_source)
      << "\",\"runtime_bootstrap_legality_semantics_image_root_identity_source\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.image_root_identity_source)
      << "\",\"runtime_bootstrap_legality_semantics_translation_unit_registration_order_ordinal\":"
      << runtime_bootstrap_legality_semantics
             .translation_unit_registration_order_ordinal
      << ",\"runtime_bootstrap_legality_semantics_fail_closed\":"
      << (runtime_bootstrap_legality_semantics.fail_closed ? "true" : "false")
      << ",\"runtime_bootstrap_legality_semantics_semantic_boundary_ready\":"
      << (runtime_bootstrap_legality_semantics.semantic_boundary_ready ? "true"
                                                                      : "false")
      << ",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_ready\":"
      << (runtime_bootstrap_legality_semantics
                  .bootstrap_legality_failure_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_contract_ready\":"
      << (runtime_bootstrap_legality_semantics
                  .registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_contract_ready\":"
      << (runtime_bootstrap_legality_semantics.bootstrap_semantics_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_duplicate_registration_semantics_landed\":"
      << (runtime_bootstrap_legality_semantics
                  .duplicate_registration_semantics_landed
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_image_order_semantics_landed\":"
      << (runtime_bootstrap_legality_semantics.image_order_semantics_landed
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_cross_image_legality_semantics_landed\":"
      << (runtime_bootstrap_legality_semantics
                  .cross_image_legality_semantics_landed
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_semantic_diagnostics_landed\":"
      << (runtime_bootstrap_legality_semantics.semantic_diagnostics_landed
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_ready_for_lowering_and_runtime\":"
      << (runtime_bootstrap_legality_semantics.ready_for_lowering_and_runtime
              ? "true"
              : "false")
      << ",\"runtime_bootstrap_legality_semantics_semantic_boundary_replay_key\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.semantic_boundary_replay_key)
      << "\",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_replay_key\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics
                 .bootstrap_legality_failure_contract_replay_key)
      << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics
                 .registration_descriptor_frontend_closure_replay_key)
      << "\",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(
             runtime_bootstrap_legality_semantics.bootstrap_semantics_replay_key)
      << "\",\"runtime_bootstrap_legality_semantics_replay_key\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics.replay_key)
      << "\",\"runtime_bootstrap_legality_semantics_failure_reason\":\""
      << EscapeJsonString(runtime_bootstrap_legality_semantics.failure_reason)
      << "\"";
}

}  // namespace objc3::artifacts::frontend
