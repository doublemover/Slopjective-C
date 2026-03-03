#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"

#include <string>
#include <utility>
#include <vector>

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
    parsed = parsed * 10u + static_cast<unsigned>(c - '0');
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

bool IsObjc3GrammarHookCode(const std::string &code) {
  if (!StartsWith(code, "O3P") || code.size() != 6u) {
    return false;
  }
  for (std::size_t i = 3u; i < code.size(); ++i) {
    const char c = code[i];
    if (c < '0' || c > '9') {
      return false;
    }
  }
  return true;
}

}  // namespace

std::string BuildObjc3DiagnosticGrammarHooksCoreFeatureKey(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold,
    const Objc3ParserContractSnapshot &parser_snapshot) {
  return "parser_diagnostic_count=" + std::to_string(surface.parser_diagnostic_count) +
         ";snapshot_parser_diagnostic_count=" + std::to_string(parser_snapshot.parser_diagnostic_count) +
         ";grammar_hook_code_count=" + std::to_string(surface.grammar_hook_code_count) +
         ";grammar_hook_namespace_consistent=" + (surface.grammar_hook_namespace_consistent ? "true" : "false") +
         ";coordinate_order_consistent=" + (surface.coordinate_order_consistent ? "true" : "false") +
         ";source_precision_consistent=" + (surface.source_precision_consistent ? "true" : "false") +
         ";source_precision_scaffold_key=" + scaffold.scaffold_key +
         ";consistent=" + (surface.core_feature_consistent ? "true" : "false");
}

Objc3DiagnosticGrammarHooksCoreFeatureSurface BuildObjc3DiagnosticGrammarHooksCoreFeatureSurface(
    const std::vector<std::string> &parser_diagnostics,
    const Objc3ParserContractSnapshot &parser_snapshot,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold) {
  Objc3DiagnosticGrammarHooksCoreFeatureSurface surface;
  surface.parser_diagnostic_count = parser_diagnostics.size();

  bool namespace_consistent = true;
  bool coordinate_order_consistent = true;
  bool first_coordinate = true;
  std::pair<unsigned, unsigned> previous_coordinate{0u, 0u};

  for (const auto &diag_text : parser_diagnostics) {
    unsigned line = 0u;
    unsigned column = 0u;
    std::string code;
    if (!TryParseDiagnosticCoordinateAndCode(diag_text, line, column, code)) {
      namespace_consistent = false;
      coordinate_order_consistent = false;
      continue;
    }
    if (!IsObjc3GrammarHookCode(code)) {
      namespace_consistent = false;
    } else {
      ++surface.grammar_hook_code_count;
    }

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
  surface.core_feature_consistent =
      surface.grammar_hook_namespace_consistent &&
      surface.coordinate_order_consistent &&
      surface.source_precision_consistent;
  surface.core_feature_key = BuildObjc3DiagnosticGrammarHooksCoreFeatureKey(
      surface,
      scaffold,
      parser_snapshot);
  return surface;
}

bool IsObjc3DiagnosticGrammarHooksCoreFeatureReady(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &surface) {
  return surface.core_feature_consistent && !surface.core_feature_key.empty();
}
