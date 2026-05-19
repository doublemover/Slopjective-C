bool CountObjc3ParserDiagnosticSourcePrecisionCoordinates(
    Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold,
    const std::vector<std::string> &parser_diagnostics) {
  bool diagnostic_owner_contract_consistent = true;

  for (const auto &diag_text : parser_diagnostics) {
    unsigned line = 0u;
    unsigned column = 0u;
    std::string code;
    if (!TryParseDiagnosticCoordinateAndCode(diag_text, line, column, code)) {
      continue;
    }
    ++scaffold.coordinate_tagged_diagnostic_count;
    ++scaffold.coded_diagnostic_count;
    diagnostic_owner_contract_consistent =
        diagnostic_owner_contract_consistent &&
        Objc3RenderedDiagnosticCodeMatchesStage(
            Objc3FrontendDiagnosticStage::kParser,
            code);
    scaffold.coordinate_fingerprint = MixObjc3ParserContractFingerprint(
        scaffold.coordinate_fingerprint,
        static_cast<std::uint64_t>(line));
    scaffold.coordinate_fingerprint = MixObjc3ParserContractFingerprint(
        scaffold.coordinate_fingerprint,
        static_cast<std::uint64_t>(column));
    scaffold.coordinate_fingerprint =
        MixObjc3ParserContractFingerprintString(scaffold.coordinate_fingerprint, code);
  }

  return diagnostic_owner_contract_consistent;
}
