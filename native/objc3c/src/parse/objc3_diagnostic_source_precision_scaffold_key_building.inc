std::string BuildObjc3ParserDiagnosticSourcePrecisionScaffoldKey(
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold,
    const Objc3ParserContractSnapshot &parser_snapshot) {
  return "parser_diagnostic_count=" + std::to_string(scaffold.parser_diagnostic_count) +
         ";snapshot_parser_diagnostic_count=" +
         std::to_string(parser_snapshot.parser_diagnostic_count) +
         ";coordinate_tagged_count=" +
         std::to_string(scaffold.coordinate_tagged_diagnostic_count) +
         ";coded_diagnostic_count=" +
         std::to_string(scaffold.coded_diagnostic_count) +
         ";coordinate_fingerprint=" +
         std::to_string(scaffold.coordinate_fingerprint) +
         ";coordinate_format_consistent=" +
         (scaffold.coordinate_format_consistent ? "true" : "false") +
         ";diagnostic_code_suffix_consistent=" +
         (scaffold.diagnostic_code_suffix_consistent ? "true" : "false") +
         ";diagnostic_owner_contract_consistent=" +
         (scaffold.diagnostic_owner_contract_consistent ? "true" : "false") +
         ";recovery_rejected_as_success=" +
         (scaffold.recovery_rejected_as_success ? "true" : "false") +
         ";consistent=" + (scaffold.scaffold_consistent ? "true" : "false");
}
