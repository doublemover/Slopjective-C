#include "driver/objc3_driver_runtime_bootstrap_manifest_inputs.h"

#include "lower/objc3_lowering_contract.h"
#include "runtime/metadata/selector_metadata.h"

void PopulateObjc3DriverRuntimeBootstrapSemanticsInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts) {
  const auto &runtime_bootstrap_semantics_summary =
      artifacts.runtime_bootstrap_semantics_summary;

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
}

void PopulateObjc3DriverRuntimeBootstrapApiInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts) {
  const auto &runtime_bootstrap_api_summary =
      artifacts.runtime_bootstrap_api_summary;

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
}

void PopulateObjc3DriverRuntimeBootstrapRegistrarInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs) {
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
}

void PopulateObjc3DriverRuntimeBootstrapResetInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs) {
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
}

void PopulateObjc3DriverRuntimeBootstrapLoweringInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts) {
  const auto &runtime_bootstrap_lowering_summary =
      artifacts.runtime_bootstrap_lowering_summary;

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
      runtime_bootstrap_lowering_summary
          .image_local_init_state_symbol_prefix;
  inputs.bootstrap_registration_table_abi_version =
      runtime_bootstrap_lowering_summary.registration_table_abi_version;
  inputs.bootstrap_registration_table_pointer_field_count =
      runtime_bootstrap_lowering_summary
          .registration_table_pointer_field_count;
}
