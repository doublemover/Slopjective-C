#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractMatrixKeyJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"scaffold_key\": \""
      << EscapeJsonString(matrix.scaffold_key) << "\",\n";
  out << indent << "\"core_feature_key\": \""
      << EscapeJsonString(matrix.core_feature_key) << "\",\n";
  out << indent << "\"core_feature_expansion_key\": \""
      << EscapeJsonString(matrix.core_feature_expansion_key) << "\",\n";
  out << indent << "\"edge_case_consistency_contract_key\": \""
      << EscapeJsonString(matrix.edge_case_consistency_contract_key) << "\",\n";
  out << indent << "\"edge_case_robustness_key\": \""
      << EscapeJsonString(matrix.edge_case_robustness_key) << "\",\n";
  out << indent << "\"diagnostics_hardening_key\": \""
      << EscapeJsonString(matrix.diagnostics_hardening_key) << "\",\n";
  out << indent << "\"recovery_determinism_key\": \""
      << EscapeJsonString(matrix.recovery_determinism_key) << "\",\n";
  out << indent << "\"conformance_matrix_key\": \""
      << EscapeJsonString(matrix.conformance_matrix_key) << "\",\n";
}
