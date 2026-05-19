#include "tools/objc3c_frontend_c_api_runner_stage_trace_json_rows.h"

#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

void WriteFrontendCApiRunnerCompileSessionStageTraceRows(
    std::ostream &out,
    const objc3c_frontend_c_compile_result_t &result) {
  WriteFrontendCApiRunnerStageSummaryJson(out, "lex", result.lex, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "parse", result.parse, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "sema", result.sema, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "lower", result.lower, true);
}
