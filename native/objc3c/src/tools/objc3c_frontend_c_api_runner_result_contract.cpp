#include "tools/objc3c_frontend_c_api_runner_result_contract.h"

bool ValidateFrontendCApiResultAccessors(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message,
    std::string &reason) {
  if (result.status != status) {
    reason = "compile status does not match result.status";
    return false;
  }
  if (status == OBJC3C_FRONTEND_STATUS_OK && result.success == 0u) {
    reason = "successful compile did not set result.success";
    return false;
  }
  if (status != OBJC3C_FRONTEND_STATUS_OK && result.success != 0u) {
    reason = "failing compile left result.success set";
    return false;
  }
  if (status == OBJC3C_FRONTEND_STATUS_OK && !result_error_message.empty()) {
    reason = "successful compile published a result-owned error message";
    return false;
  }
  if (status == OBJC3C_FRONTEND_STATUS_OK && !last_error.empty()) {
    reason = "successful compile published a context last_error";
    return false;
  }
  if (status != OBJC3C_FRONTEND_STATUS_OK && result_error_message.empty()) {
    reason = "failing compile published no result-owned error message";
    return false;
  }
  if (!last_error.empty() && !result_error_message.empty() &&
      last_error != result_error_message) {
    reason = "context last_error and result-owned error_message differ";
    return false;
  }
  return true;
}
