#include "tools/objc3c_frontend_c_api_runner_result_contract_artifacts_internal.h"

bool ValidateFrontendCApiResultOwnedArtifactRequirementContract(
    const FrontendCApiRunnerResultArtifactSnapshot &snapshot,
    const FrontendCApiRunnerCArtifactRequirement &requirement,
    std::string &reason) {
  if (snapshot.path.present && snapshot.path.text.empty()) {
    reason = std::string("result-owned ") + requirement.name +
             " artifact path is present but empty";
    return false;
  }
  if (snapshot.produced != snapshot.path.present) {
    reason = std::string("result-owned ") + requirement.name +
             " artifact path presence disagrees with result_has_artifact";
    return false;
  }
  if (requirement.required_by_runner && !snapshot.produced) {
    reason = std::string("required result-owned ") + requirement.name +
             " artifact path is missing";
    return false;
  }
  return true;
}
