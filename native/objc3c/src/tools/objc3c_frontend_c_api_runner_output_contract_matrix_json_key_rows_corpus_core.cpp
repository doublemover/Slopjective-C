#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_key_rows_internal.h"

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractMatrixCorpusCoreKeyJsonRows(
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
}
