#include "tools/objc3c_frontend_c_api_runner_status_mapping.h"

const char *FrontendCApiStatusName(objc3c_frontend_c_status_t status) {
  switch (status) {
    case OBJC3C_FRONTEND_STATUS_OK:
      return "ok";
    case OBJC3C_FRONTEND_STATUS_DIAGNOSTICS:
      return "diagnostics";
    case OBJC3C_FRONTEND_STATUS_USAGE_ERROR:
      return "usage-error";
    case OBJC3C_FRONTEND_STATUS_EMIT_ERROR:
      return "emit-error";
    case OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR:
      return "internal-error";
    default:
      return "unknown";
  }
}

int FrontendCApiExitCodeFromStatus(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result) {
  switch (status) {
    case OBJC3C_FRONTEND_STATUS_OK:
      return 0;
    case OBJC3C_FRONTEND_STATUS_DIAGNOSTICS:
      return 1;
    case OBJC3C_FRONTEND_STATUS_USAGE_ERROR:
      return 2;
    case OBJC3C_FRONTEND_STATUS_EMIT_ERROR:
      return result.process_exit_code != 0 ? result.process_exit_code : 3;
    case OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR:
    default:
      return result.process_exit_code != 0 ? result.process_exit_code : 2;
  }
}
