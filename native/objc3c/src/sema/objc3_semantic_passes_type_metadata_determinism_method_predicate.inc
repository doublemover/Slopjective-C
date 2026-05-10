  const auto is_deterministic_method_metadata = [](const Objc3SemanticMethodTypeMetadata &metadata) {
    if (metadata.selector.empty() ||
        metadata.selector_normalized.empty() ||
        metadata.selector != metadata.selector_normalized ||
        metadata.selector_parameter_piece_count > metadata.selector_piece_count ||
        metadata.selector_had_pieceless_form != (metadata.selector_piece_count == 0u) ||
        metadata.selector_has_arity_mismatch != (metadata.selector_parameter_piece_count != metadata.arity)) {
      return false;
    }
    if (metadata.selector_contract_normalized &&
        (metadata.selector_had_pieceless_form ||
         metadata.selector_has_spelling_mismatch ||
         metadata.selector_has_arity_mismatch ||
         metadata.selector_has_parameter_linkage_mismatch ||
         metadata.selector_has_normalization_flag_mismatch ||
         metadata.selector_has_missing_piece_keyword)) {
      return false;
    }
    if (metadata.selector_has_missing_piece_keyword && metadata.selector_contract_normalized) {
      return false;
    }
    const bool param_types_match_arity = metadata.param_types.size() == metadata.arity;
    const bool param_invalid_type_suffix_match_arity =
        metadata.param_has_invalid_type_suffix.size() == metadata.arity;
    const bool param_is_vector_match_arity = metadata.param_is_vector.size() == metadata.arity;
    const bool param_vector_base_spelling_match_arity =
        metadata.param_vector_base_spelling.size() == metadata.arity;
    const bool param_vector_lane_count_match_arity =
        metadata.param_vector_lane_count.size() == metadata.arity;
    if (!param_types_match_arity ||
        metadata.param_types.size() != metadata.arity ||
        !param_is_vector_match_arity ||
        !param_vector_base_spelling_match_arity ||
        !param_vector_lane_count_match_arity ||
        metadata.param_has_generic_suffix.size() != metadata.arity ||
        metadata.param_has_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
        !param_invalid_type_suffix_match_arity ||
        metadata.param_has_invalid_type_suffix.size() != metadata.arity ||
        metadata.param_ownership_insert_retain.size() != metadata.arity ||
        metadata.param_ownership_insert_release.size() != metadata.arity ||
        metadata.param_ownership_insert_autorelease.size() != metadata.arity ||
        metadata.param_ownership_arc_diagnostic_candidate.size() != metadata.arity ||
        metadata.param_ownership_arc_fixit_available.size() != metadata.arity ||
        metadata.param_ownership_arc_diagnostic_profile.size() != metadata.arity ||
        metadata.param_ownership_arc_fixit_hint.size() != metadata.arity ||
        metadata.param_has_protocol_composition.size() != metadata.arity ||
        metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
        metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
      return false;
    }
    if ((!metadata.return_ownership_arc_diagnostic_candidate &&
         (metadata.return_ownership_arc_fixit_available ||
          !metadata.return_ownership_arc_diagnostic_profile.empty() ||
          !metadata.return_ownership_arc_fixit_hint.empty())) ||
        (metadata.return_ownership_arc_fixit_available &&
         metadata.return_ownership_arc_fixit_hint.empty())) {
      return false;
    }
    if ((metadata.return_has_invalid_generic_suffix && !metadata.return_has_generic_suffix) ||
        (metadata.return_has_invalid_pointer_declarator && !metadata.return_has_pointer_declarator) ||
        (metadata.return_has_invalid_nullability_suffix && !metadata.return_has_nullability_suffix) ||
        (metadata.return_has_invalid_ownership_qualifier && !metadata.return_has_ownership_qualifier) ||
        (metadata.return_has_invalid_type_suffix !=
         (metadata.return_has_invalid_generic_suffix || metadata.return_has_invalid_pointer_declarator ||
          metadata.return_has_invalid_nullability_suffix || metadata.return_has_invalid_ownership_qualifier))) {
      return false;
    }
    if (metadata.return_has_invalid_protocol_composition && !metadata.return_has_protocol_composition) {
      return false;
    }
    if (metadata.return_has_protocol_composition &&
        !IsSortedUniqueStrings(metadata.return_protocol_composition_lexicographic)) {
      return false;
    }
    for (std::size_t i = 0; i < metadata.arity; ++i) {
      const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                    metadata.param_has_invalid_pointer_declarator[i] ||
                                    metadata.param_has_invalid_nullability_suffix[i] ||
                                    metadata.param_has_invalid_ownership_qualifier[i];
      if ((metadata.param_has_invalid_generic_suffix[i] && !metadata.param_has_generic_suffix[i]) ||
          (metadata.param_has_invalid_pointer_declarator[i] && !metadata.param_has_pointer_declarator[i]) ||
          (metadata.param_has_invalid_nullability_suffix[i] && !metadata.param_has_nullability_suffix[i]) ||
          (metadata.param_has_invalid_ownership_qualifier[i] && !metadata.param_has_ownership_qualifier[i]) ||
          metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
        return false;
      }
      if ((!metadata.param_has_ownership_qualifier[i] &&
           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i] ||
            metadata.param_ownership_insert_autorelease[i])) ||
          (metadata.param_ownership_insert_autorelease[i] &&
           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i]))) {
        return false;
      }
      if ((!metadata.param_ownership_arc_diagnostic_candidate[i] &&
           (metadata.param_ownership_arc_fixit_available[i] ||
            !metadata.param_ownership_arc_diagnostic_profile[i].empty() ||
            !metadata.param_ownership_arc_fixit_hint[i].empty())) ||
          (metadata.param_ownership_arc_fixit_available[i] &&
           metadata.param_ownership_arc_fixit_hint[i].empty())) {
        return false;
      }
      if (!IsSortedUniqueStrings(metadata.param_protocol_composition_lexicographic[i])) {
        return false;
      }
      if (metadata.param_has_invalid_protocol_composition[i] && !metadata.param_has_protocol_composition[i]) {
        return false;
      }
    }
    if ((!metadata.return_has_ownership_qualifier &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
          metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      return false;
    }
    return true;
  };
