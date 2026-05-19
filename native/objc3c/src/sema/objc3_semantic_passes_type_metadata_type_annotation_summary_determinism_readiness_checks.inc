  for (const auto &function_metadata : handoff.functions_lexicographic) {
    accumulate_function_type_annotations(function_metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property_metadata);
    }
  }
  handoff.type_annotation_surface_summary.deterministic =
      handoff.type_annotation_surface_summary.deterministic &&
      handoff.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          handoff.type_annotation_surface_summary.generic_suffix_sites &&
      handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          handoff.type_annotation_surface_summary.pointer_declarator_sites &&
      handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          handoff.type_annotation_surface_summary.nullability_suffix_sites &&
      handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          handoff.type_annotation_surface_summary.ownership_qualifier_sites &&
      handoff.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          handoff.type_annotation_surface_summary.total_type_annotation_sites();
