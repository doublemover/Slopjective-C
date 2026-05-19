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
