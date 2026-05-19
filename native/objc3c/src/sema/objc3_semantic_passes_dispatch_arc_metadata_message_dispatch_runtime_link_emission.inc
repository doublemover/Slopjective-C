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
