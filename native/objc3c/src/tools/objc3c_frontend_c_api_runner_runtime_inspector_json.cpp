#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_context.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_sections.h"

void WriteFrontendCApiRunnerRuntimeInspectorJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  const FrontendCApiRunnerRuntimeInspectorContext context =
      BuildFrontendCApiRunnerRuntimeInspectorContext(indent, result);
  out << "{\n";
  WriteFrontendCApiRunnerRuntimeInspectorSections(out, options, context);
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
