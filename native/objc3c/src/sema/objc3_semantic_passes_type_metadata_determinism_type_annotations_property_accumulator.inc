  const auto accumulate_property_type_annotations =
      [&type_annotation_summary](
          const Objc3SemanticPropertyTypeMetadata &metadata) {
        if (metadata.has_generic_suffix) {
          ++type_annotation_summary.generic_suffix_sites;
        }
        if (metadata.has_pointer_declarator) {
          ++type_annotation_summary.pointer_declarator_sites;
        }
        if (metadata.has_nullability_suffix) {
          ++type_annotation_summary.nullability_suffix_sites;
        }
        if (metadata.has_ownership_qualifier) {
          ++type_annotation_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.id_spelling,
                                          metadata.class_spelling,
                                          metadata.instancetype_spelling,
                                          metadata.object_pointer_type_spelling)) {
          ++type_annotation_summary.object_pointer_type_sites;
        }
        if (metadata.has_invalid_generic_suffix) {
          ++type_annotation_summary.invalid_generic_suffix_sites;
        }
        if (metadata.has_invalid_pointer_declarator) {
          ++type_annotation_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.has_invalid_nullability_suffix) {
          ++type_annotation_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.has_invalid_ownership_qualifier) {
          ++type_annotation_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_invalid =
            metadata.has_invalid_generic_suffix ||
            metadata.has_invalid_pointer_declarator ||
            metadata.has_invalid_nullability_suffix ||
            metadata.has_invalid_ownership_qualifier;
        if (metadata.has_invalid_type_suffix != expected_invalid) {
          type_annotation_summary.deterministic = false;
        }
      };
