  handoff.interface_implementation_summary = surface.interface_implementation_summary;
  handoff.interface_implementation_summary.resolved_interfaces = handoff.interfaces_lexicographic.size();
  handoff.interface_implementation_summary.resolved_implementations = handoff.implementations_lexicographic.size();
  handoff.interface_implementation_summary.interface_method_symbols = 0;
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    handoff.interface_implementation_summary.interface_method_symbols += metadata.methods_lexicographic.size();
  }
  handoff.interface_implementation_summary.implementation_method_symbols = 0;
  for (const auto &metadata : handoff.implementations_lexicographic) {
    handoff.interface_implementation_summary.implementation_method_symbols += metadata.methods_lexicographic.size();
  }
  handoff.interface_implementation_summary.linked_implementation_symbols = 0;
  const auto are_compatible_method_metadata = [](const Objc3SemanticMethodTypeMetadata &lhs,
                                                 const Objc3SemanticMethodTypeMetadata &rhs) {
    if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type || lhs.return_is_vector != rhs.return_is_vector ||
        lhs.is_class_method != rhs.is_class_method ||
        lhs.return_ownership_insert_retain != rhs.return_ownership_insert_retain ||
        lhs.return_ownership_insert_release != rhs.return_ownership_insert_release ||
        lhs.return_ownership_insert_autorelease != rhs.return_ownership_insert_autorelease ||
        lhs.return_ownership_is_weak_reference != rhs.return_ownership_is_weak_reference ||
        lhs.return_ownership_is_unowned_reference != rhs.return_ownership_is_unowned_reference ||
        lhs.return_ownership_is_unowned_safe_reference != rhs.return_ownership_is_unowned_safe_reference ||
        lhs.return_ownership_arc_diagnostic_candidate != rhs.return_ownership_arc_diagnostic_candidate ||
        lhs.return_ownership_arc_fixit_available != rhs.return_ownership_arc_fixit_available ||
        lhs.return_ownership_arc_diagnostic_profile != rhs.return_ownership_arc_diagnostic_profile ||
        lhs.return_ownership_arc_fixit_hint != rhs.return_ownership_arc_fixit_hint) {
      return false;
    }
    if (lhs.return_is_vector &&
        (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
         lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
      return false;
    }
    if (!AreEquivalentProtocolCompositions(lhs.return_has_protocol_composition,
                                           lhs.return_protocol_composition_lexicographic,
                                           rhs.return_has_protocol_composition,
                                           rhs.return_protocol_composition_lexicographic)) {
      return false;
    }
    for (std::size_t i = 0; i < lhs.arity; ++i) {
      if (i >= lhs.param_types.size() || i >= lhs.param_is_vector.size() || i >= lhs.param_vector_base_spelling.size() ||
          i >= lhs.param_vector_lane_count.size() || i >= lhs.param_has_protocol_composition.size() ||
          i >= lhs.param_protocol_composition_lexicographic.size() || i >= lhs.param_ownership_insert_retain.size() ||
          i >= lhs.param_ownership_insert_release.size() || i >= lhs.param_ownership_insert_autorelease.size() ||
          i >= lhs.param_ownership_is_weak_reference.size() ||
          i >= lhs.param_ownership_is_unowned_reference.size() ||
          i >= lhs.param_ownership_is_unowned_safe_reference.size() ||
          i >= lhs.param_ownership_arc_diagnostic_candidate.size() ||
          i >= lhs.param_ownership_arc_fixit_available.size() ||
          i >= lhs.param_ownership_arc_diagnostic_profile.size() ||
          i >= lhs.param_ownership_arc_fixit_hint.size() ||
          i >= rhs.param_types.size() ||
          i >= rhs.param_is_vector.size() || i >= rhs.param_vector_base_spelling.size() ||
          i >= rhs.param_vector_lane_count.size() || i >= rhs.param_has_protocol_composition.size() ||
          i >= rhs.param_protocol_composition_lexicographic.size() || i >= rhs.param_ownership_insert_retain.size() ||
          i >= rhs.param_ownership_insert_release.size() || i >= rhs.param_ownership_insert_autorelease.size() ||
          i >= rhs.param_ownership_is_weak_reference.size() ||
          i >= rhs.param_ownership_is_unowned_reference.size() ||
          i >= rhs.param_ownership_is_unowned_safe_reference.size() ||
          i >= rhs.param_ownership_arc_diagnostic_candidate.size() ||
          i >= rhs.param_ownership_arc_fixit_available.size() ||
          i >= rhs.param_ownership_arc_diagnostic_profile.size() ||
          i >= rhs.param_ownership_arc_fixit_hint.size()) {
        return false;
      }
      if (lhs.param_types[i] != rhs.param_types[i] || lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
        return false;
      }
      if (lhs.param_ownership_insert_retain[i] != rhs.param_ownership_insert_retain[i] ||
          lhs.param_ownership_insert_release[i] != rhs.param_ownership_insert_release[i] ||
          lhs.param_ownership_insert_autorelease[i] != rhs.param_ownership_insert_autorelease[i] ||
          lhs.param_ownership_is_weak_reference[i] != rhs.param_ownership_is_weak_reference[i] ||
          lhs.param_ownership_is_unowned_reference[i] != rhs.param_ownership_is_unowned_reference[i] ||
          lhs.param_ownership_is_unowned_safe_reference[i] != rhs.param_ownership_is_unowned_safe_reference[i] ||
          lhs.param_ownership_arc_diagnostic_candidate[i] != rhs.param_ownership_arc_diagnostic_candidate[i] ||
          lhs.param_ownership_arc_fixit_available[i] != rhs.param_ownership_arc_fixit_available[i] ||
          lhs.param_ownership_arc_diagnostic_profile[i] != rhs.param_ownership_arc_diagnostic_profile[i] ||
          lhs.param_ownership_arc_fixit_hint[i] != rhs.param_ownership_arc_fixit_hint[i]) {
        return false;
      }
      if (lhs.param_is_vector[i] &&
          (lhs.param_vector_base_spelling[i] != rhs.param_vector_base_spelling[i] ||
           lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
        return false;
      }
      if (!AreEquivalentProtocolCompositions(lhs.param_has_protocol_composition[i],
                                             lhs.param_protocol_composition_lexicographic[i],
                                             rhs.param_has_protocol_composition[i],
                                             rhs.param_protocol_composition_lexicographic[i])) {
        return false;
      }
    }
    return true;
  };
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name[metadata.name] = &metadata;
  }
  for (const auto &implementation : handoff.implementations_lexicographic) {
    if (!implementation.has_matching_interface) {
      continue;
    }
    const auto interface_it = interfaces_by_name.find(implementation.name);
    if (interface_it == interfaces_by_name.end()) {
      continue;
    }
    const Objc3SemanticInterfaceTypeMetadata &interface_metadata = *interface_it->second;
    for (const auto &implementation_method : implementation.methods_lexicographic) {
      const auto interface_method_it = std::find_if(
          interface_metadata.methods_lexicographic.begin(),
          interface_metadata.methods_lexicographic.end(),
          [&implementation_method](const Objc3SemanticMethodTypeMetadata &candidate) {
            return candidate.selector == implementation_method.selector;
          });
      if (interface_method_it == interface_metadata.methods_lexicographic.end()) {
        continue;
      }
      if (are_compatible_method_metadata(*interface_method_it, implementation_method)) {
        ++handoff.interface_implementation_summary.linked_implementation_symbols;
      }
    }
  }
  handoff.interface_implementation_summary.deterministic =
      handoff.interface_implementation_summary.deterministic &&
      handoff.interface_implementation_summary.linked_implementation_symbols <=
          handoff.interface_implementation_summary.implementation_method_symbols &&
      handoff.interface_implementation_summary.linked_implementation_symbols <=
          handoff.interface_implementation_summary.interface_method_symbols;
