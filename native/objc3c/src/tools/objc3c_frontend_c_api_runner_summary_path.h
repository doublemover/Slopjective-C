#pragma once

#include <filesystem>

#include "tools/objc3c_frontend_c_api_runner_options.h"

std::filesystem::path BuildFrontendCApiRunnerSummaryPath(
    const FrontendCApiRunnerOptions &options);
