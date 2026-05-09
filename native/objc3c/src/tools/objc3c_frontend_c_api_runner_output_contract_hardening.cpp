#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_conformance_report.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces.h"

bool BuildFrontendCApiRunnerOutputContractHardeningSurfaces(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error) {
  FrontendCApiRunnerOutputContractHardeningSurfaces hardening_surfaces;
  if (!BuildFrontendCApiRunnerOutputContractHardeningSurfaceChain(
          core_surfaces,
          hardening_surfaces,
          error)) {
    return false;
  }
  return BuildFrontendCApiRunnerOutputContractConformanceReport(
      hardening_surfaces,
      output_contract,
      error);
}
