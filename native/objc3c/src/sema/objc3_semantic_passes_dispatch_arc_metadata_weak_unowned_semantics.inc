static Objc3WeakUnownedSemanticsSummary
BuildWeakUnownedSemanticsSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3WeakUnownedSemanticsSummary summary;

  const auto accumulate_site = [&summary](bool ownership_candidate,
                                          bool weak_reference,
                                          bool unowned_reference,
                                          bool unowned_safe_reference,
                                          bool conflict) {
    if (ownership_candidate) {
      ++summary.ownership_candidate_sites;
    }
    if (weak_reference) {
      ++summary.weak_reference_sites;
    }
    if (unowned_reference) {
      ++summary.unowned_reference_sites;
    }
    if (unowned_safe_reference) {
      ++summary.unowned_safe_reference_sites;
    }
    if (conflict) {
      ++summary.weak_unowned_conflict_sites;
    }
    if (conflict || (unowned_safe_reference && !unowned_reference) ||
        (!ownership_candidate && (weak_reference || unowned_reference))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const FunctionInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_is_weak_reference.size() != arity ||
        info.param_ownership_is_unowned_reference.size() != arity ||
        info.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = info.param_ownership_is_weak_reference[i];
      const bool unowned_reference = info.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = info.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate = info.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = info.return_ownership_is_weak_reference;
    const bool return_unowned_reference = info.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = info.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        info.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3MethodInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_is_weak_reference.size() != arity ||
        info.param_ownership_is_unowned_reference.size() != arity ||
        info.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = info.param_ownership_is_weak_reference[i];
      const bool unowned_reference = info.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = info.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate = info.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = info.return_ownership_is_weak_reference;
    const bool return_unowned_reference = info.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = info.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        info.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3PropertyInfo &info) {
    const bool weak_reference = info.ownership_is_weak_reference || info.is_weak;
    const bool unowned_reference = info.ownership_is_unowned_reference || info.is_unowned || info.is_assign;
    const bool unowned_safe_reference = info.ownership_is_unowned_safe_reference || info.is_unowned;
    const bool ownership_candidate = info.has_ownership_qualifier || info.is_weak || info.is_unowned || info.is_assign ||
                                     weak_reference || unowned_reference;
    const bool conflict = info.has_weak_unowned_conflict || (info.is_weak && info.is_unowned) ||
                          (weak_reference && unowned_reference);
    accumulate_site(
        ownership_candidate, weak_reference, unowned_reference, unowned_safe_reference, conflict);
  };

  for (const auto &entry : surface.functions) {
    accumulate_function(entry.second);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.unowned_safe_reference_sites <= summary.unowned_reference_sites &&
      summary.weak_unowned_conflict_sites <= summary.ownership_candidate_sites &&
      summary.contract_violation_sites <= summary.ownership_candidate_sites + summary.weak_unowned_conflict_sites;
  return summary;
}

static Objc3WeakUnownedSemanticsSummary
BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3WeakUnownedSemanticsSummary summary;

  const auto accumulate_site = [&summary](bool ownership_candidate,
                                          bool weak_reference,
                                          bool unowned_reference,
                                          bool unowned_safe_reference,
                                          bool conflict) {
    if (ownership_candidate) {
      ++summary.ownership_candidate_sites;
    }
    if (weak_reference) {
      ++summary.weak_reference_sites;
    }
    if (unowned_reference) {
      ++summary.unowned_reference_sites;
    }
    if (unowned_safe_reference) {
      ++summary.unowned_safe_reference_sites;
    }
    if (conflict) {
      ++summary.weak_unowned_conflict_sites;
    }
    if (conflict || (unowned_safe_reference && !unowned_reference) ||
        (!ownership_candidate && (weak_reference || unowned_reference))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const Objc3SemanticFunctionTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_is_weak_reference.size() != arity ||
        metadata.param_ownership_is_unowned_reference.size() != arity ||
        metadata.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = metadata.param_ownership_is_weak_reference[i];
      const bool unowned_reference = metadata.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = metadata.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate =
          metadata.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = metadata.return_ownership_is_weak_reference;
    const bool return_unowned_reference = metadata.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = metadata.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        metadata.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3SemanticMethodTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_is_weak_reference.size() != arity ||
        metadata.param_ownership_is_unowned_reference.size() != arity ||
        metadata.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = metadata.param_ownership_is_weak_reference[i];
      const bool unowned_reference = metadata.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = metadata.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate =
          metadata.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = metadata.return_ownership_is_weak_reference;
    const bool return_unowned_reference = metadata.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = metadata.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        metadata.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3SemanticPropertyTypeMetadata &metadata) {
    const bool weak_reference = metadata.ownership_is_weak_reference || metadata.is_weak;
    const bool unowned_reference = metadata.ownership_is_unowned_reference || metadata.is_unowned || metadata.is_assign;
    const bool unowned_safe_reference = metadata.ownership_is_unowned_safe_reference || metadata.is_unowned;
    const bool ownership_candidate = metadata.has_ownership_qualifier || metadata.is_weak || metadata.is_unowned ||
                                     metadata.is_assign || weak_reference || unowned_reference;
    const bool conflict = metadata.has_weak_unowned_conflict || (metadata.is_weak && metadata.is_unowned) ||
                          (weak_reference && unowned_reference);
    accumulate_site(
        ownership_candidate, weak_reference, unowned_reference, unowned_safe_reference, conflict);
  };

  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function(metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.unowned_safe_reference_sites <= summary.unowned_reference_sites &&
      summary.weak_unowned_conflict_sites <= summary.ownership_candidate_sites &&
      summary.contract_violation_sites <= summary.ownership_candidate_sites + summary.weak_unowned_conflict_sites;
  return summary;
}
