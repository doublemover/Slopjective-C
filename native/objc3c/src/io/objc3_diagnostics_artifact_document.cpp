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
    const DiagSortKey key = ParseDiagSortKey(diagnostic_text);
    const unsigned line =
        key.line == std::numeric_limits<unsigned>::max() ? 0U : key.line;
    const unsigned column =
        key.column == std::numeric_limits<unsigned>::max() ? 0U : key.column;
    diagnostic_values.push_back(JsonValue::ObjectValue(
        {{"severity", JsonValue::String(ToLower(key.severity))},
         {"line", JsonValue::Number(static_cast<double>(line))},
         {"column", JsonValue::Number(static_cast<double>(column))},
         {"code", JsonValue::String(key.code)},
         {"message", JsonValue::String(key.message)},
         {"raw", JsonValue::String(diagnostic_text)}}));
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
