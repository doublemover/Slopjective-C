#pragma once

#include "libobjc3c_frontend/c_api.h"

const char *FrontendCApiStatusName(objc3c_frontend_c_status_t status);
int FrontendCApiExitCodeFromStatus(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result);
