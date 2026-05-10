#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_availability.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_contract.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_dump_commands.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_object_inspection.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_runtime_abi.h"

void WriteFrontendCApiRunnerRuntimeInspectorJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  const FrontendCApiRunnerArtifactPathView paths =
      BuildFrontendCApiRunnerArtifactPathView(result, std::string());
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";
  const bool available = FrontendCApiRunnerPathExists(paths.object);
  const std::string availability_reason =
      available ? std::string() : "object artifact missing or not emitted";
  out << "{\n";
  WriteFrontendCApiRunnerRuntimeInspectorContractAvailabilityJsonRows(
      out,
      child_indent,
      options,
      paths,
      available);
  WriteFrontendCApiRunnerRuntimeInspectorObjectInspectionJsonRows(
      out,
      child_indent,
      paths);
  WriteFrontendCApiRunnerRuntimeInspectorRuntimeAbiJsonRows(out, child_indent);
  WriteFrontendCApiRunnerRuntimeInspectorDumpCommandJsonRows(
      out,
      child_indent,
      grandchild_indent,
      paths);
  WriteFrontendCApiRunnerRuntimeInspectorAvailabilityReasonJsonRows(
      out,
      child_indent,
      availability_reason);
  out << indent << "}";
}

std::string BuildFrontendCApiRunnerRuntimeInspectorJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  std::ostringstream dump;
  WriteFrontendCApiRunnerRuntimeInspectorJson(dump, "", options, result);
  dump << "\n";
  return dump.str();
}
