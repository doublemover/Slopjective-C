#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"

void AppendFrontendCApiRunnerRuntimeInspectorDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication) {
  if (options.dump_runtime_inspector_json) {
    payloads.push_back(
        BuildFrontendCApiRunnerRuntimeInspectorJson(
            options,
            FrontendCApiRunnerDumpCompileResult(publication)));
  }
}
