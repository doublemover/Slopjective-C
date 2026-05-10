#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_diagnostics.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractDiagnosticsHardeningSurface(
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error) {
  hardening_surfaces.diagnostics_hardening =
      BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(
          hardening_surfaces.edge_case_robustness);
  std::string diagnostics_hardening_reason;
  if (!IsObjc3CliReportingOutputContractDiagnosticsHardeningSurfaceReady(
          hardening_surfaces.diagnostics_hardening,
          diagnostics_hardening_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "diagnostics hardening", diagnostics_hardening_reason, error);
  }

  return true;
}
