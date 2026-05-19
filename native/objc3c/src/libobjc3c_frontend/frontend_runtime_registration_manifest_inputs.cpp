#include "libobjc3c_frontend/frontend_runtime_registration_artifact_inputs.h"

#include "ast/objc3_ast_contracts_runtime_bootstrap_support_registrar_reset.h"
#include "io/objc3_process_runtime_metadata_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3c::frontend {

Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
BuildRuntimeRegistrationManifestInputs(
    const Objc3FrontendCompileProduct &product,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out) {
  Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs inputs;
  const auto &registration_manifest_summary =
      product.artifact_bundle
          .runtime_translation_unit_registration_manifest_summary;
  const auto &registration_descriptor_source_surface_summary =
      product.artifact_bundle
          .runtime_registration_descriptor_image_root_source_surface_summary;
  const auto &runtime_bootstrap_api_summary =
      product.artifact_bundle.runtime_bootstrap_api_summary;
  const auto &runtime_bootstrap_semantics_summary =
      product.artifact_bundle.runtime_bootstrap_semantics_summary;
  const auto &runtime_bootstrap_lowering_summary =
      product.artifact_bundle.runtime_bootstrap_lowering_summary;

  inputs.contract_id = registration_manifest_summary.contract_id;
  inputs.translation_unit_registration_contract_id =
      registration_manifest_summary.translation_unit_registration_contract_id;
  inputs.runtime_support_library_link_wiring_contract_id =
      registration_manifest_summary
          .runtime_support_library_link_wiring_contract_id;
  inputs.manifest_payload_model =
      registration_manifest_summary.manifest_payload_model;
  inputs.manifest_artifact_relative_path =
      registration_manifest_summary.manifest_artifact_relative_path;
  inputs.runtime_owned_payload_artifacts.assign(
      registration_manifest_summary.runtime_owned_payload_artifacts.begin(),
      registration_manifest_summary.runtime_owned_payload_artifacts.end());
  inputs.runtime_support_library_archive_relative_path =
      registration_manifest_summary.runtime_support_library_archive_relative_path;
  inputs.constructor_root_symbol =
      registration_manifest_summary.constructor_root_symbol;
  inputs.constructor_root_ownership_model =
      registration_manifest_summary.constructor_root_ownership_model;
  inputs.manifest_authority_model =
      registration_manifest_summary.manifest_authority_model;
  inputs.constructor_init_stub_symbol_prefix =
      registration_manifest_summary.constructor_init_stub_symbol_prefix;
  inputs.constructor_init_stub_ownership_model =
      registration_manifest_summary.constructor_init_stub_ownership_model;
  inputs.constructor_priority_policy =
      registration_manifest_summary.constructor_priority_policy;
  inputs.registration_entrypoint_symbol =
      registration_manifest_summary.registration_entrypoint_symbol;
  inputs.translation_unit_identity_model =
      registration_manifest_summary.translation_unit_identity_model;
  inputs.launch_integration_contract_id =
      registration_manifest_summary.launch_integration_contract_id;
  inputs.runtime_library_resolution_model =
      registration_manifest_summary.runtime_library_resolution_model;
  inputs.driver_linker_flag_consumption_model =
      registration_manifest_summary.driver_linker_flag_consumption_model;
  inputs.compile_wrapper_command_surface =
      registration_manifest_summary.compile_wrapper_command_surface;
  inputs.compile_proof_command_surface =
      registration_manifest_summary.compile_proof_command_surface;
  inputs.execution_smoke_command_surface =
      registration_manifest_summary.execution_smoke_command_surface;
  inputs.registration_descriptor_source_contract_id =
      registration_descriptor_source_surface_summary.contract_id;
  inputs.registration_descriptor_source_surface_path =
      registration_descriptor_source_surface_summary.source_surface_path;
  inputs.registration_descriptor_pragma_name =
      registration_descriptor_source_surface_summary.registration_descriptor_pragma_name;
  inputs.image_root_pragma_name =
      registration_descriptor_source_surface_summary.image_root_pragma_name;
  inputs.module_identity_source =
      registration_descriptor_source_surface_summary.module_identity_source;
  inputs.registration_descriptor_identifier =
      registration_descriptor_source_surface_summary
          .registration_descriptor_identifier;
  inputs.registration_descriptor_identity_source =
      registration_descriptor_source_surface_summary
          .registration_descriptor_identity_source;
  inputs.image_root_identifier =
      registration_descriptor_source_surface_summary.image_root_identifier;
  inputs.image_root_identity_source =
      registration_descriptor_source_surface_summary.image_root_identity_source;
  inputs.bootstrap_visible_metadata_ownership_model =
      registration_descriptor_source_surface_summary
          .bootstrap_visible_metadata_ownership_model;
  inputs.class_descriptor_count =
      registration_manifest_summary.class_descriptor_count;
  inputs.protocol_descriptor_count =
      registration_manifest_summary.protocol_descriptor_count;
  inputs.category_descriptor_count =
      registration_manifest_summary.category_descriptor_count;
  inputs.property_descriptor_count =
      registration_manifest_summary.property_descriptor_count;
  inputs.ivar_descriptor_count = registration_manifest_summary.ivar_descriptor_count;
  inputs.total_descriptor_count =
      registration_manifest_summary.total_descriptor_count;
  inputs.bootstrap_semantics_contract_id =
      runtime_bootstrap_semantics_summary.contract_id;
  inputs.duplicate_registration_policy =
      runtime_bootstrap_semantics_summary.duplicate_registration_policy;
  inputs.realization_order_policy =
      runtime_bootstrap_semantics_summary.realization_order_policy;
  inputs.failure_mode = runtime_bootstrap_semantics_summary.failure_mode;
  inputs.registration_result_model =
      runtime_bootstrap_semantics_summary.registration_result_model;
  inputs.registration_order_ordinal_model =
      runtime_bootstrap_semantics_summary.registration_order_ordinal_model;
  inputs.runtime_state_snapshot_symbol =
      runtime_bootstrap_semantics_summary.runtime_state_snapshot_symbol;
  inputs.bootstrap_runtime_api_contract_id =
      runtime_bootstrap_api_summary.contract_id;
  inputs.bootstrap_runtime_api_public_header_path =
      runtime_bootstrap_api_summary.public_header_path;
  inputs.bootstrap_runtime_api_archive_relative_path =
      runtime_bootstrap_api_summary.archive_relative_path;
  inputs.bootstrap_runtime_api_registration_status_enum_type =
      runtime_bootstrap_api_summary.registration_status_enum_type;
  inputs.bootstrap_runtime_api_image_descriptor_type =
      runtime_bootstrap_api_summary.image_descriptor_type;
  inputs.bootstrap_runtime_api_selector_handle_type =
      runtime_bootstrap_api_summary.selector_handle_type;
  inputs.bootstrap_runtime_api_registration_snapshot_type =
      runtime_bootstrap_api_summary.registration_snapshot_type;
  inputs.bootstrap_runtime_api_registration_entrypoint_symbol =
      runtime_bootstrap_api_summary.registration_entrypoint_symbol;
  inputs.bootstrap_runtime_api_selector_lookup_symbol =
      runtime_bootstrap_api_summary.selector_lookup_symbol;
  inputs.bootstrap_runtime_api_dispatch_entrypoint_symbol =
      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
  inputs.bootstrap_runtime_api_state_snapshot_symbol =
      runtime_bootstrap_api_summary.state_snapshot_symbol;
  inputs.bootstrap_runtime_api_reset_for_testing_symbol =
      runtime_bootstrap_api_summary.reset_for_testing_symbol;
  inputs.bootstrap_registrar_contract_id =
      kObjc3RuntimeBootstrapRegistrarContractId;
  inputs.bootstrap_registrar_internal_header_path =
      kObjc3RuntimeBootstrapInternalHeaderPath;
  inputs.bootstrap_registrar_stage_registration_table_symbol =
      kObjc3RuntimeBootstrapStageRegistrationTableSymbol;
  inputs.bootstrap_registrar_image_walk_snapshot_symbol =
      kObjc3RuntimeBootstrapImageWalkSnapshotSymbol;
  inputs.bootstrap_registrar_image_walk_model =
      kObjc3RuntimeBootstrapImageWalkModel;
  inputs.bootstrap_registrar_discovery_root_validation_model =
      kObjc3RuntimeBootstrapDiscoveryRootValidationModel;
  inputs.bootstrap_registrar_selector_pool_interning_model =
      kObjc3RuntimeBootstrapSelectorPoolInterningModel;
  inputs.bootstrap_registrar_realization_staging_model =
      kObjc3RuntimeBootstrapRealizationStagingModel;
  inputs.bootstrap_reset_contract_id = kObjc3RuntimeBootstrapResetContractId;
  inputs.bootstrap_reset_internal_header_path =
      kObjc3RuntimeBootstrapInternalHeaderPath;
  inputs.bootstrap_reset_replay_registered_images_symbol =
      kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
  inputs.bootstrap_reset_reset_replay_state_snapshot_symbol =
      kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
  inputs.bootstrap_reset_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  inputs.bootstrap_reset_replay_order_model =
      kObjc3RuntimeBootstrapReplayOrderModel;
  inputs.bootstrap_reset_image_local_init_state_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  inputs.bootstrap_reset_bootstrap_catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  inputs.bootstrap_lowering_contract_id =
      runtime_bootstrap_lowering_summary.contract_id;
  inputs.bootstrap_lowering_boundary_model =
      runtime_bootstrap_lowering_summary.lowering_boundary_model;
  inputs.bootstrap_global_ctor_list_model =
      runtime_bootstrap_lowering_summary.global_ctor_list_model;
  inputs.bootstrap_registration_table_layout_model =
      runtime_bootstrap_lowering_summary.registration_table_layout_model;
  inputs.bootstrap_image_local_initialization_model =
      runtime_bootstrap_lowering_summary.image_local_initialization_model;
  inputs.bootstrap_constructor_root_emission_state =
      runtime_bootstrap_lowering_summary.constructor_root_emission_state;
  inputs.bootstrap_init_stub_emission_state =
      runtime_bootstrap_lowering_summary.init_stub_emission_state;
  inputs.bootstrap_registration_table_emission_state =
      runtime_bootstrap_lowering_summary.registration_table_emission_state;
  inputs.bootstrap_registration_table_symbol_prefix =
      runtime_bootstrap_lowering_summary.registration_table_symbol_prefix;
  inputs.bootstrap_image_local_init_state_symbol_prefix =
      runtime_bootstrap_lowering_summary.image_local_init_state_symbol_prefix;
  inputs.bootstrap_registration_table_abi_version =
      runtime_bootstrap_lowering_summary.registration_table_abi_version;
  inputs.bootstrap_registration_table_pointer_field_count =
      runtime_bootstrap_lowering_summary.registration_table_pointer_field_count;
  inputs.success_status_code =
      runtime_bootstrap_semantics_summary.success_status_code;
  inputs.invalid_descriptor_status_code =
      runtime_bootstrap_semantics_summary.invalid_descriptor_status_code;
  inputs.duplicate_registration_status_code =
      runtime_bootstrap_semantics_summary.duplicate_registration_status_code;
  inputs.out_of_order_status_code =
      runtime_bootstrap_semantics_summary.out_of_order_status_code;
  inputs.translation_unit_registration_order_ordinal =
      registration_manifest_summary.translation_unit_registration_order_ordinal;
  inputs.object_artifact_relative_path = object_out.filename().generic_string();
  inputs.backend_artifact_relative_path = backend_out.filename().generic_string();
  return inputs;
}

}  // namespace objc3c::frontend
