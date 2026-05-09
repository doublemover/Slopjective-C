#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

struct FrontendCApiRunnerCompileSession {
  objc3c_frontend_c_compile_result_t result = {};
  FrontendCApiCompileResultGuard result_guard;
  objc3c_frontend_c_status_t status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  std::string last_error;
  FrontendCApiRunnerStringSnapshot result_error_message_snapshot;
  std::string result_error_message;
  int exit_code = 2;

  FrontendCApiRunnerCompileSession();
  FrontendCApiRunnerCompileSession(const FrontendCApiRunnerCompileSession &) =
      delete;
  FrontendCApiRunnerCompileSession &operator=(
      const FrontendCApiRunnerCompileSession &) = delete;
};
