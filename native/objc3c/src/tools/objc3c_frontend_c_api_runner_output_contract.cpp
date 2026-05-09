#include "tools/objc3c_frontend_c_api_runner_output_contract.h"

#include "io/objc3_cli_reporting_output_contract_core_feature_expansion_surface.h"
#include "io/objc3_cli_reporting_output_contract_core_feature_surface.h"
#include "io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_compatibility_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h"
#include "io/objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h"
#include "io/objc3_cli_reporting_output_contract_scaffold.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

namespace {

bool FailOutputContract(const char *boundary,
                        const std::string &reason,
                        std::string &error) {
  error = "cli/reporting output ";
  error += boundary;
  error += " fail-closed: ";
  error += reason;
  return false;
}

}  // namespace

bool BuildFrontendCApiRunnerOutputContract(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error) {
  const bool stage_report_output_contract_ready =
      FrontendCApiStageReportShapeReady(result);
  const Objc3CliReportingOutputContractScaffold scaffold =
      BuildObjc3CliReportingOutputContractScaffold(
          options.out_dir,
          options.emit_prefix,
          summary_path,
          stage_report_output_contract_ready);
  std::string scaffold_reason;
  if (!IsObjc3CliReportingOutputContractScaffoldReady(scaffold,
                                                      scaffold_reason)) {
    return FailOutputContract("scaffold", scaffold_reason, error);
  }

  const std::string diagnostics_path_text =
      FrontendCApiResultArtifactPath(result,
                                     OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  const std::filesystem::path diagnostics_output_path =
      diagnostics_path_text.empty()
          ? (options.out_dir / (options.emit_prefix + ".diagnostics.json"))
          : std::filesystem::path(diagnostics_path_text);
  const Objc3CliReportingOutputContractCoreFeatureSurface core_feature =
      BuildObjc3CliReportingOutputContractCoreFeatureSurface(
          scaffold,
          summary_path,
          diagnostics_output_path);
  std::string core_feature_reason;
  if (!IsObjc3CliReportingOutputContractCoreFeatureSurfaceReady(
          core_feature,
          core_feature_reason)) {
    return FailOutputContract("core feature", core_feature_reason, error);
  }

  const Objc3CliReportingOutputContractCoreFeatureExpansionSurface
      core_feature_expansion =
          BuildObjc3CliReportingOutputContractCoreFeatureExpansionSurface(
              core_feature,
              options.emit_prefix,
              summary_path,
              diagnostics_output_path);
  std::string core_feature_expansion_reason;
  if (!IsObjc3CliReportingOutputContractCoreFeatureExpansionSurfaceReady(
          core_feature_expansion,
          core_feature_expansion_reason)) {
    return FailOutputContract("core feature expansion",
                              core_feature_expansion_reason,
                              error);
  }

  const Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface
      edge_case_compatibility =
          BuildObjc3CliReportingOutputContractEdgeCaseCompatibilitySurface(
              core_feature_expansion);
  std::string edge_case_compatibility_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseCompatibilitySurfaceReady(
          edge_case_compatibility,
          edge_case_compatibility_reason)) {
    return FailOutputContract("edge-case compatibility",
                              edge_case_compatibility_reason,
                              error);
  }

  const Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
      edge_case_robustness =
          BuildObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface(
              edge_case_compatibility);
  std::string edge_case_robustness_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurfaceReady(
          edge_case_robustness,
          edge_case_robustness_reason)) {
    return FailOutputContract("edge-case expansion and robustness",
                              edge_case_robustness_reason,
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
    return FailOutputContract("diagnostics hardening",
                              diagnostics_hardening_reason,
                              error);
  }

  const Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
      recovery_determinism =
          BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurface(
              diagnostics_hardening);
  std::string recovery_determinism_reason;
  if (!IsObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurfaceReady(
          recovery_determinism,
          recovery_determinism_reason)) {
    return FailOutputContract("recovery and determinism hardening",
                              recovery_determinism_reason,
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
    return FailOutputContract("conformance matrix implementation",
                              conformance_matrix_reason,
                              error);
  }

  const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
      conformance_corpus =
          BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(
              conformance_matrix);
  std::string conformance_corpus_reason;
  if (!IsObjc3CliReportingOutputContractConformanceCorpusExpansionSurfaceReady(
          conformance_corpus,
          conformance_corpus_reason)) {
    return FailOutputContract("conformance corpus expansion",
                              conformance_corpus_reason,
                              error);
  }

  output_contract.conformance_matrix_surface = conformance_matrix;
  output_contract.conformance_corpus_surface = conformance_corpus;
  error.clear();
  return true;
}
