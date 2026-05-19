#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_string_snapshot.h"

FrontendCApiRunnerStringSnapshot FrontendCApiResultErrorMessageSnapshot(
    const objc3c_frontend_c_compile_result_t &result);
std::string FrontendCApiResultErrorMessage(
    const objc3c_frontend_c_compile_result_t &result);
std::string ReadFrontendCApiLastError(
    const objc3c_frontend_c_context_t *context);
