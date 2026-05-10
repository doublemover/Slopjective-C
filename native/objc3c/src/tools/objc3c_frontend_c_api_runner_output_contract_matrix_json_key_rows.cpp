#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_key_rows_internal.h"

void WriteFrontendCApiRunnerOutputContractMatrixKeyJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  WriteFrontendCApiRunnerOutputContractMatrixCorpusCoreKeyJsonRows(out,
                                                                   indent,
                                                                   matrix);
  WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseKeyJsonRows(out,
                                                                 indent,
                                                                 matrix);
  WriteFrontendCApiRunnerOutputContractMatrixHardeningReadinessKeyJsonRows(
      out,
      indent,
      matrix);
}
