std::string BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionKey(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface,
    const std::string &source_precision_scaffold_key,
    const std::string &core_feature_key) {
  std::ostringstream key;
  key << "parser-diagnostic-grammar-hooks-core-feature-expansion:v1:"
      << "parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";parser_snapshot_diagnostic_count=" << surface.parser_snapshot_diagnostic_count
      << ";grammar_hook_code_count=" << surface.grammar_hook_code_count
      << ";unique_diagnostic_code_count=" << surface.unique_diagnostic_code_count
      << ";diagnostic_code_fingerprint=" << surface.diagnostic_code_fingerprint
      << ";source_precision_fingerprint=" << surface.source_precision_fingerprint
      << ";core_feature_ready=" << (surface.core_feature_ready ? "true" : "false")
      << ";accounting_consistent=" << (surface.accounting_consistent ? "true" : "false")
      << ";source_precision_coverage_consistent="
      << (surface.source_precision_coverage_consistent ? "true" : "false")
      << ";replay_keys_ready=" << (surface.replay_keys_ready ? "true" : "false")
      << ";source_precision_scaffold_key=" << source_precision_scaffold_key
      << ";core_feature_key=" << core_feature_key
      << ";ready=" << (surface.core_feature_expansion_ready ? "true" : "false");
  return key.str();
}
