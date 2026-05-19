#include "tools/objc3c_frontend_c_api_runner_result_contract_status_internal.h"

bool ValidateFrontendCApiResultContextResultErrorParity(
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason) {
  if (!last_error.empty() && !result_error_message.text.empty() &&
      last_error != result_error_message.text) {
    reason = "context last_error and result-owned error_message differ";
    return false;
  }
  return true;
}
