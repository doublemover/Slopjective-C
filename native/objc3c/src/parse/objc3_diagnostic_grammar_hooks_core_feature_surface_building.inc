Objc3DiagnosticGrammarHooksCoreFeatureSurface BuildObjc3DiagnosticGrammarHooksCoreFeatureSurface(
    const std::vector<std::string> &parser_diagnostics,
    const Objc3ParserContractSnapshot &parser_snapshot,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold) {
  Objc3DiagnosticGrammarHooksCoreFeatureSurface surface;
  surface.parser_diagnostic_count = parser_diagnostics.size();

  bool namespace_consistent = true;
  bool coordinate_order_consistent = true;
  bool owner_contract_consistent = Objc3DiagnosticStageIsHardCutover(
      Objc3FrontendDiagnosticStage::kParser);
  bool first_coordinate = true;
  std::pair<unsigned, unsigned> previous_coordinate{0u, 0u};

  for (const auto &diag_text : parser_diagnostics) {
    unsigned line = 0u;
    unsigned column = 0u;
    std::string code;
    if (!TryParseDiagnosticCoordinateAndCode(diag_text, line, column, code)) {
      namespace_consistent = false;
      coordinate_order_consistent = false;
      owner_contract_consistent = false;
      continue;
    }
    if (!IsObjc3GrammarHookCode(code)) {
      namespace_consistent = false;
      owner_contract_consistent = false;
    } else {
      ++surface.grammar_hook_code_count;
    }
    owner_contract_consistent =
        owner_contract_consistent &&
        Objc3RenderedDiagnosticCodeMatchesStage(
            Objc3FrontendDiagnosticStage::kParser,
            code);

    if (!first_coordinate) {
      if (line < previous_coordinate.first ||
          (line == previous_coordinate.first && column < previous_coordinate.second)) {
        coordinate_order_consistent = false;
      }
    }
    previous_coordinate = std::make_pair(line, column);
    first_coordinate = false;
  }

  surface.grammar_hook_namespace_consistent =
      namespace_consistent && surface.grammar_hook_code_count == surface.parser_diagnostic_count;
  surface.coordinate_order_consistent = coordinate_order_consistent;
  surface.source_precision_consistent =
      scaffold.scaffold_consistent &&
      parser_snapshot.parser_diagnostic_count == surface.parser_diagnostic_count;
  const Objc3DiagnosticStageOwnerContract *parser_owner_contract =
      FindObjc3DiagnosticStageOwnerContract(Objc3FrontendDiagnosticStage::kParser);
  surface.owner_contract_consistent = owner_contract_consistent;
  surface.recovery_is_failure_boundary =
      parser_owner_contract != nullptr &&
      !parser_owner_contract->recovery_counts_as_success;
  surface.core_feature_consistent =
      surface.grammar_hook_namespace_consistent &&
      surface.coordinate_order_consistent &&
      surface.source_precision_consistent &&
      surface.owner_contract_consistent &&
      surface.recovery_is_failure_boundary;
  surface.core_feature_key = BuildObjc3DiagnosticGrammarHooksCoreFeatureKey(
      surface,
      scaffold,
      parser_snapshot);
  return surface;
}
