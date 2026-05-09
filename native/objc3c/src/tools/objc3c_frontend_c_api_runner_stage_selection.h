#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"

std::string LastAttemptedFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result);
std::string BlockingFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result);
