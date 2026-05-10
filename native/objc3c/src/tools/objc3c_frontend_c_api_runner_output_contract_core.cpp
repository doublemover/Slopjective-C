#include "tools/objc3c_frontend_c_api_runner_output_contract_core.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_core_feature.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_core_feature_expansion.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_core_scaffold.h"

bool BuildFrontendCApiRunnerOutputContractCoreSurfaces(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::string &error) {
  if (!BuildFrontendCApiRunnerOutputContractScaffoldSurface(
          options,
          summary_path,
          result,
          surfaces,
          error)) {
    return false;
  }

  std::filesystem::path diagnostics_output_path;
  if (!BuildFrontendCApiRunnerOutputContractCoreFeatureSurface(
          options,
          summary_path,
          result,
          surfaces,
          diagnostics_output_path,
          error)) {
    return false;
  }
  if (!BuildFrontendCApiRunnerOutputContractCoreFeatureExpansionSurface(
          options,
          summary_path,
          diagnostics_output_path,
          surfaces,
          error)) {
    return false;
  }

  error.clear();
  return true;
}
