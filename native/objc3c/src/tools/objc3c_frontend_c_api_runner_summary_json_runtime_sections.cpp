#include "tools/objc3c_frontend_c_api_runner_summary_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"

void WriteFrontendCApiRunnerSummaryObservabilityRuntimeBonusSections(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerPublicResultView &public_result) {
  out << "  \"observability\": ";
  WriteFrontendCApiRunnerObservabilityJson(
      out,
      "  ",
      public_result.paths.summary,
      result,
      status,
      public_result.diagnostics.result_error_message,
      public_result.paths.runtime_metadata_binary);
  out << ",\n";
  out << "  \"runtime_inspector\": ";
  WriteFrontendCApiRunnerRuntimeInspectorJson(out, "  ", options, result);
  out << ",\n";
  out << "  \"bonus_experiences\": ";
  WriteFrontendCApiRunnerBonusExperiencesJson(
      out,
      "  ",
      options,
      result,
      public_result.paths.summary,
      public_result.paths.runtime_metadata_binary);
  out << ",\n";
}
