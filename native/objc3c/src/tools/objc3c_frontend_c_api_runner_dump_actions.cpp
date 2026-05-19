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
    const FrontendCApiRunnerDumpPublication &publication) {
  FrontendCApiRunnerDumpEmitter emitter;
  for (const std::string &payload : BuildFrontendCApiRunnerDumpPayloads(
           options,
           publication)) {
    emitter.EmitJsonPayload(payload);
  }
}
