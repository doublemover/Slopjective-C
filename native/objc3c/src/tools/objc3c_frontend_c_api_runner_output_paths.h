#pragma once

#include <filesystem>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

std::filesystem::path BuildFrontendCApiRunnerDiagnosticsOutputPath(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result);
