#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaCoreSemanticSummaryReadinessRecord
BuildObjc3SemaCoreSemanticSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaCoreSemanticSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.interface_implementation_symbols_ready =
      surface.interface_implementation_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.interface_implementation_summary.implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.interface_implementation_summary.linked_implementation_symbols ==
          surface.linked_implementation_symbols_total;
  record.interface_implementation_handoff_ready =
      surface.interface_implementation_summary.deterministic &&
      surface.deterministic_interface_implementation_handoff;
  record.protocol_category_composition_symbols_ready =
      surface.protocol_category_composition_summary.protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.protocol_category_composition_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.protocol_category_composition_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites <=
          surface.protocol_category_composition_summary
              .total_composition_sites();
  record.protocol_category_composition_handoff_ready =
      surface.protocol_category_composition_summary.deterministic &&
      surface.deterministic_protocol_category_composition_handoff;
  record.class_protocol_category_linking_symbols_ready =
      surface.class_protocol_category_linking_summary.declared_interfaces ==
          surface.interface_implementation_summary.declared_interfaces &&
      surface.class_protocol_category_linking_summary.resolved_interfaces ==
          surface.interface_implementation_summary.resolved_interfaces &&
      surface.class_protocol_category_linking_summary.declared_implementations ==
          surface.interface_implementation_summary.declared_implementations &&
      surface.class_protocol_category_linking_summary.resolved_implementations ==
          surface.interface_implementation_summary.resolved_implementations &&
      surface.class_protocol_category_linking_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .linked_implementation_symbols ==
          surface.linked_implementation_symbols_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.class_protocol_category_linking_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites <=
          surface.class_protocol_category_linking_summary
              .total_composition_sites();
  record.class_protocol_category_linking_handoff_ready =
      surface.class_protocol_category_linking_summary.deterministic &&
      surface.deterministic_class_protocol_category_linking_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.core_semantic_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.interface_implementation_symbols_ready &&
      record.interface_implementation_handoff_ready &&
      record.protocol_category_composition_symbols_ready &&
      record.protocol_category_composition_handoff_ready &&
      record.class_protocol_category_linking_symbols_ready &&
      record.class_protocol_category_linking_handoff_ready;
  return record;
}

Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord
BuildObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.selector_normalization_ready =
      surface.deterministic_selector_normalization_handoff &&
      surface.selector_normalization_summary.methods_total ==
          surface.selector_normalization_methods_total &&
      surface.selector_normalization_summary.normalized_methods ==
          surface.selector_normalization_normalized_methods_total &&
      surface.selector_normalization_summary.selector_piece_entries ==
          surface.selector_normalization_piece_entries_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries ==
          surface.selector_normalization_parameter_piece_entries_total &&
      surface.selector_normalization_summary.selector_pieceless_methods ==
          surface.selector_normalization_pieceless_methods_total &&
      surface.selector_normalization_summary.selector_spelling_mismatches ==
          surface.selector_normalization_spelling_mismatches_total &&
      surface.selector_normalization_summary.selector_arity_mismatches ==
          surface.selector_normalization_arity_mismatches_total &&
      surface.selector_normalization_summary
              .selector_parameter_linkage_mismatches ==
          surface.selector_normalization_parameter_linkage_mismatches_total &&
      surface.selector_normalization_summary
              .selector_normalization_flag_mismatches ==
          surface.selector_normalization_flag_mismatches_total &&
      surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          surface.selector_normalization_missing_keyword_pieces_total &&
      surface.selector_normalization_summary.selector_parameter_piece_entries <=
          surface.selector_normalization_summary.selector_piece_entries &&
      surface.selector_normalization_summary.normalized_methods <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary.contract_violations() <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary.deterministic;
  record.property_attribute_ready =
      surface.deterministic_property_attribute_handoff &&
      surface.property_attribute_summary.properties_total ==
          surface.property_attribute_properties_total &&
      surface.property_attribute_summary.attribute_entries ==
          surface.property_attribute_entries_total &&
      surface.property_attribute_summary.readonly_modifiers ==
          surface.property_attribute_readonly_modifiers_total &&
      surface.property_attribute_summary.readwrite_modifiers ==
          surface.property_attribute_readwrite_modifiers_total &&
      surface.property_attribute_summary.atomic_modifiers ==
          surface.property_attribute_atomic_modifiers_total &&
      surface.property_attribute_summary.nonatomic_modifiers ==
          surface.property_attribute_nonatomic_modifiers_total &&
      surface.property_attribute_summary.copy_modifiers ==
          surface.property_attribute_copy_modifiers_total &&
      surface.property_attribute_summary.strong_modifiers ==
          surface.property_attribute_strong_modifiers_total &&
      surface.property_attribute_summary.weak_modifiers ==
          surface.property_attribute_weak_modifiers_total &&
      surface.property_attribute_summary.assign_modifiers ==
          surface.property_attribute_assign_modifiers_total &&
      surface.property_attribute_summary.getter_modifiers ==
          surface.property_attribute_getter_modifiers_total &&
      surface.property_attribute_summary.setter_modifiers ==
          surface.property_attribute_setter_modifiers_total &&
      surface.property_attribute_summary.invalid_attribute_entries ==
          surface.property_attribute_invalid_attribute_entries_total &&
      surface.property_attribute_summary.property_contract_violations ==
          surface.property_attribute_contract_violations_total &&
      surface.property_attribute_summary.getter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.setter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.deterministic;
  record.type_annotation_surface_ready =
      surface.deterministic_type_annotation_surface_handoff &&
      surface.type_annotation_surface_summary.generic_suffix_sites ==
          surface.type_annotation_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary.pointer_declarator_sites ==
          surface.type_annotation_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary.nullability_suffix_sites ==
          surface.type_annotation_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary.ownership_qualifier_sites ==
          surface.type_annotation_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.object_pointer_type_sites ==
          surface.type_annotation_object_pointer_type_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          surface.type_annotation_invalid_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
          surface.type_annotation_invalid_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
          surface.type_annotation_invalid_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites ==
          surface.type_annotation_invalid_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          surface.type_annotation_surface_summary.generic_suffix_sites &&
      surface.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          surface.type_annotation_surface_summary.pointer_declarator_sites &&
      surface.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          surface.type_annotation_surface_summary.nullability_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites <=
          surface.type_annotation_surface_summary.ownership_qualifier_sites &&
      surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      surface.type_annotation_surface_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.selector_property_type_annotation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.selector_normalization_ready &&
      record.property_attribute_ready &&
      record.type_annotation_surface_ready;
  return record;
}

