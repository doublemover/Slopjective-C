#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_recovery_determinism.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractRecoveryDeterminismSurface(
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error) {
  hardening_surfaces.recovery_determinism =
      BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurface(
          hardening_surfaces.diagnostics_hardening);
  std::string recovery_determinism_reason;
  if (!IsObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurfaceReady(
          hardening_surfaces.recovery_determinism,
          recovery_determinism_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "recovery and determinism hardening", recovery_determinism_reason,
        error);
  }

  return true;
}
