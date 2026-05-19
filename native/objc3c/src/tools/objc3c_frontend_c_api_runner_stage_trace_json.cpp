#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_stage_trace_json_rows.h"

std::string BuildFrontendCApiRunnerStageTraceJson(
    const objc3c_frontend_c_compile_result_t &result) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"mode\": \"objc3c-frontend-stage-trace-v1\",\n";
  out << "  \"semantic_skipped\": "
      << (result.semantic_skipped != 0 ? "true" : "false") << ",\n";
  out << "  \"process_exit_code\": " << result.process_exit_code << ",\n";
  out << "  \"stages\": {\n";
  WriteFrontendCApiRunnerCompileSessionStageTraceRows(out, result);
  WriteFrontendCApiRunnerArtifactStageTraceRows(out, result);
  out << "  }\n";
  out << "}\n";
  return out.str();
}
