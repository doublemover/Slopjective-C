#include "tools/objc3c_frontend_c_api_runner_output_contract.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_core.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening.h"

bool BuildFrontendCApiRunnerOutputContract(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error) {
  FrontendCApiRunnerOutputContractCoreSurfaces core_surfaces;
  if (!BuildFrontendCApiRunnerOutputContractCoreSurfaces(
          options,
          summary_path,
          result,
          core_surfaces,
          error)) {
    return false;
  }
  return BuildFrontendCApiRunnerOutputContractHardeningSurfaces(
      core_surfaces,
      output_contract,
      error);
}
