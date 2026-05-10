#include "tools/objc3c_frontend_c_api_runner_result_contract.h"

#include "tools/objc3c_frontend_c_api_runner_result_contract_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_result_contract_status.h"

bool ValidateFrontendCApiResultAccessors(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason) {
  if (!ValidateFrontendCApiResultStatusAndErrorAccessors(
          status,
          result,
          last_error,
          result_error_message,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultOwnedArtifactPaths(
          options,
          status,
          result,
          reason)) {
    return false;
  }
  return true;
}
