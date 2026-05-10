#pragma once

#include <iosfwd>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_context.h"

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityWriterContext(
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text);

void WriteFrontendCApiRunnerObservabilityJsonSectionRows(
    std::ostream &out,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerObservabilityContext &context);
