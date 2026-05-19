static Objc3BlockCopyDisposeSemanticsSummary
BuildBlockCopyDisposeSemanticsSummaryFromSites(
    const std::vector<Objc3BlockCopyDisposeSiteMetadata> &sites) {
  Objc3BlockCopyDisposeSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.mutable_capture_count_total += site.mutable_capture_count;
    summary.byref_slot_count_total += site.byref_slot_count;
    summary.parameter_entries_total += site.parameter_count;
    summary.capture_entries_total += site.capture_count;
    summary.body_statement_entries_total += site.body_statement_count;
    if (site.copy_helper_required) {
      ++summary.copy_helper_required_sites;
    }
    if (site.dispose_helper_required) {
      ++summary.dispose_helper_required_sites;
    }
    if (site.copy_dispose_profile_is_normalized) {
      ++summary.profile_normalized_sites;
    }
    if (!site.copy_helper_symbol.empty()) {
      ++summary.copy_helper_symbolized_sites;
    }
    if (!site.dispose_helper_symbol.empty()) {
      ++summary.dispose_helper_symbolized_sites;
    }
    const bool copy_dispose_profile_missing =
        (site.copy_helper_required || site.dispose_helper_required) &&
        site.copy_dispose_profile.empty();
    const bool copy_helper_symbol_missing =
        site.copy_helper_required && site.copy_helper_symbol.empty();
    const bool dispose_helper_symbol_missing =
        site.dispose_helper_required && site.dispose_helper_symbol.empty();
    const bool copy_helper_requirement_mismatch =
        site.copy_helper_required !=
        (site.byref_slot_count > 0u || site.owned_object_capture_count > 0u);
    const bool dispose_helper_requirement_mismatch =
        site.dispose_helper_required !=
        (site.byref_slot_count > 0u ||
         site.owned_object_capture_count > 0u ||
         site.explicit_move_capture_count > 0u);
    const bool count_mismatch =
        site.mutable_capture_count > site.capture_count ||
        site.byref_slot_count > site.mutable_capture_count;
    const bool site_contract_violation =
        site.has_count_mismatch || count_mismatch ||
        !site.copy_dispose_profile_is_normalized ||
        !site.ownership_helper_eligibility_is_normalized ||
        copy_dispose_profile_missing || copy_helper_symbol_missing ||
        dispose_helper_symbol_missing || copy_helper_requirement_mismatch ||
        dispose_helper_requirement_mismatch;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.copy_helper_required_sites <= summary.block_literal_sites &&
      summary.dispose_helper_required_sites <= summary.block_literal_sites &&
      summary.profile_normalized_sites <= summary.block_literal_sites &&
      summary.copy_helper_symbolized_sites <= summary.block_literal_sites &&
      summary.dispose_helper_symbolized_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites &&
      summary.mutable_capture_count_total <= summary.capture_entries_total &&
      summary.byref_slot_count_total <= summary.mutable_capture_count_total &&
      summary.copy_helper_required_sites <= summary.dispose_helper_required_sites;
  return summary;
}

static Objc3BlockCopyDisposeSemanticsSummary
BuildBlockCopyDisposeSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockCopyDisposeSemanticsSummaryFromSites(
      surface.block_copy_dispose_sites_lexicographic);
}

static Objc3BlockCopyDisposeSemanticsSummary
BuildBlockCopyDisposeSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockCopyDisposeSemanticsSummaryFromSites(
      handoff.block_copy_dispose_sites_lexicographic);
}
