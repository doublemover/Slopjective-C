#pragma once

#include <filesystem>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_session_result.h"

bool PublishFrontendCApiRunnerSessionResult(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerSessionResult &session_result,
    std::string &error);