Objc3SemaTypeBoundarySummaryReadinessRecord
BuildObjc3SemaTypeBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypeBoundarySummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.lightweight_generic_constraint_ready =
      surface.deterministic_lightweight_generic_constraint_handoff &&
      surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
          surface.lightweight_generic_constraint_sites_total &&
      surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
          surface.lightweight_generic_constraint_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
          surface
              .lightweight_generic_constraint_object_pointer_type_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites ==
          surface
              .lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
          surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
          surface.lightweight_generic_constraint_normalized_sites_total &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites ==
          surface
              .lightweight_generic_constraint_contract_violation_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites <=
          surface.lightweight_generic_constraint_summary.generic_suffix_sites &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites <=
          surface.lightweight_generic_constraint_summary.generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.deterministic;
  record.nullability_flow_warning_precision_ready =
      surface.deterministic_nullability_flow_warning_precision_handoff &&
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
          surface.nullability_flow_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .object_pointer_type_sites ==
          surface.nullability_flow_object_pointer_type_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_nullability_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nullable_suffix_sites ==
          surface.nullability_flow_nullable_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
          surface.nullability_flow_nonnull_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites ==
          surface.nullability_flow_normalized_sites_total &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites ==
          surface.nullability_flow_contract_violation_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
          surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites +
              surface.nullability_flow_warning_precision_summary
                  .nonnull_suffix_sites &&
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.protocol_qualified_object_type_ready =
      surface.deterministic_protocol_qualified_object_type_handoff &&
      surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites ==
          surface.protocol_qualified_object_type_sites_total &&
      surface.protocol_qualified_object_type_summary.protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
          surface
              .protocol_qualified_object_type_object_pointer_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
          surface
              .protocol_qualified_object_type_pointer_declarator_sites_total &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites ==
          surface
              .protocol_qualified_object_type_contract_violation_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_composition_sites &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.type_boundary_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.lightweight_generic_constraint_ready &&
      record.nullability_flow_warning_precision_ready &&
      record.protocol_qualified_object_type_ready;
  return record;
}

Objc3SemaModuleTypeAbiSummaryReadinessRecord
BuildObjc3SemaModuleTypeAbiSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaModuleTypeAbiSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.variance_bridge_cast_ready =
      surface.deterministic_variance_bridge_cast_handoff &&
      surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          surface.variance_bridge_cast_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites ==
          surface.variance_bridge_cast_protocol_composition_sites_total &&
      surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          surface.variance_bridge_cast_object_pointer_type_sites_total &&
      surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          surface.variance_bridge_cast_pointer_declarator_sites_total &&
      surface.variance_bridge_cast_summary.normalized_sites ==
          surface.variance_bridge_cast_normalized_sites_total &&
      surface.variance_bridge_cast_summary.contract_violation_sites ==
          surface.variance_bridge_cast_contract_violation_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.normalized_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.contract_violation_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.deterministic;
  record.generic_metadata_abi_ready =
      surface.deterministic_generic_metadata_abi_handoff &&
      surface.generic_metadata_abi_summary.generic_metadata_abi_sites ==
          surface.generic_metadata_abi_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites ==
          surface.generic_metadata_abi_generic_suffix_sites_total &&
      surface.generic_metadata_abi_summary.protocol_composition_sites ==
          surface.generic_metadata_abi_protocol_composition_sites_total &&
      surface.generic_metadata_abi_summary.ownership_qualifier_sites ==
          surface.generic_metadata_abi_ownership_qualifier_sites_total &&
      surface.generic_metadata_abi_summary.object_pointer_type_sites ==
          surface.generic_metadata_abi_object_pointer_type_sites_total &&
      surface.generic_metadata_abi_summary.pointer_declarator_sites ==
          surface.generic_metadata_abi_pointer_declarator_sites_total &&
      surface.generic_metadata_abi_summary.normalized_sites ==
          surface.generic_metadata_abi_normalized_sites_total &&
      surface.generic_metadata_abi_summary.contract_violation_sites ==
          surface.generic_metadata_abi_contract_violation_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.protocol_composition_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.normalized_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.contract_violation_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.deterministic;
  record.module_import_graph_ready =
      surface.deterministic_module_import_graph_handoff &&
      surface.module_import_graph_summary.module_import_graph_sites ==
          surface.module_import_graph_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites ==
          surface.module_import_graph_import_edge_candidate_sites_total &&
      surface.module_import_graph_summary.namespace_segment_sites ==
          surface.module_import_graph_namespace_segment_sites_total &&
      surface.module_import_graph_summary.object_pointer_type_sites ==
          surface.module_import_graph_object_pointer_type_sites_total &&
      surface.module_import_graph_summary.pointer_declarator_sites ==
          surface.module_import_graph_pointer_declarator_sites_total &&
      surface.module_import_graph_summary.normalized_sites ==
          surface.module_import_graph_normalized_sites_total &&
      surface.module_import_graph_summary.contract_violation_sites ==
          surface.module_import_graph_contract_violation_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.namespace_segment_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.normalized_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.contract_violation_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_type_abi_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.variance_bridge_cast_ready &&
      record.generic_metadata_abi_ready && record.module_import_graph_ready;
  return record;
}

