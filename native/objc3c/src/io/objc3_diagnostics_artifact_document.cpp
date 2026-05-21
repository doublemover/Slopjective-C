#include "io/objc3_diagnostics_artifact_document.h"

#include <limits>
#include <sstream>
#include <string>
#include <utility>

#include "diag/objc3_diag_utils.h"
#include "io/json/json_writer.h"
#include "io/objc3_file_io.h"

namespace {

using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonValue;

JsonValue PositionValue(unsigned line, unsigned column) {
  return JsonValue::ObjectValue(
      {{"line", JsonValue::Number(static_cast<double>(line))},
       {"column", JsonValue::Number(static_cast<double>(column))}});
}

JsonValue RangeValue(unsigned start_line,
                     unsigned start_column,
                     unsigned end_line,
                     unsigned end_column) {
  return JsonValue::ObjectValue(
      {{"start", PositionValue(start_line, start_column)},
       {"end", PositionValue(end_line, end_column)}});
}

const char *DiagnosticPhaseForCode(const std::string &code) {
  if (code.size() < 3) {
    return "unknown";
  }
  switch (code[2]) {
    case 'C':
    case 'P':
      return "parse";
    case 'S':
      return "sema";
    case 'L':
      return "lex";
    case 'A':
    case 'T':
      return "post-pipeline";
    case 'R':
      return "runtime";
    case 'E':
      return "frontend-api";
    default:
      return "unknown";
  }
}

unsigned DiagnosticSpanEndColumn(const std::string &code,
                                 unsigned column) {
  if (column == 0U) {
    return 0U;
  }
  if (code == "O3C004") {
    return column + 8U;
  }
  return column + 1U;
}

JsonValue::Array BuildDiagnosticFixits(const std::string &code,
                                       unsigned line,
                                       unsigned column) {
  JsonValue::Array fixits;
  if (line == 0U || column == 0U) {
    return fixits;
  }

  if (code == "O3C004") {
    fixits.push_back(JsonValue::ObjectValue(
        {{"applicability", JsonValue::String("machine-applicable")},
         {"kind", JsonValue::String("replace-token")},
         {"range", RangeValue(line, column, line, column + 8U)},
         {"replacement", JsonValue::String("Optional")},
         {"title", JsonValue::String(
                       "replace optional<T> with canonical Optional<T>")}}));
  }

  return fixits;
}

JsonValue BuildDiagnosticRecovery(const std::string &code) {
  std::string strategy = "diagnostic-artifact-boundary";
  std::string boundary = "single diagnostic";
  bool available = false;

  if (code == "O3P104") {
    strategy = "parser-statement-boundary-synchronization";
    boundary = "next statement token";
    available = true;
  } else if (code == "O3C004") {
    strategy = "parser-canonical-spelling-rejection";
    boundary = "canonical type spelling";
    available = true;
  } else if (code == "O3P100") {
    strategy = "parser-local-rejection-boundary";
    boundary = "unsupported top-level construct";
    available = true;
  } else if (code == "O3S203") {
    strategy = "semantic-symbol-resolution-boundary";
    boundary = "unresolved callee";
    available = true;
  } else if (code == "O3S211") {
    strategy = "semantic-return-contract-boundary";
    boundary = "function return statement";
    available = true;
  }

  return JsonValue::ObjectValue(
      {{"accepts_invalid_program", JsonValue::Bool(false)},
       {"available", JsonValue::Bool(available)},
       {"boundary", JsonValue::String(boundary)},
       {"deterministic", JsonValue::Bool(true)},
       {"recovery_counts_as_success", JsonValue::Bool(false)},
       {"strategy", JsonValue::String(strategy)}});
}

JsonValue BuildDiagnosticJsonValue(const std::string &diagnostic_text) {
  const DiagSortKey key = ParseDiagSortKey(diagnostic_text);
  const unsigned line =
      key.line == std::numeric_limits<unsigned>::max() ? 0U : key.line;
  const unsigned column =
      key.column == std::numeric_limits<unsigned>::max() ? 0U : key.column;
  const unsigned end_column = DiagnosticSpanEndColumn(key.code, column);
  const Objc3DiagnosticCategory category = DiagnosticCategoryForCode(key.code);

  return JsonValue::ObjectValue(
      {{"category", JsonValue::String(DiagnosticCategoryName(category))},
       {"code", JsonValue::String(key.code)},
       {"column", JsonValue::Number(static_cast<double>(column))},
       {"fixits",
        JsonValue::ArrayValue(BuildDiagnosticFixits(key.code, line, column))},
       {"line", JsonValue::Number(static_cast<double>(line))},
       {"message", JsonValue::String(key.message)},
       {"phase", JsonValue::String(DiagnosticPhaseForCode(key.code))},
       {"raw", JsonValue::String(diagnostic_text)},
       {"recovery", BuildDiagnosticRecovery(key.code)},
       {"severity", JsonValue::String(ToLower(key.severity))},
       {"span", RangeValue(line, column, line, end_column)}});
}

}  // namespace

std::string BuildDiagnosticsTextArtifact(
    const std::vector<std::string> &diagnostics) {
  return JoinLines(diagnostics);
}

std::string BuildDiagnosticsJsonArtifact(
    const std::vector<std::string> &diagnostics) {
  JsonValue::Array diagnostic_values;
  diagnostic_values.reserve(diagnostics.size());
  for (const std::string &diagnostic_text : diagnostics) {
    diagnostic_values.push_back(BuildDiagnosticJsonValue(diagnostic_text));
  }

  std::ostringstream out;
  JsonObjectWriter artifact(out);
  artifact.StringField("schema_version", "1.0.0");
  artifact.RawJsonField(
      "diagnostics",
      objc3::io::json::RenderJson(
          JsonValue::ArrayValue(std::move(diagnostic_values))));
  artifact.End();
  out << '\n';
  return out.str();
}
