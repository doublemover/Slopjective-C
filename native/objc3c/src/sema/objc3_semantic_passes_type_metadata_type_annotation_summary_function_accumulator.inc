  const auto accumulate_function_type_annotations =
      [&handoff](const Objc3SemanticFunctionTypeMetadata &metadata) {
        if (metadata.param_has_generic_suffix.size() != metadata.arity ||
            metadata.param_has_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_ownership_qualifier.size() != metadata.arity ||
            metadata.param_id_spelling.size() != metadata.arity ||
            metadata.param_class_spelling.size() != metadata.arity ||
            metadata.param_instancetype_spelling.size() != metadata.arity ||
            metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
            metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
            metadata.param_has_invalid_type_suffix.size() != metadata.arity) {
          handoff.type_annotation_surface_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          if (metadata.param_has_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.generic_suffix_sites;
          }
          if (metadata.param_has_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
          }
          if (metadata.param_has_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
          }
          if (metadata.param_has_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
          }
          if (IsObjCReferenceAnnotationSite(metadata.param_id_spelling[i],
                                            metadata.param_class_spelling[i],
                                            metadata.param_instancetype_spelling[i],
                                            metadata.param_object_pointer_type_spelling[i])) {
            ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
          }
          if (metadata.param_has_invalid_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
          }
          if (metadata.param_has_invalid_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
          }
          if (metadata.param_has_invalid_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
          }
          if (metadata.param_has_invalid_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
          }
          const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                        metadata.param_has_invalid_pointer_declarator[i] ||
                                        metadata.param_has_invalid_nullability_suffix[i] ||
                                        metadata.param_has_invalid_ownership_qualifier[i];
          if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
            handoff.type_annotation_surface_summary.deterministic = false;
          }
        }
        if (metadata.return_has_generic_suffix) {
          ++handoff.type_annotation_surface_summary.generic_suffix_sites;
        }
        if (metadata.return_has_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
        }
        if (metadata.return_has_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
        }
        if (metadata.return_has_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.return_id_spelling,
                                          metadata.return_class_spelling,
                                          metadata.return_instancetype_spelling,
                                          metadata.return_object_pointer_type_spelling)) {
          ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
        }
        if (metadata.return_has_invalid_generic_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
        }
        if (metadata.return_has_invalid_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.return_has_invalid_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.return_has_invalid_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_return_invalid = metadata.return_has_invalid_generic_suffix ||
                                             metadata.return_has_invalid_pointer_declarator ||
                                             metadata.return_has_invalid_nullability_suffix ||
                                             metadata.return_has_invalid_ownership_qualifier;
        if (metadata.return_has_invalid_type_suffix != expected_return_invalid) {
          handoff.type_annotation_surface_summary.deterministic = false;
        }
      };
