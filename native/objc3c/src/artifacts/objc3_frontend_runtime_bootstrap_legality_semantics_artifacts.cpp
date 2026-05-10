#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeBootstrapLegalitySemanticsReplayKey(
    const Objc3RuntimeBootstrapLegalitySemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_failure_contract_id="
      << summary.bootstrap_legality_failure_contract_id
      << ";registration_descriptor_frontend_closure_contract_id="
      << summary.registration_descriptor_frontend_closure_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";cross_image_legality_model=" << summary.cross_image_legality_model
      << ";semantic_diagnostic_model=" << summary.semantic_diagnostic_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";translation_unit_identity_key="
      << summary.translation_unit_identity_key
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identity_source=" << summary.image_root_identity_source
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";semantic_boundary_replay_key=" << summary.semantic_boundary_replay_key
      << ";bootstrap_legality_failure_contract_replay_key="
      << summary.bootstrap_legality_failure_contract_replay_key
      << ";registration_descriptor_frontend_closure_replay_key="
      << summary.registration_descriptor_frontend_closure_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapLegalitySemanticsSummary
BuildRuntimeBootstrapLegalitySemanticsSummary(
    const Objc3BootstrapLegalitySemanticsSummary &semantic_boundary,
    const Objc3RuntimeBootstrapLegalityFailureContractSummary
        &bootstrap_legality_failure_contract,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const std::string &translation_unit_identity_key) {
  Objc3RuntimeBootstrapLegalitySemanticsSummary summary;
  summary.fail_closed = true;
  summary.semantic_boundary_ready =
      IsReadyObjc3BootstrapLegalitySemanticsSummary(semantic_boundary);
  summary.bootstrap_legality_failure_contract_ready =
      IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(
          bootstrap_legality_failure_contract);
  summary.registration_descriptor_frontend_closure_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          registration_descriptor_frontend_closure);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.translation_unit_identity_key = translation_unit_identity_key;

  if (summary.semantic_boundary_ready) {
    summary.duplicate_registration_policy =
        semantic_boundary.duplicate_registration_policy;
    summary.image_registration_order_invariant =
        semantic_boundary.image_registration_order_invariant;
    summary.cross_image_legality_model =
        semantic_boundary.cross_image_legality_model;
    summary.semantic_diagnostic_model =
        semantic_boundary.semantic_diagnostic_model;
    summary.duplicate_registration_semantics_landed =
        semantic_boundary.duplicate_registration_semantics_landed;
    summary.image_order_semantics_landed =
        semantic_boundary.image_order_semantics_landed;
    summary.cross_image_legality_semantics_landed =
        semantic_boundary.cross_image_legality_semantics_landed;
    summary.semantic_diagnostics_landed =
        semantic_boundary.semantic_diagnostics_landed;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
  }

  if (summary.bootstrap_legality_failure_contract_ready) {
    summary.registration_descriptor_identifier =
        bootstrap_legality_failure_contract.registration_descriptor_identifier;
    summary.image_root_identifier =
        bootstrap_legality_failure_contract.image_root_identifier;
    summary.registration_descriptor_identity_source =
        bootstrap_legality_failure_contract
            .registration_descriptor_identity_source;
    summary.image_root_identity_source =
        bootstrap_legality_failure_contract.image_root_identity_source;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_legality_failure_contract
            .translation_unit_registration_order_ordinal;
    summary.bootstrap_legality_failure_contract_replay_key =
        bootstrap_legality_failure_contract.replay_key;
  }

  if (summary.registration_descriptor_frontend_closure_contract_ready) {
    summary.translation_unit_identity_model =
        registration_descriptor_frontend_closure.translation_unit_identity_model;
    summary.registration_descriptor_frontend_closure_replay_key =
        registration_descriptor_frontend_closure.replay_key;
  }

  if (summary.bootstrap_semantics_contract_ready) {
    summary.translation_unit_identity_model =
        bootstrap_semantics.translation_unit_identity_model;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }

  summary.ready_for_lowering_and_runtime =
      summary.semantic_boundary_ready &&
      summary.bootstrap_legality_failure_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.duplicate_registration_semantics_landed &&
      summary.image_order_semantics_landed &&
      summary.cross_image_legality_semantics_landed &&
      summary.semantic_diagnostics_landed &&
      summary.duplicate_registration_policy ==
          bootstrap_legality_failure_contract.duplicate_registration_policy &&
      summary.duplicate_registration_policy ==
          bootstrap_semantics.duplicate_registration_policy &&
      summary.image_registration_order_invariant ==
          bootstrap_legality_failure_contract.image_registration_order_invariant &&
      summary.image_registration_order_invariant ==
          bootstrap_semantics.registration_order_ordinal_model &&
      summary.translation_unit_identity_model ==
          registration_descriptor_frontend_closure.translation_unit_identity_model &&
      summary.translation_unit_identity_model ==
          bootstrap_semantics.translation_unit_identity_model &&
      summary.registration_descriptor_identifier ==
          registration_descriptor_frontend_closure
              .registration_descriptor_identifier &&
      summary.image_root_identifier ==
          registration_descriptor_frontend_closure.image_root_identifier &&
      summary.registration_descriptor_identity_source ==
          registration_descriptor_frontend_closure
              .registration_descriptor_identity_source &&
      summary.image_root_identity_source ==
          registration_descriptor_frontend_closure.image_root_identity_source &&
      summary.translation_unit_registration_order_ordinal ==
          registration_descriptor_frontend_closure
              .translation_unit_registration_order_ordinal &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_semantics.translation_unit_registration_order_ordinal &&
      !summary.translation_unit_identity_key.empty();

  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildRuntimeBootstrapLegalitySemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap legality duplicate/order semantics summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapLegalitySemanticsSummaryJson(
    const Objc3RuntimeBootstrapLegalitySemanticsSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"bootstrap_legality_failure_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_legality_failure_contract_id)
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
      << "\",\"cross_image_legality_model\":\""
      << EscapeJsonString(summary.cross_image_legality_model)
      << "\",\"semantic_diagnostic_model\":\""
      << EscapeJsonString(summary.semantic_diagnostic_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"translation_unit_identity_key\":\""
      << EscapeJsonString(summary.translation_unit_identity_key)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"bootstrap_legality_failure_contract_ready\":"
      << (summary.bootstrap_legality_failure_contract_ready ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_contract_ready\":"
      << (summary.registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"duplicate_registration_semantics_landed\":"
      << (summary.duplicate_registration_semantics_landed ? "true" : "false")
      << ",\"image_order_semantics_landed\":"
      << (summary.image_order_semantics_landed ? "true" : "false")
      << ",\"cross_image_legality_semantics_landed\":"
      << (summary.cross_image_legality_semantics_landed ? "true" : "false")
      << ",\"semantic_diagnostics_landed\":"
      << (summary.semantic_diagnostics_landed ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"bootstrap_legality_failure_contract_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_legality_failure_contract_replay_key)
      << "\",\"registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
