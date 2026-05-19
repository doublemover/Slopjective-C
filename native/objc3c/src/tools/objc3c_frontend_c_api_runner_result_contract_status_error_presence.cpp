#include "tools/objc3c_frontend_c_api_runner_result_contract_status_internal.h"

bool ValidateFrontendCApiResultErrorMessagePresenceRules(
    bool ok_status,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason) {
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
  return true;
}
