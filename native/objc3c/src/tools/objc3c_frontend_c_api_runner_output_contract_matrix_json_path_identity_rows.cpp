#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractMatrixPathIdentityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"summary_output_path\": \""
      << EscapeJsonString(matrix.summary_output_path) << "\",\n";
  out << indent << "\"diagnostics_output_path\": \""
      << EscapeJsonString(matrix.diagnostics_output_path) << "\",\n";
  out << indent << "\"summary_output_path_contract_consistent\": "
      << (matrix.summary_output_path_contract_consistent ? "true" : "false")
      << ",\n";
  out << indent << "\"diagnostics_output_path_contract_consistent\": "
      << (matrix.diagnostics_output_path_contract_consistent ? "true"
                                                             : "false")
      << ",\n";
  out << indent << "\"diagnostics_filename_matches_emit_prefix\": "
      << (matrix.diagnostics_filename_matches_emit_prefix ? "true" : "false")
      << ",\n";
}
