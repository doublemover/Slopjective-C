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
