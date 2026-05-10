#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

void WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseRobustnessJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"edge_case_expansion_consistent\": "
      << (matrix.edge_case_expansion_consistent ? "true" : "false") << ",\n";
  out << indent << "\"edge_case_robustness_consistent\": "
      << (matrix.edge_case_robustness_consistent ? "true" : "false") << ",\n";
  out << indent << "\"edge_case_robustness_ready\": "
      << (matrix.edge_case_robustness_ready ? "true" : "false") << ",\n";
}
