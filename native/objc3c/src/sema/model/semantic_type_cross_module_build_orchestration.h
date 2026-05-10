#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_contracts_metadata_packaging.h"
#include "sema/model/semantic_type_serialized_runtime_metadata.h"

inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationContractId =
        "objc3c.cross.module.build.runtime.orchestration.v1";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_cross_module_build_runtime_orchestration_contract";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationAuthorityModel =
        "serialized-runtime-import-surface-reuse-payload-plus-local-registration-manifest";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationInputModel =
        "filesystem-runtime-import-surface-artifact-path-list-plus-local-registration-manifest";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationRegistrationScopeModel =
        "translation-unit-registration-manifests-remain-image-local-until-cross-module-registration-aggregation-lands";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationPackagingModel =
        "no-cross-module-link-plan-artifact-or-imported-registration-manifest-ingest-during-freeze";

struct Objc3CrossModuleBuildRuntimeOrchestrationSummary {
  std::string contract_id =
      kObjc3CrossModuleBuildRuntimeOrchestrationContractId;
  std::string source_serialized_runtime_metadata_artifact_reuse_contract_id =
      kObjc3SerializedRuntimeMetadataArtifactReuseContractId;
  std::string source_local_registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_imported_runtime_metadata_semantic_rules_contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string frontend_surface_path =
      kObjc3CrossModuleBuildRuntimeOrchestrationSurfacePath;
  std::string import_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string local_registration_manifest_artifact_relative_path =
      kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath;
  std::string authority_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationAuthorityModel;
  std::string input_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationInputModel;
  std::string registration_scope_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationRegistrationScopeModel;
  std::string packaging_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationPackagingModel;
  std::vector<std::string> module_names_lexicographic;
  std::size_t module_image_count = 0;
  std::size_t direct_import_input_count = 0;
  std::size_t local_class_descriptor_count = 0;
  std::size_t local_protocol_descriptor_count = 0;
  std::size_t local_category_descriptor_count = 0;
  std::size_t local_property_descriptor_count = 0;
  std::size_t local_ivar_descriptor_count = 0;
  std::size_t local_total_descriptor_count = 0;
  std::size_t transitive_runtime_owned_declaration_count = 0;
  std::size_t transitive_metadata_reference_count = 0;
  std::size_t imported_optional_send_site_count = 0;
  std::size_t imported_typed_keypath_literal_site_count = 0;
  std::size_t imported_live_optional_lowering_site_count = 0;
  std::size_t imported_live_typed_keypath_artifact_site_count = 0;
  bool fail_closed = false;
  bool source_serialized_runtime_metadata_artifact_reuse_ready = false;
  bool source_local_registration_manifest_ready = false;
  bool source_imported_runtime_metadata_semantic_rules_ready = false;
  bool semantic_surface_published = false;
  bool local_registration_manifest_emitted = false;
  bool imported_type_system_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool cross_module_link_plan_artifact_landed = false;
  bool imported_registration_manifest_loading_landed = false;
  bool runtime_archive_aggregation_landed = false;
  bool cross_module_runtime_registration_landed = false;
  bool cross_module_launch_orchestration_landed = false;
  bool public_cross_module_orchestration_abi_landed = false;
  bool ready_for_packaging_and_runtime_registration_impl = false;
  std::string source_serialized_runtime_metadata_replay_key;
  std::string source_local_registration_manifest_replay_key;
  std::string source_imported_runtime_metadata_semantic_rules_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary
              .source_serialized_runtime_metadata_artifact_reuse_contract_id
              .empty() &&
         !summary.source_local_registration_manifest_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.import_artifact_relative_path.empty() &&
         !summary.local_registration_manifest_artifact_relative_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         !summary.registration_scope_model.empty() &&
         !summary.packaging_model.empty() &&
         summary.module_image_count == summary.module_names_lexicographic.size() &&
         summary.module_image_count > 0 &&
         summary.direct_import_input_count + 1 <= summary.module_image_count &&
         summary.local_total_descriptor_count ==
             summary.local_class_descriptor_count +
                 summary.local_protocol_descriptor_count +
                 summary.local_category_descriptor_count +
                 summary.local_property_descriptor_count +
                 summary.local_ivar_descriptor_count &&
         summary.fail_closed &&
         summary.source_serialized_runtime_metadata_artifact_reuse_ready &&
         summary.source_local_registration_manifest_ready &&
         summary.semantic_surface_published &&
         summary.local_registration_manifest_emitted &&
         !summary.cross_module_link_plan_artifact_landed &&
         !summary.imported_registration_manifest_loading_landed &&
         !summary.runtime_archive_aggregation_landed &&
         !summary.cross_module_runtime_registration_landed &&
         !summary.cross_module_launch_orchestration_landed &&
         !summary.public_cross_module_orchestration_abi_landed &&
         !summary.ready_for_packaging_and_runtime_registration_impl &&
         !summary.source_serialized_runtime_metadata_replay_key.empty() &&
         !summary.source_local_registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
