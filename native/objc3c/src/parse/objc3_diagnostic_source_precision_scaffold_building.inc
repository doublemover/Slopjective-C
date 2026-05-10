Objc3ParserDiagnosticSourcePrecisionScaffold BuildObjc3ParserDiagnosticSourcePrecisionScaffold(
    const std::vector<std::string> &parser_diagnostics,
    const Objc3ParserContractSnapshot &parser_snapshot) {
  Objc3ParserDiagnosticSourcePrecisionScaffold scaffold;
  scaffold.parser_diagnostic_count = parser_diagnostics.size();
  const bool diagnostic_owner_contract_consistent =
      CountObjc3ParserDiagnosticSourcePrecisionCoordinates(scaffold, parser_diagnostics);

  PublishObjc3ParserDiagnosticSourcePrecisionConsistency(
      scaffold,
      parser_snapshot,
      diagnostic_owner_contract_consistent);
  scaffold.scaffold_key = BuildObjc3ParserDiagnosticSourcePrecisionScaffoldKey(
      scaffold,
      parser_snapshot);
  return scaffold;
}
