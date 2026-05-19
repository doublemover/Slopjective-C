static Objc3AutoreleasePoolScopeSummary BuildAutoreleasePoolScopeSummaryFromSites(
    const std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  Objc3AutoreleasePoolScopeSummary summary;
  for (const auto &site : sites) {
    ++summary.scope_sites;
    if (site.scope_symbol.empty()) {
      ++summary.contract_violation_sites;
    } else {
      ++summary.scope_symbolized_sites;
      if (site.scope_symbol.rfind("autoreleasepool#", 0) != 0) {
        ++summary.contract_violation_sites;
      }
    }
    if (site.scope_depth == 0u) {
      ++summary.contract_violation_sites;
    } else if (site.scope_depth > summary.max_scope_depth) {
      summary.max_scope_depth = site.scope_depth;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.scope_symbolized_sites <= summary.scope_sites &&
      summary.contract_violation_sites <= summary.scope_sites &&
      (summary.scope_sites > 0u || summary.max_scope_depth == 0u) &&
      summary.max_scope_depth <= static_cast<unsigned>(summary.scope_sites);
  return summary;
}

#include "sema/objc3_semantic_passes_dispatch_arc_metadata_message_dispatch_lowering.inc"

#include "sema/objc3_semantic_passes_dispatch_arc_metadata_retain_release_operations.inc"

#include "sema/objc3_semantic_passes_dispatch_arc_metadata_weak_unowned_semantics.inc"

static Objc3ArcDiagnosticsFixitSummary
BuildArcDiagnosticsFixitSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3ArcDiagnosticsFixitSummary summary;

  const auto accumulate_site = [&summary](bool diagnostic_candidate,
                                          bool fixit_available,
                                          const std::string &diagnostic_profile,
                                          const std::string &fixit_hint,
                                          bool weak_unowned_conflict) {
    if (diagnostic_candidate) {
      ++summary.ownership_arc_diagnostic_candidate_sites;
    }
    if (fixit_available) {
      ++summary.ownership_arc_fixit_available_sites;
    }
    if (!diagnostic_profile.empty()) {
      ++summary.ownership_arc_profiled_sites;
    }
    if (diagnostic_profile == "arc-weak-unowned-conflict") {
      ++summary.ownership_arc_weak_unowned_conflict_diagnostic_sites;
    }
    if (fixit_available && fixit_hint.empty()) {
      ++summary.ownership_arc_empty_fixit_hint_sites;
    }
    if ((fixit_available && !diagnostic_candidate) ||
        (!diagnostic_profile.empty() && !diagnostic_candidate) ||
        (!fixit_hint.empty() && !fixit_available) ||
        (fixit_available && fixit_hint.empty()) ||
        (weak_unowned_conflict && diagnostic_profile != "arc-weak-unowned-conflict") ||
        (!weak_unowned_conflict && diagnostic_profile == "arc-weak-unowned-conflict")) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const FunctionInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_ownership_arc_diagnostic_candidate.size() != arity ||
        info.param_ownership_arc_fixit_available.size() != arity ||
        info.param_ownership_arc_diagnostic_profile.size() != arity ||
        info.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(info.param_ownership_arc_diagnostic_candidate[i],
                      info.param_ownership_arc_fixit_available[i],
                      info.param_ownership_arc_diagnostic_profile[i],
                      info.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(info.return_ownership_arc_diagnostic_candidate,
                    info.return_ownership_arc_fixit_available,
                    info.return_ownership_arc_diagnostic_profile,
                    info.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3MethodInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_ownership_arc_diagnostic_candidate.size() != arity ||
        info.param_ownership_arc_fixit_available.size() != arity ||
        info.param_ownership_arc_diagnostic_profile.size() != arity ||
        info.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(info.param_ownership_arc_diagnostic_candidate[i],
                      info.param_ownership_arc_fixit_available[i],
                      info.param_ownership_arc_diagnostic_profile[i],
                      info.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(info.return_ownership_arc_diagnostic_candidate,
                    info.return_ownership_arc_fixit_available,
                    info.return_ownership_arc_diagnostic_profile,
                    info.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3PropertyInfo &info) {
    const bool weak_unowned_conflict = info.has_weak_unowned_conflict || (info.is_weak && info.is_unowned);
    accumulate_site(info.ownership_arc_diagnostic_candidate,
                    info.ownership_arc_fixit_available,
                    info.ownership_arc_diagnostic_profile,
                    info.ownership_arc_fixit_hint,
                    weak_unowned_conflict);
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
      summary.ownership_arc_fixit_available_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_profiled_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites + summary.contract_violation_sites;
  return summary;
}

static Objc3ArcDiagnosticsFixitSummary
BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3ArcDiagnosticsFixitSummary summary;

  const auto accumulate_site = [&summary](bool diagnostic_candidate,
                                          bool fixit_available,
                                          const std::string &diagnostic_profile,
                                          const std::string &fixit_hint,
                                          bool weak_unowned_conflict) {
    if (diagnostic_candidate) {
      ++summary.ownership_arc_diagnostic_candidate_sites;
    }
    if (fixit_available) {
      ++summary.ownership_arc_fixit_available_sites;
    }
    if (!diagnostic_profile.empty()) {
      ++summary.ownership_arc_profiled_sites;
    }
    if (diagnostic_profile == "arc-weak-unowned-conflict") {
      ++summary.ownership_arc_weak_unowned_conflict_diagnostic_sites;
    }
    if (fixit_available && fixit_hint.empty()) {
      ++summary.ownership_arc_empty_fixit_hint_sites;
    }
    if ((fixit_available && !diagnostic_candidate) ||
        (!diagnostic_profile.empty() && !diagnostic_candidate) ||
        (!fixit_hint.empty() && !fixit_available) ||
        (fixit_available && fixit_hint.empty()) ||
        (weak_unowned_conflict && diagnostic_profile != "arc-weak-unowned-conflict") ||
        (!weak_unowned_conflict && diagnostic_profile == "arc-weak-unowned-conflict")) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const Objc3SemanticFunctionTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_ownership_arc_diagnostic_candidate.size() != arity ||
        metadata.param_ownership_arc_fixit_available.size() != arity ||
        metadata.param_ownership_arc_diagnostic_profile.size() != arity ||
        metadata.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(metadata.param_ownership_arc_diagnostic_candidate[i],
                      metadata.param_ownership_arc_fixit_available[i],
                      metadata.param_ownership_arc_diagnostic_profile[i],
                      metadata.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(metadata.return_ownership_arc_diagnostic_candidate,
                    metadata.return_ownership_arc_fixit_available,
                    metadata.return_ownership_arc_diagnostic_profile,
                    metadata.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3SemanticMethodTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_ownership_arc_diagnostic_candidate.size() != arity ||
        metadata.param_ownership_arc_fixit_available.size() != arity ||
        metadata.param_ownership_arc_diagnostic_profile.size() != arity ||
        metadata.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(metadata.param_ownership_arc_diagnostic_candidate[i],
                      metadata.param_ownership_arc_fixit_available[i],
                      metadata.param_ownership_arc_diagnostic_profile[i],
                      metadata.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(metadata.return_ownership_arc_diagnostic_candidate,
                    metadata.return_ownership_arc_fixit_available,
                    metadata.return_ownership_arc_diagnostic_profile,
                    metadata.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3SemanticPropertyTypeMetadata &metadata) {
    const bool weak_unowned_conflict = metadata.has_weak_unowned_conflict || (metadata.is_weak && metadata.is_unowned);
    accumulate_site(metadata.ownership_arc_diagnostic_candidate,
                    metadata.ownership_arc_fixit_available,
                    metadata.ownership_arc_diagnostic_profile,
                    metadata.ownership_arc_fixit_hint,
                    weak_unowned_conflict);
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
      summary.ownership_arc_fixit_available_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_profiled_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites + summary.contract_violation_sites;
  return summary;
}

static Objc3AutoreleasePoolScopeSummary
BuildAutoreleasePoolScopeSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildAutoreleasePoolScopeSummaryFromSites(surface.autoreleasepool_scope_sites_lexicographic);
}

static Objc3AutoreleasePoolScopeSummary
BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildAutoreleasePoolScopeSummaryFromSites(handoff.autoreleasepool_scope_sites_lexicographic);
}

#include "sema/objc3_semantic_passes_id_class_sel_object_pointer_type_checking.inc"
