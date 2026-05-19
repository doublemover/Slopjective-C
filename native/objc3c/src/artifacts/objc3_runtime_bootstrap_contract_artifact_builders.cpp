#include "artifacts/objc3_runtime_bootstrap_contract_artifact_builders.h"

#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend::runtime_bootstrap_contracts {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string RuntimeStartupBootstrapInvariantArtifactBuilder::BuildReplayKey(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";realization_order_policy=" << summary.realization_order_policy
      << ";failure_mode=" << summary.failure_mode
      << ";image_local_initialization_scope="
      << summary.image_local_initialization_scope
      << ";constructor_root_uniqueness_policy="
      << summary.constructor_root_uniqueness_policy
      << ";constructor_root_consumption_model="
      << summary.constructor_root_consumption_model
      << ";startup_execution_mode=" << summary.startup_execution_mode
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeStartupBootstrapInvariantSummary
RuntimeStartupBootstrapInvariantArtifactBuilder::BuildSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeStartupBootstrapInvariantSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.duplicate_registration_semantics_frozen = true;
  summary.realization_order_semantics_frozen = true;
  summary.failure_mode_semantics_frozen = true;
  summary.image_local_initialization_scope_frozen = true;
  summary.constructor_root_uniqueness_frozen = true;
  summary.startup_execution_not_yet_landed = true;
  summary.live_duplicate_registration_enforcement_not_yet_landed = true;
  summary.image_local_realization_not_yet_landed = true;
  summary.ready_for_bootstrap_implementation =
      summary.registration_manifest_contract_ready;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_bootstrap_implementation) {
    summary.replay_key = BuildReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(summary)) {
    summary.failure_reason =
        "runtime startup bootstrap invariant summary is incomplete";
  }
  return summary;
}

std::string RuntimeStartupBootstrapInvariantArtifactBuilder::BuildSummaryJson(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"realization_order_policy\":\""
      << EscapeJsonString(summary.realization_order_policy)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"image_local_initialization_scope\":\""
      << EscapeJsonString(summary.image_local_initialization_scope)
      << "\",\"constructor_root_uniqueness_policy\":\""
      << EscapeJsonString(summary.constructor_root_uniqueness_policy)
      << "\",\"constructor_root_consumption_model\":\""
      << EscapeJsonString(summary.constructor_root_consumption_model)
      << "\",\"startup_execution_mode\":\""
      << EscapeJsonString(summary.startup_execution_mode)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(summary) ? "true"
                                                                       : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"duplicate_registration_semantics_frozen\":"
      << (summary.duplicate_registration_semantics_frozen ? "true" : "false")
      << ",\"realization_order_semantics_frozen\":"
      << (summary.realization_order_semantics_frozen ? "true" : "false")
      << ",\"failure_mode_semantics_frozen\":"
      << (summary.failure_mode_semantics_frozen ? "true" : "false")
      << ",\"image_local_initialization_scope_frozen\":"
      << (summary.image_local_initialization_scope_frozen ? "true" : "false")
      << ",\"constructor_root_uniqueness_frozen\":"
      << (summary.constructor_root_uniqueness_frozen ? "true" : "false")
      << ",\"startup_execution_not_yet_landed\":"
      << (summary.startup_execution_not_yet_landed ? "true" : "false")
      << ",\"live_duplicate_registration_enforcement_not_yet_landed\":"
      << (summary.live_duplicate_registration_enforcement_not_yet_landed ? "true"
                                                                         : "false")
      << ",\"image_local_realization_not_yet_landed\":"
      << (summary.image_local_realization_not_yet_landed ? "true" : "false")
      << ",\"ready_for_bootstrap_implementation\":"
      << (summary.ready_for_bootstrap_implementation ? "true" : "false")
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string RuntimeBootstrapSemanticsArtifactBuilder::BuildReplayKey(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_invariant_contract_id="
      << summary.bootstrap_invariant_contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";realization_order_policy=" << summary.realization_order_policy
      << ";failure_mode=" << summary.failure_mode
      << ";image_local_initialization_scope="
      << summary.image_local_initialization_scope
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";runtime_library_archive_relative_path="
      << summary.runtime_library_archive_relative_path
      << ";registration_result_model=" << summary.registration_result_model
      << ";registration_order_ordinal_model="
      << summary.registration_order_ordinal_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol
      << ";success_status_code=" << summary.success_status_code
      << ";invalid_descriptor_status_code="
      << summary.invalid_descriptor_status_code
      << ";duplicate_registration_status_code="
      << summary.duplicate_registration_status_code
      << ";out_of_order_status_code="
      << summary.out_of_order_status_code
      << ";invalid_registration_roots_status_code="
      << summary.invalid_registration_roots_status_code
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";bootstrap_invariant_replay_key="
      << summary.bootstrap_invariant_replay_key
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapSemanticsSummary
RuntimeBootstrapSemanticsArtifactBuilder::BuildSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &bootstrap_invariants,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeBootstrapSemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_invariant_contract_ready =
      IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(
          bootstrap_invariants);
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.registration_manifest_bootstrap_semantics_published =
      summary.registration_manifest_contract_ready;
  summary.live_runtime_enforcement_landed = true;
  summary.runtime_probe_required = true;
  summary.no_partial_commit_on_failure = true;
  summary.ready_for_constructor_root_implementation =
      summary.bootstrap_invariant_contract_ready &&
      summary.registration_manifest_contract_ready;
  summary.translation_unit_registration_order_ordinal =
      registration_manifest.translation_unit_registration_order_ordinal;
  if (summary.bootstrap_invariant_contract_ready) {
    summary.bootstrap_invariant_replay_key = bootstrap_invariants.replay_key;
  }
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_constructor_root_implementation) {
    summary.replay_key = BuildReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapSemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap semantics summary is incomplete";
  }
  return summary;
}

