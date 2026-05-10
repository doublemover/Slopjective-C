Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface
BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurface(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
        &core_feature_expansion_surface,
    const Objc3FrontendOptions &options,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count,
    std::size_t parser_token_count,
    bool consistency_handoff_consistent) {
  Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface surface;
  NormalizeObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurface(
      surface,
      core_feature_expansion_surface,
      options,
      pragma_contract,
      parser_diagnostic_count,
      parser_snapshot_diagnostic_count,
      parser_token_count,
      consistency_handoff_consistent);
  surface.consistency_key =
      BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencyKey(
          surface,
          core_feature_expansion_surface.expansion_key);
  if (surface.edge_case_consistency_ready) {
    return surface;
  }

  PublishObjc3DiagnosticGrammarHooksEdgeCaseConsistencyFailureReason(surface);
  return surface;
}
