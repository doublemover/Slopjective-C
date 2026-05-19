    bool compatible = existing.arity == fn.params.size() && existing.return_type == fn.return_type &&
                      existing.return_is_vector == fn.return_vector_spelling &&
                      existing.return_has_ownership_qualifier == fn.has_return_ownership_qualifier &&
                      existing.return_ownership_insert_retain == fn.return_ownership_insert_retain &&
                      existing.return_ownership_insert_release == fn.return_ownership_insert_release &&
                      existing.return_ownership_insert_autorelease == fn.return_ownership_insert_autorelease &&
                      existing.return_ownership_is_weak_reference == fn.return_ownership_is_weak_reference &&
                      existing.return_ownership_is_unowned_reference == fn.return_ownership_is_unowned_reference &&
                      existing.return_ownership_is_unowned_safe_reference ==
                          fn.return_ownership_is_unowned_safe_reference &&
                      existing.return_ownership_arc_diagnostic_candidate ==
                          fn.return_ownership_arc_diagnostic_candidate &&
                      existing.return_ownership_arc_fixit_available == fn.return_ownership_arc_fixit_available &&
                      existing.return_ownership_arc_diagnostic_profile ==
                          fn.return_ownership_arc_diagnostic_profile &&
                      existing.return_ownership_arc_fixit_hint == fn.return_ownership_arc_fixit_hint;
    if (compatible && existing.return_is_vector) {
      compatible = existing.return_vector_base_spelling == fn.return_vector_base_spelling &&
                   existing.return_vector_lane_count == fn.return_vector_lane_count;
    }
    const ProtocolCompositionInfo return_protocol_composition =
        BuildProtocolCompositionInfoFromFunctionReturn(fn,
                                                      &generic_interface_names);
    if (compatible && !AreEquivalentProtocolCompositions(existing.return_has_protocol_composition,
                                                         existing.return_protocol_composition_lexicographic,
                                                         return_protocol_composition.has_protocol_composition,
                                                         return_protocol_composition.names_lexicographic)) {
      compatible = false;
    }
    if (compatible) {
      for (std::size_t i = 0; i < fn.params.size(); ++i) {
        const ProtocolCompositionInfo param_protocol_composition =
            BuildProtocolCompositionInfoFromParam(fn.params[i],
                                                 &generic_interface_names);
        if (i >= existing.param_types.size() || i >= existing.param_is_vector.size() ||
            i >= existing.param_vector_base_spelling.size() || i >= existing.param_vector_lane_count.size() ||
            i >= existing.param_has_ownership_qualifier.size() ||
            i >= existing.param_ownership_insert_retain.size() ||
            i >= existing.param_ownership_insert_release.size() ||
            i >= existing.param_ownership_insert_autorelease.size() ||
            i >= existing.param_ownership_is_weak_reference.size() ||
            i >= existing.param_ownership_is_unowned_reference.size() ||
            i >= existing.param_ownership_is_unowned_safe_reference.size() ||
            i >= existing.param_ownership_arc_diagnostic_candidate.size() ||
            i >= existing.param_ownership_arc_fixit_available.size() ||
            i >= existing.param_ownership_arc_diagnostic_profile.size() ||
            i >= existing.param_ownership_arc_fixit_hint.size() ||
            i >= existing.param_has_protocol_composition.size() ||
            i >= existing.param_protocol_composition_lexicographic.size() ||
            existing.param_types[i] != fn.params[i].type ||
            existing.param_is_vector[i] != fn.params[i].vector_spelling) {
          compatible = false;
          break;
        }
        if (existing.param_has_ownership_qualifier[i] != fn.params[i].has_ownership_qualifier) {
          compatible = false;
          break;
        }
        if (existing.param_ownership_insert_retain[i] != fn.params[i].ownership_insert_retain ||
            existing.param_ownership_insert_release[i] != fn.params[i].ownership_insert_release ||
            existing.param_ownership_insert_autorelease[i] != fn.params[i].ownership_insert_autorelease ||
            existing.param_ownership_is_weak_reference[i] != fn.params[i].ownership_is_weak_reference ||
            existing.param_ownership_is_unowned_reference[i] != fn.params[i].ownership_is_unowned_reference ||
            existing.param_ownership_is_unowned_safe_reference[i] !=
                fn.params[i].ownership_is_unowned_safe_reference ||
            existing.param_ownership_arc_diagnostic_candidate[i] !=
                fn.params[i].ownership_arc_diagnostic_candidate ||
            existing.param_ownership_arc_fixit_available[i] !=
                fn.params[i].ownership_arc_fixit_available ||
            existing.param_ownership_arc_diagnostic_profile[i] !=
                fn.params[i].ownership_arc_diagnostic_profile ||
            existing.param_ownership_arc_fixit_hint[i] != fn.params[i].ownership_arc_fixit_hint) {
          compatible = false;
          break;
        }
        if (existing.param_is_vector[i] &&
            (existing.param_vector_base_spelling[i] != fn.params[i].vector_base_spelling ||
             existing.param_vector_lane_count[i] != fn.params[i].vector_lane_count)) {
          compatible = false;
          break;
        }
        if (!AreEquivalentProtocolCompositions(existing.param_has_protocol_composition[i],
                                               existing.param_protocol_composition_lexicographic[i],
                                               param_protocol_composition.has_protocol_composition,
                                               param_protocol_composition.names_lexicographic)) {
          compatible = false;
          break;
        }
      }
    }
    if (!compatible) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S206",
                                     "type mismatch: incompatible function signature for '" + fn.name + "'"));
      continue;
    }
