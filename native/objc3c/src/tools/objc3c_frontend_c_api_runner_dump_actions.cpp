#include "tools/objc3c_frontend_c_api_runner_dump_actions.h"

#include <string>

#include "tools/objc3c_frontend_c_api_runner_dump_emitter.h"
#include "tools/objc3c_frontend_c_api_runner_dump_payloads.h"

bool ShouldEmitFrontendCApiRunnerDumpActions(
    const FrontendCApiRunnerOptions &options) {
  return options.dump_summary_json || options.dump_observability_json ||
         options.dump_playground_repro_json ||
         options.dump_runtime_inspector_json || options.dump_stage_trace_json;
}

void EmitFrontendCApiRunnerDumpActions(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text,
    const std::string &summary_json) {
  FrontendCApiRunnerDumpEmitter emitter;
  for (const std::string &payload : BuildFrontendCApiRunnerDumpPayloads(
           options,
           summary_path,
           result,
           status,
           result_error_message,
           runtime_metadata_binary_path_text,
           summary_json)) {
    emitter.EmitJsonPayload(payload);
  }
}
