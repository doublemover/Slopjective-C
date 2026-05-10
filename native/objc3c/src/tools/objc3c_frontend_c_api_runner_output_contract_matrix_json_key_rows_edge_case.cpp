#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_key_rows_internal.h"

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseKeyJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"edge_case_consistency_contract_key\": \""
      << EscapeJsonString(matrix.edge_case_consistency_contract_key) << "\",\n";
  out << indent << "\"edge_case_robustness_key\": \""
      << EscapeJsonString(matrix.edge_case_robustness_key) << "\",\n";
}
