#include "tools/objc3c_frontend_c_api_runner_result_contract_status_internal.h"

#include "tools/objc3c_frontend_c_api_runner_c_string.h"

bool ValidateFrontendCApiResultLatestErrorAccessorSnapshot(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason) {
  const FrontendCApiRunnerStringSnapshot latest_result_error =
      FrontendCApiResultErrorMessageSnapshot(result);
  if (latest_result_error.present != result_error_message.present ||
      latest_result_error.text != result_error_message.text) {
    reason = "result-owned error_message snapshot differs from accessor text";
    return false;
  }
  return true;
}
