std::string BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencyKey(
    const Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface,
    const std::string &core_feature_expansion_key) {
  std::ostringstream key;
  key << "parser-diagnostic-grammar-hooks-edge-case-consistency:v1:"
      << "parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";parser_snapshot_diagnostic_count="
      << surface.parser_snapshot_diagnostic_count
      << ";parser_token_count=" << surface.parser_token_count
      << ";language_profile_supported="
      << (surface.language_profile_supported ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";parser_snapshot_accounting_consistent="
      << (surface.parser_snapshot_accounting_consistent ? "true" : "false")
      << ";parser_diagnostic_token_budget_consistent="
      << (surface.parser_diagnostic_token_budget_consistent ? "true"
                                                            : "false")
      << ";language_version_pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                      : "false")
      << ";consistency_handoff_consistent="
      << (surface.consistency_handoff_consistent ? "true" : "false")
      << ";edge_case_consistency_consistent="
      << (surface.edge_case_consistency_consistent ? "true" : "false")
      << ";edge_case_consistency_ready="
      << (surface.edge_case_consistency_ready ? "true" : "false")
      << ";core_feature_expansion_key=" << core_feature_expansion_key;
  return key.str();
}
