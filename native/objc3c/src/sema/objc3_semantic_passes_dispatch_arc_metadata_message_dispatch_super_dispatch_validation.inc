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