Objc3SemaModuleBoundarySummaryReadinessRecord
BuildObjc3SemaModuleBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaModuleBoundarySummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.namespace_collision_shadowing_ready =
      surface.deterministic_namespace_collision_shadowing_handoff &&
      surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites ==
          surface.namespace_collision_shadowing_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites ==
          surface
              .namespace_collision_shadowing_namespace_segment_sites_total &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites ==
          surface
              .namespace_collision_shadowing_import_edge_candidate_sites_total &&
      surface.namespace_collision_shadowing_summary.object_pointer_type_sites ==
          surface
              .namespace_collision_shadowing_object_pointer_type_sites_total &&
      surface.namespace_collision_shadowing_summary.pointer_declarator_sites ==
          surface
              .namespace_collision_shadowing_pointer_declarator_sites_total &&
      surface.namespace_collision_shadowing_summary.normalized_sites ==
          surface.namespace_collision_shadowing_normalized_sites_total &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites ==
          surface
              .namespace_collision_shadowing_contract_violation_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.normalized_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.deterministic;
  record.public_private_api_partition_ready =
      surface.deterministic_public_private_api_partition_handoff &&
      surface.public_private_api_partition_summary
              .public_private_api_partition_sites ==
          surface.public_private_api_partition_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites ==
          surface.public_private_api_partition_namespace_segment_sites_total &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites ==
          surface.public_private_api_partition_import_edge_candidate_sites_total &&
      surface.public_private_api_partition_summary.object_pointer_type_sites ==
          surface.public_private_api_partition_object_pointer_type_sites_total &&
      surface.public_private_api_partition_summary.pointer_declarator_sites ==
          surface.public_private_api_partition_pointer_declarator_sites_total &&
      surface.public_private_api_partition_summary.normalized_sites ==
          surface.public_private_api_partition_normalized_sites_total &&
      surface.public_private_api_partition_summary.contract_violation_sites ==
          surface.public_private_api_partition_contract_violation_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.normalized_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.contract_violation_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.deterministic;
  record.incremental_module_cache_invalidation_ready =
      surface.deterministic_incremental_module_cache_invalidation_handoff &&
      surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites ==
          surface.incremental_module_cache_invalidation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites ==
          surface
              .incremental_module_cache_invalidation_namespace_segment_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_import_edge_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .object_pointer_type_sites ==
          surface
              .incremental_module_cache_invalidation_object_pointer_type_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .pointer_declarator_sites ==
          surface
              .incremental_module_cache_invalidation_pointer_declarator_sites_total &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites ==
          surface.incremental_module_cache_invalidation_normalized_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites ==
          surface
              .incremental_module_cache_invalidation_contract_violation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_boundary_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.namespace_collision_shadowing_ready &&
      record.public_private_api_partition_ready &&
      record.incremental_module_cache_invalidation_ready;
  return record;
}

