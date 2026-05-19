#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_result_contract_status.h"

bool ValidateFrontendCApiResultStatusSuccessConsistency(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    bool ok_status,
    std::string &reason);

bool ValidateFrontendCApiResultErrorMessagePresenceRules(
    bool ok_status,
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason);

bool ValidateFrontendCApiResultLatestErrorAccessorSnapshot(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason);

bool ValidateFrontendCApiResultContextResultErrorParity(
    const std::string &last_error,
    const FrontendCApiRunnerStringSnapshot &result_error_message,
    std::string &reason);
