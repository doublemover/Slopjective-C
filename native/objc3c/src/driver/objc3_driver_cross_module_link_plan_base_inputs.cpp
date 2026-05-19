#include "driver/objc3_driver_cross_module_link_plan_base_inputs.h"

#include <filesystem>

#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"

namespace fs = std::filesystem;

Objc3CrossModuleRuntimeLinkPlanArtifactInputs
BuildObjc3DriverCrossModuleRuntimeLinkPlanBaseInputs(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const std::filesystem::path &object_out) {
  Objc3CrossModuleRuntimeLinkPlanArtifactInputs link_plan_inputs;
  link_plan_inputs.contract_id = kObjc3CrossModuleRuntimeLinkPlanContractId;
  link_plan_inputs.source_orchestration_contract_id =
      kObjc3CrossModuleBuildRuntimeOrchestrationContractId;
  link_plan_inputs.import_surface_contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  link_plan_inputs.registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  link_plan_inputs.payload_model = kObjc3CrossModuleRuntimeLinkPlanPayloadModel;
  link_plan_inputs.artifact_relative_path =
      kObjc3CrossModuleRuntimeLinkPlanArtifactRelativePath;
  link_plan_inputs.linker_response_artifact_relative_path =
      kObjc3CrossModuleRuntimeLinkerResponseArtifactRelativePath;
  link_plan_inputs.authority_model =
      kObjc3CrossModuleRuntimeLinkPlanAuthorityModel;
  link_plan_inputs.packaging_model =
      kObjc3CrossModuleRuntimeLinkPlanPackagingModel;
  link_plan_inputs.registration_scope_model =
      kObjc3CrossModuleRuntimeLinkPlanRegistrationScopeModel;
  link_plan_inputs.link_object_order_model =
      kObjc3CrossModuleRuntimeLinkObjectOrderModel;
  link_plan_inputs.local_module_name =
      artifacts.runtime_aware_import_module_frontend_closure_summary.module_name;
  link_plan_inputs.local_import_surface_artifact_relative_path =
      fs::absolute(BuildRuntimeAwareImportModuleArtifactPath(
                       cli_options.out_dir, cli_options.emit_prefix))
          .lexically_normal()
          .generic_string();
  link_plan_inputs.local_registration_manifest_artifact_relative_path =
      fs::absolute(BuildRuntimeRegistrationManifestArtifactPath(
                       cli_options.out_dir, cli_options.emit_prefix))
          .lexically_normal()
          .generic_string();
  link_plan_inputs.local_object_artifact_relative_path =
      fs::absolute(object_out).lexically_normal().generic_string();
  link_plan_inputs.runtime_support_library_archive_relative_path =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .runtime_support_library_archive_relative_path;
  link_plan_inputs.object_format = linker_retention_artifacts.object_format;
  link_plan_inputs.local_translation_unit_identity_model =
      linker_retention_artifacts.translation_unit_identity_model;
  link_plan_inputs.local_translation_unit_identity_key =
      linker_retention_artifacts.translation_unit_identity_key;
  link_plan_inputs.local_translation_unit_registration_order_ordinal =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .translation_unit_registration_order_ordinal;
  link_plan_inputs.local_class_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .class_descriptor_count;
  link_plan_inputs.local_protocol_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .protocol_descriptor_count;
  link_plan_inputs.local_category_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .category_descriptor_count;
  link_plan_inputs.local_property_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .property_descriptor_count;
  link_plan_inputs.local_ivar_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .ivar_descriptor_count;
  link_plan_inputs.local_total_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .total_descriptor_count;
  link_plan_inputs.local_driver_linker_flags = {
      linker_retention_artifacts.driver_linker_flag};
  return link_plan_inputs;
}
