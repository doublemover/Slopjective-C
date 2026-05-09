#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeStartupBootstrapInvariantReplayKey(
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
BuildRuntimeStartupBootstrapInvariantSummary(
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
    summary.replay_key =
        BuildRuntimeStartupBootstrapInvariantReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(summary)) {
    summary.failure_reason =
        "runtime startup bootstrap invariant summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeStartupBootstrapInvariantSummaryJson(
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

std::string BuildRuntimeBootstrapSemanticsReplayKey(
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

Objc3RuntimeBootstrapSemanticsSummary BuildRuntimeBootstrapSemanticsSummary(
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
    summary.replay_key = BuildRuntimeBootstrapSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapSemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap semantics summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapSemanticsSummaryJson(
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

std::string BuildRuntimeBootstrapLoweringReplayKey(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";registration_descriptor_frontend_closure_contract_id="
      << summary.registration_descriptor_frontend_closure_contract_id
      << ";registration_descriptor_artifact="
      << summary.registration_descriptor_artifact
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";lowering_boundary_model=" << summary.lowering_boundary_model
      << ";registration_descriptor_handoff_model="
      << summary.registration_descriptor_handoff_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";registration_table_symbol_prefix="
      << summary.registration_table_symbol_prefix
      << ";image_local_init_state_symbol_prefix="
      << summary.image_local_init_state_symbol_prefix
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";global_ctor_list_model=" << summary.global_ctor_list_model
      << ";registration_table_layout_model="
      << summary.registration_table_layout_model
      << ";image_local_initialization_model="
      << summary.image_local_initialization_model
      << ";registration_table_abi_version="
      << summary.registration_table_abi_version
      << ";registration_table_pointer_field_count="
      << summary.registration_table_pointer_field_count
      << ";constructor_root_emission_state="
      << summary.constructor_root_emission_state
      << ";init_stub_emission_state=" << summary.init_stub_emission_state
      << ";registration_table_emission_state="
      << summary.registration_table_emission_state
      << ";bootstrap_ir_materialization_landed="
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ";image_local_initialization_landed="
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key
      << ";registration_descriptor_frontend_closure_replay_key="
      << summary.registration_descriptor_frontend_closure_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapLoweringSummary BuildRuntimeBootstrapLoweringSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure) {
  Objc3RuntimeBootstrapLoweringSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.registration_descriptor_frontend_closure_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          registration_descriptor_frontend_closure);
  summary.lowering_contract_published = true;
  summary.manifest_authority_preserved =
      summary.registration_manifest_contract_ready &&
      registration_manifest.constructor_root_manifest_authoritative;
  summary.no_bootstrap_ir_materialization_yet = false;
  summary.bootstrap_ir_materialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  summary.image_local_initialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  summary.ready_for_bootstrap_materialization =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.bootstrap_semantics_contract_ready) {
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }
  if (summary.registration_descriptor_frontend_closure_contract_ready) {
    summary.registration_descriptor_frontend_closure_replay_key =
        registration_descriptor_frontend_closure.replay_key;
  }
  if (summary.ready_for_bootstrap_materialization) {
    summary.replay_key = BuildRuntimeBootstrapLoweringReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLoweringSummary(summary)) {
    summary.failure_reason = "runtime bootstrap lowering summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapLoweringSummaryJson(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_contract_id)
      << "\",\"registration_descriptor_artifact\":\""
      << EscapeJsonString(summary.registration_descriptor_artifact)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"lowering_boundary_model\":\""
      << EscapeJsonString(summary.lowering_boundary_model)
      << "\",\"registration_descriptor_handoff_model\":\""
      << EscapeJsonString(summary.registration_descriptor_handoff_model)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_init_stub_symbol_prefix\":\""
      << EscapeJsonString(summary.constructor_init_stub_symbol_prefix)
      << "\",\"registration_table_symbol_prefix\":\""
      << EscapeJsonString(summary.registration_table_symbol_prefix)
      << "\",\"image_local_init_state_symbol_prefix\":\""
      << EscapeJsonString(summary.image_local_init_state_symbol_prefix)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"global_ctor_list_model\":\""
      << EscapeJsonString(summary.global_ctor_list_model)
      << "\",\"registration_table_layout_model\":\""
      << EscapeJsonString(summary.registration_table_layout_model)
      << "\",\"image_local_initialization_model\":\""
      << EscapeJsonString(summary.image_local_initialization_model)
      << "\",\"registration_table_abi_version\":"
      << summary.registration_table_abi_version
      << ",\"registration_table_pointer_field_count\":"
      << summary.registration_table_pointer_field_count
      << ",\"constructor_root_emission_state\":\""
      << EscapeJsonString(summary.constructor_root_emission_state)
      << "\",\"init_stub_emission_state\":\""
      << EscapeJsonString(summary.init_stub_emission_state)
      << "\",\"registration_table_emission_state\":\""
      << EscapeJsonString(summary.registration_table_emission_state)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLoweringSummary(summary) ? "true"
                                                               : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_contract_ready\":"
      << (summary.registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"lowering_contract_published\":"
      << (summary.lowering_contract_published ? "true" : "false")
      << ",\"manifest_authority_preserved\":"
      << (summary.manifest_authority_preserved ? "true" : "false")
      << ",\"no_bootstrap_ir_materialization_yet\":"
      << (summary.no_bootstrap_ir_materialization_yet ? "true" : "false")
      << ",\"bootstrap_ir_materialization_landed\":"
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ",\"image_local_initialization_landed\":"
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ",\"ready_for_bootstrap_materialization\":"
      << (summary.ready_for_bootstrap_materialization ? "true" : "false")
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

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
      << ";image_root_identity_source="
      << summary.image_root_identity_source
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

std::string BuildRuntimeBootstrapApiReplayKey(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";support_library_core_feature_contract_id="
      << summary.support_library_core_feature_contract_id
      << ";support_library_link_wiring_contract_id="
      << summary.support_library_link_wiring_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";public_header_path=" << summary.public_header_path
      << ";archive_relative_path=" << summary.archive_relative_path
      << ";registration_status_enum_type="
      << summary.registration_status_enum_type
      << ";image_descriptor_type=" << summary.image_descriptor_type
      << ";selector_handle_type=" << summary.selector_handle_type
      << ";registration_snapshot_type="
      << summary.registration_snapshot_type
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";selector_lookup_symbol=" << summary.selector_lookup_symbol
      << ";dispatch_entrypoint_symbol=" << summary.dispatch_entrypoint_symbol
      << ";state_snapshot_symbol=" << summary.state_snapshot_symbol
      << ";reset_for_testing_symbol=" << summary.reset_for_testing_symbol
      << ";registration_result_model=" << summary.registration_result_model
      << ";registration_order_ordinal_model="
      << summary.registration_order_ordinal_model
      << ";runtime_state_locking_model="
      << summary.runtime_state_locking_model
      << ";startup_invocation_model=" << summary.startup_invocation_model
      << ";image_walk_lifecycle_model="
      << summary.image_walk_lifecycle_model
      << ";deterministic_reset_lifecycle_model="
      << summary.deterministic_reset_lifecycle_model
      << ";support_library_core_feature_replay_key="
      << summary.support_library_core_feature_replay_key
      << ";support_library_link_wiring_replay_key="
      << summary.support_library_link_wiring_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapApiSummary BuildRuntimeBootstrapApiSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeBootstrapApiSummary summary;
  summary.fail_closed = true;
  summary.support_library_core_feature_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library);
  summary.support_library_link_wiring_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.api_surface_frozen = true;
  summary.registration_entrypoint_frozen = true;
  summary.selector_lookup_and_dispatch_frozen = true;
  summary.reset_and_snapshot_hooks_frozen = true;
  summary.runtime_probe_required = true;
  summary.image_walk_not_yet_landed = true;
  summary.deterministic_reset_expansion_not_yet_landed = true;
  summary.ready_for_registrar_implementation =
      summary.support_library_core_feature_contract_ready &&
      summary.support_library_link_wiring_contract_ready;
  if (summary.support_library_core_feature_contract_ready) {
    summary.support_library_core_feature_replay_key =
        runtime_support_library.contract_id + ";archive_relative_path=" +
        runtime_support_library.archive_relative_path + ";public_header_path=" +
        runtime_support_library.public_header_path + ";register_image_symbol=" +
        runtime_support_library.register_image_symbol +
        ";lookup_selector_symbol=" +
        runtime_support_library.lookup_selector_symbol +
        ";dispatch_i32_symbol=" + runtime_support_library.dispatch_i32_symbol +
        ";reset_for_testing_symbol=" +
        runtime_support_library.reset_for_testing_symbol;
  }
  if (summary.support_library_link_wiring_contract_ready) {
    summary.support_library_link_wiring_replay_key =
        runtime_support_library_link_wiring.contract_id +
        ";archive_relative_path=" +
        runtime_support_library_link_wiring.archive_relative_path +
        ";runtime_dispatch_symbol=" +
        runtime_support_library_link_wiring.runtime_dispatch_symbol +
        ";driver_link_mode=" +
        runtime_support_library_link_wiring.driver_link_mode;
  }
  if (summary.ready_for_registrar_implementation) {
    summary.replay_key = BuildRuntimeBootstrapApiReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapApiSummary(summary)) {
    summary.failure_reason = "runtime bootstrap api summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapApiSummaryJson(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"support_library_core_feature_contract_id\":\""
      << EscapeJsonString(summary.support_library_core_feature_contract_id)
      << "\",\"support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(summary.support_library_link_wiring_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"public_header_path\":\""
      << EscapeJsonString(summary.public_header_path)
      << "\",\"archive_relative_path\":\""
      << EscapeJsonString(summary.archive_relative_path)
      << "\",\"registration_status_enum_type\":\""
      << EscapeJsonString(summary.registration_status_enum_type)
      << "\",\"image_descriptor_type\":\""
      << EscapeJsonString(summary.image_descriptor_type)
      << "\",\"selector_handle_type\":\""
      << EscapeJsonString(summary.selector_handle_type)
      << "\",\"registration_snapshot_type\":\""
      << EscapeJsonString(summary.registration_snapshot_type)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"selector_lookup_symbol\":\""
      << EscapeJsonString(summary.selector_lookup_symbol)
      << "\",\"dispatch_entrypoint_symbol\":\""
      << EscapeJsonString(summary.dispatch_entrypoint_symbol)
      << "\",\"state_snapshot_symbol\":\""
      << EscapeJsonString(summary.state_snapshot_symbol)
      << "\",\"reset_for_testing_symbol\":\""
      << EscapeJsonString(summary.reset_for_testing_symbol)
      << "\",\"registration_result_model\":\""
      << EscapeJsonString(summary.registration_result_model)
      << "\",\"registration_order_ordinal_model\":\""
      << EscapeJsonString(summary.registration_order_ordinal_model)
      << "\",\"runtime_state_locking_model\":\""
      << EscapeJsonString(summary.runtime_state_locking_model)
      << "\",\"startup_invocation_model\":\""
      << EscapeJsonString(summary.startup_invocation_model)
      << "\",\"image_walk_lifecycle_model\":\""
      << EscapeJsonString(summary.image_walk_lifecycle_model)
      << "\",\"deterministic_reset_lifecycle_model\":\""
      << EscapeJsonString(summary.deterministic_reset_lifecycle_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapApiSummary(summary) ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"support_library_core_feature_contract_ready\":"
      << (summary.support_library_core_feature_contract_ready ? "true"
                                                              : "false")
      << ",\"support_library_link_wiring_contract_ready\":"
      << (summary.support_library_link_wiring_contract_ready ? "true"
                                                             : "false")
      << ",\"api_surface_frozen\":"
      << (summary.api_surface_frozen ? "true" : "false")
      << ",\"registration_entrypoint_frozen\":"
      << (summary.registration_entrypoint_frozen ? "true" : "false")
      << ",\"selector_lookup_and_dispatch_frozen\":"
      << (summary.selector_lookup_and_dispatch_frozen ? "true" : "false")
      << ",\"reset_and_snapshot_hooks_frozen\":"
      << (summary.reset_and_snapshot_hooks_frozen ? "true" : "false")
      << ",\"runtime_probe_required\":"
      << (summary.runtime_probe_required ? "true" : "false")
      << ",\"image_walk_not_yet_landed\":"
      << (summary.image_walk_not_yet_landed ? "true" : "false")
      << ",\"deterministic_reset_expansion_not_yet_landed\":"
      << (summary.deterministic_reset_expansion_not_yet_landed ? "true"
                                                               : "false")
      << ",\"ready_for_registrar_implementation\":"
      << (summary.ready_for_registrar_implementation ? "true" : "false")
      << ",\"support_library_core_feature_replay_key\":\""
      << EscapeJsonString(summary.support_library_core_feature_replay_key)
      << "\",\"support_library_link_wiring_replay_key\":\""
      << EscapeJsonString(summary.support_library_link_wiring_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
