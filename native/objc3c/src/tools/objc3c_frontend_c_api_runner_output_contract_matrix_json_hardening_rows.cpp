#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

void WriteFrontendCApiRunnerOutputContractMatrixHardeningJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"diagnostics_hardening_consistent\": "
      << (matrix.diagnostics_hardening_consistent ? "true" : "false")
      << ",\n";
  out << indent << "\"diagnostics_hardening_ready\": "
      << (matrix.diagnostics_hardening_ready ? "true" : "false") << ",\n";
  out << indent << "\"recovery_determinism_consistent\": "
      << (matrix.recovery_determinism_consistent ? "true" : "false") << ",\n";
  out << indent << "\"recovery_determinism_ready\": "
      << (matrix.recovery_determinism_ready ? "true" : "false") << ",\n";
}
