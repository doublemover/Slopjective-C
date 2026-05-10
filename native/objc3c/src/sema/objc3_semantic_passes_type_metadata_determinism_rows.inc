  const bool deterministic_functions =
      std::all_of(handoff.functions_lexicographic.begin(),
                  handoff.functions_lexicographic.end(),
                  [](const Objc3SemanticFunctionTypeMetadata &metadata) {
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
                          metadata.return_has_invalid_nullability_suffix ||
                          metadata.return_has_invalid_ownership_qualifier))) {
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
                          (metadata.param_has_invalid_ownership_qualifier[i] &&
                           !metadata.param_has_ownership_qualifier[i]) ||
                          metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
                        return false;
                      }
                      if ((!metadata.param_has_ownership_qualifier[i] &&
                           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i] ||
                            metadata.param_ownership_insert_autorelease[i])) ||
                          (metadata.param_ownership_insert_autorelease[i] &&
                           (metadata.param_ownership_insert_retain[i] ||
                            metadata.param_ownership_insert_release[i]))) {
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
                      if (metadata.param_has_invalid_protocol_composition[i] &&
                          !metadata.param_has_protocol_composition[i]) {
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
                  });

  const bool deterministic_interfaces =
      std::all_of(handoff.interfaces_lexicographic.begin(),
                  handoff.interfaces_lexicographic.end(),
                  [&is_deterministic_method_metadata](const Objc3SemanticInterfaceTypeMetadata &metadata) {
                    return std::is_sorted(metadata.methods_lexicographic.begin(),
                                          metadata.methods_lexicographic.end(),
                                          [](const Objc3SemanticMethodTypeMetadata &lhs,
                                             const Objc3SemanticMethodTypeMetadata &rhs) {
                                            return lhs.selector < rhs.selector;
                                          }) &&
                           std::all_of(metadata.methods_lexicographic.begin(),
                                       metadata.methods_lexicographic.end(),
                                       is_deterministic_method_metadata);
                  });

  const bool deterministic_implementations =
      std::all_of(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [&is_deterministic_method_metadata](const Objc3SemanticImplementationTypeMetadata &metadata) {
            return std::is_sorted(metadata.methods_lexicographic.begin(),
                                  metadata.methods_lexicographic.end(),
                                  [](const Objc3SemanticMethodTypeMetadata &lhs,
                                     const Objc3SemanticMethodTypeMetadata &rhs) {
                                    return lhs.selector < rhs.selector;
                                  }) &&
                   std::all_of(metadata.methods_lexicographic.begin(),
                               metadata.methods_lexicographic.end(),
                               is_deterministic_method_metadata);
          });

  if (!deterministic_functions || !deterministic_interfaces || !deterministic_implementations) {
    return false;
  }
