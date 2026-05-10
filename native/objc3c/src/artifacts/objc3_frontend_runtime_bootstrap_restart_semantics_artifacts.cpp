#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeBootstrapFailureRestartSemanticsReplayKey(
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_semantics_contract_id="
      << summary.bootstrap_legality_semantics_contract_id
      << ";bootstrap_reset_contract_id=" << summary.bootstrap_reset_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";unsupported_topology_model=" << summary.unsupported_topology_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";translation_unit_identity_key="
      << summary.translation_unit_identity_key
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol
      << ";replay_registered_images_symbol="
      << summary.replay_registered_images_symbol
      << ";reset_replay_state_snapshot_symbol="
      << summary.reset_replay_state_snapshot_symbol
      << ";invalid_descriptor_status_code="
      << summary.invalid_descriptor_status_code
      << ";invalid_registration_roots_status_code="
      << summary.invalid_registration_roots_status_code
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";semantic_boundary_replay_key=" << summary.semantic_boundary_replay_key
      << ";bootstrap_legality_semantics_replay_key="
      << summary.bootstrap_legality_semantics_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapFailureRestartSemanticsSummary
BuildRuntimeBootstrapFailureRestartSemanticsSummary(
    const Objc3BootstrapFailureRestartSemanticsSummary &semantic_boundary,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const Objc3RuntimeBootstrapApiSummary &bootstrap_api,
    const Objc3RuntimeBootstrapLoweringSummary &bootstrap_lowering,
    const std::string &translation_unit_identity_key) {
  Objc3RuntimeBootstrapFailureRestartSemanticsSummary summary;
  summary.fail_closed = true;
  summary.semantic_boundary_ready =
      IsReadyObjc3BootstrapFailureRestartSemanticsSummary(semantic_boundary);
  summary.bootstrap_legality_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(
          bootstrap_legality_semantics);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.bootstrap_reset_contract_ready =
      IsReadyObjc3RuntimeBootstrapApiSummary(bootstrap_api) &&
      IsReadyObjc3RuntimeBootstrapLoweringSummary(bootstrap_lowering);
  summary.translation_unit_identity_key = translation_unit_identity_key;

  if (summary.semantic_boundary_ready) {
    summary.failure_mode = semantic_boundary.failure_mode;
    summary.restart_lifecycle_model = semantic_boundary.restart_lifecycle_model;
    summary.replay_order_model = semantic_boundary.replay_order_model;
    summary.image_local_init_reset_model =
        semantic_boundary.image_local_init_reset_model;
    summary.catalog_retention_model = semantic_boundary.catalog_retention_model;
    summary.unsupported_topology_model =
        semantic_boundary.unsupported_topology_model;
    summary.failure_mode_semantics_landed =
        semantic_boundary.failure_mode_semantics_landed;
    summary.restart_semantics_landed =
        semantic_boundary.restart_semantics_landed;
    summary.replay_semantics_landed = semantic_boundary.replay_semantics_landed;
    summary.unsupported_topology_semantics_landed =
        semantic_boundary.unsupported_topology_semantics_landed;
    summary.deterministic_recovery_semantics_landed =
        semantic_boundary.deterministic_recovery_semantics_landed;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
  }

  if (summary.bootstrap_legality_semantics_contract_ready) {
    summary.translation_unit_identity_model =
        bootstrap_legality_semantics.translation_unit_identity_model;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_legality_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_legality_semantics_replay_key =
        bootstrap_legality_semantics.replay_key;
  }

  if (summary.bootstrap_semantics_contract_ready) {
    summary.translation_unit_identity_model =
        bootstrap_semantics.translation_unit_identity_model;
    summary.runtime_state_snapshot_symbol =
        bootstrap_semantics.runtime_state_snapshot_symbol;
    summary.invalid_descriptor_status_code =
        bootstrap_semantics.invalid_descriptor_status_code;
    summary.invalid_registration_roots_status_code =
        bootstrap_semantics.invalid_registration_roots_status_code;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }

  summary.runtime_restart_probe_required = true;
  summary.ready_for_lowering_and_runtime =
      summary.semantic_boundary_ready &&
      summary.bootstrap_legality_semantics_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.bootstrap_reset_contract_ready &&
      summary.failure_mode_semantics_landed &&
      summary.restart_semantics_landed &&
      summary.replay_semantics_landed &&
      summary.unsupported_topology_semantics_landed &&
      summary.deterministic_recovery_semantics_landed &&
      summary.failure_mode == bootstrap_semantics.failure_mode &&
      summary.restart_lifecycle_model ==
          kObjc3RuntimeBootstrapResetLifecycleModel &&
      summary.replay_order_model == kObjc3RuntimeBootstrapReplayOrderModel &&
      summary.image_local_init_reset_model ==
          kObjc3RuntimeBootstrapImageLocalInitStateResetModel &&
      summary.catalog_retention_model ==
          kObjc3RuntimeBootstrapCatalogRetentionModel &&
      summary.translation_unit_identity_model ==
          bootstrap_legality_semantics.translation_unit_identity_model &&
      summary.translation_unit_identity_model ==
          bootstrap_semantics.translation_unit_identity_model &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_legality_semantics.translation_unit_registration_order_ordinal &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_semantics.translation_unit_registration_order_ordinal &&
      !summary.translation_unit_identity_key.empty();

  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildRuntimeBootstrapFailureRestartSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap failure-mode and restart semantics summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapFailureRestartSemanticsSummaryJson(
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"bootstrap_legality_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_legality_semantics_contract_id)
      << "\",\"bootstrap_reset_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_reset_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"failure_mode\":\"" << EscapeJsonString(summary.failure_mode)
      << "\",\"restart_lifecycle_model\":\""
      << EscapeJsonString(summary.restart_lifecycle_model)
      << "\",\"replay_order_model\":\""
      << EscapeJsonString(summary.replay_order_model)
      << "\",\"image_local_init_reset_model\":\""
      << EscapeJsonString(summary.image_local_init_reset_model)
      << "\",\"catalog_retention_model\":\""
      << EscapeJsonString(summary.catalog_retention_model)
      << "\",\"unsupported_topology_model\":\""
      << EscapeJsonString(summary.unsupported_topology_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"translation_unit_identity_key\":\""
      << EscapeJsonString(summary.translation_unit_identity_key)
      << "\",\"runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.runtime_state_snapshot_symbol)
      << "\",\"replay_registered_images_symbol\":\""
      << EscapeJsonString(summary.replay_registered_images_symbol)
      << "\",\"reset_replay_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.reset_replay_state_snapshot_symbol)
      << "\",\"invalid_descriptor_status_code\":"
      << summary.invalid_descriptor_status_code
      << ",\"invalid_registration_roots_status_code\":"
      << summary.invalid_registration_roots_status_code
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"bootstrap_legality_semantics_contract_ready\":"
      << (summary.bootstrap_legality_semantics_contract_ready ? "true"
                                                              : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"bootstrap_reset_contract_ready\":"
      << (summary.bootstrap_reset_contract_ready ? "true" : "false")
      << ",\"failure_mode_semantics_landed\":"
      << (summary.failure_mode_semantics_landed ? "true" : "false")
      << ",\"restart_semantics_landed\":"
      << (summary.restart_semantics_landed ? "true" : "false")
      << ",\"replay_semantics_landed\":"
      << (summary.replay_semantics_landed ? "true" : "false")
      << ",\"unsupported_topology_semantics_landed\":"
      << (summary.unsupported_topology_semantics_landed ? "true" : "false")
      << ",\"deterministic_recovery_semantics_landed\":"
      << (summary.deterministic_recovery_semantics_landed ? "true"
                                                          : "false")
      << ",\"runtime_restart_probe_required\":"
      << (summary.runtime_restart_probe_required ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"bootstrap_legality_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_legality_semantics_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
