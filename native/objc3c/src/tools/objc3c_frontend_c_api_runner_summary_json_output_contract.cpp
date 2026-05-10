#include "tools/objc3c_frontend_c_api_runner_summary_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_json.h"

void WriteFrontendCApiRunnerSummaryOutputContractSection(
    std::ostream &out,
    const FrontendCApiRunnerOutputContract &output_contract) {
  out << "  \"output_contract\": ";
  WriteFrontendCApiRunnerOutputContractJson(out, "  ", output_contract);
  out << "\n";
}
