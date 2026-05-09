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
