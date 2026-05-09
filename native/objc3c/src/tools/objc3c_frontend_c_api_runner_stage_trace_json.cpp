#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

#include <sstream>

void WriteFrontendCApiRunnerStageSummaryJson(
    std::ostream &out,
    const char *name,
    const objc3c_frontend_c_stage_summary_t &summary,
    bool trailing_comma) {
  out << "    \"" << name << "\": {\n";
  out << "      \"stage\": " << static_cast<unsigned>(summary.stage) << ",\n";
  out << "      \"attempted\": "
      << (summary.attempted != 0 ? "true" : "false") << ",\n";
  out << "      \"skipped\": "
      << (summary.skipped != 0 ? "true" : "false") << ",\n";
  out << "      \"diagnostics_total\": " << summary.diagnostics_total << ",\n";
  out << "      \"diagnostics_notes\": " << summary.diagnostics_notes << ",\n";
  out << "      \"diagnostics_warnings\": " << summary.diagnostics_warnings
      << ",\n";
  out << "      \"diagnostics_errors\": " << summary.diagnostics_errors
      << ",\n";
  out << "      \"diagnostics_fatals\": " << summary.diagnostics_fatals << "\n";
  out << "    }";
  if (trailing_comma) {
    out << ",";
  }
  out << "\n";
}

std::string BuildFrontendCApiRunnerStageTraceJson(
    const objc3c_frontend_c_compile_result_t &result) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"mode\": \"objc3c-frontend-stage-trace-v1\",\n";
  out << "  \"semantic_skipped\": "
      << (result.semantic_skipped != 0 ? "true" : "false") << ",\n";
  out << "  \"process_exit_code\": " << result.process_exit_code << ",\n";
  out << "  \"stages\": {\n";
  WriteFrontendCApiRunnerStageSummaryJson(out, "lex", result.lex, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "parse", result.parse, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "sema", result.sema, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "lower", result.lower, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "emit", result.emit, false);
  out << "  }\n";
  out << "}\n";
  return out.str();
}
