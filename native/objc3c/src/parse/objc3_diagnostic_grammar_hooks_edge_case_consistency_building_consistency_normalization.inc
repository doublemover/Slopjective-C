namespace {

void NormalizeObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurface(
    Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface,
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
        &core_feature_expansion_surface,
    const Objc3FrontendOptions &options,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count,
    std::size_t parser_token_count,
    bool consistency_handoff_consistent) {
  surface.parser_diagnostic_count = parser_diagnostic_count;
  surface.parser_snapshot_diagnostic_count = parser_snapshot_diagnostic_count;
  surface.parser_token_count = parser_token_count;
  surface.language_profile_supported =
      options.language_profile == Objc3FrontendLanguageProfile::kCanonical;
  surface.core_feature_expansion_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
          core_feature_expansion_surface);
  surface.parser_snapshot_accounting_consistent =
      surface.parser_snapshot_diagnostic_count ==
      surface.parser_diagnostic_count;
  surface.parser_diagnostic_token_budget_consistent =
      surface.parser_diagnostic_count <= surface.parser_token_count;
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3DiagnosticGrammarHooksPragmaCoordinateOrderConsistent(
          pragma_contract);
  surface.consistency_handoff_consistent = consistency_handoff_consistent;
  surface.edge_case_consistency_consistent =
      surface.language_profile_supported &&
      surface.core_feature_expansion_ready &&
      surface.parser_snapshot_accounting_consistent &&
      surface.parser_diagnostic_token_budget_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.consistency_handoff_consistent;
  surface.edge_case_consistency_ready =
      surface.edge_case_consistency_consistent &&
      !core_feature_expansion_surface.expansion_key.empty();
}

}  // namespace
