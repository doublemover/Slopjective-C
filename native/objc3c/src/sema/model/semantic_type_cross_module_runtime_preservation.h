#pragma once

#include <cstddef>
#include <string>

#include "pipeline/results/runtime_import_evidence_record.h"

struct Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary {
  std::string contract_id =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId;
  std::string source_frontend_closure_contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  std::string frontend_surface_path =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationSurfacePath;
  std::string source_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationAuthorityModel;
  std::string conformance_shape_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationConformanceShapeModel;
  std::string dispatch_trait_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationDispatchTraitModel;
  std::string effect_trait_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationEffectTraitModel;
  std::string module_name;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_edge_count = 0;
  std::size_t protocol_conformance_edge_count = 0;
  std::size_t category_attachment_count = 0;
  std::size_t property_accessor_trait_count = 0;
  std::size_t property_ivar_binding_trait_count = 0;
  std::size_t method_selector_trait_count = 0;
  std::size_t class_method_trait_count = 0;
  std::size_t instance_method_trait_count = 0;
  std::size_t implemented_method_count = 0;
  std::size_t declaration_only_method_count = 0;
  std::size_t property_attribute_profile_count = 0;
  std::size_t ownership_effect_profile_count = 0;
  std::size_t executable_binding_trait_count = 0;
  std::size_t optional_send_site_count = 0;
  std::size_t typed_keypath_literal_site_count = 0;
  std::size_t live_optional_lowering_site_count = 0;
  std::size_t live_typed_keypath_artifact_site_count = 0;
  std::size_t imported_type_system_optional_keypath_module_count = 0;
  std::size_t imported_optional_runtime_ready_module_count = 0;
  std::size_t imported_typed_keypath_runtime_ready_module_count = 0;
  bool fail_closed = false;
  bool source_frontend_closure_ready = false;
  bool runtime_metadata_source_records_ready = false;
  bool semantic_surface_published = false;
  bool imported_conformance_shape_landed = false;
  bool imported_dispatch_traits_landed = false;
  bool imported_effect_traits_landed = false;
  bool imported_runtime_metadata_semantics_landed = false;
  bool imported_type_system_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool ready_for_imported_metadata_semantic_rules = false;
  bool ready_for_cross_module_dispatch_equivalence = false;
  std::string source_frontend_closure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_frontend_closure_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.source_artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.conformance_shape_model.empty() &&
         !summary.dispatch_trait_model.empty() &&
         !summary.effect_trait_model.empty() && !summary.module_name.empty() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.method_record_count ==
             summary.class_method_trait_count +
                 summary.instance_method_trait_count &&
         summary.method_record_count ==
             summary.implemented_method_count +
                 summary.declaration_only_method_count &&
         summary.fail_closed && summary.source_frontend_closure_ready &&
         summary.runtime_metadata_source_records_ready &&
         summary.semantic_surface_published &&
         !summary.imported_conformance_shape_landed &&
         !summary.imported_dispatch_traits_landed &&
         !summary.imported_effect_traits_landed &&
         !summary.imported_runtime_metadata_semantics_landed &&
         !summary.ready_for_imported_metadata_semantic_rules &&
         !summary.ready_for_cross_module_dispatch_equivalence &&
         !summary.source_frontend_closure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
