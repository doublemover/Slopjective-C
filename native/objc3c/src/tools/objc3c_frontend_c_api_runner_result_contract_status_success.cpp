#include "tools/objc3c_frontend_c_api_runner_result_contract_status_internal.h"

bool ValidateFrontendCApiResultStatusSuccessConsistency(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    bool ok_status,
    std::string &reason) {
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
  return true;
}
