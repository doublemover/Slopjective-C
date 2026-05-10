#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_edge_case_internal.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractEdgeCaseHardeningExpectations(
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error) {
  hardening_surfaces.edge_case_robustness =
      BuildObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface(
          hardening_surfaces.edge_case_consistency);
  std::string edge_case_robustness_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurfaceReady(
          hardening_surfaces.edge_case_robustness,
          edge_case_robustness_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "edge-case expansion and robustness", edge_case_robustness_reason,
        error);
  }

  return true;
}
