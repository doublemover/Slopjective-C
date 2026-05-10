#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeBootstrapLegalityFailureContractReplayKey(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_descriptor_frontend_closure_contract_id="
      << summary.registration_descriptor_frontend_closure_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identity_source=" << summary.image_root_identity_source
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";registration_descriptor_frontend_closure_replay_key="
      << summary.registration_descriptor_frontend_closure_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key
      << ";semantic_boundary_replay_key="
      << summary.semantic_boundary_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapLegalityFailureContractSummary
BuildRuntimeBootstrapLegalityFailureContractSummary(
    const Objc3BootstrapLegalityFailureContractSummary &semantic_boundary,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics) {
  Objc3RuntimeBootstrapLegalityFailureContractSummary summary;
  summary.fail_closed = true;
  summary.registration_descriptor_frontend_closure_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          registration_descriptor_frontend_closure);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.semantic_boundary_ready =
      IsReadyObjc3BootstrapLegalityFailureContractSummary(semantic_boundary);

  if (summary.semantic_boundary_ready) {
    summary.duplicate_registration_policy =
        semantic_boundary.duplicate_registration_policy;
    summary.image_registration_order_invariant =
        semantic_boundary.image_registration_order_invariant;
    summary.failure_mode = semantic_boundary.failure_mode;
    summary.restart_lifecycle_model =
        semantic_boundary.restart_lifecycle_model;
    summary.replay_order_model = semantic_boundary.replay_order_model;
    summary.image_local_init_reset_model =
        semantic_boundary.image_local_init_reset_model;
    summary.catalog_retention_model =
        semantic_boundary.catalog_retention_model;
    summary.runtime_state_snapshot_symbol =
        semantic_boundary.runtime_state_snapshot_symbol;
    summary.duplicate_registration_policy_frozen =
        semantic_boundary.duplicate_registration_policy_frozen;
    summary.image_order_invariant_frozen =
        semantic_boundary.image_order_invariant_frozen;
    summary.bootstrap_rejection_frozen =
        semantic_boundary.bootstrap_rejection_frozen;
    summary.restart_boundary_frozen =
        semantic_boundary.restart_boundary_frozen;
    summary.semantic_diagnostics_required =
        semantic_boundary.semantic_diagnostics_required;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
  }

  if (summary.registration_descriptor_frontend_closure_contract_ready) {
    summary.registration_descriptor_identifier =
        registration_descriptor_frontend_closure
            .registration_descriptor_identifier;
    summary.image_root_identifier =
        registration_descriptor_frontend_closure.image_root_identifier;
    summary.registration_descriptor_identity_source =
        registration_descriptor_frontend_closure
            .registration_descriptor_identity_source;
    summary.image_root_identity_source =
        registration_descriptor_frontend_closure.image_root_identity_source;
    summary.translation_unit_registration_order_ordinal =
        registration_descriptor_frontend_closure
            .translation_unit_registration_order_ordinal;
    summary.registration_descriptor_frontend_closure_replay_key =
        registration_descriptor_frontend_closure.replay_key;
  }

  if (summary.bootstrap_semantics_contract_ready) {
    summary.translation_unit_registration_order_ordinal =
        bootstrap_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }

  summary.ready_for_lowering_and_runtime =
      summary.registration_descriptor_frontend_closure_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.semantic_boundary_ready &&
      summary.duplicate_registration_policy ==
          bootstrap_semantics.duplicate_registration_policy &&
      summary.image_registration_order_invariant ==
          bootstrap_semantics.registration_order_ordinal_model &&
      summary.failure_mode == bootstrap_semantics.failure_mode &&
      summary.runtime_state_snapshot_symbol ==
          bootstrap_semantics.runtime_state_snapshot_symbol &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_semantics.translation_unit_registration_order_ordinal;

  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildRuntimeBootstrapLegalityFailureContractReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap legality duplicate/order/failure contract summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapLegalityFailureContractSummaryJson(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"image_registration_order_invariant\":\""
      << EscapeJsonString(summary.image_registration_order_invariant)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"restart_lifecycle_model\":\""
      << EscapeJsonString(summary.restart_lifecycle_model)
      << "\",\"replay_order_model\":\""
      << EscapeJsonString(summary.replay_order_model)
      << "\",\"image_local_init_reset_model\":\""
      << EscapeJsonString(summary.image_local_init_reset_model)
      << "\",\"catalog_retention_model\":\""
      << EscapeJsonString(summary.catalog_retention_model)
      << "\",\"runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.runtime_state_snapshot_symbol)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_contract_ready\":"
      << (summary.registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"duplicate_registration_policy_frozen\":"
      << (summary.duplicate_registration_policy_frozen ? "true" : "false")
      << ",\"image_order_invariant_frozen\":"
      << (summary.image_order_invariant_frozen ? "true" : "false")
      << ",\"bootstrap_rejection_frozen\":"
      << (summary.bootstrap_rejection_frozen ? "true" : "false")
      << ",\"restart_boundary_frozen\":"
      << (summary.restart_boundary_frozen ? "true" : "false")
      << ",\"semantic_diagnostics_required\":"
      << (summary.semantic_diagnostics_required ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
