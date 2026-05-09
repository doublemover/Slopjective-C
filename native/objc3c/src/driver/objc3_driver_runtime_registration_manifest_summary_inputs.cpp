#include "driver/objc3_driver_runtime_registration_manifest_summary_inputs.h"

#include "driver/objc3_driver_runtime_registration_accessor_surface.h"
#include "driver/objc3_driver_runtime_registration_command_surface.h"

void PopulateObjc3DriverRuntimeRegistrationManifestSummaryInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts) {
  const auto &registration_manifest_summary =
      artifacts.runtime_translation_unit_registration_manifest_summary;
  const auto &runtime_bootstrap_api_summary =
      artifacts.runtime_bootstrap_api_summary;

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
      registration_manifest_summary
          .runtime_support_library_archive_relative_path;
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
  PopulateObjc3DriverRuntimeRegistrationCommandSurfaceInputs(
      inputs, registration_manifest_summary);
  PopulateObjc3DriverRuntimeRegistrationAccessorSurfaceInputs(
      inputs, runtime_bootstrap_api_summary);
}

void PopulateObjc3DriverRuntimeRegistrationDescriptorSourceInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts) {
  const auto &registration_manifest_summary =
      artifacts.runtime_translation_unit_registration_manifest_summary;
  const auto &registration_descriptor_source_surface_summary =
      artifacts.runtime_registration_descriptor_image_root_source_surface_summary;

  inputs.registration_descriptor_source_contract_id =
      registration_descriptor_source_surface_summary.contract_id;
  inputs.registration_descriptor_source_surface_path =
      registration_descriptor_source_surface_summary.source_surface_path;
  inputs.registration_descriptor_pragma_name =
      registration_descriptor_source_surface_summary
          .registration_descriptor_pragma_name;
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
  inputs.ivar_descriptor_count =
      registration_manifest_summary.ivar_descriptor_count;
  inputs.total_descriptor_count =
      registration_manifest_summary.total_descriptor_count;
}

void PopulateObjc3DriverRuntimeRegistrationOutputArtifactInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out) {
  const auto &registration_manifest_summary =
      artifacts.runtime_translation_unit_registration_manifest_summary;
  const auto &runtime_bootstrap_semantics_summary =
      artifacts.runtime_bootstrap_semantics_summary;

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
  inputs.backend_artifact_relative_path =
      backend_out.filename().generic_string();
}
