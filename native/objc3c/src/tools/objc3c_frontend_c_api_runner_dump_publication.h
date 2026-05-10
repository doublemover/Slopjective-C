#pragma once

#include <filesystem>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_session_result.h"

struct FrontendCApiRunnerDumpPublication {
  const objc3c_frontend_c_compile_result_t *compile_result = nullptr;
  objc3c_frontend_c_status_t status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  std::filesystem::path summary_path;
  std::string result_error_message;
  std::string runtime_metadata_binary_path_text;
  std::string summary_json;
};

FrontendCApiRunnerDumpPublication BuildFrontendCApiRunnerDumpPublication(
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerSessionResult &session_result);

const objc3c_frontend_c_compile_result_t &FrontendCApiRunnerDumpCompileResult(
    const FrontendCApiRunnerDumpPublication &publication);