std::string RuntimeBootstrapSemanticsArtifactBuilder::BuildSummaryJson(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"bootstrap_invariant_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_invariant_contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"realization_order_policy\":\""
      << EscapeJsonString(summary.realization_order_policy)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"image_local_initialization_scope\":\""
      << EscapeJsonString(summary.image_local_initialization_scope)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"runtime_library_archive_relative_path\":\""
      << EscapeJsonString(summary.runtime_library_archive_relative_path)
      << "\",\"registration_result_model\":\""
      << EscapeJsonString(summary.registration_result_model)
      << "\",\"registration_order_ordinal_model\":\""
      << EscapeJsonString(summary.registration_order_ordinal_model)
      << "\",\"runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.runtime_state_snapshot_symbol)
      << "\",\"success_status_code\":" << summary.success_status_code
      << ",\"invalid_descriptor_status_code\":"
      << summary.invalid_descriptor_status_code
      << ",\"duplicate_registration_status_code\":"
      << summary.duplicate_registration_status_code
      << ",\"out_of_order_status_code\":"
      << summary.out_of_order_status_code
      << ",\"invalid_registration_roots_status_code\":"
      << summary.invalid_registration_roots_status_code
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapSemanticsSummary(summary) ? "true"
                                                                : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"bootstrap_invariant_contract_ready\":"
      << (summary.bootstrap_invariant_contract_ready ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"live_runtime_enforcement_landed\":"
      << (summary.live_runtime_enforcement_landed ? "true" : "false")
      << ",\"registration_manifest_bootstrap_semantics_published\":"
      << (summary.registration_manifest_bootstrap_semantics_published ? "true"
                                                                      : "false")
      << ",\"runtime_probe_required\":"
      << (summary.runtime_probe_required ? "true" : "false")
      << ",\"no_partial_commit_on_failure\":"
      << (summary.no_partial_commit_on_failure ? "true" : "false")
      << ",\"ready_for_constructor_root_implementation\":"
      << (summary.ready_for_constructor_root_implementation ? "true" : "false")
      << ",\"bootstrap_invariant_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_invariant_replay_key)
      << "\",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend::runtime_bootstrap_contracts
