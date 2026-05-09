#include "io/objc3_diagnostics_artifacts.h"

#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "diag/objc3_diag_utils.h"
#include "io/objc3_file_io.h"
#include "io/json/json_writer.h"

namespace {

using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonValue;

void WriteDiagnosticsTextArtifact(const std::filesystem::path &out_dir,
                                  const std::string &emit_prefix,
                                  const std::vector<std::string> &diagnostics) {
  WriteText(out_dir / (emit_prefix + ".diagnostics.txt"), JoinLines(diagnostics));
}

void WriteDiagnosticsJsonArtifact(const std::filesystem::path &out_dir,
                                  const std::string &emit_prefix,
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
  WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());
}

}  // namespace

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const Objc3FrontendDiagnosticsBus &stage_diagnostics,
                               const std::vector<std::string> &post_pipeline_diagnostics) {
  const std::vector<std::string> diagnostics = FlattenStageDiagnostics(stage_diagnostics, post_pipeline_diagnostics);
  WriteDiagnosticsTextArtifact(out_dir, emit_prefix, diagnostics);
  WriteDiagnosticsJsonArtifact(out_dir, emit_prefix, diagnostics);
}

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const std::vector<std::string> &diagnostics) {
  WriteDiagnosticsTextArtifact(out_dir, emit_prefix, diagnostics);
  WriteDiagnosticsJsonArtifact(out_dir, emit_prefix, diagnostics);
}
