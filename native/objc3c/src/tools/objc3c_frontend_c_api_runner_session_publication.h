#pragma once

#include <filesystem>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_compile_session.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_session_summary.h"

bool PublishFrontendCApiRunnerSessionSummary(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    const FrontendCApiRunnerSessionSummary &summary,
    std::string &error);
