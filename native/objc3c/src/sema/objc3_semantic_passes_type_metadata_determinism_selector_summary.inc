  Objc3SelectorNormalizationSummary selector_summary;
  const auto accumulate_selector_summary = [&selector_summary](const Objc3SemanticMethodTypeMetadata &metadata) {
    ++selector_summary.methods_total;
    if (metadata.selector_contract_normalized) {
      ++selector_summary.normalized_methods;
    }
    selector_summary.selector_piece_entries += metadata.selector_piece_count;
    selector_summary.selector_parameter_piece_entries += metadata.selector_parameter_piece_count;
    if (metadata.selector_had_pieceless_form) {
      ++selector_summary.selector_pieceless_methods;
    }
    if (metadata.selector_has_spelling_mismatch) {
      ++selector_summary.selector_spelling_mismatches;
    }
    if (metadata.selector_has_arity_mismatch) {
      ++selector_summary.selector_arity_mismatches;
    }
    if (metadata.selector_has_parameter_linkage_mismatch) {
      ++selector_summary.selector_parameter_linkage_mismatches;
    }
    if (metadata.selector_has_normalization_flag_mismatch) {
      ++selector_summary.selector_normalization_flag_mismatches;
    }
    if (metadata.selector_has_missing_piece_keyword) {
      ++selector_summary.selector_missing_keyword_pieces;
    }
    if (metadata.selector.empty() ||
        metadata.selector_normalized.empty() ||
        metadata.selector != metadata.selector_normalized ||
        metadata.selector_parameter_piece_count > metadata.selector_piece_count) {
      selector_summary.deterministic = false;
    }
  };
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_selector_summary(method);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_selector_summary(method);
    }
  }
  selector_summary.deterministic =
      selector_summary.deterministic &&
      selector_summary.normalized_methods <= selector_summary.methods_total &&
      selector_summary.selector_parameter_piece_entries <= selector_summary.selector_piece_entries &&
      selector_summary.contract_violations() <= selector_summary.methods_total;
