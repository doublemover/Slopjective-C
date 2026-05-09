#pragma once

#include <filesystem>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_options.h"

bool FrontendCApiRunnerPathExists(const std::string &path_text);
std::string QuoteFrontendCApiRunnerPowerShellArg(const std::string &value);
std::string BuildFrontendCApiRunnerReadCommand(const std::string &path_text);
std::string BuildFrontendCApiRunnerObjectInspectionCommand(
    const std::string &template_command,
    const std::string &object_path_text);
std::string BuildFrontendCApiRunnerReproCommand(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    bool dump_playground_repro_json);
