#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/model/semantic_type_cross_module_runtime_preservation.h"

inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesContractId =
    "objc3c.imported.runtime.metadata.semantic.rules.v1";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_imported_runtime_metadata_semantic_rules";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesAuthorityModel =
    "import-surface-artifact-consumption-derived-semantic-rules";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3ImportedRuntimeMetadataSemanticRulesSummary {
  std::string contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string source_semantic_preservation_contract_id =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId;
  std::string frontend_surface_path =
      kObjc3ImportedRuntimeMetadataSemanticRulesSurfacePath;
  std::string authority_model =
      kObjc3ImportedRuntimeMetadataSemanticRulesAuthorityModel;
  std::string input_model = kObjc3ImportedRuntimeMetadataSemanticRulesInputModel;
  std::vector<std::string> imported_module_names_lexicographic;
  std::size_t imported_input_path_count = 0;
  std::size_t imported_module_count = 0;
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
  std::size_t imported_type_system_generic_contract_module_count = 0;
  std::size_t imported_generic_interface_count = 0;
  std::size_t imported_generic_parameter_count = 0;
  std::size_t imported_generic_variance_annotation_count = 0;
  std::size_t imported_generic_argument_reference_count = 0;
  std::size_t imported_protocol_qualified_generic_argument_count = 0;
  std::size_t imported_type_system_nullability_contract_module_count = 0;
  std::size_t imported_nullability_canonical_type_count = 0;
  std::size_t imported_nullability_object_type_count = 0;
  std::size_t imported_nullable_entry_count = 0;
  std::size_t imported_nonnull_entry_count = 0;
  std::size_t imported_implicitly_unwrapped_entry_count = 0;
  std::size_t imported_null_resettable_entry_count = 0;
  std::size_t imported_unspecified_nullability_entry_count = 0;
  std::size_t imported_invalid_nullability_entry_count = 0;
  std::size_t imported_type_system_protocol_contract_module_count = 0;
  std::size_t imported_protocol_decl_count = 0;
  std::size_t imported_protocol_forward_declaration_count = 0;
  std::size_t imported_protocol_inheritance_edge_count = 0;
  std::size_t imported_protocol_required_method_count = 0;
  std::size_t imported_protocol_optional_method_count = 0;
  std::size_t imported_protocol_required_property_count = 0;
  std::size_t imported_protocol_optional_property_count = 0;
  std::size_t imported_class_protocol_adoption_count = 0;
  std::size_t imported_category_protocol_adoption_count = 0;
  bool fail_closed = false;
  bool source_semantic_preservation_contract_ready = false;
  bool semantic_surface_published = false;
  bool imported_runtime_surface_inputs_present = false;
  bool imported_runtime_surface_inputs_loaded = false;
  bool imported_conformance_shape_landed = false;
  bool imported_dispatch_traits_landed = false;
  bool imported_effect_traits_landed = false;
  bool imported_runtime_metadata_semantics_landed = false;
  bool imported_type_system_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool ready_for_imported_metadata_semantic_rules = false;
  bool ready_for_cross_module_dispatch_equivalence = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_semantic_preservation_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.imported_module_count ==
             summary.imported_module_names_lexicographic.size() &&
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
         summary.imported_input_path_count >= summary.imported_module_count &&
         summary.fail_closed &&
         summary.source_semantic_preservation_contract_ready &&
         summary.semantic_surface_published &&
         summary.imported_runtime_surface_inputs_loaded &&
         summary.imported_conformance_shape_landed &&
         summary.imported_dispatch_traits_landed &&
         summary.imported_effect_traits_landed &&
         summary.imported_runtime_metadata_semantics_landed &&
         summary.imported_type_system_type_surface_landed &&
         summary.ready_for_imported_metadata_semantic_rules &&
         summary.ready_for_cross_module_dispatch_equivalence &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
