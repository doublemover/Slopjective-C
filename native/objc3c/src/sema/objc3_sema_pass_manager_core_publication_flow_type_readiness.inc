namespace {

static void PopulateObjc3SemaCorePublicationFlowTypeReadiness(
    Objc3SemaCoreSemanticParityPublicationReadinessRecord &record,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_selector_normalization_handoff,
    bool deterministic_property_attribute_handoff,
    bool deterministic_type_annotation_surface_handoff) {
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
}

}  // namespace
