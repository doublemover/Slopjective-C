#include "tools/objc3c_frontend_c_api_runner_observability_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_observability_json_context.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_sections.h"

void WriteFrontendCApiRunnerObservabilityJson(
    std::ostream &out,
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  const FrontendCApiRunnerObservabilityContext context =
      BuildFrontendCApiRunnerObservabilityContext(
          indent,
          summary_path_text,
          result,
          result_error_message,
          runtime_metadata_binary_path_text);
  out << "{\n";
  WriteFrontendCApiRunnerObservabilitySections(out, status, context);
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
