#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_edge_case.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_edge_case_internal.h"

bool BuildFrontendCApiRunnerOutputContractEdgeCaseSurfaces(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error) {
  if (!BuildFrontendCApiRunnerOutputContractEdgeCaseSetup(
          core_surfaces,
          hardening_surfaces,
          error)) {
    return false;
  }
  if (!BuildFrontendCApiRunnerOutputContractEdgeCaseHardeningExpectations(
          hardening_surfaces,
          error)) {
    return false;
  }

  return true;
}
