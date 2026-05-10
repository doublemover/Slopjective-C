void PublishObjc3DiagnosticGrammarHooksCoreFeatureExpansionConsistency(
    Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface,
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &core_feature_surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &source_precision_scaffold) {
  surface.accounting_consistent =
      surface.grammar_hook_code_count <= surface.parser_diagnostic_count &&
      surface.unique_diagnostic_code_count <= surface.grammar_hook_code_count &&
      surface.parser_snapshot_diagnostic_count == surface.parser_diagnostic_count;
  surface.source_precision_coverage_consistent =
      source_precision_scaffold.scaffold_consistent &&
      source_precision_scaffold.coordinate_tagged_diagnostic_count ==
          surface.parser_diagnostic_count;
  surface.replay_keys_ready =
      !source_precision_scaffold.scaffold_key.empty() &&
      !core_feature_surface.core_feature_key.empty() &&
      surface.diagnostic_code_fingerprint != 0 &&
      surface.source_precision_fingerprint != 0;
  surface.core_feature_expansion_ready =
      surface.core_feature_ready &&
      surface.accounting_consistent &&
      surface.source_precision_coverage_consistent &&
      surface.replay_keys_ready;
}
