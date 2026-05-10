#include "tools/objc3c_frontend_c_api_runner_result_contract_artifacts.h"

#include "tools/objc3c_frontend_c_api_runner_result_contract_artifacts_internal.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

bool ValidateFrontendCApiResultOwnedArtifactPaths(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    std::string &reason) {
  const std::array<FrontendCApiRunnerCArtifactRequirement, 5> requirements =
      BuildFrontendCApiRunnerCArtifactRequirements(options, status);
  for (const FrontendCApiRunnerCArtifactRequirement &requirement :
       requirements) {
    const FrontendCApiRunnerResultArtifactPathProbe path_probe =
        ProbeFrontendCApiResultOwnedArtifactPath(result, requirement);
    const bool has_artifact = ProbeFrontendCApiResultOwnedArtifactAvailability(
        result,
        requirement.artifact_kind);
    if (!ValidateFrontendCApiResultOwnedArtifactRequirementContract(
            path_probe.path,
            has_artifact,
            requirement,
            reason)) {
      return false;
    }
  }
  return true;
}
