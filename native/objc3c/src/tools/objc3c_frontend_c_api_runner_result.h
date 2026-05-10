#pragma once

/*
 * Tool-only result consumer for objc3c_frontend/c_api.h. This header copies
 * borrowed public C API strings into runner-owned std::string values and owns
 * compile-result destruction; it is not a package C ABI header.
 */
#include "tools/objc3c_frontend_c_api_runner_c_string.h"
#include "tools/objc3c_frontend_c_api_runner_result_contract.h"
#include "tools/objc3c_frontend_c_api_runner_result_owner.h"
#include "tools/objc3c_frontend_c_api_runner_stage_report.h"
