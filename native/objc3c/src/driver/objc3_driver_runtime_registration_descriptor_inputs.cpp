#include "driver/objc3_driver_runtime_registration_descriptor_inputs.h"

#include "lower/objc3_lowering_contract.h"

Objc3RuntimeRegistrationDescriptorArtifactInputs
BuildObjc3DriverRuntimeRegistrationDescriptorInputs(
    const Objc3FrontendArtifactBundle &artifacts,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out) {
  Objc3RuntimeRegistrationDescriptorArtifactInputs inputs;
  const auto &registration_manifest_summary =
      artifacts.runtime_translation_unit_registration_manifest_summary;
  const auto &registration_descriptor_source_surface_summary =
      artifacts.runtime_registration_descriptor_image_root_source_surface_summary;
  const auto &registration_descriptor_frontend_closure_summary =
      artifacts.runtime_registration_descriptor_frontend_closure_summary;
  const auto &runtime_bootstrap_lowering_summary =
      artifacts.runtime_bootstrap_lowering_summary;

  inputs.contract_id = registration_descriptor_frontend_closure_summary.contract_id;
  inputs.registration_manifest_contract_id =
      registration_descriptor_frontend_closure_summary
          .registration_manifest_contract_id;
  inputs.source_surface_contract_id =
      registration_descriptor_frontend_closure_summary.source_surface_contract_id;
  inputs.bootstrap_lowering_contract_id =
      runtime_bootstrap_lowering_summary.contract_id;
  inputs.payload_model =
      registration_descriptor_frontend_closure_summary.payload_model;
  inputs.artifact_relative_path =
      registration_descriptor_frontend_closure_summary.artifact_relative_path;
  inputs.authority_model =
      registration_descriptor_frontend_closure_summary.authority_model;
  inputs.translation_unit_identity_model =
      registration_descriptor_frontend_closure_summary
          .translation_unit_identity_model;
  inputs.payload_ownership_model =
      registration_descriptor_frontend_closure_summary.payload_ownership_model;
  inputs.runtime_support_library_archive_relative_path =
      registration_manifest_summary.runtime_support_library_archive_relative_path;
  inputs.registration_entrypoint_symbol =
      registration_manifest_summary.registration_entrypoint_symbol;
  inputs.registration_descriptor_pragma_name =
      registration_descriptor_source_surface_summary
          .registration_descriptor_pragma_name;
  inputs.image_root_pragma_name =
      registration_descriptor_source_surface_summary.image_root_pragma_name;
  inputs.module_identity_source =
      registration_descriptor_source_surface_summary.module_identity_source;
  inputs.registration_descriptor_identifier =
      registration_descriptor_frontend_closure_summary
          .registration_descriptor_identifier;
  inputs.registration_descriptor_identity_source =
      registration_descriptor_frontend_closure_summary
          .registration_descriptor_identity_source;
  inputs.image_root_identifier =
      registration_descriptor_frontend_closure_summary.image_root_identifier;
  inputs.image_root_identity_source =
      registration_descriptor_frontend_closure_summary.image_root_identity_source;
  inputs.bootstrap_visible_metadata_ownership_model =
      registration_descriptor_frontend_closure_summary
          .bootstrap_visible_metadata_ownership_model;
  inputs.constructor_root_symbol =
      registration_manifest_summary.constructor_root_symbol;
  inputs.constructor_init_stub_symbol_prefix =
      registration_manifest_summary.constructor_init_stub_symbol_prefix;
  inputs.bootstrap_registration_table_symbol_prefix =
      runtime_bootstrap_lowering_summary.registration_table_symbol_prefix;
  inputs.bootstrap_image_local_init_state_symbol_prefix =
      runtime_bootstrap_lowering_summary.image_local_init_state_symbol_prefix;
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
  inputs.bootstrap_registration_table_abi_version =
      runtime_bootstrap_lowering_summary.registration_table_abi_version;
  inputs.bootstrap_registration_table_pointer_field_count =
      runtime_bootstrap_lowering_summary.registration_table_pointer_field_count;
  inputs.class_descriptor_count =
      registration_descriptor_frontend_closure_summary.class_descriptor_count;
  inputs.protocol_descriptor_count =
      registration_descriptor_frontend_closure_summary.protocol_descriptor_count;
  inputs.category_descriptor_count =
      registration_descriptor_frontend_closure_summary.category_descriptor_count;
  inputs.property_descriptor_count =
      registration_descriptor_frontend_closure_summary.property_descriptor_count;
  inputs.ivar_descriptor_count =
      registration_descriptor_frontend_closure_summary.ivar_descriptor_count;
  inputs.total_descriptor_count =
      registration_descriptor_frontend_closure_summary.total_descriptor_count;
  inputs.translation_unit_registration_order_ordinal =
      registration_descriptor_frontend_closure_summary
          .translation_unit_registration_order_ordinal;
  inputs.object_artifact_relative_path =
      object_out.filename().generic_string();
  inputs.backend_artifact_relative_path =
      backend_out.filename().generic_string();
  return inputs;
}
