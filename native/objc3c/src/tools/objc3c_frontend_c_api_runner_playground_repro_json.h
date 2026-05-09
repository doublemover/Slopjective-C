#pragma once

#include <filesystem>
#include <ostream>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

void WriteFrontendCApiRunnerPlaygroundReproJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text);

std::string BuildFrontendCApiRunnerPlaygroundReproJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::filesystem::path &summary_path);
