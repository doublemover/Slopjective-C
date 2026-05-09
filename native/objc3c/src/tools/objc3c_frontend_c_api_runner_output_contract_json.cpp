#include "tools/objc3c_frontend_c_api_runner_output_contract_json.h"

#include "io/objc3_cli_reporting_output_contract_scaffold.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_corpus_json.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json.h"

void WriteFrontendCApiRunnerOutputContractJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOutputContract &output_contract) {
  const std::string child_indent = indent + "  ";

  out << "{\n";
  out << child_indent << "\"diagnostics_schema_version\": \""
      << kObjc3CliReportingDiagnosticsSchemaVersion << "\",\n";
  out << child_indent << "\"summary_mode\": \""
      << kObjc3CliReportingSummaryMode << "\",\n";
  WriteFrontendCApiRunnerOutputContractMatrixJson(
      out,
      child_indent,
      output_contract.conformance_matrix_surface);
  WriteFrontendCApiRunnerOutputContractCorpusJson(
      out,
      child_indent,
      output_contract.conformance_corpus_surface);
  out << indent << "}";
}
