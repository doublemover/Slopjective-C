#include "tools/objc3c_frontend_c_api_runner_dump_options.h"

bool ApplyFrontendCApiRunnerDumpOption(const std::string &arg,
                                       FrontendCApiRunnerOptions &options) {
  if (arg == "--dump-summary-json") {
    options.dump_summary_json = true;
    return true;
  }
  if (arg == "--dump-observability-json") {
    options.dump_observability_json = true;
    return true;
  }
  if (arg == "--dump-playground-repro-json") {
    options.dump_playground_repro_json = true;
    return true;
  }
  if (arg == "--dump-runtime-inspector-json") {
    options.dump_runtime_inspector_json = true;
    return true;
  }
  if (arg == "--dump-stage-trace-json") {
    options.dump_stage_trace_json = true;
    return true;
  }
  return false;
}
