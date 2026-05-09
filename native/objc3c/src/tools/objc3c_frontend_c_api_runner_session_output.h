#pragma once

#include <filesystem>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_compile_session.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool EmitFrontendCApiRunnerSessionOutput(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    std::string &error);
