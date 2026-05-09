#include "tools/objc3c_frontend_c_api_runner_observability_json.h"

#include <sstream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"
#include "tools/objc3c_frontend_c_api_runner_diagnostic_totals.h"
#include "tools/objc3c_frontend_c_api_runner_stage_selection.h"
#include "tools/objc3c_frontend_c_api_runner_status_mapping.h"

using objc3::io::EscapeJsonString;

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
  out << child_indent << "\"status_name\": \"" << FrontendCApiStatusName(status)
      << "\",\n";
  out << child_indent << "\"last_attempted_stage\": \""
      << EscapeJsonString(last_attempted_stage) << "\",\n";
  out << child_indent << "\"blocking_stage\": \""
      << EscapeJsonString(blocking_stage) << "\",\n";
  out << child_indent << "\"highest_diagnostic_severity\": \""
      << HighestFrontendCApiDiagnosticSeverity(diagnostic_totals) << "\",\n";
  out << child_indent << "\"result_error_message_present\": "
      << (!result_error_message.empty() ? "true" : "false") << ",\n";
  out << child_indent << "\"diagnostics_total\": " << diagnostic_totals.total
      << ",\n";
  out << child_indent << "\"diagnostics_notes\": " << diagnostic_totals.notes
      << ",\n";
  out << child_indent << "\"diagnostics_warnings\": "
      << diagnostic_totals.warnings << ",\n";
  out << child_indent << "\"diagnostics_errors\": " << diagnostic_totals.errors
      << ",\n";
  out << child_indent << "\"diagnostics_fatals\": " << diagnostic_totals.fatals
      << ",\n";
  out << child_indent << "\"artifact_presence\": {\n";
  out << grandchild_indent << "\"summary\": true,\n";
  out << grandchild_indent << "\"diagnostics\": "
      << (FrontendCApiRunnerPathExists(paths.diagnostics) ? "true"
                                                              : "false")
      << ",\n";
  out << grandchild_indent << "\"manifest\": "
      << (FrontendCApiRunnerPathExists(paths.manifest) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"ir\": "
      << (FrontendCApiRunnerPathExists(paths.ir) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"object\": "
      << (FrontendCApiRunnerPathExists(paths.object) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"runtime_metadata_binary\": "
      << (!paths.runtime_metadata_binary.empty() ? "true" : "false")
      << "\n";
  out << child_indent << "},\n";
  out << child_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "\"summary\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.summary))
      << "\",\n";
  out << grandchild_indent << "\"diagnostics\": \""
      << EscapeJsonString(
             BuildFrontendCApiRunnerReadCommand(paths.diagnostics))
      << "\",\n";
  out << grandchild_indent << "\"manifest\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.manifest))
      << "\",\n";
  out << grandchild_indent << "\"ir\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.ir))
      << "\",\n";
  out << grandchild_indent << "\"object\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.object))
      << "\"\n";
  out << child_indent << "}\n";
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
