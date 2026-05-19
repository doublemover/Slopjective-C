#include "io/objc3_runtime_registration_manifest_document_bootstrap.h"

#include <ostream>

#include "io/objc3_process_internal.h"

void EmitObjc3RuntimeRegistrationManifestBootstrapJson(
    std::ostream &out,
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record) {
  out << "  \"bootstrap_runtime_api_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_contract_id)
      << "\",\n"
      << "  \"bootstrap_runtime_api_public_header_path\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_public_header_path)
      << "\",\n"
      << "  \"bootstrap_runtime_api_archive_relative_path\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_archive_relative_path)
      << "\",\n"
      << "  \"bootstrap_runtime_api_registration_status_enum_type\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_registration_status_enum_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_image_descriptor_type\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_image_descriptor_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_selector_handle_type\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_selector_handle_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_registration_snapshot_type\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_registration_snapshot_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_registration_entrypoint_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_registration_entrypoint_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_selector_lookup_symbol\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_selector_lookup_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_dispatch_entrypoint_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_dispatch_entrypoint_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_state_snapshot_symbol\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_reset_for_testing_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_reset_for_testing_symbol)
      << "\",\n"
      << "  \"bootstrap_registrar_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_registrar_contract_id)
      << "\",\n"
      << "  \"bootstrap_registrar_internal_header_path\": \""
      << EscapeJsonString(inputs.bootstrap_registrar_internal_header_path)
      << "\",\n"
      << "  \"bootstrap_registrar_stage_registration_table_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_stage_registration_table_symbol)
      << "\",\n"
      << "  \"bootstrap_registrar_image_walk_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_image_walk_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_registrar_image_walk_model\": \""
      << EscapeJsonString(inputs.bootstrap_registrar_image_walk_model)
      << "\",\n"
      << "  \"bootstrap_registrar_discovery_root_validation_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_discovery_root_validation_model)
      << "\",\n"
      << "  \"bootstrap_registrar_selector_pool_interning_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_selector_pool_interning_model)
      << "\",\n"
      << "  \"bootstrap_registrar_realization_staging_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_realization_staging_model)
      << "\",\n"
      << "  \"bootstrap_table_consumption_contract_id\": \""
      << EscapeJsonString(kObjc3RuntimeBootstrapTableConsumptionContractId)
      << "\",\n"
      << "  \"bootstrap_table_consumption_model\": \""
      << EscapeJsonString(kObjc3RuntimeBootstrapTableConsumptionModel)
      << "\",\n"
      << "  \"bootstrap_table_deduplication_model\": \""
      << EscapeJsonString(kObjc3RuntimeBootstrapTableDeduplicationModel)
      << "\",\n"
      << "  \"bootstrap_table_image_state_publication_model\": \""
      << EscapeJsonString(
             kObjc3RuntimeBootstrapTableImageStatePublicationModel)
      << "\",\n"
      << "  \"bootstrap_table_stage_registration_table_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_stage_registration_table_symbol)
      << "\",\n"
      << "  \"bootstrap_table_image_walk_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_image_walk_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_live_registration_contract_id\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRegistrationDiscoveryReplayContractId)
      << "\",\n"
      << "  \"bootstrap_live_registration_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRegistrationModel) << "\",\n"
      << "  \"bootstrap_live_discovery_tracking_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveDiscoveryTrackingModel)
      << "\",\n"
      << "  \"bootstrap_live_replay_tracking_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveReplayTrackingModel) << "\",\n"
      << "  \"bootstrap_live_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_live_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRestartHardeningContractId)
      << "\",\n"
      << "  \"bootstrap_live_idempotence_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveIdempotenceModel) << "\",\n"
      << "  \"bootstrap_live_teardown_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveTeardownModel) << "\",\n"
      << "  \"bootstrap_live_restart_evidence_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRestartEvidenceModel)
      << "\",\n"
      << "  \"bootstrap_live_restart_reset_for_testing_symbol\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_reset_for_testing_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_reset_contract_id) << "\",\n"
      << "  \"bootstrap_reset_internal_header_path\": \""
      << EscapeJsonString(inputs.bootstrap_reset_internal_header_path)
      << "\",\n"
      << "  \"bootstrap_reset_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_lifecycle_model\": \""
      << EscapeJsonString(inputs.bootstrap_reset_lifecycle_model) << "\",\n"
      << "  \"bootstrap_reset_replay_order_model\": \""
      << EscapeJsonString(inputs.bootstrap_reset_replay_order_model)
      << "\",\n"
      << "  \"bootstrap_reset_image_local_init_state_reset_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_image_local_init_state_reset_model)
      << "\",\n"
      << "  \"bootstrap_reset_bootstrap_catalog_retention_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_bootstrap_catalog_retention_model)
      << "\",\n"
      << "  \"bootstrap_lowering_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_lowering_contract_id) << "\",\n"
      << "  \"bootstrap_lowering_boundary_model\": \""
      << EscapeJsonString(inputs.bootstrap_lowering_boundary_model)
      << "\",\n"
      << "  \"bootstrap_global_ctor_list_model\": \""
      << EscapeJsonString(inputs.bootstrap_global_ctor_list_model)
      << "\",\n"
      << "  \"bootstrap_registration_table_layout_model\": \""
      << EscapeJsonString(inputs.bootstrap_registration_table_layout_model)
      << "\",\n"
      << "  \"bootstrap_image_local_initialization_model\": \""
      << EscapeJsonString(inputs.bootstrap_image_local_initialization_model)
      << "\",\n"
      << "  \"bootstrap_constructor_root_emission_state\": \""
      << EscapeJsonString(inputs.bootstrap_constructor_root_emission_state)
      << "\",\n"
      << "  \"bootstrap_init_stub_emission_state\": \""
      << EscapeJsonString(inputs.bootstrap_init_stub_emission_state)
      << "\",\n"
      << "  \"bootstrap_registration_table_emission_state\": \""
      << EscapeJsonString(
             inputs.bootstrap_registration_table_emission_state)
      << "\",\n"
      << "  \"bootstrap_registration_table_symbol_prefix\": \""
      << EscapeJsonString(inputs.bootstrap_registration_table_symbol_prefix)
      << "\",\n"
      << "  \"bootstrap_image_local_init_state_symbol_prefix\": \""
      << EscapeJsonString(
             inputs.bootstrap_image_local_init_state_symbol_prefix)
      << "\",\n"
      << "  \"bootstrap_registration_table_symbol\": \""
      << EscapeJsonString(
             symbol_owner_record.bootstrap_registration_table_symbol)
      << "\",\n"
      << "  \"bootstrap_image_local_init_state_symbol\": \""
      << EscapeJsonString(
             symbol_owner_record.bootstrap_image_local_init_state_symbol)
      << "\",\n"
      << "  \"bootstrap_registration_table_abi_version\": "
      << inputs.bootstrap_registration_table_abi_version << ",\n"
      << "  \"bootstrap_registration_table_pointer_field_count\": "
      << inputs.bootstrap_registration_table_pointer_field_count << ",\n"
      << "  \"success_status_code\": " << inputs.success_status_code << ",\n"
      << "  \"invalid_descriptor_status_code\": "
      << inputs.invalid_descriptor_status_code << ",\n"
      << "  \"duplicate_registration_status_code\": "
      << inputs.duplicate_registration_status_code << ",\n"
      << "  \"out_of_order_status_code\": "
      << inputs.out_of_order_status_code << ",\n"
      << "  \"translation_unit_registration_order_ordinal\": "
      << inputs.translation_unit_registration_order_ordinal << ",\n"
      << "  \"translation_unit_identity_key\": \""
      << EscapeJsonString(symbol_owner_record.translation_unit_identity_key)
      << "\",\n"
      << "  \"object_format\": \""
      << EscapeJsonString(linker_retention_artifacts.object_format)
      << "\",\n"
      << "  \"linker_anchor_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.linker_anchor_symbol)
      << "\",\n"
      << "  \"discovery_root_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.discovery_root_symbol)
      << "\",\n"
      << "  \"driver_linker_flags\": [\n"
      << "    \""
      << EscapeJsonString(linker_retention_artifacts.driver_linker_flag)
      << "\"\n"
      << "  ],\n"
      << "  \"launch_integration_ready\": true,\n"
      << "  \"ready_for_lowering_init_stub_emission\": true,\n"
      << "  \"ready_for_bootstrap_lowering_materialization\": true,\n"
      << "  \"ready_for_runtime_bootstrap_enforcement\": true,\n"
      << "  \"ready_for_runtime_bootstrap_table_consumption\": true,\n"
      << "  \"ready_for_live_registration_discovery_replay\": true,\n"
      << "  \"ready_for_live_restart_hardening\": true\n"
      << "}\n";
}
