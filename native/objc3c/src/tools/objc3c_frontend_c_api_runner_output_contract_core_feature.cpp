#include "tools/objc3c_frontend_c_api_runner_output_contract_core_feature.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"
#include "tools/objc3c_frontend_c_api_runner_output_paths.h"

bool BuildFrontendCApiRunnerOutputContractCoreFeatureSurface(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::filesystem::path &diagnostics_output_path,
    std::string &error) {
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

  return true;
}