Objc3SemaTypedSemanticHandoffRecord BuildObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypedSemanticHandoffRecord record;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.type_metadata_identity_handoffs_ready =
      surface.deterministic_interface_implementation_handoff &&
      surface.deterministic_protocol_category_composition_handoff &&
      surface.deterministic_class_protocol_category_linking_handoff &&
      surface.deterministic_selector_normalization_handoff;
  record.type_annotation_handoffs_ready =
      surface.deterministic_property_attribute_handoff &&
      surface.deterministic_type_annotation_surface_handoff &&
      surface.deterministic_lightweight_generic_constraint_handoff &&
      surface.deterministic_nullability_flow_warning_precision_handoff &&
      surface.deterministic_protocol_qualified_object_type_handoff &&
      surface.deterministic_variance_bridge_cast_handoff &&
      surface.deterministic_generic_metadata_abi_handoff;
  record.module_boundary_handoffs_ready =
      surface.deterministic_module_import_graph_handoff &&
      surface.deterministic_namespace_collision_shadowing_handoff &&
      surface.deterministic_public_private_api_partition_handoff &&
      surface.deterministic_incremental_module_cache_invalidation_handoff &&
      surface.deterministic_cross_module_conformance_handoff;
  record.concurrency_recovery_handoffs_ready =
      surface.deterministic_throws_propagation_handoff &&
      surface.deterministic_unwind_cleanup_handoff &&
      surface.deterministic_async_continuation_handoff &&
      surface.deterministic_actor_isolation_sendability_handoff &&
      surface.deterministic_task_runtime_cancellation_handoff &&
      surface.deterministic_concurrency_replay_race_guard_handoff &&
      surface.deterministic_unsafe_pointer_extension_handoff &&
      surface.deterministic_inline_asm_intrinsic_governance_handoff &&
      surface.deterministic_ns_error_bridging_handoff &&
      surface.deterministic_error_diagnostics_recovery_handoff &&
      surface.deterministic_result_like_lowering_handoff &&
      surface.deterministic_await_lowering_suspension_state_lowering_handoff;
  record.symbol_dispatch_handoffs_ready =
      surface.deterministic_symbol_graph_scope_resolution_handoff &&
      surface.deterministic_method_lookup_override_conflict_handoff &&
      surface.deterministic_property_synthesis_ivar_binding_handoff &&
      surface.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      surface.deterministic_message_send_selector_lowering_handoff &&
      surface.deterministic_dispatch_abi_marshalling_handoff &&
      surface.deterministic_nil_receiver_semantics_foldability_handoff &&
      surface.deterministic_super_dispatch_method_family_handoff;
  record.block_dispatch_handoffs_ready =
      surface.deterministic_block_literal_capture_semantics_handoff &&
      surface.deterministic_block_abi_invoke_trampoline_handoff &&
      surface.deterministic_block_storage_escape_handoff &&
      surface.deterministic_block_copy_dispose_handoff &&
      surface.deterministic_block_determinism_perf_baseline_handoff;
  record.ownership_runtime_handoffs_ready =
      surface.deterministic_runtime_link_host_link_handoff &&
      surface.deterministic_retain_release_operation_handoff &&
      surface.deterministic_weak_unowned_semantics_handoff &&
      surface.deterministic_arc_diagnostics_fixit_handoff &&
      surface.deterministic_autoreleasepool_scope_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.typed_semantic_handoff_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.type_metadata_identity_handoffs_ready &&
      record.type_annotation_handoffs_ready &&
      record.module_boundary_handoffs_ready &&
      record.concurrency_recovery_handoffs_ready &&
      record.symbol_dispatch_handoffs_ready &&
      record.block_dispatch_handoffs_ready &&
      record.ownership_runtime_handoffs_ready;
  return record;
}

Objc3SemaAtomicVectorMappingPublicationRecord
BuildObjc3SemaAtomicVectorMappingPublicationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3AtomicMemoryOrderMappingSummary &atomic_memory_order_mapping,
    bool deterministic_atomic_memory_order_mapping,
    const Objc3VectorTypeLoweringSummary &vector_type_lowering,
    bool deterministic_vector_type_lowering) {
  Objc3SemaAtomicVectorMappingPublicationRecord record;
  record.integration_surface_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.atomic_memory_order_mapping = atomic_memory_order_mapping;
  record.deterministic_atomic_memory_order_mapping =
      deterministic_atomic_memory_order_mapping;
  record.vector_type_lowering = vector_type_lowering;
  record.deterministic_vector_type_lowering =
      deterministic_vector_type_lowering;
  record.atomic_memory_order_mapping_ready =
      record.deterministic_atomic_memory_order_mapping &&
      record.atomic_memory_order_mapping.deterministic;
  record.vector_type_lowering_ready =
      record.deterministic_vector_type_lowering &&
      record.vector_type_lowering.deterministic;
  record.mapping_summaries_ready =
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.atomic_vector_mapping_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready && record.mapping_summaries_ready;
  return record;
}

Objc3SemaTypeMetadataMappingReadinessRecord
BuildObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypeMetadataMappingReadinessRecord record;
  record.integration_surface_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.globals_total = surface.globals_total;
  record.functions_total = surface.functions_total;
  record.interfaces_total = surface.interfaces_total;
  record.implementations_total = surface.implementations_total;
  record.type_metadata_global_entries = surface.type_metadata_global_entries;
  record.type_metadata_function_entries =
      surface.type_metadata_function_entries;
  record.type_metadata_interface_entries =
      surface.type_metadata_interface_entries;
  record.type_metadata_implementation_entries =
      surface.type_metadata_implementation_entries;
  record.type_metadata_publication_ready =
      surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          surface.type_metadata_publication_record);
  record.type_metadata_handoff_ready =
      surface.deterministic_type_metadata_handoff;
  record.cardinality_consistent =
      record.globals_total == record.type_metadata_global_entries &&
      record.functions_total == record.type_metadata_function_entries &&
      record.interfaces_total == record.type_metadata_interface_entries &&
      record.implementations_total ==
          record.type_metadata_implementation_entries;
  const Objc3SemaAtomicVectorMappingPublicationRecord &mapping_publication =
      surface.atomic_vector_mapping_publication_record;
  record.atomic_vector_mapping_publication_owner =
      mapping_publication.atomic_vector_mapping_publication_owner;
  record.atomic_memory_order_mapping_ready =
      surface.deterministic_atomic_vector_mapping_publication_record &&
      mapping_publication.atomic_memory_order_mapping_ready;
  record.vector_type_lowering_ready =
      surface.deterministic_atomic_vector_mapping_publication_record &&
      mapping_publication.vector_type_lowering_ready;
  record.mapping_summaries_ready =
      surface.deterministic_atomic_vector_mapping_publication_record &&
      mapping_publication.mapping_summaries_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.type_metadata_mapping_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.atomic_vector_mapping_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.type_metadata_publication_ready &&
      record.type_metadata_handoff_ready && record.cardinality_consistent &&
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready && record.mapping_summaries_ready;
  return record;
}

