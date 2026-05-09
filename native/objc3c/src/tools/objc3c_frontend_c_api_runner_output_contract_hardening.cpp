#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening.h"

#include "io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_consistency_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h"
#include "io/objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractHardeningSurfaces(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error) {
  const Objc3CliReportingOutputContractEdgeCaseConsistencySurface
      edge_case_consistency =
          BuildObjc3CliReportingOutputContractEdgeCaseConsistencySurface(
              core_surfaces.core_feature_expansion);
  std::string edge_case_consistency_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseConsistencySurfaceReady(
          edge_case_consistency,
          edge_case_consistency_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "edge-case consistency", edge_case_consistency_reason, error);
  }

  const Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
      edge_case_robustness =
          BuildObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface(
              edge_case_consistency);
  std::string edge_case_robustness_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurfaceReady(
          edge_case_robustness,
          edge_case_robustness_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "edge-case expansion and robustness", edge_case_robustness_reason,
        error);
  }

  const Objc3CliReportingOutputContractDiagnosticsHardeningSurface
      diagnostics_hardening =
          BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(
              edge_case_robustness);
  std::string diagnostics_hardening_reason;
  if (!IsObjc3CliReportingOutputContractDiagnosticsHardeningSurfaceReady(
          diagnostics_hardening,
          diagnostics_hardening_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "diagnostics hardening", diagnostics_hardening_reason, error);
  }

  const Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
      recovery_determinism =
          BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurface(
              diagnostics_hardening);
  std::string recovery_determinism_reason;
  if (!IsObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurfaceReady(
          recovery_determinism,
          recovery_determinism_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "recovery and determinism hardening", recovery_determinism_reason,
        error);
  }

  const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
      conformance_matrix =
          BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(
              recovery_determinism);
  std::string conformance_matrix_reason;
  if (!IsObjc3CliReportingOutputContractConformanceMatrixImplementationSurfaceReady(
          conformance_matrix,
          conformance_matrix_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "conformance matrix implementation", conformance_matrix_reason, error);
  }

  const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
      conformance_corpus =
          BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(
              conformance_matrix);
  std::string conformance_corpus_reason;
  if (!IsObjc3CliReportingOutputContractConformanceCorpusExpansionSurfaceReady(
          conformance_corpus,
          conformance_corpus_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "conformance corpus expansion", conformance_corpus_reason, error);
  }

  output_contract.conformance_matrix_surface = conformance_matrix;
  output_contract.conformance_corpus_surface = conformance_corpus;
  error.clear();
  return true;
}
