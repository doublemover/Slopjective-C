#include "tools/objc3c_frontend_c_api_runner_output_contract_conformance_report_matrix.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractConformanceMatrix(
    const FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &conformance_matrix,
    std::string &error) {
  conformance_matrix =
      BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(
          hardening_surfaces.recovery_determinism);
  std::string conformance_matrix_reason;
  if (!IsObjc3CliReportingOutputContractConformanceMatrixImplementationSurfaceReady(
          conformance_matrix,
          conformance_matrix_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "conformance matrix implementation", conformance_matrix_reason, error);
  }

  return true;
}
