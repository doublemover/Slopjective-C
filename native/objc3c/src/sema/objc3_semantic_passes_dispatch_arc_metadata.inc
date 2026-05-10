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

static Objc3MessageSendSelectorLoweringSummary BuildMessageSendSelectorLoweringSummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3MessageSendSelectorLoweringSummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.unary_form) {
      ++summary.unary_form_sites;
    }
    if (site.keyword_form) {
      ++summary.keyword_form_sites;
    }
    if (!site.selector_lowering_symbol.empty()) {
      ++summary.selector_lowering_symbol_sites;
    } else {
      ++summary.selector_lowering_missing_symbol_sites;
    }
    summary.selector_lowering_piece_entries += site.selector_piece_count;
    summary.selector_lowering_argument_piece_entries += site.selector_argument_piece_count;
    if (site.selector_lowering_is_normalized) {
      ++summary.selector_lowering_normalized_sites;
    }

    bool contract_violation = false;
    if (site.unary_form == site.keyword_form) {
      ++summary.selector_lowering_form_mismatch_sites;
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.unary_form && site.argument_count != 0) {
      ++summary.selector_lowering_form_mismatch_sites;
      contract_violation = true;
    }
    if (site.keyword_form && site.argument_count == 0) {
      ++summary.selector_lowering_form_mismatch_sites;
      contract_violation = true;
    }
    if (site.selector_argument_piece_count > site.selector_piece_count) {
      ++summary.selector_lowering_arity_mismatch_sites;
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.unary_form && site.selector_argument_piece_count != 0) {
      ++summary.selector_lowering_arity_mismatch_sites;
      contract_violation = true;
    }
    if (site.keyword_form && site.selector_argument_piece_count != site.argument_count) {
      ++summary.selector_lowering_arity_mismatch_sites;
      contract_violation = true;
    }
    if (!site.selector.empty() && !site.selector_lowering_symbol.empty() &&
        site.selector != site.selector_lowering_symbol) {
      ++summary.selector_lowering_symbol_mismatch_sites;
      contract_violation = true;
    }
    if (site.selector_lowering_symbol.empty()) {
      contract_violation = true;
    }
    if (site.selector_lowering_is_normalized && site.selector_lowering_symbol.empty()) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (contract_violation) {
      ++summary.selector_lowering_contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.unary_form_sites + summary.keyword_form_sites == summary.message_send_sites &&
      summary.selector_lowering_symbol_sites <= summary.message_send_sites &&
      summary.selector_lowering_normalized_sites <= summary.selector_lowering_symbol_sites &&
      summary.selector_lowering_argument_piece_entries <= summary.selector_lowering_piece_entries &&
      summary.selector_lowering_form_mismatch_sites <= summary.message_send_sites &&
      summary.selector_lowering_arity_mismatch_sites <= summary.message_send_sites &&
      summary.selector_lowering_symbol_mismatch_sites <= summary.message_send_sites &&
      summary.selector_lowering_missing_symbol_sites <= summary.message_send_sites &&
      summary.selector_lowering_contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3MessageSendSelectorLoweringSummary BuildMessageSendSelectorLoweringSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildMessageSendSelectorLoweringSummaryFromSites(
      surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3MessageSendSelectorLoweringSummary BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildMessageSendSelectorLoweringSummaryFromSites(
      handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3DispatchAbiMarshallingSummary BuildDispatchAbiMarshallingSummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3DispatchAbiMarshallingSummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    ++summary.receiver_slots;
    summary.argument_slots += site.argument_count;
    if (site.unary_form) {
      summary.unary_argument_slots += site.argument_count;
    }
    if (site.keyword_form) {
      summary.keyword_argument_slots += site.argument_count;
    }
    if (!site.selector_lowering_symbol.empty()) {
      ++summary.selector_symbol_slots;
    } else {
      ++summary.missing_selector_symbol_sites;
    }

    bool arity_mismatch = false;
    bool contract_violation = false;
    if (site.unary_form == site.keyword_form) {
      summary.deterministic = false;
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.unary_form && (site.argument_count != 0 || site.selector_argument_piece_count != 0)) {
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.keyword_form && (site.argument_count == 0 || site.selector_argument_piece_count != site.argument_count)) {
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.selector_argument_piece_count > site.argument_count) {
      summary.deterministic = false;
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.selector_lowering_symbol.empty()) {
      contract_violation = true;
    }
    if (arity_mismatch) {
      ++summary.arity_mismatch_sites;
    }
    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.receiver_slots == summary.message_send_sites &&
      summary.selector_symbol_slots + summary.missing_selector_symbol_sites == summary.message_send_sites &&
      summary.keyword_argument_slots + summary.unary_argument_slots == summary.argument_slots &&
      summary.keyword_argument_slots <= summary.argument_slots &&
      summary.unary_argument_slots <= summary.argument_slots &&
      summary.selector_symbol_slots <= summary.message_send_sites &&
      summary.missing_selector_symbol_sites <= summary.message_send_sites &&
      summary.arity_mismatch_sites <= summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3DispatchAbiMarshallingSummary BuildDispatchAbiMarshallingSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildDispatchAbiMarshallingSummaryFromSites(
      surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3DispatchAbiMarshallingSummary BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildDispatchAbiMarshallingSummaryFromSites(
      handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3NilReceiverSemanticsFoldabilitySummary BuildNilReceiverSemanticsFoldabilitySummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3NilReceiverSemanticsFoldabilitySummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.receiver_is_nil_literal) {
      ++summary.receiver_nil_literal_sites;
    }
    if (site.nil_receiver_semantics_enabled) {
      ++summary.nil_receiver_semantics_enabled_sites;
    } else {
      ++summary.non_nil_receiver_sites;
    }
    if (site.nil_receiver_foldable) {
      ++summary.nil_receiver_foldable_sites;
    }
    if (site.nil_receiver_requires_runtime_dispatch) {
      ++summary.nil_receiver_runtime_dispatch_required_sites;
    }

    bool contract_violation = false;
    if (!site.nil_receiver_semantics_is_normalized) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.receiver_is_nil_literal != site.nil_receiver_semantics_enabled) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.nil_receiver_semantics_enabled != site.nil_receiver_foldable) {
      contract_violation = true;
    }
    if (site.nil_receiver_requires_runtime_dispatch == site.nil_receiver_foldable) {
      contract_violation = true;
    }
    if (!site.nil_receiver_semantics_enabled && site.nil_receiver_foldable) {
      contract_violation = true;
    }
    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.receiver_nil_literal_sites == summary.nil_receiver_semantics_enabled_sites &&
      summary.nil_receiver_foldable_sites <= summary.nil_receiver_semantics_enabled_sites &&
      summary.nil_receiver_runtime_dispatch_required_sites + summary.nil_receiver_foldable_sites ==
          summary.message_send_sites &&
      summary.nil_receiver_semantics_enabled_sites + summary.non_nil_receiver_sites == summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3NilReceiverSemanticsFoldabilitySummary
BuildNilReceiverSemanticsFoldabilitySummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildNilReceiverSemanticsFoldabilitySummaryFromSites(
      surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3NilReceiverSemanticsFoldabilitySummary
BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildNilReceiverSemanticsFoldabilitySummaryFromSites(
      handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3SuperDispatchMethodFamilySummary BuildSuperDispatchMethodFamilySummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3SuperDispatchMethodFamilySummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.receiver_is_super_identifier) {
      ++summary.receiver_super_identifier_sites;
    }
    if (site.super_dispatch_enabled) {
      ++summary.super_dispatch_enabled_sites;
    }
    if (site.super_dispatch_requires_class_context) {
      ++summary.super_dispatch_requires_class_context_sites;
    }
    if (site.method_family_name == "init") {
      ++summary.method_family_init_sites;
    } else if (site.method_family_name == "copy") {
      ++summary.method_family_copy_sites;
    } else if (site.method_family_name == "mutableCopy") {
      ++summary.method_family_mutable_copy_sites;
    } else if (site.method_family_name == "new") {
      ++summary.method_family_new_sites;
    } else {
      ++summary.method_family_none_sites;
    }
    if (site.method_family_returns_retained_result) {
      ++summary.method_family_returns_retained_result_sites;
    }
    if (site.method_family_returns_related_result) {
      ++summary.method_family_returns_related_result_sites;
    }

    bool contract_violation = false;
    if (!site.super_dispatch_semantics_is_normalized || !site.method_family_semantics_is_normalized) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.receiver_is_super_identifier != site.super_dispatch_enabled) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.super_dispatch_enabled != site.super_dispatch_requires_class_context) {
      contract_violation = true;
    }
    if (site.method_family_returns_related_result && site.method_family_name != "init") {
      contract_violation = true;
    }
    const bool retained_family =
        site.method_family_name == "init" || site.method_family_name == "copy" ||
        site.method_family_name == "mutableCopy" || site.method_family_name == "new";
    if (site.method_family_returns_retained_result != retained_family) {
      contract_violation = true;
    }
    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.receiver_super_identifier_sites == summary.super_dispatch_enabled_sites &&
      summary.super_dispatch_requires_class_context_sites == summary.super_dispatch_enabled_sites &&
      summary.method_family_init_sites + summary.method_family_copy_sites + summary.method_family_mutable_copy_sites +
              summary.method_family_new_sites + summary.method_family_none_sites ==
          summary.message_send_sites &&
      summary.method_family_returns_related_result_sites <= summary.method_family_init_sites &&
      summary.method_family_returns_retained_result_sites <= summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3SuperDispatchMethodFamilySummary
BuildSuperDispatchMethodFamilySummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildSuperDispatchMethodFamilySummaryFromSites(surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3SuperDispatchMethodFamilySummary
BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildSuperDispatchMethodFamilySummaryFromSites(handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3RuntimeLinkHostLinkSummary BuildRuntimeLinkHostLinkSummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3RuntimeLinkHostLinkSummary summary;
  bool baseline_initialized = false;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.runtime_link_host_link_required) {
      ++summary.runtime_link_required_sites;
    }
    if (site.runtime_link_host_link_elided) {
      ++summary.runtime_link_elided_sites;
    }

    bool contract_violation = false;
    if (!site.runtime_link_host_link_is_normalized) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.runtime_link_host_link_required == site.runtime_link_host_link_elided) {
      contract_violation = true;
    }
    if (site.runtime_link_host_link_required != site.nil_receiver_requires_runtime_dispatch) {
      contract_violation = true;
    }
    if (site.runtime_link_host_link_declaration_parameter_count !=
        site.runtime_link_host_link_runtime_dispatch_arg_slots + 2u) {
      contract_violation = true;
    }
    if (site.runtime_dispatch_bridge_symbol.empty()) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.runtime_link_host_link_symbol.empty()) {
      contract_violation = true;
    }

    const std::string site_dispatch_symbol = site.runtime_dispatch_bridge_symbol.empty()
                                                 ? std::string(kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol)
                                                 : site.runtime_dispatch_bridge_symbol;
    if (!baseline_initialized) {
      baseline_initialized = true;
      summary.runtime_dispatch_arg_slots = site.runtime_link_host_link_runtime_dispatch_arg_slots;
      summary.runtime_dispatch_declaration_parameter_count =
          site.runtime_link_host_link_declaration_parameter_count;
      summary.runtime_dispatch_symbol = site_dispatch_symbol;
      summary.default_runtime_dispatch_symbol_binding =
          summary.runtime_dispatch_symbol == kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol;
    } else if (summary.runtime_dispatch_arg_slots != site.runtime_link_host_link_runtime_dispatch_arg_slots ||
               summary.runtime_dispatch_declaration_parameter_count !=
                   site.runtime_link_host_link_declaration_parameter_count ||
               summary.runtime_dispatch_symbol != site_dispatch_symbol) {
      contract_violation = true;
    }

    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  if (!baseline_initialized) {
    summary.runtime_dispatch_symbol = kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol;
    summary.default_runtime_dispatch_symbol_binding = true;
  } else {
    summary.default_runtime_dispatch_symbol_binding =
        summary.runtime_dispatch_symbol == kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol;
  }

  summary.deterministic =
      summary.deterministic &&
      summary.runtime_link_required_sites + summary.runtime_link_elided_sites == summary.message_send_sites &&
      summary.runtime_link_required_sites <= summary.message_send_sites &&
      summary.runtime_link_elided_sites <= summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites &&
      (summary.message_send_sites == 0 ||
       summary.runtime_dispatch_declaration_parameter_count == summary.runtime_dispatch_arg_slots + 2u) &&
      (summary.default_runtime_dispatch_symbol_binding ==
       (summary.runtime_dispatch_symbol == kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol));
  return summary;
}

static Objc3RuntimeLinkHostLinkSummary
BuildRuntimeLinkHostLinkSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildRuntimeLinkHostLinkSummaryFromSites(surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3RuntimeLinkHostLinkSummary
BuildRuntimeLinkHostLinkSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildRuntimeLinkHostLinkSummaryFromSites(handoff.message_send_selector_lowering_sites_lexicographic);
}

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
