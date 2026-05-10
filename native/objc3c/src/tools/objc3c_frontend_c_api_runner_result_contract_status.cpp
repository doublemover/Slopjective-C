#include "tools/objc3c_frontend_c_api_runner_result_contract_status.h"

#include "tools/objc3c_frontend_c_api_runner_result_contract_status_internal.h"

bool ValidateFrontendCApiResultStatusAndErrorAccessors(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  if (!ValidateFrontendCApiResultStatusSuccessConsistency(
          status,
          result,
          ok_status,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultErrorMessagePresenceRules(
          ok_status,
          last_error,
          result_error_message,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultLatestErrorAccessorSnapshot(
          result,
          result_error_message,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultContextResultErrorParity(
          last_error,
          result_error_message,
          reason)) {
    return false;
  }
  return true;
}
