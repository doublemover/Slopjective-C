  if (handoff.type_annotation_surface_summary.generic_suffix_sites !=
          type_annotation_summary.generic_suffix_sites ||
      handoff.type_annotation_surface_summary.pointer_declarator_sites !=
          type_annotation_summary.pointer_declarator_sites ||
      handoff.type_annotation_surface_summary.nullability_suffix_sites !=
          type_annotation_summary.nullability_suffix_sites ||
      handoff.type_annotation_surface_summary.ownership_qualifier_sites !=
          type_annotation_summary.ownership_qualifier_sites ||
      handoff.type_annotation_surface_summary.object_pointer_type_sites !=
          type_annotation_summary.object_pointer_type_sites ||
      handoff.type_annotation_surface_summary.invalid_generic_suffix_sites !=
          type_annotation_summary.invalid_generic_suffix_sites ||
      handoff.type_annotation_surface_summary
              .invalid_pointer_declarator_sites !=
          type_annotation_summary.invalid_pointer_declarator_sites ||
      handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites !=
          type_annotation_summary.invalid_nullability_suffix_sites ||
      handoff.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites !=
          type_annotation_summary.invalid_ownership_qualifier_sites) {
    std::ostringstream detail;
    detail << "type-annotation summary drifted from recomputed handoff"
           << " (object-pointer="
           << handoff.type_annotation_surface_summary.object_pointer_type_sites
           << "/" << type_annotation_summary.object_pointer_type_sites
           << ", ownership="
           << handoff.type_annotation_surface_summary.ownership_qualifier_sites
           << "/" << type_annotation_summary.ownership_qualifier_sites
           << ", invalid="
           << handoff.type_annotation_surface_summary.invalid_type_annotation_sites()
           << "/" << type_annotation_summary.invalid_type_annotation_sites()
           << ")";
    return detail.str();
  }
