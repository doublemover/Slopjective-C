Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionSurface(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &core_feature_surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &source_precision_scaffold,
    std::size_t unique_diagnostic_code_count,
    std::uint64_t diagnostic_code_fingerprint,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count) {
  Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface surface;
  PopulateObjc3DiagnosticGrammarHooksCoreFeatureExpansionEvidenceCounts(
      surface,
      core_feature_surface,
      source_precision_scaffold,
      unique_diagnostic_code_count,
      diagnostic_code_fingerprint,
      parser_diagnostic_count,
      parser_snapshot_diagnostic_count);
  PublishObjc3DiagnosticGrammarHooksCoreFeatureExpansionConsistency(
      surface,
      core_feature_surface,
      source_precision_scaffold);
  surface.expansion_key = BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionKey(
      surface,
      source_precision_scaffold.scaffold_key,
      core_feature_surface.core_feature_key);
  PublishObjc3DiagnosticGrammarHooksCoreFeatureExpansionFailure(surface);
  return surface;
}
