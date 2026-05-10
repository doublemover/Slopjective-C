#include "tools/objc3c_frontend_c_api_runner_summary_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_public_result_json.h"

FrontendCApiRunnerPublicResultView BuildFrontendCApiRunnerSummaryPublicResult(
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &artifact_paths,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message) {
  return BuildFrontendCApiRunnerPublicResultView(
      options,
      artifact_paths,
      status,
      result,
      last_error,
      result_error_message);
}

void WriteFrontendCApiRunnerSummaryPublicResultFields(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerPublicResultView &public_result) {
  WriteFrontendCApiRunnerPublicResultSummaryFields(
      out,
      options,
      public_result);
}
