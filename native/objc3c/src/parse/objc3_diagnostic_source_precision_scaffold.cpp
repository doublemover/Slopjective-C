#include "parse/objc3_diagnostic_source_precision_scaffold.h"

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "diag/objc3_diag_utils.h"

#include <string>

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

Objc3ParserDiagnosticSourcePrecisionScaffold BuildObjc3ParserDiagnosticSourcePrecisionScaffold(
    const std::vector<std::string> &parser_diagnostics,
    const Objc3ParserContractSnapshot &parser_snapshot) {
  Objc3ParserDiagnosticSourcePrecisionScaffold scaffold;
  scaffold.parser_diagnostic_count = parser_diagnostics.size();
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
  scaffold.scaffold_key = BuildObjc3ParserDiagnosticSourcePrecisionScaffoldKey(
      scaffold,
      parser_snapshot);
  return scaffold;
}

bool IsObjc3ParserDiagnosticSourcePrecisionScaffoldReady(
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold) {
  return scaffold.scaffold_consistent && !scaffold.scaffold_key.empty();
}
