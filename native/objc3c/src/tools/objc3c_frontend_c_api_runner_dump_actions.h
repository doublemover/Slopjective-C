#pragma once

#include <filesystem>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool ShouldEmitFrontendCApiRunnerDumpActions(
    const FrontendCApiRunnerOptions &options);

void EmitFrontendCApiRunnerDumpActions(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text,
    const std::string &summary_json);
