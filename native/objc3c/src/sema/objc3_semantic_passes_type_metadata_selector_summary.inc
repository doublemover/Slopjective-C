  handoff.selector_normalization_summary = Objc3SelectorNormalizationSummary{};
  const auto accumulate_method_selector_metadata =
      [&handoff](const Objc3SemanticMethodTypeMetadata &metadata) {
        ++handoff.selector_normalization_summary.methods_total;
        if (metadata.selector_contract_normalized) {
          ++handoff.selector_normalization_summary.normalized_methods;
        }
        handoff.selector_normalization_summary.selector_piece_entries += metadata.selector_piece_count;
        handoff.selector_normalization_summary.selector_parameter_piece_entries +=
            metadata.selector_parameter_piece_count;
        if (metadata.selector_had_pieceless_form) {
          ++handoff.selector_normalization_summary.selector_pieceless_methods;
        }
        if (metadata.selector_has_spelling_mismatch) {
          ++handoff.selector_normalization_summary.selector_spelling_mismatches;
        }
        if (metadata.selector_has_arity_mismatch) {
          ++handoff.selector_normalization_summary.selector_arity_mismatches;
        }
        if (metadata.selector_has_parameter_linkage_mismatch) {
          ++handoff.selector_normalization_summary.selector_parameter_linkage_mismatches;
        }
        if (metadata.selector_has_normalization_flag_mismatch) {
          ++handoff.selector_normalization_summary.selector_normalization_flag_mismatches;
        }
        if (metadata.selector_has_missing_piece_keyword) {
          ++handoff.selector_normalization_summary.selector_missing_keyword_pieces;
        }
        if (metadata.selector_normalized.empty() ||
            metadata.selector_parameter_piece_count > metadata.selector_piece_count) {
          handoff.selector_normalization_summary.deterministic = false;
        }
      };
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_selector_metadata(method_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method_selector_metadata(method_metadata);
    }
  }
  handoff.selector_normalization_summary.deterministic =
      handoff.selector_normalization_summary.deterministic &&
      handoff.selector_normalization_summary.normalized_methods <=
          handoff.selector_normalization_summary.methods_total &&
      handoff.selector_normalization_summary.selector_parameter_piece_entries <=
          handoff.selector_normalization_summary.selector_piece_entries &&
      handoff.selector_normalization_summary.contract_violations() <=
          handoff.selector_normalization_summary.methods_total;
