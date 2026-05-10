#include "tools/objc3c_frontend_c_api_runner_result_contract_artifacts.h"

#include "tools/objc3c_frontend_c_api_runner_c_string.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

namespace {

bool ValidateFrontendCApiResultOwnedArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerCArtifactRequirement &requirement,
    std::string &reason) {
  const FrontendCApiRunnerStringSnapshot path =
      FrontendCApiResultArtifactPathSnapshot(
          result,
          requirement.artifact_kind);
  const bool has_artifact =
      objc3c_frontend_c_result_has_artifact(&result,
                                            requirement.artifact_kind) != 0u;
  if (path.present && path.text.empty()) {
    reason = std::string("result-owned ") + requirement.name +
             " artifact path is present but empty";
    return false;
  }
  if (has_artifact != path.present) {
    reason = std::string("result-owned ") + requirement.name +
             " artifact path presence disagrees with result_has_artifact";
    return false;
  }
  if (requirement.required_by_runner && !has_artifact) {
    reason = std::string("required result-owned ") + requirement.name +
             " artifact path is missing";
    return false;
  }
  return true;
}

}  // namespace

bool ValidateFrontendCApiResultOwnedArtifactPaths(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    std::string &reason) {
  const std::array<FrontendCApiRunnerCArtifactRequirement, 5> requirements =
      BuildFrontendCApiRunnerCArtifactRequirements(options, status);
  for (const FrontendCApiRunnerCArtifactRequirement &requirement :
       requirements) {
    if (!ValidateFrontendCApiResultOwnedArtifactPath(
            result,
            requirement,
            reason)) {
      return false;
    }
  }
  return true;
}
