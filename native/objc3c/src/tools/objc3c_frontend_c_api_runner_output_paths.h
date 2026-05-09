#pragma once

#include <filesystem>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool ResolveFrontendCApiRunnerDiagnosticsOutputPath(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    std::filesystem::path &diagnostics_path,
    std::string &error);
