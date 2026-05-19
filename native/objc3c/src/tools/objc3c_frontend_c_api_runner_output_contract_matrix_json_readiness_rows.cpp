#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

void WriteFrontendCApiRunnerOutputContractMatrixCoreJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"core_feature_expansion_ready\": "
      << (matrix.core_feature_expansion_ready ? "true" : "false") << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixReadinessJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"conformance_matrix_consistent\": "
      << (matrix.conformance_matrix_consistent ? "true" : "false") << ",\n";
  out << indent << "\"conformance_matrix_ready\": "
      << (matrix.conformance_matrix_ready ? "true" : "false") << ",\n";
  out << indent << "\"conformance_matrix_key_ready\": "
      << (matrix.conformance_matrix_key_ready ? "true" : "false") << ",\n";
}
