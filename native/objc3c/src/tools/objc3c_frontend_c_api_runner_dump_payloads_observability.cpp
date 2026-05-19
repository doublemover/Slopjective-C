#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

#include "tools/objc3c_frontend_c_api_runner_observability_json.h"

void AppendFrontendCApiRunnerObservabilityDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication) {
  if (options.dump_observability_json) {
    const FrontendCApiRunnerObservabilityPublication
        observability_publication =
            BuildFrontendCApiRunnerObservabilityPublication(publication);
    payloads.push_back(
        BuildFrontendCApiRunnerObservabilityJson(observability_publication));
  }
}
