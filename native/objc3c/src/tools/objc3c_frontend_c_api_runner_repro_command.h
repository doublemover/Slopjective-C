#pragma once

#include <filesystem>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_options.h"

std::string BuildFrontendCApiRunnerReproCommand(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    bool dump_playground_repro_json);
