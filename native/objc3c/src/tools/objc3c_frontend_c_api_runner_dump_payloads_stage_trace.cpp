#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

void AppendFrontendCApiRunnerStageTraceDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  if (options.dump_stage_trace_json) {
    payloads.push_back(BuildFrontendCApiRunnerStageTraceJson(result));
  }
}
