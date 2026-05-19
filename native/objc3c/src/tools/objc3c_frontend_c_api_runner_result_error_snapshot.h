#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_string_snapshot.h"

struct FrontendCApiRunnerResultErrorSnapshot {
  std::string last_error;
  FrontendCApiRunnerStringSnapshot result_error_message;
};

FrontendCApiRunnerResultErrorSnapshot
CaptureFrontendCApiRunnerResultErrorSnapshot(
    const objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_result_t &result);
