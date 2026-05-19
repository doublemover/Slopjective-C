#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_playground_rows.h"

#include <filesystem>
#include <ostream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"

namespace fs = std::filesystem;

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerBonusExperiencesPlaygroundDumpCommandRows(
    std::ostream &out,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths) {
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
}
