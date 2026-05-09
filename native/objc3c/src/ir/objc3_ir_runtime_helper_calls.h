#pragma once

#include <string>
#include <vector>

std::string BuildObjc3IRRuntimeI32CallLine(const std::string &result_value,
                                           const std::string &symbol,
                                           const std::vector<std::string> &args);

std::string BuildObjc3IRRuntimeVoidCallLine(
    const std::string &symbol, const std::vector<std::string> &args);
