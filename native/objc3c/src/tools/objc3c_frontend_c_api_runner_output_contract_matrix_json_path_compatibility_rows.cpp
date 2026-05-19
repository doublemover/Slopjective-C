#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

void WriteFrontendCApiRunnerOutputContractMatrixPathCompatibilityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"summary_output_extension_compatible\": "
      << (matrix.summary_output_extension_compatible ? "true" : "false")
      << ",\n";
  out << indent << "\"diagnostics_output_suffix_compatible\": "
      << (matrix.diagnostics_output_suffix_compatible ? "true" : "false")
      << ",\n";
  out << indent << "\"case_folded_paths_distinct\": "
      << (matrix.case_folded_paths_distinct ? "true" : "false") << ",\n";
  out << indent << "\"output_paths_control_char_free\": "
      << (matrix.output_paths_control_char_free ? "true" : "false") << ",\n";
}
