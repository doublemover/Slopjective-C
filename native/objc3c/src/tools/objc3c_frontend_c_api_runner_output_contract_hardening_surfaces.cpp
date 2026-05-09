#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractHardeningSurfaceChain(
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

  error.clear();
  return true;
}
