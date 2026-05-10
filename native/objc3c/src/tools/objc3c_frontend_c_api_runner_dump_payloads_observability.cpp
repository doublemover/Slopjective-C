#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

#include "tools/objc3c_frontend_c_api_runner_observability_json.h"

void AppendFrontendCApiRunnerObservabilityDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication) {
  if (options.dump_observability_json) {
    payloads.push_back(BuildFrontendCApiRunnerObservabilityJson(
        publication.summary_path,
        FrontendCApiRunnerDumpCompileResult(publication),
        publication.status,
        publication.result_error_message,
        publication.runtime_metadata_binary_path_text));
  }
}
