#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

struct FrontendCApiRunnerPublicResultView {
  const char *backend_name = "clang";
  unsigned status_code = 0;
  int process_exit_code = 0;
  bool success = false;
  bool semantic_skipped = false;
  FrontendCApiRunnerArtifactPathView paths;
  std::string last_error;
  std::string result_error_message;
  bool result_error_message_present = false;
  FrontendCApiRunnerCOwnershipView c_api_ownership;
};

FrontendCApiRunnerPublicResultView BuildFrontendCApiRunnerPublicResultView(
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message);
