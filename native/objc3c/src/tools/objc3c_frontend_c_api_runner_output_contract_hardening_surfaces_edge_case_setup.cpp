#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces_edge_case_internal.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractEdgeCaseSetup(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error) {
  hardening_surfaces.edge_case_consistency =
      BuildObjc3CliReportingOutputContractEdgeCaseConsistencySurface(
          core_surfaces.core_feature_expansion);
  std::string edge_case_consistency_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseConsistencySurfaceReady(
          hardening_surfaces.edge_case_consistency,
          edge_case_consistency_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "edge-case consistency", edge_case_consistency_reason, error);
  }

  return true;
}
