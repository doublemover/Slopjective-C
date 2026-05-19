#include "tools/objc3c_frontend_c_api_runner_summary_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

void WriteFrontendCApiRunnerSummaryStageBlock(
    std::ostream &out,
    const objc3c_frontend_c_compile_result_t &result) {
  out << "  \"stages\": {\n";
  WriteFrontendCApiRunnerStageSummaryJson(out, "lex", result.lex, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "parse", result.parse, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "sema", result.sema, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "lower", result.lower, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "emit", result.emit, false);
  out << "  },\n";
}
