#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_result_error_snapshot.h"

struct FrontendCApiRunnerPublicResultDiagnostics {
  std::string last_error;
  std::string result_error_message;
  bool result_error_message_present = false;
};

FrontendCApiRunnerPublicResultDiagnostics
BuildFrontendCApiRunnerPublicResultDiagnostics(
    const FrontendCApiRunnerResultErrorSnapshot &error_snapshot);
