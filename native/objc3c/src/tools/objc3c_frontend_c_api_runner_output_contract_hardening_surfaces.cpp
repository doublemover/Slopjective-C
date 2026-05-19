#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_diagnostics.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_edge_case.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_recovery_determinism.h"

bool BuildFrontendCApiRunnerOutputContractHardeningSurfaceChain(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error) {
  if (!BuildFrontendCApiRunnerOutputContractEdgeCaseSurfaces(
          core_surfaces,
          hardening_surfaces,
          error)) {
    return false;
  }
  if (!BuildFrontendCApiRunnerOutputContractDiagnosticsHardeningSurface(
          hardening_surfaces,
          error)) {
    return false;
  }
  if (!BuildFrontendCApiRunnerOutputContractRecoveryDeterminismSurface(
          hardening_surfaces,
          error)) {
    return false;
  }

  error.clear();
  return true;
}
