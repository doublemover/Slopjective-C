#include "parse/objc3_diagnostic_source_precision_scaffold.h"

#include <cstddef>
#include <string>

namespace {

bool StartsWith(const std::string &value, const std::string &prefix) {
  return value.size() >= prefix.size() &&
         value.compare(0, prefix.size(), prefix) == 0;
}

bool TryParseUnsignedSegment(
    const std::string &text,
    std::size_t begin,
    std::size_t delimiter_offset,
    unsigned &value) {
  if (delimiter_offset <= begin) {
    return false;
  }
  unsigned parsed = 0;
  for (std::size_t i = begin; i < delimiter_offset; ++i) {
    const char c = text[i];
    if (c < '0' || c > '9') {
      return false;
    }
    const unsigned digit = static_cast<unsigned>(c - '0');
    parsed = parsed * 10u + digit;
  }
  value = parsed;
  return true;
}

bool TryParseDiagnosticCoordinateAndCode(
    const std::string &diag_text,
    unsigned &line,
    unsigned &column,
    std::string &code) {
  constexpr const char *kPrefix = "error:";
  if (!StartsWith(diag_text, kPrefix)) {
    return false;
  }

  const std::size_t line_begin = 6u;
  const std::size_t first_colon = diag_text.find(':', line_begin);
  if (first_colon == std::string::npos ||
      !TryParseUnsignedSegment(diag_text, line_begin, first_colon, line)) {
    return false;
  }

  const std::size_t column_begin = first_colon + 1u;
  const std::size_t second_colon = diag_text.find(':', column_begin);
  if (second_colon == std::string::npos ||
      !TryParseUnsignedSegment(diag_text, column_begin, second_colon, column)) {
    return false;
  }
  if (second_colon + 1u >= diag_text.size() || diag_text[second_colon + 1u] != ' ') {
    return false;
  }

  const std::size_t code_begin_marker = diag_text.rfind(" [");
  if (code_begin_marker == std::string::npos ||
      code_begin_marker + 3u >= diag_text.size() ||
      diag_text.back() != ']') {
    return false;
  }

  code = diag_text.substr(code_begin_marker + 2u, diag_text.size() - code_begin_marker - 3u);
  return !code.empty();
}

}  // namespace

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
         ";consistent=" + (scaffold.scaffold_consistent ? "true" : "false");
}

Objc3ParserDiagnosticSourcePrecisionScaffold BuildObjc3ParserDiagnosticSourcePrecisionScaffold(
    const std::vector<std::string> &parser_diagnostics,
    const Objc3ParserContractSnapshot &parser_snapshot) {
  Objc3ParserDiagnosticSourcePrecisionScaffold scaffold;
  scaffold.parser_diagnostic_count = parser_diagnostics.size();

  for (const auto &diag_text : parser_diagnostics) {
    unsigned line = 0u;
    unsigned column = 0u;
    std::string code;
    if (!TryParseDiagnosticCoordinateAndCode(diag_text, line, column, code)) {
      continue;
    }
    ++scaffold.coordinate_tagged_diagnostic_count;
    ++scaffold.coded_diagnostic_count;
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
  scaffold.scaffold_consistent =
      scaffold.coordinate_format_consistent &&
      scaffold.diagnostic_code_suffix_consistent &&
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
