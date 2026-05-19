#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord
BuildObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
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
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
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
  record.strict_no_retired_route = input.strict_no_retired_route;
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
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.lightweight_generic_constraint_ready &&
      record.nullability_flow_warning_precision_ready &&
      record.protocol_qualified_object_type_ready;
  return record;
}
