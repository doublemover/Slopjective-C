void PublishObjc3ParserDiagnosticSourcePrecisionConsistency(
    Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold,
    const Objc3ParserContractSnapshot &parser_snapshot,
    bool diagnostic_owner_contract_consistent) {
  scaffold.coordinate_format_consistent =
      scaffold.coordinate_tagged_diagnostic_count == scaffold.parser_diagnostic_count;
  scaffold.diagnostic_code_suffix_consistent =
      scaffold.coded_diagnostic_count == scaffold.parser_diagnostic_count;
  scaffold.diagnostic_owner_contract_consistent =
      scaffold.parser_diagnostic_count == 0 ||
      (diagnostic_owner_contract_consistent &&
       Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kParser));
  const Objc3DiagnosticStageOwnerContract *parser_owner_contract =
      FindObjc3DiagnosticStageOwnerContract(Objc3FrontendDiagnosticStage::kParser);
  scaffold.recovery_rejected_as_success =
      parser_owner_contract != nullptr &&
      !parser_owner_contract->recovery_counts_as_success;
  scaffold.scaffold_consistent =
      scaffold.coordinate_format_consistent &&
      scaffold.diagnostic_code_suffix_consistent &&
      scaffold.diagnostic_owner_contract_consistent &&
      scaffold.recovery_rejected_as_success &&
      parser_snapshot.parser_diagnostic_count == scaffold.parser_diagnostic_count &&
      (scaffold.parser_diagnostic_count == 0 || scaffold.coordinate_fingerprint != 0);
}
