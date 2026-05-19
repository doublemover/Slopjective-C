  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function_type_annotations(metadata);
  }
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method);
    }
    for (const auto &property : metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method);
    }
    for (const auto &property : metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property);
    }
  }
  type_annotation_summary.deterministic =
      type_annotation_summary.deterministic &&
      type_annotation_summary.invalid_generic_suffix_sites <=
          type_annotation_summary.generic_suffix_sites &&
      type_annotation_summary.invalid_pointer_declarator_sites <=
          type_annotation_summary.pointer_declarator_sites &&
      type_annotation_summary.invalid_nullability_suffix_sites <=
          type_annotation_summary.nullability_suffix_sites &&
      type_annotation_summary.invalid_ownership_qualifier_sites <=
          type_annotation_summary.ownership_qualifier_sites &&
      type_annotation_summary.invalid_type_annotation_sites() <=
          type_annotation_summary.total_type_annotation_sites();
  if (!type_annotation_summary.deterministic) {
    return "type-annotation summary is not deterministic";
  }
