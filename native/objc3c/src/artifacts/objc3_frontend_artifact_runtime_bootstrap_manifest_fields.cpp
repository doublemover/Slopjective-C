#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_manifest_fields.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering) {
  manifest << ",\"runtime_bootstrap_failure_restart_semantics_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_legality_semantics_contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_reset_contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_semantics_contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_frontend_surface_path\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .frontend_surface_path)
           << "\",\"runtime_bootstrap_failure_restart_semantics_failure_mode\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.failure_mode)
           << "\",\"runtime_bootstrap_failure_restart_semantics_restart_lifecycle_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .restart_lifecycle_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_replay_order_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.replay_order_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_image_local_init_reset_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .image_local_init_reset_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_catalog_retention_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .catalog_retention_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .unsupported_topology_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .translation_unit_identity_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .translation_unit_identity_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_runtime_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .runtime_state_snapshot_symbol)
           << "\",\"runtime_bootstrap_failure_restart_semantics_replay_registered_images_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .replay_registered_images_symbol)
           << "\",\"runtime_bootstrap_failure_restart_semantics_reset_replay_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .reset_replay_state_snapshot_symbol)
           << "\",\"runtime_bootstrap_failure_restart_semantics_invalid_descriptor_status_code\":"
           << runtime_bootstrap_failure_restart_semantics
                  .invalid_descriptor_status_code
           << ",\"runtime_bootstrap_failure_restart_semantics_translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_failure_restart_semantics
                  .translation_unit_registration_order_ordinal
           << ",\"runtime_bootstrap_failure_restart_semantics_fail_closed\":"
           << (runtime_bootstrap_failure_restart_semantics.fail_closed ? "true"
                                                                       : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .semantic_boundary_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .bootstrap_legality_semantics_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .bootstrap_semantics_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .bootstrap_reset_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_failure_mode_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .failure_mode_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_restart_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .restart_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_replay_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .replay_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .unsupported_topology_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_deterministic_recovery_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .deterministic_recovery_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_runtime_restart_probe_required\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .runtime_restart_probe_required
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_ready_for_lowering_and_runtime\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .ready_for_lowering_and_runtime
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .semantic_boundary_replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_legality_semantics_replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_semantics_replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_failure_reason\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.failure_reason)
           << "\""
           << ",\"runtime_bootstrap_api_contract_id\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"runtime_bootstrap_api_public_header_path\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.public_header_path)
           << "\",\"runtime_bootstrap_api_archive_relative_path\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.archive_relative_path)
           << "\",\"runtime_bootstrap_api_registration_status_enum_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_status_enum_type)
           << "\",\"runtime_bootstrap_api_image_descriptor_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.image_descriptor_type)
           << "\",\"runtime_bootstrap_api_selector_handle_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.selector_handle_type)
           << "\",\"runtime_bootstrap_api_registration_snapshot_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_snapshot_type)
           << "\",\"runtime_bootstrap_api_registration_entrypoint_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_entrypoint_symbol)
           << "\",\"runtime_bootstrap_api_selector_lookup_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.selector_lookup_symbol)
           << "\",\"runtime_bootstrap_api_dispatch_entrypoint_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.dispatch_entrypoint_symbol)
           << "\",\"runtime_bootstrap_api_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.state_snapshot_symbol)
           << "\",\"runtime_bootstrap_api_reset_for_testing_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.reset_for_testing_symbol)
           << "\",\"runtime_bootstrap_api_registration_result_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_result_model)
           << "\",\"runtime_bootstrap_api_registration_order_ordinal_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_order_ordinal_model)
           << "\",\"runtime_bootstrap_api_runtime_state_locking_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.runtime_state_locking_model)
           << "\",\"runtime_bootstrap_api_startup_invocation_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.startup_invocation_model)
           << "\",\"runtime_bootstrap_api_image_walk_lifecycle_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.image_walk_lifecycle_model)
           << "\",\"runtime_bootstrap_api_deterministic_reset_lifecycle_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.deterministic_reset_lifecycle_model)
           << "\",\"runtime_bootstrap_api_ready_for_registrar_implementation\":"
           << (runtime_bootstrap_api.ready_for_registrar_implementation ? "true"
                                                                        : "false")
           << ",\"runtime_bootstrap_api_support_library_core_feature_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.support_library_core_feature_replay_key)
           << "\",\"runtime_bootstrap_api_support_library_link_wiring_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.support_library_link_wiring_replay_key)
           << "\""
           << ",\"runtime_bootstrap_api_replay_key\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.replay_key)
           << "\",\"runtime_bootstrap_api_failure_reason\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.failure_reason)
           << "\""
           << ",\"runtime_bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_semantics.contract_id
           << "\",\"runtime_bootstrap_semantics_bootstrap_invariant_contract_id\":\""
           << runtime_bootstrap_semantics.bootstrap_invariant_contract_id
           << "\",\"runtime_bootstrap_semantics_registration_manifest_contract_id\":\""
           << runtime_bootstrap_semantics.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_semantics_duplicate_registration_policy\":\""
           << runtime_bootstrap_semantics.duplicate_registration_policy
           << "\",\"runtime_bootstrap_semantics_realization_order_policy\":\""
           << runtime_bootstrap_semantics.realization_order_policy
           << "\",\"runtime_bootstrap_semantics_failure_mode\":\""
           << runtime_bootstrap_semantics.failure_mode
           << "\",\"runtime_bootstrap_semantics_result_model\":\""
           << runtime_bootstrap_semantics.registration_result_model
           << "\",\"runtime_bootstrap_semantics_registration_order_ordinal_model\":\""
           << runtime_bootstrap_semantics.registration_order_ordinal_model
           << "\",\"runtime_bootstrap_semantics_runtime_state_snapshot_symbol\":\""
           << runtime_bootstrap_semantics.runtime_state_snapshot_symbol
           << "\",\"runtime_bootstrap_semantics_runtime_library_archive_relative_path\":\""
           << runtime_bootstrap_semantics.runtime_library_archive_relative_path
           << "\",\"runtime_bootstrap_semantics_translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_semantics
                  .translation_unit_registration_order_ordinal
           << ",\"runtime_bootstrap_semantics_success_status_code\":"
           << runtime_bootstrap_semantics.success_status_code
           << ",\"runtime_bootstrap_semantics_invalid_descriptor_status_code\":"
           << runtime_bootstrap_semantics.invalid_descriptor_status_code
           << ",\"runtime_bootstrap_semantics_duplicate_registration_status_code\":"
           << runtime_bootstrap_semantics.duplicate_registration_status_code
           << ",\"runtime_bootstrap_semantics_out_of_order_status_code\":"
           << runtime_bootstrap_semantics.out_of_order_status_code
           << ",\"runtime_bootstrap_semantics_fail_closed\":"
           << (runtime_bootstrap_semantics.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_semantics_live_runtime_enforcement_landed\":"
           << (runtime_bootstrap_semantics.live_runtime_enforcement_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_semantics_no_partial_commit_on_failure\":"
           << (runtime_bootstrap_semantics.no_partial_commit_on_failure ? "true"
                                                                        : "false")
           << ",\"runtime_bootstrap_semantics_ready_for_constructor_root_implementation\":"
           << (runtime_bootstrap_semantics
                       .ready_for_constructor_root_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_semantics_replay_key\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_semantics.replay_key)
           << "\",\"runtime_bootstrap_semantics_failure_reason\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_semantics.failure_reason)
           << "\""
           << ",\"runtime_bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"runtime_bootstrap_lowering_registration_manifest_contract_id\":\""
           << runtime_bootstrap_lowering.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"runtime_bootstrap_registrar_stage_registration_table_symbol\":\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_model\":\""
           << kObjc3RuntimeBootstrapImageWalkModel
           << "\",\"runtime_bootstrap_registrar_discovery_root_validation_model\":\""
           << kObjc3RuntimeBootstrapDiscoveryRootValidationModel
           << "\",\"runtime_bootstrap_registrar_selector_pool_interning_model\":\""
           << kObjc3RuntimeBootstrapSelectorPoolInterningModel
           << "\",\"runtime_bootstrap_registrar_realization_staging_model\":\""
           << kObjc3RuntimeBootstrapRealizationStagingModel
           << "\",\"runtime_bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"runtime_bootstrap_reset_replay_registered_images_symbol\":\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
           << "\",\"runtime_bootstrap_reset_reset_replay_state_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\",\"runtime_bootstrap_reset_lifecycle_model\":\""
           << kObjc3RuntimeBootstrapResetLifecycleModel
           << "\",\"runtime_bootstrap_reset_replay_order_model\":\""
           << kObjc3RuntimeBootstrapReplayOrderModel
           << "\",\"runtime_bootstrap_reset_image_local_init_state_reset_model\":\""
           << kObjc3RuntimeBootstrapImageLocalInitStateResetModel
           << "\",\"runtime_bootstrap_reset_bootstrap_catalog_retention_model\":\""
           << kObjc3RuntimeBootstrapCatalogRetentionModel
           << "\",\"runtime_bootstrap_lowering_bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_lowering.bootstrap_semantics_contract_id
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_contract_id\":\""
           << runtime_bootstrap_lowering
                  .registration_descriptor_frontend_closure_contract_id
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_artifact\":\""
           << runtime_bootstrap_lowering.registration_descriptor_artifact
           << "\",\"runtime_bootstrap_lowering_boundary_model\":\""
           << runtime_bootstrap_lowering.lowering_boundary_model
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_handoff_model\":\""
           << runtime_bootstrap_lowering.registration_descriptor_handoff_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_symbol\":\""
           << runtime_bootstrap_lowering.constructor_root_symbol
           << "\",\"runtime_bootstrap_lowering_init_stub_symbol_prefix\":\""
           << runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_table_symbol_prefix\":\""
           << runtime_bootstrap_lowering.registration_table_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_entrypoint_symbol\":\""
           << runtime_bootstrap_lowering.registration_entrypoint_symbol
           << "\",\"runtime_bootstrap_lowering_global_ctor_list_model\":\""
           << runtime_bootstrap_lowering.global_ctor_list_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_emission_state\":\""
           << runtime_bootstrap_lowering.constructor_root_emission_state
           << "\",\"runtime_bootstrap_lowering_init_stub_emission_state\":\""
           << runtime_bootstrap_lowering.init_stub_emission_state
           << "\",\"runtime_bootstrap_lowering_registration_table_emission_state\":\""
           << runtime_bootstrap_lowering.registration_table_emission_state
           << "\",\"runtime_bootstrap_lowering_fail_closed\":"
           << (runtime_bootstrap_lowering.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_contract_ready\":"
           << (runtime_bootstrap_lowering
                       .registration_descriptor_frontend_closure_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_no_bootstrap_ir_materialization_yet\":"
           << (runtime_bootstrap_lowering.no_bootstrap_ir_materialization_yet
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_ready_for_bootstrap_materialization\":"
           << (runtime_bootstrap_lowering.ready_for_bootstrap_materialization
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_replay_key\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_lowering.replay_key)
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_lowering
                      .registration_descriptor_frontend_closure_replay_key)
           << "\",\"runtime_bootstrap_lowering_failure_reason\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_lowering.failure_reason)
           << "\"";
}

}  // namespace objc3::artifacts::frontend
