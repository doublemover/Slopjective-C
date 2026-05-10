#include "tools/objc3c_frontend_c_api_runner_playground_repro_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_compile_profile.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_contract_source.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_dump_commands.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_public_surfaces.h"

void WriteFrontendCApiRunnerPlaygroundReproJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text) {
  const FrontendCApiRunnerArtifactPathView paths =
      BuildFrontendCApiRunnerArtifactPathView(result, summary_path_text);
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";

  out << "{\n";
  WriteFrontendCApiRunnerPlaygroundReproContractSourceJsonRows(
      out,
      child_indent,
      options,
      result,
      paths);
  WriteFrontendCApiRunnerPlaygroundReproArtifactPathJsonRows(
      out,
      child_indent,
      grandchild_indent,
      paths);
  WriteFrontendCApiRunnerPlaygroundReproCompileProfileJsonRows(
      out,
      child_indent,
      grandchild_indent,
      options);
  WriteFrontendCApiRunnerPlaygroundReproPublicSurfaceJsonRows(
      out,
      child_indent);
  WriteFrontendCApiRunnerPlaygroundReproDumpCommandJsonRows(
      out,
      child_indent,
      grandchild_indent,
      options,
      paths);
  out << indent << "}";
}

std::string BuildFrontendCApiRunnerPlaygroundReproJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::filesystem::path &summary_path) {
  std::ostringstream dump;
  WriteFrontendCApiRunnerPlaygroundReproJson(
      dump,
      "",
      options,
      result,
      summary_path.generic_string());
  dump << "\n";
  return dump.str();
}
