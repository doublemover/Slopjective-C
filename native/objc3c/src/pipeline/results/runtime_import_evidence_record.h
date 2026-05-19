#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3RuntimeAwareImportModuleSurfaceContractId =
    "objc3c.runtime.aware.import.module.surface.v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureContractId =
        "objc3c.runtime.aware.import.module.frontend.closure.v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_aware_import_module_frontend_closure";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel =
        "runtime-aware-import-module-surface-json-v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix =
        ".runtime-import-surface.json";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath =
        "module.runtime-import-surface.json";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendAuthorityModel =
        "runtime-import-surface-artifact-derived-from-frozen-import-surface-and-runtime-metadata-source-records";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel =
        "compiler-emits-runtime-import-surface-artifact-frontend-and-later-module-consumers-own-cross-translation-unit-handoff";

struct Objc3RuntimeAwareImportModuleFrontendClosureSummary {
  std::string contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  std::string source_surface_contract_id =
      kObjc3RuntimeAwareImportModuleSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath;
  std::string payload_model =
      kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel;
  std::string artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3RuntimeAwareImportModuleFrontendAuthorityModel;
  std::string payload_ownership_model =
      kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel;
  std::string module_name;
  std::size_t protocol_decl_count = 0;
  std::size_t interface_decl_count = 0;
  std::size_t implementation_decl_count = 0;
  std::size_t interface_category_decl_count = 0;
  std::size_t implementation_category_decl_count = 0;
  std::size_t function_decl_count = 0;
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_reference_count = 0;
  std::size_t protocol_reference_count = 0;
  std::size_t property_accessor_reference_count = 0;
  std::size_t property_ivar_binding_reference_count = 0;
  std::size_t method_selector_reference_count = 0;
  std::size_t metadata_reference_count = 0;
  bool fail_closed = false;
  bool source_surface_contract_ready = false;
  bool runtime_metadata_source_records_ready = false;
  bool frontend_surface_published = false;
  bool import_artifact_template_published = false;
  bool runtime_aware_import_declarations_landed = false;
  bool module_metadata_import_surface_landed = false;
  bool runtime_owned_declaration_import_landed = false;
  bool runtime_metadata_reference_import_landed = false;
  bool public_frontend_api_module_surface_landed = false;
  bool ready_for_import_artifact_emission = false;
  bool ready_for_frontend_module_consumption = false;
  std::string source_surface_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.payload_ownership_model.empty() &&
         !summary.module_name.empty() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.metadata_reference_count ==
             summary.superclass_reference_count +
                 summary.protocol_reference_count +
                 summary.property_accessor_reference_count +
                 summary.property_ivar_binding_reference_count +
                 summary.method_selector_reference_count &&
         summary.fail_closed && summary.source_surface_contract_ready &&
         summary.runtime_metadata_source_records_ready &&
         summary.frontend_surface_published &&
         summary.import_artifact_template_published &&
         summary.runtime_aware_import_declarations_landed &&
         summary.module_metadata_import_surface_landed &&
         summary.runtime_owned_declaration_import_landed &&
         summary.runtime_metadata_reference_import_landed &&
         summary.public_frontend_api_module_surface_landed &&
         summary.ready_for_import_artifact_emission &&
         summary.ready_for_frontend_module_consumption &&
         !summary.source_surface_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId =
        "objc3c.cross.module.runtime.metadata.semantic.preservation.v1";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_cross_module_runtime_metadata_semantic_preservation_contract";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationAuthorityModel =
        "semantic-preservation-freeze-derived-from-runtime-import-surface-and-runtime-metadata-source-records";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationConformanceShapeModel =
        "superclass-protocol-and-category-attachment-shape";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationDispatchTraitModel =
        "selector-classness-accessor-ivar-binding-and-body-availability";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationEffectTraitModel =
        "property-attribute-and-ownership-effect-profiles";
