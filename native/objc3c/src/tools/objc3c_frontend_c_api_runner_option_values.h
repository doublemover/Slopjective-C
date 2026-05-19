#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#include "libobjc3c_frontend/c_api.h"

constexpr std::size_t kFrontendCApiRunnerMaxMessageSendArgs = 16;

bool ParseFrontendCApiRunnerIrObjectBackend(
    const std::string &value,
    objc3c_frontend_c_ir_object_backend_t &backend);
bool ParseFrontendCApiRunnerMaxMessageSendArgs(const std::string &value,
                                               std::uint32_t &parsed_value,
                                               std::string &error);
bool ParseFrontendCApiRunnerRegistrationOrderOrdinal(
    const std::string &value,
    std::uint64_t &parsed_value,
    std::string &error);
bool ParseFrontendCApiRunnerRuntimeDispatchSymbol(
    const std::string &value,
    std::string &parsed_value,
    std::string &error);
