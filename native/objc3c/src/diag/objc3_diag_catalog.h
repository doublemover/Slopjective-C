#pragma once

#include <string_view>

unsigned DiagSeverityRank(std::string_view severity);
bool IsNativeDiagCode(std::string_view candidate);