bool Objc3ParserSemaSyncCountsReady(std::size_t expected_count,
                                    std::size_t required_count,
                                    std::size_t passed_count,
                                    std::size_t failed_count) {
  return required_count == expected_count && passed_count == required_count &&
         failed_count == 0u;
}

std::size_t Objc3SemaEvidenceCount(bool ready) {
  return ready ? 1u : 0u;
}

Objc3ParserSemaParityPublicationReadinessRecord
BuildObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffPublicationTransferRecord &transfer_record,
    bool deterministic_transfer_record) {
  Objc3ParserSemaParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.handoff_publication_transfer_ready =
      deterministic_transfer_record &&
      IsReadyObjc3ParserSemaHandoffPublicationTransferRecord(transfer_record);
  record.conformance_matrix_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.conformance_matrix_ready;
  record.conformance_corpus_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.conformance_corpus_ready;
  record.performance_quality_guardrails_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.performance_quality_guardrails_ready;
  record.cross_lane_integration_sync_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.cross_lane_integration_sync_ready;
  record.docs_runbook_sync_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.docs_runbook_sync_ready;
  record.release_candidate_replay_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.release_candidate_replay_ready;
  record.advanced_core_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_core_shard1_ready;
  record.advanced_contract_rejection_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_contract_rejection_shard1_ready;
  record.advanced_diagnostics_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_diagnostics_shard1_ready;
  record.advanced_conformance_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_conformance_shard1_ready;
  record.advanced_integration_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_integration_shard1_ready;
  record.advanced_performance_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_performance_shard1_ready;
  record.advanced_core_shard2_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_core_shard2_ready;
  record.advanced_contract_rejection_shard2_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_contract_rejection_shard2_ready;
  record.advanced_diagnostics_shard2_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_diagnostics_shard2_ready;
  record.integration_closeout_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.integration_closeout_ready;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.handoff_publication_transfer_ready) +
      Objc3SemaEvidenceCount(record.conformance_matrix_ready) +
      Objc3SemaEvidenceCount(record.conformance_corpus_ready) +
      Objc3SemaEvidenceCount(record.performance_quality_guardrails_ready) +
      Objc3SemaEvidenceCount(record.cross_lane_integration_sync_ready) +
      Objc3SemaEvidenceCount(record.docs_runbook_sync_ready) +
      Objc3SemaEvidenceCount(record.release_candidate_replay_ready) +
      Objc3SemaEvidenceCount(record.advanced_core_shard1_ready) +
      Objc3SemaEvidenceCount(
          record.advanced_contract_rejection_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_diagnostics_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_conformance_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_integration_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_performance_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_core_shard2_ready) +
      Objc3SemaEvidenceCount(
          record.advanced_contract_rejection_shard2_ready) +
      Objc3SemaEvidenceCount(record.advanced_diagnostics_shard2_ready) +
      Objc3SemaEvidenceCount(record.integration_closeout_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.handoff_publication_transfer_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 17u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

Objc3SemaCoreSemanticParityPublicationReadinessRecord
BuildObjc3SemaCoreSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff,
    bool deterministic_selector_normalization_handoff,
    bool deterministic_property_attribute_handoff,
    bool deterministic_type_annotation_surface_handoff,
    bool deterministic_lightweight_generic_constraint_handoff,
    bool deterministic_nullability_flow_warning_precision_handoff,
    bool deterministic_protocol_qualified_object_type_handoff,
    bool deterministic_variance_bridge_cast_handoff) {
  Objc3SemaCoreSemanticParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.semantic_diagnostics_ready = deterministic_semantic_diagnostics;
  record.type_metadata_handoff_ready = deterministic_type_metadata_handoff;
  record.interface_implementation_ready =
      deterministic_interface_implementation_handoff &&
      surface.interfaces_total == surface.type_metadata_interface_entries &&
      surface.implementations_total ==
          surface.type_metadata_implementation_entries &&
      surface.interface_implementation_summary.resolved_interfaces ==
          surface.type_metadata_interface_entries &&
      surface.interface_implementation_summary.resolved_implementations ==
          surface.type_metadata_implementation_entries;
  record.protocol_category_composition_ready =
      deterministic_protocol_category_composition_handoff &&
      surface.protocol_category_composition_summary.protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.protocol_category_composition_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.protocol_category_composition_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites <=
          surface.protocol_category_composition_summary
              .total_composition_sites();
  record.class_protocol_category_linking_ready =
      deterministic_class_protocol_category_linking_handoff &&
      surface.class_protocol_category_linking_summary.declared_interfaces ==
          surface.interface_implementation_summary.declared_interfaces &&
      surface.class_protocol_category_linking_summary.resolved_interfaces ==
          surface.interface_implementation_summary.resolved_interfaces &&
      surface.class_protocol_category_linking_summary.declared_implementations ==
          surface.interface_implementation_summary.declared_implementations &&
      surface.class_protocol_category_linking_summary.resolved_implementations ==
          surface.interface_implementation_summary.resolved_implementations &&
      surface.class_protocol_category_linking_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .linked_implementation_symbols ==
          surface.linked_implementation_symbols_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites <=
          surface.class_protocol_category_linking_summary
              .total_composition_sites() &&
      surface.class_protocol_category_linking_summary.deterministic;
  record.selector_normalization_ready =
      deterministic_selector_normalization_handoff &&
      surface.selector_normalization_summary.methods_total ==
          surface.selector_normalization_methods_total &&
      surface.selector_normalization_summary.normalized_methods ==
          surface.selector_normalization_normalized_methods_total &&
      surface.selector_normalization_summary.selector_piece_entries ==
          surface.selector_normalization_piece_entries_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries ==
          surface.selector_normalization_parameter_piece_entries_total &&
      surface.selector_normalization_summary.selector_pieceless_methods ==
          surface.selector_normalization_pieceless_methods_total &&
      surface.selector_normalization_summary.selector_spelling_mismatches ==
          surface.selector_normalization_spelling_mismatches_total &&
      surface.selector_normalization_summary.selector_arity_mismatches ==
          surface.selector_normalization_arity_mismatches_total &&
      surface.selector_normalization_summary
              .selector_parameter_linkage_mismatches ==
          surface.selector_normalization_parameter_linkage_mismatches_total &&
      surface.selector_normalization_summary
              .selector_normalization_flag_mismatches ==
          surface.selector_normalization_flag_mismatches_total &&
      surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          surface.selector_normalization_missing_keyword_pieces_total &&
      surface.selector_normalization_summary.normalized_methods <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries <=
          surface.selector_normalization_summary.selector_piece_entries &&
      surface.selector_normalization_summary.contract_violations() <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary.deterministic;
  record.property_attribute_ready =
      deterministic_property_attribute_handoff &&
      surface.property_attribute_summary.properties_total ==
          surface.property_attribute_properties_total &&
      surface.property_attribute_summary.attribute_entries ==
          surface.property_attribute_entries_total &&
      surface.property_attribute_summary.readonly_modifiers ==
          surface.property_attribute_readonly_modifiers_total &&
      surface.property_attribute_summary.readwrite_modifiers ==
          surface.property_attribute_readwrite_modifiers_total &&
      surface.property_attribute_summary.atomic_modifiers ==
          surface.property_attribute_atomic_modifiers_total &&
      surface.property_attribute_summary.nonatomic_modifiers ==
          surface.property_attribute_nonatomic_modifiers_total &&
      surface.property_attribute_summary.copy_modifiers ==
          surface.property_attribute_copy_modifiers_total &&
      surface.property_attribute_summary.strong_modifiers ==
          surface.property_attribute_strong_modifiers_total &&
      surface.property_attribute_summary.weak_modifiers ==
          surface.property_attribute_weak_modifiers_total &&
      surface.property_attribute_summary.assign_modifiers ==
          surface.property_attribute_assign_modifiers_total &&
      surface.property_attribute_summary.getter_modifiers ==
          surface.property_attribute_getter_modifiers_total &&
      surface.property_attribute_summary.setter_modifiers ==
          surface.property_attribute_setter_modifiers_total &&
      surface.property_attribute_summary.invalid_attribute_entries ==
          surface.property_attribute_invalid_attribute_entries_total &&
      surface.property_attribute_summary.property_contract_violations ==
          surface.property_attribute_contract_violations_total &&
      surface.property_attribute_summary.getter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.setter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.deterministic;
  record.type_annotation_surface_ready =
      deterministic_type_annotation_surface_handoff &&
      surface.type_annotation_surface_summary.generic_suffix_sites ==
          surface.type_annotation_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary.pointer_declarator_sites ==
          surface.type_annotation_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary.nullability_suffix_sites ==
          surface.type_annotation_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary.ownership_qualifier_sites ==
          surface.type_annotation_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.object_pointer_type_sites ==
          surface.type_annotation_object_pointer_type_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          surface.type_annotation_invalid_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_pointer_declarator_sites ==
          surface.type_annotation_invalid_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_nullability_suffix_sites ==
          surface.type_annotation_invalid_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites ==
          surface.type_annotation_invalid_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          surface.type_annotation_surface_summary.generic_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_pointer_declarator_sites <=
          surface.type_annotation_surface_summary.pointer_declarator_sites &&
      surface.type_annotation_surface_summary
              .invalid_nullability_suffix_sites <=
          surface.type_annotation_surface_summary.nullability_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites <=
          surface.type_annotation_surface_summary.ownership_qualifier_sites &&
      surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      surface.type_annotation_surface_summary.deterministic;
  record.lightweight_generic_constraint_ready =
      deterministic_lightweight_generic_constraint_handoff &&
      surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
          surface.lightweight_generic_constraint_sites_total &&
      surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
          surface.lightweight_generic_constraint_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
          surface
              .lightweight_generic_constraint_object_pointer_type_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites ==
          surface
              .lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
          surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
          surface.lightweight_generic_constraint_normalized_sites_total &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites ==
          surface
              .lightweight_generic_constraint_contract_violation_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_suffix_sites &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.deterministic;
  record.nullability_flow_warning_precision_ready =
      deterministic_nullability_flow_warning_precision_handoff &&
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
          surface.nullability_flow_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .object_pointer_type_sites ==
          surface.nullability_flow_object_pointer_type_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_nullability_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites ==
          surface.nullability_flow_nullable_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
          surface.nullability_flow_nonnull_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites ==
          surface.nullability_flow_normalized_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .contract_violation_sites ==
          surface.nullability_flow_contract_violation_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites +
              surface.nullability_flow_warning_precision_summary
                  .nonnull_suffix_sites &&
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.protocol_qualified_object_type_ready =
      deterministic_protocol_qualified_object_type_handoff &&
      surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites ==
          surface.protocol_qualified_object_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
          surface
              .protocol_qualified_object_type_object_pointer_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
          surface
              .protocol_qualified_object_type_pointer_declarator_sites_total &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites ==
          surface
              .protocol_qualified_object_type_contract_violation_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_composition_sites &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.deterministic;
  record.variance_bridge_cast_ready =
      deterministic_variance_bridge_cast_handoff &&
      surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          surface.variance_bridge_cast_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites ==
          surface.variance_bridge_cast_protocol_composition_sites_total &&
      surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          surface.variance_bridge_cast_object_pointer_type_sites_total &&
      surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          surface.variance_bridge_cast_pointer_declarator_sites_total &&
      surface.variance_bridge_cast_summary.normalized_sites ==
          surface.variance_bridge_cast_normalized_sites_total &&
      surface.variance_bridge_cast_summary.contract_violation_sites ==
          surface.variance_bridge_cast_contract_violation_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.normalized_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.contract_violation_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.semantic_diagnostics_ready) +
      Objc3SemaEvidenceCount(record.type_metadata_handoff_ready) +
      Objc3SemaEvidenceCount(record.interface_implementation_ready) +
      Objc3SemaEvidenceCount(record.protocol_category_composition_ready) +
      Objc3SemaEvidenceCount(record.class_protocol_category_linking_ready) +
      Objc3SemaEvidenceCount(record.selector_normalization_ready) +
      Objc3SemaEvidenceCount(record.property_attribute_ready) +
      Objc3SemaEvidenceCount(record.type_annotation_surface_ready) +
      Objc3SemaEvidenceCount(record.lightweight_generic_constraint_ready) +
      Objc3SemaEvidenceCount(
          record.nullability_flow_warning_precision_ready) +
      Objc3SemaEvidenceCount(record.protocol_qualified_object_type_ready) +
      Objc3SemaEvidenceCount(record.variance_bridge_cast_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.core_semantic_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 12u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

Objc3SemaIntermoduleFlowSummaryReadinessRecord
BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff) {
  Objc3SemaIntermoduleFlowSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const auto &cross_module_summary =
      surface.cross_module_conformance_summary;
  record.cross_module_conformance_ready =
      deterministic_cross_module_conformance_handoff &&
      cross_module_summary.cross_module_conformance_sites ==
          surface.cross_module_conformance_sites_total &&
      cross_module_summary.namespace_segment_sites ==
          surface.cross_module_conformance_namespace_segment_sites_total &&
      cross_module_summary.import_edge_candidate_sites ==
          surface
              .cross_module_conformance_import_edge_candidate_sites_total &&
      cross_module_summary.object_pointer_type_sites ==
          surface.cross_module_conformance_object_pointer_type_sites_total &&
      cross_module_summary.pointer_declarator_sites ==
          surface.cross_module_conformance_pointer_declarator_sites_total &&
      cross_module_summary.normalized_sites ==
          surface.cross_module_conformance_normalized_sites_total &&
      cross_module_summary.cache_invalidation_candidate_sites ==
          surface
              .cross_module_conformance_cache_invalidation_candidate_sites_total &&
      cross_module_summary.contract_violation_sites ==
          surface.cross_module_conformance_contract_violation_sites_total &&
      cross_module_summary.namespace_segment_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.import_edge_candidate_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.normalized_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.cache_invalidation_candidate_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.normalized_sites +
              cross_module_summary.cache_invalidation_candidate_sites ==
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.contract_violation_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.deterministic;
  const auto &throws_summary = surface.throws_propagation_summary;
  record.throws_propagation_ready =
      deterministic_throws_propagation_handoff &&
      throws_summary.throws_propagation_sites ==
          surface.throws_propagation_sites_total &&
      throws_summary.namespace_segment_sites ==
          surface.throws_propagation_namespace_segment_sites_total &&
      throws_summary.import_edge_candidate_sites ==
          surface.throws_propagation_import_edge_candidate_sites_total &&
      throws_summary.object_pointer_type_sites ==
          surface.throws_propagation_object_pointer_type_sites_total &&
      throws_summary.pointer_declarator_sites ==
          surface.throws_propagation_pointer_declarator_sites_total &&
      throws_summary.normalized_sites ==
          surface.throws_propagation_normalized_sites_total &&
      throws_summary.cache_invalidation_candidate_sites ==
          surface
              .throws_propagation_cache_invalidation_candidate_sites_total &&
      throws_summary.contract_violation_sites ==
          surface.throws_propagation_contract_violation_sites_total &&
      throws_summary.namespace_segment_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.import_edge_candidate_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.normalized_sites <= throws_summary.throws_propagation_sites &&
      throws_summary.cache_invalidation_candidate_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.normalized_sites +
              throws_summary.cache_invalidation_candidate_sites ==
          throws_summary.throws_propagation_sites &&
      throws_summary.contract_violation_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.intermodule_flow_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.cross_module_conformance_ready &&
      record.throws_propagation_ready;
  return record;
}

Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
BuildObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff) {
  Objc3SemaIntermoduleFlowParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const Objc3SemaIntermoduleFlowSummaryReadinessRecord
      intermodule_summary_readiness =
          BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
              input,
              surface,
              deterministic_cross_module_conformance_handoff,
              deterministic_throws_propagation_handoff);
  record.cross_module_conformance_ready =
      intermodule_summary_readiness.cross_module_conformance_ready;
  record.throws_propagation_ready =
      intermodule_summary_readiness.throws_propagation_ready;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.cross_module_conformance_ready) +
      Objc3SemaEvidenceCount(record.throws_propagation_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.intermodule_flow_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 2u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u &&
      IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          intermodule_summary_readiness);
  return record;
}

