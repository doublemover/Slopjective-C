#pragma once

#include <filesystem>
#include <ostream>
#include <string>

#include "libobjc3c_frontend/c_api.h"

void WriteFrontendCApiRunnerObservabilityJson(
    std::ostream &out,
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text);

std::string BuildFrontendCApiRunnerObservabilityJson(
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text);
