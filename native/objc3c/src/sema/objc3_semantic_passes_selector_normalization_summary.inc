static void AccumulateSelectorNormalizationFromMethodInfo(const Objc3MethodInfo &method,
                                                          Objc3SelectorNormalizationSummary &summary) {
  ++summary.methods_total;
  if (method.selector_contract_normalized) {
    ++summary.normalized_methods;
  }
  summary.selector_piece_entries += method.selector_piece_count;
  summary.selector_parameter_piece_entries += method.selector_parameter_piece_count;
  if (method.selector_had_pieceless_form) {
    ++summary.selector_pieceless_methods;
  }
  if (method.selector_has_spelling_mismatch) {
    ++summary.selector_spelling_mismatches;
  }
  if (method.selector_has_arity_mismatch) {
    ++summary.selector_arity_mismatches;
  }
  if (method.selector_has_parameter_linkage_mismatch) {
    ++summary.selector_parameter_linkage_mismatches;
  }
  if (method.selector_has_normalization_flag_mismatch) {
    ++summary.selector_normalization_flag_mismatches;
  }
  if (method.selector_has_missing_piece_keyword) {
    ++summary.selector_missing_keyword_pieces;
  }
  if (method.selector_parameter_piece_count > method.selector_piece_count ||
      method.selector_normalized.empty()) {
    summary.deterministic = false;
  }
}

static Objc3SelectorNormalizationSummary BuildSelectorNormalizationSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3SelectorNormalizationSummary summary;
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateSelectorNormalizationFromMethodInfo(method_entry.second, summary);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateSelectorNormalizationFromMethodInfo(method_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.normalized_methods <= summary.methods_total &&
                          summary.selector_parameter_piece_entries <= summary.selector_piece_entries &&
                          summary.contract_violations() <= summary.methods_total;
  return summary;
}
