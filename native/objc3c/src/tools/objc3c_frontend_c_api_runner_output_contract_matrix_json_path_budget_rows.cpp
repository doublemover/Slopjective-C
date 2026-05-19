#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

void WriteFrontendCApiRunnerOutputContractMatrixPathBudgetJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"summary_output_parent_present\": "
      << (matrix.summary_output_parent_present ? "true" : "false") << ",\n";
  out << indent << "\"diagnostics_output_parent_present\": "
      << (matrix.diagnostics_output_parent_present ? "true" : "false")
      << ",\n";
  out << indent << "\"output_paths_within_length_budget\": "
      << (matrix.output_paths_within_length_budget ? "true" : "false")
      << ",\n";
  out << indent << "\"output_paths_no_trailing_space\": "
      << (matrix.output_paths_no_trailing_space ? "true" : "false") << ",\n";
}
