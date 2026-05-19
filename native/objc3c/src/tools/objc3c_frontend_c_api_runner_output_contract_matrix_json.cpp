#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_internal.h"

void WriteFrontendCApiRunnerOutputContractMatrixJson(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  WriteFrontendCApiRunnerOutputContractMatrixJsonRowGroups(out,
                                                           indent,
                                                           matrix);
}
