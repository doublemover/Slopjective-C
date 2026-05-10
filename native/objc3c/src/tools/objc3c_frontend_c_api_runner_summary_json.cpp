#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_summary_json_sections.h"

std::string BuildFrontendCApiRunnerSummaryJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerPublicResultView &public_result,
    const FrontendCApiRunnerOutputContract &output_contract) {
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
      public_result);
  WriteFrontendCApiRunnerSummaryOutputContractSection(out, output_contract);
  out << "}\n";
  return out.str();
}
