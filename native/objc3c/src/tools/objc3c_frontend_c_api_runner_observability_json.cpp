#include "tools/objc3c_frontend_c_api_runner_observability_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_diagnostic_totals.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_diagnostics.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_dump_commands.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_status.h"
#include "tools/objc3c_frontend_c_api_runner_stage_selection.h"

void WriteFrontendCApiRunnerObservabilityJson(
    std::ostream &out,
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  FrontendCApiRunnerArtifactPathView paths =
      BuildFrontendCApiRunnerArtifactPathView(result, summary_path_text);
  paths.runtime_metadata_binary = runtime_metadata_binary_path_text;
  const FrontendCApiDiagnosticTotals diagnostic_totals =
      BuildFrontendCApiDiagnosticTotals(result);
  const std::string last_attempted_stage =
      LastAttemptedFrontendCApiStageName(result);
  const std::string blocking_stage = BlockingFrontendCApiStageName(result);
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";
  out << "{\n";
  WriteFrontendCApiRunnerObservabilityStatusStageJsonRows(
      out,
      child_indent,
      status,
      last_attempted_stage,
      blocking_stage);
  WriteFrontendCApiRunnerObservabilityDiagnosticTotalJsonRows(
      out,
      child_indent,
      diagnostic_totals,
      !result_error_message.empty());
  WriteFrontendCApiRunnerObservabilityArtifactPresenceJsonRows(
      out,
      child_indent,
      grandchild_indent,
      paths);
  WriteFrontendCApiRunnerObservabilityDumpCommandJsonRows(
      out,
      child_indent,
      grandchild_indent,
      paths);
  out << indent << "}";
}

std::string BuildFrontendCApiRunnerObservabilityJson(
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  std::ostringstream dump;
  WriteFrontendCApiRunnerObservabilityJson(
      dump,
      "",
      summary_path.generic_string(),
      result,
      status,
      result_error_message,
      runtime_metadata_binary_path_text);
  dump << "\n";
  return dump.str();
}
