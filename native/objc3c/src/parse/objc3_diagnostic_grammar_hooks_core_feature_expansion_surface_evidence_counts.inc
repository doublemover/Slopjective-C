void PopulateObjc3DiagnosticGrammarHooksCoreFeatureExpansionEvidenceCounts(
    Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface,
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &core_feature_surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &source_precision_scaffold,
    std::size_t unique_diagnostic_code_count,
    std::uint64_t diagnostic_code_fingerprint,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count) {
  surface.parser_diagnostic_count = parser_diagnostic_count;
  surface.parser_snapshot_diagnostic_count = parser_snapshot_diagnostic_count;
  surface.grammar_hook_code_count = core_feature_surface.grammar_hook_code_count;
  surface.unique_diagnostic_code_count = unique_diagnostic_code_count;
  surface.diagnostic_code_fingerprint = diagnostic_code_fingerprint;
  surface.source_precision_fingerprint = source_precision_scaffold.coordinate_fingerprint;
  surface.core_feature_ready = IsObjc3DiagnosticGrammarHooksCoreFeatureReady(core_feature_surface);
}
