static Objc3MethodLookupOverrideConflictSummary BuildMethodLookupOverrideConflictSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3MethodLookupOverrideConflictSummary summary;
  std::unordered_set<std::string> unresolved_units;

  for (const auto &implementation_entry : surface.implementations) {
    const std::string &implementation_name = implementation_entry.first;
    const Objc3ImplementationInfo &implementation_info = implementation_entry.second;
    summary.method_lookup_sites += implementation_info.methods.size();

    const auto interface_it = surface.interfaces.find(implementation_name);
    if (interface_it == surface.interfaces.end()) {
      unresolved_units.insert("impl:" + implementation_name);
      summary.method_lookup_misses += implementation_info.methods.size();
      continue;
    }

    for (const auto &method_entry : implementation_info.methods) {
      const auto interface_method_it = interface_it->second.methods.find(method_entry.first);
      if (interface_method_it == interface_it->second.methods.end()) {
        ++summary.method_lookup_misses;
      } else {
        ++summary.method_lookup_hits;
      }
    }
  }

  for (const auto &interface_entry : surface.interfaces) {
    const std::string &interface_name = interface_entry.first;
    const Objc3InterfaceInfo &interface_info = interface_entry.second;
    if (interface_info.super_name.empty()) {
      continue;
    }
    for (const auto &method_entry : interface_info.methods) {
      ++summary.override_lookup_sites;
      // anchor: this superclass walk and signature check remain the
      // canonical override-legality source that the executable metadata graph
      // replays with `method-to-overridden-method` owner edges.
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3MethodInfo *base_method =
          FindSurfaceMethodInSuperChain(surface, interface_name,
                                       method_entry.first,
                                       method_entry.second.is_class_method,
                                       missing_base, cycle_detected);
      if (cycle_detected) {
        summary.deterministic = false;
      }
      if (missing_base) {
        unresolved_units.insert("iface:" + interface_name);
        ++summary.override_lookup_misses;
        continue;
      }
      if (base_method == nullptr) {
        ++summary.override_lookup_misses;
        continue;
      }
      ++summary.override_lookup_hits;
      if (!IsCompatibleMethodSignature(*base_method, method_entry.second)) {
        ++summary.override_conflicts;
      }
    }
  }

  summary.unresolved_base_interfaces = unresolved_units.size();
  summary.deterministic =
      summary.deterministic &&
      summary.method_lookup_hits <= summary.method_lookup_sites &&
      summary.method_lookup_hits + summary.method_lookup_misses == summary.method_lookup_sites &&
      summary.override_lookup_hits <= summary.override_lookup_sites &&
      summary.override_lookup_hits + summary.override_lookup_misses == summary.override_lookup_sites &&
      summary.override_conflicts <= summary.override_lookup_hits;
  return summary;
}

static bool IsCompatibleMethodTypeMetadataSignature(const Objc3SemanticMethodTypeMetadata &lhs,
                                                    const Objc3SemanticMethodTypeMetadata &rhs) {
  if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type ||
      lhs.return_is_vector != rhs.return_is_vector ||
      lhs.is_class_method != rhs.is_class_method) {
    return false;
  }
  if (lhs.return_is_vector &&
      (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
       lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
    return false;
  }
  if (lhs.param_types.size() != rhs.param_types.size() ||
      lhs.param_is_vector.size() != rhs.param_is_vector.size() ||
      lhs.param_vector_base_spelling.size() != rhs.param_vector_base_spelling.size() ||
      lhs.param_vector_lane_count.size() != rhs.param_vector_lane_count.size()) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.param_types.size(); ++i) {
    if (lhs.param_types[i] != rhs.param_types[i] ||
        lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
      return false;
    }
    if (lhs.param_is_vector[i] &&
        (lhs.param_vector_base_spelling[i] != rhs.param_vector_base_spelling[i] ||
         lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
      return false;
    }
  }
  return true;
}

static const Objc3SemanticMethodTypeMetadata *FindMethodInInterfaceMetadata(
    const Objc3SemanticInterfaceTypeMetadata &metadata,
    const std::string &selector) {
  for (const auto &method_metadata : metadata.methods_lexicographic) {
    if (method_metadata.selector == selector) {
      return &method_metadata;
    }
  }
  return nullptr;
}

static const Objc3SemanticMethodTypeMetadata *FindHandoffMethodInSuperChain(
    const std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> &interfaces_by_name,
    const Objc3SemanticInterfaceTypeMetadata &metadata,
    const std::string &selector,
    bool &missing_base,
    bool &cycle_detected) {
  missing_base = false;
  cycle_detected = false;
  std::string next_super = metadata.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      cycle_detected = true;
      return nullptr;
    }
    const auto super_it = interfaces_by_name.find(next_super);
    if (super_it == interfaces_by_name.end()) {
      missing_base = true;
      return nullptr;
    }
    const Objc3SemanticMethodTypeMetadata *method =
        FindMethodInInterfaceMetadata(*super_it->second, selector);
    if (method != nullptr) {
      return method;
    }
    next_super = super_it->second->super_name;
  }
  return nullptr;
}

static Objc3MethodLookupOverrideConflictSummary BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3MethodLookupOverrideConflictSummary summary;
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name.emplace(metadata.name, &metadata);
  }
  std::unordered_set<std::string> unresolved_units;

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    summary.method_lookup_sites += implementation_metadata.methods_lexicographic.size();
    const auto interface_it = interfaces_by_name.find(implementation_metadata.name);
    if (interface_it == interfaces_by_name.end()) {
      unresolved_units.insert("impl:" + implementation_metadata.name);
      summary.method_lookup_misses += implementation_metadata.methods_lexicographic.size();
      continue;
    }
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      const Objc3SemanticMethodTypeMetadata *interface_method =
          FindMethodInInterfaceMetadata(*interface_it->second, method_metadata.selector);
      if (interface_method == nullptr) {
        ++summary.method_lookup_misses;
      } else {
        ++summary.method_lookup_hits;
      }
    }
  }

  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    if (interface_metadata.super_name.empty()) {
      continue;
    }
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      ++summary.override_lookup_sites;
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3SemanticMethodTypeMetadata *base_method =
          FindHandoffMethodInSuperChain(interfaces_by_name,
                                        interface_metadata,
                                        method_metadata.selector,
                                        missing_base,
                                        cycle_detected);
      if (cycle_detected) {
        summary.deterministic = false;
      }
      if (missing_base) {
        unresolved_units.insert("iface:" + interface_metadata.name);
        ++summary.override_lookup_misses;
        continue;
      }
      if (base_method == nullptr) {
        ++summary.override_lookup_misses;
        continue;
      }
      ++summary.override_lookup_hits;
      if (!IsCompatibleMethodTypeMetadataSignature(*base_method, method_metadata)) {
        ++summary.override_conflicts;
      }
    }
  }

  summary.unresolved_base_interfaces = unresolved_units.size();
  summary.deterministic =
      summary.deterministic &&
      summary.method_lookup_hits <= summary.method_lookup_sites &&
      summary.method_lookup_hits + summary.method_lookup_misses == summary.method_lookup_sites &&
      summary.override_lookup_hits <= summary.override_lookup_sites &&
      summary.override_lookup_hits + summary.override_lookup_misses == summary.override_lookup_sites &&
      summary.override_conflicts <= summary.override_lookup_hits;
  return summary;
}
