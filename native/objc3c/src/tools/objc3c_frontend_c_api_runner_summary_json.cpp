#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_json.h"
#include "tools/objc3c_frontend_c_api_runner_public_result.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"
#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

namespace fs = std::filesystem;

std::string BuildFrontendCApiRunnerSummaryJson(
    const FrontendCApiRunnerOptions &options,
    const fs::path &summary_path,
    const FrontendCApiRunnerArtifactPathView &artifact_paths,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    const FrontendCApiRunnerOutputContract &output_contract) {
  const FrontendCApiRunnerPublicResultView public_result =
      BuildFrontendCApiRunnerPublicResultView(
          options,
          artifact_paths,
          status,
          result,
          last_error,
          result_error_message);

  std::ostringstream out;
  out << "{\n";
  WriteFrontendCApiRunnerPublicResultSummaryFields(
      out,
      options,
      public_result);
  out << "  \"stages\": {\n";
  WriteFrontendCApiRunnerStageSummaryJson(out, "lex", result.lex, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "parse", result.parse, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "sema", result.sema, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "lower", result.lower, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "emit", result.emit, false);
  out << "  },\n";
  out << "  \"observability\": ";
  WriteFrontendCApiRunnerObservabilityJson(
      out,
      "  ",
      public_result.paths.summary,
      result,
      status,
      result_error_message.text,
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
  out << "  \"output_contract\": ";
  WriteFrontendCApiRunnerOutputContractJson(out, "  ", output_contract);
  out << "\n";
  out << "}\n";
  return out.str();
}
