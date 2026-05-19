#pragma once

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_result_error_snapshot.h"
#include "tools/objc3c_frontend_c_api_runner_result_owner.h"

struct FrontendCApiRunnerCompileSession {
  FrontendCApiCompileResultOwner compile_result;
  objc3c_frontend_c_status_t status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  FrontendCApiRunnerResultErrorSnapshot error_snapshot;
  int exit_code = 2;

  FrontendCApiRunnerCompileSession();
  FrontendCApiRunnerCompileSession(const FrontendCApiRunnerCompileSession &) =
      delete;
  FrontendCApiRunnerCompileSession &operator=(
      const FrontendCApiRunnerCompileSession &) = delete;
};
