#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "parse/objc3_parser_contract.h"

struct Objc3ParserDiagnosticSourcePrecisionScaffold {
  std::size_t parser_diagnostic_count = 0;
  std::size_t coordinate_tagged_diagnostic_count = 0;
  std::size_t coded_diagnostic_count = 0;
  std::uint64_t coordinate_fingerprint = 1469598103934665603ull;
  bool coordinate_format_consistent = false;
  bool diagnostic_code_suffix_consistent = false;
  bool scaffold_consistent = false;
  std::string scaffold_key;
};

std::string BuildObjc3ParserDiagnosticSourcePrecisionScaffoldKey(
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold,
    const Objc3ParserContractSnapshot &parser_snapshot);

Objc3ParserDiagnosticSourcePrecisionScaffold BuildObjc3ParserDiagnosticSourcePrecisionScaffold(
    const std::vector<std::string> &parser_diagnostics,
    const Objc3ParserContractSnapshot &parser_snapshot);

bool IsObjc3ParserDiagnosticSourcePrecisionScaffoldReady(
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold);
