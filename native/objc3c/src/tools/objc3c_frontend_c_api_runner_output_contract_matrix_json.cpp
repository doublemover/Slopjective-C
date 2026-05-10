#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

void WriteFrontendCApiRunnerOutputContractMatrixJson(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  WriteFrontendCApiRunnerOutputContractMatrixKeyJsonRows(out, indent, matrix);
  WriteFrontendCApiRunnerOutputContractMatrixPathIdentityJsonRows(
      out,
      indent,
      matrix);
  WriteFrontendCApiRunnerOutputContractMatrixCoreJsonRows(out, indent, matrix);
  WriteFrontendCApiRunnerOutputContractMatrixPathCompatibilityJsonRows(
      out,
      indent,
      matrix);
  WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseContractJsonRows(
      out,
      indent,
      matrix);
  WriteFrontendCApiRunnerOutputContractMatrixPathBudgetJsonRows(
      out,
      indent,
      matrix);
  WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseRobustnessJsonRows(
      out,
      indent,
      matrix);
  WriteFrontendCApiRunnerOutputContractMatrixHardeningJsonRows(
      out,
      indent,
      matrix);
  WriteFrontendCApiRunnerOutputContractMatrixReadinessJsonRows(
      out,
      indent,
      matrix);
}
