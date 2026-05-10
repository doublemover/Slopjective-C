#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

#include <cstddef>

#include "tools/objc3c_frontend_c_api_runner_public_result_ownership_artifact_contract.h"

FrontendCApiRunnerCOwnershipView BuildFrontendCApiRunnerCOwnershipView(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerStringSnapshot &result_error_message) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  FrontendCApiRunnerCOwnershipView ownership;
  ownership.error_message.present = result_error_message.present;
  ownership.error_message.null_contract =
      ok_status ? "must-be-null-on-success"
                : "must-be-present-and-nonempty-on-failure";
  const std::array<FrontendCApiRunnerCArtifactRequirement, 5> requirements =
      BuildFrontendCApiRunnerCArtifactRequirements(options, status);
  for (std::size_t index = 0; index < requirements.size(); ++index) {
    ownership.artifacts[index] =
        BuildFrontendCApiRunnerCArtifactOwnershipContract(
            result,
            requirements[index]);
  }
  return ownership;
}
