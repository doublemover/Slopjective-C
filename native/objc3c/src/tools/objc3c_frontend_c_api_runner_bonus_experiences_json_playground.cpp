#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_playground.h"

#include <filesystem>
#include <ostream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"

namespace fs = std::filesystem;

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerBonusExperiencesPlaygroundJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths,
    bool compile_surface_ready) {
  out << child_indent << "\"playground\": {\n";
  out << grandchild_indent << "\"available\": "
      << (compile_surface_ready ? "true" : "false") << ",\n";
  out << grandchild_indent << "\"source_path\": \""
      << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << grandchild_indent << "\"summary_path\": \""
      << EscapeJsonString(paths.summary) << "\",\n";
  out << grandchild_indent << "\"artifact_roots\": [\n";
  out << grandchild_indent << "  \"tmp/artifacts/playground\",\n";
  out << grandchild_indent << "  \"tmp/reports/playground\",\n";
  out << grandchild_indent << "  \"tmp/artifacts/showcase\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"public_actions\": [\n";
  out << grandchild_indent << "  \"materialize-playground-workspace\",\n";
  out << grandchild_indent << "  \"compile-objc3c\",\n";
  out << grandchild_indent << "  \"inspect-playground-repro\",\n";
  out << grandchild_indent << "  \"inspect-compile-observability\",\n";
  out << grandchild_indent << "  \"trace-compile-stages\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "  \"summary\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.summary))
      << "\",\n";
  out << grandchild_indent << "  \"diagnostics\": \""
      << EscapeJsonString(
             BuildFrontendCApiRunnerReadCommand(paths.diagnostics))
      << "\",\n";
  out << grandchild_indent << "  \"manifest\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.manifest))
      << "\",\n";
  out << grandchild_indent << "  \"repro_runner\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReproCommand(
             options,
             fs::path(paths.summary),
             true))
      << "\"\n";
  out << grandchild_indent << "}\n";
  out << child_indent << "},\n";
}
