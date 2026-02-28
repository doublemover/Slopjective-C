#include "io/objc3_diagnostics_artifacts.h"

#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "diag/objc3_diag_utils.h"
#include "io/objc3_file_io.h"

namespace {

std::string EscapeJsonString(const std::string &value) {
  std::ostringstream out;
  for (unsigned char c : value) {
    switch (c) {
      case '"':
        out << "\\\"";
        break;
      case '\\':
        out << "\\\\";
        break;
      case '\b':
        out << "\\b";
        break;
      case '\f':
        out << "\\f";
        break;
      case '\n':
        out << "\\n";
        break;
      case '\r':
        out << "\\r";
        break;
      case '\t':
        out << "\\t";
        break;
      default:
        if (c < 0x20) {
          std::ostringstream code;
          code << std::hex << std::uppercase << static_cast<int>(c);
          std::string hex = code.str();
          while (hex.size() < 4) {
            hex = "0" + hex;
          }
          out << "\\u" << hex;
        } else {
          out << static_cast<char>(c);
        }
        break;
    }
  }
  return out.str();
}

std::vector<std::string> FlattenStageDiagnostics(const Objc3FrontendDiagnosticsBus &stage_diagnostics,
                                                 const std::vector<std::string> &post_pipeline_diagnostics) {
  std::vector<std::string> diagnostics;
  diagnostics.reserve(stage_diagnostics.size() + post_pipeline_diagnostics.size());
  diagnostics.insert(diagnostics.end(), stage_diagnostics.lexer.begin(), stage_diagnostics.lexer.end());
  diagnostics.insert(diagnostics.end(), stage_diagnostics.parser.begin(), stage_diagnostics.parser.end());
  diagnostics.insert(diagnostics.end(), stage_diagnostics.semantic.begin(), stage_diagnostics.semantic.end());
  diagnostics.insert(diagnostics.end(), post_pipeline_diagnostics.begin(), post_pipeline_diagnostics.end());
  return diagnostics;
}

void WriteDiagnosticsTextArtifact(const std::filesystem::path &out_dir,
                                  const std::string &emit_prefix,
                                  const std::vector<std::string> &diagnostics) {
  WriteText(out_dir / (emit_prefix + ".diagnostics.txt"), JoinLines(diagnostics));
}

void WriteDiagnosticsJsonArtifact(const std::filesystem::path &out_dir,
                                  const std::string &emit_prefix,
                                  const std::vector<std::string> &diagnostics) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"schema_version\": \"1.0.0\",\n";
  out << "  \"diagnostics\": [\n";
  for (std::size_t i = 0; i < diagnostics.size(); ++i) {
    const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);
    const unsigned line = key.line == std::numeric_limits<unsigned>::max() ? 0U : key.line;
    const unsigned column = key.column == std::numeric_limits<unsigned>::max() ? 0U : key.column;
    out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line
        << ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""
        << EscapeJsonString(key.message) << "\",\"raw\":\"" << EscapeJsonString(diagnostics[i]) << "\"}";
    if (i + 1 != diagnostics.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "  ]\n";
  out << "}\n";
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
