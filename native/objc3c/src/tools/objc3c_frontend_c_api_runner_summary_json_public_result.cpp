#include "tools/objc3c_frontend_c_api_runner_summary_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_public_result_json.h"

void WriteFrontendCApiRunnerSummaryPublicResultFields(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerPublicResultView &public_result) {
  WriteFrontendCApiRunnerPublicResultSummaryFields(
      out,
      options,
      public_result);
}
