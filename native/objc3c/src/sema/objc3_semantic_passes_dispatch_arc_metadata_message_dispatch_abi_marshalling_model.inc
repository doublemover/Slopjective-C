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
