#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

#include "tools/objc3c_frontend_c_api_runner_observability_json.h"

void AppendFrontendCApiRunnerObservabilityDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  if (options.dump_observability_json) {
    payloads.push_back(BuildFrontendCApiRunnerObservabilityJson(
        summary_path,
        result,
        status,
        result_error_message,
        runtime_metadata_binary_path_text));
  }
}
