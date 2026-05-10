#include "tools/objc3c_frontend_c_api_runner_observability_json_writer.h"

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityWriterContext(
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  return BuildFrontendCApiRunnerObservabilityContext(
      indent,
      summary_path_text,
      result,
      result_error_message,
      runtime_metadata_binary_path_text);
}
