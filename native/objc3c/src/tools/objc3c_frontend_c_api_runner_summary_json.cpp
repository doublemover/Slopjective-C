#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_summary_json_sections.h"

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
      BuildFrontendCApiRunnerSummaryPublicResult(
          options,
          artifact_paths,
          status,
          result,
          last_error,
          result_error_message);

  std::ostringstream out;
  out << "{\n";
  WriteFrontendCApiRunnerSummaryPublicResultFields(
      out,
      options,
      public_result);
  WriteFrontendCApiRunnerSummaryStageBlock(out, result);
  WriteFrontendCApiRunnerSummaryObservabilityRuntimeBonusSections(
      out,
      options,
      result,
      status,
      result_error_message,
      public_result);
  WriteFrontendCApiRunnerSummaryOutputContractSection(out, output_contract);
  out << "}\n";
  return out.str();
}
