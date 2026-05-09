#include "tools/objc3c_frontend_c_api_runner_output_contract_core.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"
#include "tools/objc3c_frontend_c_api_runner_output_paths.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

bool BuildFrontendCApiRunnerOutputContractCoreSurfaces(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::string &error) {
  const bool stage_report_output_contract_ready =
      FrontendCApiStageReportShapeReady(result);
  surfaces.scaffold = BuildObjc3CliReportingOutputContractScaffold(
      options.out_dir,
      options.emit_prefix,
      summary_path,
      stage_report_output_contract_ready);
  std::string scaffold_reason;
  if (!IsObjc3CliReportingOutputContractScaffoldReady(surfaces.scaffold,
                                                      scaffold_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "scaffold", scaffold_reason, error);
  }

  std::filesystem::path diagnostics_output_path;
  if (!ResolveFrontendCApiRunnerDiagnosticsOutputPath(
          options,
          result,
          diagnostics_output_path,
          error)) {
    return false;
  }
  surfaces.core_feature = BuildObjc3CliReportingOutputContractCoreFeatureSurface(
      surfaces.scaffold,
      summary_path,
      diagnostics_output_path);
  std::string core_feature_reason;
  if (!IsObjc3CliReportingOutputContractCoreFeatureSurfaceReady(
          surfaces.core_feature,
          core_feature_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "core feature", core_feature_reason, error);
  }

  surfaces.core_feature_expansion =
      BuildObjc3CliReportingOutputContractCoreFeatureExpansionSurface(
          surfaces.core_feature,
          options.emit_prefix,
          summary_path,
          diagnostics_output_path);
  std::string core_feature_expansion_reason;
  if (!IsObjc3CliReportingOutputContractCoreFeatureExpansionSurfaceReady(
          surfaces.core_feature_expansion,
          core_feature_expansion_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "core feature expansion", core_feature_expansion_reason, error);
  }
  error.clear();
  return true;
}
