#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_key_rows_internal.h"

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractMatrixHardeningReadinessKeyJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"diagnostics_hardening_key\": \""
      << EscapeJsonString(matrix.diagnostics_hardening_key) << "\",\n";
  out << indent << "\"recovery_determinism_key\": \""
      << EscapeJsonString(matrix.recovery_determinism_key) << "\",\n";
  out << indent << "\"conformance_matrix_key\": \""
      << EscapeJsonString(matrix.conformance_matrix_key) << "\",\n";
}
