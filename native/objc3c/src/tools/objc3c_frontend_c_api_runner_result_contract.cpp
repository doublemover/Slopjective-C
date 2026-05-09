#include "tools/objc3c_frontend_c_api_runner_result_contract.h"

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

bool ValidateFrontendCApiResultAccessors(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  if (result.status != status) {
    reason = "compile status does not match result.status";
    return false;
  }
  if (ok_status && result.success == 0u) {
    reason = "successful compile did not set result.success";
    return false;
  }
  if (!ok_status && result.success != 0u) {
    reason = "failing compile left result.success set";
    return false;
  }
  if (ok_status && result_error_message.present) {
    reason = "successful compile published a result-owned error message";
    return false;
  }
  if (ok_status && !last_error.empty()) {
    reason = "successful compile published a context last_error";
    return false;
  }
  if (!ok_status && !result_error_message.present) {
    reason = "failing compile published no result-owned error message";
    return false;
  }
  if (result_error_message.present && result_error_message.text.empty()) {
    reason = "result-owned error message is present but empty";
    return false;
  }
  const FrontendCApiRunnerStringSnapshot latest_result_error =
      FrontendCApiResultErrorMessageSnapshot(result);
  if (latest_result_error.present != result_error_message.present ||
      latest_result_error.text != result_error_message.text) {
    reason = "result-owned error_message snapshot differs from accessor text";
    return false;
  }
  if (!last_error.empty() && !result_error_message.text.empty() &&
      last_error != result_error_message.text) {
    reason = "context last_error and result-owned error_message differ";
    return false;
  }
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
