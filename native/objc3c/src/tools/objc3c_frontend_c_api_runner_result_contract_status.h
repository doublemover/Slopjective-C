#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_c_string.h"

bool ValidateFrontendCApiResultStatusAndErrorAccessors(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason);
