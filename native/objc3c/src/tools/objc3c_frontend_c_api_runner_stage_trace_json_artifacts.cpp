#include "tools/objc3c_frontend_c_api_runner_stage_trace_json_rows.h"

#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

void WriteFrontendCApiRunnerArtifactStageTraceRows(
    std::ostream &out,
    const objc3c_frontend_c_compile_result_t &result) {
  WriteFrontendCApiRunnerStageSummaryJson(out, "emit", result.emit, false);
}
