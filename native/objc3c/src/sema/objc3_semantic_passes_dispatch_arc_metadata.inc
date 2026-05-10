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

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3RetainReleaseOperationSummary summary;
  std::size_t operation_sites = 0;

  const auto accumulate_function = [&summary, &operation_sites](const FunctionInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_insert_retain.size() != arity ||
        info.param_ownership_insert_release.size() != arity ||
        info.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = info.param_has_ownership_qualifier[i];
      const bool insert_retain = info.param_ownership_insert_retain[i];
      const bool insert_release = info.param_ownership_insert_release[i];
      const bool insert_autorelease = info.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = info.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (info.return_ownership_insert_retain || info.return_ownership_insert_release ||
                               info.return_ownership_insert_autorelease)) ||
        (info.return_ownership_insert_autorelease &&
         (info.return_ownership_insert_retain || info.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_method = [&summary, &operation_sites](const Objc3MethodInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_insert_retain.size() != arity ||
        info.param_ownership_insert_release.size() != arity ||
        info.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = info.param_has_ownership_qualifier[i];
      const bool insert_retain = info.param_ownership_insert_retain[i];
      const bool insert_release = info.param_ownership_insert_release[i];
      const bool insert_autorelease = info.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = info.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (info.return_ownership_insert_retain || info.return_ownership_insert_release ||
                               info.return_ownership_insert_autorelease)) ||
        (info.return_ownership_insert_autorelease &&
         (info.return_ownership_insert_retain || info.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_property = [&summary, &operation_sites](const Objc3PropertyInfo &info) {
    ++operation_sites;
    const bool qualified = info.has_ownership_qualifier;
    if (qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!qualified &&
         (info.ownership_insert_retain || info.ownership_insert_release || info.ownership_insert_autorelease)) ||
        (info.ownership_insert_autorelease && (info.ownership_insert_retain || info.ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
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

  const std::size_t qualified_or_violation = summary.ownership_qualified_sites + summary.contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      summary.contract_violation_sites <= operation_sites &&
      summary.retain_insertion_sites <= qualified_or_violation &&
      summary.release_insertion_sites <= qualified_or_violation &&
      summary.autorelease_insertion_sites <= qualified_or_violation;
  return summary;
}

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3RetainReleaseOperationSummary summary;
  std::size_t operation_sites = 0;

  const auto accumulate_function = [&summary, &operation_sites](const Objc3SemanticFunctionTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_insert_retain.size() != arity ||
        metadata.param_ownership_insert_release.size() != arity ||
        metadata.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = metadata.param_has_ownership_qualifier[i];
      const bool insert_retain = metadata.param_ownership_insert_retain[i];
      const bool insert_release = metadata.param_ownership_insert_release[i];
      const bool insert_autorelease = metadata.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = metadata.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                               metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_method = [&summary, &operation_sites](const Objc3SemanticMethodTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_insert_retain.size() != arity ||
        metadata.param_ownership_insert_release.size() != arity ||
        metadata.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = metadata.param_has_ownership_qualifier[i];
      const bool insert_retain = metadata.param_ownership_insert_retain[i];
      const bool insert_release = metadata.param_ownership_insert_release[i];
      const bool insert_autorelease = metadata.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = metadata.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                               metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_property = [&summary, &operation_sites](const Objc3SemanticPropertyTypeMetadata &metadata) {
    ++operation_sites;
    const bool qualified = metadata.has_ownership_qualifier;
    if (qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!qualified && (metadata.ownership_insert_retain || metadata.ownership_insert_release ||
                        metadata.ownership_insert_autorelease)) ||
        (metadata.ownership_insert_autorelease &&
         (metadata.ownership_insert_retain || metadata.ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
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

  const std::size_t qualified_or_violation = summary.ownership_qualified_sites + summary.contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      summary.contract_violation_sites <= operation_sites &&
      summary.retain_insertion_sites <= qualified_or_violation &&
      summary.release_insertion_sites <= qualified_or_violation &&
      summary.autorelease_insertion_sites <= qualified_or_violation;
  return summary;
}

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

static void ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(
    Objc3IdClassSelObjectPointerTypeCheckingSummary &summary) {
  const Objc3TypeFormScaffoldSummary scaffold = BuildObjc3TypeFormScaffoldSummary();
  summary.canonical_reference_form_count = scaffold.canonical_reference_form_count;
  summary.canonical_message_scalar_form_count = scaffold.canonical_message_scalar_form_count;
  summary.canonical_bridge_top_form_count = scaffold.canonical_bridge_top_form_count;
  summary.canonical_reference_forms_unique = scaffold.canonical_reference_forms_unique;
  summary.canonical_message_scalar_forms_unique = scaffold.canonical_message_scalar_forms_unique;
  summary.canonical_bridge_top_forms_unique = scaffold.canonical_bridge_top_forms_unique;
  summary.canonical_bridge_top_subset_of_reference = scaffold.canonical_bridge_top_subset_of_reference;
  summary.canonical_type_form_scaffold_ready = IsReadyObjc3TypeFormScaffoldSummary(scaffold);
  summary.canonical_type_form_diagnostics_hardening_consistent = scaffold.diagnostics_hardening_consistent;
  summary.canonical_type_form_diagnostics_hardening_ready = scaffold.diagnostics_hardening_ready;
  summary.canonical_type_form_diagnostics_hardening_key = scaffold.diagnostics_hardening_key;
  summary.canonical_type_form_recovery_determinism_consistent = scaffold.recovery_determinism_consistent;
  summary.canonical_type_form_recovery_determinism_ready = scaffold.recovery_determinism_ready;
  summary.canonical_type_form_recovery_determinism_key = scaffold.recovery_determinism_key;
  summary.canonical_type_form_conformance_matrix_consistent = scaffold.conformance_matrix_consistent;
  summary.canonical_type_form_conformance_matrix_ready = scaffold.conformance_matrix_ready;
  summary.canonical_type_form_conformance_matrix_key = scaffold.conformance_matrix_key;
  summary.canonical_type_form_conformance_corpus_case_count = scaffold.conformance_corpus_case_count;
  summary.canonical_type_form_conformance_corpus_passed_case_count = scaffold.conformance_corpus_passed_case_count;
  summary.canonical_type_form_conformance_corpus_failed_case_count = scaffold.conformance_corpus_failed_case_count;
  summary.canonical_type_form_conformance_corpus_consistent = scaffold.conformance_corpus_consistent;
  summary.canonical_type_form_conformance_corpus_ready = scaffold.conformance_corpus_ready;
  summary.canonical_type_form_conformance_corpus_key = scaffold.conformance_corpus_key;
  summary.canonical_type_form_performance_quality_required_guardrail_count =
      scaffold.performance_quality_required_guardrail_count;
  summary.canonical_type_form_performance_quality_passed_guardrail_count =
      scaffold.performance_quality_passed_guardrail_count;
  summary.canonical_type_form_performance_quality_failed_guardrail_count =
      scaffold.performance_quality_failed_guardrail_count;
  summary.canonical_type_form_performance_quality_guardrails_consistent =
      scaffold.performance_quality_guardrails_consistent;
  summary.canonical_type_form_performance_quality_guardrails_ready =
      scaffold.performance_quality_guardrails_ready;
  summary.canonical_type_form_performance_quality_guardrails_key =
      scaffold.performance_quality_guardrails_key;
}

static Objc3IdClassSelObjectPointerTypeCheckingSummary
BuildIdClassSelObjectPointerTypeCheckingSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3IdClassSelObjectPointerTypeCheckingSummary summary;
  ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(summary);

  const auto accumulate_function = [&summary](const FunctionInfo &info) {
    ++summary.return_type_sites;
    if (info.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (info.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (info.return_type == ValueType::ObjCSel) {
      ++summary.return_sel_spelling_sites;
    }
    if (info.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (info.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += info.param_types.size();
    const std::size_t count = std::min(
        info.param_types.size(),
        std::min(info.param_id_spelling.size(),
                 std::min(info.param_class_spelling.size(),
                          std::min(info.param_instancetype_spelling.size(),
                                   info.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (info.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (info.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (info.param_types[i] == ValueType::ObjCSel) {
        ++summary.param_sel_spelling_sites;
      }
      if (info.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (info.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != info.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_method = [&summary](const Objc3MethodInfo &info) {
    ++summary.return_type_sites;
    if (info.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (info.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (info.return_type == ValueType::ObjCSel) {
      ++summary.return_sel_spelling_sites;
    }
    if (info.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (info.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += info.param_types.size();
    const std::size_t count = std::min(
