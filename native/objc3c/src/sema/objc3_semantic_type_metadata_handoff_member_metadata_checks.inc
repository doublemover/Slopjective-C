  const auto is_deterministic_method_metadata =
      [](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.selector.empty() ||
            metadata.selector_normalized.empty() ||
            metadata.selector != metadata.selector_normalized ||
            metadata.selector_parameter_piece_count > metadata.selector_piece_count ||
            metadata.selector_had_pieceless_form !=
                (metadata.selector_piece_count == 0u) ||
            metadata.selector_has_arity_mismatch !=
                (metadata.selector_parameter_piece_count != metadata.arity)) {
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
        if (metadata.param_types.size() != metadata.arity ||
            metadata.param_is_vector.size() != metadata.arity ||
            metadata.param_vector_base_spelling.size() != metadata.arity ||
            metadata.param_vector_lane_count.size() != metadata.arity ||
            metadata.param_has_generic_suffix.size() != metadata.arity ||
            metadata.param_has_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_ownership_qualifier.size() != metadata.arity ||
            metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
            metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
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
        return true;
      };
  const auto is_deterministic_property_metadata =
      [](const Objc3SemanticPropertyTypeMetadata &metadata) {
        const bool expected_invalid_type_suffix =
            metadata.has_invalid_generic_suffix ||
            metadata.has_invalid_pointer_declarator ||
            metadata.has_invalid_nullability_suffix ||
            metadata.has_invalid_ownership_qualifier;
        if (metadata.name.empty() ||
            (metadata.has_invalid_generic_suffix &&
             !metadata.has_generic_suffix) ||
            (metadata.has_invalid_pointer_declarator &&
             !metadata.has_pointer_declarator) ||
            (metadata.has_invalid_nullability_suffix &&
             !metadata.has_nullability_suffix) ||
            (metadata.has_invalid_ownership_qualifier &&
             !metadata.has_ownership_qualifier) ||
            metadata.has_invalid_type_suffix !=
                expected_invalid_type_suffix) {
          return false;
        }
        if ((!metadata.has_ownership_qualifier &&
             (metadata.ownership_insert_retain ||
              metadata.ownership_insert_release ||
              metadata.ownership_insert_autorelease)) ||
            (metadata.ownership_insert_autorelease &&
             (metadata.ownership_insert_retain ||
              metadata.ownership_insert_release))) {
          return false;
        }
        if ((!metadata.ownership_arc_diagnostic_candidate &&
             (metadata.ownership_arc_fixit_available ||
              !metadata.ownership_arc_diagnostic_profile.empty() ||
              !metadata.ownership_arc_fixit_hint.empty())) ||
            (metadata.ownership_arc_fixit_available &&
             metadata.ownership_arc_fixit_hint.empty())) {
          return false;
        }
        if (!IsSortedUniqueStrings(metadata.attribute_names_lexicographic)) {
          return false;
        }
        if (metadata.attribute_entries !=
            metadata.attribute_names_lexicographic.size()) {
          return false;
        }
        if (!metadata.has_getter && !metadata.getter_selector.empty()) {
          return false;
        }
        if (!metadata.has_setter && !metadata.setter_selector.empty()) {
          return false;
        }
        if (!metadata.effective_setter_available &&
            !metadata.effective_setter_selector.empty()) {
          return false;
        }
        return true;
      };

  for (const auto &metadata : handoff.functions_lexicographic) {
    if (metadata.param_types.size() != metadata.arity ||
        metadata.param_is_vector.size() != metadata.arity ||
        metadata.param_vector_base_spelling.size() != metadata.arity ||
        metadata.param_vector_lane_count.size() != metadata.arity ||
        metadata.param_has_generic_suffix.size() != metadata.arity ||
        metadata.param_has_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
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
      return "function metadata row '" + metadata.name +
             "' has parameter metadata arrays that drift from arity";
    }
  }

  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    if (!std::is_sorted(interface_metadata.methods_lexicographic.begin(),
                        interface_metadata.methods_lexicographic.end(),
                        [](const Objc3SemanticMethodTypeMetadata &lhs,
                           const Objc3SemanticMethodTypeMetadata &rhs) {
                          return lhs.selector < rhs.selector;
                        })) {
      return "interface '" + interface_metadata.name +
             "' method metadata rows are not lexicographically sorted";
    }
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      if (!is_deterministic_method_metadata(method_metadata)) {
        return "interface '" + interface_metadata.name + "' method '" +
               method_metadata.selector +
               "' metadata is not deterministic";
      }
    }
    if (!std::is_sorted(interface_metadata.properties_lexicographic.begin(),
                        interface_metadata.properties_lexicographic.end(),
                        [](const Objc3SemanticPropertyTypeMetadata &lhs,
                           const Objc3SemanticPropertyTypeMetadata &rhs) {
                          return lhs.name < rhs.name;
                        })) {
      return "interface '" + interface_metadata.name +
             "' property metadata rows are not lexicographically sorted";
    }
    for (const auto &property_metadata :
         interface_metadata.properties_lexicographic) {
      if (!is_deterministic_property_metadata(property_metadata)) {
        return "interface '" + interface_metadata.name + "' property '" +
               property_metadata.name +
               "' metadata is not deterministic";
      }
    }
  }

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    if (!std::is_sorted(implementation_metadata.methods_lexicographic.begin(),
                        implementation_metadata.methods_lexicographic.end(),
                        [](const Objc3SemanticMethodTypeMetadata &lhs,
                           const Objc3SemanticMethodTypeMetadata &rhs) {
                          return lhs.selector < rhs.selector;
                        })) {
      return "implementation '" + implementation_metadata.name +
             "' method metadata rows are not lexicographically sorted";
    }
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      if (!is_deterministic_method_metadata(method_metadata)) {
        return "implementation '" + implementation_metadata.name + "' method '" +
               method_metadata.selector +
               "' metadata is not deterministic";
      }
    }
    if (!std::is_sorted(implementation_metadata.properties_lexicographic.begin(),
                        implementation_metadata.properties_lexicographic.end(),
                        [](const Objc3SemanticPropertyTypeMetadata &lhs,
                           const Objc3SemanticPropertyTypeMetadata &rhs) {
                          return lhs.name < rhs.name;
                        })) {
      return "implementation '" + implementation_metadata.name +
             "' property metadata rows are not lexicographically sorted";
    }
    for (const auto &property_metadata :
         implementation_metadata.properties_lexicographic) {
      if (!is_deterministic_property_metadata(property_metadata)) {
        return "implementation '" + implementation_metadata.name +
               "' property '" + property_metadata.name +
               "' metadata is not deterministic";
      }
    }
  }