Objc3SemaParityCloseoutPublicationReadinessRecord
BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaParityCloseoutReadinessInputs readiness =
      BuildObjc3SemaParityCloseoutReadinessInputs(surface);
  Objc3SemaParityCloseoutPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.pass_manager_executed = pass_manager_executed;
  record.parser_sema_contract_ready = readiness.parser_sema_contract_ready;
  record.pass_flow_summary_ready = readiness.pass_flow_summary_ready;
  record.publication_records_ready = readiness.publication_records_ready;
  record.diagnostics_publication_ready =
      readiness.diagnostics_publication_ready;
  record.pass_flow_recovery_ready = readiness.pass_flow_recovery_ready;
  record.type_metadata_cardinality_ready =
      readiness.type_metadata_cardinality_ready;
  record.typed_semantic_handoffs_ready =
      readiness.typed_semantic_handoffs_ready;
  record.mapping_summaries_ready = readiness.mapping_summaries_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parity_closeout_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.type_metadata_mapping_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.typed_semantic_handoff_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.pass_manager_executed &&
      IsReadyObjc3SemaParityCloseoutReadinessInputs(readiness);
  return record;
}

Objc3SemaCloseoutSurfaceReadinessRecord
BuildObjc3SemaCloseoutSurfaceReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaCloseoutSurfaceReadinessInputs readiness =
      BuildObjc3SemaCloseoutSurfaceReadinessInputs(surface);
  Objc3SemaCloseoutSurfaceReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.parser_sema_contract_ready = readiness.parser_sema_contract_ready;
  record.parser_sema_conformance_evidence_ready =
      readiness.parser_sema_conformance_evidence_ready;
  record.diagnostics_publication_ready =
      readiness.diagnostics_publication_ready;
  record.pass_flow_recovery_ready = readiness.pass_flow_recovery_ready;
  record.pass_manager_publication_ready =
      readiness.pass_manager_publication_ready;
  record.type_metadata_publication_ready =
      readiness.type_metadata_publication_ready;
  record.type_metadata_mapping_ready = readiness.type_metadata_mapping_ready;
  record.typed_semantic_handoff_ready =
      readiness.typed_semantic_handoff_ready;
  record.parity_closeout_publication_ready =
      readiness.parity_closeout_publication_ready;
  record.parity_validation_ready = readiness.parity_validation_ready;
  record.core_semantic_publication_ready =
      readiness.core_semantic_publication_ready;
  record.module_semantic_publication_ready =
      readiness.module_semantic_publication_ready;
  record.intermodule_flow_publication_ready =
      readiness.intermodule_flow_publication_ready;
  record.concurrency_publication_ready = readiness.concurrency_publication_ready;
  record.unsafe_error_validation_ready =
      readiness.unsafe_error_validation_ready;
  record.control_binding_validation_ready =
      readiness.control_binding_validation_ready;
  record.async_block_message_validation_ready =
      readiness.async_block_message_validation_ready;
  record.dispatch_runtime_arc_validation_ready =
      readiness.dispatch_runtime_arc_validation_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.closeout_surface_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parity_closeout_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      IsReadyObjc3SemaCloseoutSurfaceReadinessInputs(readiness);
  return record;
}

Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaParityCloseoutPublicationReadinessRecord
      &closeout_readiness =
          surface.parity_closeout_publication_readiness_record;
  return BuildObjc3SemaCloseoutSignoffRecord(
      input,
      surface.ready && surface.deterministic_parity_validation_record &&
          IsReadyObjc3SemaParityValidationRecord(
              surface.parity_validation_record),
      closeout_readiness.parser_sema_contract_ready,
      surface.deterministic_pass_manager_publication_record &&
          IsReadyObjc3SemaPassManagerPublicationRecord(
              surface.pass_manager_publication_record),
      surface.deterministic_type_metadata_publication_record &&
          IsReadyObjc3SemaTypeMetadataPublicationRecord(
              surface.type_metadata_publication_record),
      closeout_readiness.diagnostics_publication_ready,
      closeout_readiness.pass_flow_recovery_ready,
      closeout_readiness.mapping_summaries_ready,
      closeout_readiness.typed_semantic_handoffs_ready);
}

bool IsReadyObjc3SemaParityContractSurface(
    const Objc3SemaParityContractSurface &surface) {
  return IsReadyObjc3SemaParityContractSurfaceReadinessGates(
      BuildObjc3SemaParityContractSurfaceReadinessGates(surface));
}
