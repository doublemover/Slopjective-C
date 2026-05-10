#include "sema/objc3_semantic_passes_type_metadata_protocol_category_summary.inc"

#include "sema/objc3_semantic_passes_type_metadata_type_annotation_summary.inc"
#include "sema/objc3_semantic_passes_type_metadata_summary_publication.inc"
  return handoff;
}

bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
#include "sema/objc3_semantic_passes_type_metadata_determinism_ordering.inc"

#include "sema/objc3_semantic_passes_type_metadata_determinism_method_predicate.inc"

#include "sema/objc3_semantic_passes_type_metadata_determinism_rows.inc"

#include "sema/objc3_semantic_passes_type_metadata_determinism_protocol_category.inc"

#include "sema/objc3_semantic_passes_type_metadata_determinism_selector_summary.inc"

#include "sema/objc3_semantic_passes_type_metadata_determinism_type_annotations.inc"

#include "sema/objc3_semantic_passes_type_metadata_determinism_summary_recompute.inc"
  const bool deterministic = summary.deterministic &&
         summary.resolved_interfaces == handoff.interfaces_lexicographic.size() &&
         summary.resolved_implementations == handoff.implementations_lexicographic.size() &&
         summary.interface_method_symbols == interface_method_symbols &&
         summary.implementation_method_symbols == implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.interface_method_symbols &&
         handoff.class_protocol_category_linking_summary.deterministic &&
         handoff.class_protocol_category_linking_summary.declared_interfaces ==
             class_protocol_category_linking_summary.declared_interfaces &&
         handoff.class_protocol_category_linking_summary.resolved_interfaces ==
             class_protocol_category_linking_summary.resolved_interfaces &&
         handoff.class_protocol_category_linking_summary.declared_implementations ==
             class_protocol_category_linking_summary.declared_implementations &&
         handoff.class_protocol_category_linking_summary.resolved_implementations ==
             class_protocol_category_linking_summary.resolved_implementations &&
         handoff.class_protocol_category_linking_summary.interface_method_symbols ==
             class_protocol_category_linking_summary.interface_method_symbols &&
         handoff.class_protocol_category_linking_summary.implementation_method_symbols ==
             class_protocol_category_linking_summary.implementation_method_symbols &&
         handoff.class_protocol_category_linking_summary.linked_implementation_symbols ==
             class_protocol_category_linking_summary.linked_implementation_symbols &&
         handoff.class_protocol_category_linking_summary.protocol_composition_sites ==
             class_protocol_category_linking_summary.protocol_composition_sites &&
         handoff.class_protocol_category_linking_summary.protocol_composition_symbols ==
             class_protocol_category_linking_summary.protocol_composition_symbols &&
         handoff.class_protocol_category_linking_summary.category_composition_sites ==
             class_protocol_category_linking_summary.category_composition_sites &&
         handoff.class_protocol_category_linking_summary.category_composition_symbols ==
             class_protocol_category_linking_summary.category_composition_symbols &&
         handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
             class_protocol_category_linking_summary.invalid_protocol_composition_sites &&
         handoff.protocol_category_composition_summary.deterministic &&
         handoff.protocol_category_composition_summary.protocol_composition_sites ==
             protocol_category_summary.protocol_composition_sites &&
         handoff.protocol_category_composition_summary.protocol_composition_symbols ==
             protocol_category_summary.protocol_composition_symbols &&
         handoff.protocol_category_composition_summary.category_composition_sites ==
             protocol_category_summary.category_composition_sites &&
         handoff.protocol_category_composition_summary.category_composition_symbols ==
              protocol_category_summary.category_composition_symbols &&
         handoff.protocol_category_composition_summary.invalid_protocol_composition_sites ==
              protocol_category_summary.invalid_protocol_composition_sites &&
         handoff.selector_normalization_summary.deterministic &&
         handoff.selector_normalization_summary.methods_total == selector_summary.methods_total &&
         handoff.selector_normalization_summary.normalized_methods == selector_summary.normalized_methods &&
         handoff.selector_normalization_summary.selector_piece_entries == selector_summary.selector_piece_entries &&
         handoff.selector_normalization_summary.selector_parameter_piece_entries ==
             selector_summary.selector_parameter_piece_entries &&
         handoff.selector_normalization_summary.selector_pieceless_methods ==
             selector_summary.selector_pieceless_methods &&
         handoff.selector_normalization_summary.selector_spelling_mismatches ==
             selector_summary.selector_spelling_mismatches &&
         handoff.selector_normalization_summary.selector_arity_mismatches ==
             selector_summary.selector_arity_mismatches &&
         handoff.selector_normalization_summary.selector_parameter_linkage_mismatches ==
             selector_summary.selector_parameter_linkage_mismatches &&
         handoff.selector_normalization_summary.selector_normalization_flag_mismatches ==
             selector_summary.selector_normalization_flag_mismatches &&
         handoff.selector_normalization_summary.selector_missing_keyword_pieces ==
             selector_summary.selector_missing_keyword_pieces &&
         handoff.type_annotation_surface_summary.deterministic &&
         handoff.type_annotation_surface_summary.generic_suffix_sites == type_annotation_summary.generic_suffix_sites &&
         handoff.type_annotation_surface_summary.pointer_declarator_sites ==
             type_annotation_summary.pointer_declarator_sites &&
         handoff.type_annotation_surface_summary.nullability_suffix_sites ==
             type_annotation_summary.nullability_suffix_sites &&
         handoff.type_annotation_surface_summary.ownership_qualifier_sites ==
             type_annotation_summary.ownership_qualifier_sites &&
         handoff.type_annotation_surface_summary.object_pointer_type_sites ==
             type_annotation_summary.object_pointer_type_sites &&
         handoff.type_annotation_surface_summary.invalid_generic_suffix_sites ==
             type_annotation_summary.invalid_generic_suffix_sites &&
         handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
             type_annotation_summary.invalid_pointer_declarator_sites &&
         handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
             type_annotation_summary.invalid_nullability_suffix_sites &&
         handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==
             type_annotation_summary.invalid_ownership_qualifier_sites &&
         handoff.lightweight_generic_constraint_summary.deterministic &&
         handoff.lightweight_generic_constraint_summary.generic_constraint_sites ==
             lightweight_generic_constraint_summary.generic_constraint_sites &&
         handoff.lightweight_generic_constraint_summary.generic_suffix_sites ==
             lightweight_generic_constraint_summary.generic_suffix_sites &&
         handoff.lightweight_generic_constraint_summary.object_pointer_type_sites ==
             lightweight_generic_constraint_summary.object_pointer_type_sites &&
         handoff.lightweight_generic_constraint_summary.terminated_generic_suffix_sites ==
             lightweight_generic_constraint_summary.terminated_generic_suffix_sites &&
         handoff.lightweight_generic_constraint_summary.pointer_declarator_sites ==
             lightweight_generic_constraint_summary.pointer_declarator_sites &&
         handoff.lightweight_generic_constraint_summary.normalized_constraint_sites ==
             lightweight_generic_constraint_summary.normalized_constraint_sites &&
         handoff.lightweight_generic_constraint_summary.contract_violation_sites ==
             lightweight_generic_constraint_summary.contract_violation_sites &&
         handoff.lightweight_generic_constraint_summary.terminated_generic_suffix_sites <=
             handoff.lightweight_generic_constraint_summary.generic_suffix_sites &&
         handoff.lightweight_generic_constraint_summary.normalized_constraint_sites <=
             handoff.lightweight_generic_constraint_summary.generic_constraint_sites &&
         handoff.lightweight_generic_constraint_summary.contract_violation_sites <=
             handoff.lightweight_generic_constraint_summary.generic_constraint_sites &&
         handoff.nullability_flow_warning_precision_summary.deterministic &&
         handoff.nullability_flow_warning_precision_summary.nullability_flow_sites ==
             nullability_flow_warning_precision_summary.nullability_flow_sites &&
         handoff.nullability_flow_warning_precision_summary.object_pointer_type_sites ==
             nullability_flow_warning_precision_summary.object_pointer_type_sites &&
         handoff.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
             nullability_flow_warning_precision_summary.nullability_suffix_sites &&
         handoff.nullability_flow_warning_precision_summary.nullable_suffix_sites ==
             nullability_flow_warning_precision_summary.nullable_suffix_sites &&
         handoff.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
             nullability_flow_warning_precision_summary.nonnull_suffix_sites &&
         handoff.nullability_flow_warning_precision_summary.normalized_sites ==
             nullability_flow_warning_precision_summary.normalized_sites &&
         handoff.nullability_flow_warning_precision_summary.contract_violation_sites ==
             nullability_flow_warning_precision_summary.contract_violation_sites &&
