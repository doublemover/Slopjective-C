#pragma once

#include <ostream>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

void WriteFrontendCApiRunnerRuntimeInspectorJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result);

std::string BuildFrontendCApiRunnerRuntimeInspectorJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result);
