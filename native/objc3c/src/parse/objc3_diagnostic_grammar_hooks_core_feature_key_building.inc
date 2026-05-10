std::string BuildObjc3DiagnosticGrammarHooksCoreFeatureKey(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold,
    const Objc3ParserContractSnapshot &parser_snapshot) {
  return "parser_diagnostic_count=" + std::to_string(surface.parser_diagnostic_count) +
         ";snapshot_parser_diagnostic_count=" + std::to_string(parser_snapshot.parser_diagnostic_count) +
         ";grammar_hook_code_count=" + std::to_string(surface.grammar_hook_code_count) +
         ";grammar_hook_namespace_consistent=" + (surface.grammar_hook_namespace_consistent ? "true" : "false") +
         ";coordinate_order_consistent=" + (surface.coordinate_order_consistent ? "true" : "false") +
         ";source_precision_consistent=" + (surface.source_precision_consistent ? "true" : "false") +
         ";owner_contract_consistent=" + (surface.owner_contract_consistent ? "true" : "false") +
         ";recovery_is_failure_boundary=" + (surface.recovery_is_failure_boundary ? "true" : "false") +
         ";source_precision_scaffold_key=" + scaffold.scaffold_key +
         ";consistent=" + (surface.core_feature_consistent ? "true" : "false");
}
