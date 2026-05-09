#pragma once

#include <filesystem>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

struct FrontendCApiRunnerArtifactPathView {
  std::string summary;
  std::string diagnostics;
  std::string manifest;
  std::string ir;
  std::string object;
  std::string runtime_metadata_binary;
};

struct FrontendCApiRunnerCOwnershipView {
  bool result_owned_error_message = false;
  bool diagnostics_path_borrowed = false;
  bool manifest_path_borrowed = false;
  bool ir_path_borrowed = false;
  bool object_path_borrowed = false;
  bool runtime_metadata_path_borrowed = false;
};

struct FrontendCApiRunnerPublicResultView {
  const char *backend_name = "clang";
  unsigned status_code = 0;
  int process_exit_code = 0;
  bool success = false;
  bool semantic_skipped = false;
  FrontendCApiRunnerArtifactPathView paths;
  std::string last_error;
  std::string result_error_message;
  FrontendCApiRunnerCOwnershipView c_api_ownership;
};

FrontendCApiRunnerPublicResultView BuildFrontendCApiRunnerPublicResultView(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message);
