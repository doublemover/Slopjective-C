#include "tools/objc3c_frontend_c_api_runner_dump_actions.h"

#include <iostream>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_observability_json.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"
#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

namespace {

void EmitFrontendCApiRunnerDumpJson(const std::string &payload,
                                    bool &emitted_dump) {
  if (emitted_dump) {
    std::cout << "\n";
  }
  std::cout << payload;
  emitted_dump = true;
}

}  // namespace

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
  bool emitted_dump = false;
  if (options.dump_summary_json) {
    EmitFrontendCApiRunnerDumpJson(summary_json, emitted_dump);
  }
  if (options.dump_observability_json) {
    EmitFrontendCApiRunnerDumpJson(
        BuildFrontendCApiRunnerObservabilityJson(
            summary_path,
            result,
            status,
            result_error_message,
            runtime_metadata_binary_path_text),
        emitted_dump);
  }
  if (options.dump_playground_repro_json) {
    EmitFrontendCApiRunnerDumpJson(
        BuildFrontendCApiRunnerPlaygroundReproJson(
            options,
            result,
            summary_path),
        emitted_dump);
  }
  if (options.dump_runtime_inspector_json) {
    EmitFrontendCApiRunnerDumpJson(
        BuildFrontendCApiRunnerRuntimeInspectorJson(options, result),
        emitted_dump);
  }
  if (options.dump_stage_trace_json) {
    EmitFrontendCApiRunnerDumpJson(
        BuildFrontendCApiRunnerStageTraceJson(result),
        emitted_dump);
  }
}
