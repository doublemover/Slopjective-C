#include "tools/objc3c_frontend_c_api_runner_output_contract_core_feature_expansion.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractCoreFeatureExpansionSurface(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const std::filesystem::path &diagnostics_output_path,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::string &error) {
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

  return true;
}
